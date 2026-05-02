#include "bsp_relay.h"

/* 继电器引脚定义 */
#define RELAY_DRAIN_PORT    GPIOB
#define RELAY_DRAIN_PIN     GPIO_Pin_15

#define RELAY_OXYGEN_PORT   GPIOA
#define RELAY_OXYGEN_PIN    GPIO_Pin_15

#define RELAY_FILL_PORT     GPIOB
#define RELAY_FILL_PIN      GPIO_Pin_14

#define RELAY_HEAT_PORT     GPIOB
#define RELAY_HEAT_PIN      GPIO_Pin_12

/**
 * @brief  继电器IO初始化，全部关闭
 */
void Relay_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* 使能时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);

    /* PB15, PB14, PB12 配置为推挽输出 */
    GPIO_InitStructure.GPIO_Pin = RELAY_DRAIN_PIN | RELAY_FILL_PIN | RELAY_HEAT_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* PA15 需要先禁用JTAG（已在main.c中配置过，这里确保PA15可用） */
    GPIO_InitStructure.GPIO_Pin = RELAY_OXYGEN_PIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* 初始全部关闭（低电平） */
    GPIO_ResetBits(RELAY_DRAIN_PORT, RELAY_DRAIN_PIN);
    GPIO_ResetBits(RELAY_OXYGEN_PORT, RELAY_OXYGEN_PIN);
    GPIO_ResetBits(RELAY_FILL_PORT, RELAY_FILL_PIN);
    GPIO_ResetBits(RELAY_HEAT_PORT, RELAY_HEAT_PIN);
}

/**
 * @brief  控制单个继电器
 * @param  relay: RELAY_DRAIN/RELAY_OXYGEN/RELAY_FILL/RELAY_HEAT
 * @param  state: RELAY_ON(1) 开 / RELAY_OFF(0) 关
 */
void Relay_Control(uint8_t relay, uint8_t state)
{
    switch (relay)
    {
        case RELAY_DRAIN:
            if (state) GPIO_SetBits(RELAY_DRAIN_PORT, RELAY_DRAIN_PIN);
            else       GPIO_ResetBits(RELAY_DRAIN_PORT, RELAY_DRAIN_PIN);
            break;
        case RELAY_OXYGEN:
            if (state) GPIO_SetBits(RELAY_OXYGEN_PORT, RELAY_OXYGEN_PIN);
            else       GPIO_ResetBits(RELAY_OXYGEN_PORT, RELAY_OXYGEN_PIN);
            break;
        case RELAY_FILL:
            if (state) GPIO_SetBits(RELAY_FILL_PORT, RELAY_FILL_PIN);
            else       GPIO_ResetBits(RELAY_FILL_PORT, RELAY_FILL_PIN);
            break;
        case RELAY_HEAT:
            if (state) GPIO_SetBits(RELAY_HEAT_PORT, RELAY_HEAT_PIN);
            else       GPIO_ResetBits(RELAY_HEAT_PORT, RELAY_HEAT_PIN);
            break;
        default:
            break;
    }
}

/**
 * @brief  全部继电器关闭
 */
void Relay_AllOff(void)
{
    uint8_t i;
    for (i = 0; i < RELAY_NUM; i++)
    {
        Relay_Control(i, RELAY_OFF);
    }
}

/**
 * @brief  获取继电器状态
 * @param  relay: RELAY_DRAIN/RELAY_OXYGEN/RELAY_FILL/RELAY_HEAT
 * @retval 1=开, 0=关
 */
uint8_t Relay_GetState(uint8_t relay)
{
    switch (relay)
    {
        case RELAY_DRAIN:
            return (GPIO_ReadOutputDataBit(RELAY_DRAIN_PORT, RELAY_DRAIN_PIN) != 0);
        case RELAY_OXYGEN:
            return (GPIO_ReadOutputDataBit(RELAY_OXYGEN_PORT, RELAY_OXYGEN_PIN) != 0);
        case RELAY_FILL:
            return (GPIO_ReadOutputDataBit(RELAY_FILL_PORT, RELAY_FILL_PIN) != 0);
        case RELAY_HEAT:
            return (GPIO_ReadOutputDataBit(RELAY_HEAT_PORT, RELAY_HEAT_PIN) != 0);
        default:
            return 0;
    }
}
