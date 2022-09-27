//------------------------------------------------
// #### PROJECT: O3 TREATMENT - Custom Board #####
// ## Internal Test Functions Module
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ##
// #### TEST_FUNCTIONS.C #########################
//------------------------------------------------

// Includes --------------------------------------------------------------------
#include "test_functions.h"
#include "GTK_Hard.h"
#include "timer.h"
#include "usart.h"
#include "adc.h"
#include "dma.h"

#include "antennas.h"

#include <stdio.h>


// Module Private Types & Macros -----------------------------------------------


// Externals -------------------------------------------------------------------
extern volatile unsigned short antenna_timer;
extern volatile unsigned short adc_ch [];

// Globals ---------------------------------------------------------------------


// Module Private Functions ----------------------------------------------------
void TF_Usart5 (void);
void TF_Usart5_timer_antenna (void);
void TF_Pwm_Channels (void);
void TF_ADC_Single_Conversion (void);
void TF_ADC_Multiple_Conversion (void);
void TF_ADC_Multiple_Conversion_DMA (void);
void TF_ADC_Multiple_Conversion_DMA_2 (void);


// Module Functions ------------------------------------------------------------
void TF_Hardware_Tests (void)
{
    TF_Usart5 ();
    TF_Usart5_timer_antenna ();
    TF_Pwm_Channels ();
    TF_ADC_Single_Conversion ();
    TF_ADC_Multiple_Conversion ();
    TF_ADC_Multiple_Conversion_DMA ();
    TF_ADC_Multiple_Conversion_DMA_2 ();
}


void TF_Usart5 (void)
{
    UART_CH4_Init();
    
    while (1)
    {
        if (LED3)
            LED3_OFF;
        else
            LED3_ON;

        Wait_ms(1000);
        UART_CH4_Send("get_params\r\n");
    }
}


void TF_Usart5_timer_antenna (void)
{
    UART_CH4_Init();
    
    while (1)
    {
        if (!antenna_timer)
        {
            if (LED3)
                LED3_OFF;
            else
                LED3_ON;

            UART_CH4_Send("get_params\r\n");
            antenna_timer = ANTENNA_KEEP_ALIVE_TIMER;
        }
    }
}


void TF_Pwm_Channels (void)
{
    TIM_1_Init();        
    TIM_2_Init();
    TIM_3_Init();
    TIM_4_Init();

    while (1)
    {
        PWM_CH1_TiempoSubida(DUTY_10_PERCENT);
        PWM_CH1_TiempoMantenimiento(DUTY_NONE);
        PWM_CH1_TiempoBajada(DUTY_NONE);
        Wait_ms(5000);
        
        PWM_CH1_TiempoSubida(DUTY_NONE);
        PWM_CH1_TiempoMantenimiento(DUTY_10_PERCENT);
        PWM_CH1_TiempoBajada(DUTY_NONE);
        Wait_ms(5000);

        PWM_CH1_TiempoSubida(DUTY_NONE);
        PWM_CH1_TiempoMantenimiento(DUTY_NONE);
        PWM_CH1_TiempoBajada(DUTY_10_PERCENT);
        Wait_ms(5000);
    }
}


void TF_ADC_Single_Conversion (void)
{
    unsigned short dummy16 = 0;
    char buffSendErr [100];
    
    //--- Test ADC Single conversion ----------------------------------//
    SetChannelSampleTime(ADC_Channel_4, ADC_SampleTime_239_5Cycles);
    SetChannelSampleTime(ADC_Channel_5, ADC_SampleTime_239_5Cycles);
    SetChannelSampleTime(ADC_Channel_6, ADC_SampleTime_239_5Cycles);
    SetChannelSampleTime(ADC_Channel_7, ADC_SampleTime_239_5Cycles);
    SetChannelSampleTime(ADC_Channel_14, ADC_SampleTime_239_5Cycles);
    SetChannelSampleTime(ADC_Channel_15, ADC_SampleTime_239_5Cycles);
        
    while (1)
    {
        UART_PC_Send("before convertion... ");
        do {
            dummy16 = ADC1->DR;
        }
        while (ADC1->SR & ADC_SR_EOC);

        UART_PC_Send("no EOC\n");
            
        ConvertChannel(ADC_Channel_4);

        while (!(ADC1->SR & ADC_SR_EOC));
        sprintf(buffSendErr, "DR: %d dummy16: %d\n", (unsigned short) (ADC1->DR), dummy16);
        UART_PC_Send(buffSendErr);            
            
        Wait_ms(1000);
    }
    //--- End Test ADC Single conversion ----------------------------------//
}


