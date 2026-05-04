#ifndef __OLED_SHOW_H
#define __OLED_SHOW_H
#include "sys.h"


void OLED_ShowStart(void);
void OLED_Show_Main(void);
void OLED_Show_Status(void);
void OLED_Show_Threshold(void);
void OLED_Show_Page(uint8_t page);
void OLED_ShowNumF6X12_Float(uint8_t row, uint8_t column, float num, uint8_t decimal);


#endif /*__OLED_SHOW_H*/
