
#include "timer.h"
#include "stm32f10x.h"
#include "GTK_Signal.h"
#include "adc.h"
#include "uart.h"
#include "misc.h"

extern volatile unsigned short timeRun;
extern volatile unsigned char take_current_samples;

//Wait_ms
volatile unsigned short timer_wait;
extern volatile unsigned char flagMuestreo;

void TIM7_IRQHandler (void)	//1mS
{

	Signal_TIM1MS ();

	//Led3Toggle();

	//Led3Toggle();
	flagMuestreo = 1;
	take_current_samples = 1;

	ADC_TIM7_ISR();
	//GTK_SIGNAL_TIME_1MS ();

	if (timeRun)
		timeRun--;

	//Wait_ms
	if (timer_wait)
		timer_wait--;

	//bajar flag
	if (TIM7->SR & 0x01)	//bajo el flag
		TIM7->SR = 0x00;
}


void TIM6_IRQHandler (void)	//100mS
{
	UART_Tim6 ();

	//bajar flag
	if (TIM6->SR & 0x01)	//bajo el flag
		TIM6->SR = 0x00;
}


//inicializo el TIM7 para interrupciones
void TIM7_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
//    		Counter Register (TIMx_CNT)
//    		Prescaler Register (TIMx_PSC)
//    		Auto-Reload Register (TIMx_ARR)
//			The counter clock frequency CK_CNT is equal to fCK_PSC / (PSC[15:0] + 1)

//			Quiero una interrupcion por ms CK_INT es 72MHz

	//---- Clk ----//
	if (!(RCC->APB1ENR & 0x00000020))
		RCC->APB1ENR |= 0x00000020;

	//--- Config ----//
	TIM7->ARR = 1000;
	//TIM7->ARR = 100;
	TIM7->CNT = 0;
	TIM7->PSC = 71;
	TIM7->EGR = TIM_EGR_UG; //update registers

	// Enable timer ver UDIS
	TIM7->DIER |= TIM_DIER_UIE;
	TIM7->CR1 |= TIM_CR1_CEN;

	//Habilito NVIC
	//Interrupcion timer7.
	NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 10;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 10;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	//NVIC_SetPriority(TIM7_IRQn, 10);
}

void TIM6_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
//    		Counter Register (TIMx_CNT)
//    		Prescaler Register (TIMx_PSC)
//    		Auto-Reload Register (TIMx_ARR)
//			The counter clock frequency CK_CNT is equal to fCK_PSC / (PSC[15:0] + 1)

//			Quiero una interrupcion por ms CK_INT es 72MHz

	//---- Clk ----//
	if (!(RCC->APB1ENR & 0x00000010))
		RCC->APB1ENR |= 0x00000010;

	//--- Config ----//
	TIM6->ARR = 10000; //100mS.
	TIM6->CNT = 0;
	TIM6->PSC = 719;
	TIM6->EGR = TIM_EGR_UG;

	// Enable timer ver UDIS
	TIM6->DIER |= TIM_DIER_UIE;
	TIM6->CR1 |= TIM_CR1_CEN;

	//Habilito NVIC
	//Interrupcion timer6.
	NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 11;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 11;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void Wait_ms (unsigned short a)
{
	timer_wait = a;

	while (timer_wait);
}
