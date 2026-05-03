/*
 * OLED SSD1306 软件 I2C 驱动头文件 (PA11=SDA, PA12=SCL)
 * 纯 CMSIS，零 HAL 依赖
 */

#ifndef APPLICATIONS_DRV_OLED_H_
#define APPLICATIONS_DRV_OLED_H_

#include <rtthread.h>
#include <stdint.h>

/* ===================== I2C 地址 ===================== */
#define OLED_I2C_ADDR       0x78

/* ===================== 屏幕参数 ===================== */
#define OLED_WIDTH          128
#define OLED_HEIGHT         64
#define OLED_PAGES          8

/* ===================== 函数声明 ===================== */

void oled_gpio_init(void);

void oled_i2c_start(void);
void oled_i2c_stop(void);
void oled_i2c_ack(void);
void oled_i2c_send_byte(uint8_t data);

void oled_write_cmd(uint8_t cmd);
void oled_write_data(uint8_t data);

void oled_init(void);

void oled_clear(void);
void oled_clear_region(uint8_t page_start, uint8_t page_end, uint8_t col_start, uint8_t col_end);
void oled_fill(uint8_t pattern);
void oled_set_pos(uint8_t x, uint8_t page);
void oled_display_on(void);
void oled_display_off(void);

void oled_draw_pixel(uint8_t x, uint8_t y, uint8_t on);
void oled_draw_char(uint8_t x, uint8_t page, char ch);
void oled_draw_char_offset(uint8_t x, uint8_t page, char ch, uint8_t y_offset);
void oled_draw_string(uint8_t x, uint8_t page, const char *str);

void oled_draw_number(uint8_t x, uint8_t page, int32_t num);
void oled_draw_float(uint8_t x, uint8_t page, float num, uint8_t decimal);

/* 16x16 汉字绘制 (仅主页使用) */
void oled_draw_chinese_char(uint8_t x, uint8_t page, uint16_t unicode);

/* 混合行: 汉字标签(16px高) + ASCII值(6x12)
 * chinese_label: 汉字标签 UTF-8 字符串
 * ascii_text:    ASCII 文本 (数字/英文/符号)
 * page:          起始页 (0/2/4/6)
 */
void oled_draw_mix_line(uint8_t page, const char *chinese_label, const char *ascii_text);

void oled_refresh(void);

#endif /* APPLICATIONS_DRV_OLED_H_ */
