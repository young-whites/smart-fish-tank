/**
 * @file    app_sensor.c
 * @brief   传感器采集模块 (DS18B20 + ADC)
 *
 *  DS18B20 时序参数移植自 ds18b20-latest RTT 官方组件
 *  使用 CMSIS 寄存器操作 (RT-Thread Nano 无 rtdevice)
 */

#include "app_data.h"
#include "app_sensor.h"
#include "stm32f1xx.h"
#include <stdint.h>
#include <stdio.h>
#include <finsh.h>

/* ========== DS18B20 1-Wire (PB1, CMSIS) ========== */

/* 简单微秒延时 (72MHz, 每us约18轮×4周期=72周期) */
static void delay_us(uint32_t us)
{
    while(us--) {
        volatile uint32_t i = 18;
        while(i--);
    }
}

/* PB1 引脚: 输出模式 */
static void DS18B20_IO_OUT(void)
{
    GPIOB->CRL &= ~(0xFU << 4);
    GPIOB->CRL |=  (0x3U << 4);  /* 推挽输出 50MHz */
}

/* PB1 引脚: 上拉输入模式 */
static void DS18B20_IO_IN(void)
{
    GPIOB->CRL &= ~(0xFU << 4);
    GPIOB->CRL |=  (0x8U << 4);  /* 上拉输入 */
    GPIOB->BSRR = (1U << 1);
}

/* 复位 */
static void DS18B20_Reset(void)
{
    DS18B20_IO_OUT();
    GPIOB->BRR = (1U << 1);    /* 拉低 */
    delay_us(750);
    GPIOB->BSRR = (1U << 1);   /* 释放 */
    delay_us(15);
}

/* 检测存在脉冲, 返回 0=存在, 1=不存在 */
static uint8_t DS18B20_Check(void)
{
    uint8_t retry = 0;
    DS18B20_IO_IN();
    while((GPIOB->IDR & (1U << 1)) && retry < 200) { retry++; delay_us(1); }
    if(retry >= 200) return 1;
    retry = 0;
    while(!(GPIOB->IDR & (1U << 1)) && retry < 240) { retry++; delay_us(1); }
    if(retry >= 240) return 1;
    return 0;
}

/* 读一个位 */
static uint8_t DS18B20_Read_Bit(void)
{
    uint8_t data;
    DS18B20_IO_OUT();
    GPIOB->BRR = (1U << 1);
    delay_us(2);
    GPIOB->BSRR = (1U << 1);
    DS18B20_IO_IN();
    delay_us(12);
    data = (GPIOB->IDR & (1U << 1)) ? 1 : 0;
    delay_us(50);
    return data;
}

/* 读一个字节 */
static uint8_t DS18B20_Read_Byte(void)
{
    uint8_t i, j, dat = 0;
    for(i = 1; i <= 8; i++) {
        j = DS18B20_Read_Bit();
        dat = (j << 7) | (dat >> 1);
    }
    return dat;
}

/* 写一个字节 */
static void DS18B20_Write_Byte(uint8_t dat)
{
    uint8_t j, testb;
    DS18B20_IO_OUT();
    for(j = 1; j <= 8; j++) {
        testb = dat & 0x01;
        dat >>= 1;
        if(testb) {
            GPIOB->BRR = (1U << 1); delay_us(2);
            GPIOB->BSRR = (1U << 1); delay_us(60);
        } else {
            GPIOB->BRR = (1U << 1); delay_us(60);
            GPIOB->BSRR = (1U << 1); delay_us(2);
        }
    }
}

/* 启动温度转换 */
static void DS18B20_Start(void)
{
    DS18B20_Reset();
    DS18B20_Check();
    DS18B20_Write_Byte(0xCC);
    DS18B20_Write_Byte(0x44);
}

/*
 * 读取温度
 * 返回: 0=成功, 1=传感器无响应
 */
static uint8_t ds18b20_read_temp_x10(int32_t *temp_x10)
{
    uint8_t TL, TH;
    int32_t tem;

    DS18B20_Start();
    DS18B20_Reset();
    DS18B20_Check();
    DS18B20_Write_Byte(0xCC);
    DS18B20_Write_Byte(0xBE);
    TL = DS18B20_Read_Byte();
    TH = DS18B20_Read_Byte();

    if(TH > 7) {
        TH = ~TH; TL = ~TL;
        tem = TH; tem <<= 8; tem += TL;
        *temp_x10 = -(int32_t)(tem * 0.0625 * 10 + 0.5);
    } else {
        tem = TH; tem <<= 8; tem += TL;
        *temp_x10 = (int32_t)(tem * 0.0625 * 10 + 0.5);
    }
    return 0;
}

/* ========== ADC (CMSIS) ========== */

#define ADC_VREF        3.3f
#define ADC_MAX         4095
#define ADC_AVG_SAMPLES 8

