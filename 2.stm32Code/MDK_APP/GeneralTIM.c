#include "GeneralTIM.h"







static void GENERAL_TIM_2_NVIC_Config(void)
{
    NVIC_InitTypeDef 	NVIC_InitStructure; 

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);		

    NVIC_InitStructure.NVIC_IRQChannel = GENERAL_TIM_2_IRQ ;	
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	 

    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}







static void GENERAL_TIM_2_Mode_Config(void)
{

	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

    GENERAL_TIM_2_APBxClock_FUN(GENERAL_TIM_2_CLK, ENABLE);	
    TIM_TimeBaseStructure.TIM_Period=GENERAL_TIM_2_Period;
    TIM_TimeBaseStructure.TIM_Prescaler= GENERAL_TIM_2_Prescaler;	
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;		
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 		
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;	
    TIM_TimeBaseInit(GENERAL_TIM_2, &TIM_TimeBaseStructure);
	
    TIM_ClearFlag(GENERAL_TIM_2, TIM_FLAG_Update);
	  
    TIM_ITConfig(GENERAL_TIM_2,TIM_IT_Update,ENABLE);

    TIM_Cmd(GENERAL_TIM_2, ENABLE);
}








#if GENERAL_TIM3
// TIM3 中断优先级配置
static void GENERAL_TIM_3_NVIC_Config(void)
{
	NVIC_InitTypeDef 	NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	NVIC_InitStructure.NVIC_IRQChannel = GENERAL_TIM_3_IRQ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}







static void GENERAL_TIM_3_Mode_Config(void)
{

	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	GENERAL_TIM_3_APBxClock_FUN(GENERAL_TIM_3_CLK, ENABLE);

	TIM_TimeBaseStructure.TIM_Period = GENERAL_TIM_3_Period;

	TIM_TimeBaseStructure.TIM_Prescaler = GENERAL_TIM_3_Prescaler;

	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;

	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

	TIM_TimeBaseInit(GENERAL_TIM_3, &TIM_TimeBaseStructure);

	TIM_ClearFlag(GENERAL_TIM_3, TIM_FLAG_Update);

	TIM_ITConfig(GENERAL_TIM_3, TIM_IT_Update, ENABLE);

	TIM_Cmd(GENERAL_TIM_3, DISABLE);
}
#endif /* GENERAL_TIM3 */


void GENERAL_TIM_Init(void)
{
	GENERAL_TIM_2_NVIC_Config();
	GENERAL_TIM_2_Mode_Config();

#if GENERAL_TIM3
	GENERAL_TIM_3_NVIC_Config();
	GENERAL_TIM_3_Mode_Config();
#endif
}





