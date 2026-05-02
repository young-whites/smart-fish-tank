#ifndef __BSP_KEY_H
#define __BSP_KEY_H
#include "sys.h"


/* KEY1 = PA8 (返回/退出) */
#define             macKEY_A_GPIO_APBxClock_FUN              RCC_APB2PeriphClockCmd
#define             macKEY_A_GPIO_CLK                        RCC_APB2Periph_GPIOA
#define             macKEY_A_GPIO_PORT                       GPIOA
#define             macKEY_A_GPIO_PIN                        GPIO_Pin_8

/* KEY2 = PB3 (页面切换) */
#define             macKEY_B_GPIO_APBxClock_FUN              RCC_APB2PeriphClockCmd
#define             macKEY_B_GPIO_CLK                        RCC_APB2Periph_GPIOB
#define             macKEY_B_GPIO_PORT                       GPIOB
#define             macKEY_B_GPIO_PIN                        GPIO_Pin_3

/* KEY3 = PB4 (加/模式切换) */
#define             macKEY_C_GPIO_APBxClock_FUN              RCC_APB2PeriphClockCmd
#define             macKEY_C_GPIO_CLK                        RCC_APB2Periph_GPIOB
#define             macKEY_C_GPIO_PORT                       GPIOB
#define             macKEY_C_GPIO_PIN                        GPIO_Pin_4

/* KEY4 = PB8 (减) */
#define             macKEY_D_GPIO_APBxClock_FUN              RCC_APB2PeriphClockCmd
#define             macKEY_D_GPIO_CLK                        RCC_APB2Periph_GPIOB
#define             macKEY_D_GPIO_PORT                       GPIOB
#define             macKEY_D_GPIO_PIN                        GPIO_Pin_8

/* KEY5 = PB1 (手动触发喂食) */
#define             macKEY_E_GPIO_APBxClock_FUN              RCC_APB2PeriphClockCmd
#define             macKEY_E_GPIO_CLK                        RCC_APB2Periph_GPIOB
#define             macKEY_E_GPIO_PORT                       GPIOB
#define             macKEY_E_GPIO_PIN                        GPIO_Pin_1


#define KEYA        GPIO_ReadInputDataBit(macKEY_A_GPIO_PORT, macKEY_A_GPIO_PIN)
#define KEYB        GPIO_ReadInputDataBit(macKEY_B_GPIO_PORT, macKEY_B_GPIO_PIN)
#define KEYC        GPIO_ReadInputDataBit(macKEY_C_GPIO_PORT, macKEY_C_GPIO_PIN)
#define KEYD        GPIO_ReadInputDataBit(macKEY_D_GPIO_PORT, macKEY_D_GPIO_PIN)
#define KEYE        GPIO_ReadInputDataBit(macKEY_E_GPIO_PORT, macKEY_E_GPIO_PIN)

#define		KEY_NUM			(5)		// 按键总数量
typedef	enum	                    //  键值定义
{
	KEY_A	= (0x01),
    KEY_B,
    KEY_C,
    KEY_D,
    KEY_E,
}KEY_Val_TypeDef;

typedef	enum {					    // 按键事件类型
    KEY_Evt_Press   = (0x80),		// 按下
    KEY_Evt_Release = (0x40),		// 松开
    KEY_Evt_Long2S  = (0x20),		// 长按2s
    KEY_Evt_Long4S  = (0x10),		// 长按4s
}KEY_Evt_TypeDef;


void KEY_GPIO_Config(void);
void KEY_Scan(void);
void KEY_DrvScan(void);
uint8_t KEY_Read(void);
void KEY_Write(uint8_t value);


#endif /*__BSP_KEY_H*/
