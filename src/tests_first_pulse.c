//---------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    TEST PLATFORM FOR FIRMWARE
// ##
// #### TESTS_FIRST_PULSE.C ###################
//---------------------------------------------

// Includes Modules for tests --------------------------------------------------
#include "first_pulse.h"
#include "answers_defs.h"
#include "antennas.h"
#include "GTK_Estructura.h"

// Include Auxiliary modules ---------------------------------------------------
#include "tests_ok.h"

#include <stdio.h>
#include <string.h>
#include <math.h>


// Module Types Constants and Macros -------------------------------------------


// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------
unsigned short session_warming_up_channel_1_time = 0;
unsigned short session_warming_up_channel_2_time = 0;
unsigned short session_warming_up_channel_3_time = 0;
unsigned short session_warming_up_channel_4_time = 0;

unsigned short session_warming_up_channel_1_time_2 = 0;
unsigned short session_warming_up_channel_2_time_2 = 0;
unsigned short session_warming_up_channel_3_time_2 = 0;
unsigned short session_warming_up_channel_4_time_2 = 0;

// for current meas
unsigned short actual_current [5] = { 0 };

unsigned char new_current_sample_ch1 = 0;
unsigned char new_current_sample_ch2 = 0;
unsigned char new_current_sample_ch3 = 0;
unsigned char new_current_sample_ch4 = 0;


session_typedef session_ch_1;
session_typedef session_ch_2;
session_typedef session_ch_3;
session_typedef session_ch_4;




// Module Tester Functions -----------------------------------------------------
void Tests_First_Pulse_Sequence (void);
void Tests_First_Pulse_Sequence_Out (void);

void Tests_First_Pulse_Channel_1_Sequence (void);
void Tests_First_Pulse_Channel_2_Sequence (void);
void Tests_First_Pulse_Channel_3_Sequence (void);
void Tests_First_Pulse_Channel_4_Sequence (void);

void Tests_First_Pulse_Shuffle_Channels_Sequence (void);
void Tests_First_Pulse_Shuffle_Channels_Sequence_Out (void);


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

// void AntennaEndTreatment (unsigned char a);
// void Usart1Send (char * send);
// void Wait_ms (unsigned short a);
// void BuzzerCommands (unsigned char a, unsigned char b);

// void Error_SetString (char * p_errstr, unsigned short err_num);
// unsigned char ConvertSingleChannelFinishFlag (void);
// void ConvertChannel (unsigned char a);

unsigned char AntennaVerifyForTreatment (unsigned char a);
unsigned char AntennaGetCurrentTemp (unsigned char a);
unsigned char AntennaGetConnection (unsigned char a);
void AntennaGetParamsStruct (unsigned char ch, antenna_typedef *ant);

// Module Functions ------------------------------------------------------------
int main (int argc, char *argv[])
{
    // Tests_First_Pulse_Sequence ();
    Tests_First_Pulse_Sequence_Out ();
    
    // Tests_First_Pulse_Channel_1_Sequence ();
    // Tests_First_Pulse_Channel_2_Sequence ();
    // Tests_First_Pulse_Channel_3_Sequence ();
    // Tests_First_Pulse_Channel_4_Sequence ();

    // Tests_First_Pulse_Shuffle_Channels_Sequence ();
    // Tests_First_Pulse_Shuffle_Channels_Sequence_Out ();    
        
    return 0;
}


