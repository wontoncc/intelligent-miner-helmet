#ifndef _DMA_H_
#define _DMA_H_

#include "stm32f0xx.h"

#define bufsize	2

void init_dma(DMA_Channel_TypeDef* DMA_CHx,uint32_t  cpar,uint32_t cmar,uint32_t  p_length,uint32_t  m_length);


#endif

