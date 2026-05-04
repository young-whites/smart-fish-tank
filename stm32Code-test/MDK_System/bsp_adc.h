#ifndef __BSP_ADC_H
#define __BSP_ADC_H
#include "sys.h"



#define     SINGLE_CHANNEL      1
#define     MULTI_CHANNEL       0







/***************************************************************************************************************/
/*                                      多通道DMA传输                                                          */
/*                       ADC1 对应 DMA1通道1，ADC3对应DMA2通道5，ADC2没有DMA功能                               */ 
/***************************************************************************************************************/

#if MULTI_CHANNEL
/********************ADC1输入通道（引脚）配置**************************/
#define    ADCx_1_APBxClock_FUN        RCC_APB2PeriphClockCmd
#define    ADCx_1_CLK                  RCC_APB2Periph_ADC1
#define    ADCx_1_DMA_CLK              RCC_AHBPeriph_DMA1


// 转换通道个数
#define    NOFCHANEL					2
// ADC选择宏定义
#define    ADCx_1                       ADC1
/* GPIOA_Pin1 的DMA引脚对应通道宏定义 *************/
#define    ADC_GPIOA_PIN1				GPIO_Pin_1
#define    ADC_GPIOA_PIN1_CHANNEL		ADC_Channel_1
#define    ADC_DMA_CHANNEL				DMA1_Channel1
/* GPIOA_Pin2 的DMA引脚对应通道宏定义 *************/
#define    ADC_GPIOA_PIN2				GPIO_Pin_2
#define    ADC_GPIOA_PIN2_CHANNEL		ADC_Channel_2
#define    ADC_DMA_CHANNEL				DMA1_Channel1
/* GPIOA_Pin4 的DMA引脚对应通道宏定义 *************/
#define    ADC_GPIOA_PIN4				GPIO_Pin_4
#define    ADC_GPIOA_PIN4_CHANNEL		ADC_Channel_4
#define    ADC_DMA_CHANNEL				DMA1_Channel1

void ADC_GPIO_Mode_Config(void);

#endif /*MULTI_CHANNEL*/








/***************************************************************************************************************/
/*                                      单通道DMA传输                                                          */
/*                       ADC1 对应 DMA1通道1，ADC3对应DMA2通道5，ADC2没有DMA功能                               */
/***************************************************************************************************************/
#if SINGLE_CHANNEL

/****************************** ADC1 配置参数定义***************************************/
#define	   ADCx_1_APBxClock_FUN								RCC_APB2PeriphClockCmd
#define	   ADCx_1_CLK										RCC_APB2Periph_ADC1




/* GPIOA_Pin1 的DMA引脚对应通道宏定义 *************/
#define    ADC_GPIOA_PIN1				GPIO_Pin_1
#define    ADC_GPIOA_PIN1_CHANNEL		ADC_Channel_1
#define    ADC_DMA_CHANNEL				DMA1_Channel1



void ADC_DMA_GPIO_MODE_Config(void);

#endif 












#endif /*__BSP_ADC_H*/