void TF_ADC_Multiple_Conversion (void)
{
    unsigned short dummy16 = 0;
    char buffSendErr [100];
    
    //--- Test ADC Multiple conversion Scanning Mode ----------------------------------//
    SetChannelSampleTime(ADC_Channel_4, ADC_SampleTime_239_5Cycles);
    SetChannelSampleTime(ADC_Channel_5, ADC_SampleTime_239_5Cycles);
    SetChannelSampleTime(ADC_Channel_6, ADC_SampleTime_239_5Cycles);
    SetChannelSampleTime(ADC_Channel_7, ADC_SampleTime_239_5Cycles);
    SetChannelSampleTime(ADC_Channel_14, ADC_SampleTime_239_5Cycles);
    SetChannelSampleTime(ADC_Channel_15, ADC_SampleTime_239_5Cycles);

    SetChannelSamplePosition(ADC_Channel_4, 1);
    SetChannelSamplePosition(ADC_Channel_5, 2);
    SetChannelSamplePosition(ADC_Channel_6, 3);
    SetChannelSamplePosition(ADC_Channel_7, 4);
    SetChannelSamplePosition(ADC_Channel_14, 5);
    SetChannelSamplePosition(ADC_Channel_15, 6);

    SetChannelsQuantity(ADC_Channels_Qtty_6);
    // SetChannelsQuantity(ADC_Channels_Qtty_1);        

    ADC1->CR1 |= ADC_CR1_SCAN;    //convertir toda la secuencia de canales
    // ADC1->CR2 |= ADC_CR2_CONT;    //convertir en forma continua
        
    //activo primera conversion por las dudas        
    if (ADC1->CR2 & ADC_CR2_ADON)
    {
        UART_PC_Send("Adon is on\n");
        //activo una primera conversion
        ADC1->CR2 |= ADC_CR2_SWSTART | ADC_CR2_EXTTRIG;
    }

    while (1)
    {
        if (ADC1->SR & ADC_SR_EOC)
        {
            LED2_ON;
            dummy16 = (unsigned short) ADC1->DR;
            sprintf(buffSendErr, "last ch: %d\n", dummy16);
            UART_PC_Send(buffSendErr);
            Wait_ms(100);
                
            //reviso el bit
            if (!(ADC1->SR & ADC_SR_EOC))
            {
                ADC1->CR2 |= ADC_CR2_SWSTART | ADC_CR2_EXTTRIG;
                LED2_OFF;
            }
        }
    }        
    //--- End Test ADC Multiple conversion Scanning Mode ----------------------------------//
}


