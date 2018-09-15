//---------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F103
// ##
// #### TIMER.C ################################
//---------------------------------------------

/* Includes ------------------------------------------------------------------*/
#include "timer.h"
#include "stm32f10x.h"
#include "GTK_Signal.h"
#include "GTK_Hard.h"
#include "adc.h"
#include "uart.h"
#include "misc.h"


/* Externals -------------------------------------------------------------------*/
extern volatile unsigned short timeRun;
extern volatile unsigned char take_current_samples;
#ifdef SOFTWARE_VERSION_1_2
extern unsigned short buzzer_timeout;
#endif


/* Globals ---------------------------------------------------------------------*/
//Wait_ms
volatile unsigned short timer_wait;
extern volatile unsigned char flagMuestreo;


/* Module Exported Functions ---------------------------------------------------*/
void TIM7_IRQHandler (void)	//1mS
{

	Signal_TIM1MS ();

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

#ifdef SOFTWARE_VERSION_1_2
	if (buzzer_timeout)
		buzzer_timeout--;
#endif
}


void TIM6_IRQHandler (void)	//100mS
{
	// UART_Tim6 ();

	//bajar flag
	if (TIM6->SR & 0x01)	//bajo el flag
		TIM6->SR = 0x00;
}


//inicializo el TIM7 para interrupciones
void TIM7_Init(void)
{
    // Counter Register (TIMx_CNT)
    // Prescaler Register (TIMx_PSC)
    // Auto-Reload Register (TIMx_ARR)
    // The counter clock frequency CK_CNT is equal to fCK_PSC / (PSC[15:0] + 1)
    // Quiero una interrupcion por ms CK_INT es 72MHz

    //---- Clk ----//
    if (!RCC_TIM7_CLK)
        RCC_TIM7_CLK_ON;

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
    NVIC_EnableIRQ(TIM7_IRQn);
    NVIC_SetPriority(TIM7_IRQn, 10);    
}

void TIM6_Init(void)
{
    // Counter Register (TIMx_CNT)
    // Prescaler Register (TIMx_PSC)
    // Auto-Reload Register (TIMx_ARR)
    // The counter clock frequency CK_CNT is equal to fCK_PSC / (PSC[15:0] + 1)
    // Quiero una interrupcion por ms CK_INT es 72MHz

    //---- Clk ----//
    if (!RCC_TIM6_CLK)
        RCC_TIM6_CLK_ON;

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
    NVIC_EnableIRQ(TIM6_IRQn);
    NVIC_SetPriority(TIM6_IRQn, 11);    
        
}

void Wait_ms (unsigned short a)
{
    timer_wait = a;

    while (timer_wait);
}
