#include "app_data.h"
#include "app_key.h"
#include "stm32f1xx.h"
#include <stdint.h>

uint8_t g_key_event = 0;

/* KEY1=PA6, KEY2=PA7, KEY3=PA4, KEY4=PA3, KEY5=PB0 */
/* 所有按键: 下拉输入, 高电平有效 */

static void key_gpio_init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN;

    /* PA3: 下拉输入 -> CRL[15:12] */
    GPIOA->CRL &= ~(0xFU << 12);
    GPIOA->CRL |=  (0x8U << 12);
    GPIOA->BRR = (1U << 3);

    /* PA4: 下拉输入 -> CRL[19:16] */
    GPIOA->CRL &= ~(0xFU << 16);
    GPIOA->CRL |=  (0x8U << 16);
    GPIOA->BRR = (1U << 4);

    /* PA6: 下拉输入 -> CRL[27:24] */
    GPIOA->CRL &= ~(0xFU << 24);
    GPIOA->CRL |=  (0x8U << 24);
    GPIOA->BRR = (1U << 6);

    /* PA7: 下拉输入 -> CRL[31:28] */
    GPIOA->CRL &= ~(0xFU << 28);
    GPIOA->CRL |=  (0x8U << 28);
    GPIOA->BRR = (1U << 7);

    /* PB0: 下拉输入 -> CRL[3:0] */
    GPIOB->CRL &= ~(0xFU << 0);
    GPIOB->CRL |=  (0x8U << 0);
    GPIOB->BRR = (1U << 0);
}

static uint8_t key_scan(void)
{
    if( (GPIOA->IDR & (1U << 6))) return 1;   /* KEY1=PA6 */
    if( (GPIOA->IDR & (1U << 7))) return 2;   /* KEY2=PA7 */
    if( (GPIOA->IDR & (1U << 4))) return 3;   /* KEY3=PA4 */
    if( (GPIOA->IDR & (1U << 3))) return 4;   /* KEY4=PA3 */
    if( (GPIOB->IDR & (1U << 0))) return 5;   /* KEY5=PB0 */
    return 0;
}

static struct rt_thread key_thread;
static rt_uint8_t key_stack[256];

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
    rt_thread_init(&key_thread,
        "t_key", key_thread_entry, RT_NULL,
        &key_stack[0], sizeof(key_stack),
        14, 10);
    rt_thread_startup(&key_thread);
}
