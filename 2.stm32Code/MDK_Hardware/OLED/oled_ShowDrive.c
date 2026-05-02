#include "oled_ShowDrive.h"
#include "oledchar_6X12.h"
#include "oledChinese_6X12.h"
#include "oledChinese_8X16.h"




void OLED_Full_OFF(void)
{
	uint8_t i, n;
	for (i = 0; i < 8; i++)
	{

		OLED_Write_Byte(0xb0 + i, OLED_CMD);   
		OLED_Write_Byte(0x00, OLED_CMD);      
		OLED_Write_Byte(0x01, OLED_CMD);       
		for (n = 0; n < X_WIDTH; n++)
			OLED_Write_Byte(0x00, OLED_DAT);
	}
}


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



void OLED_Set_Pos(uint8_t row, uint8_t column)
{
	OLED_Write_Byte(0xB0 + row, OLED_CMD);
	OLED_Write_Byte(((column & 0xF0) >> 4) | 0x10, OLED_CMD);
	OLED_Write_Byte((column & 0x0F) | 0x00, OLED_CMD);
}





uint32_t OLED_Calculate_Pow(uint8_t m, uint8_t n)
{
	uint32_t result = 1;

	while (n--)
	{
		result *= m;
	}
	return result;
}




void ZoneBitCode_OLED_Show_ChineseF6X12(uint8_t row, uint8_t column, uint8_t* chinese, uint8_t flag)
{
	uint8_t  i = 0;
	uint32_t index;
	for (index = 0; index < sizeof(Hzk12) / sizeof(Hzk12[0]); index++)
	{
		if (Hzk12[index].name[0] == chinese[0] && Hzk12[index].name[1] == chinese[1])  // �ԱȺ�������λ��
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
  * @brief  ��ʾ�����СΪ6*12������
  * @param  row		:  0~7��
  *			column  :  0~127��
  *			NumLen  :  ���ֳ���
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
/* ---------------------------------------------- ��ʾ 8*16 �������ʾ�������� ---------------------------------------------*/
/****************************************************************************************************************************/



 /**
   * @brief  ��ʾ�����СΪ8*16�ĵ�������
   * @param  row      :  0~7��
   *		 column   :  0~127��
   *		 flag     :  ���ױ�־����0ʱ������ʾ
   *		 *chinese :  �ַ�ָ��
   * @retval 
   */
void ZoneBitCode_OLED_Show_ChineseF8X16(uint8_t row, uint8_t column, uint8_t* chinese, uint8_t flag)
{
	uint8_t  i = 0;
	uint32_t index;
	for (index = 0; index < sizeof(Hzk16) / sizeof(Hzk16[0]); index++)
	{
		if (Hzk16[index].name[0] == chinese[0] && Hzk16[index].name[1] == chinese[1])  // �ԱȺ�������λ��
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
/* ---------------------------------------------- ��ͼ�������� -------------------------------------------------------------*/
/****************************************************************************************************************************/

 /**
   * @brief  ��ʾ��ʾBMPͼƬ128��64��ʼ������(x,y),x�ķ�Χ0��127��yΪҳ�ķ�Χ0��7
   * @param  row     :  0~7��
   *		 column  :  0~127��
   *		 num	 : �����������
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
/* ---------------------------------------------- ��ʼ���������� -----------------------------------------------------------*/
/****************************************************************************************************************************/


/**
  * @brief  OLED�����ڴ��ַģʽ(��)
  * @param  None
  * @retval ˮƽ��ַģʽ:0x00
  *			��ֱ��ַģʽ:0x01
  */
void OLED_set_memory_address_mode(uint8_t mode)
{
	OLED_Write_Byte(0x20, OLED_CMD);                                         //�����ڴ��ַģʽ(ʹ��ˮƽ��ַģʽ��ֱ��ַģʽ)
	OLED_Write_Byte(mode, OLED_CMD);                                         //ˮƽ��ַģʽ:0x00,��ֱ��ַģʽ:0x01,ҳ��ַģʽ:0x02
	OLED_Write_Byte(0x21, OLED_CMD);                                         //������ʾλ���г�ʼ��ַ����ֹ��ַ
	OLED_Write_Byte(0x00, OLED_CMD);                                         //�г�ʼ��ַ
	OLED_Write_Byte(0x7F, OLED_CMD);                                         //����ֹ��ַ
	OLED_Write_Byte(0x22, OLED_CMD);                                         //������ʾλ��ҳ��ʼ��ַ����ֹ��ַ
	OLED_Write_Byte(0x00, OLED_CMD);                                         //ҳ��ʼ��ַ
	OLED_Write_Byte(0x07, OLED_CMD);                                         //ҳ��ֹ��ַ
}





/**
  * @brief  OLED��ʼ����
  * @param  None
  * @retval None
  */
void OLED_Init(void)
{
	OLED_GPIO_Config();
	delay_ms(500);
	OLED_Write_Byte(0xAE, OLED_CMD);         //�ر���ʾ
	OLED_Write_Byte(0x00, OLED_CMD);         //���õ��е�ַ
	OLED_Write_Byte(0x10, OLED_CMD);         //���ø��е�ַ
	OLED_Write_Byte(0x40, OLED_CMD);         //������ʾ��ʼ��

	OLED_Write_Byte(0xB0, OLED_CMD);         //����ҳ��ַ
	OLED_Write_Byte(0x81, OLED_CMD);         //�Աȶ�����    ������ͨ���Ե����Ŀ��������ƶԱȶ�
	OLED_Write_Byte(0xFF, OLED_CMD);         //00H~FFH       �������ڶԱȶ�

	OLED_Write_Byte(0xA1, OLED_CMD);         //����ϸ����ӳ��(0xA1����,0xA0���ҷ���)
	OLED_Write_Byte(0xA6, OLED_CMD);         //��������/����(0xA6������ʾ,0xA7������ʾ)
	OLED_Write_Byte(0xA8, OLED_CMD);         //���ö�·����
	OLED_Write_Byte(0x3F, OLED_CMD);
	OLED_Write_Byte(0xC8, OLED_CMD);         //���������ɨ�跽��(0xC8����,0xC0���·���)
	OLED_Write_Byte(0xD3, OLED_CMD);         //������ʾƫ��(��ֱƫ��)
	OLED_Write_Byte(0x00, OLED_CMD);
	OLED_Write_Byte(0xD5, OLED_CMD);         //������ʾʱ�ӷ�Ƶ
	OLED_Write_Byte(0x80, OLED_CMD);
	OLED_Write_Byte(0xD9, OLED_CMD);         //����Ԥ�������
	OLED_Write_Byte(0xF1, OLED_CMD);         //��Ԥ�������Ϊ15��ʱ�Ӽ�����ŵ�����Ϊ1��ʱ��
	OLED_Write_Byte(0xDA, OLED_CMD);         //����COM��Ӳ������
	OLED_Write_Byte(0x12, OLED_CMD);         //���ñ���COM������
	OLED_Write_Byte(0xDB, OLED_CMD);         //����Vcomhȡ��ѡ��ȼ�
	OLED_Write_Byte(0x00, OLED_CMD);         //0.65 * VCC
	OLED_Write_Byte(0x8D, OLED_CMD);         //��簼͹����
	OLED_Write_Byte(0x14, OLED_CMD);         //����ʾ�ڼ����õ罬��
	OLED_Write_Byte(0xA4, OLED_CMD);         // entire display on
	OLED_Write_Byte(0xA6, OLED_CMD);         // set Normal display

	//���ó�ʼ�Ĺ��λ��
	OLED_Set_Pos(0, 0);

	//�����ڴ��ַģʽ(ʹ��ˮƽ��ַģʽ:0x00��ֱ��ַģʽ:0x01)
	OLED_set_memory_address_mode(0x00);

	OLED_Clr_Screen();
	OLED_Write_Byte(0xAF, OLED_CMD);
}

