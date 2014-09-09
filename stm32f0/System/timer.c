#include "timer.h"

void TIM3_Configuration(){
	TIM_TimeBaseInitTypeDef Timer_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	uint16_t PrescalerValue = (uint16_t)(SystemCoreClock / 1000 - 1);
	// The computing method of prescaler can be reference in the definition of the init-structure.
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	Timer_InitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	Timer_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	Timer_InitStructure.TIM_Prescaler = PrescalerValue;
	Timer_InitStructure.TIM_Period = 800 - 1;	
	
	TIM_TimeBaseInit(TIM3, &Timer_InitStructure);
	TIM_PrescalerConfig(TIM3, PrescalerValue, TIM_PSCReloadMode_Immediate);
	TIM_Cmd(TIM3, ENABLE);
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}
