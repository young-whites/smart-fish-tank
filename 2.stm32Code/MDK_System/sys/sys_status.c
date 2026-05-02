#include "sys_status.h"


/**
  * @brief  系统全局参数初始?
  */
void SysFlag_Init(void)
{
	/* 鱼缸系统默认参数 */
	FishTank_Set.TempUpper      = 30.0f;
	FishTank_Set.TempLower      = 20.0f;
	FishTank_Set.PH_Upper       = 8.5f;
	FishTank_Set.PH_Lower       = 6.5f;
	FishTank_Set.WaterLevel_Min = 40;      /* 水位下限 40% */
	FishTank_Set.WaterLevel_Max = 80;      /* 水位上限 80% */
	FishTank_Set.AirQuality_Max = 500;     /* 空气质量阈值 */
	FishTank_Set.FeedInterval   = 14400;   /* 喂食间隔 4小时(秒) */
}


/**
  * @brief  检测系统工作状态标志
  */
void Check_SysWorkStatus(void)
{

}