void TF_ADC_Multiple_Conversion_DMA (void)
{
    char buffSendErr [100];
    
    //--- Test ADC Multiple conversion Scanning Mode and DMA ------------------------------//
    //-- DMA configuration.
    DMAConfig();
    DMA1_Channel1->CCR |= DMA_CCR1_EN;
        
    SetChannelSampleTime(ADC_Channel_4, ADC_SampleTime_239_5Cycles);
    SetChannelSampleTime(ADC_Channel_5, ADC_SampleTime_239_5Cycles);
    SetChannelSampleTime(ADC_Channel_6, ADC_SampleTime_239_5Cycles);
    SetChannelSampleTime(ADC_Channel_7, ADC_SampleTime_239_5Cycles);
    SetChannelSampleTime(ADC_Channel_14, ADC_SampleTime_239_5Cycles);
    SetChannelSampleTime(ADC_Channel_15, ADC_SampleTime_239_5Cycles);

    SetChannelSamplePosition(ADC_Channel_4, 1);
    SetChannelSamplePosition(ADC_Channel_5, 2);
    SetChannelSamplePosition(ADC_Channel_6, 3);
    SetChannelSamplePosition(ADC_Channel_7, 4);
    SetChannelSamplePosition(ADC_Channel_14, 5);
    SetChannelSamplePosition(ADC_Channel_15, 6);

    SetChannelsQuantity(ADC_Channels_Qtty_6);

    ADC1->CR1 |= ADC_CR1_SCAN;    //convertir toda la secuencia de canales
    // ADC1->CR2 |= ADC_CR2_CONT;    //convertir en forma continua
        
    //activo primera conversion por las dudas        
    if (ADC1->CR2 & ADC_CR2_ADON)
    {
        UART_PC_Send("Adon is on\n");
        //activo una primera conversion
        ADC1->CR2 |= ADC_CR2_SWSTART | ADC_CR2_EXTTRIG;
    }

    while (1)
    {
        if (sequence_ready)
        {
            LED2_ON;
            sprintf(buffSendErr, "IS1: %d, IS2: %d, IS3: %d, IS4: %d\n", IS1, IS2, IS3, IS4);
            UART_PC_Send(buffSendErr);
            Wait_ms(100);
            sprintf(buffSendErr, "V200: %d, V40: %d\n", V200_Sense, V40_Sense);
            UART_PC_Send(buffSendErr);            
            Wait_ms(100);
                
            ADC1->CR2 |= ADC_CR2_SWSTART | ADC_CR2_EXTTRIG;
            LED2_OFF;
            sequence_ready_reset;
        }
    }
    //--- End Test ADC Multiple conversion Scanning Mode and DMA --------------------------//
}


void TF_ADC_Multiple_Conversion_DMA_2 (void)
{
    char buffSendErr [100];
    
    //--- Test ADC Multiple conversion Scanning Continuous Mode and DMA -------------------//
    //-- DMA configuration.
    DMAConfig();
    DMA1_Channel1->CCR |= DMA_CCR1_EN;
        
    SetChannelSampleTime(ADC_Channel_4, ADC_SampleTime_239_5Cycles);
    SetChannelSampleTime(ADC_Channel_5, ADC_SampleTime_239_5Cycles);
    SetChannelSampleTime(ADC_Channel_6, ADC_SampleTime_239_5Cycles);
    SetChannelSampleTime(ADC_Channel_7, ADC_SampleTime_239_5Cycles);
    SetChannelSampleTime(ADC_Channel_14, ADC_SampleTime_239_5Cycles);
    SetChannelSampleTime(ADC_Channel_15, ADC_SampleTime_239_5Cycles);

    SetChannelSamplePosition(ADC_Channel_4, 1);
    SetChannelSamplePosition(ADC_Channel_5, 2);
    SetChannelSamplePosition(ADC_Channel_6, 3);
    SetChannelSamplePosition(ADC_Channel_7, 4);
    SetChannelSamplePosition(ADC_Channel_14, 5);
    SetChannelSamplePosition(ADC_Channel_15, 6);

    SetChannelsQuantity(ADC_Channels_Qtty_6);

    ADC1->CR1 |= ADC_CR1_SCAN;    //convertir toda la secuencia de canales
    ADC1->CR2 |= ADC_CR2_CONT;    //convertir en forma continua
        
    //activo primera conversion por las dudas        
    if (ADC1->CR2 & ADC_CR2_ADON)
    {
        UART_PC_Send("Adon is on\n");
        //activo una primera conversion
        ADC1->CR2 |= ADC_CR2_SWSTART | ADC_CR2_EXTTRIG;
    }

    while (1)
    {
        Wait_ms(1000);
        sprintf(buffSendErr, "IS1: %d, IS2: %d, IS3: %d, IS4: %d\n", IS1, IS2, IS3, IS4);
        UART_PC_Send(buffSendErr);
        Wait_ms(1000);
        sprintf(buffSendErr, "V200: %d, V40: %d\n", V200_Sense, V40_Sense);
        UART_PC_Send(buffSendErr);            
    }
    //--- End Test ADC Multiple conversion Scanning Continuous Mode and DMA ----------------//        
}


//--- end of file ---//
