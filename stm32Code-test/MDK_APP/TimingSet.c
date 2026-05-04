#include "TimingSet.h"
#include "MyTypedef.h"
#include "bsp_Key.h"

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

	if (Record.waterTemp < Record.tempLower) {

	}

	else if (Record.waterTemp > Record.tempUpper) {

	}


	if (Record.waterLevel < Record.waterLevelMin) {

		if (Record.runMode == 0) {
			Flag.relayFill = 1;
			Flag.relayDrain = 0;
		}
	}

	else if (Record.waterLevel > Record.waterLevelMax) {

		if (Record.runMode == 0) {
			Flag.relayFill = 0;
			Flag.relayDrain = 1;
		}
	}
	else {

		if (Record.runMode == 0) {
			Flag.relayFill = 0;
			Flag.relayDrain = 0;
		}
	}


	if (Record.airQuality > Record.airQualityMax) {

	}


	if (Record.phValue < Record.phLower) {

	}

	else if (Record.phValue > Record.phUpper) {

	}

	if (Record.runMode == 0) {
		if (Record.waterTemp < Record.tempLower) {
			Flag.relayHeat = 1;   
		} else if (Record.waterTemp > Record.tempUpper) {
			Flag.relayHeat = 0; 
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
	// 报�?�输出：蜂鸣�?+LED
	// 温度报�??
	if (Record.waterTemp < Record.tempLower || Record.waterTemp > Record.tempUpper) {
		if (Flag.alarmEnable) {
			BEEP_Blink(1, 0, 0);
		}
	}
	// 水位报�??
	else if (Record.waterLevel < Record.waterLevelMin || Record.waterLevel > Record.waterLevelMax) {
		if (Flag.alarmEnable) {
			BEEP_Blink(1, 0, 0);
		}
	}
	// 空气质量报�??
	else if (Record.airQuality > Record.airQualityMax) {
		if (Flag.alarmEnable) {
			BEEP_Blink(1, 0, 0);
		}
	}
}
