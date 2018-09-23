//---------------------------------------------------------
// #### PROYECTO MAGNETO GAUSSTEK - MotherBoard ###########
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F103
// ##
// #### COMMS.C ###########################################
//---------------------------------------------------------

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "GTK_Hard.h"
#include "timer.h"
#include "usart.h"
#include "adc.h"
#include "GTK_Estructura.h"
#include "GTK_Signal.h"
#include "flash_program.h"
#include "GTK_Errors.h"

#include "comms.h"
// #include "hard.h"
#include "startup_clocks.h"
#include "gpio.h"
#include "dma.h"
#include "antennas.h"

#include <stdio.h>

/* Externals ------------------------------------------------------------------*/
//--- Externals para enviar errores en UART
char buffSendErr[64];
volatile unsigned char usart1_have_data = 0;
volatile unsigned char usart2_have_data = 0;
volatile unsigned char usart3_have_data = 0;
volatile unsigned char usart4_have_data = 0;
volatile unsigned char usart5_have_data = 0;

//--- Externals para enviar keepalive por UART (nuevo en modulo antenna)
volatile unsigned short antenna_timer = 0;

//--- Externals para muestreos de corriente con el ADC
volatile unsigned char take_current_samples = 0;
volatile unsigned short adc_ch [ADC_CHANNEL_QUANTITY];

//--- Externals para armar seniales y comprobar el TIM5 en el inicio del programa
volatile unsigned int session_warming_up_channel_1_stage_time = 0;

//--- Externals para el BUZZER
#ifdef USE_BUZZER_ON_BOARD
unsigned short buzzer_timeout = 0;
#endif

//Estructuras.
session_typedef session_slot_aux;

session_typedef session_ch_1;
session_typedef session_ch_2;
session_typedef session_ch_3;
session_typedef session_ch_4;

//session_typedef session_slot_1;
//session_typedef session_slot_2;
//session_typedef session_slot_3;
//session_typedef session_slot_4;
//session_typedef session_slot_5;

unsigned char channel_1_pause = 0;
unsigned char channel_2_pause = 0;
unsigned char channel_3_pause = 0;
unsigned char channel_4_pause = 0;

/* Globals --------------------------------------------------------------------*/


