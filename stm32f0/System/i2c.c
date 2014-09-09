#include "i2c.h"

void I2C_Configuration(){
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHBPeriphClockCmd(RCC_I2C_Port, ENABLE);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_Pin = I2C_Pin_SDA | I2C_Pin_SCL;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(I2C_Port, &GPIO_InitStructure);
}

void SetSDADirection(unsigned char io){
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = I2C_Pin_SDA;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	switch(io){
		case IN:
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
			break;
		case OUT:
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
			break;
	}
	GPIO_Init(I2C_Port, &GPIO_InitStructure);
}

void I2C_Start(){
	SetSDADirection(OUT);
	SDA_ON();
	SCL_ON();
	DelayUS(10);
	SDA_OFF();
	DelayUS(10);
	SCL_OFF();
}

void I2C_Stop(){
	SetSDADirection(OUT);
	SDA_OFF();
	SCL_ON();
	DelayUS(5);
	SDA_ON();
	DelayUS(5);
}

unsigned int I2C_WaitAck(){
	int wait = 250;
	SCL_ON();
	DelayUS(5);
	SetSDADirection(IN);
	while(GPIO_ReadInputDataBit(I2C_Port, I2C_Pin_SDA)){
		wait--;
		if(wait == 0){
			I2C_Stop();
			return 1;
		}
	}
	SCL_OFF();
	return 0;
}

void I2C_SendAck(uint8_t ack){
	SetSDADirection(OUT);
	if(ack){
		SDA_ON();
	} else{
		SDA_OFF();
	}
	SCL_ON();
	DelayUS(5);
	SCL_OFF();
	DelayUS(5);
}

void I2C_SendByte(uint8_t b){
	uint8_t t = 8;
	SetSDADirection(OUT);
	while(t--){
		if((b & 0x80) == 0x80){
			SDA_ON();
		} else{
			SDA_OFF();
		}
		b <<= 1;
		
		SCL_ON();
		DelayUS(5);
		SCL_OFF();
		DelayUS(5);
	}
	I2C_WaitAck();
}

uint8_t I2C_ReceiveByte(){
	int t;
	uint8_t byte = 0x00;
	SetSDADirection(IN);
	for(t=0;t<8;t++){
		SCL_OFF();
		DelayUS(5);
		SCL_ON();
		byte <<= 1;
		if(GPIO_ReadInputDataBit(I2C_Port, I2C_Pin_SDA)){
			byte++;
		}
		DelayUS(5);
	}
	return byte;
}
