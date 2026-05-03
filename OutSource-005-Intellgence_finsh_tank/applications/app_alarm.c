#include "app_data.h"
#include "app_alarm.h"
#include "stm32f1xx.h"
#include <stdint.h>

/* 蜂鸣器: PB9 (推挽输出) */

static void beep_init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;

    /* PB9: 推挽输出 50MHz -> CRH[7:4] */
    GPIOB->CRH &= ~(0xFU << 4);
    GPIOB->CRH |=  (0x3U << 4);

    GPIOB->BRR = (1U << 9);  /* 初始关闭 */
}

static void beep_on(void)  { GPIOB->BSRR = (1U << 9); }
static void beep_off(void) { GPIOB->BRR  = (1U << 9); }

static void alarm_check(void)
{
    uint8_t alarm = 0;
    float temp, ph;
    uint8_t wl, en;
    uint16_t air;

    rt_mutex_take(&mutex_data, RT_WAITING_FOREVER);
    temp = g_sensor.water_temp;
    wl = g_sensor.water_level;
    air = g_sensor.air_quality;
    ph = g_sensor.ph_value;
    en = g_status.alarm_enable;
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

static struct rt_thread alarm_thread;
static rt_uint8_t alarm_stack[384];

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
    rt_thread_init(&alarm_thread,
        "t_alarm", alarm_thread_entry, RT_NULL,
        &alarm_stack[0], sizeof(alarm_stack),
        13, 10);
    rt_thread_startup(&alarm_thread);
}
