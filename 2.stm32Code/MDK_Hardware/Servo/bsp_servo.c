#include "bsp_servo.h"

/*
 * 舵机PWM配置:
 * TIM3_CH1 on PA6
 * 72MHz / 72 = 1MHz 计数频率
 * 周期: 20000 counts = 20ms = 50Hz
 * 脉宽: 500(0°) ~ 2500(180°)
 */

/**
 * @brief  舵机初始化 - PA6, TIM3_CH1, 50Hz PWM
 */
void Servo_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;

    /* 使能时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    /* PA6 配置为复用推挽输出 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* TIM3 时基配置: 72MHz / 72 = 1MHz, 周期20000 = 20ms(50Hz) */
    TIM_TimeBaseStructure.TIM_Period = 20000 - 1;
    TIM_TimeBaseStructure.TIM_Prescaler = 72 - 1;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    /* TIM3_CH1 PWM模式1 */
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 1500;        /* 默认1.5ms = 90° */
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC1Init(TIM3, &TIM_OCInitStructure);

    /* 使能预装载 */
    TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
    TIM_ARRPreloadConfig(TIM3, ENABLE);

    /* 使能TIM3 */
    TIM_Cmd(TIM3, ENABLE);
}

/**
 * @brief  设置舵机角度
 * @param  angle: 0~180
 * @retval None
 */
void Servo_SetAngle(uint8_t angle)
{
    uint16_t pulse;

    if (angle > 180) angle = 180;

    /* 0°→500, 180°→2500 */
    pulse = 500 + (uint16_t)((uint32_t)angle * 2000 / 180);

    TIM_SetCompare1(TIM3, pulse);
}

/**
 * @brief  关闭舵机（输出0脉宽）
 */
void Servo_Off(void)
{
    TIM_SetCompare1(TIM3, 0);
}
