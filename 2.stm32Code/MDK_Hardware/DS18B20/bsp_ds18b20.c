#include "bsp_ds18b20.h"

/* 引脚配置为输出模式 */
static void DS18B20_Mode_Out(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = DS18B20_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DS18B20_GPIO_PORT, &GPIO_InitStructure);
}

/* 引脚配置为输入模式 */
static void DS18B20_Mode_In(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = DS18B20_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DS18B20_GPIO_PORT, &GPIO_InitStructure);
}

/**
 * @brief  DS18B20 初始化
 */
void DS18B20_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(DS18B20_RCC_APB2Periph_GPIOX, ENABLE);

    GPIO_InitStructure.GPIO_Pin = DS18B20_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DS18B20_GPIO_PORT, &GPIO_InitStructure);

    DS18B20_DQ_OUT_H();
}

/**
 * @brief  复位DS18B20，检测存在脉冲
 * @retval 0=成功, 1=失败
 */
static uint8_t DS18B20_Reset(void)
{
    uint8_t retry = 0;

    DS18B20_Mode_Out();
    DS18B20_DQ_OUT_L();
    delay_us(480);      /* 拉低480us */
    DS18B20_DQ_OUT_H();
    delay_us(60);        /* 释放后等待60us */

    DS18B20_Mode_In();
    /* 等待DS18B20拉低（存在脉冲） */
    while (DS18B20_DQ_IN() && retry < 200)
    {
        retry++;
        delay_us(1);
    }
    if (retry >= 200) return 1;

    /* 等待存在脉冲结束 */
    retry = 0;
    while (!DS18B20_DQ_IN() && retry < 240)
    {
        retry++;
        delay_us(1);
    }
    if (retry >= 240) return 1;

    return 0;
}

/**
 * @brief  写一个位
 */
static void DS18B20_WriteBit(uint8_t bit)
{
    DS18B20_Mode_Out();
    if (bit)
    {
        DS18B20_DQ_OUT_L();
        delay_us(2);
        DS18B20_DQ_OUT_H();
        delay_us(60);
    }
    else
    {
        DS18B20_DQ_OUT_L();
        delay_us(60);
        DS18B20_DQ_OUT_H();
        delay_us(2);
    }
}

/**
 * @brief  读一个位
 */
static uint8_t DS18B20_ReadBit(void)
{
    uint8_t bit = 0;

    DS18B20_Mode_Out();
    DS18B20_DQ_OUT_L();
    delay_us(2);
    DS18B20_DQ_OUT_H();

    DS18B20_Mode_In();
    delay_us(10);
    if (DS18B20_DQ_IN()) bit = 1;
    delay_us(50);

    return bit;
}

/**
 * @brief  写一个字节
 */
static void DS18B20_WriteByte(uint8_t dat)
{
    uint8_t i;
    for (i = 0; i < 8; i++)
    {
        DS18B20_WriteBit(dat & 0x01);
        dat >>= 1;
    }
}

/**
 * @brief  读一个字节
 */
static uint8_t DS18B20_ReadByte(void)
{
    uint8_t i, dat = 0;
    for (i = 0; i < 8; i++)
    {
        dat >>= 1;
        if (DS18B20_ReadBit())
            dat |= 0x80;
    }
    return dat;
}

/**
 * @brief  读取温度值
 * @retval 温度值（摄氏度），读取失败返回 -100.0f
 */
/**
 * @brief  启动温度转换 (非阻塞)
 *         调用后需等待 >=750ms 再调用 DS18B20_ReadTemp
 */
void DS18B20_StartConvert(void)
{
    if (DS18B20_Reset()) return;
    DS18B20_WriteByte(0xCC);  /* 跳过ROM */
    DS18B20_WriteByte(0x44);  /* 启动转换 */
}

/**
 * @brief  读取温度值 (非阻塞, 无延时)
 *         调用前需保证距 DS18B20_StartConvert 已过 >=750ms
 * @retval 温度值(°C), 读取失败返回 -100.0f
 */
float DS18B20_ReadTemp(void)
{
    uint8_t temp_l, temp_h;
    int16_t temp_raw;

    if (DS18B20_Reset())
        return -100.0f;

    DS18B20_WriteByte(0xCC);  /* 跳过ROM */
    DS18B20_WriteByte(0xBE);  /* 读暂存器 */

    temp_l = DS18B20_ReadByte();
    temp_h = DS18B20_ReadByte();

    temp_raw = (int16_t)((temp_h << 8) | temp_l);
    return (float)temp_raw * 0.0625f;
}