// Tester Functions ------------------------------------------------------------
void Tests_First_Pulse_Sequence (void)
{
    unsigned char answer = TRABAJANDO;
    
    printf("Testing First Pulse Sequence...\n");

    printf("Fixing antenna params...\n");
// const char s_antena [] = { "ant0,012.27,087.90,001.80,065.00\r\n" };        
    session_ch_1.ant_resistance_int = 12;
    session_ch_1.ant_resistance_dec = 27;
    session_ch_1.ant_inductance_int = 87;
    session_ch_1.ant_inductance_dec = 90;
    session_ch_1.ant_current_limit_int = 1;
    session_ch_1.ant_current_limit_dec = 80;
    session_ch_1.ant_temp_max_int = 65;
    session_ch_1.ant_temp_max_dec = 0;


    unsigned char channel = 1;
    while (answer == TRABAJANDO)
    {
        answer = FirstPulseCheck (channel);

        session_warming_up_channel_1_time++;
        session_warming_up_channel_2_time++;
        session_warming_up_channel_3_time++;
        session_warming_up_channel_4_time++;

        new_current_sample_ch1 = 1;
    }

    printf("channel%d time is: %d sequence ended with: ",
           channel,
           session_warming_up_channel_1_time
        );

    if (answer == FIN_OK)
    {
        PrintOK();
    }
    else if (answer == FIN_ERROR)
    {
        PrintERR();
        printf("sequence ended with FIN_ERROR\n");
    }
    
}


void Tests_First_Pulse_Sequence_Out (void)
{
    unsigned char answer = TRABAJANDO;
    
    printf("Testing First Pulse Sequence Out...\n");

    printf("Fixing antenna params...\n");
// const char s_antena [] = { "ant0,012.27,087.90,001.80,065.00\r\n" };        
    session_ch_1.ant_resistance_int = 12;
    session_ch_1.ant_resistance_dec = 27;
    session_ch_1.ant_inductance_int = 87;
    session_ch_1.ant_inductance_dec = 90;
    session_ch_1.ant_current_limit_int = 1;
    session_ch_1.ant_current_limit_dec = 80;
    session_ch_1.ant_temp_max_int = 65;
    session_ch_1.ant_temp_max_dec = 0;


    unsigned char channel = 1;
    actual_current[1] = 100;    //base current 100
    while (answer == TRABAJANDO)
    {
        answer = FirstPulseCheck (channel);

        session_warming_up_channel_1_time++;
        session_warming_up_channel_2_time++;
        session_warming_up_channel_3_time++;
        session_warming_up_channel_4_time++;

        new_current_sample_ch1 = 1;
        if (session_warming_up_channel_1_time == 100)
        {
            // actual_current[1] = 670;    //err, base current 0
            // actual_current[1] = 671;    //ok, base current 0
            actual_current[1] = 770;    //err, base current 100
            // actual_current[1] = 771;    //ok, base current 100            
        }
    }

    printf("channel%d step time: %d sequence ended with: ",
           channel,
           session_warming_up_channel_1_time
        );

    if (answer == FIN_OK)
    {
        PrintOK();
    }
    else if (answer == FIN_ERROR)
    {
        PrintERR();
        printf("sequence ended with FIN_ERROR\n");
    }
    
}


void Tests_First_Pulse_Channel_1_Sequence (void)
{
    unsigned char answer = TRABAJANDO;
    
    printf("Testing First Pulse Channel 1 Sequence...\n");

    printf("Fixing channel 1 antenna params...\n");
// const char s_antena [] = { "ant0,012.27,087.90,001.80,065.00\r\n" };        
    session_ch_1.ant_resistance_int = 12;
    session_ch_1.ant_resistance_dec = 27;
    session_ch_1.ant_inductance_int = 87;
    session_ch_1.ant_inductance_dec = 90;
    session_ch_1.ant_current_limit_int = 1;
    session_ch_1.ant_current_limit_dec = 80;
    session_ch_1.ant_temp_max_int = 65;
    session_ch_1.ant_temp_max_dec = 0;

    float res = 0.0;
    float ind = 0.0;
    
    float five_tau_ch1 = 0.0;

    res = session_ch_1.ant_resistance_int + session_ch_1.ant_resistance_dec / 100.0;
    ind = (session_ch_1.ant_inductance_int + session_ch_1.ant_inductance_dec / 100.0) / 1000.0;
    five_tau_ch1 = (ind / res) * 5;

    unsigned short ch1_time_steps = (unsigned short) (five_tau_ch1 * 10000);
    printf("ch1 5 Tau local: %f s ch1_time_steps local: %d\n",
           five_tau_ch1,
           ch1_time_steps
        );

    unsigned char channel = 1;
    while (answer == TRABAJANDO)
    {
        answer = FirstPulseCheck (channel);

        session_warming_up_channel_1_time++;
        // session_warming_up_channel_2_time++;
        // session_warming_up_channel_3_time++;
        // session_warming_up_channel_4_time++;
    }

    printf("channel%d step time: %d sequence ended with: ",
           channel,
           session_warming_up_channel_1_time
        );

    // check the error and the sequence out
    if ((answer == FIN_ERROR) &&
        (session_warming_up_channel_1_time == (ch1_time_steps + 1)))
    {
        PrintOK();
    }
    else
    {
        PrintERR();
        printf("sequence ended with %d\n", session_warming_up_channel_1_time);
    }
    
}


