#ifndef __DISCOVERY_LED
#define __DISCOVERY_LED

#define LED_ON 1
#define LED_OFF 0

#include "stm32f0xx.h"

void DiscoveryLED_Configuration(){
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}

void LED_Blue(int s){
	switch(s){
		case LED_ON: GPIO_SetBits(GPIOC, GPIO_Pin_8); break;
		case LED_OFF: GPIO_ResetBits(GPIOC, GPIO_Pin_8); break;
	}
}

void LED_Green(int s){
	switch(s){
		case LED_ON: GPIO_SetBits(GPIOC, GPIO_Pin_9); break;
		case LED_OFF: GPIO_ResetBits(GPIOC, GPIO_Pin_9); break;
	}
}

#endif
