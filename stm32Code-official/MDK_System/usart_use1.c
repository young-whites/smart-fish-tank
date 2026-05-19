#include "usart_use1.h"
#include <stdio.h>

/* ======================== Debug USART1 Init ======================== */
/* PA9: TX - AF_PP, PA10: RX - IN_FLOATING
 * USART1 115200 8N1, no interrupt (pure TX debug output)
 */
void Debug_USART1_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	/* Enable clocks for GPIOA and USART1 */
	DEBUG_USART1_GPIO_APBxClkCmd(DEBUG_USART1_GPIO_CLK, ENABLE);
	DEBUG_USART1_APBxClkCmd(DEBUG_USART1_CLK, ENABLE);

	/* PA9: TX - Alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin   = DEBUG_USART1_TX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DEBUG_USART1_TX_GPIO_PORT, &GPIO_InitStructure);

	/* PA10: RX - Floating input */
	GPIO_InitStructure.GPIO_Pin  = DEBUG_USART1_RX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(DEBUG_USART1_RX_GPIO_PORT, &GPIO_InitStructure);

	/* USART1 configuration: 115200 8N1 */
	USART_InitStructure.USART_BaudRate            = DEBUG_USART1_BAUDRATE;
	USART_InitStructure.USART_WordLength          = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits            = USART_StopBits_1;
	USART_InitStructure.USART_Parity              = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(DEBUG_USART_1, &USART_InitStructure);

	USART_Cmd(DEBUG_USART_1, ENABLE);
}

/* ======================== printf Retarget (Keil ARM Compiler) ======================== */
struct __FILE { int handle; };
FILE __stdout;

int fputc(int ch, FILE *f)
{
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	USART_SendData(USART1, (uint16_t)ch);
	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
	return ch;
}

/* ======================== USART Helper Functions ======================== */

void Usart_SendByte(USART_TypeDef* pUSARTx, uint8_t ch)
{
	USART_SendData(pUSARTx, ch);
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
}

void Usart_SendString(USART_TypeDef* pUSARTx, char* str)
{
	unsigned int k = 0;
	do
	{
		Usart_SendByte(pUSARTx, *(str + k));
		k++;
	} while (*(str + k) != '\0');

	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TC) == RESET);
}

void Usart_SendHalfWord(USART_TypeDef* pUSARTx, uint16_t ch)
{
	uint8_t temp_h, temp_l;

	temp_h = (ch & 0XFF00) >> 8;
	temp_l = ch & 0XFF;

	USART_SendData(pUSARTx, temp_h);
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);

	USART_SendData(pUSARTx, temp_l);
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
}