void Tests_First_Pulse_Channel_2_Sequence (void)
{
    unsigned char answer = TRABAJANDO;
    
    printf("Testing First Pulse Channel 2 Sequence...\n");

    printf("Fixing channel 2 antenna params...\n");
// const char s_antena [] = { "ant0,012.27,087.90,001.80,065.00\r\n" };        
    session_ch_2.ant_resistance_int = 12;
    session_ch_2.ant_resistance_dec = 27;
    session_ch_2.ant_inductance_int = 87;
    session_ch_2.ant_inductance_dec = 90;
    session_ch_2.ant_current_limit_int = 1;
    session_ch_2.ant_current_limit_dec = 80;
    session_ch_2.ant_temp_max_int = 65;
    session_ch_2.ant_temp_max_dec = 0;

    float res = 0.0;
    float ind = 0.0;
    
    float five_tau_ch2 = 0.0;

    res = session_ch_2.ant_resistance_int + session_ch_2.ant_resistance_dec / 100.0;
    ind = (session_ch_2.ant_inductance_int + session_ch_2.ant_inductance_dec / 100.0) / 1000.0;
    five_tau_ch2 = (ind / res) * 5;

    unsigned short ch2_time_steps = (unsigned short) (five_tau_ch2 * 10000);
    printf("ch2 5 Tau local: %f s ch2_time_steps local: %d\n",
           five_tau_ch2,
           ch2_time_steps
        );

    unsigned char channel = 2;
    while (answer == TRABAJANDO)
    {
        answer = FirstPulseCheck (channel);
        session_warming_up_channel_2_time++;
    }

    printf("channel%d step time: %d sequence ended with: ",
           channel,
           session_warming_up_channel_2_time
        );

    // check the error and the sequence out
    if ((answer == FIN_ERROR) &&
        (session_warming_up_channel_2_time == (ch2_time_steps + 1)))
    {
        PrintOK();
    }
    else
    {
        PrintERR();
        printf("sequence ended with %d\n", session_warming_up_channel_2_time);
    }
}


void Tests_First_Pulse_Channel_3_Sequence (void)
{
    unsigned char answer = TRABAJANDO;
    
    printf("Testing First Pulse Channel 3 Sequence...\n");

    printf("Fixing channel 3 antenna params...\n");
// const char s_antena [] = { "ant0,012.27,087.90,001.80,065.00\r\n" };        
    session_ch_3.ant_resistance_int = 12;
    session_ch_3.ant_resistance_dec = 27;
    session_ch_3.ant_inductance_int = 87;
    session_ch_3.ant_inductance_dec = 90;
    session_ch_3.ant_current_limit_int = 1;
    session_ch_3.ant_current_limit_dec = 80;
    session_ch_3.ant_temp_max_int = 65;
    session_ch_3.ant_temp_max_dec = 0;

    float res = 0.0;
    float ind = 0.0;
    
    float five_tau_ch3 = 0.0;

    res = session_ch_3.ant_resistance_int + session_ch_3.ant_resistance_dec / 100.0;
    ind = (session_ch_3.ant_inductance_int + session_ch_3.ant_inductance_dec / 100.0) / 1000.0;
    five_tau_ch3 = (ind / res) * 5;

    unsigned short ch3_time_steps = (unsigned short) (five_tau_ch3 * 10000);
    printf("ch3 5 Tau local: %f s ch3_time_steps local: %d\n",
           five_tau_ch3,
           ch3_time_steps
        );

    unsigned char channel = 3;
    while (answer == TRABAJANDO)
    {
        answer = FirstPulseCheck (channel);
        session_warming_up_channel_3_time++;
    }

    printf("channel%d step time: %d sequence ended with: ",
           channel,
           session_warming_up_channel_3_time
        );

    // check the error and the sequence out
    if ((answer == FIN_ERROR) &&
        (session_warming_up_channel_3_time == (ch3_time_steps + 1)))
    {
        PrintOK();
    }
    else
    {
        PrintERR();
        printf("sequence ended with %d\n", session_warming_up_channel_3_time);
    }
}


