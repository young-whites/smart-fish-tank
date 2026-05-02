#include "app_data.h"
#include "app_key.h"
#include "stm32f1xx.h"
#include <stdint.h>

uint8_t g_key_event = 0;

/* KEY1=PA8, KEY2=PB3, KEY3=PB4, KEY4=PB8, KEY5=PB1 */
/* 所有按键: 上拉输入, 低电平有效 */

static void key_gpio_init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN;

    /* PA8: 上拉输入 -> CRH[3:0] */
    GPIOA->CRH &= ~(0xFU << 0);
    GPIOA->CRH |=  (0x8U << 0);  /* MODE=00, CNF=10 */
    GPIOA->BSRR = (1U << 8);     /* 上拉 */

    /* PB1: 上拉输入 -> CRL[7:4] */
    GPIOB->CRL &= ~(0xFU << 4);
    GPIOB->CRL |=  (0x8U << 4);
    GPIOB->BSRR = (1U << 1);

    /* PB3: 上拉输入 -> CRL[15:12] */
    GPIOB->CRL &= ~(0xFU << 12);
    GPIOB->CRL |=  (0x8U << 12);
    GPIOB->BSRR = (1U << 3);

    /* PB4: 上拉输入 -> CRL[19:16] */
    GPIOB->CRL &= ~(0xFU << 16);
    GPIOB->CRL |=  (0x8U << 16);
    GPIOB->BSRR = (1U << 4);

    /* PB8: 上拉输入 -> CRH[3:0] */
    GPIOB->CRH &= ~(0xFU << 0);
    GPIOB->CRH |=  (0x8U << 0);
    GPIOB->BSRR = (1U << 8);
}

static uint8_t key_scan(void)
{
    if(!(GPIOA->IDR & (1U << 8))) return 1;   /* KEY1 */
    if(!(GPIOB->IDR & (1U << 3))) return 2;   /* KEY2 */
    if(!(GPIOB->IDR & (1U << 4))) return 3;   /* KEY3 */
    if(!(GPIOB->IDR & (1U << 8))) return 4;   /* KEY4 */
    if(!(GPIOB->IDR & (1U << 1))) return 5;   /* KEY5 */
    return 0;
}

static struct rt_thread key_thread;
static rt_uint8_t key_stack[512];

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
