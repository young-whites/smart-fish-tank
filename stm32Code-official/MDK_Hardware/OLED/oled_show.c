#include "oled_show.h"
#include "MyTypedef.h"
#include <stdio.h>

extern uint8_t ESP01S_WiFiConnected;

/*****************************************************************************
* 名称:   OLED_ShowNumF6X12_Float
* 功能:   在指定位置显示浮点数
*****************************************************************************/
void OLED_ShowNumF6X12_Float(uint8_t row, uint8_t column, float num, uint8_t decimal)
{
	int intPart, decPart;
	uint8_t col;
	col = column;
	intPart = (int)num;
	if (num < 0) { OLED_Show_OneCharF6X12(row, col, '-'); col += 6; intPart = (int)(-num); }
	if (decimal == 1) {
		decPart = (int)((num - intPart) * 10 + 0.5f);
		if (decPart >= 10) { intPart++; decPart = 0; }
		if (intPart >= 100) {
			OLED_ShowNumF6X12(row, col, 3, (uint32_t)intPart); col += 18;
		} else if (intPart >= 10) {
			OLED_ShowNumF6X12(row, col, 2, (uint32_t)intPart); col += 12;
		} else {
			OLED_ShowNumF6X12(row, col, 1, (uint32_t)intPart); col += 6;
		}
		OLED_Show_OneCharF6X12(row, col, '.'); col += 6;
		OLED_ShowNumF6X12(row, col, 1, (uint32_t)decPart);
	} else {
		decPart = (int)((num - intPart) * 100 + 0.5f);
		if (decPart >= 100) { intPart++; decPart = 0; }
		if (intPart >= 10) {
			OLED_ShowNumF6X12(row, col, 2, (uint32_t)intPart); col += 12;
		} else {
			OLED_ShowNumF6X12(row, col, 1, (uint32_t)intPart); col += 6;
		}
		OLED_Show_OneCharF6X12(row, col, '.'); col += 6;
		OLED_ShowNumF6X12(row, col, 2, (uint32_t)decPart);
	}
}

/* ===== 辅助函数 ===== */
static void _ShowOnOff(uint8_t row, uint8_t col, uint8_t state)
{
	if (state) {
		OLED_Show_OneCharF6X12(row, col, 'O');
		OLED_Show_OneCharF6X12(row, col + 6, 'N');
	} else {
		OLED_Show_OneCharF6X12(row, col, 'O');
		OLED_Show_OneCharF6X12(row, col + 6, 'F');
		OLED_Show_OneCharF6X12(row, col + 12, 'F');
	}
}

static void _ShowWiFi(uint8_t row, uint8_t col)
{
	OLED_Show_OneCharF6X12(row, col, 'W');
	OLED_Show_OneCharF6X12(row, col + 6, ESP01S_WiFiConnected ? '+' : '-');
}

static void _ShowMode(uint8_t row, uint8_t col)
{
	if (Record.runMode == 0) {
		ZoneBitCode_OLED_Show_ChineseF6X12(row, col,      (uint8_t*)"\xd7\xd4", 0);
		ZoneBitCode_OLED_Show_ChineseF6X12(row, col + 12, (uint8_t*)"\xb6\xaf", 0);
	} else {
		ZoneBitCode_OLED_Show_ChineseF6X12(row, col,      (uint8_t*)"\xca\xd6", 0);
		ZoneBitCode_OLED_Show_ChineseF6X12(row, col + 12, (uint8_t*)"\xb6\xaf", 0);
	}
}

static void _ShowFeedStatus(uint8_t row, uint8_t col)
{
	ZoneBitCode_OLED_Show_ChineseF6X12(row, col,      (uint8_t*)"\xce\xb9", 0);
	ZoneBitCode_OLED_Show_ChineseF6X12(row, col + 12, (uint8_t*)"\xca\xb3", 0);
	OLED_Show_OneCharF6X12(row, col + 24, ':');
	if (Flag.feeding) {
		OLED_ShowNumF6X12(row, col + 30, 4, (uint32_t)Record.feedCountdown);
		OLED_Show_OneCharF6X12(row, col + 54, 's');
	} else {
		OLED_Show_OneCharF6X12(row, col + 30, '-');
		OLED_Show_OneCharF6X12(row, col + 36, '-');
	}
}

/* 页码指示器 */
static void _ShowPageIndicator(void)
{
	uint8_t mainP = Flag.currentPage + 1;
	uint8_t subP = Flag.subPage + 1;
	OLED_ShowNumF6X12(0, 102, 1, (uint32_t)mainP);
	OLED_Show_OneCharF6X12(0, 108, '/');
	OLED_ShowNumF6X12(0, 114, 1, (uint32_t)subP);
}

