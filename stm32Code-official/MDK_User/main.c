#include <stdio.h>	 	 
#include <string.h>	
#include <stdarg.h>
#include "sys.h"
#include "bsp_Key.h"
#include "oled_show.h"
#include "MyTypedef.h"
#include "ds18b20.h"
#include "bsp_adc.h"
#include "TimingSet.h"

/*******************************************/
/*              Interrupt Priority          */
/*-----------------------------------------*/
/*	TIM2    -> Preempt priority 2  Sub priority 0
 */

int main ( void )
{	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	delay_init(72);
	GENERAL_TIM_Init();

	LED_Init();
	Beep_Init();
	KEY_GPIO_Config();
	OLED_Init();
	DS18B20_Init();
	ADC_Polling_Init();

	/* Relay GPIO initialization */
	{
		GPIO_InitTypeDef GPIO_InitStructure;
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_14 | GPIO_Pin_15;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOB, &GPIO_InitStructure);

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
		GPIO_Init(GPIOA, &GPIO_InitStructure);

		/* Default: turn off all relays */
		GPIO_ResetBits(GPIOB, GPIO_Pin_12 | GPIO_Pin_14 | GPIO_Pin_15);
		GPIO_ResetBits(GPIOA, GPIO_Pin_15);
	}

	/* Global variable initialization */
	Flag.currentPage = 0;
	Flag.subPage = 0;
	Flag.thresholdField = 0;
	Flag.thresholdIndex = 0;
	Flag.alarmEnable = 1;
	Flag.feeding = 0;
	Flag.relayHeat = 0;
	Flag.relayFill = 0;
	Flag.relayDrain = 0;
	Flag.relayOxygen = 0;
	Flag.sensorError = 0;

	Record.waterTemp = 0.0f;
	Record.phValue = 7.2f;
	Record.waterLevel = 65;
	Record.airQuality = 230;
	Record.runMode = 0;
	Record.feedCountdown = 0;
	Record.tempLower = 20.0f;
	Record.tempUpper = 30.0f;
	Record.airQualityMax = 500;
	Record.phLower = 6.5f;
	Record.phUpper = 8.5f;
	Record.waterLevelMin = 40;
	Record.waterLevelMax = 80;

	/* Enable TIM3 for servo PWM */
	TIM_Cmd(TIM3, ENABLE);
	Servo_SetAngle(0);  /* Servo initial position */

	/* Show startup screen */
	OLED_ShowStart();
	delay_ms(2000);
	OLED_Clr_Screen();

	{
		static uint8_t lastPage = 0xFF;
		uint8_t curPage;

		while ( 1 )
		{
			/* Relay GPIO output */
			GPIO_WriteBit(GPIOB, GPIO_Pin_12, Flag.relayHeat ? Bit_SET : Bit_RESET);
			GPIO_WriteBit(GPIOB, GPIO_Pin_14, Flag.relayFill ? Bit_SET : Bit_RESET);
			GPIO_WriteBit(GPIOB, GPIO_Pin_15, Flag.relayDrain ? Bit_SET : Bit_RESET);
			GPIO_WriteBit(GPIOA, GPIO_Pin_15, Flag.relayOxygen ? Bit_SET : Bit_RESET);

			/* Page change detection and clear screen */
			curPage = Flag.currentPage * 10 + Flag.subPage;
			if (curPage != lastPage) {
				lastPage = curPage;
				OLED_Clr_Screen();
			}

			/* Refresh OLED display */
			OLED_Show_Page(Flag.currentPage);

			delay_ms(100);
		}
	}
}


/*********************************************END OF FILE**********************/
