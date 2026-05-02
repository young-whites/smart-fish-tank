#include "oled_show.h"
#include <stdio.h>
#include <string.h>

extern FishTank_DataStruct  FishTank_Data;
extern FishTank_FlagStruct  FishTank_Flag;
extern FishTank_SetStruct   FishTank_Set;
extern FishTank_FeedStruct  FishTank_Feed;
extern uint8_t AutoSet_Index;
extern uint8_t ManualSet_Index;
extern volatile uint8_t ESP01S_WiFiConnected;


static char _showBuf[22];


#define AUTOSET_ITEMS       7
#define MANUALSET_ITEMS     6


void OLED_ShowString(uint8_t row, uint8_t col, const char *str)
{
    OLED_Show_MoreCharF6X12(row, col, (uint8_t *)str);
}

void OLED_ShowFloat(uint8_t row, uint8_t col, float val, uint8_t decimal)
{
    char buf[14];
    if (decimal == 1)      sprintf(buf, "%.1f", val);
    else if (decimal == 2) sprintf(buf, "%.2f", val);
    else                   sprintf(buf, "%.0f", val);
    OLED_Show_MoreCharF6X12(row, col, (uint8_t *)buf);
}

void OLED_ShowInt(uint8_t row, uint8_t col, int val)
{
    char buf[10];
    sprintf(buf, "%d", val);
    OLED_Show_MoreCharF6X12(row, col, (uint8_t *)buf);
}

void OLED_ShowCN(uint8_t row, uint8_t col, const char *cn, uint8_t inv)
{
    ZoneBitCode_OLED_Show_ChineseF6X12(row, col, (uint8_t *)cn, inv);
}


void OLED_ShowStart(void)
{
    ZoneBitCode_OLED_Show_ChineseF6X12(2, 18, "\xd6\xc7", 0);
    ZoneBitCode_OLED_Show_ChineseF6X12(2, 30, "\xc4\xdc", 0);
    ZoneBitCode_OLED_Show_ChineseF6X12(2, 42, "\xd3\xe3", 0);
    ZoneBitCode_OLED_Show_ChineseF6X12(2, 54, "\xb8\xd7", 0);
    ZoneBitCode_OLED_Show_ChineseF6X12(2, 66, "\xcf\xb5", 0);
    ZoneBitCode_OLED_Show_ChineseF6X12(2, 78, "\xcd\xb3", 0);
}



void OLED_ShowMainPage(void)
{
    uint32_t total_sec;
    uint16_t min, sec;
    uint8_t state;

    OLED_Clr_Screen();

    /* === 行0: 温度 + 空气质量 === */
    OLED_ShowCN(0, 0, "\xce\xc2", 0);
    sprintf(_showBuf, "%.1fC", FishTank_Data.WaterTemp);
    OLED_ShowString(0, 14, _showBuf);
    OLED_ShowCN(0, 68, "\xbf\xd5", 0);
    OLED_ShowCN(0, 80, "\xc6\xf8", 0);
    OLED_ShowInt(0, 98, FishTank_Data.AirQuality);

    /* === 行2: 水位 + PH值 === */
    OLED_ShowCN(2, 0, "\xcb\xae", 0);
    OLED_ShowCN(2, 12, "\xce\xbb", 0);
    sprintf(_showBuf, "%d%%", FishTank_Data.WaterLevel);
    OLED_ShowString(2, 28, _showBuf);
    OLED_ShowCN(2, 56, "P", 0);
    OLED_ShowCN(2, 62, "H", 0);
    OLED_ShowFloat(2, 74, FishTank_Data.PH_Value, 1);

   
    OLED_ShowCN(4, 0, "\xc4\xa3", 0);
    if (FishTank_Flag.RunMode == 0)
        OLED_ShowCN(4, 14, "\xd7\xd4", 0);
    else
        OLED_ShowCN(4, 14, "\xca\xd6", 0);
    OLED_ShowCN(4, 56, "\xce\xb9", 0);

    /* WiFi 状态指示 */
    OLED_ShowString(4, 108, ESP01S_WiFiConnected ? "W+" : "W-");
    if (FishTank_Feed.Servo_Open)
    {
        OLED_ShowString(4, 68, "FEED");
    }
    else if (FishTank_Feed.Countdown > 0)
    {
        total_sec = FishTank_Feed.Countdown;
        min = total_sec / 60;
        sec = total_sec % 60;
        sprintf(_showBuf, "%02d:%02d", min, sec);
        OLED_ShowString(4, 68, _showBuf);
    }
    else
    {
        OLED_ShowString(4, 68, "--:--");
    }

    OLED_ShowCN(6, 0, "\xc8\xc8", 0);
    state = (FishTank_Flag.RunMode == 0)
            ? Relay_GetState(RELAY_HEAT)
            : FishTank_Flag.Relay_Heat_Manual;
    OLED_Show_OneCharF6X12(6, 12, state ? '+' : '-');

    /* 氧 */
    OLED_ShowCN(6, 26, "\xd1\xf5", 0);
    state = (FishTank_Flag.RunMode == 0)
            ? Relay_GetState(RELAY_OXYGEN)
            : FishTank_Flag.Relay_Oxygen_Manual;
    OLED_Show_OneCharF6X12(6, 38, state ? '+' : '-');

    /* 排 */
    OLED_ShowCN(6, 52, "\xc5\xc5", 0);
    state = (FishTank_Flag.RunMode == 0)
            ? Relay_GetState(RELAY_DRAIN)
            : FishTank_Flag.Relay_Drain_Manual;
    OLED_Show_OneCharF6X12(6, 64, state ? '+' : '-');

    /* 加 */
    OLED_ShowCN(6, 78, "\xbc\xd3", 0);
    state = (FishTank_Flag.RunMode == 0)
            ? Relay_GetState(RELAY_FILL)
            : FishTank_Flag.Relay_Fill_Manual;
    OLED_Show_OneCharF6X12(6, 90, state ? '+' : '-');
}