/* ================================================================
 *  启动画面
 * ================================================================ */
void OLED_ShowStart(void)
{
	uint8_t col;
	col = 30;
	OLED_Show_OneCharF6X12(2, col, 'S'); col+=6;
	OLED_Show_OneCharF6X12(2, col, 'm'); col+=6;
	OLED_Show_OneCharF6X12(2, col, 'a'); col+=6;
	OLED_Show_OneCharF6X12(2, col, 'r'); col+=6;
	OLED_Show_OneCharF6X12(2, col, 't'); col+=6;
	OLED_Show_OneCharF6X12(2, col, ' '); col+=6;
	OLED_Show_OneCharF6X12(2, col, 'T'); col+=6;
	OLED_Show_OneCharF6X12(2, col, 'a'); col+=6;
	OLED_Show_OneCharF6X12(2, col, 'n'); col+=6;
	OLED_Show_OneCharF6X12(2, col, 'k'); col+=6;

	col = 27;
	OLED_Show_OneCharF6X12(4, col, 'S'); col+=6;
	OLED_Show_OneCharF6X12(4, col, 'y'); col+=6;
	OLED_Show_OneCharF6X12(4, col, 's'); col+=6;
	OLED_Show_OneCharF6X12(4, col, 't'); col+=6;
	OLED_Show_OneCharF6X12(4, col, 'e'); col+=6;
	OLED_Show_OneCharF6X12(4, col, 'm'); col+=6;
	OLED_Show_OneCharF6X12(4, col, ' '); col+=6;
	OLED_Show_OneCharF6X12(4, col, 'v'); col+=6;
	OLED_Show_OneCharF6X12(4, col, '1'); col+=6;
	OLED_Show_OneCharF6X12(4, col, '.'); col+=6;
	OLED_Show_OneCharF6X12(4, col, '0'); col+=6;

	col = 30;
	OLED_Show_OneCharF6X12(6, col, '2'); col+=6;
	OLED_Show_OneCharF6X12(6, col, '0'); col+=6;
	OLED_Show_OneCharF6X12(6, col, '2'); col+=6;
	OLED_Show_OneCharF6X12(6, col, '6'); col+=6;
	OLED_Show_OneCharF6X12(6, col, '-'); col+=6;
	OLED_Show_OneCharF6X12(6, col, '0'); col+=6;
	OLED_Show_OneCharF6X12(6, col, '5'); col+=6;
	OLED_Show_OneCharF6X12(6, col, '-'); col+=6;
	OLED_Show_OneCharF6X12(6, col, '0'); col+=6;
	OLED_Show_OneCharF6X12(6, col, '4'); col+=6;
}

/* ================================================================
 *  主页面 1 - 数据显示
 * ================================================================ */
static void _Main_Sub0(void)
{
	_ShowPageIndicator();
	/* row2: 水温 */
	ZoneBitCode_OLED_Show_ChineseF6X12(2, 0,  (uint8_t*)"\xcb\xae", 0);
	ZoneBitCode_OLED_Show_ChineseF6X12(2, 12, (uint8_t*)"\xce\xc2", 0);
	OLED_Show_OneCharF6X12(2, 24, ':');
	OLED_ShowNumF6X12_Float(2, 30, Record.waterTemp, 1);
	OLED_Show_OneCharF6X12(2, 54, 'C');
	/* row2右: PH */
	OLED_Show_OneCharF6X12(2, 72, 'P');
	OLED_Show_OneCharF6X12(2, 78, 'H');
	OLED_Show_OneCharF6X12(2, 84, ':');
	OLED_ShowNumF6X12_Float(2, 90, Record.phValue, 1);
	/* row6: 状态栏 */
	_ShowWiFi(6, 0);
	_ShowMode(6, 30);
	_ShowFeedStatus(6, 66);
}