void Tests_First_Pulse_Channel_4_Sequence (void)
{
    unsigned char answer = TRABAJANDO;
    
    printf("Testing First Pulse Channel 4 Sequence...\n");

    printf("Fixing channel 4 antenna params...\n");
// const char s_antena [] = { "ant0,012.27,087.90,001.80,065.00\r\n" };        
    session_ch_4.ant_resistance_int = 12;
    session_ch_4.ant_resistance_dec = 27;
    session_ch_4.ant_inductance_int = 87;
    session_ch_4.ant_inductance_dec = 90;
    session_ch_4.ant_current_limit_int = 1;
    session_ch_4.ant_current_limit_dec = 80;
    session_ch_4.ant_temp_max_int = 65;
    session_ch_4.ant_temp_max_dec = 0;

    float res = 0.0;
    float ind = 0.0;
    
    float five_tau_ch4 = 0.0;

    res = session_ch_4.ant_resistance_int + session_ch_4.ant_resistance_dec / 100.0;
    ind = (session_ch_4.ant_inductance_int + session_ch_4.ant_inductance_dec / 100.0) / 1000.0;
    five_tau_ch4 = (ind / res) * 5;

    unsigned short ch4_time_steps = (unsigned short) (five_tau_ch4 * 10000);
    printf("ch4 5 Tau local: %f s ch4_time_steps local: %d\n",
           five_tau_ch4,
           ch4_time_steps
        );

    unsigned char channel = 4;
    while (answer == TRABAJANDO)
    {
        answer = FirstPulseCheck (channel);
        session_warming_up_channel_4_time++;
    }

    printf("channel%d step time: %d sequence ended with: ",
           channel,
           session_warming_up_channel_4_time
        );

    // check the error and the sequence out
    if ((answer == FIN_ERROR) &&
        (session_warming_up_channel_4_time == (ch4_time_steps + 1)))
    {
        PrintOK();
    }
    else
    {
        PrintERR();
        printf("sequence ended with %d\n", session_warming_up_channel_4_time);
    }
}


