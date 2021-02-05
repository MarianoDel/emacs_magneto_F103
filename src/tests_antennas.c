//---------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    TEST PLATFORM FOR FIRMWARE
// ##
// #### TESTS.C ###############################
//---------------------------------------------

// Includes Modules for tests --------------------------------------------------
#include "antennas.h"
#include "channels_defs.h"
// #include "GTK_Estructura.h"

#include <stdio.h>
#include <string.h>

// Module Types Constants and Macros -------------------------------------------


// Externals -------------------------------------------------------------------
volatile unsigned char usart1_have_data = 0;
// volatile unsigned char usart2_have_data = 0;
// volatile unsigned char usart3_have_data = 0;
// volatile unsigned char usart4_have_data = 0;
// volatile unsigned char usart5_have_data = 0;

volatile unsigned short antenna_timer = 0;
volatile unsigned short antenna_info_timer = 0;

session_typedef session_ch_1;
session_typedef session_ch_2;
session_typedef session_ch_3;
session_typedef session_ch_4;

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

// Module Functions ------------------------------------------------------------
int main (int argc, char *argv[])
{

    printf("Test current temp settings...\n");
    unsigned char temp;
    for (int i = CH1; i <= CH4; i++)
    {
        AntennaSetCurrentTemp (i, 35, 0);
        temp = AntennaGetCurrentTemp (CH1);

        if (temp == 35)
            printf("OK CH%d antenna temp: %d\n", i, temp);
        else
            printf("ERROR CH%d antenna temp: %d\n", i, temp);
    }

    printf("Test antenna no connection...\n");
    unsigned char conn;
    for (int i = CH1; i <= CH4; i++)
    {
        conn = AntennaGetConnection (i);
        if (conn)
            printf("ERROR CH%d antenna conn: %d\n", i, conn);
        else
            printf("OK CH%d antenna conn: %d\n", i, conn);
    }

    printf("Test antenna params...\n");

    antenna_typedef antenna_st;
    antenna_st.resistance_int = 20;
    antenna_st.resistance_dec = 0;
    antenna_st.inductance_int = 20;
    antenna_st.inductance_dec = 0;
    antenna_st.current_limit_int = 4;
    antenna_st.current_limit_dec = 4;
    antenna_st.temp_max_int = 65;
    antenna_st.temp_max_dec = 0;

    for (int i = CH1; i <= CH4; i++)
        AntennaSetParamsStruct(i, &antenna_st);

    antenna_typedef antenna2_st;
    for (int i = CH1; i <= CH4; i++)
    {
        AntennaGetParamsStruct(i, &antenna2_st);

        if ((antenna_st.resistance_int == antenna2_st.resistance_int) &&
            (antenna_st.resistance_dec == antenna2_st.resistance_dec) &&
            (antenna_st.inductance_int == antenna2_st.inductance_int) &&
            (antenna_st.inductance_dec == antenna2_st.inductance_dec) &&
            (antenna_st.current_limit_int == antenna2_st.current_limit_int) &&
            (antenna_st.current_limit_dec == antenna2_st.current_limit_dec) &&
            (antenna_st.temp_max_int == antenna2_st.temp_max_int) &&
            (antenna_st.temp_max_dec == antenna2_st.temp_max_dec))
        {
            printf("OK Params for CH%d\n", i);
        }
        else
            printf("ERROR Params for CH%d\n", i);
    }
    

    printf("Test antenna connection not ready...\n");
    for (int i = CH1; i <= CH4; i++)
    {
        conn = AntennaGetConnection (i);
        if (!conn)
            printf("OK CH%d antenna not ready conn: %d\n", i, conn);
        else
            printf("ERROR CH%d antenna ready? conn: %d\n", i, conn);
    }
    
    printf("Test antenna verify for treat...\n");
    for (int i = CH1; i <= CH4; i++)
    {
        conn = AntennaVerifyForTreatment(i);

        if (conn)
            printf("Antenna ready on CH%d\n", i);
        else
            printf("Antenna not ready on CH%d\n", i);
    }    
    
    printf("Test antenna connection...\n");
    for (int i = CH1; i <= CH4; i++)
    {
        conn = AntennaGetConnection (i);
        if (conn)
            printf("OK CH%d antenna conn: %d\n", i, conn);
        else
            printf("ERROR CH%d antenna conn: %d\n", i, conn);
    }
        
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


// void AntennaSetCurrentTemp (unsigned char ch, unsigned char t_int, unsigned char t_dec)
// {
//     printf("Temp in antenna %d setted to %d.%d\n",
//            ch,
//            t_int,
//            t_dec);
// }


// void AntennaSetParamsStruct (unsigned char ch, antenna_typedef *ant)
// {
//     printf("Params in antenna %d setted:\n", ch);
//     printf("R: %d.%d\n", ant->resistance_int, ant->resistance_dec);
//     printf("L: %d.%d\n", ant->inductance_int, ant->inductance_dec);
//     printf("I: %d.%d\n", ant->current_limit_int, ant->current_limit_dec);
//     printf("T: %d.%d\n", ant->temp_max_int, ant->temp_max_dec);    
    
// }


// void AntennaIsAnswering (unsigned char ch)
// {
//     printf("Antenna answer on CH%d\n", ch);
// }


// void AntennaSetName (unsigned char ch, char * pname)
// {
//     printf("Antenna new name on CH%d: %s\n", ch, pname);
// }

//--- end of file ---//


