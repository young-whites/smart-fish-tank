#include "app_data.h"
#include "app_feed.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_tim.h"
#include <stdint.h>

static TIM_HandleTypeDef htim3;

static void servo_init(void)
{
    __HAL_RCC_TIM3_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitTypeDef g = {0};
    g.Pin = GPIO_PIN_6;
    g.Mode = GPIO_MODE_AF_PP;
    g.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &g);

    htim3.Instance = TIM3;
    htim3.Init.Prescaler = 71;
    htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim3.Init.Period = 19999;
    htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_PWM_Init(&htim3);

    TIM_OC_InitTypeDef oc = {0};
    oc.OCMode = TIM_OCMODE_PWM1;
    oc.Pulse = 1500;
    oc.OCPolarity = TIM_OCPOLARITY_HIGH;
    oc.OCFastMode = TIM_OCFAST_DISABLE;
    HAL_TIM_PWM_ConfigChannel(&htim3, &oc, TIM_CHANNEL_1);
    HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);
}

static void servo_open(void)
{
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 2500);
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
}

static void servo_close(void)
{
    HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_1);
}

static volatile uint8_t feed_trigger = 0;

void app_feed_trigger(void)
{
    feed_trigger = 1;
}

static void feed_thread_entry(void *param)
{
    servo_init();
    uint32_t countdown = g_threshold.feed_interval;

    while(1) {
        if(feed_trigger) {
            feed_trigger = 0;
            g_status.feeding = 1;
            servo_open();
            rt_thread_mdelay(3000);
            servo_close();
            g_status.feeding = 0;
            countdown = g_threshold.feed_interval;
        }

        if(countdown > 0) {
            countdown--;
            g_sensor.feed_countdown = countdown;
        } else {
            if(!g_status.feeding) {
                g_status.feeding = 1;
                servo_open();
                rt_thread_mdelay(3000);
                servo_close();
                g_status.feeding = 0;
                countdown = g_threshold.feed_interval;
            }
        }

        rt_thread_mdelay(1000);
    }
}

void app_feed_init(void)
{
    rt_thread_t tid = rt_thread_create("t_feed", feed_thread_entry, RT_NULL, 512, 16, 10);
    if(tid) rt_thread_startup(tid);
}
