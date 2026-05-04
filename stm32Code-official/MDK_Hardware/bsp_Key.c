#include "bsp_Key.h"
#include "MyTypedef.h"

extern void OLED_Clr_Screen(void);
extern void OLED_ShowStart(void);

void KEY_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;


	/* KEY1 PA8 涓嬫媺杈撳叆 */
	macKEY_A_GPIO_APBxClock_FUN(macKEY_A_GPIO_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = macKEY_A_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;    // 涓嬫媺杈撳叆
	GPIO_Init(macKEY_A_GPIO_PORT, &GPIO_InitStructure);

	/* KEY2 PB3 涓嬫媺杈撳叆 (闇?鍏抽棴JTAG, 寮?鍚疭WD) */
	macKEY_B_GPIO_APBxClock_FUN(macKEY_B_GPIO_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = macKEY_B_GPIO_PIN | macKEY_C_GPIO_PIN | macKEY_D_GPIO_PIN | macKEY_E_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;    // 涓嬫媺杈撳叆
	GPIO_Init(macKEY_B_GPIO_PORT, &GPIO_InitStructure);


}



/* 鎸夐敭椹卞姩鎵?鎻忔墍鐢ㄧ殑鍙傛暟瀹氫箟 **************************************************************************************************************************************************************/


//----------------------------------------------------------------------------
#define		KEY_MAX						(5)			// 鏀?鎸佹寜閿?鏁伴噺
//----------------------------------------------------------------------------
#define		KEY_VAL_BUF_SIZE			(KEY_MAX+3)	// 閿?鍊肩紦鍐插尯澶у皬
#define		KEY_SCAN_FILTER_TIMES		(5)			// 鎵?鎻忔护娉㈡?℃暟锛堟护娉㈡椂闂?=閿?鍊?*鎵?鎻忓懆鏈燂紝濡傦細鎵?鎻忓懆鏈熶负10ms锛屾护娉㈡椂闂翠负50ms锛屼互姝ょ被鎺?锛?
#define		KEY_SCAN_LONG_PRESS_2S		(200)		// 闀挎寜2s璁℃暟闃堝?硷紙璁℃暟鍊?=閿?鍊?*鎵?鎻忓懆鏈燂紝濡傦細鎵?鎻忓懆鏈熶负10ms鐨勯暱鎸夋椂闂? 涓? 2 s锛屼互姝ょ被鎺?锛?
#define		KEY_SCAN_LONG_PRESS_4S		(400)		// 闀挎寜4s璁℃暟闃堝?硷紙璁℃暟鍊?=閿?鍊?*鎵?鎻忓懆鏈燂紝濡傦細鎵?鎻忓懆鏈熶负10ms鐨勯暱鎸夋椂闂? 涓? 4 s锛屼互姝ょ被鎺?锛?
//----------------------------------------------------------------------------
#define		KEY_Val_First				(0x01)		// port_key.c涓?鐨勯?栦釜閿?鍊煎簲涓庢?ゅ?间竴鑷达紝鍚庣画閿?鍊间緷娆￠?掑??
//----------------------------------------------------------------------------
static	uint8_t	_keyValBuf[KEY_VAL_BUF_SIZE];		// 閿?鍊肩紦鍐插尯
static	uint8_t	_keyValBufR = 0;						// 閿?鍊肩紦鍐插尯璇昏?℃暟锛堣?诲彇缂撳啿鍖烘椂璇昏?℃暟閫掑噺锛岃?诲埌缂撳啿鍖轰负绌烘椂璁℃暟涓?0锛?
static	uint8_t	_keyValBufW = 0;						// 閿?鍊肩紦鍐插尯鍐欒?℃暟锛堝啓鍏ョ紦鍐插尯鏃跺啓璁℃暟閫掑?烇紝鍐欏埌缂撳啿鍖轰负婊℃椂璁℃暟閫掑?烇級
static	uint8_t	_keyValBufCnt = 0;					// 閿?鍊肩紦鍐插尯寰呰?诲彇鍊间釜鏁?
//----------------------------------------------------------------------------


/*****************************************************************************
* 鍚嶇О:		閿?鍊煎啓鍏?
* 鍙傛暟:		value锛岄敭鍊?
*****************************************************************************/
void KEY_Write(uint8_t value)
{
	if (_keyValBufCnt < KEY_VAL_BUF_SIZE) {			// 濡傛灉缂撳啿鍖烘湭鍐欐弧鍒欙紝鍏佽?稿綋鍓嶅啓鍏ュ?硷紝鍚﹀垯涓㈠純
		_keyValBufCnt++;							// 姣忓啓鍏ヤ竴涓?閿?鍊硷紝璁℃暟閫掑??
		_keyValBuf[_keyValBufW] = value;				// 鍐欏叆鍊煎埌缂撳啿鍖?
		if (++_keyValBufW >= KEY_VAL_BUF_SIZE) {		// 妫?鏌ュ啓鍏ヤ綅缃?
			_keyValBufW = 0;							// 褰掗浂
		}
	}
}


/*****************************************************************************
* 鍚嶇О:		閿?鍊艰?诲彇
* 杩斿洖:		閿?鍊硷紝0琛ㄧず娌℃湁閿?鍊?
*****************************************************************************/
uint8_t KEY_Read(void)
{
	uint8_t	value = 0;
	if (_keyValBufCnt) {							// 濡傛灉缂撳啿鍖烘湁鏈?璇诲彇鐨勯敭鍊?
		_keyValBufCnt--;							// 姣忚?诲嚭涓?涓?閿?鍊硷紝璁℃暟閫掑噺
		value = _keyValBuf[_keyValBufR];				// 璇诲彇缂撳啿鍖洪敭鍊?
		if (++_keyValBufR >= KEY_VAL_BUF_SIZE) {		// 妫?鏌ヨ?诲彇浣嶇疆
			_keyValBufR = 0;							// 褰掗浂
		}
	}
	return value;
}





/*****************************************************************************
* 鍚嶇О:		鎸夐敭鎵?鎻?
* 璇存槑:		鎵?鎻忓懆鏈燂細10ms锛屾护娉㈡椂闂达細50ms锛岄暱鎸夋椂闂达細2s
*****************************************************************************/
// Forward declarations
extern uint8_t KEY_GetState(uint8_t keyName);
extern uint8_t KEY_GetNumber(void);

void KEY_DrvScan(void)
{
	static	uint8_t	    step[KEY_MAX] = { 0 };						// 鎵?鎻忕姸鎬?
	static	uint16_t	holdingCnt[KEY_MAX] = { 0 };				// 鎸変綇璁℃椂
	static	uint8_t	    filterCnt[KEY_MAX] = { 0 };					// 婊ゆ尝璁℃椂
	static	uint8_t	    pressLast[KEY_MAX] = { 0 };					// 涓婃?℃寜閿?鐘舵??
	uint8_t	i, num, pressCurr;										// 褰撳墠鎸夐敭鐘舵??

	num = KEY_GetNumber();											// 鑾峰彇鎸夐敭鎬讳釜鏁?
	if (num > KEY_MAX)	num = KEY_MAX;								// 鎸夐敭鏁伴噺涓嶈兘瓒呭嚭鑼冨洿
	for (i = 0; i < num; i++) {										// 鎸夐敭閫愪釜鎵?鎻?
		pressCurr = KEY_GetState(KEY_Val_First + i);				// 鑾峰彇鎸夐敭褰撳墠鎸変笅鐘舵??
		switch (step[i]) {											// 鎵?鎻忕姸鎬佸垎涓虹ǔ瀹氱姸鎬佸拰婊ゆ尝鐘舵??
		case 0:														// 绋冲畾鐘舵?侊紙鎸夐敭閲婃斁鎴栨寜浣忕姸鎬侊級
			if (pressCurr != pressLast[i]) {						// 鎸夐敭鐘舵?佹湁鍙樺寲
				pressLast[i] = pressCurr;							// 鎸夐敭鐘舵?佷竴鏃︽敼鍙橈紝璁板綍涓婃?￠敭鍊肩姸鎬?
				filterCnt[i] = 0;									// 寮?濮嬫护娉㈣?℃椂
				step[i] = 1;										// 鎵?鎻忓垏鎹㈠埌婊ゆ尝纭?璁ょ姸鎬?
			}
			else if (pressCurr) {									// 鎸夐敭涓?鐩村?勪簬鎸変綇鐘舵??
				if (holdingCnt[i] != 0xffff) {						// 闃叉?㈣?℃暟婧㈠嚭鍚庨噸澶嶅搷搴?
					holdingCnt[i]++;
					if (holdingCnt[i] == KEY_SCAN_LONG_PRESS_2S) {		// 闀挎寜2S杈惧埌
						KEY_Write((KEY_Val_First + i) + KEY_Evt_Long2S);// 閿?鍊煎啓鍏ョ紦鍐插尯
					}
					if (holdingCnt[i] == KEY_SCAN_LONG_PRESS_4S) {		// 闀挎寜4S杈惧埌
						KEY_Write((KEY_Val_First + i) + KEY_Evt_Long4S);// 閿?鍊煎啓鍏ョ紦鍐插尯
					}
				}
			}
			break;
		case 1:															// 婊ゆ尝鐘舵?侊紙姝ｅ湪娑堟姈涓?锛?
			if (pressCurr == pressLast[i]) {
				if (++filterCnt[i] >= KEY_SCAN_FILTER_TIMES) {			// 濡傛灉N娆＄姸鎬佹湭鍙橈紝璁や负鎸夐敭鐘舵?佺‘瀹炲凡鏀瑰彉
					if (pressCurr) {									// 纭?瀹炲凡鎸変笅
						KEY_Write((KEY_Val_First + i) + KEY_Evt_Press); // 閿?鍊煎啓鍏ョ紦鍐插尯
						holdingCnt[i] = 0;								// 寮?濮嬭?℃椂
					}
					else {												// 纭?瀹炲凡閲婃斁
						KEY_Write((KEY_Val_First + i) + KEY_Evt_Release);// 閿?鍊煎啓鍏ョ紦鍐插尯
					}
					step[i] = 0;										// 杩斿洖绋冲畾鐘舵??
				}
			}
			else {														// 鎶栧姩鏈熼棿锛屼竴鑸?琛ㄧ幇涓烘姈鍔ㄥ拰涓婃?＄姸鎬佷笉鍚岋紝鍒欒?嗕负鎶栧姩
				pressLast[i] = pressCurr;								// 鎸夐敭鐘舵?佷竴鏃︽敼鍙橈紝璁板綍涓婃?￠敭鍊肩姸鎬?
				step[i] = 0;											// 杩斿洖绋冲畾鐘舵??
			}
			break;
		default:
			step[i] = 0;												// 闈炴硶鐘舵?侊紝寮哄埗杩斿洖绋冲畾鐘舵??
			break;
		}
	}
}


/* 鎸夐敭浜嬩欢澶勭悊鍑芥暟瀹氫箟 ***************************************************************************************************************************************/



/*****************************************************************************
* 鍚嶇О:		璇诲彇鎸夐敭褰撳墠鐘舵??
* 杩斿洖:		0=鏈?鎸変笅, >0=鎸変笅
* 璇存槑:		涓嬫媺杈撳叆锛屾寜閿?鎸変笅寮曡剼琚?鎷変綆鍒癎ND=浣庣數骞筹紝鏁呰繑鍥炲?煎彇鍙?
*****************************************************************************/
uint8_t KEY_GetState(uint8_t keyName)
{
	uint8_t	stat = 0;
	switch (keyName)
	{
		case KEY_A:		stat = (GPIO_ReadInputDataBit(macKEY_A_GPIO_PORT, macKEY_A_GPIO_PIN) ? 1 : 0);	break;	// PA8
		case KEY_B:		stat = (GPIO_ReadInputDataBit(macKEY_B_GPIO_PORT, macKEY_B_GPIO_PIN) ? 1 : 0);	break;	// PB3
		case KEY_C:		stat = (GPIO_ReadInputDataBit(macKEY_C_GPIO_PORT, macKEY_C_GPIO_PIN) ? 1 : 0);	break;	// PB4
		case KEY_D:		stat = (GPIO_ReadInputDataBit(macKEY_D_GPIO_PORT, macKEY_D_GPIO_PIN) ? 1 : 0);	break;	// PB8
		case KEY_E:		stat = (GPIO_ReadInputDataBit(macKEY_E_GPIO_PORT, macKEY_E_GPIO_PIN) ? 1 : 0);	break;	// PB1
	}
	return stat;
}

/*****************************************************************************
* 鍚嶇О:		璇诲彇鎸夐敭鎬讳釜鏁?
* 杩斿洖:		鎸夐敭涓?鏁?
*****************************************************************************/
uint8_t KEY_GetNumber(void)
{
	return KEY_NUM;
}




/*****************************************************************************
* 鍚嶇О:		鎸夐敭A鎸変笅澶勭悊鍑芥暟 (KEY1 - PA8)
* 鍔熻兘:		椤甸潰鍒囨崲 涓婚〉鈫掔姸鎬佲啋闃堝?尖啋涓婚〉寰?鐜?
*****************************************************************************/
static void _KEYA_Press(void)
{
	Flag.currentPage++;
	if (Flag.currentPage > 2) {
		Flag.currentPage = 0;
	}
	// 杩涘叆闃堝?奸〉闈㈡椂閲嶇疆閫変腑鍙傛暟绱㈠紩
	if (Flag.currentPage == 2) {
		Flag.thresholdIndex = 0;
	}
	OLED_Clr_Screen();
}


/*****************************************************************************
* 鍚嶇О:		鎸夐敭B鎸変笅澶勭悊鍑芥暟 (KEY2 - PB3)
* 鍔熻兘:		鍦ㄩ槇鍊奸〉闈㈠垏鎹㈤?変腑鍙傛暟锛堝惊鐜? 0-6锛?
*			 闈為槇鍊奸〉闈㈡椂鏃犲姛鑳?
*****************************************************************************/
static void _KEYB_Press(void)
{
	// 子页面切换: 主页面1有3个子页面, 主页面2有4个, 主页面3有4个
	/* 主页面3(手动控制)在自动模式下不切换子页面 */
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
	/* 空气页面(subPage==1)只有上限, 强制选中上限 */
	if (Flag.currentPage == 1 && Flag.subPage == 1) {
		Flag.thresholdField = 1;
	}
	/* 非空气页面恢复默认选中下限 */
	if (Flag.currentPage == 1 && Flag.subPage != 1) {
		Flag.thresholdField = 0;
	}
	OLED_Clr_Screen();
}



static void _KEYC_Press(void)
{
	if (Flag.currentPage == 1) {  // 阈值页面
		if (Flag.thresholdField == 0) {  // 选中下限: KEY3加
			switch (Flag.subPage) {
				case 0: Record.tempLower += THRESHOLD_TEMP_STEP;
					if (Record.tempLower > Record.tempUpper - 1.0f) Record.tempLower = Record.tempUpper - 1.0f; break;
				case 1: break;  // 空气无下限, 跳过
				case 2: Record.waterLevelMin += THRESHOLD_WL_STEP;
					if (Record.waterLevelMin > Record.waterLevelMax - 1) Record.waterLevelMin = Record.waterLevelMax - 1; break;
				case 3: Record.phLower += THRESHOLD_PH_STEP;
					if (Record.phLower > Record.phUpper - 0.1f) Record.phLower = Record.phUpper - 0.1f; break;
			}
		} else {  // 选中上限: KEY3加
			switch (Flag.subPage) {
				case 0: Record.tempUpper += THRESHOLD_TEMP_STEP;
					if (Record.tempUpper > 50.0f) Record.tempUpper = 50.0f; break;
				case 1: Record.airQualityMax += THRESHOLD_AIR_STEP;
					if (Record.airQualityMax > 999) Record.airQualityMax = 999; break;
				case 2: Record.waterLevelMax += THRESHOLD_WL_STEP;
					if (Record.waterLevelMax > 100) Record.waterLevelMax = 100; break;
				case 3: Record.phUpper += THRESHOLD_PH_STEP;
					if (Record.phUpper > 14.0f) Record.phUpper = 14.0f; break;
			}
		}
	}
	if (Flag.currentPage == 2) {  /* 主页面3: 短按切换自动/手动 */
		Record.runMode = !Record.runMode;
		if (Record.runMode == 0) {
			Flag.relayHeat = 0;
			Flag.relayFill = 0;
			Flag.relayDrain = 0;
			Flag.relayOxygen = 0;
		}
	}
}

static void _KEYC_LongPress(void)  // 闀挎寜2s 妯″紡鍒囨崲
{
	Record.runMode = !Record.runMode;  // 鑷?鍔ㄢ噭鎵嬪姩
}


/*****************************************************************************
* 鍚嶇О:		鎸夐敭D鎸変笅澶勭悊鍑芥暟 (KEY4 - PB8)
* 鍔熻兘:		闃堝?奸〉闈㈡暟鍊?-锛岄潪闃堝?奸〉闈㈡棤鍔熻兘
*****************************************************************************/
static void _KEYD_Press(void)
{
	if (Flag.currentPage == 1) {  // 阈值页面
		if (Flag.thresholdField == 0) {  // 选中下限: KEY4减
			switch (Flag.subPage) {
				case 0: Record.tempLower -= THRESHOLD_TEMP_STEP;
					if (Record.tempLower < 0.0f) Record.tempLower = 0.0f; break;
				case 1: break;  // 空气无下限, 跳过
				case 2: if (Record.waterLevelMin >= THRESHOLD_WL_STEP)
						Record.waterLevelMin -= THRESHOLD_WL_STEP;
					else Record.waterLevelMin = 0; break;
				case 3: Record.phLower -= THRESHOLD_PH_STEP;
					if (Record.phLower < 0.0f) Record.phLower = 0.0f; break;
			}
		} else {  // 选中上限: KEY4减
			switch (Flag.subPage) {
				case 0: Record.tempUpper -= THRESHOLD_TEMP_STEP;
					if (Record.tempUpper < Record.tempLower + 1.0f) Record.tempUpper = Record.tempLower + 1.0f; break;
				case 1: if (Record.airQualityMax >= THRESHOLD_AIR_STEP)
						Record.airQualityMax -= THRESHOLD_AIR_STEP;
					else Record.airQualityMax = 0; break;
				case 2: Record.waterLevelMax -= THRESHOLD_WL_STEP;
					if (Record.waterLevelMax < Record.waterLevelMin + 1) Record.waterLevelMax = Record.waterLevelMin + 1; break;
				case 3: Record.phUpper -= THRESHOLD_PH_STEP;
					if (Record.phUpper < Record.phLower + 0.1f) Record.phUpper = Record.phLower + 0.1f; break;
			}
		}
	}
	if (Flag.currentPage == 2 && Record.runMode == 1) {  // 手动控制页面, 手动模式下关继电器
		switch (Flag.subPage) {
			case 0: Flag.relayHeat = !Flag.relayHeat; break;
			case 1: Flag.relayFill = !Flag.relayFill; break;
			case 2: Flag.relayDrain = !Flag.relayDrain; break;
			case 3: Flag.relayOxygen = !Flag.relayOxygen; break;
		}
	}
}

static void _KEYE_Press(void)
{
	/* 在阈值页面, KEY5短按切换 上限/下限 */
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
					case KEY_Evt_Long2S:	_KEYE_LongPress();	break;	
				}
			}break;
		}
	}

}






