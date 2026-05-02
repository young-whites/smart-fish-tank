#ifndef __BSP_ADC_H
#define __BSP_ADC_H
#include "sys.h"



#define     SINGLE_CHANNEL      1
#define     MULTI_CHANNEL       0







/***************************************************************************************************************/
/*                                      ��ͨ��DMA����                                                          */
/*                       ADC1 ��Ӧ DMA1ͨ��1��ADC3��ӦDMA2ͨ��5��ADC2û��DMA����                               */ 
/***************************************************************************************************************/

#if MULTI_CHANNEL
/********************ADC1����ͨ�������ţ�����**************************/
#define    ADCx_1_APBxClock_FUN        RCC_APB2PeriphClockCmd
#define    ADCx_1_CLK                  RCC_APB2Periph_ADC1
#define    ADCx_1_DMA_CLK              RCC_AHBPeriph_DMA1


// ת��ͨ������
#define    NOFCHANEL					2
// ADCѡ��궨��
#define    ADCx_1                       ADC1
/* GPIOA_Pin1 ��DMA���Ŷ�Ӧͨ���궨�� *************/
#define    ADC_GPIOA_PIN1				GPIO_Pin_1
#define    ADC_GPIOA_PIN1_CHANNEL		ADC_Channel_1
#define    ADC_DMA_CHANNEL				DMA1_Channel1
/* GPIOA_Pin2 ��DMA���Ŷ�Ӧͨ���궨�� *************/
#define    ADC_GPIOA_PIN2				GPIO_Pin_2
#define    ADC_GPIOA_PIN2_CHANNEL		ADC_Channel_2
#define    ADC_DMA_CHANNEL				DMA1_Channel1
/* GPIOA_Pin4 ��DMA���Ŷ�Ӧͨ���궨�� *************/
#define    ADC_GPIOA_PIN4				GPIO_Pin_4
#define    ADC_GPIOA_PIN4_CHANNEL		ADC_Channel_4
#define    ADC_DMA_CHANNEL				DMA1_Channel1

void ADC_GPIO_Mode_Config(void);

#endif /*MULTI_CHANNEL*/








/***************************************************************************************************************/
/*                                      ��ͨ��DMA����                                                          */
/*                       ADC1 ��Ӧ DMA1ͨ��1��ADC3��ӦDMA2ͨ��5��ADC2û��DMA����                               */
/***************************************************************************************************************/
#if SINGLE_CHANNEL

/****************************** ADC1 ���ò�������***************************************/
#define	   ADCx_1_APBxClock_FUN								RCC_APB2PeriphClockCmd
#define	   ADCx_1_CLK										RCC_APB2Periph_ADC1




/* GPIOA_Pin1 ��DMA���Ŷ�Ӧͨ���궨�� *************/
#define    ADC_GPIOA_PIN1				GPIO_Pin_1
#define    ADC_GPIOA_PIN1_CHANNEL		ADC_Channel_1
#define    ADC_DMA_CHANNEL				DMA1_Channel1



void ADCx_Init(void);

#endif 












#endif /*__BSP_ADC_H*/
