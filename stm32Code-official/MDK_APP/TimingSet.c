#include "TimingSet.h"
#include "MyTypedef.h"
#include "bsp_Key.h"
#include "ds18b20.h"
#include "bsp_beep.h"

extern void OLED_Show_Page(uint8_t page);
extern void OLED_Clr_Screen(void);
extern void OLED_ShowStart(void);


static uint8_t _lastPage = 0xFF;


void Timing_1s(void)
{

	if (Flag.feeding == 1) {
		if (Record.feedCountdown > 0) {
			Record.feedCountdown--;
		}
		if (Record.feedCountdown == 0) {
			Flag.feeding = 0;
		}
	}
}



void Timing_1ms(void)
{
	KEY_DrvScan();
}



void Timing_2ms(void)
{

}



void Timing_5ms(void)
{
	/* ADC 轮询采集（水位、PH、空气质量） */
	static uint8_t adcCh = 0;
	switch (adcCh) {
		case 0: /* 水位 PA1 ADC1_IN1 */
			ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_55Cycles5);
			ADC_SoftwareStartConvCmd(ADC1, ENABLE);
			while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
			Record.waterLevel = (uint8_t)(ADC_GetConversionValue(ADC1) * 100 / 4095);
			break;
		case 1: /* PH PA0 ADC1_IN0 */
			ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5);
			ADC_SoftwareStartConvCmd(ADC1, ENABLE);
			while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
			Record.phValue = ADC_GetConversionValue(ADC1) * 14.0f / 4095.0f;
			break;
		case 2: /* 空气质量 PA4 ADC1_IN4 */
			ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 1, ADC_SampleTime_55Cycles5);
			ADC_SoftwareStartConvCmd(ADC1, ENABLE);
			while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
			Record.airQuality = ADC_GetConversionValue(ADC1);
			break;
	}
	adcCh = (adcCh + 1) % 3;
}



void Timing_10ms(void)
{

}



void Timing_50ms(void)
{
	KEY_Scan();
}



void Timing_500ms(void)
{
	/* --- 传感器采集 --- */
	Record.waterTemp = DS18B20_GetTemperture();

	/* --- 自动控制（仅自动模式） --- */
	if (Record.runMode == 0) {
		/* 温度滞环控制 */
		if (Record.waterTemp < Record.tempLower) {
			Flag.relayHeat = 1;
		} else if (Record.waterTemp > Record.tempUpper) {
			Flag.relayHeat = 0;
		}

		/* 水位区间控制 */
		if (Record.waterLevel > Record.waterLevelMax) {
			Flag.relayDrain = 1;
			Flag.relayFill = 0;
		} else if (Record.waterLevel < Record.waterLevelMin) {
			Flag.relayFill = 1;
			Flag.relayDrain = 0;
		} else {
			Flag.relayFill = 0;
			Flag.relayDrain = 0;
		}

		/* 空气质量控制 */
		if (Record.airQuality > Record.airQualityMax) {
			Flag.relayOxygen = 1;
		} else {
			Flag.relayOxygen = 0;
		}
	}

	/* --- 继电器 GPIO 输出 --- */
	GPIO_WriteBit(GPIOB, GPIO_Pin_12, Flag.relayHeat ? Bit_SET : Bit_RESET);    /* 加热 */
	GPIO_WriteBit(GPIOB, GPIO_Pin_14, Flag.relayFill ? Bit_SET : Bit_RESET);    /* 加水 */
	GPIO_WriteBit(GPIOB, GPIO_Pin_15, Flag.relayDrain ? Bit_SET : Bit_RESET);   /* 排水 */
	GPIO_WriteBit(GPIOA, GPIO_Pin_15, Flag.relayOxygen ? Bit_SET : Bit_RESET);  /* 增氧 */

	/* --- 报警检测 --- */
	{
		uint8_t alarm = 0;
		if (Record.waterTemp < Record.tempLower || Record.waterTemp > Record.tempUpper) alarm = 1;
		if (Record.waterLevel < Record.waterLevelMin || Record.waterLevel > Record.waterLevelMax) alarm = 1;
		if (Record.airQuality > Record.airQualityMax) alarm = 1;
		if (Record.phValue < Record.phLower || Record.phValue > Record.phUpper) alarm = 1;

		if (alarm && Flag.alarmEnable) {
			BEEP_Blink(1, 2, 100);
		} else {
			BEEP_Off();
		}
	}
}
