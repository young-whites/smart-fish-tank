#ifndef __USART_USE1_H
#define __USART_USE1_H
#include "sys.h"

/* ======================== USART1 Debug Configuration ======================== */
/* PA9  -> USART1_TX (AF_PP)
 * PA10 -> USART1_RX (IN_FLOATING)
 * Baud: 115200, 8N1, no interrupt (TX-only debug)
 */

#define DEBUG_USART_1                   USART1
#define DEBUG_USART1_CLK                RCC_APB2Periph_USART1
#define DEBUG_USART1_APBxClkCmd         RCC_APB2PeriphClockCmd
#define DEBUG_USART1_BAUDRATE           115200

#define DEBUG_USART1_GPIO_CLK           (RCC_APB2Periph_GPIOA)
#define DEBUG_USART1_GPIO_APBxClkCmd    RCC_APB2PeriphClockCmd

#define DEBUG_USART1_TX_GPIO_PORT       GPIOA
#define DEBUG_USART1_TX_GPIO_PIN        GPIO_Pin_9
#define DEBUG_USART1_RX_GPIO_PORT       GPIOA
#define DEBUG_USART1_RX_GPIO_PIN        GPIO_Pin_10

/* ======================== Function Declarations ======================== */
void Debug_USART1_Init(void);
void Usart_SendByte(USART_TypeDef* pUSARTx, uint8_t ch);
void Usart_SendString(USART_TypeDef* pUSARTx, char* str);
void Usart_SendHalfWord(USART_TypeDef* pUSARTx, uint16_t ch);

#endif /* __USART_USE1_H */
