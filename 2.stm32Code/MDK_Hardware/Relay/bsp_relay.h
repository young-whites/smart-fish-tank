#ifndef __BSP_RELAY_H
#define __BSP_RELAY_H
#include "sys.h"

/* 继电器枚举 */
typedef enum
{
    RELAY_DRAIN  = 0,   /* 排水 - PB15 */
    RELAY_OXYGEN = 1,   /* 增氧 - PA15 */
    RELAY_FILL   = 2,   /* 加水 - PB14 */
    RELAY_HEAT   = 3,   /* 加热 - PB12 */
    RELAY_NUM    = 4
} Relay_TypeDef;

/* 继电器状态 */
#define RELAY_OFF   0
#define RELAY_ON    1

/* 函数声明 */
void Relay_Init(void);
void Relay_Control(uint8_t relay, uint8_t state);
void Relay_AllOff(void);
uint8_t Relay_GetState(uint8_t relay);

#endif /* __BSP_RELAY_H */
