#include "mpu6050.h"
#include "usart.h"

int MPU6050_ReadByte(int RegisterAddress){
	uint8_t data;
	I2C_Start();
	I2C_SendByte(MPU6050_SlaveAddress);
	I2C_SendByte(RegisterAddress);
	I2C_Start();
	I2C_SendByte(MPU6050_SlaveAddress+1);
	data = I2C_ReceiveByte();
	I2C_SendAck(1);
	I2C_Stop();
	USART_SendData(USART1, data);
	return data;
}

void MPU6050_WriteByte(int RegisterAddress, int data){
	I2C_Start();
	I2C_SendByte(MPU6050_SlaveAddress);
	I2C_SendByte(RegisterAddress);
	I2C_SendByte(data);
	I2C_Stop();
}

