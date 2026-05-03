#include "app_data.h"
#include "app_feed.h"
#include "stm32f1xx.h"
#include <stdint.h>

/* 舵机 PWM: TIM3 CH1 -> PA6 (复用推挽输出) */

static void servo_init(void)
{
    /* 使能 TIM3, GPIOA, AFIO 时钟 */
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;

    /* PA6: 复用推挽输出 50MHz -> CRL[27:24] */
    /* MODE=11, CNF=10 */
    GPIOA->CRL &= ~(0xFU << 24);
    GPIOA->CRL |=  (0xBU << 24);

    /* TIM3 基本配置 */
    TIM3->PSC = 71;       /* 72MHz / (71+1) = 1MHz */
    TIM3->ARR = 19999;    /* 1MHz / (19999+1) = 50Hz */
    TIM3->CCR1 = 1500;    /* 中位脉宽 1.5ms */
    TIM3->CCMR1 &= ~TIM_CCMR1_OC1M;
    TIM3->CCMR1 |= (6U << TIM_CCMR1_OC1M_Pos);  /* PWM mode 1 */
    TIM3->CCMR1 |= TIM_CCMR1_OC1PE;             /* 预装载使能 */
    TIM3->CR1 |= TIM_CR1_ARPE;                   /* ARR 预装载 */
    /* 不启动定时器, 等待触发 */
}

static void servo_open(void)
{
    TIM3->CCR1 = 2500;
    TIM3->CCER |= TIM_CCER_CC1E;
    TIM3->CR1 |= TIM_CR1_CEN;
}

static void servo_close(void)
{
    TIM3->CR1 &= ~TIM_CR1_CEN;
    TIM3->CCER &= ~TIM_CCER_CC1E;
}

static volatile uint8_t feed_trigger = 0;

void app_feed_trigger(void)
{
    feed_trigger = 1;
}

static struct rt_thread feed_thread;
static rt_uint8_t feed_stack[384];

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
    rt_thread_init(&feed_thread,
        "t_feed", feed_thread_entry, RT_NULL,
        &feed_stack[0], sizeof(feed_stack),
        16, 10);
    rt_thread_startup(&feed_thread);
}
