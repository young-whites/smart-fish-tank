#ifndef __OLED_SHOW_H
#define __OLED_SHOW_H
#include "sys.h"


void OLED_ShowStart(void);
void OLED_ShowMainPage(void);
void OLED_ShowAutoSetPage(void);
void OLED_ShowManualPage(void);

/* 显示工具函数 */
void OLED_ShowString(uint8_t row, uint8_t col, const char *str);
void OLED_ShowFloat(uint8_t row, uint8_t col, float val, uint8_t decimal);
void OLED_ShowInt(uint8_t row, uint8_t col, int val);
void OLED_ShowCN(uint8_t row, uint8_t col, const char *cn, uint8_t inv);


#endif /*__OLED_SHOW_H*/
