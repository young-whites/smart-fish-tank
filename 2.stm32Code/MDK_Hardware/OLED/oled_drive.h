#ifndef __OLED_DRIVE_H
#define __OLED_DRIVE_H
#include "sys.h"




//*************OLED ���ź궨��******************
//SDA  PA11
#define OLED_SDA_RCC_APB2Periph_GPIOX   RCC_APB2Periph_GPIOA
#define OLED_SDA_Port	                GPIOA
#define OLED_SDA_Pin	                GPIO_Pin_11
#define OLED_SDA_Set()	                GPIO_SetBits(OLED_SDA_Port,OLED_SDA_Pin)
#define OLED_SDA_Clr()	                GPIO_ResetBits(OLED_SDA_Port,OLED_SDA_Pin)

//SCL  PA12
#define OLED_SCL_RCC_APB2Periph_GPIOX   RCC_APB2Periph_GPIOA
#define OLED_SCL_Port	                GPIOA
#define OLED_SCL_Pin	                GPIO_Pin_12
#define OLED_SCL_Set()	                GPIO_SetBits(OLED_SCL_Port,OLED_SCL_Pin)
#define OLED_SCL_Clr()	                GPIO_ResetBits(OLED_SCL_Port,OLED_SCL_Pin)



//***************OLED ���������������******************
void OLED_GPIO_Config(void);
void OLED_IIC_Start(void);
void OLED_IIC_Stop(void);
void OLED_IIC_Wait_ACK(void);
void OLED_IIC_Send_Byte(uint8_t txd);
void OLED_IIC_Write_Command(uint8_t IIC_Command);
void OLED_IIC_Write_Data(uint8_t IIC_Data);












#endif /*__OLED_DRIVE_H*/