static void _Main_Sub1(void)
{
	_ShowPageIndicator();
	/* row2: 水位 */
	ZoneBitCode_OLED_Show_ChineseF6X12(2, 0,  (uint8_t*)"\xcb\xae", 0);
	ZoneBitCode_OLED_Show_ChineseF6X12(2, 12, (uint8_t*)"\xce\xbb", 0);
	OLED_Show_OneCharF6X12(2, 24, ':');
	OLED_ShowNumF6X12(2, 30, 3, (uint32_t)Record.waterLevel);
	OLED_Show_OneCharF6X12(2, 48, '%');
	/* row2右: 空气 */
	OLED_Show_OneCharF6X12(2, 66, 'A');
	OLED_Show_OneCharF6X12(2, 72, 'Q');
	OLED_Show_OneCharF6X12(2, 78, 'I');
	OLED_Show_OneCharF6X12(2, 84, ':');
	OLED_ShowNumF6X12(2, 90, 2, (uint32_t)Record.airQuality);
	/* row6: 加热+加水 */
	ZoneBitCode_OLED_Show_ChineseF6X12(6, 0,  (uint8_t*)"\xbc\xd3", 0);
	ZoneBitCode_OLED_Show_ChineseF6X12(6, 12, (uint8_t*)"\xc8\xc8", 0);
	OLED_Show_OneCharF6X12(6, 24, ':');
	_ShowOnOff(6, 30, Flag.relayHeat);
	ZoneBitCode_OLED_Show_ChineseF6X12(6, 60,  (uint8_t*)"\xbc\xd3", 0);
	ZoneBitCode_OLED_Show_ChineseF6X12(6, 72,  (uint8_t*)"\xcb\xae", 0);
	OLED_Show_OneCharF6X12(6, 84, ':');
	_ShowOnOff(6, 90, Flag.relayFill);
}

static void _Main_Sub2(void)
{
	_ShowPageIndicator();
	/* row2: 排水+增氧 */
	ZoneBitCode_OLED_Show_ChineseF6X12(2, 0,  (uint8_t*)"\xc5\xc5", 0);
	ZoneBitCode_OLED_Show_ChineseF6X12(2, 12, (uint8_t*)"\xcb\xae", 0);
	OLED_Show_OneCharF6X12(2, 24, ':');
	_ShowOnOff(2, 30, Flag.relayDrain);
	ZoneBitCode_OLED_Show_ChineseF6X12(2, 60,  (uint8_t*)"\xd4\xf6", 0);
	ZoneBitCode_OLED_Show_ChineseF6X12(2, 72,  (uint8_t*)"\xd1\xf5", 0);
	OLED_Show_OneCharF6X12(2, 84, ':');
	_ShowOnOff(2, 90, Flag.relayOxygen);
	/* row6: ALM+模式 */
	OLED_Show_OneCharF6X12(6, 0,  'A');
	OLED_Show_OneCharF6X12(6, 6,  'L');
	OLED_Show_OneCharF6X12(6, 12, 'M');
	OLED_Show_OneCharF6X12(6, 18, ':');
	_ShowOnOff(6, 24, Flag.alarmEnable);
	ZoneBitCode_OLED_Show_ChineseF6X12(6, 60,  (uint8_t*)"\xc4\xa3", 0);
	ZoneBitCode_OLED_Show_ChineseF6X12(6, 72,  (uint8_t*)"\xca\xbd", 0);
	OLED_Show_OneCharF6X12(6, 84, ':');
	_ShowMode(6, 90);
}

/* ================================================================
 *  主页面 2 - 阈值设置 (上下限各占一行, KEY5切换选中)
 * ================================================================ */

/* 子2-1: 温度 */
static void _Threshold_Sub0(void)
{
	_ShowPageIndicator();
	/* row2: 总览 */
	ZoneBitCode_OLED_Show_ChineseF6X12(2, 0,  (uint8_t*)"\xce\xc2", 0);
	ZoneBitCode_OLED_Show_ChineseF6X12(2, 12, (uint8_t*)"\xb6\xc8", 0);
	OLED_Show_OneCharF6X12(2, 24, ':');
	OLED_ShowNumF6X12_Float(2, 30, Record.tempLower, 1);
	OLED_Show_OneCharF6X12(2, 54, '~');
	OLED_ShowNumF6X12_Float(2, 60, Record.tempUpper, 1);

	/* row4: 下限 - 光标 */
	OLED_Show_OneCharF6X12(4, 0, (Flag.thresholdField == 0) ? '>' : ' ');
	ZoneBitCode_OLED_Show_ChineseF6X12(4, 6,  (uint8_t*)"\xcf\xc2", 0);
	ZoneBitCode_OLED_Show_ChineseF6X12(4, 18, (uint8_t*)"\xcf\xde", 0);
	OLED_Show_OneCharF6X12(4, 30, ':');
	OLED_ShowNumF6X12_Float(4, 36, Record.tempLower, 1);

	/* row6: 上限 - 光标 */
	OLED_Show_OneCharF6X12(6, 0, (Flag.thresholdField == 1) ? '>' : ' ');
	ZoneBitCode_OLED_Show_ChineseF6X12(6, 6,  (uint8_t*)"\xc9\xcf", 0);
	ZoneBitCode_OLED_Show_ChineseF6X12(6, 18, (uint8_t*)"\xcf\xde", 0);
	OLED_Show_OneCharF6X12(6, 30, ':');
	OLED_ShowNumF6X12_Float(6, 36, Record.tempUpper, 1);
	OLED_Show_OneCharF6X12(6, 60, 'C');

	OLED_Show_OneCharF6X12(6, 78, 'K');
	OLED_Show_OneCharF6X12(6, 84, '5');
	OLED_Show_OneCharF6X12(6, 90, ':');
	OLED_Show_OneCharF6X12(6, 96, 'S');
	OLED_Show_OneCharF6X12(6, 102, 'w');
}

