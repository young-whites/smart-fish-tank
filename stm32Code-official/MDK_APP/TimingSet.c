#include "TimingSet.h"
#include "MyTypedef.h"
#include "bsp_Key.h"
#include "ds18b20.h"
#include "bsp_beep.h"
#include "bsp_adc.h"
#include "stm32f10x_tim.h"

extern void OLED_Show_Page(uint8_t page);
extern void OLED_Clr_Screen(void);
extern void OLED_ShowStart(void);


static uint8_t _lastPage = 0xFF;


/* Servo angle: 0~180 degrees, maps to PWM duty 500~2500us (period 20ms=20000us) */
void Servo_SetAngle(uint8_t angle)
{
	uint16_t pulse;
	if (angle > 180) angle = 180;
	pulse = 500 + (uint16_t)((uint32_t)angle * 2000 / 180);
	TIM_SetCompare1(TIM3, pulse);
}


void Timing_1s(void)
{
	/* DS18B20 read with fault detection */
	{
		float t = DS18B20_GetTemperture();
		/* DS18B20 returns 85.0 on power-up error, -127.0 on read failure */
		if (t > -50.0f && t < 125.0f) {
			Record.waterTemp = t;
			Flag.sensorError &= ~0x01;  /* Clear DS18B20 error */
		} else {
			Flag.sensorError |= 0x01;   /* Set DS18B20 error */
		}
	}

	/* Feed control */
	if (Flag.feeding == 1) {
		if (Record.feedCountdown > 30 - 3) {
			/* First 3 seconds: servo open */
			Servo_SetAngle(90);
		} else {
			/* After 3 seconds: servo close */
			Servo_SetAngle(0);
		}
		if (Record.feedCountdown > 0) {
			Record.feedCountdown--;
		}
		if (Record.feedCountdown == 0) {
			Flag.feeding = 0;
			Servo_SetAngle(0);
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
	/* ADC polling acquisition (Water level, PH, Air quality) */
	static uint8_t adcCh = 0;
	switch (adcCh) {
		case 0: /* Water level PA1 ADC1_IN1 */
			ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_55Cycles5);
			ADC_SoftwareStartConvCmd(ADC1, ENABLE);
			while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
			Record.waterLevel = (uint8_t)(ADC_GetConversionValue(ADC1) * 100 / 4095);
			if (Record.waterLevel > 100) Record.waterLevel = 100;
			break;
		case 1: /* PH PA0 ADC1_IN0 */
			ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5);
			ADC_SoftwareStartConvCmd(ADC1, ENABLE);
			while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
			Record.phValue = ADC_GetConversionValue(ADC1) * 14.0f / 4095.0f;
			if (Record.phValue > 14.0f) Record.phValue = 14.0f;
			if (Record.phValue < 0.0f) Record.phValue = 0.0f;
			break;
		case 2: /* Air quality PA4 ADC1_IN4 */
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
	/* --- Auto control (Auto mode only) --- */
	if (Record.runMode == 0) {
		/* Temperature hysteresis control */
		if (Record.waterTemp < Record.tempLower) {
			Flag.relayHeat = 1;
		} else if (Record.waterTemp > Record.tempUpper) {
			Flag.relayHeat = 0;
		}

		/* Water level range control */
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

		/* Air quality control */
		if (Record.airQuality > Record.airQualityMax) {
			Flag.relayOxygen = 1;
		} else {
			Flag.relayOxygen = 0;
		}
	}

	/* --- Alarm check --- */
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