static void adc_init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    GPIOA->CRL &= ~((0xFU << 0) | (0xFU << 4) | (0xFU << 16));
    ADC1->CR2 = 0;
    ADC1->CR2 |= ADC_CR2_ADON;
    ADC1->SQR1 = 0;
    rt_thread_mdelay(10);
}

static uint16_t adc_read_raw(uint8_t channel)
{
    ADC1->SQR3 = channel;
    ADC1->CR2 |= ADC_CR2_ADON;
    while(!(ADC1->SR & ADC_SR_EOC));
    return (uint16_t)(ADC1->DR);
}

static uint16_t adc_read_avg(uint8_t channel)
{
    uint32_t sum = 0;
    uint8_t i;
    for(i = 0; i < ADC_AVG_SAMPLES; i++) sum += adc_read_raw(channel);
    return (uint16_t)(sum / ADC_AVG_SAMPLES);
}

static float adc_to_voltage(uint16_t adc_val)
{
    return (float)adc_val * ADC_VREF / (float)ADC_MAX;
}

static uint16_t calc_air_quality(uint16_t adc_val)
{
    return (uint32_t)adc_val * 999 / ADC_MAX;
}

static uint8_t calc_water_level(uint16_t adc_val)
{
    uint8_t level = (uint8_t)((uint32_t)adc_val * 100 / ADC_MAX);
    if(level > 100) level = 100;
    return level;
}

#define PH_NEUTRAL_ADC  2048
#define PH_SLOPE        7.0f
#define PH_RANGE        2048.0f

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
uint8_t g_sensor_temp_valid = 0;

static void sensor_thread_entry(void *param)
{
    int32_t temp_x10;
    uint16_t adc_air, adc_water, adc_ph;

    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    adc_init();

    while(1) {
        /* DS18B20 */
        if(ds18b20_read_temp_x10(&temp_x10) == 0 && temp_x10 > -500 && temp_x10 < 1250) {
            rt_mutex_take(&mutex_data, RT_WAITING_FOREVER);
            g_sensor.water_temp = (float)temp_x10 * 0.1f;
            rt_mutex_release(&mutex_data);
            g_sensor_temp_valid = 1;
        } else {
            g_sensor_temp_valid = 0;
        }

        /* ADC */
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

/* ========== FinSH 命令 ========== */

static void temp(void)
{
    if(g_sensor_temp_valid)
        rt_kprintf("Temp: %d.%d C\n",
                   (int)g_sensor.water_temp,
                   ((int)(g_sensor.water_temp * 10)) % 10);
    else
        rt_kprintf("Temp: sensor offline\n");
}
MSH_CMD_EXPORT(temp, show DS18B20 temperature);

static void sensor(void)
{
    uint16_t adc_air   = adc_read_avg(ADC_AIR_CHANNEL);
    uint16_t adc_water = adc_read_avg(ADC_WATER_CHANNEL);
    uint16_t adc_ph    = adc_read_avg(ADC_PH_CHANNEL);
    int mv_air   = (int)(adc_to_voltage(adc_air) * 1000);
    int mv_water = (int)(adc_to_voltage(adc_water) * 1000);
    int mv_ph    = (int)(adc_to_voltage(adc_ph) * 1000);

    rt_kprintf("--- Sensor Data ---\n");
    if(g_sensor_temp_valid) {
        rt_kprintf("Temp:  %d.%d C\n",
                   (int)g_sensor.water_temp,
                   ((int)(g_sensor.water_temp * 10)) % 10);
    } else {
        rt_kprintf("Temp:  [offline]\n");
    }
    rt_kprintf("Air:   AQI=%d  ADC=%d %dmV\n", g_sensor.air_quality, adc_air, mv_air);
    rt_kprintf("Water: %d%%     ADC=%d %dmV\n", g_sensor.water_level, adc_water, mv_water);
    rt_kprintf("PH:    %d.%02d    ADC=%d %dmV\n",
              (int)g_sensor.ph_value,
              (int)(g_sensor.ph_value * 100) % 100,
              adc_ph, mv_ph);
    rt_kprintf("Mode:  %s\n", g_sensor.run_mode == MODE_AUTO ? "Auto" : "Manual");
}
MSH_CMD_EXPORT(sensor, show all sensor data);

static void clk(void)
{
    rt_kprintf("HCLK:  %d Hz\n", HAL_RCC_GetHCLKFreq());
    rt_kprintf("PCLK1: %d Hz\n", HAL_RCC_GetPCLK1Freq());
    rt_kprintf("PCLK2: %d Hz\n", HAL_RCC_GetPCLK2Freq());
    rt_kprintf("SYSCLK:%d Hz\n", HAL_RCC_GetSysClockFreq());
}
MSH_CMD_EXPORT(clk, show system clock frequencies);