/* 子2-2: 空气 (只有上限) */
static void _Threshold_Sub1(void)
{
	_ShowPageIndicator();
	OLED_Show_OneCharF6X12(2, 0, 'A');
	OLED_Show_OneCharF6X12(2, 6, 'Q');
	OLED_Show_OneCharF6X12(2, 12, 'I');
	OLED_Show_OneCharF6X12(2, 18, ':');
	OLED_ShowNumF6X12(2, 24, 2, (uint32_t)Record.airQualityMax);

	OLED_Show_OneCharF6X12(4, 0, '>');
	ZoneBitCode_OLED_Show_ChineseF6X12(4, 6,  (uint8_t*)"\xc9\xcf", 0);
	ZoneBitCode_OLED_Show_ChineseF6X12(4, 18, (uint8_t*)"\xcf\xde", 0);
	OLED_Show_OneCharF6X12(4, 30, ':');
	OLED_Show_OneCharF6X12(4, 36, '<');
	OLED_ShowNumF6X12(4, 42, 4, (uint32_t)Record.airQualityMax);

	OLED_Show_OneCharF6X12(6, 0,  'K');
	OLED_Show_OneCharF6X12(6, 6,  '3');
	OLED_Show_OneCharF6X12(6, 12, '+');
	OLED_Show_OneCharF6X12(6, 18, '1');
	OLED_Show_OneCharF6X12(6, 24, ' ');
	OLED_Show_OneCharF6X12(6, 36, 'K');
	OLED_Show_OneCharF6X12(6, 42, '4');
	OLED_Show_OneCharF6X12(6, 48, '-');
	OLED_Show_OneCharF6X12(6, 54, '1');
	OLED_Show_OneCharF6X12(6, 60, '0');
}

/* 子2-3: 水位 */
static void _Threshold_Sub2(void)
{
	_ShowPageIndicator();
	ZoneBitCode_OLED_Show_ChineseF6X12(2, 0,  (uint8_t*)"\xcb\xae", 0);
	ZoneBitCode_OLED_Show_ChineseF6X12(2, 12, (uint8_t*)"\xce\xbb", 0);
	OLED_Show_OneCharF6X12(2, 24, ':');
	OLED_ShowNumF6X12(2, 30, 3, (uint32_t)Record.waterLevelMin);
	OLED_Show_OneCharF6X12(2, 48, '~');
	OLED_ShowNumF6X12(2, 54, 3, (uint32_t)Record.waterLevelMax);
	OLED_Show_OneCharF6X12(2, 72, '%');

	OLED_Show_OneCharF6X12(4, 0, (Flag.thresholdField == 0) ? '>' : ' ');
	ZoneBitCode_OLED_Show_ChineseF6X12(4, 6,  (uint8_t*)"\xcf\xc2", 0);
	ZoneBitCode_OLED_Show_ChineseF6X12(4, 18, (uint8_t*)"\xcf\xde", 0);
	OLED_Show_OneCharF6X12(4, 30, ':');
	OLED_ShowNumF6X12(4, 36, 3, (uint32_t)Record.waterLevelMin);
	OLED_Show_OneCharF6X12(4, 54, '%');

	OLED_Show_OneCharF6X12(6, 0, (Flag.thresholdField == 1) ? '>' : ' ');
	ZoneBitCode_OLED_Show_ChineseF6X12(6, 6,  (uint8_t*)"\xc9\xcf", 0);
	ZoneBitCode_OLED_Show_ChineseF6X12(6, 18, (uint8_t*)"\xcf\xde", 0);
	OLED_Show_OneCharF6X12(6, 30, ':');
	OLED_ShowNumF6X12(6, 36, 3, (uint32_t)Record.waterLevelMax);
	OLED_Show_OneCharF6X12(6, 54, '%');

	OLED_Show_OneCharF6X12(6, 78, 'K');
	OLED_Show_OneCharF6X12(6, 84, '5');
	OLED_Show_OneCharF6X12(6, 90, ':');
	OLED_Show_OneCharF6X12(6, 96, 'S');
	OLED_Show_OneCharF6X12(6, 102, 'w');
}

