#ifndef __MPU6050_H
#define __MPU6050_H

#include "stm32f0xx.h"
#include "i2c.h"

#define MPU6050_SlaveAddress 0xD0 // 0x68 << 1  ->  0xD0

int MPU6050_ReadByte(int RegisterAddress);
void MPU6050_WriteByte(int RegisterAddress, int data);

#endif
