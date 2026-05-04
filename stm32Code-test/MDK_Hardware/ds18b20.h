#ifndef __DS18B20_H
#define __DS18B20_H
#include "stm32f10x.h"
#include "sys.h"



#define DS18B20_PORT 			GPIOB 
#define DS18B20_PIN 			GPIO_Pin_1
#define DS18B20_PORT_RCC		RCC_APB2Periph_GPIOB


								   
#define	DS18B20_DQ_OUT PBout(1) 
#define	DS18B20_DQ_IN  PBin(1)  
   	
uint8_t DS18B20_Init(void);			  
float   DS18B20_GetTemperture(void);
void    DS18B20_Start(void);		 
void    DS18B20_Write_Byte(uint8_t dat);
uint8_t DS18B20_Read_Byte(void);	
uint8_t DS18B20_Read_Bit(void);		
uint8_t DS18B20_Check(void);			
void    DS18B20_Reset(void);		


#endif /*__DS18B20_H*/
