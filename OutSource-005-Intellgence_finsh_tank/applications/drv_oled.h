/*
 * Copyright (c) 2026, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2026-05-02     Mr.Hu        OLED SSD1306 软件 I2C 驱动 (PA11=SDA, PA12=SCL)
 */

#ifndef APPLICATIONS_DRV_OLED_H_
#define APPLICATIONS_DRV_OLED_H_

#include <rtthread.h>
#include "stm32f1xx_hal.h"

/* ===================== 引脚定义 ===================== */
#define OLED_SCL_PORT       GPIOA
#define OLED_SCL_PIN        GPIO_PIN_12
#define OLED_SDA_PORT       GPIOA
#define OLED_SDA_PIN        GPIO_PIN_11

/* ===================== I2C 地址 ===================== */
#define OLED_I2C_ADDR       0x78    /* SSD1306 写地址 (0x3C << 1) */

/* ===================== 屏幕参数 ===================== */
#define OLED_WIDTH          128
#define OLED_HEIGHT         64
#define OLED_PAGES          8       /* 64 / 8 */

/* ===================== 函数声明 ===================== */

/* 底层初始化 */
void oled_gpio_init(void);

/* I2C 操作 */
void oled_i2c_start(void);
void oled_i2c_stop(void);
void oled_i2c_ack(void);
void oled_i2c_send_byte(uint8_t data);

/* 命令/数据发送 */
void oled_write_cmd(uint8_t cmd);
void oled_write_data(uint8_t data);

/* 驱动初始化 */
void oled_init(void);

/* 显示控制 */
void oled_clear(void);
void oled_fill(uint8_t pattern);
void oled_set_pos(uint8_t x, uint8_t page);
void oled_display_on(void);
void oled_display_off(void);

/* 基本绘图 */
void oled_draw_pixel(uint8_t x, uint8_t y, uint8_t on);
void oled_draw_char(uint8_t x, uint8_t page, char ch);
void oled_draw_string(uint8_t x, uint8_t page, const char *str);
void oled_draw_cn_char(uint8_t x, uint8_t page, const uint8_t *ch);
void oled_draw_number(uint8_t x, uint8_t page, int32_t num);
void oled_draw_float(uint8_t x, uint8_t page, float num, uint8_t decimal);

/* 缓冲区操作（提高刷新效率） */
void oled_refresh(void);

#endif /* APPLICATIONS_DRV_OLED_H_ */
