//---------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    TEST PLATFORM FOR FIRMWARE
// ##
// #### TESTS.C ###############################
//---------------------------------------------

// Includes Modules for tests --------------------------------------------------
#include "comms.h"
#include "GTK_Estructura.h"

#include "tests_ok.h"
#include <stdio.h>
#include <string.h>

// Module Types Constants and Macros -------------------------------------------


// Externals -------------------------------------------------------------------
volatile unsigned char usart1_have_data = 0;
// antenna_typedef antenna_st;

// externals for module to compile
char s_ok [] = {"OK\r\n"};
char s_nok [] = {"ERROR\r\n"};
session_typedef session_ch_1;
session_typedef session_ch_2;
session_typedef session_ch_3;
session_typedef session_ch_4;

session_typedef session_slot_aux;

unsigned char channel_1_pause;
unsigned char channel_2_pause;
unsigned char channel_3_pause;
unsigned char channel_4_pause;


// Globals ---------------------------------------------------------------------
char mock_received_str [256] = { 0 };
char mock_sended_str [256] = { 0 };



// Module Functions to Test ----------------------------------------------------
// Mock for usart module
unsigned char ReadUsart1Buffer (unsigned char * str, unsigned short len);
void Usart1Send (char * str);
void Usart1FillRxBuffer (char * msg);


// Mock Functions for module to compile ----------------------------------------
// from usart module
void Usart2Send (char * str);
void Usart3Send (char * str);
void Uart4Send (char * str);
void Uart5Send (char * str);
// from antennas module
void AntennaIsAnswering (unsigned char ch);
void AntennaSetParamsStruct (unsigned char ch, antenna_typedef *ant);
void AntennaSetCurrentTemp (unsigned char ch, unsigned char t_int, unsigned char t_dec);
void AntennaSetName (unsigned char ch, char * pname);
void AntennaGetParamsStruct (unsigned char ch, antenna_typedef *ant);
void AntennaSendKnowInfoWithTimer (void);
// from gtk_estructura module
void Session_Set_Signal (session_typedef * ptr_session, unsigned char stage, signal_typedef * ptr_signal);
void Session_Set_Duration (session_typedef * ptr_session, unsigned char stage , duration_typedef * ptr_duration);
void Session_Set_Status (session_typedef * ptr_session, unsigned char stage , unsigned char new_state);
// from gtk_signal module
void Session_Channel_1_Start(void);
void Session_Channel_2_Start(void);
void Session_Channel_3_Start(void);
void Session_Channel_4_Start(void);
void ResetCheckGlobalErrors (void);
void Current_Limit_Counter_Reset (void);
void StopAllChannels (void);
// from gtk_hard module
void ChangeLed (unsigned char how_many);
void BuzzerCommands(unsigned char command, unsigned char multiple);
// from adc module
void ConvertChannel (unsigned char ADC_Channel);
unsigned char ConvertSingleChannelFinishFlag (void);

// Private Tests Functions -----------------------------------------------------


// Module Functions ------------------------------------------------------------
int main (int argc, char *argv[])
{
    // test for durations
    Usart1FillRxBuffer ("duration,00,10,00,1\r\n");
    UART1_Receive ();

    printf("\nTesting duration string: ");
    if (strncmp (mock_sended_str, "OK\r\n", sizeof("OK\r\n") - 1) == 0)
        PrintOK ();
    else
        PrintERR ();

    // test for signal
    // square 83.33Hz o 86.22Hz
    Usart1FillRxBuffer ("signal,100,100,0000,0001,0004,0001,0006,0000,0000,1\r\n");
    UART1_Receive ();

    printf("\nTesting signal string: ");
    if (strncmp (mock_sended_str, "OK\r\n", sizeof("OK\r\n") - 1) == 0)
        PrintOK ();
    else
        PrintERR ();

    // triangular 83.33Hz o 86.22Hz
    Usart1FillRxBuffer ("signal,100,100,0000,0004,0001,0001,0006,0000,0000,1\r\n");
    UART1_Receive ();

    printf("\nTesting signal string: ");
    if (strncmp (mock_sended_str, "OK\r\n", sizeof("OK\r\n") - 1) == 0)
        PrintOK ();
    else
        PrintERR ();

    // sinusoidal 83.33Hz o 86.22Hz
    Usart1FillRxBuffer ("signal,100,100,0000,0002,0002,0002,0006,0000,0000,1\r\n");
    UART1_Receive ();

    printf("\nTesting signal string: ");
    if (strncmp (mock_sended_str, "OK\r\n", sizeof("OK\r\n") - 1) == 0)
        PrintOK ();
    else
        PrintERR ();

    // square 1Hz
    Usart1FillRxBuffer ("signal,100,100,0000,0001,0498,0001,0500,0000,0000,1\r\n");
    UART1_Receive ();

    printf("\nTesting signal string: ");
    if (strncmp (mock_sended_str, "OK\r\n", sizeof("OK\r\n") - 1) == 0)
        PrintOK ();
    else
        PrintERR ();

    // triangular 1Hz
    Usart1FillRxBuffer ("signal,100,100,0000,0498,0001,0001,0500,0000,0000,1\r\n");
    UART1_Receive ();

    printf("\nTesting signal string: ");
    if (strncmp (mock_sended_str, "OK\r\n", sizeof("OK\r\n") - 1) == 0)
        PrintOK ();
    else
        PrintERR ();

    // sinusoidal 1Hz
    Usart1FillRxBuffer ("signal,100,100,0000,0166,0167,0166,0500,0000,0000,1\r\n");
    UART1_Receive ();

    printf("\nTesting signal string: ");
    if (strncmp (mock_sended_str, "OK\r\n", sizeof("OK\r\n") - 1) == 0)
        PrintOK ();
    else
        PrintERR ();
    
    return 0;
}


