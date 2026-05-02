#ifndef __BSP_SERVO_H
#define __BSP_SERVO_H
#include "sys.h"

/* 舵机引脚 PA6 = TIM3_CH1 */

/* 函数声明 */
void Servo_Init(void);
void Servo_SetAngle(uint8_t angle);
void Servo_Off(void);

#endif /* __BSP_SERVO_H */
