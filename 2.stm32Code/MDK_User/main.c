#include <stdio.h>	 	 
#include <string.h>	
#include <stdarg.h>
#include "sys.h"

/* 全局变量 */
uint8_t CurrentPage = 0;        /* 当前页面: 0=主页面, 1=自动设置, 2=手动控制 */
uint8_t AutoSet_Index = 0;      /* 自动设置页当前选中项 */
uint8_t ManualSet_Index = 0;    /* 手动控制页当前选中项 */
uint8_t Feed_Trigger = 0;       /* 喂食触发标志 */

/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main ( void )
{	
	/* 设置中断优先级分组为4，4位抢占优先级 */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	delay_init(72);
	/* 通用定时器初始化，用于定时中断 */
	GENERAL_TIM_Init();
	
    /* PB3/PB4复用为普通IO使用，需要进行此步配置，需要开启复用时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    /* PB3/PB4属于JTAG的调试引脚，为了配置为GPIO，需要关闭JTAG功能 */
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

    /* 各硬件初始化 */
    LED_Init();
    Beep_Init();
    KEY_GPIO_Config();
    OLED_Init();
    ADCx_Init();
    DS18B20_Init();
    Relay_Init();
    Servo_Init();
    FireUSART_1_Config();
    ESP01S_Init();

    /* 启动画面 */
    OLED_ShowStart();
    delay_ms(2000);

    /* 全局参数初始化 */
    SysFlag_Init();

    /* 初始化默认状态 */
    FishTank_Flag.RunMode = 0;         /* 默认自动模式 */
    FishTank_Flag.Alarm_Enable = 1;    /* 默认报警开启 */
    FishTank_Feed.Countdown = FishTank_Set.FeedInterval;
    Servo_Off();

    /* 进入主页面 */
    OLED_ShowMainPage();

    while ( 1 )
    {
        IWDG_Feed();
    }
}


/*********************************************END OF FILE**********************/
