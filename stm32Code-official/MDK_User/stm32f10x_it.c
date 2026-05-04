/**
  ******************************************************************************
  * @file    GPIO/IOToggle/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and peripherals
  *          interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h" 
#include "sys.h"

 
 
/********** Global variables *******************/
volatile uint32_t TimeCnt_ms = 0; // ms timer counter
 
 
void NMI_Handler(void)
{
}
 
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}
 
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

 
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}
 
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}
 
void SVC_Handler(void)
{
}
 
void DebugMon_Handler(void)
{
}
 
void PendSV_Handler(void)
{
}
 
/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{

}


















/**
  * @brief  This function handles Basic TIM6 interrupt request.
  * @param  None
  * @retval None
  */
void  GENERAL_TIM_2_IRQHandler (void)
{
	static int msCnt=0;
	
	
	if ( TIM_GetITStatus( GENERAL_TIM_2, TIM_IT_Update) != RESET ) 
	{	
		TimeCnt_ms++;
		
		/* Scan time counter, overflow wraps at 60000 */
		if (++msCnt >= 60000)
		{
			msCnt = 0;
		}

		/* 1s scan time: when msCnt is multiple of period, execute function */
		if ((msCnt % 1000) == 0)		Timing_1s();
		if ((msCnt % 500)  == 0)		Timing_500ms();
		if ((msCnt % 50)   == 0)		Timing_50ms();
		if ((msCnt % 10)   == 0)		Timing_10ms();
		if ((msCnt % 5)    == 0)		Timing_5ms();
		if ((msCnt % 2)    == 0)		Timing_2ms();
										Timing_1ms();
		
		
		TIM_ClearITPendingBit(GENERAL_TIM_2 , TIM_FLAG_Update);  		 
	}		 	
}




















/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/**
  * @brief  This function handles TIM3 interrupt.
  * @note   TIM3 is used for servo PWM, no update interrupt processing needed.
  */
void TIM3_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM3, TIM_FLAG_Update);
	}
}

/******************************************************************************/
/******************************************************************************/
