#include "bsp_Key.h"

/* 全局页面状态变量定义在 main.c 中, 此处仅声明 */
extern uint8_t CurrentPage;
extern uint8_t AutoSet_Index;
extern uint8_t ManualSet_Index;
extern uint8_t Feed_Trigger;

extern FishTank_DataStruct  FishTank_Data;
extern FishTank_FlagStruct  FishTank_Flag;
extern FishTank_SetStruct   FishTank_Set;
extern FishTank_FeedStruct  FishTank_Feed;


void KEY_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* KEY_A - PA8 */
	macKEY_A_GPIO_APBxClock_FUN(macKEY_A_GPIO_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = macKEY_A_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(macKEY_A_GPIO_PORT, &GPIO_InitStructure);

	/* KEY_B - PB3 */
	macKEY_B_GPIO_APBxClock_FUN(macKEY_B_GPIO_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = macKEY_B_GPIO_PIN;
	GPIO_Init(macKEY_B_GPIO_PORT, &GPIO_InitStructure);

	/* KEY_C - PB4 */
	macKEY_C_GPIO_APBxClock_FUN(macKEY_C_GPIO_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = macKEY_C_GPIO_PIN;
	GPIO_Init(macKEY_C_GPIO_PORT, &GPIO_InitStructure);

	/* KEY_D - PB8 */
	macKEY_D_GPIO_APBxClock_FUN(macKEY_D_GPIO_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = macKEY_D_GPIO_PIN;
	GPIO_Init(macKEY_D_GPIO_PORT, &GPIO_InitStructure);

	/* KEY_E - PB1 */
	macKEY_E_GPIO_APBxClock_FUN(macKEY_E_GPIO_CLK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = macKEY_E_GPIO_PIN;
	GPIO_Init(macKEY_E_GPIO_PORT, &GPIO_InitStructure);
}



/* 以下是按键驱动内部使用 *****************************************************************/

extern uint8_t  KEY_GetState(uint8_t keyName);
extern uint8_t  KEY_GetNumber(void);
//----------------------------------------------------------------------------
#define		KEY_MAX						(6)
//----------------------------------------------------------------------------
#define		KEY_VAL_BUF_SIZE			(KEY_MAX+3)
#define		KEY_SCAN_FILTER_TIMES		(5)
#define		KEY_SCAN_LONG_PRESS_2S		(200)
#define		KEY_SCAN_LONG_PRESS_4S		(400)
//----------------------------------------------------------------------------
#define		KEY_Val_First				(0x01)
//----------------------------------------------------------------------------
static	uint8_t	_keyValBuf[KEY_VAL_BUF_SIZE];
static	uint8_t	_keyValBufR = 0;
static	uint8_t	_keyValBufW = 0;
static	uint8_t	_keyValBufCnt = 0;
//----------------------------------------------------------------------------

void KEY_Write(uint8_t value)
{
	if (_keyValBufCnt < KEY_VAL_BUF_SIZE) {
		_keyValBufCnt++;
		_keyValBuf[_keyValBufW] = value;
		if (++_keyValBufW >= KEY_VAL_BUF_SIZE) {
			_keyValBufW = 0;
		}
	}
}

uint8_t KEY_Read(void)
{
	uint8_t	value = 0;
	if (_keyValBufCnt) {
		_keyValBufCnt--;
		value = _keyValBuf[_keyValBufR];
		if (++_keyValBufR >= KEY_VAL_BUF_SIZE) {
			_keyValBufR = 0;
		}
	}
	return value;
}

void KEY_DrvScan(void)
{
	static	uint8_t	    step[KEY_MAX] = { 0 };
	static	uint16_t	holdingCnt[KEY_MAX] = { 0 };
	static	uint8_t	    filterCnt[KEY_MAX] = { 0 };
	static	uint8_t	    pressLast[KEY_MAX] = { 0 };
	uint8_t	i, num, pressCurr;

	num = KEY_GetNumber();
	if (num > KEY_MAX)	num = KEY_MAX;
	for (i = 0; i < num; i++) {
		pressCurr = KEY_GetState(KEY_Val_First + i);
		switch (step[i]) {
		case 0:
			if (pressCurr != pressLast[i]) {
				pressLast[i] = pressCurr;
				filterCnt[i] = 0;
				step[i] = 1;
			}
			else if (pressCurr) {
				if (holdingCnt[i] != 0xffff) {
					holdingCnt[i]++;
					if (holdingCnt[i] == KEY_SCAN_LONG_PRESS_2S) {
						KEY_Write((KEY_Val_First + i) + KEY_Evt_Long2S);
					}
					if (holdingCnt[i] == KEY_SCAN_LONG_PRESS_4S) {
						KEY_Write((KEY_Val_First + i) + KEY_Evt_Long4S);
					}
				}
			}
			break;
		case 1:
			if (pressCurr == pressLast[i]) {
				if (++filterCnt[i] >= KEY_SCAN_FILTER_TIMES) {
					if (pressCurr) {
						KEY_Write((KEY_Val_First + i) + KEY_Evt_Press);
						holdingCnt[i] = 0;
					}
					else {
						KEY_Write((KEY_Val_First + i) + KEY_Evt_Release);
					}
					step[i] = 0;
				}
			}
			else {
				pressLast[i] = pressCurr;
				step[i] = 0;
			}
			break;
		default:
			step[i] = 0;
			break;
		}
	}
}


/* 按键状态获取 *****************************************************************/

uint8_t KEY_GetState(uint8_t keyName)
{
	uint8_t	stat = 0;
	switch (keyName)
	{
		case KEY_A:		stat = (GPIO_ReadInputDataBit(macKEY_A_GPIO_PORT, macKEY_A_GPIO_PIN) ? 1 : 0);		break;
		case KEY_B:		stat = (GPIO_ReadInputDataBit(macKEY_B_GPIO_PORT, macKEY_B_GPIO_PIN) ? 1 : 0);		break;
		case KEY_C:		stat = (GPIO_ReadInputDataBit(macKEY_C_GPIO_PORT, macKEY_C_GPIO_PIN) ? 1 : 0);		break;
		case KEY_D:		stat = (GPIO_ReadInputDataBit(macKEY_D_GPIO_PORT, macKEY_D_GPIO_PIN) ? 1 : 0);		break;
		case KEY_E:		stat = (GPIO_ReadInputDataBit(macKEY_E_GPIO_PORT, macKEY_E_GPIO_PIN) ? 1 : 0);		break;
	}
	return stat;
}

uint8_t KEY_GetNumber(void)
{
	return KEY_NUM;
}


/* 按键业务逻辑 *****************************************************************/

/*
 * 按键映射:
 * KEY1(PA8) 短按: 设置页返回主页
 * KEY2(PB3) 短按: 页面循环切换 0→1→2→0
 * KEY3(PB4) 短按: 主页切模式/设置页加1/手动页开关
 * KEY4(PB8) 短按: 设置页减1
 * KEY5(PB1) 长按2秒: 主页触发喂食
 */
void KEY_Scan(void)
{
	uint8_t	key, event;
	for (key = KEY_Read(); key; key = KEY_Read())
	{
		event = key & 0xf0;
		key   = key & 0x0f;
		switch (key)
		{
			case KEY_A:  /* 返回键 */
			{
				switch (event)
				{
					case KEY_Evt_Press:
						if (CurrentPage == 1 || CurrentPage == 2)
						{
							CurrentPage = 0;
							AutoSet_Index = 0;
							ManualSet_Index = 0;
						}
						break;
				}
			}break;

			case KEY_B:  /* 页面循环切换 */
			{
				switch (event)
				{
					case KEY_Evt_Press:
						CurrentPage = (CurrentPage + 1) % 3;
						AutoSet_Index = 0;
						ManualSet_Index = 0;
						break;
				}
			}break;

			case KEY_C:  /* 加/模式切换/开关 */
			{
				switch (event)
				{
					case KEY_Evt_Press:
						if (CurrentPage == 0)  /* 主页面: 切换手/自动模式 */
						{
							if (FishTank_Flag.RunMode == 0)
							{
								/* 自动→手动: 关闭所有继电器 */
								Relay_AllOff();
								FishTank_Flag.Relay_Drain_Manual  = 0;
								FishTank_Flag.Relay_Oxygen_Manual = 0;
								FishTank_Flag.Relay_Fill_Manual   = 0;
								FishTank_Flag.Relay_Heat_Manual   = 0;
								FishTank_Flag.RunMode = 1;
							}
							else
							{
								/* 手动→自动: 关闭所有继电器后由自动逻辑接管 */
								Relay_AllOff();
								FishTank_Flag.Relay_Drain_Manual  = 0;
								FishTank_Flag.Relay_Oxygen_Manual = 0;
								FishTank_Flag.Relay_Fill_Manual   = 0;
								FishTank_Flag.Relay_Heat_Manual   = 0;
								FishTank_Flag.RunMode = 0;
							}
						}
						else if (CurrentPage == 1)  /* 设置页: 当前项加1 */
						{
							switch (AutoSet_Index)
							{
								case 0: FishTank_Set.TempLower     += 1.0f; if (FishTank_Set.TempLower > 40.0f) FishTank_Set.TempLower = 40.0f; break;
								case 1: FishTank_Set.TempUpper     += 1.0f; if (FishTank_Set.TempUpper > 40.0f) FishTank_Set.TempUpper = 40.0f; break;
								case 2: FishTank_Set.AirQuality_Max += 10; if (FishTank_Set.AirQuality_Max > 1000) FishTank_Set.AirQuality_Max = 1000; break;
								case 3: FishTank_Set.PH_Lower      += 0.5f; if (FishTank_Set.PH_Lower > 14.0f) FishTank_Set.PH_Lower = 14.0f; break;
								case 4: FishTank_Set.PH_Upper      += 0.5f; if (FishTank_Set.PH_Upper > 14.0f) FishTank_Set.PH_Upper = 14.0f; break;
								case 5: FishTank_Set.WaterLevel_Min += 5; if (FishTank_Set.WaterLevel_Min > 100) FishTank_Set.WaterLevel_Min = 100; break;
								case 6: FishTank_Set.FeedInterval   += 60; if (FishTank_Set.FeedInterval > 86400) FishTank_Set.FeedInterval = 86400; break;
							}
						}
						else if (CurrentPage == 2)  /* 手动页: 切换当前设备开/关 */
						{
							switch (ManualSet_Index)
							{
								case 0: /* 加热 */
									FishTank_Flag.Relay_Heat_Manual = !FishTank_Flag.Relay_Heat_Manual;
									Relay_Control(RELAY_HEAT, FishTank_Flag.Relay_Heat_Manual);
									break;
								case 1: /* 加水 */
									FishTank_Flag.Relay_Fill_Manual = !FishTank_Flag.Relay_Fill_Manual;
									Relay_Control(RELAY_FILL, FishTank_Flag.Relay_Fill_Manual);
									break;
								case 2: /* 排水 */
									FishTank_Flag.Relay_Drain_Manual = !FishTank_Flag.Relay_Drain_Manual;
									Relay_Control(RELAY_DRAIN, FishTank_Flag.Relay_Drain_Manual);
									break;
								case 3: /* 增氧 */
									FishTank_Flag.Relay_Oxygen_Manual = !FishTank_Flag.Relay_Oxygen_Manual;
									Relay_Control(RELAY_OXYGEN, FishTank_Flag.Relay_Oxygen_Manual);
									break;
								case 4: /* 报警开关 */
									FishTank_Flag.Alarm_Enable = !FishTank_Flag.Alarm_Enable;
									break;
								case 5: /* 手动喂食 */
									if (!FishTank_Feed.Feeding)
									{
										Feed_Trigger = 1;
									}
									break;
							}
						}
						break;
				}
			}break;

			case KEY_D:  /* 减 */
			{
				switch (event)
				{
					case KEY_Evt_Press:
						if (CurrentPage == 1)  /* 设置页: 当前项减1 */
						{
							switch (AutoSet_Index)
							{
								case 0: FishTank_Set.TempLower     -= 1.0f; if (FishTank_Set.TempLower < 0.0f) FishTank_Set.TempLower = 0.0f; break;
								case 1: FishTank_Set.TempUpper     -= 1.0f; if (FishTank_Set.TempUpper < 0.0f) FishTank_Set.TempUpper = 0.0f; break;
								case 2: if (FishTank_Set.AirQuality_Max >= 10) FishTank_Set.AirQuality_Max -= 10; break;
								case 3: FishTank_Set.PH_Lower      -= 0.5f; if (FishTank_Set.PH_Lower < 0.0f) FishTank_Set.PH_Lower = 0.0f; break;
								case 4: FishTank_Set.PH_Upper      -= 0.5f; if (FishTank_Set.PH_Upper < 0.0f) FishTank_Set.PH_Upper = 0.0f; break;
								case 5: if (FishTank_Set.WaterLevel_Min >= 5) FishTank_Set.WaterLevel_Min -= 5; break;
								case 6: if (FishTank_Set.FeedInterval >= 120) FishTank_Set.FeedInterval -= 60; else FishTank_Set.FeedInterval = 10; break;
							}
						}
						else if (CurrentPage == 1)
						{
							/* 减操作后, 设置页选中项循环上移 */
							if (AutoSet_Index > 0) AutoSet_Index--;
							else AutoSet_Index = 6;
						}
						break;
					case KEY_Evt_Long2S:
						/* 设置页长按减: 选中项循环下移 */
						if (CurrentPage == 1)
						{
							AutoSet_Index = (AutoSet_Index + 1) % 7;
						}
						break;
				}
			}break;

			case KEY_E:  /* 手动触发喂食 - 长按2秒 */
			{
				switch (event)
				{
					case KEY_Evt_Long2S:
						if (CurrentPage == 0)
						{
							Feed_Trigger = 1;
						}
						break;
				}
			}break;
		}
	}

	/* KEY2 短按时已做页面切换, 设置页/手动页的选中项由 KEY2 短按重置 */
	/* KEY3 短按时设置页选中项循环下移（加操作后移到下一个）*/
	if (CurrentPage == 1)
	{
		/* 由 KEY_C 短按触发加操作后自动移到下一项 */
		/* 此逻辑在 KEY_C 事件中不做自动移动，通过 KEY2 循环选中 */
	}
}