void OLED_ShowAutoSetPage(void)
{
    uint8_t screen;      /* 0=屏1, 1=屏2 */
    uint8_t sel;
    uint32_t h, m, s;

    OLED_Clr_Screen();

    /* 标题 - 行0 */
    OLED_ShowString(0, 12, "--AutoSet--");

    /* 判断当前选中项在哪个屏 */
    if (AutoSet_Index < 4)
        screen = 0;
    else
        screen = 1;

    if (screen == 0)
    {
        /* === 屏1: 索引 0-3 === */
        /* 行2: 温Lo */
        sel = (AutoSet_Index == 0) ? 1 : 0;
        OLED_ShowCN(2, 0, "\xce\xc2", sel);
        OLED_ShowString(2, 12, "Lo:");
        sprintf(_showBuf, "%.1f", FishTank_Set.TempLower);
        OLED_ShowString(2, 30, _showBuf);

        /* 行4: 温Hi */
        sel = (AutoSet_Index == 1) ? 1 : 0;
        OLED_ShowCN(4, 0, "\xce\xc2", sel);
        OLED_ShowString(4, 12, "Hi:");
        sprintf(_showBuf, "%.1f", FishTank_Set.TempUpper);
        OLED_ShowString(4, 30, _showBuf);

        /* 行6: 空气 或 PH Lo */
        if (AutoSet_Index == 2)
        {
            sel = 1;
        }
        else
        {
            sel = 0;
        }
        if (AutoSet_Index < 3)
        {
            OLED_ShowCN(6, 0, "\xbf\xd5", sel);
            OLED_ShowCN(6, 12, "\xc6\xf8", 0); OLED_ShowString(6, 24, ":");
            OLED_ShowInt(6, 30, FishTank_Set.AirQuality_Max);
        }
        else
        {
            /* Index 3: PH Lo */
            sel = (AutoSet_Index == 3) ? 1 : 0;
            OLED_ShowString(6, 0, "PH");
            OLED_ShowString(6, 12, "Lo:");
            sprintf(_showBuf, "%.1f", FishTank_Set.PH_Lower);
            OLED_ShowString(6, 30, _showBuf);
        }
    }
    else
    {
        /* === 屏2: 索引 4-6 === */
        /* 行2: PH Lo 或 PH Hi */
        if (AutoSet_Index <= 4)
        {
            sel = (AutoSet_Index == 4) ? 1 : 0;
            OLED_ShowString(2, 0, "PH");
            OLED_ShowString(2, 12, "Lo:");
            sprintf(_showBuf, "%.1f", FishTank_Set.PH_Lower);
            OLED_ShowString(2, 30, _showBuf);
        }

        /* 行4: PH Hi 或 水位 */
        if (AutoSet_Index <= 5)
        {
            sel = (AutoSet_Index == 5) ? 1 : 0;
            OLED_ShowString(4, 0, "PH");
            OLED_ShowString(4, 12, "Hi:");
            sprintf(_showBuf, "%.1f", FishTank_Set.PH_Upper);
            OLED_ShowString(4, 30, _showBuf);
        }

        /* 行6: 水位 或 喂食间隔 */
        sel = (AutoSet_Index == 6) ? 1 : 0;
        if (AutoSet_Index == 6)
        {
            /* 喂食间隔 HH:MM:SS */
            OLED_ShowCN(6, 0, "\xce\xb9", sel);
            OLED_ShowCN(6, 12, "\xca\xb3", 0); OLED_ShowString(6, 24, ":");
            h = FishTank_Set.FeedInterval / 3600;
            m = (FishTank_Set.FeedInterval % 3600) / 60;
            s = FishTank_Set.FeedInterval % 60;
            sprintf(_showBuf, "%02d:%02d:%02d", (int)h, (int)m, (int)s);
            OLED_ShowString(6, 30, _showBuf);
        }
        else
        {
            OLED_ShowCN(6, 0, "\xcb\xae", sel);
            OLED_ShowCN(6, 12, "\xce\xbb", 0); OLED_ShowString(6, 24, ":");
            sprintf(_showBuf, "%d%%", FishTank_Set.WaterLevel_Min);
            OLED_ShowString(6, 30, _showBuf);
        }
    }
}

