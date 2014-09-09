#include "main.h"
#include "discovery_led.h"

uint8_t USART1_DataBuffer[512];
uint8_t USART2_DataBuffer[527];
uint8_t TxBuf[N]/*={0xbc,0x9d,0xa6,0xeb,0x1f,0xab,0xbc,0xcd,0xde,0xef,0x1a,0x2b,0x3c,0x4d,0x5e,0x6f}*/;

unsigned char number = '0';

void miscellaneousGPIO_Configuration(){
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);

	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15 | GPIO_Pin_13;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

int readDialSwitch(){
	int number = 
		!GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_9) * 1 +
		!GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_7) * 2 + 
		!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15) * 4;
	return number;
}

void fillArray(uint8_t *arr, int size, uint8_t ch){
	int i;
	for(i = 0; i < size; i++){
		*(arr + i) = ch;
	}
}

void USART1_ReceiverTimeout_Configuration(){
	USART_SetReceiverTimeOut(USART1, 7100);
	USART_ReceiverTimeOutCmd(USART1, ENABLE);
	USART_ITConfig(USART1, USART_IT_RTO, ENABLE);
}

// Referenced in stm32f0xx_it.c:
// * to handle USART_IT_RTO flag, parse the data in buffer.
void USART1_RTOCallback(){
	int j = 0;
	NMEA_GLLStructure GLLData;
	while(j < serialBuffer.end){
		autoSetNMEAStructrue(serialBuffer.array, &j, &GLLData);
	    // USART_SendChar(USART2, GLLData.status);
	}
	fillBuffer(&serialBuffer, 0);
	serialBuffer.end = 0;
	USART_ClearFlag(USART1, USART_IT_RTO);
	USART_ClearITPendingBit(USART1, USART_IT_RTO);
}

void TIM3_UpdateCallback(){
	// USART1_RxDMA_Restart((uint32_t)test);
}

void NRF2401_Send32BytePackage(uint8_t *package){
	int i;
	for(i = 0; i < 31; i++){
		TxBuf[i+1] = *(package + i);
	}
	TxBuf[0] = number;
	NRF24L01_TxPacket(TxBuf);
	
	DelayMS (1);
}

void NRF2401_Send512BytePackage(uint8_t *package){
	int i = 0;
	for(i = 0; i < 17; i++){
		NRF2401_Send32BytePackage(package+31*i);
		// DelayMS(20);
	}
}

void NRF2401_SendADCPackage(){
	int i = 0, j = 0;
	for(i=0;i<32;i++){
		TxBuf[i] = 0;
	}
	TxBuf[0] = number;
	for(i=0;i<bufsize;i++){
		TxBuf[1+j++] = adcdata[i]>>8;
		TxBuf[1+j++] = adcdata[i]&0x00ff;

	}
	NRF24L01_TxPacket(TxBuf);
	DelayMS (1);
}

void NRF2401_SendSeperateLine(char *s){
	int i = 0;
	char toBeFilled[32] = ">>>>>>>>>>>>>      <<<<<<<<<<<<<";
	toBeFilled[0] = number;
	for(i = 0; i < 32; i++){
		TxBuf[i] = toBeFilled[i];
	}
	for(i = 0; i < 6 && s != 0; i++){
		TxBuf[i+13] = *(s+i);
	}
	
	NRF24L01_TxPacket(TxBuf);
	DelayMS (1);
}

void NRF2401_SendBlackPackage(){
	int i = 0;
	for(i = 0; i < 32; i++){
		TxBuf[i] = 0;
	}
	NRF24L01_TxPacket(TxBuf);
	DelayMS (1);
}

void DMA_Channel5TC_Callback(){
	NRF2401_SendSeperateLine("MPUDAT");
	NRF2401_Send512BytePackage(USART2_DataBuffer);
	
	NRF2401_SendSeperateLine("ADCDAT");
	NRF2401_SendADCPackage();	
}

int main(){
	NMEA_GLLStructure GLLData;
	int i = 0;
	int ADCData1 = 0;
	int ADCData2 = 0;

	miscellaneousGPIO_Configuration();
	number += readDialSwitch();
	
	Nrf_Init();
	Init_NRF24L01_Tx();
	
//	SystemInit();

	USART2_Configuration();
	USART1_Configuration();
	
	// TIM3_Configuration();
	USART1_RxDMA_Configuration((uint32_t)USART1_DataBuffer);
	USART2_RxDMA_Configuration((uint32_t)USART2_DataBuffer, 527);
//	
	Adc_Init(); 
	AdcDma_Init();

	AdcDma_Send(ADC_CHSELR_CHSEL0);
	AdcDma_Send(ADC_CHSELR_CHSEL1);//adcdata[]
	
	NRF2401_SendSeperateLine("MREADY");

	while(1){
		i = 0;
		autoSetNMEAStructrue((char*)USART1_DataBuffer, &i, &GLLData);
		if(GLLData.status == 'A'){
			NRF2401_SendSeperateLine("GPSDAT");
			NRF2401_Send512BytePackage(USART1_DataBuffer);
			initializeNMEAGLLStructure(&GLLData);
		}
//		
		ADCData1 = adcdata[0] >> 4; // ADCData1 => CO
		ADCData2 = adcdata[1] >> 4; // ADCData2 => CH4
		if(ADCData1 >2867 || ADCData2 >3890){
			Alarm_ON();
		} else{
			Alarm_OFF();
		}
		
		DelayMS(1000);
		
	}
}
