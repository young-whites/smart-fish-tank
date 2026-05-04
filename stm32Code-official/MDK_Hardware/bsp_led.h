#ifndef __BSP_LED_H_
#define	__BSP_LED_H_



#include "stm32f10x.h"




#define		LED_NUM			(1)				// LED数量

	/*LED名称枚举*/
	typedef	enum	
	{								
		LED_Name_1	= (0x01),			// 蜂鸣器+LED PB9
	}LED_Name_TypeDef;
	//------------------------------------------------------------------------







 
/****************************** LED1 PB9 配置参数定义 ***************************************/
#define             macLED_1_GPIO_APBxClock_FUN              RCC_APB2PeriphClockCmd
#define             macLED_1_GPIO_CLK                        RCC_APB2Periph_GPIOB
#define             macLED_1_GPIO_PORT                       GPIOB
#define             macLED_1_GPIO_PIN                        GPIO_Pin_9



/****************************** LED 控制宏定义 ***************************************/
#define             macLED_1_OFF()                           GPIO_ResetBits ( macLED_1_GPIO_PORT, macLED_1_GPIO_PIN )
#define             macLED_1_ON()                            GPIO_SetBits ( macLED_1_GPIO_PORT, macLED_1_GPIO_PIN )


/************************** LED 函数声明 ********************************/
void LED_Init(void);
void LED_Out(int8_t ledName, int8_t ledState);
int8_t 	LED_GetNumber(void);
void LED_Off(int8_t ledName);
void LED_On(int8_t ledName);
void LED_Toggle(int8_t ledName);
void LED_Grad(int8_t ledName);
void LED_BlinkSetCycleDuty(int8_t ledName, int8_t Cycle, int8_t Duty);
void LED_Blink(int8_t ledName, int8_t cry, int8_t mute, int8_t repeat);
void LED_Fancy(int8_t mode);
void LED_DrvScan(void);

#endif /* __LED_H_ */
