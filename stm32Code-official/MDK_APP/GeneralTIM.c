#include "GeneralTIM.h"

/*
 * Note: TIM_TimeBaseInitTypeDef structure has 5 members for TIM6 and TIM7,
 * but only TIM_Prescaler and TIM_Period are used. When using TIM6 or TIM7,
 * only initialize these two members. Other members are for general and advanced timers.
 *---------------------------------------------------------------------
 *typedef struct
 *{ TIM_Prescaler            Prescaler
 *	TIM_CounterMode			 TIMx,x[6,7] not available, reserved
 *  TIM_Period               Period
 *  TIM_ClockDivision        TIMx,x[6,7] not available, reserved
 *  TIM_RepetitionCounter    TIMx,x[1,8,15,16,17] only
 *}TIM_TimeBaseInitTypeDef; 
 *---------------------------------------------------------------------
 */





// Interrupt priority config
static void GENERAL_TIM_2_NVIC_Config(void)
{
    NVIC_InitTypeDef 	NVIC_InitStructure; 
		// Set interrupt priority group to 4
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);		
		// Set interrupt source
    NVIC_InitStructure.NVIC_IRQChannel = GENERAL_TIM_2_IRQ ;	
		// Set preempt priority to 2
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;	 
		// Set sub priority to 0
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}







static void GENERAL_TIM_2_Mode_Config(void)
{
/*-------------------- Timer structure init -------------------------*/

	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

		// Enable timer clock, internal clock CK_INT=72M
    GENERAL_TIM_2_APBxClock_FUN(GENERAL_TIM_2_CLK, ENABLE);	
		// Auto-reload register value, accumulate TIM_Period+1 frequency then generate update interrupt
    TIM_TimeBaseStructure.TIM_Period=GENERAL_TIM_2_Period;
		// Timer prescaler
    TIM_TimeBaseStructure.TIM_Prescaler= GENERAL_TIM_2_Prescaler;	
		// Clock division, not used, reserved
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;		
		// Counter mode, set to up-counting
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 		
		// Repetition counter value, not used, reserved
	TIM_TimeBaseStructure.TIM_RepetitionCounter=0;	
		// Initialize timer
    TIM_TimeBaseInit(GENERAL_TIM_2, &TIM_TimeBaseStructure);
	
		// Clear timer update interrupt flag
    TIM_ClearFlag(GENERAL_TIM_2, TIM_FLAG_Update);
	  
		// Enable timer update interrupt
    TIM_ITConfig(GENERAL_TIM_2,TIM_IT_Update,ENABLE);
	
		// Enable counter
    TIM_Cmd(GENERAL_TIM_2, ENABLE);
}


/*  End of General Timer TIM2 Configuration ****************************************************************************************************************************************/






// Interrupt priority config
static void GENERAL_TIM_3_NVIC_Config(void)
{
	NVIC_InitTypeDef 	NVIC_InitStructure;
	// Set interrupt priority group to 4
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	// Set interrupt source
	NVIC_InitStructure.NVIC_IRQChannel = GENERAL_TIM_3_IRQ;
	// Set preempt priority to 2
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	// Set sub priority to 0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}







static void GENERAL_TIM_3_Mode_Config(void)
{
	/*-------------------- Timer structure init -------------------------*/

	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	// Enable timer clock, internal clock CK_INT=72M
	GENERAL_TIM_3_APBxClock_FUN(GENERAL_TIM_3_CLK, ENABLE);
	// Auto-reload register value, accumulate TIM_Period+1 frequency then generate update interrupt
	TIM_TimeBaseStructure.TIM_Period = GENERAL_TIM_3_Period;
	// Timer prescaler
	TIM_TimeBaseStructure.TIM_Prescaler = GENERAL_TIM_3_Prescaler;
	// Clock division, not used, reserved
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	// Counter mode, set to up-counting
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	// Repetition counter value, not used, reserved
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	// Initialize timer
	TIM_TimeBaseInit(GENERAL_TIM_3, &TIM_TimeBaseStructure);

	// Clear timer update interrupt flag
	TIM_ClearFlag(GENERAL_TIM_3, TIM_FLAG_Update);

	// Enable timer update interrupt
	TIM_ITConfig(GENERAL_TIM_3, TIM_IT_Update, ENABLE);

	// Enable counter
	TIM_Cmd(GENERAL_TIM_3, DISABLE);
}


/*  End of General Timer TIM3 Configuration ****************************************************************************************************************************************/





void GENERAL_TIM_Init(void)
{
	GENERAL_TIM_2_NVIC_Config();
	GENERAL_TIM_2_Mode_Config();

	GENERAL_TIM_3_NVIC_Config();
	GENERAL_TIM_3_Mode_Config();
}
/*   End of General Timer TIM3 Configuration ****************************************************************************************************************************************/



