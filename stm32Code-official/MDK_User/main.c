#include <stdio.h>	 	 
#include <string.h>	
#include <stdarg.h>
#include "sys.h"
#include "bsp_Key.h"
#include "oled_show.h"
#include "MyTypedef.h"
#include "ds18b20.h"
#include "bsp_adc.h"

/*******************************************/
/*              �ж����ȼ�����             */
/*-----------------------------------------*/
/*	TIM2    -> �����ȼ���2  �����ȼ���0
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

	/* 继电器 GPIO 初始化 */
	{
		GPIO_InitTypeDef GPIO_InitStructure;
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_14 | GPIO_Pin_15;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOB, &GPIO_InitStructure);

		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
		GPIO_Init(GPIOA, &GPIO_InitStructure);

		/* 默认关闭所有继电器 */
		GPIO_ResetBits(GPIOB, GPIO_Pin_12 | GPIO_Pin_14 | GPIO_Pin_15);
		GPIO_ResetBits(GPIOA, GPIO_Pin_15);
	}

	/* ȫ�ֲ�����ʼ�� */
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

	/* ��ʾ�������� */
	OLED_ShowStart();
	delay_ms(2000);
	OLED_Clr_Screen();

	{
		static uint8_t lastPage = 0xFF;
		uint8_t curPage;

		while ( 1 )
		{
			/* 页面变化检测和清屏 */
			curPage = Flag.currentPage * 10 + Flag.subPage;
			if (curPage != lastPage) {
				lastPage = curPage;
				OLED_Clr_Screen();
			}

			/* 刷新OLED显示 */
			OLED_Show_Page(Flag.currentPage);

			delay_ms(100);
		}
	}
}


/*********************************************END OF FILE**********************/