/* ============================================================
 *  手动控制页 (规格书 §4.3) - 分屏显示
 *
 *  6 项控制, 分 2 屏:
 *  屏1 (ManualSet_Index 0-3):
 *    行0: -- 手动控制 --
 *    行2: 加热: 开/关  (选中反显)
 *    行4: 加水: 开/关
 *    行6: 排水: 开/关
 *  屏2 (ManualSet_Index 4-5):
 *    行0: -- 手动控制 --
 *    行2: 增氧: 开/关
 *    行4: 报警: 开/关
 *    行6: 喂食: 开/关
 * ============================================================ */

void OLED_ShowManualPage(void)
{
    uint8_t screen;
    uint8_t sel;

    OLED_Clr_Screen();

    /* 标题 - 行0 */
    OLED_ShowString(0, 18, "--Manual--");

    if (ManualSet_Index < 4)
        screen = 0;
    else
        screen = 1;

    if (screen == 0)
    {
        /* === 屏1: 索引 0-3 === */
        /* 行2: 加热 */
        sel = (ManualSet_Index == 0) ? 1 : 0;
        OLED_ShowCN(2, 0, "\xbc\xd3", sel);
        OLED_ShowCN(2, 12, "\xc8\xc8", sel);
        OLED_Show_OneCharF6X12(2, 24, ':');
        if (FishTank_Flag.Relay_Heat_Manual)
            OLED_ShowCN(2, 36, "\xbf\xaa", 0);
        else
            OLED_ShowCN(2, 36, "\xb9\xd8", 0);

        /* 行4: 加水 */
        sel = (ManualSet_Index == 1) ? 1 : 0;
        OLED_ShowCN(4, 0, "\xbc\xd3", sel);
        OLED_ShowCN(4, 12, "\xcb\xae", sel);
        OLED_Show_OneCharF6X12(4, 24, ':');
        if (FishTank_Flag.Relay_Fill_Manual)
            OLED_ShowCN(4, 36, "\xbf\xaa", 0);
        else
            OLED_ShowCN(4, 36, "\xb9\xd8", 0);

        /* 行6: 排水 */
        sel = (ManualSet_Index == 2) ? 1 : 0;
        OLED_ShowCN(6, 0, "\xc5\xc5", sel);
        OLED_ShowCN(6, 12, "\xcb\xae", sel);
        OLED_Show_OneCharF6X12(6, 24, ':');
        if (FishTank_Flag.Relay_Drain_Manual)
            OLED_ShowCN(6, 36, "\xbf\xaa", 0);
        else
            OLED_ShowCN(6, 36, "\xb9\xd8", 0);
    }
    else
    {
        /* === 屏2: 索引 4-5 === */
        /* 行2: 增氧 */
        sel = (ManualSet_Index == 3) ? 1 : 0;
        OLED_ShowCN(2, 0, "\xd4\xf6", sel);
        OLED_ShowCN(2, 12, "\xd1\xf5", sel);
        OLED_Show_OneCharF6X12(2, 24, ':');
        if (FishTank_Flag.Relay_Oxygen_Manual)
            OLED_ShowCN(2, 36, "\xbf\xaa", 0);
        else
            OLED_ShowCN(2, 36, "\xb9\xd8", 0);

        /* 行4: 报警 */
        sel = (ManualSet_Index == 4) ? 1 : 0;
        OLED_ShowCN(4, 0, "\xb1\xa8", sel);
        OLED_ShowCN(4, 12, "\xbe\xaf", sel);
        OLED_Show_OneCharF6X12(4, 24, ':');
        if (FishTank_Flag.Alarm_Enable)
            OLED_ShowCN(4, 36, "\xbf\xaa", 0);
        else
            OLED_ShowCN(4, 36, "\xb9\xd8", 0);

        /* 行6: 喂食 */
        sel = (ManualSet_Index == 5) ? 1 : 0;
        OLED_ShowCN(6, 0, "\xce\xb9", sel);
        OLED_ShowCN(6, 12, "\xca\xb3", sel);
        OLED_Show_OneCharF6X12(6, 24, ':');
        if (FishTank_Feed.Feeding)
            OLED_ShowCN(6, 36, "\xbf\xaa", 0);
        else
            OLED_ShowCN(6, 36, "\xb9\xd8", 0);
    }
}
