#ifndef __BSP_ADC_H
#define __BSP_ADC_H
#include "sys.h"

/* ADC1 channels: PA0(IN0)=PH, PA1(IN1)=Water level, PA4(IN4)=Air quality */

void ADC_Polling_Init(void);
uint16_t ADC_ReadChannel(uint8_t channel);

#endif /*__BSP_ADC_H*/
