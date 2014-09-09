#ifndef __I2C_H
#define __I2C_H

#include "stm32f0xx.h"
#include "delay.h"

#define I2C_Pin_SCL GPIO_Pin_4
#define I2C_Pin_SDA GPIO_Pin_5
#define I2C_Port GPIOA
#define RCC_I2C_Port RCC_AHBPeriph_GPIOA

#define IN 1
#define OUT 2
#define SDA_ON() GPIO_SetBits(I2C_Port, I2C_Pin_SDA)
#define SDA_OFF() GPIO_ResetBits(I2C_Port, I2C_Pin_SDA)
#define SCL_ON() GPIO_SetBits(I2C_Port, I2C_Pin_SCL)
#define SCL_OFF() GPIO_ResetBits(I2C_Port, I2C_Pin_SCL)

void I2C_Configuration(void);
void I2C_Start(void);
void I2C_Stop(void);
unsigned int I2C_WaitAck(void);
void I2C_SendAck(uint8_t ack);
void I2C_SendByte(uint8_t b);
uint8_t I2C_ReceiveByte(void);

#endif
