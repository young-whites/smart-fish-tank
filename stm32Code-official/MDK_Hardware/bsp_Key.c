#include "bsp_Key.h"
#include "MyTypedef.h"

extern void OLED_Clr_Screen(void);
extern void OLED_ShowStart(void);

void KEY_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Disable JTAG, keep SWD (release PB3, PB4, PA15 for GPIO) */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

	/* KEY1 PA8 Pull-down input */
	macKEY_A_GPIO_APBxClock_FUN(macKEY_A_GPIO_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = macKEY_A_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;    // Pull-down input
	GPIO_Init(macKEY_A_GPIO_PORT, &GPIO_InitStructure);

	/* KEY2 PB3 Pull-down input (need to disable JTAG, enable SWD) */
	macKEY_B_GPIO_APBxClock_FUN(macKEY_B_GPIO_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = macKEY_B_GPIO_PIN | macKEY_C_GPIO_PIN | macKEY_D_GPIO_PIN | macKEY_E_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;    // Pull-down input
	GPIO_Init(macKEY_B_GPIO_PORT, &GPIO_InitStructure);


}



/* Key driver scan parameters **************************************************************************************************************************************************************/


//----------------------------------------------------------------------------
#define		KEY_MAX						(5)			// Max supported key count
//----------------------------------------------------------------------------
#define		KEY_VAL_BUF_SIZE			(KEY_MAX+3)	// Key value buffer size
#define		KEY_SCAN_FILTER_TIMES		(5)			// Scan filter count (filter time = count * scan period, e.g. scan period 10ms, filter time 50ms)
#define		KEY_SCAN_LONG_PRESS_2S		(200)		// Long press 2s count threshold (count = value * scan period, e.g. scan period 10ms, long press time = 2s)
#define		KEY_SCAN_LONG_PRESS_4S		(400)		// Long press 4s count threshold (count = value * scan period, e.g. scan period 10ms, long press time = 4s)
//----------------------------------------------------------------------------
#define		KEY_Val_First				(0x01)		// First key value in port_key.c should match this, subsequent values increment
//----------------------------------------------------------------------------
static	uint8_t	_keyValBuf[KEY_VAL_BUF_SIZE];		// Key value buffer
static	uint8_t	_keyValBufR = 0;						// Key value buffer read index (decremented when reading buffer, reset to 0 when empty)
static	uint8_t	_keyValBufW = 0;						// Key value buffer write index (incremented when writing buffer, wraps around when full)
static	uint8_t	_keyValBufCnt = 0;					// Key value buffer pending read count
//----------------------------------------------------------------------------


/*****************************************************************************
* Name:     Key value write
* Params:   value - key value
*****************************************************************************/
void KEY_Write(uint8_t value)
{
	if (_keyValBufCnt < KEY_VAL_BUF_SIZE) {			// If buffer not full, allow write; otherwise discard
		_keyValBufCnt++;							// Increment count on each write
		_keyValBuf[_keyValBufW] = value;				// Write value to buffer
		if (++_keyValBufW >= KEY_VAL_BUF_SIZE) {		// Check write position
			_keyValBufW = 0;							// Wrap around
		}
	}
}


/*****************************************************************************
* Name:     Key value read
* Return:   Key value, 0 means no key
*****************************************************************************/
uint8_t KEY_Read(void)
{
	uint8_t	value = 0;
	if (_keyValBufCnt) {							// If buffer has readable key values
		_keyValBufCnt--;							// Decrement count on each read
		value = _keyValBuf[_keyValBufR];				// Read key value from buffer
		if (++_keyValBufR >= KEY_VAL_BUF_SIZE) {		// Check read position
			_keyValBufR = 0;							// Wrap around
		}
	}
	return value;
}





/*****************************************************************************
* Name:     Key scan
* Note:     Scan period: 10ms, Filter time: 50ms, Long press time: 2s
*****************************************************************************/
// Forward declarations
extern uint8_t KEY_GetState(uint8_t keyName);
extern uint8_t KEY_GetNumber(void);

void KEY_DrvScan(void)
{
	static	uint8_t	    step[KEY_MAX] = { 0 };						// Scan state
	static	uint16_t	holdingCnt[KEY_MAX] = { 0 };				// Hold timer
	static	uint8_t	    filterCnt[KEY_MAX] = { 0 };					// Filter timer
	static	uint8_t	    pressLast[KEY_MAX] = { 0 };					// Previous key state
	uint8_t	i, num, pressCurr;										// Current key state

	num = KEY_GetNumber();											// Get total key count
	if (num > KEY_MAX)	num = KEY_MAX;								// Key count must not exceed range
	for (i = 0; i < num; i++) {										// Scan each key
		pressCurr = KEY_GetState(KEY_Val_First + i);				// Get current key press state
		switch (step[i]) {											// Scan state: stable or filtering
		case 0:														// Stable state (key released or held)
			if (pressCurr != pressLast[i]) {						// Key state changed
				pressLast[i] = pressCurr;							// Record new key state
				filterCnt[i] = 0;									// Start filter timer
				step[i] = 1;										// Switch to filter confirm state
			}
			else if (pressCurr) {									// Key continuously held
				if (holdingCnt[i] != 0xffff) {						// Prevent overflow repeat
					holdingCnt[i]++;
					if (holdingCnt[i] == KEY_SCAN_LONG_PRESS_2S) {		// Long press 2s reached
						KEY_Write((KEY_Val_First + i) + KEY_Evt_Long2S);// Write key value to buffer
					}
					if (holdingCnt[i] == KEY_SCAN_LONG_PRESS_4S) {		// Long press 4s reached
						KEY_Write((KEY_Val_First + i) + KEY_Evt_Long4S);// Write key value to buffer
					}
				}
			}
			break;
		case 1:															// Filter state (debouncing)
			if (pressCurr == pressLast[i]) {
				if (++filterCnt[i] >= KEY_SCAN_FILTER_TIMES) {			// If N consecutive same states, confirm key change
					if (pressCurr) {									// Confirmed pressed
						KEY_Write((KEY_Val_First + i) + KEY_Evt_Press); // Write key value to buffer
						holdingCnt[i] = 0;								// Start hold timer
					}
					else {												// Confirmed released
						KEY_Write((KEY_Val_First + i) + KEY_Evt_Release);// Write key value to buffer
					}
					step[i] = 0;										// Return to stable state
				}
			}
			else {														// Bounce detected, reset
				pressLast[i] = pressCurr;								// Update key state
				step[i] = 0;											// Return to stable state
			}
			break;
		default:
			step[i] = 0;												// Invalid state, force return to stable
			break;
		}
	}
}


/* Key event handler functions ***************************************************************************************************************************************/



/*****************************************************************************
* Name:     Read current key state
* Return:   0=not pressed, >0=pressed
* Note:     Pull-down input, key press pulls pin to GND=low level, return read value
*****************************************************************************/
uint8_t KEY_GetState(uint8_t keyName)
{
	uint8_t	stat = 0;
	switch (keyName)
	{
		case KEY_A:		stat = (GPIO_ReadInputDataBit(macKEY_A_GPIO_PORT, macKEY_A_GPIO_PIN) ? 0 : 1);	break;	// PA8
		case KEY_B:		stat = (GPIO_ReadInputDataBit(macKEY_B_GPIO_PORT, macKEY_B_GPIO_PIN) ? 0 : 1);	break;	// PB3
		case KEY_C:		stat = (GPIO_ReadInputDataBit(macKEY_C_GPIO_PORT, macKEY_C_GPIO_PIN) ? 0 : 1);	break;	// PB4
		case KEY_D:		stat = (GPIO_ReadInputDataBit(macKEY_D_GPIO_PORT, macKEY_D_GPIO_PIN) ? 0 : 1);	break;	// PB8
		case KEY_E:		stat = (GPIO_ReadInputDataBit(macKEY_E_GPIO_PORT, macKEY_E_GPIO_PIN) ? 0 : 1);	break;	// PB1
	}
	return stat;
}

/*****************************************************************************
* Name:     Get total key count
* Return:   Key count
*****************************************************************************/
uint8_t KEY_GetNumber(void)
{
	return KEY_NUM;
}




/*****************************************************************************
* Name:     Key A press handler (KEY1 - PA8)
* Function: Page switch: Main -> Status -> Threshold -> Main (cycle)
*****************************************************************************/
static void _KEYA_Press(void)
{
	Flag.currentPage++;
	if (Flag.currentPage > 2) {
		Flag.currentPage = 0;
	}
	// Reset selected param index when entering threshold page
	if (Flag.currentPage == 2) {
		Flag.thresholdIndex = 0;
	}
	OLED_Clr_Screen();
}


/*****************************************************************************
* Name:     Key B press handler (KEY2 - PB3)
* Function: Switch sub page on threshold page (cycle 0-3)
*           No effect on non-threshold pages
*****************************************************************************/
static void _KEYB_Press(void)
{
	// Sub page switch: page 1 has 3 sub pages, page 2 has 4, page 3 has 4
	/* Page 3 (manual control) in auto mode: no sub page switch */
	if (Flag.currentPage == 2 && Record.runMode == 0) {
		return;
	}

	uint8_t maxSub = 3;
	if (Flag.currentPage == 1) maxSub = 4;
	if (Flag.currentPage == 2) maxSub = 4;

	Flag.subPage++;
	if (Flag.subPage >= maxSub) {
		Flag.subPage = 0;
	}
	/* Air quality sub page (subPage==1) only has upper limit, force select upper */
	if (Flag.currentPage == 1 && Flag.subPage == 1) {
		Flag.thresholdField = 1;
	}
	/* Non-air sub page: reset to default selection */
	if (Flag.currentPage == 1 && Flag.subPage != 1) {
		Flag.thresholdField = 0;
	}
	OLED_Clr_Screen();
}



static void _KEYC_Press(void)
{
	if (Flag.currentPage == 1) {  // Threshold page
		if (Flag.thresholdField == 0) {  // Select lower: KEY3 increase
			switch (Flag.subPage) {
				case 0: Record.tempLower += THRESHOLD_TEMP_STEP;
					if (Record.tempLower > Record.tempUpper - 1.0f) Record.tempLower = Record.tempUpper - 1.0f; break;
				case 1: break;  // Air quality has no lower limit, skip
				case 2: Record.waterLevelMin += THRESHOLD_WL_STEP;
					if (Record.waterLevelMin > Record.waterLevelMax - 1) Record.waterLevelMin = Record.waterLevelMax - 1; break;
				case 3: Record.phLower += THRESHOLD_PH_STEP;
					if (Record.phLower > Record.phUpper - 0.1f) Record.phLower = Record.phUpper - 0.1f; break;
			}
		} else {  // Select upper: KEY3 increase
			switch (Flag.subPage) {
				case 0: Record.tempUpper += THRESHOLD_TEMP_STEP;
					if (Record.tempUpper > 50.0f) Record.tempUpper = 50.0f; break;
				case 1: Record.airQualityMax += 1;
					if (Record.airQualityMax > 20) Record.airQualityMax = 20; break;
				case 2: Record.waterLevelMax += THRESHOLD_WL_STEP;
					if (Record.waterLevelMax > 100) Record.waterLevelMax = 100; break;
				case 3: Record.phUpper += THRESHOLD_PH_STEP;
					if (Record.phUpper > 14.0f) Record.phUpper = 14.0f; break;
			}
		}
	}
	if (Flag.currentPage == 2) {  /* Page 3: short press toggles Auto/Manual */
		Record.runMode = !Record.runMode;
		if (Record.runMode == 0) {
			Flag.relayHeat = 0;
			Flag.relayFill = 0;
			Flag.relayDrain = 0;
			Flag.relayOxygen = 0;
		}
	}
}

static void _KEYC_LongPress(void)  // Long press 2s - mode switch
{
	Record.runMode = !Record.runMode;  // Auto <-> Manual
}


/*****************************************************************************
* Name:     Key D press handler (KEY4 - PB8)
* Function: Threshold page value decrease, no function on non-threshold pages
*****************************************************************************/
static void _KEYD_Press(void)
{
	if (Flag.currentPage == 1) {  // Threshold page
		if (Flag.thresholdField == 0) {  // Select lower: KEY4 decrease
			switch (Flag.subPage) {
				case 0: Record.tempLower -= THRESHOLD_TEMP_STEP;
					if (Record.tempLower < 0.0f) Record.tempLower = 0.0f; break;
				case 1: break;  // Air quality has no lower limit, skip
				case 2: if (Record.waterLevelMin >= THRESHOLD_WL_STEP)
						Record.waterLevelMin -= THRESHOLD_WL_STEP;
					else Record.waterLevelMin = 0; break;
				case 3: Record.phLower -= THRESHOLD_PH_STEP;
					if (Record.phLower < 0.0f) Record.phLower = 0.0f; break;
			}
		} else {  // Select upper: KEY4 decrease
			switch (Flag.subPage) {
				case 0: Record.tempUpper -= THRESHOLD_TEMP_STEP;
					if (Record.tempUpper < Record.tempLower + 1.0f) Record.tempUpper = Record.tempLower + 1.0f; break;
				case 1: if (Record.airQualityMax >= THRESHOLD_AIR_STEP)
						Record.airQualityMax -= 1;
					else Record.airQualityMax = 0; break;
				case 2: Record.waterLevelMax -= THRESHOLD_WL_STEP;
					if (Record.waterLevelMax < Record.waterLevelMin + 1) Record.waterLevelMax = Record.waterLevelMin + 1; break;
				case 3: Record.phUpper -= THRESHOLD_PH_STEP;
					if (Record.phUpper < Record.phLower + 0.1f) Record.phUpper = Record.phLower + 0.1f; break;
			}
		}
	}
	if (Flag.currentPage == 2 && Record.runMode == 1) {  // Manual control page, manual mode: toggle relay
		switch (Flag.subPage) {
			case 0: Flag.relayHeat = !Flag.relayHeat; break;
			case 1: Flag.relayFill = !Flag.relayFill; break;
			case 2: Flag.relayDrain = !Flag.relayDrain; break;
			case 3: Flag.relayOxygen = !Flag.relayOxygen; break;
		}
		Flag.manualTimeout = 60;  /* Reset safety timeout */
	}
}

static void _KEYE_Press(void)
{
	/* On threshold page, KEY5 short press toggles upper/lower */
	if (Flag.currentPage == 1) {
		Flag.thresholdField = !Flag.thresholdField;
	}
}

static void _KEYE_LongPress(void) 
{
	if (Flag.feeding == 0) {
		Flag.feeding = 1;
		Record.feedCountdown = 30; 
	}
}




extern void (*current_operation_index)(void);
void KEY_Scan(void)
{
	uint8_t	key, event;
	for (key = KEY_Read(); key; key = KEY_Read()) 
	{
		event = key & 0xf0;	
		key   = key & 0x0f;		
		switch (key)
		{
			case KEY_A:  
			{
				switch (event)
				{
					case KEY_Evt_Press:		_KEYA_Press();	break;	
				}
			}break;

			case KEY_B:  
			{
				switch (event)
				{
					case KEY_Evt_Press:		_KEYB_Press();	break;	
				}
			}break;

			case KEY_C:  
			{
				switch (event)
				{
					case KEY_Evt_Press:		_KEYC_Press();	break;	
					case KEY_Evt_Long2S:	_KEYC_LongPress();	break;
				}
			}break;

			case KEY_D:
			{
				switch (event)
				{
					case KEY_Evt_Press:		_KEYD_Press();	break;	
				}
			}break;

			case KEY_E: 
			{
				switch (event)
				{
					case KEY_Evt_Press:	_KEYE_Press();	break;
					case KEY_Evt_Long2S:	_KEYE_LongPress();	break;	
				}
			}break;
		}
	}

}