/* 子2-4: PH */
static void _Threshold_Sub3(void)
{
	_ShowPageIndicator();
	OLED_Show_OneCharF6X12(2, 0,  'P');
	OLED_Show_OneCharF6X12(2, 6,  'H');
	OLED_Show_OneCharF6X12(2, 12, ':');
	OLED_ShowNumF6X12_Float(2, 18, Record.phLower, 1);
	OLED_Show_OneCharF6X12(2, 42, '~');
	OLED_ShowNumF6X12_Float(2, 48, Record.phUpper, 1);

	OLED_Show_OneCharF6X12(4, 0, (Flag.thresholdField == 0) ? '>' : ' ');
	ZoneBitCode_OLED_Show_ChineseF6X12(4, 6,  (uint8_t*)"\xcf\xc2", 0);
	ZoneBitCode_OLED_Show_ChineseF6X12(4, 18, (uint8_t*)"\xcf\xde", 0);
	OLED_Show_OneCharF6X12(4, 30, ':');
	OLED_ShowNumF6X12_Float(4, 36, Record.phLower, 1);

	OLED_Show_OneCharF6X12(6, 0, (Flag.thresholdField == 1) ? '>' : ' ');
	ZoneBitCode_OLED_Show_ChineseF6X12(6, 6,  (uint8_t*)"\xc9\xcf", 0);
	ZoneBitCode_OLED_Show_ChineseF6X12(6, 18, (uint8_t*)"\xcf\xde", 0);
	OLED_Show_OneCharF6X12(6, 30, ':');
	OLED_ShowNumF6X12_Float(6, 36, Record.phUpper, 1);

	OLED_Show_OneCharF6X12(6, 78, 'K');
	OLED_Show_OneCharF6X12(6, 84, '5');
	OLED_Show_OneCharF6X12(6, 90, ':');
	OLED_Show_OneCharF6X12(6, 96, 'S');
	OLED_Show_OneCharF6X12(6, 102, 'w');
}

/* ================================================================
 *  主页面 3 - 手动控制
 * ================================================================ */
static void _ShowClearRow(uint8_t row)
{
	uint8_t i;
	for (i = 0; i < 21; i++) {
		OLED_Show_OneCharF6X12(row, i * 6, ' ');
	}
}

