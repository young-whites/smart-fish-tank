#include "bsp_adc.h"



/***************************************************************************************************************/
/*                                      ��ͨ��DMA����                                                           */
/***************************************************************************************************************/

#if SINGLE_CHANNEL

/**
 * @brief  ��ͨ��ADC��DMA����ģʽ����
 * @param  None
 * @retval None
 */
__IO uint32_t ADC_ConvertedValue;
void ADC_DMA_GPIO_MODE_Config(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;

	/* DMA����ģʽ���� ****************************************/
		// ��DMAʱ��
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	// ��λDMA������
	DMA_DeInit(ADC_DMA_CHANNEL);
	// �����ַΪ��ADC ���ݼĴ�����ַ
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(ADC1->DR));
	// �洢����ַ��ʵ���Ͼ���һ���ڲ�SRAM�ı���
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADC_ConvertedValue;
	// ����Դ��������
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	// ��������СΪ1���������Ĵ�СӦ�õ��ڴ洢���Ĵ�С
	DMA_InitStructure.DMA_BufferSize = 1;
	// ����Ĵ���ֻ��һ������ַ���õ���
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	// �洢����ַ�̶�
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
	// �������ݴ�СΪ���֣��������ֽ�
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	// �洢�����ݴ�СҲΪ���֣����������ݴ�С��ͬ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	// ѭ������ģʽ
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	// DMA ����ͨ�����ȼ�Ϊ�ߣ���ʹ��һ��DMAͨ��ʱ�����ȼ����ò�Ӱ��
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	// ��ֹ�洢�����洢��ģʽ����Ϊ�Ǵ����赽�洢��
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	// ��ʼ��DMA
	DMA_Init(ADC_DMA_CHANNEL, &DMA_InitStructure);
	// ʹ�� DMA ͨ��
	DMA_Cmd(ADC_DMA_CHANNEL, ENABLE);




	/* ADC����ģʽ���� ****************************************/
		// ��ADCʱ��
	ADCx_1_APBxClock_FUN(ADCx_1_CLK, ENABLE);
	// ֻʹ��һ��ADC�����ڵ�ģʽ
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	// ��ֹɨ��ģʽ����ͨ����Ҫ����ͨ������Ҫ
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	// ����ת��ģʽ
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	// �����ⲿ����ת����������������
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	// ת������Ҷ���
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	// ת��ͨ��1��
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	// ��ʼ��ADC
	ADC_Init(ADC1, &ADC_InitStructure);
	// ����ADCʱ��ΪPCLK2��8��Ƶ����9MHz
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);
	// ���� ADC ͨ��ת��˳��Ϊ1����һ��ת��������ʱ��Ϊ55.5��ʱ������
	ADC_RegularChannelConfig(ADC1, ADC_GPIOA_PIN1_CHANNEL, 1, ADC_SampleTime_55Cycles5);
	// ʹ��ADC DMA ����
	ADC_DMACmd(ADC1, ENABLE);
	// ����ADC ������ʼת��
	ADC_Cmd(ADC1, ENABLE);
	// ��ʼ��ADC У׼�Ĵ���  
	ADC_ResetCalibration(ADC1);
	// �ȴ�У׼�Ĵ�����ʼ�����
	while (ADC_GetResetCalibrationStatus(ADC1));
	// ADC��ʼУ׼
	ADC_StartCalibration(ADC1);
	// �ȴ�У׼���
	while (ADC_GetCalibrationStatus(ADC1));
	// ����û�в����ⲿ����������ʹ����������ADCת�� 
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);

}


#endif /*SINGLE_CHANNEL*/








/***************************************************************************************************************/
/*                                      ��ͨ��DMA����                                                           */
/***************************************************************************************************************/

#if MULTI_CHANNEL


/**
  * @brief  ����ADC����ģʽ
  * @param  ��
  * @retval ��
  */
 // ADC1ת���ĵ�ѹֵͨ��MDA��ʽ����SRAM
 __IO uint16_t ADC_ConvertedValue[NOFCHANEL]={0,0};
 // �ֲ����������ڱ���ת�������ĵ�ѹֵ 	 
