#ifndef _ADC_H_
#define _ADC_H_

#include "stm32f0xx.h"
#include "dma.h"

extern uint16_t	adcdata[bufsize];
//adcdata[0] : ADC_CHSELR_CHSEL2_data
//adcdata[1] : ADC_CHSELR_CHSEL0_data
//adcdata[2] : ADC_CHSELR_CHSEL1_data
//���һ��ͨ����ת��, Ȼ��ʣ�µ�ͨ������ת��

void Adc_Init(void);
uint16_t get_adc(uint8_t ch);
void AdcDma_Init(void );
void AdcDma_Send(uint8_t ch);

#endif

