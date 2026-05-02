#include "app_data.h"
#include "app_control.h"
#include "stm32f1xx_hal.h"
#include <stdint.h>

/* 继电器引脚定义 */
#define RELAY_HEAT_PORT   GPIOB
#define RELAY_HEAT_PIN    GPIO_PIN_12
#define RELAY_FILL_PORT   GPIOB
#define RELAY_FILL_PIN    GPIO_PIN_14
#define RELAY_DRAIN_PORT  GPIOB
#define RELAY_DRAIN_PIN   GPIO_PIN_15
#define RELAY_OXYGEN_PORT GPIOA
#define RELAY_OXYGEN_PIN  GPIO_PIN_15

static void relay_set(GPIO_TypeDef *port, uint16_t pin, uint8_t state)
{
    HAL_GPIO_WritePin(port, pin, state ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

static void relay_init(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitTypeDef g = {0};
    g.Mode = GPIO_MODE_OUTPUT_PP;
    g.Speed = GPIO_SPEED_FREQ_HIGH;
    g.Pull = GPIO_NOPULL;

    g.Pin = GPIO_PIN_12 | GPIO_PIN_14 | GPIO_PIN_15;
    HAL_GPIO_Init(GPIOB, &g);

    g.Pin = GPIO_PIN_15;
    HAL_GPIO_Init(GPIOA, &g);

    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12 | GPIO_PIN_14 | GPIO_PIN_15, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET);
}

static void auto_control(void)
{
    rt_mutex_take(&mutex_data, RT_WAITING_FOREVER);
    float temp = g_sensor.water_temp;
    uint8_t wl = g_sensor.water_level;
    uint16_t air = g_sensor.air_quality;
    float t_low = g_threshold.temp_lower;
    float t_high = g_threshold.temp_upper;
    uint8_t wl_min = g_threshold.water_level_min;
    uint8_t wl_max = g_threshold.water_level_max;
    uint16_t air_max = g_threshold.air_quality_max;
    rt_mutex_release(&mutex_data);

    /* 温度控制 */
    if(temp < t_low)
        relay_set(RELAY_HEAT_PORT, RELAY_HEAT_PIN, 1);
    else if(temp > t_high)
        relay_set(RELAY_HEAT_PORT, RELAY_HEAT_PIN, 0);

    /* 水位控制 */
    if(wl > wl_max) {
        relay_set(RELAY_FILL_PORT, RELAY_FILL_PIN, 0);
        relay_set(RELAY_DRAIN_PORT, RELAY_DRAIN_PIN, 1);
    } else if(wl < wl_min) {
        relay_set(RELAY_DRAIN_PORT, RELAY_DRAIN_PIN, 0);
        relay_set(RELAY_FILL_PORT, RELAY_FILL_PIN, 1);
    } else {
        relay_set(RELAY_FILL_PORT, RELAY_FILL_PIN, 0);
        relay_set(RELAY_DRAIN_PORT, RELAY_DRAIN_PIN, 0);
    }

    /* 空气质量 */
    if(air > air_max)
        relay_set(RELAY_OXYGEN_PORT, RELAY_OXYGEN_PIN, 1);
    else
        relay_set(RELAY_OXYGEN_PORT, RELAY_OXYGEN_PIN, 0);
}

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
    rt_thread_t tid = rt_thread_create("t_ctrl", control_thread_entry, RT_NULL, 1024, 12, 10);
    if(tid) rt_thread_startup(tid);
}
