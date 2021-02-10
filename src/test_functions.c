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

#include "antennas.h"

#include <stdio.h>


// Externals -------------------------------------------------------------------
extern volatile unsigned short antenna_timer;

// Globals ---------------------------------------------------------------------


// Module Private Types & Macros -----------------------------------------------


// Module Private Functions ----------------------------------------------------


// Module Functions ------------------------------------------------------------
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



//--- end of file ---//
