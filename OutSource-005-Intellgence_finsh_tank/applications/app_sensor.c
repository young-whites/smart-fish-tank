#include "app_data.h"
#include "app_sensor.h"
#include "stm32f1xx.h"
#include <stdint.h>

/* ========== DS18B20 1-Wire (PB13) ========== */

/* PB13 配置为推挽输出 */
static void ds18b20_pin_out(void)
{
    /* CRH: PB8-PB15, PB13 -> CRH[23:20] */
    GPIOB->CRH &= ~(0xFU << 20);
    GPIOB->CRH |=  (0x3U << 20);  /* MODE=11(50MHz), CNF=00(推挽输出) */
}

/* PB13 配置为上拉输入 */
static void ds18b20_pin_in(void)
{
    /* MODE=00(输入), CNF=10(上拉/下拉输入) */
    GPIOB->CRH &= ~(0xFU << 20);
    GPIOB->CRH |=  (0x8U << 20);
    GPIOB->BSRR = (1U << 13);  /* 上拉 */
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

/* 读取温度，返回整数部分和小数部分(0-9) */
static int16_t ds18b20_read_temp_raw(int16_t *frac)
{
    uint8_t tl, th;
    int16_t temp;
    if(ds18b20_reset()) { *frac = 0; return -999; }
    ds18b20_write_byte(0xCC);
    ds18b20_write_byte(0x44);
    rt_thread_mdelay(750);
    if(ds18b20_reset()) { *frac = 0; return -999; }
    ds18b20_write_byte(0xCC);
    ds18b20_write_byte(0xBE);
    tl = ds18b20_read_byte();
    th = ds18b20_read_byte();
    temp = (int16_t)((th << 8) | tl);
    /* temp * 0.0625 = temp / 16 */
    *frac = ((temp & 0x0F) * 10) / 16;  /* 小数部分 0-9 */
    return temp / 16;
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
static rt_uint8_t sensor_stack[512];

static void sensor_thread_entry(void *param)
{
    int16_t temp_int, temp_frac;
    uint16_t adc_val;

    /* 使能 GPIOB 时钟 (DS18B20 在 PB13) */
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;

    adc_init();

    while(1) {
        temp_int = ds18b20_read_temp_raw(&temp_frac);
        if(temp_int > -50 && temp_int < 125) {
            rt_mutex_take(&mutex_data, RT_WAITING_FOREVER);
            g_sensor.water_temp = (float)temp_int + (float)temp_frac * 0.1f;
            rt_mutex_release(&mutex_data);
        }

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
