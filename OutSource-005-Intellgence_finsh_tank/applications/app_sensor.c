#include "app_data.h"
#include "app_sensor.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_adc.h"
#include <stdint.h>
#include <rtdbg.h>

#define DBG_TAG "sensor"

/* ========== DS18B20 1-Wire (PB13) ========== */

static void ds18b20_pin_out(void)
{
    GPIO_InitTypeDef g = {0};
    g.Pin = GPIO_PIN_13;
    g.Mode = GPIO_MODE_OUTPUT_PP;
    g.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &g);
}

static void ds18b20_pin_in(void)
{
    GPIO_InitTypeDef g = {0};
    g.Pin = GPIO_PIN_13;
    g.Mode = GPIO_MODE_INPUT;
    g.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOB, &g);
}

static void ds18b20_dq_high(void) { HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET); }
static void ds18b20_dq_low(void)  { HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET); }
static uint8_t ds18b20_dq_read(void) { return HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13); }

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

static float ds18b20_read_temp(void)
{
    uint8_t tl, th;
    int16_t temp;
    if(ds18b20_reset()) return -999.0f;
    ds18b20_write_byte(0xCC);
    ds18b20_write_byte(0x44);
    rt_thread_mdelay(750);
    if(ds18b20_reset()) return -999.0f;
    ds18b20_write_byte(0xCC);
    ds18b20_write_byte(0xBE);
    tl = ds18b20_read_byte();
    th = ds18b20_read_byte();
    temp = (th << 8) | tl;
    return (float)temp * 0.0625f;
}

/* ========== ADC ========== */

static ADC_HandleTypeDef hadc1;

static void adc_init(void)
{
    __HAL_RCC_ADC1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitTypeDef g = {0};
    g.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4;
    g.Mode = GPIO_MODE_ANALOG;
    HAL_GPIO_Init(GPIOA, &g);

    hadc1.Instance = ADC1;
    hadc1.Init.ScanConvMode = DISABLE;
    hadc1.Init.ContinuousConvMode = DISABLE;
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.NbrOfConversion = 1;
    HAL_ADC_Init(&hadc1);
}

static uint16_t adc_read_channel(uint32_t channel)
{
    ADC_ChannelConfTypeDef s = {0};
    s.Channel = channel;
    s.Rank = ADC_REGULAR_RANK_1;
    s.SamplingTime = ADC_SAMPLETIME_55CYCLES_5;
    HAL_ADC_ConfigChannel(&hadc1, &s);
    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, 100);
    return HAL_ADC_GetValue(&hadc1);
}

/* ========== 传感器采集线程 ========== */

static void sensor_thread_entry(void *param)
{
    float temp;
    __HAL_RCC_GPIOB_CLK_ENABLE();
    adc_init();

    while(1) {
        temp = ds18b20_read_temp();
        if(temp > -50.0f && temp < 125.0f) {
            rt_mutex_take(&mutex_data, RT_WAITING_FOREVER);
            g_sensor.water_temp = temp;
            rt_mutex_release(&mutex_data);
        }

        rt_mutex_take(&mutex_data, RT_WAITING_FOREVER);
        g_sensor.air_quality = adc_read_channel(ADC_CHANNEL_4);
        g_sensor.water_level = (uint8_t)(adc_read_channel(ADC_CHANNEL_1) * 100 / 4095);
        g_sensor.ph_value = (float)adc_read_channel(ADC_CHANNEL_0) * 14.0f / 4095.0f;
        rt_mutex_release(&mutex_data);

        rt_thread_mdelay(500);
    }
}

void app_sensor_init(void)
{
    rt_thread_t tid = rt_thread_create("t_sensor", sensor_thread_entry, RT_NULL, 1024, 15, 10);
    if(tid) rt_thread_startup(tid);
}
