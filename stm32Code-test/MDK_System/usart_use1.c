#include "usart_use1.h"




#if FireUSART3_ENABLE
/**
 * @brief  ����Ƕ�������жϿ�����NVIC
 * @param  ��
 * @retval ��
 */
static void FireUSART3_NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Ƕ�������жϿ�������ѡ�� */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

	/* ����USARTΪ�ж�Դ */
	NVIC_InitStructure.NVIC_IRQChannel = DEBUG_USART3_IRQ;
	/* �������ȼ�*/
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;
	/* �����ȼ� */
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	/* ʹ���ж� */
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	/* ��ʼ������NVIC */
	NVIC_Init(&NVIC_InitStructure);
}

/**
 * @brief  USART GPIO ����,������������
 * @param  ��
 * @retval ��
 */
void FireUSART_3_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	// �򿪴���GPIO��ʱ��
	DEBUG_USART3_GPIO_APBxClkCmd(DEBUG_USART3_GPIO_CLK, ENABLE);

	// �򿪴��������ʱ��
	DEBUG_USART3_APBxClkCmd(DEBUG_USART3_CLK, ENABLE);

	// ��USART Tx��GPIO����Ϊ���츴��ģʽ
	GPIO_InitStructure.GPIO_Pin = DEBUG_USART3_TX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DEBUG_USART3_TX_GPIO_PORT, &GPIO_InitStructure);

	// ��USART Rx��GPIO����Ϊ��������ģʽ
	GPIO_InitStructure.GPIO_Pin = DEBUG_USART3_RX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(DEBUG_USART3_RX_GPIO_PORT, &GPIO_InitStructure);

	// ���ô��ڵĹ�������
	// ���ò�����
	USART_InitStructure.USART_BaudRate = DEBUG_USART3_BAUDRATE;
	// ���� �������ֳ�
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	// ����ֹͣλ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	// ����У��λ
	USART_InitStructure.USART_Parity = USART_Parity_No;
	// ����Ӳ��������
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	// ���ù���ģʽ���շ�һ��
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	// ��ɴ��ڵĳ�ʼ������
	USART_Init(DEBUG_USART_3, &USART_InitStructure);

	// �����ж����ȼ�����
	FireUSART3_NVIC_Configuration();

	// ʹ�ܴ��ڽ����ж�
	USART_ITConfig(DEBUG_USART_3, USART_IT_RXNE, ENABLE);

	// ʹ�ܴ���
	USART_Cmd(DEBUG_USART_3, ENABLE);

}



// �����жϷ�����
void DEBUG_USART3_IRQHandler(void)
{
	uint8_t ucTemp;
	if (USART_GetITStatus(DEBUG_USART_3, USART_IT_RXNE) != RESET)
	{
		ucTemp = USART_ReceiveData(DEBUG_USART_3);
		Tk1300F_Calculate(ucTemp);

	}
}


#endif /*FireUSART3_ENABLE*/











#if FireUSART1_ENABLE
/**
 * @brief  ����Ƕ�������жϿ�����NVIC
 * @param  ��
 * @retval ��
 */
static void FireUSART1_NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Ƕ�������жϿ�������ѡ�� */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

	/* ����USARTΪ�ж�Դ */
	NVIC_InitStructure.NVIC_IRQChannel = DEBUG_USART1_IRQ;
	/* �������ȼ�*/
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;
	/* �����ȼ� */
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	/* ʹ���ж� */
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	/* ��ʼ������NVIC */
	NVIC_Init(&NVIC_InitStructure);
}

/**
 * @brief  USART GPIO ����,������������
 * @param  ��
 * @retval ��
 */
void FireUSART_1_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	// �򿪴���GPIO��ʱ��
	DEBUG_USART1_GPIO_APBxClkCmd(DEBUG_USART1_GPIO_CLK, ENABLE);

	// �򿪴��������ʱ��
	DEBUG_USART1_APBxClkCmd(DEBUG_USART1_CLK, ENABLE);

	// ��USART Tx��GPIO����Ϊ���츴��ģʽ
	GPIO_InitStructure.GPIO_Pin = DEBUG_USART1_TX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DEBUG_USART1_TX_GPIO_PORT, &GPIO_InitStructure);

	// ��USART Rx��GPIO����Ϊ��������ģʽ
	GPIO_InitStructure.GPIO_Pin = DEBUG_USART1_RX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(DEBUG_USART1_RX_GPIO_PORT, &GPIO_InitStructure);

	// ���ô��ڵĹ�������
	// ���ò�����
	USART_InitStructure.USART_BaudRate = DEBUG_USART1_BAUDRATE;
	// ���� �������ֳ�
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	// ����ֹͣλ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	// ����У��λ
	USART_InitStructure.USART_Parity = USART_Parity_No;
	// ����Ӳ��������
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	// ���ù���ģʽ���շ�һ��
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	// ��ɴ��ڵĳ�ʼ������
	USART_Init(DEBUG_USART_1, &USART_InitStructure);

	// �����ж����ȼ�����
	FireUSART1_NVIC_Configuration();

	// ʹ�ܴ��ڽ����ж�
	USART_ITConfig(DEBUG_USART_1, USART_IT_RXNE, ENABLE);

	// ʹ�ܴ���
	USART_Cmd(DEBUG_USART_1, ENABLE);

}



uint8_t PressCnt = 0;
uint8_t PressBuf[5];
/* TK1300F传感器已移除，此函数保留为空，供后续鱼缸传感器数据解析使用 */
void Tk1300F_Calculate(unsigned  char data)
{
	(void)data;  // 避免未使用参数警告
}


// �����жϷ�����
void DEBUG_USART1_IRQHandler(void)
{
	uint8_t ucTemp;
	if (USART_GetITStatus(DEBUG_USART_1, USART_IT_RXNE) != RESET)
	{
		ucTemp = USART_ReceiveData(DEBUG_USART_1);
		Tk1300F_Calculate(ucTemp);
	}
}


#endif /*FireUSART3_ENABLE*/


















































/****************************************************************************************************************************************************/
/*                                                              �����������                                                                        */
/****************************************************************************************************************************************************/



/*****************  ����һ���ַ� **********************/
void Usart_SendByte(USART_TypeDef* pUSARTx, uint8_t ch)
{
	/* ����һ���ֽ����ݵ�USART */
	USART_SendData(pUSARTx, ch);

	/* �ȴ��������ݼĴ���Ϊ�� */
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
}

/*****************  �����ַ��� **********************/
void Usart_SendString(USART_TypeDef* pUSARTx, char* str)
{
	unsigned int k = 0;
	do
	{
		Usart_SendByte(pUSARTx, *(str + k));
		k++;
	} while (*(str + k) != '\0');

	/* �ȴ�������� */
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TC) == RESET)
	{
	}
}

/*****************  ����һ��16λ�� **********************/
void Usart_SendHalfWord(USART_TypeDef* pUSARTx, uint16_t ch)
{
	uint8_t temp_h, temp_l;

	/* ȡ���߰�λ */
	temp_h = (ch & 0XFF00) >> 8;
	/* ȡ���Ͱ�λ */
	temp_l = ch & 0XFF;

	/* ���͸߰�λ */
	USART_SendData(pUSARTx, temp_h);
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);

	/* ���͵Ͱ�λ */
	USART_SendData(pUSARTx, temp_l);
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
}