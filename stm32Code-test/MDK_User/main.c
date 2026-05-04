#include <stdio.h>	 	 
#include <string.h>	
#include <stdarg.h>
#include "sys.h"
#include "bsp_Key.h"
#include "oled_show.h"
#include "MyTypedef.h"
#include "ds18b20.h"

/*******************************************/
/*              中断优先级分配             */
/*-----------------------------------------*/
/*	TIM2    -> 主优先级：2  子优先级：0
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

	/* 全局参数初始化 */
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

	/* 显示启动画面 */
	OLED_ShowStart();
	delay_ms(2000);
	OLED_Clr_Screen();

	{
		static uint8_t lastPage = 0xFF;
		uint8_t curPage;
		uint32_t tempReadCnt = 0;

		while ( 1 )
		{
			/* 每500ms读取一次DS18B20温度 */
			if (++tempReadCnt >= 24) {
				tempReadCnt = 0;
				Record.waterTemp = DS18B20_GetTemperture();
			}

			/* 检测页面变化, 变化时清屏 */
			curPage = Flag.currentPage * 10 + Flag.subPage;
			if (curPage != lastPage) {
				lastPage = curPage;
				OLED_Clr_Screen();
			}

			/* 刷新OLED显示 */
			OLED_Show_Page(Flag.currentPage);

			delay_ms(125);
		}
	}
}


/*********************************************END OF FILE**********************/