/* Module Functions -----------------------------------------------------------*/
int main (void)
{
    unsigned char i = 0;
    unsigned short dummy16 = 0;

    //Configuracion de clock.
    // SystemInit();    //SystemInit() is called from startup_stm32f10x_hd.s before main()

    //Configure all gpios and alternative functions pins
    GpioInit();
    LED2_OFF;
    LED3_OFF;

    //Timer 1ms -- Wait_ms()
    TIM7_Init();
        
    //Timer 100ms.
    TIM6_Init();

    //Signal timer -- 100us
    TIM5_Init();

    //Ajusto frecuencia del clock, debe prender el led Ton = 100ms T = 200ms
    //de otra forma revisar el cristal elegido
    //pruebo TIM5 100us step
    for (i = 0; i < 20; i++)
    {
        if (LED1)
            LED1_OFF;
        else
            LED1_ON;

        while (session_warming_up_channel_1_stage_time != 0);
        session_warming_up_channel_1_stage_time = 1000;	//100ms
        // Wait_ms(100);
    }

    //TODO: luego habilitar sin libST
    //ADC1.
    // ADC1_Init();
    AdcConfig();

#ifdef USE_ADC_SAMPLE_BY_SAMPLE
    SetChannelSampleTime(ADC_Channel_4, ADC_SampleTime_239_5Cycles);
    SetChannelSampleTime(ADC_Channel_5, ADC_SampleTime_239_5Cycles);
    SetChannelSampleTime(ADC_Channel_6, ADC_SampleTime_239_5Cycles);
    SetChannelSampleTime(ADC_Channel_7, ADC_SampleTime_239_5Cycles);
    SetChannelSampleTime(ADC_Channel_14, ADC_SampleTime_239_5Cycles);
    SetChannelSampleTime(ADC_Channel_15, ADC_SampleTime_239_5Cycles);

    //nothing more to be done, use ConvertChannel(ADC_Channel_X) from now on
#endif

    //UART_Debug Config.
    UART_PC_Init();
    UART_CH1_Init();
    UART_CH2_Init();
    UART_CH3_Init();
    UART_CH4_Init();

    //Activate timers for the PWM outputs
    TIM_1_Init();        
    TIM_2_Init();
    TIM_3_Init();
    TIM_4_Init();

    PWM_CH1_TiempoSubida(DUTY_NONE);
    PWM_CH1_TiempoMantenimiento(DUTY_NONE);
    PWM_CH1_TiempoBajada(DUTY_100_PERCENT);
        
    PWM_CH2_TiempoSubida(DUTY_NONE);
    PWM_CH2_TiempoMantenimiento(DUTY_NONE);
    PWM_CH2_TiempoBajada(DUTY_100_PERCENT);
        
    PWM_CH3_TiempoSubida(DUTY_NONE);
    PWM_CH3_TiempoMantenimiento(DUTY_NONE);
    PWM_CH3_TiempoBajada(DUTY_100_PERCENT);

    PWM_CH4_TiempoSubida(DUTY_NONE);
    PWM_CH4_TiempoMantenimiento(DUTY_NONE);
    PWM_CH4_TiempoBajada(DUTY_100_PERCENT);


    // while (1)
    // {
    //     session_ch_1.status = 1;
    //     session_ch_2.status = 1;
    //     session_ch_3.status = 1;
    //     session_ch_4.status = 1;

    //     Session_Current_Limit_control();
    // }


    //--- Test ADC Single conversion ----------------------------------//
    // SetChannelSampleTime(ADC_Channel_4, ADC_SampleTime_239_5Cycles);
    // SetChannelSampleTime(ADC_Channel_5, ADC_SampleTime_239_5Cycles);
    // SetChannelSampleTime(ADC_Channel_6, ADC_SampleTime_239_5Cycles);
    // SetChannelSampleTime(ADC_Channel_7, ADC_SampleTime_239_5Cycles);
    // SetChannelSampleTime(ADC_Channel_14, ADC_SampleTime_239_5Cycles);
    // SetChannelSampleTime(ADC_Channel_15, ADC_SampleTime_239_5Cycles);
        
    // while (1)
    // {
    //     UART_PC_Send("before convertion... ");
    //     do {
    //         dummy16 = ADC1->DR;
    //     }
    //     while (ADC1->SR & ADC_SR_EOC);

    //     UART_PC_Send("no EOC\n");
            
    //     ConvertChannel(ADC_Channel_4);

    //     while (!(ADC1->SR & ADC_SR_EOC));
    //     sprintf(buffSendErr, "DR: %d dummy16: %d\n", (unsigned short) (ADC1->DR), dummy16);
    //     UART_PC_Send(buffSendErr);            
            
    //     Wait_ms(1000);
    // }
    //--- End Test ADC Single conversion ----------------------------------//                    

    //--- Test ADC Multiple conversion Scanning Mode ----------------------------------//
    // SetChannelSampleTime(ADC_Channel_4, ADC_SampleTime_239_5Cycles);
    // SetChannelSampleTime(ADC_Channel_5, ADC_SampleTime_239_5Cycles);
    // SetChannelSampleTime(ADC_Channel_6, ADC_SampleTime_239_5Cycles);
    // SetChannelSampleTime(ADC_Channel_7, ADC_SampleTime_239_5Cycles);
    // SetChannelSampleTime(ADC_Channel_14, ADC_SampleTime_239_5Cycles);
    // SetChannelSampleTime(ADC_Channel_15, ADC_SampleTime_239_5Cycles);

    // SetChannelSamplePosition(ADC_Channel_4, 1);
    // SetChannelSamplePosition(ADC_Channel_5, 2);
    // SetChannelSamplePosition(ADC_Channel_6, 3);
    // SetChannelSamplePosition(ADC_Channel_7, 4);
    // SetChannelSamplePosition(ADC_Channel_14, 5);
    // SetChannelSamplePosition(ADC_Channel_15, 6);

    // SetChannelsQuantity(ADC_Channels_Qtty_6);
    // // SetChannelsQuantity(ADC_Channels_Qtty_1);        

    // ADC1->CR1 |= ADC_CR1_SCAN;    //convertir toda la secuencia de canales
    // // ADC1->CR2 |= ADC_CR2_CONT;    //convertir en forma continua
        
    // //activo primera conversion por las dudas        
    // if (ADC1->CR2 & ADC_CR2_ADON)
    // {
    //     UART_PC_Send("Adon is on\n");
    //     //activo una primera conversion
    //     ADC1->CR2 |= ADC_CR2_SWSTART | ADC_CR2_EXTTRIG;
    // }

    // while (1)
    // {
    //     if (ADC1->SR & ADC_SR_EOC)
    //     {
    //         LED2_ON;
    //         dummy16 = (unsigned short) ADC1->DR;
    //         sprintf(buffSendErr, "last ch: %d\n", dummy16);
    //         UART_PC_Send(buffSendErr);
    //         Wait_ms(100);
                
    //         //reviso el bit
    //         if (!(ADC1->SR & ADC_SR_EOC))
    //         {
    //             ADC1->CR2 |= ADC_CR2_SWSTART | ADC_CR2_EXTTRIG;
    //             LED2_OFF;
    //         }
    //     }
    // }        
    //--- End Test ADC Multiple conversion Scanning Mode ----------------------------------//

    //--- Test ADC Multiple conversion Scanning Mode and DMA ------------------------------//
    //-- DMA configuration.
    // DMAConfig();
    // DMA1_Channel1->CCR |= DMA_CCR1_EN;
        
    // SetChannelSampleTime(ADC_Channel_4, ADC_SampleTime_239_5Cycles);
    // SetChannelSampleTime(ADC_Channel_5, ADC_SampleTime_239_5Cycles);
    // SetChannelSampleTime(ADC_Channel_6, ADC_SampleTime_239_5Cycles);
    // SetChannelSampleTime(ADC_Channel_7, ADC_SampleTime_239_5Cycles);
    // SetChannelSampleTime(ADC_Channel_14, ADC_SampleTime_239_5Cycles);
    // SetChannelSampleTime(ADC_Channel_15, ADC_SampleTime_239_5Cycles);

    // SetChannelSamplePosition(ADC_Channel_4, 1);
    // SetChannelSamplePosition(ADC_Channel_5, 2);
    // SetChannelSamplePosition(ADC_Channel_6, 3);
    // SetChannelSamplePosition(ADC_Channel_7, 4);
    // SetChannelSamplePosition(ADC_Channel_14, 5);
    // SetChannelSamplePosition(ADC_Channel_15, 6);

    // SetChannelsQuantity(ADC_Channels_Qtty_6);

    // ADC1->CR1 |= ADC_CR1_SCAN;    //convertir toda la secuencia de canales
    // // ADC1->CR2 |= ADC_CR2_CONT;    //convertir en forma continua
        
    // //activo primera conversion por las dudas        
    // if (ADC1->CR2 & ADC_CR2_ADON)
    // {
    //     UART_PC_Send("Adon is on\n");
    //     //activo una primera conversion
    //     ADC1->CR2 |= ADC_CR2_SWSTART | ADC_CR2_EXTTRIG;
    // }

    // while (1)
    // {
    //     if (sequence_ready)
    //     {
    //         LED2_ON;
    //         sprintf(buffSendErr, "IS1: %d, IS2: %d, IS3: %d, IS4: %d\n", IS1, IS2, IS3, IS4);
    //         UART_PC_Send(buffSendErr);
    //         Wait_ms(100);
    //         sprintf(buffSendErr, "V200: %d, V40: %d\n", V200_Sense, V40_Sense);
    //         UART_PC_Send(buffSendErr);            
    //         Wait_ms(100);
                
    //         ADC1->CR2 |= ADC_CR2_SWSTART | ADC_CR2_EXTTRIG;
    //         LED2_OFF;
    //         sequence_ready_reset;
    //     }
    // }
    //--- End Test ADC Multiple conversion Scanning Mode and DMA --------------------------//        

    //--- Test ADC Multiple conversion Scanning Continuous Mode and DMA -------------------//
    // //-- DMA configuration.
    // DMAConfig();
    // DMA1_Channel1->CCR |= DMA_CCR1_EN;
        
    // SetChannelSampleTime(ADC_Channel_4, ADC_SampleTime_239_5Cycles);
    // SetChannelSampleTime(ADC_Channel_5, ADC_SampleTime_239_5Cycles);
    // SetChannelSampleTime(ADC_Channel_6, ADC_SampleTime_239_5Cycles);
    // SetChannelSampleTime(ADC_Channel_7, ADC_SampleTime_239_5Cycles);
    // SetChannelSampleTime(ADC_Channel_14, ADC_SampleTime_239_5Cycles);
    // SetChannelSampleTime(ADC_Channel_15, ADC_SampleTime_239_5Cycles);

    // SetChannelSamplePosition(ADC_Channel_4, 1);
    // SetChannelSamplePosition(ADC_Channel_5, 2);
    // SetChannelSamplePosition(ADC_Channel_6, 3);
    // SetChannelSamplePosition(ADC_Channel_7, 4);
    // SetChannelSamplePosition(ADC_Channel_14, 5);
    // SetChannelSamplePosition(ADC_Channel_15, 6);

    // SetChannelsQuantity(ADC_Channels_Qtty_6);

    // ADC1->CR1 |= ADC_CR1_SCAN;    //convertir toda la secuencia de canales
    // ADC1->CR2 |= ADC_CR2_CONT;    //convertir en forma continua
        
    // //activo primera conversion por las dudas        
    // if (ADC1->CR2 & ADC_CR2_ADON)
    // {
    //     UART_PC_Send("Adon is on\n");
    //     //activo una primera conversion
    //     ADC1->CR2 |= ADC_CR2_SWSTART | ADC_CR2_EXTTRIG;
    // }

    // while (1)
    // {
    //     Wait_ms(1000);
    //     sprintf(buffSendErr, "IS1: %d, IS2: %d, IS3: %d, IS4: %d\n", IS1, IS2, IS3, IS4);
    //     UART_PC_Send(buffSendErr);
    //     Wait_ms(1000);
    //     sprintf(buffSendErr, "V200: %d, V40: %d\n", V200_Sense, V40_Sense);
    //     UART_PC_Send(buffSendErr);            
    // }
    //--- End Test ADC Multiple conversion Scanning Continuous Mode and DMA ----------------//        
        
        
    // while (1);
    // PWM1_Init();
    // PWM2_Init();
    // PWM3_Init();
    // PWM4_Init();

    //Initialize channels.
    Channel_1_Init();
    Channel_2_Init();
    Channel_3_Init();
    Channel_4_Init();

    LED1_OFF;

//---- Defines from GTK_Hard.h -----//
    UART_PC_Send("\r\nGausstek Limited Inc. -- Magnet Equipment\r\n");
    UART_PC_Send("powered by: Kirno Technology\r\n");

#ifdef HARD
    UART_PC_Send(HARD);
#else
#error	"No Hardware defined in GTK_Hard.h file"
#endif

#ifdef SOFT
    UART_PC_Send(SOFT);
#else
#error	"No Soft Version defined in GTK_Hard.h file"
#endif

#ifdef ANTENNA_HARDCODED
    UART_PC_Send("Antenna HardCoded!!!\r\n");
#endif
#ifdef NO_ERRORS_IN_RISING
    UART_PC_Send("No errors in rising\r\n");
#endif
    UART_PC_Send("Ready!\r\n");
    // BuzzerCommands(BUZZER_MULTIPLE_SHORT, 1);
//---- End of Defines from GTK_Hard.h -----//

    //mando ENABLE los canales
    ENA_CH1_ON;
    ENA_CH2_ON;
    ENA_CH3_ON;
    ENA_CH4_ON;
    Wait_ms(10);
    ENA_CH1_OFF;
    ENA_CH2_OFF;
    ENA_CH3_OFF;
    ENA_CH4_OFF;

    while (1)
    {
        AntennaUpdateStates ();

        //ADC control.
        Session_Current_Limit_control ();

        //Channel 1.
        Session_Channel_1 ();
        UART_CH1_Receive();

        //Channel 2.
        // SetBitGlobalErrors (CH2, BIT_ERROR_ANTENNA);
        // AntennaVerifyForTreatment(CH2);
        Session_Channel_2 ();
        UART_CH2_Receive();

        //Channel 3.
        // SetBitGlobalErrors (CH3, BIT_ERROR_ANTENNA);
        // AntennaVerifyForTreatment(CH3);        
        Session_Channel_3 ();
        UART_CH3_Receive();

        //Channel 4.
        // SetBitGlobalErrors (CH4, BIT_ERROR_ANTENNA);
        // AntennaVerifyForTreatment(CH4);        
        Session_Channel_4 ();
        UART_CH4_Receive();

        //Recepcion de la configuracion por PC.
        UART_PC_Receive();

        //Chequeo de errores globales
        CheckforGlobalErrors();

#ifdef USE_BUZZER_ON_BOARD
        //Funciones del Buzzer
        UpdateBuzzer();
#endif

    }
}


//--- end of file ---//
