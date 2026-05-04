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
	/* Manual relay safety timeout */
	if (Record.runMode == 1) {
		/* Check if any relay is ON */
		if (Flag.relayHeat || Flag.relayFill || Flag.relayDrain || Flag.relayOxygen) {
			if (Flag.manualTimeout > 0) {
				Flag.manualTimeout--;
			}
			if (Flag.manualTimeout == 0) {
				/* Timeout: turn off all relays */
				Flag.relayHeat = 0;
				Flag.relayFill = 0;
				Flag.relayDrain = 0;
				Flag.relayOxygen = 0;
				BEEP_Blink(3, 0, 2);  /* Beep 3 times to warn */
			}
		} else {
			/* No relay ON: reset timeout */
			Flag.manualTimeout = 60;
		}
	} else {
		/* Auto mode: reset timeout */
		Flag.manualTimeout = 60;
	}

	/* DS18B20 read moved to main loop to avoid ISR blocking */

	/* Feed control */
	{
		static uint8_t servoOpenSec = 0;
		static uint16_t autoFeedTimer = 0;  /* Auto feed countdown */

		if (Record.runMode == 0) {
			/* Auto mode: auto feeding every 30s */
			if (Flag.feeding == 0) {
				autoFeedTimer++;
				Record.feedCountdown = Record.feedInterval - autoFeedTimer;
				if (autoFeedTimer >= Record.feedInterval) {
					Flag.feeding = 1;
					autoFeedTimer = 0;
					servoOpenSec = 0;
				}
			}
		} else {
			/* Manual mode: stop auto countdown */
			autoFeedTimer = 0;
		}

		if (Flag.feeding == 1) {
			if (servoOpenSec < 3) {
				Servo_SetAngle(90);
				servoOpenSec++;
			} else {
				Servo_SetAngle(0);
			}
			if (Record.feedCountdown > 0) {
				Record.feedCountdown--;
			}
			if (Record.feedCountdown == 0) {
				Flag.feeding = 0;
				Servo_SetAngle(0);
				servoOpenSec = 0;
			}
		} else {
			servoOpenSec = 0;
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
	/* ADC sequential read: all 3 channels every 15ms */
	static uint8_t adcTick = 0;
	adcTick++;
	if (adcTick >= 3) {
		adcTick = 0;

		/* Read water level PA1 (Channel 1) */
		ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_55Cycles5);
		ADC_SoftwareStartConvCmd(ADC1, ENABLE);
		{ uint16_t _to=1000; while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) && --_to); }
		Record.waterLevel = (uint8_t)(ADC_GetConversionValue(ADC1) * 100.0f / 2432.0f);
		if (Record.waterLevel > 100) Record.waterLevel = 100;

		/* Read PH PA0 (Channel 0) */
		ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5);
		ADC_SoftwareStartConvCmd(ADC1, ENABLE);
		{ uint16_t _to=1000; while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) && --_to); }
		Record.phValue = ADC_GetConversionValue(ADC1) * 14.0f / 4095.0f;
		if (Record.phValue > 14.0f) Record.phValue = 14.0f;
		if (Record.phValue < 0.0f) Record.phValue = 0.0f;

		/* Read air quality PA4 (Channel 4) */
		ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 1, ADC_SampleTime_55Cycles5);
		ADC_SoftwareStartConvCmd(ADC1, ENABLE);
		{ uint16_t _to=1000; while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) && --_to); }
		{
				uint16_t raw = ADC_GetConversionValue(ADC1);
				/* MQ-135: default ~40=clean air(20), max ~2000=heavy pollution(0) */
				if (raw <= 40) {
					Record.airQuality = 20;
				} else if (raw >= 2000) {
					Record.airQuality = 0;
				} else {
					Record.airQuality = (uint16_t)(20.0f - (raw - 40) * 20.0f / 1960.0f);
				}
			}
	}
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
		if (Record.airQuality < Record.airQualityMax) {
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
		if (Record.airQuality < Record.airQualityMax) alarm = 1;
		if (Record.phValue < Record.phLower || Record.phValue > Record.phUpper) alarm = 1;

		if (alarm && Flag.alarmEnable) {
			BEEP_Blink(1, 2, 100);
		} else {
			BEEP_Off();
		}
	}
}
