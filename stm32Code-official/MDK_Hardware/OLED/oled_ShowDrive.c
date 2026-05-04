#include "oled_ShowDrive.h"
#include "oledchar_6X12.h"
#include "oledChinese_6X12.h"
#include "oledChinese_8X16.h"



/**
  * @brief  OLED 清屏函数
  *	@param 	None
  * @retval None
  */
void OLED_Full_OFF(void)
{
	uint8_t i, n;
	for (i = 0; i < 8; i++)
	{

		OLED_Write_Byte(0xb0 + i, OLED_CMD);   // 设置起始页地址（每一页有8小格子）
		OLED_Write_Byte(0x00, OLED_CMD);       // 设置水平寻址方式
		OLED_Write_Byte(0x01, OLED_CMD);       // 设置垂直寻址方式
		for (n = 0; n < X_WIDTH; n++)
			OLED_Write_Byte(0x00, OLED_DAT);
	}
}

/**
  * @brief  OLED 清屏函数
  *	@param 	None
  * @retval None
  */
void OLED_Clr_Screen(void)
{
	uint8_t i, j;
	for (i = 0; i < 8; i++)
	{
		OLED_Set_Pos(i, 0);
		for (j = 0; j < 128; j++)
		{
			OLED_Write_Byte(0x00, OLED_DAT);
		}
	}
}


/**
  * @brief  设置坐标函数
  * @param  row     :  0~7行
  *		 column  :  0~127列
  * @retval None
  */
void OLED_Set_Pos(uint8_t row, uint8_t column)
{
	OLED_Write_Byte(0xB0 + row, OLED_CMD);
	OLED_Write_Byte(((column & 0xF0) >> 4) | 0x10, OLED_CMD);
	OLED_Write_Byte((column & 0x0F) | 0x00, OLED_CMD);
}




/**
  * @brief  计算 m^n 幂函数
  * @param  m : 底数
  *			n : 幂级数
  * @retval 返回值为计算结果
  */
uint32_t OLED_Calculate_Pow(uint8_t m, uint8_t n)
{
	uint32_t result = 1;

	while (n--)
	{
		result *= m;
	}
	return result;
}



/****************************************************************************************************************************/
/* ---------------------------------------------- 显示 6*12 字体的显示驱动函数 ---------------------------------------------*/
/****************************************************************************************************************************/



 /**
   * @brief  显示字体大小为6*12的单个汉字(区位码)
   * @param  row      :  0~7行
   *		 column   :  0~127列
   *		 flag     :  反白标志，非0时反白显示
   *		 *chinese :  字符指针
   * @retval
   */
void ZoneBitCode_OLED_Show_ChineseF6X12(uint8_t row, uint8_t column, uint8_t* chinese, uint8_t flag)
{
	uint8_t  i = 0;
	uint32_t index;
	for (index = 0; index < sizeof(Hzk12) / 35; index++)      // name[3] + dat[32] ==35
	{
		if (Hzk12[index].name[0] == chinese[0] && Hzk12[index].name[1] == chinese[1])  // 对比汉字区码位码
		{
			OLED_Set_Pos(row, column);
			for (i = 0; i < 12; i++)
			{
				if (flag == 0)
					OLED_Write_Byte(Hzk12[index].dat[i], OLED_DAT);
				else
					OLED_Write_Byte(~Hzk12[index].dat[i], OLED_DAT);
			}

			OLED_Set_Pos(row + 1, column);
			for (i = 0; i < 12; i++)
			{
				if (flag == 0)
					OLED_Write_Byte(Hzk12[index].dat[i + 12], OLED_DAT);
				else
					OLED_Write_Byte(~Hzk12[index].dat[i + 12], OLED_DAT);
			}
		}
	}
}








/**
  * @brief  显示字体大小为6*12的单个汉字
  * @param  row     :  0~7行
  *		    column  :  0~127列
  *		    num	    : 数组的索引号
  * @retval None
  */
void OLED_Show_OneCharF6X12(uint8_t row, uint8_t column, uint8_t character)
{
	uint8_t i, index;
	index = character - ' ';
	if (column > X_WIDTH)
	{
		row = 0;
		column = column + 2;
	}
	OLED_Set_Pos(row, column);
	for (i = 0; i < 6; i++)
	{
		OLED_Write_Byte(Character_6X12[index][i], OLED_DAT);
	}
	OLED_Set_Pos(row + 1, column);

	for (i = 0; i < 6; i++)
	{
		OLED_Write_Byte(Character_6X12[index][i + 6], OLED_DAT);
	}
}





/**
  * @brief  显示字体大小为6*12的单个汉字
  * @param  row     :  0~7行
  *		    column  :  0~127列
  *		    num	    : 数组的索引号
  * @retval None
  */