static void _Manual_Sub(const char* name_gbk, uint8_t* relayState)
{
	static uint8_t lastMode = 0xFF;

	_ShowPageIndicator();
	if (Flag.feeding) {
		ZoneBitCode_OLED_Show_ChineseF6X12(2, 0,  (uint8_t*)"\xce\xb9", 0);
		ZoneBitCode_OLED_Show_ChineseF6X12(2, 12, (uint8_t*)"\xca\xb3", 0);
		OLED_Show_OneCharF6X12(2, 24, ':');
		OLED_ShowNumF6X12(2, 30, 3, (uint32_t)Record.feedCountdown);
		OLED_Show_OneCharF6X12(2, 48, 's');
	} else {
		ZoneBitCode_OLED_Show_ChineseF6X12(2, 0,  (uint8_t*)"\xca\xd6", 0);
		ZoneBitCode_OLED_Show_ChineseF6X12(2, 12, (uint8_t*)"\xb6\xaf", 0);
		ZoneBitCode_OLED_Show_ChineseF6X12(2, 24, (uint8_t*)"\xbf\xd8", 0);
		ZoneBitCode_OLED_Show_ChineseF6X12(2, 36, (uint8_t*)"\xd6\xc6", 0);
	}

	/* 模式变化时清除 row4 和 row6 */
	if (lastMode != Record.runMode) {
		lastMode = Record.runMode;
		_ShowClearRow(4);
		_ShowClearRow(6);
	}

	if (Record.runMode == 0) {
		/* 自动模式: 显示"自动模式!"，所有子页面统一显示 */
		ZoneBitCode_OLED_Show_ChineseF6X12(4, 6,  (uint8_t*)"\xd7\xd4", 0);
		ZoneBitCode_OLED_Show_ChineseF6X12(4, 18, (uint8_t*)"\xb6\xaf", 0);
		ZoneBitCode_OLED_Show_ChineseF6X12(4, 30, (uint8_t*)"\xc4\xa3", 0);
		ZoneBitCode_OLED_Show_ChineseF6X12(4, 42, (uint8_t*)"\xca\xbd", 0);
		OLED_Show_OneCharF6X12(4, 54, '!');
		/* row6: 提示按键 */
		OLED_Show_OneCharF6X12(6, 0,  'K');
		OLED_Show_OneCharF6X12(6, 6,  '3');
		OLED_Show_OneCharF6X12(6, 12, ':');
		ZoneBitCode_OLED_Show_ChineseF6X12(6, 18, (uint8_t*)"\xca\xd6", 0);
		ZoneBitCode_OLED_Show_ChineseF6X12(6, 30, (uint8_t*)"\xb6\xaf", 0);
		return;
	}

	/* 手动模式: 显示继电器控制 */
	OLED_Show_OneCharF6X12(4, 0, '>');
	ZoneBitCode_OLED_Show_ChineseF6X12(4, 6,  (uint8_t*)name_gbk, 0);
	ZoneBitCode_OLED_Show_ChineseF6X12(4, 18, (uint8_t*)(name_gbk + 2), 0);
	OLED_Show_OneCharF6X12(4, 30, ':');
	if (*relayState) {
		OLED_Show_OneCharF6X12(4, 36, 'O');
		OLED_Show_OneCharF6X12(4, 42, 'N');
		OLED_Show_OneCharF6X12(4, 48, ' ');
	} else {
		OLED_Show_OneCharF6X12(4, 36, 'O');
		OLED_Show_OneCharF6X12(4, 42, 'F');
		OLED_Show_OneCharF6X12(4, 48, 'F');
	}

	/* row6: 操作提示 */
	OLED_Show_OneCharF6X12(6, 0,  'K');
	OLED_Show_OneCharF6X12(6, 6,  '3');
	OLED_Show_OneCharF6X12(6, 12, ':');
	ZoneBitCode_OLED_Show_ChineseF6X12(6, 18, (uint8_t*)"\xc4\xa3", 0);
	ZoneBitCode_OLED_Show_ChineseF6X12(6, 30, (uint8_t*)"\xca\xbd", 0);
	OLED_Show_OneCharF6X12(6, 48, 'K');
	OLED_Show_OneCharF6X12(6, 54, '4');
	OLED_Show_OneCharF6X12(6, 60, ':');
	OLED_Show_OneCharF6X12(6, 66, '+');
	OLED_Show_OneCharF6X12(6, 72, '/');
	OLED_Show_OneCharF6X12(6, 78, '-');
	if (Flag.relayHeat || Flag.relayFill || Flag.relayDrain || Flag.relayOxygen) {
		OLED_ShowNumF6X12(6, 90, 2, (uint32_t)Flag.manualTimeout);
		OLED_Show_OneCharF6X12(6, 102, 's');
	}
}

/* ================================================================
 *  主调度函数
 * ================================================================ */
void OLED_Show_Page(uint8_t page)
{
	uint8_t sub = Flag.subPage;

	switch (page) {
		case 0:
			switch (sub) {
				case 0: _Main_Sub0(); break;
				case 1: _Main_Sub1(); break;
				case 2: _Main_Sub2(); break;
				default: _Main_Sub0(); break;
			}
			break;

		case 1:
			switch (sub) {
				case 0: _Threshold_Sub0(); break;
				case 1: _Threshold_Sub1(); break;
				case 2: _Threshold_Sub2(); break;
				case 3: _Threshold_Sub3(); break;
				default: _Threshold_Sub0(); break;
			}
			break;

		case 2:
			switch (sub) {
				case 0: _Manual_Sub("\xbc\xd3\xc8\xc8", &Flag.relayHeat); break;
				case 1: _Manual_Sub("\xbc\xd3\xcb\xae", &Flag.relayFill); break;
				case 2: _Manual_Sub("\xc5\xc5\xcb\xae", &Flag.relayDrain); break;
				case 3: _Manual_Sub("\xd4\xf6\xd1\xf5", &Flag.relayOxygen); break;
				default: _Manual_Sub("\xbc\xd3\xc8\xc8", &Flag.relayHeat); break;
			}
			break;

		default:
			_Main_Sub0();
			break;
	}
}