float ADC_ConvertedValueLocal[NOFCHANEL];  
void ADC_GPIO_Mode_Config(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	
    /* DMA����ģʽ����****************************************/   
	    // ��DMAʱ��
	RCC_AHBPeriphClockCmd(ADCx_1_DMA_CLK, ENABLE);
	    // ��λDMA������
	DMA_DeInit(ADC_DMA_CHANNEL);
	    // �����ַΪ��ADC ���ݼĴ�����ַ
	DMA_InitStructure.DMA_PeripheralBaseAddr = ( uint32_t ) ( & ( ADCx_1->DR ) );
	    // �洢����ַ
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)ADC_ConvertedValue;
	    // ����Դ��������
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	    // ��������С��Ӧ�õ�������Ŀ�ĵصĴ�С
	DMA_InitStructure.DMA_BufferSize = NOFCHANEL;
	    // ����Ĵ���ֻ��һ������ַ���õ���
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	    // �洢����ַ����
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; 
	    // �������ݴ�СΪ���֣��������ֽ�
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	    // �ڴ����ݴ�СҲΪ���֣����������ݴ�С��ͬ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	    // ѭ������ģʽ
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;	
	    // DMA ����ͨ�����ȼ�Ϊ�ߣ���ʹ��һ��DMAͨ��ʱ�����ȼ����ò�Ӱ��
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	    // ��ֹ�洢�����洢��ģʽ����Ϊ�Ǵ����赽�洢��
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	    // ��ʼ��DMA
	DMA_Init(ADC_DMA_CHANNEL, &DMA_InitStructure);
	    // ʹ�� DMA ͨ��
	DMA_Cmd(ADC_DMA_CHANNEL , ENABLE);




	/* ADC����ģʽ���� ****************************************/
	    // ��ADCʱ��
	ADCx_1_APBxClock_FUN ( ADCx_1_CLK, ENABLE );
	    // ֻʹ��һ��ADC�����ڵ�ģʽ
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	    // ɨ��ģʽ
	ADC_InitStructure.ADC_ScanConvMode = ENABLE ; 
	    // ����ת��ģʽ
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	    // �����ⲿ����ת����������������
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	    // ת������Ҷ���
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	    // ת��ͨ������
	ADC_InitStructure.ADC_NbrOfChannel = NOFCHANEL;	
	    // ��ʼ��ADC
	ADC_Init(ADCx_1, &ADC_InitStructure);
	    // ����ADCʱ�ӣΪPCLK2��8��Ƶ����9MHz
	RCC_ADCCLKConfig(RCC_PCLK2_Div8); 
	    // ����ADC ͨ����ת��˳��Ͳ���ʱ��
	ADC_RegularChannelConfig(ADCx_1, ADC_GPIOA_PIN1_CHANNEL, 1, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADCx_1, ADC_GPIOA_PIN2_CHANNEL, 2, ADC_SampleTime_55Cycles5);
	    // ʹ��ADC DMA ����
	ADC_DMACmd(ADCx_1, ENABLE);
	    // ����ADC ������ʼת��
	ADC_Cmd(ADCx_1, ENABLE);
	    // ��ʼ��ADC У׼�Ĵ���  
	ADC_ResetCalibration(ADCx_1);
	    // �ȴ�У׼�Ĵ�����ʼ�����
	while(ADC_GetResetCalibrationStatus(ADCx_1));
	    // ADC��ʼУ׼
	ADC_StartCalibration(ADCx_1);
	    // �ȴ�У׼���
	while(ADC_GetCalibrationStatus(ADCx_1));
	    // ����û�в����ⲿ����������ʹ����������ADCת�� 
	ADC_SoftwareStartConvCmd(ADCx_1, ENABLE);
}


#endif


/**
 * @brief  ADC1 polling mode initialization
 *         Configures PA0, PA1, PA4 as analog inputs
 *         ADC1 independent mode, single conversion, software trigger
 */
void ADC_Polling_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    ADC_InitTypeDef ADC_InitStructure;

    /* Enable GPIOA and ADC1 clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1, ENABLE);

    /* Configure PA0(IN0), PA1(IN1), PA4(IN4) as analog input */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* ADC1 configuration */
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &ADC_InitStructure);

    /* ADC clock = PCLK2/8 = 9MHz */
    RCC_ADCCLKConfig(RCC_PCLK2_Div8);

    /* Enable ADC1 */
    ADC_Cmd(ADC1, ENABLE);

    /* Calibration */
    ADC_ResetCalibration(ADC1);
    while (ADC_GetResetCalibrationStatus(ADC1));
    ADC_StartCalibration(ADC1);
    while (ADC_GetCalibrationStatus(ADC1));
}


uint16_t ADC_ReadChannel(uint8_t channel)
{
    ADC_RegularChannelConfig(ADC1, channel, 1, ADC_SampleTime_55Cycles5);
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
    return ADC_GetConversionValue(ADC1);
}
