#include "app_data.h"
#include "app_sensor.h"
#include "stm32f1xx.h"
#include <stdint.h>
#include <stdio.h>
#include <finsh.h>

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

#define ADC_VREF        3.3f    /* 参考电压 */
#define ADC_MAX         4095    /* 12bit ADC */
#define ADC_AVG_SAMPLES 8       /* 多次采样取平均 */

static void adc_init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    GPIOA->CRL &= ~((0xFU << 0) | (0xFU << 4) | (0xFU << 16));

    ADC1->CR2 = 0;
    ADC1->CR2 |= ADC_CR2_ADON;
    ADC1->SQR1 = 0;
    rt_thread_mdelay(10);  /* ADC 稳定 */
}

static uint16_t adc_read_raw(uint8_t channel)
{
    ADC1->SQR3 = channel;
    ADC1->CR2 |= ADC_CR2_ADON;
    while(!(ADC1->SR & ADC_SR_EOC));
    return (uint16_t)(ADC1->DR);
}

/* 多次采样取平均，滤除噪声 */
static uint16_t adc_read_avg(uint8_t channel)
{
    uint32_t sum = 0;
    uint8_t i;
    for(i = 0; i < ADC_AVG_SAMPLES; i++) {
        sum += adc_read_raw(channel);
    }
    return (uint16_t)(sum / ADC_AVG_SAMPLES);
}

/* ADC 值 → 电压 (V) */
static float adc_to_voltage(uint16_t adc_val)
{
    return (float)adc_val * ADC_VREF / (float)ADC_MAX;
}

/*
 * 空气质量: MQ-135
 *   ADC 原始值 → 电压 → 电阻比 Rs/R0 → PPM
 *   简化: 直接用 ADC 值映射为 0~999 的空气质量指数
 *   ADC 高 = 空气差 (电压高 = Rs 低 = 气体浓度高)
 */
static uint16_t calc_air_quality(uint16_t adc_val)
{
    /* 映射: ADC 0~4095 → 0~999 (空气质量指数) */
    uint16_t aqi = (uint32_t)adc_val * 999 / ADC_MAX;
    return aqi;
}

/*
 * 水位: 模拟水位传感器
 *   传感器输出 0~3.3V 对应 0~100%
 *   线性映射，可根据实际传感器标定量程调整
 */
static uint8_t calc_water_level(uint16_t adc_val)
{
    /* 线性映射: ADC 0~4095 → 0~100% */
    uint8_t level = (uint8_t)((uint32_t)adc_val * 100 / ADC_MAX);
    if(level > 100) level = 100;
    return level;
}

/*
 * PH 值: 模拟 PH 传感器
 *   标定参数 (需根据实际传感器和缓冲液校准):
 *   - 中性溶液 (PH=7.0) 输出约 1.65V → ADC 2048
 *   - 酸性溶液 (PH=4.0) 输出约 0.825V → ADC 1024
 *   - 碱性溶液 (PH=10.0) 输出约 2.475V → ADC 3072
 *   公式: PH = 7.0 + (ADC - 2048) * 7.0 / 2048
 *         = 7.0 + (V - 1.65) * 7.0 / 1.65
 */
#define PH_NEUTRAL_ADC  2048    /* 中性 (PH=7.0) 对应 ADC 值 */
#define PH_SLOPE        7.0f    /* PH 斜率: 每 1.65V 变化 7 个 PH */
#define PH_RANGE        2048.0f /* ADC 范围 (对应 ±7 PH) */

static float calc_ph_value(uint16_t adc_val)
{
    float ph = 7.0f + ((float)adc_val - PH_NEUTRAL_ADC) * PH_SLOPE / PH_RANGE;
    if(ph < 0.0f) ph = 0.0f;
    if(ph > 14.0f) ph = 14.0f;
    return ph;
}

/* ========== 传感器采集线程 ========== */

static struct rt_thread sensor_thread;
static rt_uint8_t sensor_stack[512];

/* 传感器状态标志 */
uint8_t g_sensor_temp_valid = 0;  /* 1=DS18B20在线且数据有效 */

static void sensor_thread_entry(void *param)
{
    int16_t temp_raw;
    uint16_t adc_air, adc_water, adc_ph;

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
            g_sensor_temp_valid = 0;
        }

        /* ADC 多次采样 + 换算 */
        adc_air   = adc_read_avg(ADC_AIR_CHANNEL);
        adc_water = adc_read_avg(ADC_WATER_CHANNEL);
        adc_ph    = adc_read_avg(ADC_PH_CHANNEL);

        rt_mutex_take(&mutex_data, RT_WAITING_FOREVER);
        g_sensor.air_quality = calc_air_quality(adc_air);
        g_sensor.water_level = calc_water_level(adc_water);
        g_sensor.ph_value    = calc_ph_value(adc_ph);
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

/* ========== FinSH 调试命令 ========== */

static void temp(void)
{
    if(g_sensor_temp_valid)
        rt_kprintf("Temp: %.2f C\n", g_sensor.water_temp);
    else
        rt_kprintf("Temp: sensor offline\n");
}
MSH_CMD_EXPORT(temp, show DS18B20 temperature);

static void sensor(void)
{
    rt_kprintf("--- Sensor Data ---\n");
    if(g_sensor_temp_valid)
        rt_kprintf("Temp:  %.2f C\n", g_sensor.water_temp);
    else
        rt_kprintf("Temp:  --.- (offline)\n");
    rt_kprintf("Air:   %d\n", g_sensor.air_quality);
    rt_kprintf("Water: %d%%\n", g_sensor.water_level);
    rt_kprintf("PH:    %.2f\n", g_sensor.ph_value);
    rt_kprintf("Mode:  %s\n", g_sensor.run_mode == MODE_AUTO ? "Auto" : "Manual");
}
MSH_CMD_EXPORT(sensor, show all sensor data);
