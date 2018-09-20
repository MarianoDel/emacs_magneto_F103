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
// #include "uart.h"
// #include "misc.h"


/* Externals -------------------------------------------------------------------*/
extern volatile unsigned short timeRun;
extern volatile unsigned char take_current_samples;
#ifdef USE_BUZZER_ON_BOARD
extern unsigned short buzzer_timeout;
#endif


/* Globals ---------------------------------------------------------------------*/
//Wait_ms
volatile unsigned short timer_wait;
// extern volatile unsigned char flagMuestreo;

/* Module Exported Functions ---------------------------------------------------*/
void Update_TIM1_CH1 (unsigned short a)
{
    TIM1->CCR1 = a;
}

void Update_TIM1_CH4 (unsigned short a)
{
    TIM1->CCR4 = a;
}

void Update_TIM2_CH1 (unsigned short a)
{
    TIM2->CCR1 = a;
}

void Update_TIM2_CH2 (unsigned short a)
{
    TIM2->CCR2 = a;
}

void Update_TIM3_CH1 (unsigned short a)
{
    TIM3->CCR1 = a;
}

void Update_TIM3_CH2 (unsigned short a)
{
    TIM3->CCR2 = a;
}

void Update_TIM3_CH3 (unsigned short a)
{
    TIM3->CCR3 = a;
}

void Update_TIM3_CH4 (unsigned short a)
{
    TIM3->CCR4 = a;
}

void Update_TIM4_CH1 (unsigned short a)
{
    TIM4->CCR1 = a;
}

void Update_TIM4_CH2 (unsigned short a)
{
    TIM4->CCR2 = a;
}

void Update_TIM4_CH3 (unsigned short a)
{
    TIM4->CCR3 = a;
}

void Update_TIM4_CH4 (unsigned short a)
{
    TIM4->CCR4 = a;
}

//PA8 TIM1_CH1 para PWMN1
//PA11 TIM1_CH4 para PWM2002
void TIM_1_Init (void)
{
    if (!RCC_TIM1_CLK)
        RCC_TIM1_CLK_ON;

    //Nuevo
    //Configuracion del timer.
    //TIM1->CR1 |= TIM_CR1_OPM;        //clk int / 1; upcounting; one pulse
    TIM1->CR1 = 0x00;        //clk int / 1;
    TIM1->CR1 |= TIM_CR1_ARPE;        //preload enable
    // TIM1->CR2 |= TIM_CR2_MMS_1;        //UEV -> TRG0
    //TIM1->CR2 = 0x00;
    //TIM1->SMCR |= TIM_SMCR_MSM | TIM_SMCR_SMS_2 | TIM_SMCR_SMS_1 | TIM_SMCR_TS_1;    //link timer3
    TIM1->SMCR = 0x0000;

    TIM1->CCMR1 = 0x0060;            //CH1 output PWM mode 1 (channel active TIM1->CNT < TIM1->CCR1)    
    TIM1->CCMR2 = 0x6000;            //CH4 output PWM mode 1
    TIM1->CCER |= TIM_CCER_CC1E | TIM_CCER_CC4E;
        
    TIM1->BDTR |= TIM_BDTR_MOE;
    TIM1->ARR = DUTY_100_PERCENT;    //cada tick 20.83ns

    TIM1->CNT = 0;
    TIM1->PSC = PWM_PRESCALER;

    TIM1->CCR1 = 0;
    TIM1->CCR4 = 0;
    
    // Enable timer ver UDIS
    //TIM1->DIER |= TIM_DIER_UIE;
    TIM1->CR1 |= TIM_CR1_CEN;

}

//PA0 TIM2_CH1_ETR para PWMN2001
//PA1 TIM2_CH2 para PWM401
void TIM_2_Init (void)
{
    if (!RCC_TIM2_CLK)
        RCC_TIM2_CLK_ON;

    //Nuevo
    //Configuracion del timer.
    //TIM2->CR1 |= TIM_CR1_OPM;        //clk int / 1; upcounting; one pulse
    TIM2->CR1 = 0x00;        //clk int / 1;
    TIM2->CR1 |= TIM_CR1_ARPE;        //preload enable
    // TIM2->CR2 |= TIM_CR2_MMS_1;        //UEV -> TRG0
    //TIM2->CR2 = 0x00;
    //TIM2->SMCR |= TIM_SMCR_MSM | TIM_SMCR_SMS_2 | TIM_SMCR_SMS_1 | TIM_SMCR_TS_1;    //link timer3
    TIM2->SMCR = 0x0000;

    TIM2->CCMR1 = 0x6060;            //CH1 & CH2 output PWM mode 1 (channel active TIM2->CNT < TIM2->CCR1)    
    // TIM2->CCMR2 = 0x6000;            //CH4 output PWM mode 1
    TIM2->CCER |= TIM_CCER_CC1E | TIM_CCER_CC2E;
        
    // TIM2->BDTR |= TIM_BDTR_MOE;
    TIM2->ARR = DUTY_100_PERCENT;    //cada tick 20.83ns

    TIM2->CNT = 0;
    TIM2->PSC = PWM_PRESCALER;

    TIM2->CCR1 = 0;
    TIM2->CCR2 = 0;
    
    // Enable timer ver UDIS
    //TIM2->DIER |= TIM_DIER_UIE;
    TIM2->CR1 |= TIM_CR1_CEN;

}

