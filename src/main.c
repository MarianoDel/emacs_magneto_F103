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

#include "comms.h"
#include "comms_channels.h"
#include "test_functions.h"

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

char s_ok [] = {"OK\r\n"};
char s_nok [] = {"ERROR\r\n"};

//--- Externals para enviar keepalive por UART (nuevo en modulo antenna)
volatile unsigned short antenna_timer = 0;
volatile unsigned short antenna_info_timer = 0;


//--- Externals para muestreos de corriente con el ADC
volatile unsigned char take_current_samples = 0;
volatile unsigned short adc_ch [ADC_CHANNEL_QUANTITY];

//--- Externals para armar seniales y comprobar el TIM5 en el inicio del programa
volatile unsigned int session_warming_up_channel_1_stage_time = 0;

//--- Externals para el BUZZER
#ifdef USE_BUZZER_ON_BOARD
volatile unsigned short buzzer_timeout = 0;
#endif
volatile unsigned short timer_led = 0;

//Estructuras.
session_typedef session_slot_aux;

session_typedef session_ch_1;
session_typedef session_ch_2;
session_typedef session_ch_3;
session_typedef session_ch_4;

unsigned char channel_1_pause = 0;
unsigned char channel_2_pause = 0;
unsigned char channel_3_pause = 0;
unsigned char channel_4_pause = 0;

/* Globals --------------------------------------------------------------------*/


// Module Private Functions ----------------------------------------------------
void CheckLEDs (void);


// Module Functions ------------------------------------------------------------
int main (void)
{
    // Clock Configuration
    // SystemInit() is called from startup_stm32f10x_hd.s before main()

    //Configure all gpios and alternative functions pins
    GpioInit();
    LED2_OFF;
    LED3_OFF;

    // --- Begin Hardware Tests ----
    // TF_Hardware_Tests ();    
    // --- End of Hardware Tests ---

    //Timer 1ms -- Wait_ms()
    TIM7_Init();
        
    //Timer 100ms.
    TIM6_Init();

    //Signal timer -- 100us
    TIM5_Init();


    // Verify Hardware Clock (Xtal frequency)
    // LED1 Ton = 100ms Toff = 100ms T = 200ms
    // otherwise change hardware xtal or select the correct one
    // works with TIM5 at 100us
    for (unsigned char i = 0; i < 20; i++)
    {
        if (LED1)
            LED1_OFF;
        else
            LED1_ON;

        while (session_warming_up_channel_1_stage_time != 0);
        
        session_warming_up_channel_1_stage_time = 1000;	//100ms
    }

    // ADC1 Config
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
#ifdef USE_CURRENT_SLOT_WITHOUT_LOAD_CMD
    UART_PC_Send("Start without Load activated\r\n");
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
        Session_Channel_2 ();
        UART_CH2_Receive();

        //Channel 3.
        Session_Channel_3 ();
        UART_CH3_Receive();

        //Channel 4.
        Session_Channel_4 ();
        UART_CH4_Receive();

        //Recepcion de la configuracion por PC.
        UART_PC_Receive();

        //Chequeo de errores globales
        CheckforGlobalErrors();

        //Check LED Status
        CheckLEDs();

#ifdef USE_BUZZER_ON_BOARD
        //Funciones del Buzzer
        UpdateBuzzer();
#endif

    }
}


unsigned char in_treatment = 1;
void CheckLEDs (void)
{
    if ((session_ch_1.status == 0) &&
        (session_ch_2.status == 0) &&
        (session_ch_3.status == 0) &&
        (session_ch_4.status == 0))
    {
        //no treatment ongoing
        if (in_treatment)
        {
            ChangeLed(LED_TREATMENT_STANDBY);
            in_treatment = 0;
        }
    }

    if ((session_ch_1.status != 0) ||
        (session_ch_2.status != 0) ||
        (session_ch_3.status != 0) ||
        (session_ch_4.status != 0))
    {
        if (!in_treatment)
        {
            ChangeLed(LED_TREATMENT_GENERATING);
            in_treatment = 1;
        }
    }

    UpdateLed();
}


//--- end of file ---//
