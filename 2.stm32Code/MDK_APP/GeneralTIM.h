#ifndef __GENERALTIM_H
#define __GENERALTIM_H
#include "sys.h"



#define GENERAL_TIM2    1
#define GENERAL_TIM3    0  /* 禁用: TIM3 已被舵机PWM(bsp_servo.c)占用 */

#if  		GENERAL_TIM2
#define            GENERAL_TIM_2                   TIM2
#define            GENERAL_TIM_2_APBxClock_FUN     RCC_APB1PeriphClockCmd
#define            GENERAL_TIM_2_CLK               RCC_APB1Periph_TIM2
#define            GENERAL_TIM_2_Period            (1000-1)
#define            GENERAL_TIM_2_Prescaler         71
#define            GENERAL_TIM_2_IRQ               TIM2_IRQn
#define            GENERAL_TIM_2_IRQHandler        TIM2_IRQHandler
#endif /*GENERAL_TIM2*/




#if  		GENERAL_TIM3
#define            GENERAL_TIM_3                   TIM3
#define            GENERAL_TIM_3_APBxClock_FUN     RCC_APB1PeriphClockCmd
#define            GENERAL_TIM_3_CLK               RCC_APB1Periph_TIM3
#define            GENERAL_TIM_3_Period            (1000-1)
#define            GENERAL_TIM_3_Prescaler         71
#define            GENERAL_TIM_3_IRQ               TIM3_IRQn
#define            GENERAL_TIM_3_IRQHandler        TIM3_IRQHandler
#endif /*GENERAL_TIM3*/



void GENERAL_TIM_Init(void);

#endif /*__GENERALTIM_H*/
