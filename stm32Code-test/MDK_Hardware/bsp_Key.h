#ifndef __BSP_KEY_H
#define __BSP_KEY_H
#include "sys.h"



/****************************** KEY1 = PA6 配置 ***************************************/
#define             macKEY_A_GPIO_APBxClock_FUN              RCC_APB2PeriphClockCmd
#define             macKEY_A_GPIO_CLK                        RCC_APB2Periph_GPIOA
#define             macKEY_A_GPIO_PORT                       GPIOA
#define             macKEY_A_GPIO_PIN                        GPIO_Pin_6

/****************************** KEY2 = PA7 配置 ***************************************/
#define             macKEY_B_GPIO_APBxClock_FUN              RCC_APB2PeriphClockCmd
#define             macKEY_B_GPIO_CLK                        RCC_APB2Periph_GPIOA
#define             macKEY_B_GPIO_PORT                       GPIOA
#define             macKEY_B_GPIO_PIN                        GPIO_Pin_7

/****************************** KEY3 = PB0 配置 ***************************************/
#define             macKEY_C_GPIO_APBxClock_FUN              RCC_APB2PeriphClockCmd
#define             macKEY_C_GPIO_CLK                        RCC_APB2Periph_GPIOB
#define             macKEY_C_GPIO_PORT                       GPIOB
#define             macKEY_C_GPIO_PIN                        GPIO_Pin_0

/****************************** KEY4 = PA3 配置 ***************************************/
#define             macKEY_D_GPIO_APBxClock_FUN              RCC_APB2PeriphClockCmd
#define             macKEY_D_GPIO_CLK                        RCC_APB2Periph_GPIOA
#define             macKEY_D_GPIO_PORT                       GPIOA
#define             macKEY_D_GPIO_PIN                        GPIO_Pin_3

/****************************** KEY5 = PA4 配置 ***************************************/
#define             macKEY_E_GPIO_APBxClock_FUN              RCC_APB2PeriphClockCmd
#define             macKEY_E_GPIO_CLK                        RCC_APB2Periph_GPIOA
#define             macKEY_E_GPIO_PORT                       GPIOA
#define             macKEY_E_GPIO_PIN                        GPIO_Pin_4




#define KEYA        GPIO_ReadInputDataBit(macKEY_A_GPIO_PORT, macKEY_A_GPIO_PIN)     /* KEY1引脚 PA6 */
#define KEYB        GPIO_ReadInputDataBit(macKEY_B_GPIO_PORT, macKEY_B_GPIO_PIN)     /* KEY2引脚 PA7 */
#define KEYC        GPIO_ReadInputDataBit(macKEY_C_GPIO_PORT, macKEY_C_GPIO_PIN)     /* KEY3引脚 PB0 */
#define KEYD        GPIO_ReadInputDataBit(macKEY_D_GPIO_PORT, macKEY_D_GPIO_PIN)     /* KEY4引脚 PA3 */
#define KEYE        GPIO_ReadInputDataBit(macKEY_E_GPIO_PORT, macKEY_E_GPIO_PIN)     /* KEY5引脚 PA4 */

#define		KEY_NUM			(5)		// 按键数量

typedef	enum	                    //  按键�?�?
{					
	KEY_A	= (0x01),		        //  KEY1 - PA6 - 页面切换
    KEY_B,                          //  KEY2 - PA7 - 阈�?�参数�?�择
    KEY_C,                          //  KEY3 - PB0 - �?/模式切换
    KEY_D,                          //  KEY4 - PA3 - �?
    KEY_E,                          //  KEY5 - PA4 - 喂�?�触�?
}KEY_Val_TypeDef;
//------------------------------------------------------------------------

typedef	enum {					    // 按键事件类型
    KEY_Evt_Press   = (0x80),		// 按下
    KEY_Evt_Release = (0x40),		// 释放
    KEY_Evt_Long2S  = (0x20),		// 长按2s
    KEY_Evt_Long4S  = (0x10),		// 长按4s
}KEY_Evt_TypeDef;
//------------------------------------------------------------------------

// 阈�?�参数数�? (温度下限,温度上限,空气上限,水位下限,水位上限,PH下限,PH上限 = 7�?)
#define THRESHOLD_PARAM_NUM  3

// 阈�?�调节�?�进
#define THRESHOLD_TEMP_STEP    1.0f    // 温度步进 1�?
#define THRESHOLD_PH_STEP      0.1f    // PH步进 0.1
#define THRESHOLD_AIR_STEP     10      // 空气质量步进 10
#define THRESHOLD_WL_STEP      1       // 水位步进 1%

void KEY_GPIO_Config(void);
void KEY_Scan(void);
void KEY_DrvScan(void);
uint8_t KEY_Read(void);
void KEY_Write(uint8_t value);


#endif /*__BSP_KEY_H*/
