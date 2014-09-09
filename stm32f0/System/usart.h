#ifndef __USART_H
#define __USART_H

#include "stm32f0xx.h"

void USART_SendChar(USART_TypeDef *USARTx, char ch);
void USART_SendString(USART_TypeDef *USARTx, char *ch);
void USART2_Configuration(void);
void USART1_Configuration(void);
// Handle the data receive from USART in stm32f0xx_it.c with interrupt.

void USART1_RxDMA_Configuration(uint32_t bufferAddress);
void USART2_RxDMA_Configuration(uint32_t bufferAddress, unsigned int bufferSize);
void USART1_RxDMA_Restart(uint32_t bufAddr);
void USART2_RxDMA_Restart(uint32_t bufAddr, int bufSize);
#endif
