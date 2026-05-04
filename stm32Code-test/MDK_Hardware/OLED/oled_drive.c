#include "oled_drive.h"



 /*******************************************/
 /*					OLED的寄存器            */
 /*-----------------------------------------*/
 /*	
  * OLED  IIC的写地址	0x78
  * OLED  写数据指令	0x40
  * OLED  写命令指令    0x00
  * 
  */




 /* OLED 相关的引脚配置**********************************************************/

 /**
   * @brief  OLED_GPIO SCL 引脚配置函数
   * @param  None
   * @retval None
   */
void OLED_GPIO_Config(void)
{
	GPIO_InitTypeDef 	GPIO_InitStructure;

	//GPIOC端口时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitStructure.GPIO_Pin = OLED_SCL_Pin | OLED_SDA_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_SetBits(GPIOA, OLED_SCL_Pin | OLED_SDA_Pin);
}





/* OLED IIC相关的配置**********************************************************/

/**
  * @brief  IIC的启动信号
  * @param  None
  * @retval 当SCL为高电平期间，SDA线由高变低
  */
void OLED_IIC_Start(void)
{
	/* SDA线 和 SCL线 同时设置为高（输出模式）*/
	OLED_SCL_Set();
	OLED_SDA_Set();
	OLED_SDA_Clr();
	OLED_SCL_Clr();
}




/**
  * @brief  IIC的停止信号
  * @param  None
  * @retval 当SCL为高电平期间，SDA线由低到高
  */
void OLED_IIC_Stop(void)
{
	/* SCL线 设置为高，SDA线 设置为低（输出模式）*/
	OLED_SCL_Set();
	OLED_SDA_Clr();
	OLED_SDA_Set();
}






/**
  * @brief  IIC等待应答信号
  * @param  None
  * @retval 每当主机发送完一个字节数据，主机总是等待从机返回一个应答信号，以确认主机是否成功接收到了数据
  *         从机应答主机所需要的时钟仍是主机提供的（即SCL线 为输出模式），应答出现在每一次主机完成8个数据位
  *			传输后紧跟着的时钟周期，低电平表示应答，高电平表示非应答
  */
void OLED_IIC_Wait_ACK(void)
{
	OLED_SCL_Set();
	OLED_SCL_Clr();
}






/**
  * @brief  IIC 发送一个字节
  * @param  txd ：字节数据
  * @retval None
  */
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







/* 以下是向从机指定寄存器地址中写/读指令的一些函数定义***********************************************************/
/**
  * @brief  向OLED从机写指令
  * @param  IIC_Command ：指令码
  * @retval None
  */
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






/**
  * @brief  向OLED从机写数据码
  * @param  IIC_Data ：数据码
  * @retval None
  */
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






/* 以下是 OLED 显示屏的专用的一些写入/读取函数******************************************************************/
/**
  * @brief  向 OLED 指定寄存器写一个字节
  *	@param 	byt		: 写入的字节数据
  *			cmdType	: 0 --> 命令类型
  * 				  1 --> 数据类型
  * @retval None
  */
void OLED_Write_Byte(uint8_t byt, uint8_t cmdType)
{
	/* 写命令数据 */
	if (cmdType == 0)
	{
		OLED_IIC_Write_Command(byt);
	}

	/* OLED 写数据 */
	else if (cmdType == 1)
	{
		OLED_IIC_Write_Data(byt);
	}

}