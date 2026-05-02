#ifndef __BSP_DS18B20_H
#define __BSP_DS18B20_H
#include "sys.h"

/* DS18B20 引脚定义 - PB13 */
#define DS18B20_RCC_APB2Periph_GPIOX    RCC_APB2Periph_GPIOB
#define DS18B20_GPIO_PORT               GPIOB
#define DS18B20_GPIO_PIN                GPIO_Pin_13

/* IO操作宏 */
#define DS18B20_DQ_OUT_H()              GPIO_SetBits(DS18B20_GPIO_PORT, DS18B20_GPIO_PIN)
#define DS18B20_DQ_OUT_L()              GPIO_ResetBits(DS18B20_GPIO_PORT, DS18B20_GPIO_PIN)
#define DS18B20_DQ_IN()                 GPIO_ReadInputDataBit(DS18B20_GPIO_PORT, DS18B20_GPIO_PIN)

/* 函数声明 */
void    DS18B20_Init(void);
void    DS18B20_StartConvert(void);
float   DS18B20_ReadTemp(void);

#endif /* __BSP_DS18B20_H */
