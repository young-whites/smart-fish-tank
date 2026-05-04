#include "bsp_adc.h"



/***************************************************************************************************************/
/*                                      单通道DMA传输                                                           */
/***************************************************************************************************************/

#if SINGLE_CHANNEL

/**
 * @brief  单通道ADC—DMA工作模式配置
 * @param  None
 * @retval None
 */
__IO uint32_t ADC_ConvertedValue;
void ADC_DMA_GPIO_MODE_Config(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;

	/* DMA工作模式配置 ****************************************/
		// 打开DMA时钟
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	// 复位DMA控制器
	DMA_DeInit(ADC_DMA_CHANNEL);
	// 外设基址为：ADC 数据寄存器地址
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(ADC1->DR));
	// 存储器地址，实际上就是一个内部SRAM的变量
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&ADC_ConvertedValue;
	// 数据源来自外设
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	// 缓冲区大小为1，缓冲区的大小应该等于存储器的大小
	DMA_InitStructure.DMA_BufferSize = 1;
	// 外设寄存器只有一个，地址不用递增
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	// 存储器地址固定
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
	// 外设数据大小为半字，即两个字节
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	// 存储器数据大小也为半字，跟外设数据大小相同
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	// 循环传输模式
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	// DMA 传输通道优先级为高，当使用一个DMA通道时，优先级设置不影响
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	// 禁止存储器到存储器模式，因为是从外设到存储器
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	// 初始化DMA
	DMA_Init(ADC_DMA_CHANNEL, &DMA_InitStructure);
	// 使能 DMA 通道
	DMA_Cmd(ADC_DMA_CHANNEL, ENABLE);




	/* ADC工作模式配置 ****************************************/
		// 打开ADC时钟
	ADCx_1_APBxClock_FUN(ADCx_1_CLK, ENABLE);
	// 只使用一个ADC，属于单模式
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	// 禁止扫描模式，多通道才要，单通道不需要
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	// 连续转换模式
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	// 不用外部触发转换，软件开启即可
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	// 转换结果右对齐
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	// 转换通道1个
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	// 初始化ADC
	ADC_Init(ADC1, &ADC_InitStructure);
	// 配置ADC时钟为PCLK2的8分频，即9MHz
	RCC_ADCCLKConfig(RCC_PCLK2_Div8);
	// 配置 ADC 通道转换顺序为1，第一个转换，采样时间为55.5个时钟周期
	ADC_RegularChannelConfig(ADC1, ADC_GPIOA_PIN1_CHANNEL, 1, ADC_SampleTime_55Cycles5);
	// 使能ADC DMA 请求
	ADC_DMACmd(ADC1, ENABLE);
	// 开启ADC ，并开始转换
	ADC_Cmd(ADC1, ENABLE);
	// 初始化ADC 校准寄存器  
	ADC_ResetCalibration(ADC1);
	// 等待校准寄存器初始化完成
	while (ADC_GetResetCalibrationStatus(ADC1));
	// ADC开始校准
	ADC_StartCalibration(ADC1);
	// 等待校准完成
	while (ADC_GetCalibrationStatus(ADC1));
	// 由于没有采用外部触发，所以使用软件触发ADC转换 
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);

}


#endif /*SINGLE_CHANNEL*/








/***************************************************************************************************************/
/*                                      多通道DMA传输                                                           */
/***************************************************************************************************************/

#if MULTI_CHANNEL


/**
  * @brief  配置ADC工作模式
  * @param  无
  * @retval 无
  */
 // ADC1转换的电压值通过MDA方式传到SRAM
 __IO uint16_t ADC_ConvertedValue[NOFCHANEL]={0,0};
 // 局部变量，用于保存转换计算后的电压值 	 
float ADC_ConvertedValueLocal[NOFCHANEL];  
void ADC_GPIO_Mode_Config(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	
    /* DMA工作模式配置****************************************/   
	    // 打开DMA时钟
	RCC_AHBPeriphClockCmd(ADCx_1_DMA_CLK, ENABLE);
	    // 复位DMA控制器
	DMA_DeInit(ADC_DMA_CHANNEL);
	    // 外设基址为：ADC 数据寄存器地址
	DMA_InitStructure.DMA_PeripheralBaseAddr = ( uint32_t ) ( & ( ADCx_1->DR ) );
	    // 存储器地址
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)ADC_ConvertedValue;
	    // 数据源来自外设
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	    // 缓冲区大小，应该等于数据目的地的大小
	DMA_InitStructure.DMA_BufferSize = NOFCHANEL;
	    // 外设寄存器只有一个，地址不用递增
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	    // 存储器地址递增
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; 
	    // 外设数据大小为半字，即两个字节
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	    // 内存数据大小也为半字，跟外设数据大小相同
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	    // 循环传输模式
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;	
	    // DMA 传输通道优先级为高，当使用一个DMA通道时，优先级设置不影响
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	    // 禁止存储器到存储器模式，因为是从外设到存储器
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	    // 初始化DMA
	DMA_Init(ADC_DMA_CHANNEL, &DMA_InitStructure);
	    // 使能 DMA 通道
	DMA_Cmd(ADC_DMA_CHANNEL , ENABLE);




	/* ADC工作模式配置 ****************************************/
	    // 打开ADC时钟
	ADCx_1_APBxClock_FUN ( ADCx_1_CLK, ENABLE );
	    // 只使用一个ADC，属于单模式
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	    // 扫描模式
	ADC_InitStructure.ADC_ScanConvMode = ENABLE ; 
	    // 连续转换模式
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	    // 不用外部触发转换，软件开启即可
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	    // 转换结果右对齐
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	    // 转换通道个数
	ADC_InitStructure.ADC_NbrOfChannel = NOFCHANEL;	
	    // 初始化ADC
	ADC_Init(ADCx_1, &ADC_InitStructure);
	    // 配置ADC时钟Ｎ狿CLK2的8分频，即9MHz
	RCC_ADCCLKConfig(RCC_PCLK2_Div8); 
	    // 配置ADC 通道的转换顺序和采样时间
	ADC_RegularChannelConfig(ADCx_1, ADC_GPIOA_PIN1_CHANNEL, 1, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADCx_1, ADC_GPIOA_PIN2_CHANNEL, 2, ADC_SampleTime_55Cycles5);
	    // 使能ADC DMA 请求
	ADC_DMACmd(ADCx_1, ENABLE);
	    // 开启ADC ，并开始转换
	ADC_Cmd(ADCx_1, ENABLE);
	    // 初始化ADC 校准寄存器  
	ADC_ResetCalibration(ADCx_1);
	    // 等待校准寄存器初始化完成
	while(ADC_GetResetCalibrationStatus(ADCx_1));
	    // ADC开始校准
	ADC_StartCalibration(ADCx_1);
	    // 等待校准完成
	while(ADC_GetCalibrationStatus(ADCx_1));
	    // 由于没有采用外部触发，所以使用软件触发ADC转换 
	ADC_SoftwareStartConvCmd(ADCx_1, ENABLE);
}


#endif

uint16_t ADC_ReadChannel(uint8_t channel)
{
    ADC_RegularChannelConfig(ADC1, channel, 1, ADC_SampleTime_55Cycles5);
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
    return ADC_GetConversionValue(ADC1);
}
