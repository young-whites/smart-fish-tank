#include "app_data.h"
#include "app_sensor.h"
#include "stm32f1xx.h"
#include <stdint.h>

/* ========== DS18B20 1-Wire (PB13) ========== */

static uint8_t ds18b20_valid = 0;  /* 1=传感器在线 */

static void ds18b20_pin_out(void)
{
    GPIOB->CRH &= ~(0xFU << 20);
    GPIOB->CRH |=  (0x3U << 20);
}

static void ds18b20_pin_in(void)
{
    GPIOB->CRH &= ~(0xFU << 20);
    GPIOB->CRH |=  (0x8U << 20);
    GPIOB->BSRR = (1U << 13);
}

static void ds18b20_dq_high(void) { GPIOB->BSRR = (1U << 13); }
static void ds18b20_dq_low(void)  { GPIOB->BRR  = (1U << 13); }
static uint8_t ds18b20_dq_read(void) { return (GPIOB->IDR & (1U << 13)) ? 1 : 0; }

static void ds18b20_delay_us(uint32_t us)
{
    volatile uint32_t n = us * 72 / 4;
    while(n--);
}

static uint8_t ds18b20_reset(void)
{
    uint8_t retry = 0;
    ds18b20_pin_out();
    ds18b20_dq_low();
    ds18b20_delay_us(500);
    ds18b20_dq_high();
    ds18b20_delay_us(60);
    ds18b20_pin_in();
    while(ds18b20_dq_read() && retry < 200) { retry++; ds18b20_delay_us(1); }
    if(retry >= 200) return 1;
    retry = 0;
    while(!ds18b20_dq_read() && retry < 240) { retry++; ds18b20_delay_us(1); }
    return 0;
}

static void ds18b20_write_byte(uint8_t dat)
{
    uint8_t i;
    ds18b20_pin_out();
    for(i = 0; i < 8; i++) {
        ds18b20_dq_low();
        ds18b20_delay_us(2);
        if(dat & 0x01) ds18b20_dq_high(); else ds18b20_dq_low();
        ds18b20_delay_us(60);
        ds18b20_dq_high();
        ds18b20_delay_us(2);
        dat >>= 1;
    }
}

static uint8_t ds18b20_read_byte(void)
{
    uint8_t i, dat = 0;
    for(i = 0; i < 8; i++) {
        ds18b20_pin_out();
        ds18b20_dq_low();
        ds18b20_delay_us(2);
        ds18b20_dq_high();
        ds18b20_delay_us(2);
        ds18b20_pin_in();
        if(ds18b20_dq_read()) dat |= (1 << i);
        ds18b20_delay_us(60);
    }
    return dat;
}

static uint8_t ds18b20_crc8(const uint8_t *data, uint8_t len)
{
    uint8_t crc = 0;
    uint8_t i, j;
    for(i = 0; i < len; i++) {
        crc ^= data[i];
        for(j = 0; j < 8; j++) {
            if(crc & 0x01) crc = (crc >> 1) ^ 0x8C;
            else crc >>= 1;
        }
    }
    return crc;
}

/*
 * 读取温度 (完整 scratchpad 9字节 + CRC校验)
 * 返回: 0=成功, 1=传感器无响应, 2=CRC错误
 */
static uint8_t ds18b20_read_temp_raw(int16_t *temp_out)
{
    uint8_t sp[9];
    uint8_t i;

    if(ds18b20_reset()) return 1;
    ds18b20_write_byte(0xCC);
    ds18b20_write_byte(0x44);
    rt_thread_mdelay(750);

    if(ds18b20_reset()) return 1;
    ds18b20_write_byte(0xCC);
    ds18b20_write_byte(0xBE);
    for(i = 0; i < 9; i++) sp[i] = ds18b20_read_byte();

    if(ds18b20_crc8(sp, 9) != 0) return 2;

    *temp_out = (int16_t)((sp[1] << 8) | sp[0]);
    return 0;
}

static float ds18b20_raw_to_float(int16_t raw)
{
    int16_t int_part = raw >> 4;
    uint8_t frac_part = raw & 0x0F;
    return (float)int_part + (float)frac_part * 0.0625f;
}

/* ========== ADC (CMSIS 寄存器) ========== */

static void adc_init(void)
{
    /* 使能 ADC1 和 GPIOA 时钟 */
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    /* PA0, PA1, PA4 配置为模拟输入 (MODE=00, CNF=00) */
    /* PA0 -> CRL[3:0], PA1 -> CRL[7:4], PA4 -> CRL[19:16] */
    GPIOA->CRL &= ~((0xFU << 0) | (0xFU << 4) | (0xFU << 16));

    /* ADC 基本配置 */
    ADC1->CR2 = 0;
    ADC1->CR2 |= ADC_CR2_ADON;  /* 开启 ADC */
    ADC1->SQR1 = 0;             /* 1 次转换 */
}

static uint16_t adc_read_channel(uint8_t channel)
{
    ADC1->SQR3 = channel;
    ADC1->CR2 |= ADC_CR2_ADON;           /* 启动转换 */
    while(!(ADC1->SR & ADC_SR_EOC));     /* 等待完成 */
    return (uint16_t)(ADC1->DR);
}

/* ========== 传感器采集线程 ========== */

static struct rt_thread sensor_thread;
static rt_uint8_t sensor_stack[384];

/* 传感器状态标志 */
uint8_t g_sensor_temp_valid = 0;  /* 1=DS18B20在线且数据有效 */

static void sensor_thread_entry(void *param)
{
    int16_t temp_raw;
    uint16_t adc_val;

    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    adc_init();

    while(1) {
        /* DS18B20 温度采集 (CRC校验) */
        uint8_t ret = ds18b20_read_temp_raw(&temp_raw);
        if(ret == 0 && temp_raw > (-50 << 4) && temp_raw < (125 << 4)) {
            rt_mutex_take(&mutex_data, RT_WAITING_FOREVER);
            g_sensor.water_temp = ds18b20_raw_to_float(temp_raw);
            rt_mutex_release(&mutex_data);
            g_sensor_temp_valid = 1;
        } else {
            g_sensor_temp_valid = 0;  /* 传感器断线或CRC错误 */
        }

        /* ADC 采集 */
        rt_mutex_take(&mutex_data, RT_WAITING_FOREVER);
        g_sensor.air_quality = adc_read_channel(ADC_AIR_CHANNEL);
        adc_val = adc_read_channel(ADC_WATER_CHANNEL);
        g_sensor.water_level = (uint8_t)((uint32_t)adc_val * 100 / 4095);
        adc_val = adc_read_channel(ADC_PH_CHANNEL);
        g_sensor.ph_value = (float)adc_val * 14.0f / 4095.0f;
        rt_mutex_release(&mutex_data);

        rt_thread_mdelay(500);
    }
}

void app_sensor_init(void)
{
    rt_thread_init(&sensor_thread,
        "t_sens", sensor_thread_entry, RT_NULL,
        &sensor_stack[0], sizeof(sensor_stack),
        15, 10);
    rt_thread_startup(&sensor_thread);
}