// Mock Functions --------------------------------------------------------------
unsigned char ReadUsart1Buffer (unsigned char * str, unsigned short len)
{
    int mock_len = 0;
    mock_len = strlen(mock_received_str);

    if (len > mock_len)
        strcpy(str, mock_received_str);
    else
        printf("error on ReadUsart1Buffer max_len\n");

    return mock_len;
}


void Usart1Send (char * str)
{
    PrintYellow("usart1 tx -> ");
    printf("%s\n", str);
    strcpy(mock_sended_str, str);
}


void Usart1FillRxBuffer (char * msg)
{
    strcpy(mock_received_str, msg);
    PrintYellow("usart1 rx <- ");
    printf("%s\n", msg);
    usart1_have_data = 1;
}


// Mock Functions for module to compile ----------------------------------------
// from usart module
void Usart2Send (char * str)
{
    printf("in Usart2Send\n");
}


void Usart3Send (char * str)
{
    printf("in Usart3Send\n");    
}


void Uart4Send (char * str)
{
    printf("in Uart4Send\n");
}


void Uart5Send (char * str)
{
    printf("in Uart5Send\n");    
}

// from antennas module
void AntennaSetCurrentTemp (unsigned char ch, unsigned char t_int, unsigned char t_dec)
{
    printf("Temp in antenna %d setted to %d.%d\n",
           ch,
           t_int,
           t_dec);
}


void AntennaSetParamsStruct (unsigned char ch, antenna_typedef *ant)
{
    printf("Params in antenna %d setted:\n", ch);
    printf("R: %d.%d\n", ant->resistance_int, ant->resistance_dec);
    printf("L: %d.%d\n", ant->inductance_int, ant->inductance_dec);
    printf("I: %d.%d\n", ant->current_limit_int, ant->current_limit_dec);
    printf("T: %d.%d\n", ant->temp_max_int, ant->temp_max_dec);    
    
}


void AntennaIsAnswering (unsigned char ch)
{
    printf("Antenna answer on CH%d\n", ch);
}


void AntennaSetName (unsigned char ch, char * pname)
{
    printf("Antenna new name on CH%d: %s\n", ch, pname);
}


void AntennaGetParamsStruct (unsigned char ch, antenna_typedef *ant)
{
    printf("in AntennaGetParamsStruct\n");
}

void AntennaSendKnowInfoWithTimer (void)
{
    printf("in AntennaSendKnowInfoWithTimer\n");
}


// from gtk_estructura module
void Session_Set_Signal (session_typedef * ptr_session, unsigned char stage, signal_typedef * ptr_signal)
{
    printf("in Session_Set_Signal\n");
}


void Session_Set_Duration (session_typedef * ptr_session, unsigned char stage , duration_typedef * ptr_duration)
{
    printf("in Session_Set_Duration\n");
}


void Session_Set_Status (session_typedef * ptr_session, unsigned char stage , unsigned char new_state)
{
    printf("in Session_Set_Status\n");
}


// from gtk_signal module
void Session_Channel_1_Start(void)
{
    printf("in Session_Channel_1_Start\n");
}


void Session_Channel_2_Start(void)
{
    printf("in Session_Channel_2_Start\n");
}


void Session_Channel_3_Start(void)
{
    printf("in Session_Channel_3_Start\n");
}


void Session_Channel_4_Start(void)
{
    printf("in Session_Channel_4_Start\n");
}


void ResetCheckGlobalErrors (void)
{
    printf("in ResetCheckGlobalErrors\n");
}


void Current_Limit_Counter_Reset (void)
{
    printf("in Current_Limit_Counter_Reset\n");
}


void StopAllChannels (void)
{
    printf("in StopAllChannels\n");
}


// from gtk_hard module
void ChangeLed (unsigned char how_many)
{
    printf("in ChangeLed\n");
}


void BuzzerCommands(unsigned char command, unsigned char multiple)
{
    printf("in BuzzerCommands\n");    
}


// from adc module
void ConvertChannel (unsigned char ADC_Channel)
{
    printf("in ConvertChannel\n");
}


unsigned char ConvertSingleChannelFinishFlag (void)
{
    printf("in ConvertSingleChannelFinishFlag\n");
}

//--- end of file ---//


