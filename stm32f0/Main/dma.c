#include "dma.h"



DMA_InitTypeDef DMA_InitStruct;

//DMA_CHx:DMA通道CHx
//cpar:外设地址
//cmar:存储器地址
//length:数据传输量 
void init_dma(DMA_Channel_TypeDef* DMA_CHx,uint32_t  cpar,uint32_t cmar,uint32_t  p_length,uint32_t  m_length)
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	
	DMA_InitStruct.DMA_BufferSize = bufsize;//数据个数
	DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;//外设为数据源
	DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;
	DMA_InitStruct.DMA_MemoryBaseAddr = cmar;
	DMA_InitStruct.DMA_MemoryDataSize = m_length;
	DMA_InitStruct.DMA_MemoryInc = DMA_PeripheralInc_Disable;//地址不增加
	DMA_InitStruct.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStruct.DMA_PeripheralBaseAddr = cpar;
	DMA_InitStruct.DMA_PeripheralDataSize = p_length;
	DMA_InitStruct.DMA_PeripheralInc = DMA_MemoryInc_Enable;//地址增加
	DMA_InitStruct.DMA_Priority = DMA_Priority_VeryHigh;
	DMA_Init(DMA_CHx , &DMA_InitStruct);
	
	DMA_Cmd(DMA_CHx, ENABLE);
}


