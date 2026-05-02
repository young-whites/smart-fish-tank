#include "TimingSet.h"
#include <string.h>

/* 外部变量 */
extern FishTank_DataStruct  FishTank_Data;
extern FishTank_FlagStruct  FishTank_Flag;
extern FishTank_SetStruct   FishTank_Set;
extern FishTank_FeedStruct  FishTank_Feed;
extern uint8_t CurrentPage;
extern uint8_t Feed_Trigger;

/* ADC外部变量 */
extern __IO uint32_t ADC_ConvertedValue;

/* ADC采集通道计数 */
static uint8_t adc_channel_idx = 0;

/* 传感器滤波 */
static float _temp_buf = 25.0f;
static uint16_t _air_buf = 0;
static uint8_t  _water_buf = 0;
static float _ph_buf = 7.0f;


/**
 * @brief  自动控制逻辑 - 500ms调用
 */
static void Auto_Control(void)
{
    if (FishTank_Flag.RunMode != 0) return;  /* 非自动模式不执行 */

    /* 温度控制 */
    if (FishTank_Data.WaterTemp < FishTank_Set.TempLower)
    {
        Relay_Control(RELAY_HEAT, RELAY_ON);
    }
    else if (FishTank_Data.WaterTemp > FishTank_Set.TempUpper)
    {
        Relay_Control(RELAY_HEAT, RELAY_OFF);
    }

    /* 水位控制 */
    if (FishTank_Data.WaterLevel > FishTank_Set.WaterLevel_Max)
    {
        Relay_Control(RELAY_FILL, RELAY_OFF);
        Relay_Control(RELAY_DRAIN, RELAY_ON);
    }
    else if (FishTank_Data.WaterLevel < FishTank_Set.WaterLevel_Min)
    {
        Relay_Control(RELAY_DRAIN, RELAY_OFF);
        Relay_Control(RELAY_FILL, RELAY_ON);
    }
    else
    {
        Relay_Control(RELAY_DRAIN, RELAY_OFF);
        Relay_Control(RELAY_FILL, RELAY_OFF);
    }

    /* 空气质量控制（增氧）*/
    if (FishTank_Data.AirQuality > FishTank_Set.AirQuality_Max)
    {
        Relay_Control(RELAY_OXYGEN, RELAY_ON);
    }
    else
    {
        Relay_Control(RELAY_OXYGEN, RELAY_OFF);
    }
}


/**
 * @brief  报警逻辑 - 500ms调用
 */
static void Alarm_Process(void)
{
    uint8_t alarm_active = 0;

    if (!FishTank_Flag.Alarm_Enable) return;

    /* 温度越限 */
    if (FishTank_Data.WaterTemp < FishTank_Set.TempLower ||
        FishTank_Data.WaterTemp > FishTank_Set.TempUpper)
    {
        FishTank_Flag.TempLow_Alarm = (FishTank_Data.WaterTemp < FishTank_Set.TempLower) ? 1 : 0;
        FishTank_Flag.TempHigh_Alarm = (FishTank_Data.WaterTemp > FishTank_Set.TempUpper) ? 1 : 0;
        alarm_active = 1;
    }
    else
    {
        FishTank_Flag.TempLow_Alarm = 0;
        FishTank_Flag.TempHigh_Alarm = 0;
    }

    /* 水位异常 */
    if (FishTank_Data.WaterLevel < FishTank_Set.WaterLevel_Min ||
        FishTank_Data.WaterLevel > FishTank_Set.WaterLevel_Max)
    {
        FishTank_Flag.WaterLevel_Alarm = 1;
        alarm_active = 1;
    }
    else
    {
        FishTank_Flag.WaterLevel_Alarm = 0;
    }

    /* 空气超标 */
    if (FishTank_Data.AirQuality > FishTank_Set.AirQuality_Max)
    {
        FishTank_Flag.AirQuality_Alarm = 1;
        alarm_active = 1;
    }
    else
    {
        FishTank_Flag.AirQuality_Alarm = 0;
    }

    /* PH越限 */
    if (FishTank_Data.PH_Value < FishTank_Set.PH_Lower ||
        FishTank_Data.PH_Value > FishTank_Set.PH_Upper)
    {
        FishTank_Flag.PHLow_Alarm = (FishTank_Data.PH_Value < FishTank_Set.PH_Lower) ? 1 : 0;
        FishTank_Flag.PHHigh_Alarm = (FishTank_Data.PH_Value > FishTank_Set.PH_Upper) ? 1 : 0;
        alarm_active = 1;
    }
    else
    {
        FishTank_Flag.PHLow_Alarm = 0;
        FishTank_Flag.PHHigh_Alarm = 0;
    }

    /* 触发声光报警 - 响0.5s/停2s 循环 (W1/W2修复) */
    if (alarm_active)
    {
        BEEP_SetCycleDuty(2500, 500);   /* 周期2500ms, 响500ms */
        BEEP_Blink(1, 0, 100);           /* 无限重复 */
        LED_BlinkSetCycleDuty(LED_Name_1, 25, 5);  /* LED: 25*100ms=2.5s, 亮5*100ms=0.5s */
        LED_Blink(LED_Name_1, 1, 0, 100);
    }
}