void Tests_First_Pulse_Shuffle_Channels_Sequence (void)
{
    unsigned char answer = TRABAJANDO;
    
    printf("Testing First Pulse Shuffle Sequence...\n");

    printf("Fixing channel 1 antenna params...\n");
// const char s_antena [] = { "ant0,012.27,087.90,001.80,065.00\r\n" };        
    session_ch_1.ant_resistance_int = 12;
    session_ch_1.ant_resistance_dec = 27;
    session_ch_1.ant_inductance_int = 87;
    session_ch_1.ant_inductance_dec = 90;
    session_ch_1.ant_current_limit_int = 1;
    session_ch_1.ant_current_limit_dec = 80;
    session_ch_1.ant_temp_max_int = 65;
    session_ch_1.ant_temp_max_dec = 0;

    printf("Fixing channel 2 antenna params...\n");
// const char s_antena [] = { "ant0,012.27,087.90,001.80,065.00\r\n" };        
    session_ch_2.ant_resistance_int = 12;
    session_ch_2.ant_resistance_dec = 27;
    session_ch_2.ant_inductance_int = 87;
    session_ch_2.ant_inductance_dec = 90;
    session_ch_2.ant_current_limit_int = 1;
    session_ch_2.ant_current_limit_dec = 80;
    session_ch_2.ant_temp_max_int = 65;
    session_ch_2.ant_temp_max_dec = 0;

    printf("Fixing channel 3 antenna params...\n");
// const char s_antena [] = { "ant0,012.27,087.90,001.80,065.00\r\n" };        
    session_ch_3.ant_resistance_int = 12;
    session_ch_3.ant_resistance_dec = 27;
    session_ch_3.ant_inductance_int = 87;
    session_ch_3.ant_inductance_dec = 90;
    session_ch_3.ant_current_limit_int = 1;
    session_ch_3.ant_current_limit_dec = 80;
    session_ch_3.ant_temp_max_int = 65;
    session_ch_3.ant_temp_max_dec = 0;

    printf("Fixing channel 4 antenna params...\n");
// const char s_antena [] = { "ant0,012.27,087.90,001.80,065.00\r\n" };        
    session_ch_4.ant_resistance_int = 12;
    session_ch_4.ant_resistance_dec = 27;
    session_ch_4.ant_inductance_int = 87;
    session_ch_4.ant_inductance_dec = 90;
    session_ch_4.ant_current_limit_int = 1;
    session_ch_4.ant_current_limit_dec = 80;
    session_ch_4.ant_temp_max_int = 65;
    session_ch_4.ant_temp_max_dec = 0;
    
    float res = 0.0;
    float ind = 0.0;
    
    float five_tau_ch_1 = 0.0;
    float five_tau_ch_2 = 0.0;
    float five_tau_ch_3 = 0.0;
    float five_tau_ch_4 = 0.0;    

    res = session_ch_1.ant_resistance_int + session_ch_1.ant_resistance_dec / 100.0;
    ind = (session_ch_1.ant_inductance_int + session_ch_1.ant_inductance_dec / 100.0) / 1000.0;
    five_tau_ch_1 = (ind / res) * 5;

    res = session_ch_2.ant_resistance_int + session_ch_2.ant_resistance_dec / 100.0;
    ind = (session_ch_2.ant_inductance_int + session_ch_2.ant_inductance_dec / 100.0) / 1000.0;
    five_tau_ch_2 = (ind / res) * 5;

    res = session_ch_3.ant_resistance_int + session_ch_3.ant_resistance_dec / 100.0;
    ind = (session_ch_3.ant_inductance_int + session_ch_3.ant_inductance_dec / 100.0) / 1000.0;
    five_tau_ch_3 = (ind / res) * 5;

    res = session_ch_4.ant_resistance_int + session_ch_4.ant_resistance_dec / 100.0;
    ind = (session_ch_4.ant_inductance_int + session_ch_4.ant_inductance_dec / 100.0) / 1000.0;
    five_tau_ch_4 = (ind / res) * 5;
    
    unsigned short ch_1_time_steps = (unsigned short) (five_tau_ch_1 * 10000);
    printf("ch1 5 Tau local: %f s ch1_time_steps local: %d\n",
           five_tau_ch_1,
           ch_1_time_steps
        );

    unsigned short ch_2_time_steps = (unsigned short) (five_tau_ch_2 * 10000);
    printf("ch1 5 Tau local: %f s ch1_time_steps local: %d\n",
           five_tau_ch_2,
           ch_2_time_steps
        );

    unsigned short ch_3_time_steps = (unsigned short) (five_tau_ch_3 * 10000);
    printf("ch1 5 Tau local: %f s ch1_time_steps local: %d\n",
           five_tau_ch_3,
           ch_3_time_steps
        );

    unsigned short ch_4_time_steps = (unsigned short) (five_tau_ch_4 * 10000);
    printf("ch1 5 Tau local: %f s ch1_time_steps local: %d\n",
           five_tau_ch_4,
           ch_4_time_steps
        );
    
    unsigned char channel = 1;
    unsigned char end_channels = 0;
    while (answer == TRABAJANDO)
    {
        answer = FirstPulseCheck (channel);

        if (answer == FIN_ERROR)
        {
            printf("FIN_ERROR on channel: %d ", channel);
            switch (channel)
            {
            case 1:
                printf("sequence ended with %d\n", session_warming_up_channel_1_time);
                break;
            case 2:
                printf("sequence ended with %d\n", session_warming_up_channel_2_time);
                break;
            case 3:
                printf("sequence ended with %d\n", session_warming_up_channel_3_time);                
                break;
            case 4:
                printf("sequence ended with %d\n", session_warming_up_channel_4_time);
                break;
            }
            
            end_channels |= (1 << channel);
            answer = TRABAJANDO;
        }

        if (end_channels == 0x1e)
            answer = FIN_OK;
        else
        {
            // sequence between the not in error channels
            unsigned char actual_channel = 0;
            do {
                if (channel < 4)
                    channel++;
                else
                    channel = 1;

                actual_channel = (1 << channel);            
            } while (actual_channel & end_channels);
        }

        session_warming_up_channel_1_time++;
        session_warming_up_channel_2_time++;
        session_warming_up_channel_3_time++;
        session_warming_up_channel_4_time++;

    }
}


