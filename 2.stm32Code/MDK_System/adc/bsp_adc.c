#include "bsp_adc.h"



#if SINGLE_CHANNEL


__IO uint32_t ADC_ConvertedValue;
void ADCx_Init(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	DMA_DeInit(ADC_DMA_CHANNEL);

	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(ADC1->DR));

	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADC_ConvertedValue;

	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;

	DMA_InitStructure.DMA_BufferSize = 1;
	
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;

	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;

	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;

	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;

	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;

	DMA_InitStructure.DMA_Priority = DMA_Priority_High;

	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

	DMA_Init(ADC_DMA_CHANNEL, &DMA_InitStructure);

	DMA_Cmd(ADC_DMA_CHANNEL, ENABLE);


	ADCx_1_APBxClock_FUN(ADCx_1_CLK, ENABLE);

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;

	ADC_InitStructure.ADC_ScanConvMode = DISABLE;

	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;

	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;

	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;

	ADC_InitStructure.ADC_NbrOfChannel = 1;

	ADC_Init(ADC1, &ADC_InitStructure);

	RCC_ADCCLKConfig(RCC_PCLK2_Div8);

	ADC_RegularChannelConfig(ADC1, ADC_GPIOA_PIN1_CHANNEL, 1, ADC_SampleTime_55Cycles5);

	ADC_DMACmd(ADC1, ENABLE);

	ADC_Cmd(ADC1, ENABLE);

	ADC_ResetCalibration(ADC1);

	while (ADC_GetResetCalibrationStatus(ADC1));

	ADC_StartCalibration(ADC1);

	while (ADC_GetCalibrationStatus(ADC1));

	ADC_SoftwareStartConvCmd(ADC1, ENABLE);

}


#endif /*SINGLE_CHANNEL*/