/**
 * @brief  喂食控制 - 1s调用
 */
static void Feed_Process(void)
{
    /* 手动触发喂食 */
    if (Feed_Trigger)
    {
        Feed_Trigger = 0;
        if (!FishTank_Feed.Feeding)
        {
            FishTank_Feed.Feeding = 1;
            FishTank_Feed.Servo_Open = 1;
            FishTank_Feed.Servo_Timer = 0;
            Servo_SetAngle(90);  /* 打开舵机 */
        }
    }

    /* 喂食倒计时 */
    if (FishTank_Feed.Countdown > 0)
    {
        FishTank_Feed.Countdown--;
        if (FishTank_Feed.Countdown == 0)
        {
            /* 倒计时归零, 触发喂食 */
            if (!FishTank_Feed.Feeding)
            {
                FishTank_Feed.Feeding = 1;
                FishTank_Feed.Servo_Open = 1;
                FishTank_Feed.Servo_Timer = 0;
                Servo_SetAngle(90);
            }
        }
    }

    /* 喂食中: 舵机打开3秒后关闭 */
    if (FishTank_Feed.Servo_Open)
    {
        FishTank_Feed.Servo_Timer++;
        if (FishTank_Feed.Servo_Timer >= 3)
        {
            Servo_Off();
            FishTank_Feed.Servo_Open = 0;
            FishTank_Feed.Feeding = 0;
            /* 重置喂食倒计时 */
            FishTank_Feed.Countdown = FishTank_Set.FeedInterval;
        }
    }
}


/**
 * @brief  OLED刷新 - 1s调用
 */
static void OLED_Refresh(void)
{
    switch (CurrentPage)
    {
        case 0:
            OLED_ShowMainPage();
            break;
        case 1:
            OLED_ShowAutoSetPage();
            break;
        case 2:
            OLED_ShowManualPage();
            break;
        default:
            CurrentPage = 0;
            OLED_ShowMainPage();
            break;
    }
}


/* 定时任务函数 ***********************************************************************/


/**
 * @brief  1s定时任务
 *
 * DS18B20 非阻塞读取方案 (W4/W6修复):
 *  奇数秒: 发送温度转换命令 (0x44)
 *  偶数秒: 读取温度结果, 有效范围检查
 */
static uint8_t ds18b20_step = 0;

void Timing_1s(void)
{
    float temp;

    /* DS18B20 非阻塞分步读取 */
    if (ds18b20_step == 0)
    {
        /* 第1步: 启动转换 */
        DS18B20_StartConvert();
        ds18b20_step = 1;
    }
    else
    {
        /* 第2步: 读取结果 */
        temp = DS18B20_ReadTemp();
        if (temp > -50.0f && temp < 125.0f)
        {
            FishTank_Data.WaterTemp = temp;
        }
        ds18b20_step = 0;
    }

    Feed_Process();
    OLED_Refresh();

    /* WiFi 后台重连 + 数据上报 */
    ESP01S_ReconnectWiFi();
    ESP01S_SendSensorData();
}


/**
 * @brief  1ms定时任务
 */
void Timing_1ms(void)
{
	static int cntNum = 0;
	BEEP_DrvScan();
	LED_DrvScan();
	KEY_DrvScan();

	if (++cntNum > 60000)
	{
		cntNum = 0;
	}
}


void Timing_2ms(void)
{
}


void Timing_5ms(void)
{
}


/**
 * @brief  10ms定时任务 - 传感器采集
 */
void Timing_10ms(void)
{
    Check_SysWorkStatus();

    /* ADC 轮流采集三个通道 */
    adc_channel_idx++;
    if (adc_channel_idx > 2) adc_channel_idx = 0;

    switch (adc_channel_idx)
    {
        case 0:  /* 空气质量 - PA4 ADC1_IN4 */
        {
            ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 1, ADC_SampleTime_55Cycles5);
            ADC_SoftwareStartConvCmd(ADC1, ENABLE);
            while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
            _air_buf = ADC_GetConversionValue(ADC1);
            FishTank_Data.AirQuality = _air_buf;
            break;
        }
        case 1:  /* 水位 - PA1 ADC1_IN1 */
        {
            ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_55Cycles5);
            ADC_SoftwareStartConvCmd(ADC1, ENABLE);
            while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
            _water_buf = (uint8_t)(ADC_GetConversionValue(ADC1) * 100 / 4095);
            FishTank_Data.WaterLevel = _water_buf;
            break;
        }
        case 2:  /* PH值 - PA0 ADC1_IN0 */
        {
            ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_55Cycles5);
            ADC_SoftwareStartConvCmd(ADC1, ENABLE);
            while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
            _ph_buf = (float)ADC_GetConversionValue(ADC1) * 14.0f / 4095.0f;
            FishTank_Data.PH_Value = _ph_buf;
            break;
        }
    }
}


/**
 * @brief  50ms定时任务 - 按键扫描
 */
void Timing_50ms(void)
{
    KEY_Scan();
    ESP01S_ProcessReceived();
}


/**
 * @brief  500ms定时任务 - 控制与报警
 */
void Timing_500ms(void)
{
    Auto_Control();
    Alarm_Process();

    /* WiFi 上报设备状态 */
    ESP01S_SendDeviceStatus();
}
