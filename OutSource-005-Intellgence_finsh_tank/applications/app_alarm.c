#include "app_data.h"
#include "app_alarm.h"
#include "stm32f1xx_hal.h"
#include <stdint.h>

/* 蜂鸣器引脚: PB9 */
#define BEEP_PORT  GPIOB
#define BEEP_PIN   GPIO_PIN_9

static void beep_init(void)
{
    __HAL_RCC_GPIOB_CLK_ENABLE();
    GPIO_InitTypeDef g = {0};
    g.Pin = BEEP_PIN;
    g.Mode = GPIO_MODE_OUTPUT_PP;
    g.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(BEEP_PORT, &g);
    HAL_GPIO_WritePin(BEEP_PORT, BEEP_PIN, GPIO_PIN_RESET);
}

static void beep_on(void)  { HAL_GPIO_WritePin(BEEP_PORT, BEEP_PIN, GPIO_PIN_SET); }
static void beep_off(void) { HAL_GPIO_WritePin(BEEP_PORT, BEEP_PIN, GPIO_PIN_RESET); }

static void alarm_check(void)
{
    uint8_t alarm = 0;

    rt_mutex_take(&mutex_data, RT_WAITING_FOREVER);
    float temp = g_sensor.water_temp;
    uint8_t wl = g_sensor.water_level;
    uint16_t air = g_sensor.air_quality;
    float ph = g_sensor.ph_value;
    uint8_t en = g_status.alarm_enable;
    rt_mutex_release(&mutex_data);

    if(!en) return;

    if(temp < g_threshold.temp_lower || temp > g_threshold.temp_upper) alarm = 1;
    else if(wl < g_threshold.water_level_min || wl > g_threshold.water_level_max) alarm = 1;
    else if(air > g_threshold.air_quality_max) alarm = 1;
    else if(ph < g_threshold.ph_lower || ph > g_threshold.ph_upper) alarm = 1;

    if(alarm) {
        rt_ubase_t evt = 1;
        rt_mb_send(&mb_alarm, evt);
    }
}

static void alarm_thread_entry(void *param)
{
    beep_init();
    rt_ubase_t evt;

    while(1) {
        alarm_check();
        if(rt_mb_recv(&mb_alarm, &evt, RT_TICK_PER_SECOND / 2) == RT_EOK) {
            beep_on();
            rt_thread_mdelay(500);
            beep_off();
            rt_thread_mdelay(2000);
        } else {
            beep_off();
        }
    }
}

void app_alarm_init(void)
{
    rt_thread_t tid = rt_thread_create("t_alarm", alarm_thread_entry, RT_NULL, 512, 13, 10);
    if(tid) rt_thread_startup(tid);
}
