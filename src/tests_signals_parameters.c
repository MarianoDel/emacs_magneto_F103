//---------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    TEST PLATFORM FOR FIRMWARE
// ##
// #### TESTS_ERRORS_STR.C ####################
//---------------------------------------------

// Includes Modules for tests --------------------------------------------------
#include "GTK_Signal.h"
#include "answers_defs.h"

// Include Auxiliary modules ---------------------------------------------------
#include "tests_ok.h"

#include <stdio.h>
#include <string.h>


// Module Types Constants and Macros -------------------------------------------


// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------
char buffSendErr[64];

unsigned char channel_1_pause;
unsigned char channel_2_pause;
unsigned char channel_3_pause;
unsigned char channel_4_pause;

session_typedef session_ch_1;
session_typedef session_ch_2;
session_typedef session_ch_3;
session_typedef session_ch_4;

//la uso  para comprobar el funcionamiento de TIM5 desde el main
volatile unsigned int session_warming_up_channel_1_stage_time;

//--- Current limit Externals ---//
volatile unsigned char take_current_samples;



// Module Tester Functions -----------------------------------------------------
void Tests_Parameters (void);


// Module Mocked Functions -----------------------------------------------------
void Update_TIM1_CH1 (unsigned short a);
void Update_TIM1_CH4 (unsigned short a);

void Update_TIM2_CH1 (unsigned short a);
void Update_TIM2_CH2 (unsigned short a);

void Update_TIM3_CH1 (unsigned short a);
void Update_TIM3_CH2 (unsigned short a);
void Update_TIM3_CH3 (unsigned short a);
void Update_TIM3_CH4 (unsigned short a);

void Update_TIM4_CH1 (unsigned short a);
void Update_TIM4_CH2 (unsigned short a);
void Update_TIM4_CH3 (unsigned short a);
void Update_TIM4_CH4 (unsigned short a);

void AntennaEndTreatment (unsigned char a);
void Usart1Send (char * send);
void Wait_ms (unsigned short a);
void BuzzerCommands (unsigned char a, unsigned char b);

void Error_SetString (char * p_errstr, unsigned short err_num);
unsigned char ConvertSingleChannelFinishFlag (void);
void ConvertChannel (unsigned char a);

unsigned char AntennaVerifyForTreatment (unsigned char a);
unsigned char AntennaGetCurrentTemp (unsigned char a);
unsigned char AntennaGetConnection (unsigned char a);
void AntennaGetParamsStruct (unsigned char ch, antenna_typedef *ant);

// Module Functions ------------------------------------------------------------
int main (int argc, char *argv[])
{
    Tests_Parameters ();
        
    return 0;
}


// Tester Functions ------------------------------------------------------------
void Tests_Parameters (void)
{
    printf("Testing Parameters Calculations...\n");

    printf("Parameters Calculation sequence: ");

    unsigned char i = 0;
    unsigned char channel = 1;
    // answers with FIN_OK or FIN_ERROR
    i = Session_Channels_Parameters_Calculate(channel, WARMING_UP);

    if (i == FIN_OK)
    {
        PrintOK();
    }
    else if (i == FIN_ERROR)
    {
        PrintERR();
        printf("sequence ended with FIN_ERROR\n");
    }
    
}
    

// Mocked for module compilation
void Update_TIM1_CH1 (unsigned short a)
{
}

void Update_TIM1_CH4 (unsigned short a)
{
}

void Update_TIM2_CH1 (unsigned short a)
{
}

void Update_TIM2_CH2 (unsigned short a)
{
}

void Update_TIM3_CH1 (unsigned short a)
{
}

void Update_TIM3_CH2 (unsigned short a)
{
}

void Update_TIM3_CH3 (unsigned short a)
{
}

void Update_TIM3_CH4 (unsigned short a)
{
}

void Update_TIM4_CH1 (unsigned short a)
{
}

void Update_TIM4_CH2 (unsigned short a)
{
}

void Update_TIM4_CH3 (unsigned short a)
{
}

void Update_TIM4_CH4 (unsigned short a)
{
}

void AntennaEndTreatment (unsigned char a)
{
}

void Usart1Send (char * send)
{
}

void Wait_ms (unsigned short a)
{
}

void BuzzerCommands (unsigned char a, unsigned char b)
{
}

void Error_SetString (char * p_errstr, unsigned short err_num)
{
}

unsigned char ConvertSingleChannelFinishFlag (void)
{
    return 1;
}

void ConvertChannel (unsigned char a)
{
}

unsigned char AntennaVerifyForTreatment (unsigned char a)
{
    return 1;
}

unsigned char AntennaGetCurrentTemp (unsigned char a)
{
    return 1;
}

unsigned char AntennaGetConnection (unsigned char a)
{
    return 1;
}

void AntennaGetParamsStruct (unsigned char ch, antenna_typedef *ant)
{
}

//--- end of file ---//