void Tests_First_Pulse_Shuffle_Channels_Sequence_Out (void)
{
    unsigned char answer = TRABAJANDO;
    
    printf("Testing First Pulse Shuffle Sequence...\n");

    printf("Fixing channel 1 antenna params...\n");
// const char s_antena [] = { "ant0,012.27,087.90,001.80,065.00\r\n" };        
    session_ch_1.ant_resistance_int = 12;
    session_ch_1.ant_resistance_dec = 27;
    session_ch_1.ant_inductance_int = 87;
    session_ch_1.ant_inductance_dec = 90;
    session_ch_1.ant_current_limit_int = 1;
    session_ch_1.ant_current_limit_dec = 80;
    session_ch_1.ant_temp_max_int = 65;
    session_ch_1.ant_temp_max_dec = 0;

    printf("Fixing channel 2 antenna params...\n");
// const char s_antena [] = { "ant0,012.27,087.90,001.80,065.00\r\n" };        
    session_ch_2.ant_resistance_int = 12;
    session_ch_2.ant_resistance_dec = 27;
    session_ch_2.ant_inductance_int = 87;
    session_ch_2.ant_inductance_dec = 90;
    session_ch_2.ant_current_limit_int = 1;
    session_ch_2.ant_current_limit_dec = 80;
    session_ch_2.ant_temp_max_int = 65;
    session_ch_2.ant_temp_max_dec = 0;

    printf("Fixing channel 3 antenna params...\n");
// const char s_antena [] = { "ant0,012.27,087.90,001.80,065.00\r\n" };        
    session_ch_3.ant_resistance_int = 12;
    session_ch_3.ant_resistance_dec = 27;
    session_ch_3.ant_inductance_int = 87;
    session_ch_3.ant_inductance_dec = 90;
    session_ch_3.ant_current_limit_int = 1;
    session_ch_3.ant_current_limit_dec = 80;
    session_ch_3.ant_temp_max_int = 65;
    session_ch_3.ant_temp_max_dec = 0;

    printf("Fixing channel 4 antenna params...\n");
// const char s_antena [] = { "ant0,012.27,087.90,001.80,065.00\r\n" };        
    session_ch_4.ant_resistance_int = 12;
    session_ch_4.ant_resistance_dec = 27;
    session_ch_4.ant_inductance_int = 87;
    session_ch_4.ant_inductance_dec = 90;
    session_ch_4.ant_current_limit_int = 1;
    session_ch_4.ant_current_limit_dec = 80;
    session_ch_4.ant_temp_max_int = 65;
    session_ch_4.ant_temp_max_dec = 0;
    
    float res = 0.0;
    float ind = 0.0;
    
    float five_tau_ch_1 = 0.0;
    float five_tau_ch_2 = 0.0;
    float five_tau_ch_3 = 0.0;
    float five_tau_ch_4 = 0.0;    

    res = session_ch_1.ant_resistance_int + session_ch_1.ant_resistance_dec / 100.0;
    ind = (session_ch_1.ant_inductance_int + session_ch_1.ant_inductance_dec / 100.0) / 1000.0;
    five_tau_ch_1 = (ind / res) * 5;

    res = session_ch_2.ant_resistance_int + session_ch_2.ant_resistance_dec / 100.0;
    ind = (session_ch_2.ant_inductance_int + session_ch_2.ant_inductance_dec / 100.0) / 1000.0;
    five_tau_ch_2 = (ind / res) * 5;

    res = session_ch_3.ant_resistance_int + session_ch_3.ant_resistance_dec / 100.0;
    ind = (session_ch_3.ant_inductance_int + session_ch_3.ant_inductance_dec / 100.0) / 1000.0;
    five_tau_ch_3 = (ind / res) * 5;

    res = session_ch_4.ant_resistance_int + session_ch_4.ant_resistance_dec / 100.0;
    ind = (session_ch_4.ant_inductance_int + session_ch_4.ant_inductance_dec / 100.0) / 1000.0;
    five_tau_ch_4 = (ind / res) * 5;
    
    unsigned short ch_1_time_steps = (unsigned short) (five_tau_ch_1 * 10000);
    printf("ch1 5 Tau local: %f s ch1_time_steps local: %d\n",
           five_tau_ch_1,
           ch_1_time_steps
        );

    unsigned short ch_2_time_steps = (unsigned short) (five_tau_ch_2 * 10000);
    printf("ch1 5 Tau local: %f s ch1_time_steps local: %d\n",
           five_tau_ch_2,
           ch_2_time_steps
        );

    unsigned short ch_3_time_steps = (unsigned short) (five_tau_ch_3 * 10000);
    printf("ch1 5 Tau local: %f s ch1_time_steps local: %d\n",
           five_tau_ch_3,
           ch_3_time_steps
        );

    unsigned short ch_4_time_steps = (unsigned short) (five_tau_ch_4 * 10000);
    printf("ch1 5 Tau local: %f s ch1_time_steps local: %d\n",
           five_tau_ch_4,
           ch_4_time_steps
        );

    int print_loop_seq = 0;
    int print_loop_seq_cnt = 0;
    int print_loop_seq_event = 0;
    unsigned char channel = 1;
    unsigned char end_channels = 0;
    while (answer == TRABAJANDO)
    {
        answer = FirstPulseCheck (channel);

        if (answer == FIN_ERROR)
        {
            if (!print_loop_seq)
            {
                printf("FIN_ERROR on channel: %d ", channel);
                switch (channel)
                {
                case 1:
                    printf("sequence ended with %d\n", session_warming_up_channel_1_time);
                    break;
                case 2:
                    printf("sequence ended with %d\n", session_warming_up_channel_2_time);
                    break;
                case 3:
                    printf("sequence ended with %d\n", session_warming_up_channel_3_time);                
                    break;
                case 4:
                    printf("sequence ended with %d\n", session_warming_up_channel_4_time);
                    break;
                }
            }

            end_channels |= (1 << channel);            
            answer = TRABAJANDO;

            if (print_loop_seq)
                print_loop_seq_event = channel;
            else
                print_loop_seq_event = 0;
        }

        if (answer == FIN_OK)
        {
            if (!print_loop_seq)
            {
                printf("FIN_OK on channel: %d ", channel);
                switch (channel)
                {
                case 1:
                    printf("sequence ended with %d\n", session_warming_up_channel_1_time);
                    break;
                case 2:
                    printf("sequence ended with %d\n", session_warming_up_channel_2_time);
                    break;
                case 3:
                    printf("sequence ended with %d\n", session_warming_up_channel_3_time);                
                    break;
                case 4:
                    printf("sequence ended with %d\n", session_warming_up_channel_4_time);
                    break;
                }
            }
            
            end_channels |= (1 << channel);
            answer = TRABAJANDO;

            if (print_loop_seq)
                print_loop_seq_event = channel;
            else
                print_loop_seq_event = 0;
            
        }

        if (end_channels == 0x1e)
            answer = FIN_OK;
        else
        {
            // sequence between the not in ended channels
            unsigned char actual_channel = 0;
            do {
                if (channel < 4)
                    channel++;
                else
                    channel = 1;

                actual_channel = (1 << channel);            
            } while (actual_channel & end_channels);
        }

        session_warming_up_channel_1_time++;
        session_warming_up_channel_2_time++;
        session_warming_up_channel_3_time++;
        session_warming_up_channel_4_time++;

        if (session_warming_up_channel_1_time == 100)
        {
            new_current_sample_ch1 = 1;
            actual_current[1] = 1;
        }

        if (session_warming_up_channel_2_time == 125)
        {
            new_current_sample_ch2 = 1;
            actual_current[2] = 1;
        }

        if (session_warming_up_channel_3_time == 150)
        {
            new_current_sample_ch3 = 1;
            actual_current[3] = 1;
        }

        if (session_warming_up_channel_4_time == 200)
        {
            new_current_sample_ch4 = 1;
            actual_current[4] = 1;
        }

        if (print_loop_seq)
        {
            if (((print_loop_seq_cnt + 1) % 50) != 0)
            {
                if (print_loop_seq_event)
                {
                    printf("%d", print_loop_seq_event);
                    print_loop_seq_event = 0;
                }
                else
                    printf(".");
            }
            else
            {
                if (print_loop_seq_event)
                {
                    printf("%d\t(%d)\n", print_loop_seq_event);
                    print_loop_seq_event = 0;
                }
                else
                    printf(".\t(%d)\n", print_loop_seq_cnt + 1);
            }
            
            print_loop_seq_cnt++;
        }
    }

    printf("\n");
    
}





