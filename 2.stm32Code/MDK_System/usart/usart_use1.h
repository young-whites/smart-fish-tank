#ifndef __USART_USE1_H
#define __USART_USE1_H
#include "sys.h"


#define FireUSART3_ENABLE				0
#define FireUSART1_ENABLE				1


/* ESP-01S 使用 USART2 (PA2=RX, PA3=TX), 波特率 115200 */
#if FireUSART1_ENABLE

#define  DEBUG_USART_1                   USART2
#define  DEBUG_USART1_CLK                RCC_APB1Periph_USART2
#define  DEBUG_USART1_APBxClkCmd         RCC_APB1PeriphClockCmd
#define  DEBUG_USART1_BAUDRATE           115200

#define  DEBUG_USART1_GPIO_CLK           (RCC_APB2Periph_GPIOA)
#define  DEBUG_USART1_GPIO_APBxClkCmd    RCC_APB2PeriphClockCmd

#define  DEBUG_USART1_TX_GPIO_PORT       GPIOA
#define  DEBUG_USART1_TX_GPIO_PIN        GPIO_Pin_2
#define  DEBUG_USART1_RX_GPIO_PORT       GPIOA
#define  DEBUG_USART1_RX_GPIO_PIN        GPIO_Pin_3

#define  DEBUG_USART1_IRQ                USART2_IRQn
#define  DEBUG_USART1_IRQHandler         USART2_IRQHandler


void FireUSART_1_Config(void);
void ESP01S_RxByte(uint8_t byte);  /* ESP-01S 接收回调 */


#endif /*FireUSART1_ENABLE*/




#if FireUSART3_ENABLE

#define  DEBUG_USART_3                   USART3
#define  DEBUG_USART3_CLK                RCC_APB1Periph_USART3
#define  DEBUG_USART3_APBxClkCmd         RCC_APB1PeriphClockCmd
#define  DEBUG_USART3_BAUDRATE           9600

#define  DEBUG_USART3_GPIO_CLK           (RCC_APB2Periph_GPIOB)
#define  DEBUG_USART3_GPIO_APBxClkCmd    RCC_APB2PeriphClockCmd

#define  DEBUG_USART3_TX_GPIO_PORT       GPIOB
#define  DEBUG_USART3_TX_GPIO_PIN        GPIO_Pin_10
#define  DEBUG_USART3_RX_GPIO_PORT       GPIOB
#define  DEBUG_USART3_RX_GPIO_PIN        GPIO_Pin_11

#define  DEBUG_USART3_IRQ                USART3_IRQn
#define  DEBUG_USART3_IRQHandler         USART3_IRQHandler


void FireUSART_3_Config(void);


#endif /*FireUSART3_ENABLE*/


void Usart_SendByte(USART_TypeDef* pUSARTx, uint8_t ch);
void Usart_SendString(USART_TypeDef* pUSARTx, char* str);
void Usart_SendHalfWord(USART_TypeDef* pUSARTx, uint16_t ch);


#endif /*__USART_USE1_H*/
