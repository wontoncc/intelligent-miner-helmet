#include "dma.h"



DMA_InitTypeDef DMA_InitStruct;

//DMA_CHx:DMAͨ��CHx
//cpar:�����ַ
//cmar:�洢����ַ
//length:���ݴ����� 
void init_dma(DMA_Channel_TypeDef* DMA_CHx,uint32_t  cpar,uint32_t cmar,uint32_t  p_length,uint32_t  m_length)
{
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	
	DMA_InitStruct.DMA_BufferSize = bufsize;//���ݸ���
	DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;//����Ϊ����Դ
	DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;
	DMA_InitStruct.DMA_MemoryBaseAddr = cmar;
	DMA_InitStruct.DMA_MemoryDataSize = m_length;
	DMA_InitStruct.DMA_MemoryInc = DMA_PeripheralInc_Disable;//��ַ������
	DMA_InitStruct.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStruct.DMA_PeripheralBaseAddr = cpar;
	DMA_InitStruct.DMA_PeripheralDataSize = p_length;
	DMA_InitStruct.DMA_PeripheralInc = DMA_MemoryInc_Enable;//��ַ����
	DMA_InitStruct.DMA_Priority = DMA_Priority_VeryHigh;
	DMA_Init(DMA_CHx , &DMA_InitStruct);
	
	DMA_Cmd(DMA_CHx, ENABLE);
}


