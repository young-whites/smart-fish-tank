#ifndef __BSP_OLED_H
#define __BSP_OLED_H
#include "sys.h"





//*************  宏定义参数  *****************
#define X_WIDTH     128     // 水平坐标最大宽度
#define Y_HEIGHT    64      // 垂直坐标最大宽度
#define OLED_CMD    0       // OLED 发送命令指令
#define OLED_DAT    1       // OLED 发送数据指令













void OLED_Write_Byte(uint8_t byt, uint8_t cmdType);
void OLED_Clr_Screen(void);
void OLED_Set_Pos(uint8_t row, uint8_t column);
uint32_t OLED_Calculate_Pow(uint8_t m, uint8_t n);
void OLED_DrawBMP(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t* BMP);
void OLED_Show_OneCharF6X12(uint8_t row, uint8_t column, uint8_t character);
void OLED_Show_MoreCharF6X12(uint8_t row, uint8_t column, uint8_t* String);
void OLED_ShowNumF6X12(uint8_t row, uint8_t column, uint8_t NumLen, uint32_t Num);
void ZoneBitCode_OLED_Show_ChineseF8X16(uint8_t row, uint8_t column, uint8_t* chinese, uint8_t flag);
void ZoneBitCode_OLED_Show_ChineseF6X12(uint8_t row, uint8_t column, uint8_t* chinese, uint8_t flag);
void OLED_set_memory_address_mode(uint8_t mode);
void OLED_Init(void);









#endif // !__BSP_OLED_H