//PC6 TIM3_CH1 para PWM402
//PC7 TIM3_CH2 para PWMN2
//PC8 TIM3_CH3 para PWM2003
//PC9 TIM3_CH4 para PWM403
void TIM_3_Init (void)
{
    if (!RCC_TIM3_CLK)
        RCC_TIM3_CLK_ON;

    //Full Remap
    AFIO->MAPR |= 0x00000C00;

    //Nuevo
    //Configuracion del timer.
    //TIM3->CR1 |= TIM_CR1_OPM;        //clk int / 1; upcounting; one pulse
    TIM3->CR1 = 0x00;        //clk int / 1;
    TIM3->CR1 |= TIM_CR1_ARPE;        //preload enable
    // TIM3->CR2 |= TIM_CR2_MMS_1;        //UEV -> TRG0
    //TIM3->CR2 = 0x00;
    //TIM3->SMCR |= TIM_SMCR_MSM | TIM_SMCR_SMS_2 | TIM_SMCR_SMS_1 | TIM_SMCR_TS_1;    //link timer3
    TIM3->SMCR = 0x0000;

    TIM3->CCMR1 = 0x6060;            //CH1 & CH2 output PWM mode 1 (channel active TIM3->CNT < TIM3->CCR1)    
    TIM3->CCMR2 = 0x6060;            //CH3 & CH4 output PWM mode 1
    TIM3->CCER |= TIM_CCER_CC1E | TIM_CCER_CC2E | TIM_CCER_CC3E | TIM_CCER_CC4E;
        
    // TIM3->BDTR |= TIM_BDTR_MOE;
    TIM3->ARR = DUTY_100_PERCENT;    //cada tick 20.83ns

    TIM3->CNT = 0;
    TIM3->PSC = PWM_PRESCALER;

    TIM3->CCR1 = 0;
    TIM3->CCR2 = 0;
    TIM3->CCR3 = 0;
    TIM3->CCR4 = 0;
    
    // Enable timer ver UDIS
    //TIM3->DIER |= TIM_DIER_UIE;
    TIM3->CR1 |= TIM_CR1_CEN;

}

//PB6 TIM4_CH1 para PWMN3
//PB7 TIM4_CH2 para PWM2004
//PB8 TIM4_CH3 para PWM404
//PB9 TIM4_CH4 para PWMN4
void TIM_4_Init (void)
{
    if (!RCC_TIM4_CLK)
        RCC_TIM4_CLK_ON;

    //Nuevo
    //Configuracion del timer.
    //TIM4->CR1 |= TIM_CR1_OPM;        //clk int / 1; upcounting; one pulse
    TIM4->CR1 = 0x00;        //clk int / 1;
    TIM4->CR1 |= TIM_CR1_ARPE;        //preload enable
    // TIM4->CR2 |= TIM_CR2_MMS_1;        //UEV -> TRG0
    //TIM4->CR2 = 0x00;
    //TIM4->SMCR |= TIM_SMCR_MSM | TIM_SMCR_SMS_2 | TIM_SMCR_SMS_1 | TIM_SMCR_TS_1;    //link timer3
    TIM4->SMCR = 0x0000;

    TIM4->CCMR1 = 0x6060;            //CH1 & CH2 output PWM mode 1 (channel active TIM4->CNT < TIM4->CCR1)    
    TIM4->CCMR2 = 0x6060;            //CH3 & CH4 output PWM mode 1
    TIM4->CCER |= TIM_CCER_CC1E | TIM_CCER_CC2E | TIM_CCER_CC3E | TIM_CCER_CC4E;
        
    // TIM4->BDTR |= TIM_BDTR_MOE;
    TIM4->ARR = DUTY_100_PERCENT;    //cada tick 20.83ns

    TIM4->CNT = 0;
    TIM4->PSC = PWM_PRESCALER;

    TIM4->CCR1 = 0;
    TIM4->CCR2 = 0;
    TIM4->CCR3 = 0;
    TIM4->CCR4 = 0;
    
    // Enable timer ver UDIS
    //TIM4->DIER |= TIM_DIER_UIE;
    TIM4->CR1 |= TIM_CR1_CEN;

}

void TIM7_IRQHandler (void)	//1mS
{

    Signal_TIM1MS ();

    //Led3Toggle();
    // flagMuestreo = 1;
    if (take_current_samples)
        take_current_samples--;

    // ADC_TIM7_ISR();
    //GTK_SIGNAL_TIME_1MS ();

    if (timeRun)
        timeRun--;

    //Wait_ms
    if (timer_wait)
        timer_wait--;

#ifdef USE_BUZZER_ON_BOARD
    if (buzzer_timeout)
        buzzer_timeout--;
#endif
        
    //bajar flag
    if (TIM7->SR & 0x01)	
        TIM7->SR = 0x00;
}

void TIM5_Init (void)
{
    if (!RCC_TIM5_CLK)
        RCC_TIM5_CLK_ON;

    TIM5->CR1 = 0x0000;

    //Configuracion del timer.
    TIM5->ARR = 100; //100uS.
    TIM5->CNT = 0;
    TIM5->PSC = 71;
    // TIM5->PSC = 143;		//error en frecuencia para TIM5 me da x2
    //TIM5->EGR = TIM_EGR_UG;

    // Enable timer ver UDIS
    TIM5->DIER |= TIM_DIER_UIE;
    TIM5->CR1 |= TIM_CR1_CEN;

    // NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
    // NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 10;
    // NVIC_InitStructure.NVIC_IRQChannelSubPriority = 10;
    // NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    // NVIC_Init(&NVIC_InitStructure);

    //Habilito NVIC
    //Interrupcion timer6.
    NVIC_EnableIRQ(TIM5_IRQn);
    NVIC_SetPriority(TIM5_IRQn, 10);
}

void TIM5_IRQHandler (void)	//100uS
{

    TIM5_IRQ_Callback();
    //bajar flag
    if (TIM5->SR & 0x01)	//bajo el flag
        TIM5->SR = 0x00;
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
