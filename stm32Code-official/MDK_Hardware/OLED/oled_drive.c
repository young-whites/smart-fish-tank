#include "oled_drive.h"








void OLED_GPIO_Config(void)
{
	GPIO_InitTypeDef 	GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitStructure.GPIO_Pin = OLED_SCL_Pin | OLED_SDA_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_SetBits(GPIOA, OLED_SCL_Pin | OLED_SDA_Pin);
}





void OLED_IIC_Start(void)
{
	OLED_SCL_Set();
	OLED_SDA_Set();
	OLED_SDA_Clr();
	OLED_SCL_Clr();
}





void OLED_IIC_Stop(void)
{
	OLED_SCL_Set();
	OLED_SDA_Clr();
	OLED_SDA_Set();
}







void OLED_IIC_Wait_ACK(void)
{
	OLED_SCL_Set();
	OLED_SCL_Clr();
}






void OLED_IIC_Send_Byte(uint8_t txd)
{
	uint8_t k;
	uint8_t datTemp, datTemp2;
	datTemp = txd;
	OLED_SCL_Clr();

	for (k = 0; k < 8; k++)
	{
		datTemp2 = datTemp;
		datTemp2 = datTemp2 & 0x80;

		if (datTemp2 == 0x80)
		{
			OLED_SDA_Set();
		}
		else
			OLED_SDA_Clr();

		datTemp = datTemp << 1;
		OLED_SCL_Set();
		OLED_SCL_Clr();
	}
}







void OLED_IIC_Write_Command(uint8_t IIC_Command)
{
	OLED_IIC_Start();
	OLED_IIC_Send_Byte(0x78);
	OLED_IIC_Wait_ACK();
	OLED_IIC_Send_Byte(0x00);
	OLED_IIC_Wait_ACK();
	OLED_IIC_Send_Byte(IIC_Command);
	OLED_IIC_Wait_ACK();
	OLED_IIC_Stop();
}





void OLED_IIC_Write_Data(uint8_t IIC_Data)
{
	OLED_IIC_Start();
	OLED_IIC_Send_Byte(0x78);
	OLED_IIC_Wait_ACK();
	OLED_IIC_Send_Byte(0x40);
	OLED_IIC_Wait_ACK();
	OLED_IIC_Send_Byte(IIC_Data);
	OLED_IIC_Wait_ACK();
	OLED_IIC_Stop();
}




void OLED_Write_Byte(uint8_t byt, uint8_t cmdType)
{

	if (cmdType == 0)
	{
		OLED_IIC_Write_Command(byt);
	}

	else if (cmdType == 1)
	{
		OLED_IIC_Write_Data(byt);
	}

}