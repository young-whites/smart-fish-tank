/**
 * @file    font_lib.h
 * @brief   16x16 汉字点阵字体驱动
 * @note    横向扫描, 字节高位在前, 每字符 32 字节
 */

#ifndef __FONT_LIB_H__
#define __FONT_LIB_H__

#include <stdint.h>

/* 字体参数 */
#define FONT_WIDTH          16
#define FONT_HEIGHT         16
#define FONT_BYTES_PER_CHAR 32   /* 16行 * 2字节/行 */
#define FONT_CHAR_COUNT     32   /* 字库中汉字数量 */

/**
 * @brief  根据 Unicode 码点获取点阵数据
 * @param  unicode  UTF-16/Unicode 码点
 * @retval 32 字节点阵数据指针, 未找到返回 NULL
 */
const unsigned char* font_get_bitmap(uint16_t unicode);

/**
 * @brief  检查字符是否在字库中
 * @param  unicode  Unicode 码点
 * @retval 1=存在, 0=不存在
 */
int font_char_exists(uint16_t unicode);

/**
 * @brief  在 LCD 指定位置绘制单个汉字
 * @param  x, y       左上角像素坐标
 * @param  unicode    Unicode 码点
 * @param  color      前景色
 * @param  bg_color   背景色 (0xFFFF 表示透明背景, 不画背景)
 */
void font_draw_char(uint16_t x, uint16_t y, uint16_t unicode,
                    uint16_t color, uint16_t bg_color);

/**
 * @brief  在 LCD 指定位置绘制 UTF-8 字符串
 * @param  x, y       起始像素坐标
 * @param  utf8_str   UTF-8 编码字符串
 * @param  color      前景色
 * @param  bg_color   背景色
 * @note   字库中不存在的字符会被跳过(不占宽度)
 *         ASCII 字符不在本字库范围内, 请使用其他 ASCII 字体
 */
void font_draw_string(uint16_t x, uint16_t y, const char* utf8_str,
                      uint16_t color, uint16_t bg_color);

#endif /* __FONT_LIB_H__ */
