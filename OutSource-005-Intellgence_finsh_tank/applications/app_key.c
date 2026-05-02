#include "app_data.h"
#include "app_key.h"
#include "stm32f1xx_hal.h"
#include <stdint.h>

uint8_t g_key_event = 0;

static void key_gpio_init(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitTypeDef g = {0};
    g.Mode = GPIO_MODE_INPUT;
    g.Pull = GPIO_PULLUP;
    g.Speed = GPIO_SPEED_FREQ_LOW;

    g.Pin = GPIO_PIN_8;
    HAL_GPIO_Init(GPIOA, &g);  /* KEY1 */

    g.Pin = GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_8;
    HAL_GPIO_Init(GPIOB, &g);  /* KEY2-4 */

    g.Pin = GPIO_PIN_1;
    HAL_GPIO_Init(GPIOB, &g);  /* KEY5 */
}

static uint8_t key_scan(void)
{
    if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8) == GPIO_PIN_RESET) return 1;
    if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_3) == GPIO_PIN_RESET) return 2;
    if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_4) == GPIO_PIN_RESET) return 3;
    if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_8) == GPIO_PIN_RESET) return 4;
    if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1) == GPIO_PIN_RESET) return 5;
    return 0;
}

static void key_thread_entry(void *param)
{
    key_gpio_init();
    uint8_t key, last_key = 0;
    uint8_t debounce = 0;

    while(1) {
        key = key_scan();
        if(key != 0 && key == last_key) {
            debounce++;
            if(debounce == 3) {
                g_key_event = key;
                rt_sem_release(&sem_key);
            }
        } else {
            debounce = 0;
        }
        last_key = key;
        rt_thread_mdelay(50);
    }
}

void app_key_init(void)
{
    rt_thread_t tid = rt_thread_create("t_key", key_thread_entry, RT_NULL, 512, 14, 10);
    if(tid) rt_thread_startup(tid);
}
