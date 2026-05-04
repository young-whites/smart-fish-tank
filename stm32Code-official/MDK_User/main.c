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

extern volatile uint32_t TimeCnt_ms;

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

	/* Servo PA6: TIM3_CH1 PWM output */
	{
		GPIO_InitTypeDef GPIO_InitStructure;
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
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
	Flag.manualTimeout = 60;

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
			/* DS18B20 read with fault detection (outside ISR to avoid blocking) */
			{
				static uint32_t dsReadCnt = 0;
				if (++dsReadCnt >= 100) {  /* Every ~10s at 100ms loop */
					dsReadCnt = 0;
					float t = DS18B20_GetTemperture();
					if (t > -50.0f && t < 125.0f) {
						Record.waterTemp = t;
						Flag.sensorError &= ~0x01;
					} else {
						Flag.sensorError |= 0x01;
					}
				}
			}

			/* Fill/Drain mutual exclusion safety */
			if (Flag.relayFill && Flag.relayDrain) {
				Flag.relayDrain = 0;  /* Drain takes priority */
			}

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

			/* Key debug: show GPIO states on OLED row0 */
			{
				uint8_t k1 = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8);
				uint8_t k2 = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_3);
				uint8_t k3 = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_4);
				uint8_t k4 = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_8);
				uint8_t k5 = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1);
				OLED_Show_OneCharF6X12(0, 30, k1 ? '0' : '1');
				OLED_Show_OneCharF6X12(0, 42, k2 ? '0' : '1');
				OLED_Show_OneCharF6X12(0, 54, k3 ? '0' : '1');
				OLED_Show_OneCharF6X12(0, 66, k4 ? '0' : '1');
				OLED_Show_OneCharF6X12(0, 78, k5 ? '0' : '1');
				/* TIM2 alive check: show running counter */
				OLED_ShowNumF6X12(6, 90, 5, TimeCnt_ms / 1000);
			}

			/* Refresh OLED display */
			OLED_Show_Page(Flag.currentPage);

			delay_ms(100);
		}
	}
}


/*********************************************END OF FILE**********************/
