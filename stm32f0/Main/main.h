#ifndef __MAIN_H
#define __MAIN_H

#include "stm32f0xx.h"
#include "usart.h"
#include "delay.h"
#include "buffer.h"
#include "nmea.h"
#include "timer.h"
#include "mpu6050.h"
#include "adc.h"
#include "dma.h"
#include "2401.h"

void USART1_RTOCallback(void);
void TIM3_UpdateCallback(void);
void DMA_Channel5TC_Callback(void);

extern uint8_t USART1_DataBuffer[512];

#define Alarm_ON()	GPIO_SetBits(GPIOA, GPIO_Pin_5)
#define Alarm_OFF()	GPIO_ResetBits(GPIOA, GPIO_Pin_5);

#endif
