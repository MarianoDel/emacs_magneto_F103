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



//--- end of file ---//
