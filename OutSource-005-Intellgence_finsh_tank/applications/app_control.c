#include "app_data.h"
#include "app_control.h"
#include "stm32f1xx.h"
#include <stdint.h>

/* 继电器引脚定义 (CMSIS) */
/* PB12=加热, PB14=注水, PB15=排水, PA15=增氧 */

static void relay_set_pb(uint8_t pin, uint8_t state)
{
    if(state)
        GPIOB->BSRR = (1U << pin);
    else
        GPIOB->BRR = (1U << pin);
}

static void relay_set_pa(uint8_t pin, uint8_t state)
{
    if(state)
        GPIOA->BSRR = (1U << pin);
    else
        GPIOA->BRR = (1U << pin);
}

static void relay_init(void)
{
    /* 使能 GPIOA, GPIOB, AFIO 时钟 (PA15 需要 AFIO) */
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_AFIOEN;

    /* 禁用 JTAG (PA15 是 JTDI)，保留 SWD */
    AFIO->MAPR &= ~AFIO_MAPR_SWJ_CFG;
    AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_JTAGDISABLE;

    /* PB12, PB14, PB15: 推挽输出 50MHz */
    /* PB12 -> CRH[19:16], PB14 -> CRH[27:24], PB15 -> CRH[31:28] */
    GPIOB->CRH &= ~((0xFU << 16) | (0xFU << 24) | (0xFU << 28));
    GPIOB->CRH |=  ((0x3U << 16) | (0x3U << 24) | (0x3U << 28));

    /* PA15: 推挽输出 50MHz -> CRH[31:28] */
    GPIOA->CRH &= ~(0xFU << 28);
    GPIOA->CRH |=  (0x3U << 28);

    /* 初始全部关闭 */
    GPIOB->BRR = (1U << 12) | (1U << 14) | (1U << 15);
    GPIOA->BRR = (1U << 15);
}

static void auto_control(void)
{
    float temp, t_low, t_high;
    uint8_t wl, wl_min, wl_max;
    uint16_t air, air_max;

    rt_mutex_take(&mutex_data, RT_WAITING_FOREVER);
    temp = g_sensor.water_temp;
    wl = g_sensor.water_level;
    air = g_sensor.air_quality;
    t_low = g_threshold.temp_lower;
    t_high = g_threshold.temp_upper;
    wl_min = g_threshold.water_level_min;
    wl_max = g_threshold.water_level_max;
    air_max = g_threshold.air_quality_max;
    rt_mutex_release(&mutex_data);

    /* 温度控制: PB12 */
    if(temp < t_low) {
        relay_set_pb(12, 1);
    } else if(temp > t_high) {
        relay_set_pb(12, 0);
    }

    /* 水位控制: PB14=注水, PB15=排水 */
    if(wl > wl_max) {
        relay_set_pb(14, 0);
        relay_set_pb(15, 1);
    } else if(wl < wl_min) {
        relay_set_pb(15, 0);
        relay_set_pb(14, 1);
    } else {
        relay_set_pb(14, 0);
        relay_set_pb(15, 0);
    }

    /* 空气质量: PA15 */
    if(air > air_max)
        relay_set_pa(15, 1);
    else
        relay_set_pa(15, 0);
}

static struct rt_thread control_thread;
static rt_uint8_t control_stack[512];

static void control_thread_entry(void *param)
{
    relay_init();
    while(1) {
        if(g_sensor.run_mode == MODE_AUTO) {
            auto_control();
        }
        rt_thread_mdelay(500);
    }
}

void app_control_init(void)
{
    rt_thread_init(&control_thread,
        "t_ctrl", control_thread_entry, RT_NULL,
        &control_stack[0], sizeof(control_stack),
        12, 10);
    rt_thread_startup(&control_thread);
}