void OLED_Show_MoreCharF6X12(uint8_t row, uint8_t column, uint8_t* String)
{
	uint8_t i = 0;
	while (String[i] != '\0')
	{
		OLED_Show_OneCharF6X12(row, column, String[i]);
		column += 6;
		if (column > 120)
		{
			column = 0;
			row += 2;
		}
		i++;
	}
}






 /**
   * @brief  显示字体大小为6*12的单个汉字
   * @param  row     :  0~7行
   *		 column  :  0~127列
   *		 num	 : 数组的索引号
   * @retval None
   */
void OLED_Show_ChineseF6X12(uint8_t row, uint8_t column, uint8_t num)
{
	uint8_t i, adder;
	adder = num;

	OLED_Set_Pos(row, column);
	for (i = 0; i < 12; i++)
		OLED_Write_Byte(Chinese_6X12[2 * adder][i], OLED_DAT);

	OLED_Set_Pos(row + 1, column);
	for (i = 0; i < 12; i++)
		OLED_Write_Byte(Chinese_6X12[2 * adder + 1][i], OLED_DAT);
}





 /**
   * @brief  显示字体大小为6*12的不在ASCSII上的字符
   * @param  row     :  0~7行
   *		 column  :  0~127列
   *		 num	 : 数组的索引号
   * @retval None	 
   */
void OLED_Show_OtherCharF6X12(uint8_t row, uint8_t column, uint8_t num)
{
	uint8_t i, adder;
	adder = num;

	OLED_Set_Pos(row, column);
	for (i = 0; i < 12; i++)
		OLED_Write_Byte(OtherChar_6X12[2 * adder][i], OLED_DAT);

	OLED_Set_Pos(row + 1, column);
	for (i = 0; i < 12; i++)
		OLED_Write_Byte(OtherChar_6X12[2 * adder + 1][i], OLED_DAT);
}









/**
  * @brief  显示字体大小为6*12的数字
  * @param  row		:  0~7行
  *			column  :  0~127列
  *			NumLen  :  数字长度
  * @retval None
  */
void OLED_ShowNumF6X12( uint8_t row, uint8_t column, uint8_t NumLen ,uint32_t Num)
{
	static uint8_t i = 0;
	static uint8_t empty = 0;
	static uint8_t enshow = 0;
	for (i = 0; i < NumLen; i++)
	{
		empty = (Num / OLED_Calculate_Pow(10, NumLen - 1 - i)) % 10;

		if ((enshow == 0) && (i < NumLen - 1))
		{
			if (empty == 0)
			{
				OLED_Show_OneCharF6X12(row, (column + i * 6), '0');
				continue;
			}
			else
			{
				enshow = 1;
			}
		}

		OLED_Show_OneCharF6X12(row, (column + i * 6), empty + '0');
	}
}





/****************************************************************************************************************************/
/* ---------------------------------------------- 显示 8*16 字体的显示驱动函数 ---------------------------------------------*/
/****************************************************************************************************************************/



 /**
   * @brief  显示字体大小为8*16的单个汉字
   * @param  row      :  0~7行
   *		 column   :  0~127列
   *		 flag     :  反白标志，非0时反白显示
   *		 *chinese :  字符指针
   * @retval 
   */
void ZoneBitCode_OLED_Show_ChineseF8X16(uint8_t row, uint8_t column, uint8_t* chinese, uint8_t flag)
{
	uint8_t  i = 0;
	uint32_t index;
	for (index = 0; index < sizeof(Hzk16) / 35; index++)      // name[3] + dat[32] ==35
	{
		if (Hzk16[index].name[0] == chinese[0] && Hzk16[index].name[1] == chinese[1])  // 对比汉字区码位码
		{
			OLED_Set_Pos(row, column);
			for (i = 0; i < 16; i++)
			{
				if (flag == 0)
					OLED_Write_Byte(Hzk16[index].dat[i], OLED_DAT);
				else
					OLED_Write_Byte(~Hzk16[index].dat[i], OLED_DAT);
			}

			OLED_Set_Pos(row + 1, column);
			for (i = 0; i < 16; i++)
			{
				if (flag == 0)
					OLED_Write_Byte(Hzk16[index].dat[i + 16], OLED_DAT);
				else
					OLED_Write_Byte(~Hzk16[index].dat[i + 16], OLED_DAT);
			}
		}
	}
}
























/****************************************************************************************************************************/
/* ---------------------------------------------- 画图驱动函数 -------------------------------------------------------------*/
/****************************************************************************************************************************/

 /**
   * @brief  显示显示BMP图片128×64起始点坐标(x,y),x的范围0～127，y为页的范围0～7
   * @param  row     :  0~7行
   *		 column  :  0~127列
   *		 num	 : 数组的索引号
   * @retval None
   */