// Mocked Functions for module compilation -------------------------------------
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
    switch (ch)
    {
    case 1:
        ant->resistance_int = session_ch_1.ant_resistance_int;
        ant->resistance_dec = session_ch_1.ant_resistance_dec;
        ant->inductance_int = session_ch_1.ant_inductance_int;
        ant->inductance_dec = session_ch_1.ant_inductance_dec;
        ant->current_limit_int = session_ch_1.ant_current_limit_int;
        ant->current_limit_dec = session_ch_1.ant_current_limit_dec;
        ant->temp_max_int = session_ch_1.ant_temp_max_int;
        ant->temp_max_dec = session_ch_1.ant_temp_max_dec;        
        break;

    case 2:
        ant->resistance_int = session_ch_2.ant_resistance_int;
        ant->resistance_dec = session_ch_2.ant_resistance_dec;
        ant->inductance_int = session_ch_2.ant_inductance_int;
        ant->inductance_dec = session_ch_2.ant_inductance_dec;
        ant->current_limit_int = session_ch_2.ant_current_limit_int;
        ant->current_limit_dec = session_ch_2.ant_current_limit_dec;
        ant->temp_max_int = session_ch_2.ant_temp_max_int;
        ant->temp_max_dec = session_ch_2.ant_temp_max_dec;                
        break;

    case 3:
        ant->resistance_int = session_ch_3.ant_resistance_int;
        ant->resistance_dec = session_ch_3.ant_resistance_dec;
        ant->inductance_int = session_ch_3.ant_inductance_int;
        ant->inductance_dec = session_ch_3.ant_inductance_dec;
        ant->current_limit_int = session_ch_3.ant_current_limit_int;
        ant->current_limit_dec = session_ch_3.ant_current_limit_dec;
        ant->temp_max_int = session_ch_3.ant_temp_max_int;
        ant->temp_max_dec = session_ch_3.ant_temp_max_dec;                
        break;

    case 4:
        ant->resistance_int = session_ch_4.ant_resistance_int;
        ant->resistance_dec = session_ch_4.ant_resistance_dec;
        ant->inductance_int = session_ch_4.ant_inductance_int;
        ant->inductance_dec = session_ch_4.ant_inductance_dec;
        ant->current_limit_int = session_ch_4.ant_current_limit_int;
        ant->current_limit_dec = session_ch_4.ant_current_limit_dec;
        ant->temp_max_int = session_ch_4.ant_temp_max_int;
        ant->temp_max_dec = session_ch_4.ant_temp_max_dec;                
        break;
    }        
    
}


//--- end of file ---//


