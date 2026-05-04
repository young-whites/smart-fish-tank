#ifndef __BEEP_H_
#define	__BEEP_H_

#include "stm32f10x.h"
#include "sys.h"








/******************** BEEP Pin Configuration Parameters **************************/
#define             macBEEP_GPIO_APBxClock_FUN              RCC_APB2PeriphClockCmd
#define             macBEEP_GPIO_CLK                        RCC_APB2Periph_GPIOB
#define             macBEEP_PORT                            GPIOB
#define             macBEEP_PIN                             GPIO_Pin_9



/******************** BEEP Function Macro Definitions **************************/
#define             macBEEP_OFF()	                        GPIO_ResetBits ( macBEEP_PORT, macBEEP_PIN )
#define             macBEEP_ON()                         	GPIO_SetBits ( macBEEP_PORT, macBEEP_PIN )



/************************** BEEP Function Declarations ********************************/
void Beep_Init(void);
void BEEP_Off(void);
void BEEP_On(void);
void BEEP_SetCycleDuty(int16_t Cycle, int16_t Duty);
void BEEP_Blink(int8_t cry, int8_t mute, int8_t repeat);
void BEEP_DrvScan(void);

#endif /* __BEEP_H_ */
