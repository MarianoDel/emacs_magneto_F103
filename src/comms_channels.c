//---------------------------------------------------------
// #### PROYECTO MAGNETO GAUSSTEK - MotherBoard ###########
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F103
// ##
// #### COMMS_CHANNELS.C ##################################
//---------------------------------------------------------

// Includes --------------------------------------------------------------------
#include "comms_channels.h"
#include "channels_defs.h"
#include "usart.h"
// #include "GTK_Estructura.h"
// #include "GTK_Signal.h"
// #include "flash_program.h"
// #include "GTK_Hard.h"
// #include "timer.h"
#include "antennas.h"
#include "utils.h"

#include <string.h>
#include <stdio.h>



// Externals -------------------------------------------------------------------
//main.c para usart.c
// extern volatile unsigned char usart1_have_data;
extern volatile unsigned char usart2_have_data;
extern volatile unsigned char usart3_have_data;
extern volatile unsigned char usart4_have_data;
extern volatile unsigned char usart5_have_data;

//desde main.c
extern char s_ok [];
extern char s_nok [];


// Globals ---------------------------------------------------------------------
unsigned char localbuff2 [SIZEOF_RXDATA] = { '\0' };



// Module Private Functions ----------------------------------------------------
static comms_ch_answer_e ParseCurrentTemp (char * str, unsigned char * t_int, unsigned char * t_dec);
static comms_ch_answer_e ParseAntennaParams (char * str, antenna_typedef * antenna);



// Module Exported Functions ---------------------------------------------------

//channel 1
void UART2_Receive (void)
{
    if (usart2_have_data)
    {        
        ReadUsart2Buffer(localbuff2, SIZEOF_RXDATA);

        comms_ch_answer_e answer = nok_no_answer;
        answer = ParseCommsWithChannels((char *) localbuff2, CH1);

        if (answer == ok_answer)
            UART_CH1_Send(s_ok);

        if (answer == nok_answer)
            UART_CH1_Send(s_nok);
            
        usart2_have_data = 0;
    }
}


//channel 2
void UART3_Receive (void)
{
    if (usart3_have_data)
    {        
        ReadUsart3Buffer(localbuff2, SIZEOF_RXDATA);

        comms_ch_answer_e answer = nok_no_answer;
        answer = ParseCommsWithChannels((char *) localbuff2, CH2);

        if (answer == ok_answer)
            UART_CH2_Send(s_ok);

        if (answer == nok_answer)
            UART_CH2_Send(s_nok);
            
        usart3_have_data = 0;
    }
}


//channel 3
void UART4_Receive (void)
{
    if (usart4_have_data)
    {        
        ReadUart4Buffer(localbuff2, SIZEOF_RXDATA);

        comms_ch_answer_e answer = nok_no_answer;
        answer = ParseCommsWithChannels((char *) localbuff2, CH3);

        if (answer == ok_answer)
            UART_CH3_Send(s_ok);

        if (answer == nok_answer)
            UART_CH3_Send(s_nok);
            
        usart4_have_data = 0;
    }
}


//channel 4
void UART5_Receive (void)
{
    if (usart5_have_data)
    {        
        ReadUart5Buffer(localbuff2, SIZEOF_RXDATA);

        // unsigned char len = strlen((const char *)localbuff2);
        // if (len)
        //     UART_PC_Send((char *)localbuff2);


        comms_ch_answer_e answer = nok_no_answer;
        answer = ParseCommsWithChannels((char *) localbuff2, CH4);

        if (answer == ok_answer)
            UART_CH4_Send(s_ok);

        if (answer == nok_answer)
            UART_CH4_Send(s_nok);
            
        usart5_have_data = 0;
    }
}


