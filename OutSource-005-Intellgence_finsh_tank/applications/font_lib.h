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

#endif /* __FONT_LIB_H__ */
