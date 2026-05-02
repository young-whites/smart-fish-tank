#ifndef __BSP_LED_H_
#define	__BSP_LED_H_

#include "stm32f10x.h"

#define		LED_NUM			(1)

typedef	enum
{
	LED_Name_1	= (0x01),
} LED_Name_TypeDef;

/* LED 接 PB9 (与蜂鸣器共用) */
#define             macLED_1_GPIO_APBxClock_FUN              RCC_APB2PeriphClockCmd
#define             macLED_1_GPIO_CLK                        RCC_APB2Periph_GPIOB
#define             macLED_1_GPIO_PORT                       GPIOB
#define             macLED_1_GPIO_PIN                        GPIO_Pin_9

#define             macLED_1_OFF()                           GPIO_ResetBits(macLED_1_GPIO_PORT, macLED_1_GPIO_PIN)
#define             macLED_1_ON()                            GPIO_SetBits(macLED_1_GPIO_PORT, macLED_1_GPIO_PIN)

void LED_Init(void);
void LED_Out(int8_t ledName, int8_t ledState);
int8_t LED_GetNumber(void);
void LED_Off(int8_t ledName);
void LED_On(int8_t ledName);
void LED_Toggle(int8_t ledName);
void LED_Grad(int8_t ledName);
void LED_BlinkSetCycleDuty(int8_t ledName, int8_t Cycle, int8_t Duty);
void LED_Blink(int8_t ledName, int8_t cry, int8_t mute, int8_t repeat);
void LED_Fancy(int8_t mode);
void LED_DrvScan(void);

#endif /* __BSP_LED_H_ */
