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
#include "answers_defs.h"
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

char mock_str [256];
char s_ok [] = {"OK\r\n"};
char s_nok [] = {"ERROR\r\n"};

int channel1_connect = 0;
int channel2_connect = 0;
int channel3_connect = 0;
int channel4_connect = 0;

// Globals ---------------------------------------------------------------------
antenna_typedef antenna_ch4;
char antenna_name_ch4 [20] = { 0 };


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


// Functions for Tests
void Test_Treatment (void);
void Test_Reconnect (void);

// Module Functions ------------------------------------------------------------
int main (int argc, char *argv[])
{
 
   // Test_Reconnect();
    Test_Treatment();
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

    if (channel1_connect)
    {
        if (!strcmp(str, "get_params\r\n"))
        {
            printf("Usart2 sending params\n");
            antenna_typedef antenna_st;

            antenna_st.resistance_int = 20;
            antenna_st.resistance_dec = 0;
            antenna_st.inductance_int = 20;
            antenna_st.inductance_dec = 0;
            antenna_st.current_limit_int = 4;
            antenna_st.current_limit_dec = 4;
            antenna_st.temp_max_int = 65;
            antenna_st.temp_max_dec = 0;

            AntennaSetParamsStruct(CH1, &antenna_st);
        
        }

        if (!strcmp(str, "get_name\r\n"))
        {
            char s_name [] = {"Tunnel 12 inches"};
            printf("Usart2 sending name\n");
            AntennaSetName(CH1, s_name);
        }

        if (!strcmp(str, "keepalive\r\n"))
        {
            printf("Usart2 keepalive\n");
            AntennaIsAnswering(CH1);
        }
    }
}


void Usart3Send (char * str)
{
    printf("Send to CH2: %s\n", str);

    if (channel2_connect)
    {
        if (!strcmp(str, "get_params\r\n"))
        {
            printf("Usart3 sending params\n");
            antenna_typedef antenna_st;

            antenna_st.resistance_int = 20;
            antenna_st.resistance_dec = 0;
            antenna_st.inductance_int = 20;
            antenna_st.inductance_dec = 0;
            antenna_st.current_limit_int = 4;
            antenna_st.current_limit_dec = 4;
            antenna_st.temp_max_int = 65;
            antenna_st.temp_max_dec = 0;

            AntennaSetParamsStruct(CH2, &antenna_st);
        
        }

        if (!strcmp(str, "keepalive\r\n"))
        {
            printf("Usart3 keepalive\n");
            AntennaIsAnswering(CH2);
        }
    }
}


void Uart4Send (char * str)
{
    printf("Send to CH3: %s\n", str);
}


void Uart5Send (char * str)
{
    printf("Send to CH4: %s\n", str);

    if (channel4_connect)
    {
        if (!strcmp(str, "get_params\r\n"))
        {
            printf("Usart5 sending params\n");
            AntennaSetParamsStruct(CH4, &antenna_ch4);
        }

        if (!strcmp(str, "get_name\r\n"))
        {
            printf("Usart5 sending name\n");
            AntennaSetName(CH4, antenna_name_ch4);
        }

        if (!strcmp(str, "keepalive\r\n"))
        {
            printf("Usart5 keepalive\n");
            AntennaIsAnswering(CH4);
        }
    }
}


void Test_Reconnect (void)
{
    printf("Test Antenna Reconnect...\n\n");
    
    printf("Antenna SM simulation...\n");
    printf(" CH4 connect with name\n");
    printf(" CH4 disconnect\n");
    printf(" CH4 connect with other name\n");

    channel1_connect = 0;
    channel2_connect = 0;
    channel3_connect = 0;
    channel4_connect = 0;
    
    for (int j = 0; j < 40; j++)
    {
        AntennaUpdateStates();

        if (antenna_timer)
            antenna_timer = 0;

        if (antenna_info_timer)
            antenna_info_timer = 0;

        if (j == 5)
        {
            antenna_ch4.resistance_int = 20;
            antenna_ch4.resistance_dec = 0;
            antenna_ch4.inductance_int = 20;
            antenna_ch4.inductance_dec = 0;
            antenna_ch4.current_limit_int = 4;
            antenna_ch4.current_limit_dec = 4;
            antenna_ch4.temp_max_int = 65;
            antenna_ch4.temp_max_dec = 0;

            strcpy(antenna_name_ch4, "Tunnel 10 inches");
            
            channel4_connect = 1;
        }

        if (j == 15)
            channel4_connect = 0;

        if (j == 35)
        {
            strcpy(antenna_name_ch4, "palette");
            channel4_connect = 1;
        }
    }
}


void Test_Treatment (void)
{
    printf("Test Antenna Treatment...\n\n");
    
    printf("Antenna SM simulation...\n");
    printf(" CH1 antenna connected with name\n");
    printf(" CH2 antenna connected with name\n");
    printf(" CH3 no antenna\n");
    printf(" CH4 no antenna\n");

    channel1_connect = 1;
    channel2_connect = 1;
    channel3_connect = 0;
    channel4_connect = 0;
    
    for (int j = 0; j < 20; j++)
    {
        AntennaUpdateStates();

        if (antenna_timer)
            antenna_timer = 0;

        if (antenna_info_timer)
            antenna_info_timer = 0;
        
    }

    printf(" CH4 antenna connected with name\n");
    strcpy(antenna_name_ch4, "Tunnel 10 inches");
    channel4_connect = 1;

    antenna_typedef antenna_st;    
    antenna_st.resistance_int = 20;
    antenna_st.resistance_dec = 0;
    antenna_st.inductance_int = 20;
    antenna_st.inductance_dec = 0;
    antenna_st.current_limit_int = 4;
    antenna_st.current_limit_dec = 4;
    antenna_st.temp_max_int = 65;
    antenna_st.temp_max_dec = 0;
    
    printf("Usart5 sending params\n");
    AntennaSetParamsStruct(CH4, &antenna_st);
    
    for (int j = 0; j < 20; j++)
    {
        AntennaUpdateStates();

        if (antenna_timer)
            antenna_timer = 0;

        if (antenna_info_timer)
            antenna_info_timer = 0;

        if (j == 10)
        {
            if (AntennaVerifyForTreatment(CH1) != FIN_OK)
                printf("CH1 not ready for treatment\n");

            if (AntennaVerifyForTreatment(CH2) != FIN_OK)
                printf("CH2 not ready for treatment\n");

            if (AntennaVerifyForTreatment(CH3) != FIN_OK)
                printf("CH3 not ready for treatment\n");

            if (AntennaVerifyForTreatment(CH4) != FIN_OK)
                printf("CH4 not ready for treatment\n");
        }
    }    
}
//--- end of file ---//


