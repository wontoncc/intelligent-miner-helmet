#include "adc.h"



#define adcdatat_length		DMA_PeripheralDataSize_HalfWord
#define memdata_length		DMA_MemoryDataSize_HalfWord


uint16_t	adcdata[bufsize];

void Adc_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef ADC_InitStruct;
	//NVIC_InitTypeDef NVIC_InitStruct;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN ;
//	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 |GPIO_Pin_1;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz ;
	GPIO_Init(GPIOA,&GPIO_InitStructure);

	ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStruct.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_InitStruct.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStruct.ADC_ScanDirection = ADC_ScanDirection_Upward;
	ADC_Init(ADC1,&ADC_InitStruct);
	
	ADC_GetCalibrationFactor(ADC1);
	
	ADC_Cmd(ADC1 ,ENABLE);
	
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_ADRDY)); 
  
	/* ADC1 regular Software Start Conv */ 
	ADC_StartOfConversion(ADC1);
	
}

//ch: ADC_CHSELR_CHSEL0~16

uint16_t get_adc(uint8_t ch)
{
	ADC_ChannelConfig(ADC1 ,ch ,ADC_SampleTime_239_5Cycles);
	
	/* ADC1 regular Software Start Conv */ 
	ADC_StartOfConversion(ADC1);
	
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//等待转换结束,读完DR自动清除或直接写一

	return ADC_GetConversionValue(ADC1);
}




void AdcDma_Init(void )
{
	init_dma(DMA1_Channel1,(uint32_t )&ADC1->DR,(uint32_t)&adcdata,adcdatat_length,memdata_length);
	ADC_DMARequestModeConfig(ADC1,ADC_DMAMode_Circular);
 	ADC_DMACmd(ADC1, ENABLE);	
}


//ADC_Channel:DMA1_Channel1	
//ADC_Channel:ADC_CHSELR_CHSEL0
void AdcDma_Send(uint8_t ch)
{
	ADC_ChannelConfig(ADC1,ch,ADC_SampleTime_239_5Cycles);
	
	/* ADC1 regular Software Start Conv */ 
	ADC_StartOfConversion(ADC1);
	
	ADC_ClearFlag(ADC1,ADC_FLAG_EOC);//
}