comms_ch_answer_e ParseCommsWithChannels (char * str, unsigned char channel)
{
    comms_ch_answer_e error = nok_no_answer;
    char dummy_str [10] = { 0 };
    
    //temp,055.00\r\n
    if (!strncmp(str, (const char *)"temp", (sizeof("temp") - 1)))
    {
        if ((*(str + 4) == ',') &&
            (*(str + 8) == '.') &&
            (*(str + 11) == '\r'))
        {
            unsigned char temp_i = 0;
            unsigned char temp_d = 0;

            if (ParseCurrentTemp(str, &temp_i, &temp_d) == ok_answer)
            {
                AntennaSetCurrentTemp (channel, temp_i, temp_d);

                sprintf(dummy_str, ",%d\r\n", channel);
                strcpy((str + 11), dummy_str);
                UART_PC_Send(str);
                error = ok_answer;
            }
        }
    }

    //ant0,012.27,087.90,001.80,065.00\r\n.
    else if (!strncmp(str, (const char *)"ant", (sizeof("ant") - 1)))
    {
        sprintf(dummy_str, ",%d\r\n", channel);
        strcpy((str + 32), dummy_str);
        UART_PC_Send(str);

        if ((*(str + 4) == ',') &&
            (*(str + 11) == ',') &&
            (*(str + 18) == ',') &&
            (*(str + 25) == ','))
        {
            antenna_typedef antenna_aux;
            if (ParseAntennaParams ((char *) localbuff2, &antenna_aux) == ok_answer)
            {
                AntennaSetParamsStruct (channel, &antenna_aux);
                error = ok_answer;
            }
        }
    }

    //respuesta al keepalive
    else if (!strncmp(str, (const char *)"ok\r", (sizeof("ok\r") - 1)))
    {
        AntennaIsAnswering(channel);
        error = ok_no_answer;
    }

    else if ((!strncmp(str, (const char *)"name:", sizeof("name:") - 1)))
    {
        AntennaSetName(channel, (str + (sizeof("name:") - 1)));
        error = ok_no_answer;
    }

    return error;
}


static comms_ch_answer_e ParseAntennaParams (char * str, antenna_typedef * antenna)
{
    unsigned short i = 0;

    // check all the strings first, then do the load    
    if ((StringCheckNumbers((str + 5), 3) == 3) &&
        (StringCheckNumbers((str + 9), 2) == 2) &&
        (StringCheckNumbers((str + 12), 3) == 3) && 
        (StringCheckNumbers((str + 16), 2) == 2) &&
        (StringCheckNumbers((str + 19), 3) == 3) &&
        (StringCheckNumbers((str + 23), 2) == 2) &&
        (StringCheckNumbers((str + 26), 3) == 3) &&
        (StringCheckNumbers((str + 30), 2) == 2))
    {
        i = (*(str + 5) - '0') * 100;
        i += (*(str + 6) - '0') * 10;
        i += (*(str + 7) - '0');
        
        antenna->resistance_int = i;

        i = (*(str + 9) - '0') * 10;
        i += (*(str + 10) - '0');

        antenna->resistance_dec = i;

        i = (*(str + 12) - '0') * 100;
        i += (*(str + 13) - '0') * 10;
        i += (*(str + 14) - '0');

        antenna->inductance_int = i;

        i = (*(str + 16) - '0') * 10;
        i += (*(str + 17) - '0');

        antenna->inductance_dec = i;

        i = (*(str + 19) - '0') * 100;
        i += (*(str + 20) - '0') * 10;
        i += (*(str + 21) - '0');

        antenna->current_limit_int = i;

        i = (*(str + 23) - '0') * 10;
        i += (*(str + 24) - '0');

        antenna->current_limit_dec = i;

        i = (*(str + 26) - '0') * 100;
        i += (*(str + 27) - '0') * 10;
        i += (*(str + 28) - '0');

        antenna->temp_max_int = i;

        i = (*(str + 30) - '0') * 10;
        i += (*(str + 31) - '0');

        antenna->temp_max_dec = i;

        return ok_answer;
    }

    return nok_answer;
}


static comms_ch_answer_e ParseCurrentTemp (char * str, unsigned char * t_int, unsigned char * t_dec)
{
    unsigned short i = 0;
    unsigned short d = 0;

    if (StringCheckNumbers((str + 5), 3) == 3)
    {
        i = (*(str + 5) - '0') * 100;
        i += (*(str + 6) - '0') * 10;
        i += (*(str + 7) - '0');
        *t_int = i;
    }
    else
        return nok_answer;

    if (StringCheckNumbers((str + 9), 2) == 2)
    {
        d = (*(str + 9) - '0') * 10;
        d += (*(str + 10) - '0');
        *t_dec = d;
    }
    else
        return nok_answer;

    return ok_answer;
}


//---- end of file ----//
