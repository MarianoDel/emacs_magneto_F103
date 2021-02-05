//---------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    TEST PLATFORM FOR FIRMWARE
// ##
// #### TESTS.C ###############################
//---------------------------------------------

// Includes Modules for tests --------------------------------------------------
#include "comms_channels.h"
#include "GTK_Estructura.h"

#include <stdio.h>
#include <string.h>

// Module Types Constants and Macros -------------------------------------------


// Externals -------------------------------------------------------------------
volatile unsigned char usart2_have_data = 0;
volatile unsigned char usart3_have_data = 0;
volatile unsigned char usart4_have_data = 0;
volatile unsigned char usart5_have_data = 0;
antenna_typedef antenna_st;

char s_ok [] = {"OK\r\n"};
char s_nok [] = {"ERROR\r\n"};


// Globals ---------------------------------------------------------------------
char mock_str [256] = { 0 };



// Module Functions to Test ----------------------------------------------------
// Mock for usart module
unsigned char ReadUsart2Buffer (unsigned char * str, unsigned short len);
unsigned char ReadUsart3Buffer (unsigned char * str, unsigned short len);
unsigned char ReadUart4Buffer (unsigned char * str, unsigned short len);
unsigned char ReadUart5Buffer (unsigned char * str, unsigned short len);
void Usart1Send (char * str);
void Usart2Send (char * str);
void Usart3Send (char * str);
void Uart4Send (char * str);
void Uart5Send (char * str);

// Mock for antennas module
void AntennaIsAnswering (unsigned char ch);
void AntennaSetParamsStruct (unsigned char ch, antenna_typedef *ant);
void AntennaSetCurrentTemp (unsigned char ch, unsigned char t_int, unsigned char t_dec);
void AntennaSetName (unsigned char ch, char * pname);

// Module Functions ------------------------------------------------------------
int main (int argc, char *argv[])
{
    // test for temps
    // strcpy(mock_str, "temp,055.00\r\n");
    // strcpy(mock_str, "temp,0X5.00\r\n");
    strcpy(mock_str, "temp,055.e0\r\n");

    // test for params
    // strcpy(mock_str, "ant0,012.27,087.90,001.80,065.00\r\n");
    // strcpy(mock_str, "ant0,012.27,087.90,001.80,0F5.00\r\n");

    // test for keepalive answer
    // strcpy(mock_str, "ok\r");

    // test for antenna name
    // strcpy(mock_str, "name:Tunnel 12\"\r");
    usart2_have_data = 1;
    UART2_Receive();

    usart3_have_data = 1;
    UART3_Receive();

    usart4_have_data = 1;
    UART4_Receive();

    usart5_have_data = 1;
    UART5_Receive();
    
    
    return 0;
}


// Mock Functions --------------------------------------------------------------
unsigned char ReadUsart2Buffer (unsigned char * str, unsigned short len)
{
    unsigned char mock_len = 0;
    mock_len = strlen(mock_str);

    if (len >= mock_len)
        strcpy(str, mock_str);
    else
    {
        strncpy(str, mock_str, len);
        mock_len = len;
    }

    return mock_len;
}


unsigned char ReadUsart3Buffer (unsigned char * str, unsigned short len)
{
    unsigned char mock_len = 0;
    mock_len = strlen(mock_str);

    if (len >= mock_len)
        strcpy(str, mock_str);
    else
    {
        strncpy(str, mock_str, len);
        mock_len = len;
    }

    return mock_len;
}


unsigned char ReadUart4Buffer (unsigned char * str, unsigned short len)
{
    unsigned char mock_len = 0;
    mock_len = strlen(mock_str);

    if (len >= mock_len)
        strcpy(str, mock_str);
    else
    {
        strncpy(str, mock_str, len);
        mock_len = len;
    }

    return mock_len;
}


unsigned char ReadUart5Buffer (unsigned char * str, unsigned short len)
{
    unsigned char mock_len = 0;
    mock_len = strlen(mock_str);

    if (len >= mock_len)
        strcpy(str, mock_str);
    else
    {
        strncpy(str, mock_str, len);
        mock_len = len;
    }

    return mock_len;
}


void Usart1Send (char * str)
{
    printf("Send to PC: %s\n", str);
}


void Usart2Send (char * str)
{
    printf("Send to CH1: %s\n", str);
}


void Usart3Send (char * str)
{
    printf("Send to CH2: %s\n", str);
}


void Uart4Send (char * str)
{
    printf("Send to CH3: %s\n", str);
}


void Uart5Send (char * str)
{
    printf("Send to CH4: %s\n", str);
}


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

//--- end of file ---//