void OLED_DrawBMP(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t* BMP)
{
	uint16_t j = 0;
	uint8_t x, y;

	if (y1 % 8 == 0)
	{
		y = y1 / 8;
	}
	else
	{
		y = (y1 / 8) + 1;;
	}
	for (y = y0; y < y1; y++)
	{
		OLED_Set_Pos(y, x0);
		for (x = x0; x < x1; x++)
		{
			OLED_Write_Byte(BMP[j++], OLED_DAT);
		}
	}
}























/****************************************************************************************************************************/
/* ---------------------------------------------- 初始化驱动函数 -----------------------------------------------------------*/
/****************************************************************************************************************************/


/**
  * @brief  OLED设置内存地址模式(或)
  * @param  None
  * @retval 水平地址模式:0x00
  *			垂直地址模式:0x01
  */
void OLED_set_memory_address_mode(uint8_t mode)
{
	OLED_Write_Byte(0x20, OLED_CMD);                                         //设置内存地址模式(使用水平地址模式或垂直地址模式)
	OLED_Write_Byte(mode, OLED_CMD);                                         //水平地址模式:0x00,垂直地址模式:0x01,页地址模式:0x02
	OLED_Write_Byte(0x21, OLED_CMD);                                         //设置显示位置列初始地址和终止地址
	OLED_Write_Byte(0x00, OLED_CMD);                                         //列初始地址
	OLED_Write_Byte(0x7F, OLED_CMD);                                         //列终止地址
	OLED_Write_Byte(0x22, OLED_CMD);                                         //设置显示位置页初始地址和终止地址
	OLED_Write_Byte(0x00, OLED_CMD);                                         //页初始地址
	OLED_Write_Byte(0x07, OLED_CMD);                                         //页终止地址
}





/**
  * @brief  OLED初始化函
  * @param  None
  * @retval None
  */
void OLED_Init(void)
{
	OLED_GPIO_Config();
	delay_ms(500);
	OLED_Write_Byte(0xAE, OLED_CMD);         //关闭显示
	OLED_Write_Byte(0x00, OLED_CMD);         //设置低列地址
	OLED_Write_Byte(0x10, OLED_CMD);         //设置高列地址
	OLED_Write_Byte(0x40, OLED_CMD);         //设置显示起始行

	OLED_Write_Byte(0xB0, OLED_CMD);         //设置页地址
	OLED_Write_Byte(0x81, OLED_CMD);         //对比度设置    该设置通过对电流的控制来控制对比度
	OLED_Write_Byte(0xFF, OLED_CMD);         //00H~FFH       用来调节对比度

	OLED_Write_Byte(0xA1, OLED_CMD);         //设置细分重映射(0xA1正常,0xA0左右反置)
	OLED_Write_Byte(0xA6, OLED_CMD);         //设置正常/反向(0xA6正常显示,0xA7反向显示)
	OLED_Write_Byte(0xA8, OLED_CMD);         //设置多路比率
	OLED_Write_Byte(0x3F, OLED_CMD);
	OLED_Write_Byte(0xC8, OLED_CMD);         //设置列输出扫描方向(0xC8正常,0xC0上下反置)
	OLED_Write_Byte(0xD3, OLED_CMD);         //设置显示偏移(垂直偏移)
	OLED_Write_Byte(0x00, OLED_CMD);
	OLED_Write_Byte(0xD5, OLED_CMD);         //设置显示时钟分频
	OLED_Write_Byte(0x80, OLED_CMD);
	OLED_Write_Byte(0xD9, OLED_CMD);         //设置预充电周期
	OLED_Write_Byte(0xF1, OLED_CMD);         //将预充电设置为15个时钟加上其放电设置为1个时钟
	OLED_Write_Byte(0xDA, OLED_CMD);         //设置COM口硬件配置
	OLED_Write_Byte(0x12, OLED_CMD);         //启用备用COM向导配置
	OLED_Write_Byte(0xDB, OLED_CMD);         //设置Vcomh取消选择等级
	OLED_Write_Byte(0x00, OLED_CMD);         //0.65 * VCC
	OLED_Write_Byte(0x8D, OLED_CMD);         //充电凹凸设置
	OLED_Write_Byte(0x14, OLED_CMD);         //在显示期间启用电浆泵
	OLED_Write_Byte(0xA4, OLED_CMD);         // entire display on
	OLED_Write_Byte(0xA6, OLED_CMD);         // set Normal display

	//设置初始的光标位置
	OLED_Set_Pos(0, 0);

	//设置内存地址模式(使用水平地址模式:0x00或垂直地址模式:0x01)
	OLED_set_memory_address_mode(0x00);

	OLED_Clr_Screen();
	OLED_Write_Byte(0xAF, OLED_CMD);
}

