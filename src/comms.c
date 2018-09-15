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
// #include "stm32f10x.h"
// #include "hard.h"

#include "comms.h"
#include "usart.h"
// #include "uart.h"
#include "GTK_Estructura.h"
#include "GTK_Signal.h"
#include "flash_program.h"
#include "GTK_Hard.h"
#include "timer.h"

#include <string.h>
#include <stdio.h>



/* Externals ------------------------------------------------------------------*/
//main.c para usart.c
extern volatile unsigned char usart1_have_data;
extern volatile unsigned char usart2_have_data;
extern volatile unsigned char usart3_have_data;
extern volatile unsigned char usart4_have_data;
extern volatile unsigned char usart5_have_data;

//desde main.c
extern session_typedef session_ch_1;
extern session_typedef session_ch_2;
extern session_typedef session_ch_3;
extern session_typedef session_ch_4;

extern session_typedef session_slot_aux;

extern unsigned char channel_1_pause;
extern unsigned char channel_2_pause;
extern unsigned char channel_3_pause;
extern unsigned char channel_4_pause;

extern unsigned char temp_actual_channel_1_int;
extern unsigned char temp_actual_channel_1_dec;
extern unsigned char temp_actual_channel_2_int;
extern unsigned char temp_actual_channel_2_dec;
extern unsigned char temp_actual_channel_3_int;
extern unsigned char temp_actual_channel_3_dec;
extern unsigned char temp_actual_channel_4_int;
extern unsigned char temp_actual_channel_4_dec;


/* Globals ------------------------------------------------------------------*/
unsigned char localbuff [SIZEOF_RXDATA] = { '\0' };


/* Module Private Functions -----------------------------------------------------------*/



/* Module Exported Functions -----------------------------------------------------------*/
//MODIFICADO MAGNETO V2.0 28-02-15 ANTES ERA USART3
//--- Recepcion de la PC
void UART1_Receive (void)
{
    unsigned char i;
    antenna_typedef 	antenna;
    signal_typedef 		signal;
    duration_typedef 	duration;
    //char buf[10];

    // if (msUART1rxTimeOut == 0)
    // {

    // 	MSUART1RXTIMEOUT;

    // 	pBuffUART1rxW = &buffUART1rx[0];
    // 	pBuffUART1rxR = &buffUART1rx[0];
    // }

    if (usart1_have_data)
    {
        ReadUsart1Buffer(localbuff, sizeof(localbuff));
        //--- Introduce your code here ---//
        UART_PC_Send((char *)&localbuff[0]);

        if (!strncmp((const char *)&localbuff[0], (const char *)"get_temp,", (sizeof("get_temp,") - 1)))
        {
            switch ((localbuff[9] - 48))
            {
            case 1:
                UART_CH1_Send("get_temp\r\n");
                UART_PC_Send("OK\r\n");
                break;

            case 2:
                UART_CH2_Send("get_temp\r\n");
                UART_PC_Send("OK\r\n");
                break;

            case 3:
                UART_CH3_Send("get_temp\r\n");
                UART_PC_Send("OK\r\n");
                break;

            case 4:
                UART_CH4_Send("get_temp\r\n");
                UART_PC_Send("OK\r\n");
                break;

            default:
                UART_PC_Send("ERROR\r\n");
                break;
            }
        }

        else if (!strncmp((const char *)&localbuff[0], (const char *)"get_antenna_con,", (sizeof("get_antenna_con,") - 1)))
        {
            //0 si no tengo, 1 si esta 1, 2 si est 2 "ant_con,0,2,0,0\r\n"
            /*
              sprintf((const char *)&localbuff[0], "ant_con,0,2,0,0\r\n",
              antenna.resistance_int,
              antenna.resistance_dec,
              antenna.inductance_int,
              antenna.inductance_dec,
              antenna.current_limit_int,
              antenna.current_limit_dec,
              antenna.temp_max_int,
              antenna.temp_max_dec);
              UART_PC_Send(localbuff);
              localbuff[0] = '\0';
            */
            UART_PC_Send((char *) "ant_con,0,2,0,0\r\n");
            localbuff[0] = '\0';
        }

        else if (!strncmp((const char *)&localbuff[0], (const char *)"get_antenna,", (sizeof("get_antenna,") - 1)))
        {
            //--- Get antenna parameters ---//
            Session_Get_Antenna (&session_ch_1, 1, &antenna);
            //strcpy((const char *)&localbuff[0], (const char *)"ant0,");
            //sprintf((const char *)&localbuff[0], "ant0,%03d.%02d,%03d.%02d,%03d.%02d,%03d.%02d,1\r\n",
            sprintf((char *)&localbuff[0], "ant0,%03d.%02d,%03d.%02d,%03d.%02d,%03d.%02d,1\r\n",
                    antenna.resistance_int,
                    antenna.resistance_dec,
                    antenna.inductance_int,
                    antenna.inductance_dec,
                    antenna.current_limit_int,
                    antenna.current_limit_dec,
                    antenna.temp_max_int,
                    antenna.temp_max_dec);

            //UART_PC_Send((const char *) localbuff);
            UART_PC_Send((char *) localbuff);
            localbuff[0] = '\0';

            Session_Get_Antenna (&session_ch_2, 1, &antenna);
            //strcpy((const char *)&localbuff[0], (const char *)"ant0,");
            sprintf((char *)&localbuff[0], "ant0,%03d.%02d,%03d.%02d,%03d.%02d,%03d.%02d,2\r\n",
                    antenna.resistance_int,
                    antenna.resistance_dec,
                    antenna.inductance_int,
                    antenna.inductance_dec,
                    antenna.current_limit_int,
                    antenna.current_limit_dec,
                    antenna.temp_max_int,
                    antenna.temp_max_dec);

            //UART_PC_Send((const char *) localbuff);
            UART_PC_Send((char *) localbuff);
            localbuff[0] = '\0';

            Session_Get_Antenna (&session_ch_3, 1, &antenna);
            //strcpy((const char *)&localbuff[0], (const char *)"ant0,");
            sprintf((char *)&localbuff[0], "ant0,%03d.%02d,%03d.%02d,%03d.%02d,%03d.%02d,3\r\n",
                    antenna.resistance_int,
                    antenna.resistance_dec,
                    antenna.inductance_int,
                    antenna.inductance_dec,
                    antenna.current_limit_int,
                    antenna.current_limit_dec,
                    antenna.temp_max_int,
                    antenna.temp_max_dec);

            UART_PC_Send((char *) localbuff);
            localbuff[0] = '\0';

            Session_Get_Antenna (&session_ch_4, 1, &antenna);
            //strcpy((const char *)&localbuff[0], (const char *)"ant0,");
            sprintf((char *)&localbuff[0], "ant0,%03d.%02d,%03d.%02d,%03d.%02d,%03d.%02d,4\r\n",
                    antenna.resistance_int,
                    antenna.resistance_dec,
                    antenna.inductance_int,
                    antenna.inductance_dec,
                    antenna.current_limit_int,
                    antenna.current_limit_dec,
                    antenna.temp_max_int,
                    antenna.temp_max_dec);

            UART_PC_Send((char *) localbuff);
            localbuff[0] = '\0';

            if (Get_Antenna_Name(CH1, (char *) &localbuff[0]) != 0)
            {
                UART_PC_Send((char *) (const char *) "ant_name,1,");
                UART_PC_Send((char *) localbuff);
                UART_PC_Send((char *) (const char *) "\r\n");
            }

            if (Get_Antenna_Name(CH2, (char *) &localbuff[0]) != 0)
            {
                UART_PC_Send((char *) (const char *) "ant_name,2,");
                UART_PC_Send((char *) localbuff);
                UART_PC_Send((char *) (const char *) "\r\n");
            }

            if (Get_Antenna_Name(CH3, (char *) &localbuff[0]) != 0)
            {
                UART_PC_Send((char *) (const char *) "ant_name,3,");
                UART_PC_Send((char *) localbuff);
                UART_PC_Send((char *) (const char *) "\r\n");
            }

            if (Get_Antenna_Name(CH4, (char *) &localbuff[0]) != 0)
            {
                UART_PC_Send((char *) (const char *) "ant_name,4,");
                UART_PC_Send((char *) localbuff);
                UART_PC_Send((char *) (const char *) "\r\n");
            }

            localbuff[0] = '\0';
        }

        else if (!strncmp((const char *)&localbuff[0], (const char *)"antenna,", (sizeof("antenna,") - 1)))
        {

            if (((localbuff[28] - 48) > 0) && ((localbuff[28] - 48) < 4) && (localbuff[27] == ','))
            {
                antenna.resistance_int = 	(localbuff[8] - 48) * 100 + (localbuff[9] - 48) * 10 + (localbuff[10] - 48);
                antenna.resistance_dec = 	(localbuff[12] - 48) * 10 + (localbuff[13] - 48);
                antenna.inductance_int = 	(localbuff[15] - 48) * 100 + (localbuff[16] - 48) * 10 + (localbuff[17] - 48);
                antenna.inductance_dec = 	(localbuff[19] - 48) * 10 + (localbuff[20] - 48);
                antenna.current_limit_int = (localbuff[22] - 48) * 10 + (localbuff[23] - 48);
                antenna.current_limit_dec = (localbuff[25] - 48) * 10 + (localbuff[26] - 48);

                //--- Save antenna parameters ---//
                Session_Set_Antenna (&session_slot_aux, (localbuff[28] - 48), &antenna);

                UART_PC_Send((char *)"OK\r\n");
            }
            else
                UART_PC_Send((char *)"ERROR\r\n");
        }
        //example. signal,100,100,0000,0003,0003,0003,0006,0000,0000,1
        else if (!strncmp((const char *)&localbuff[0], (const char *)"signal,", (sizeof("signal,") - 1)))
        {
            //signal,%03d,%03d,0%x%x%d,%04d,%04d,%04d,%04d,%04d,%04d,1\r\n",
            if (((localbuff[50] - 48) > 0) && ((localbuff[50] - 48) < 4) && (localbuff[49] == ','))
            {
                signal.initial_power = 		(localbuff[7] - 48) * 100 + (localbuff[8] - 48) * 10 + (localbuff[9] - 48);
                signal.final_power = 		(localbuff[11] - 48) * 100 + (localbuff[12] - 48) * 10 + (localbuff[13] - 48);
                signal.sync_on =  			(localbuff[18] - 48);
                //signal.step_number = 		(localbuff[15] - 48) * 1000 + (localbuff[16] - 48) * 100 + (localbuff[17] - 48) * 10 + (localbuff[18] - 48);
                signal.rising_time = 		(localbuff[20] - 48) * 1000 + (localbuff[21] - 48) * 100 + (localbuff[22] - 48) * 10 + (localbuff[23] - 48);
                signal.maintenance_time = 	(localbuff[25] - 48) * 1000 + (localbuff[26] - 48) * 100 + (localbuff[27] - 48) * 10 + (localbuff[28] - 48);
                signal.falling_time = 		(localbuff[30] - 48) * 1000 + (localbuff[31] - 48) * 100 + (localbuff[32] - 48) * 10 + (localbuff[33] - 48);
                signal.low_time = 			(localbuff[35] - 48) * 1000 + (localbuff[36] - 48) * 100 + (localbuff[37] - 48) * 10 + (localbuff[38] - 48);
                signal.burst_mode_on = 		(localbuff[40] - 48) * 1000 + (localbuff[41] - 48) * 100 + (localbuff[42] - 48) * 10 + (localbuff[43] - 48);
                signal.burst_mode_off = 	(localbuff[45] - 48) * 1000 + (localbuff[46] - 48) * 100 + (localbuff[47] - 48) * 10 + (localbuff[48] - 48);
                Session_Set_Signal (&session_slot_aux, (localbuff[50] - 48), &signal);

                UART_PC_Send((char *)"OK\r\n");
            }
            else
                UART_PC_Send((char *)"ERROR\r\n");

        }
        //example.	duration,00,10,00,1
        else if (!strncmp((const char *)&localbuff[0], (const char *)"duration,", (sizeof("duration,") - 1)))
        {

            if (((localbuff[18] - 48) > 0) && ((localbuff[18] - 48) < 4) && (localbuff[17] == ','))
            {
                duration.hours =  	(localbuff[9] - 48) * 10 + (localbuff[10] - 48);
                duration.minutes =	(localbuff[12] - 48) * 10 + (localbuff[13] - 48);
                duration.seconds =	(localbuff[15] - 48) * 10 + (localbuff[16] - 48);

                Session_Set_Duration (&session_slot_aux, (localbuff[18] - 48), &duration);

                UART_PC_Send((char *)"OK\r\n");
            }
            else
                UART_PC_Send((char *)"ERROR\r\n");
        }

        else if (!strncmp((const char *)&localbuff[0], (const char *)"save,", (sizeof("save,") - 1)))
        {

            if (((localbuff[5] - 48) < 2) && (localbuff[6] - 48 < 10))
            {
                //Session_Save (&session_slot_aux, buffUART3rx2[5] - 48);
                i = FLASH_Program(&session_slot_aux, ((localbuff[5] - 48) * 10 + (localbuff[6] - 48)));

                if (i == FIN_OK)
                {
                    UART_PC_Send("Guardado OK\r\n");
                    Wait_ms(1000);
                    //Reset.
                    NVIC_SystemReset();
                }

                if (i == FIN_ERROR)
                {
                    UART_PC_Send("Guardado ERROR\r\n");
                    Wait_ms(1000);
                    //Reset.
                    NVIC_SystemReset();
                }


                UART_PC_Send((char *)"OK\r\n");
            }
            else
                UART_PC_Send((char *)"ERROR\r\n");
        }

        else if (!strncmp((const char *)&localbuff[0], (const char *)"reset", (sizeof("reset") - 1)))
        {
            UART_PC_Send("Restarting...\r\n");
            Wait_ms(1000);
            NVIC_SystemReset();
        }

        else if (!strncmp((const char *)&localbuff[0], (const char *)"pause,", (sizeof("pause,") - 1)))
        {
            if (localbuff[6] - 48)
            {
                channel_1_pause = 1;
                channel_2_pause = 1;
                channel_3_pause = 1;
                channel_4_pause = 1;
            }
            else
            {
                channel_1_pause = 0;
                channel_2_pause = 0;
                channel_3_pause = 0;
                channel_4_pause = 0;
            }
/*			switch(buffUART3rx2[6] - 48)
			{
                        case 1:
                        if (buffUART3rx2[8] - 48)
                        channel_1_pause = 1;
                        else
                        channel_1_pause = 0;

                        break;

                        case 2:
                        if (buffUART3rx2[8] - 48)
                        channel_2_pause = 1;
                        else
                        channel_2_pause = 0;
                        break;

                        case 3:
                        if (buffUART3rx2[8] - 48)
                        channel_3_pause = 1;
                        else
                        channel_3_pause = 0;
                        break;

                        case 4:
                        if (buffUART3rx2[8] - 48)
                        channel_4_pause = 1;
                        else
                        channel_4_pause = 0;
                        break;

                        default:
                        UART3Send((char *)"ERROR\r\n");
                        break;
			}
*/
        }

        else if (!strncmp((const char *)&localbuff[0], (const char *)"load,", (sizeof("load,") - 1)))
        {
            //load,slot[2],destino[1]
            if (((localbuff[6] - 48) < 10) && ((localbuff[5] - 48) < 2))
            {
                Session_Load (&session_slot_aux, (((localbuff[5] - 48) * 10) + localbuff[6] - 48), 0);
                Session_Load (&session_slot_aux, (((localbuff[5] - 48) * 10) + localbuff[6] - 48), 1);
                Session_Load (&session_slot_aux, (((localbuff[5] - 48) * 10) + localbuff[6] - 48), 2);
                Session_Load (&session_slot_aux, (((localbuff[5] - 48) * 10) + localbuff[6] - 48), 3);
                Session_Load (&session_slot_aux, (((localbuff[5] - 48) * 10) + localbuff[6] - 48), 4);
                UART_PC_Send((char *)"OK\r\n");
            }
            else
                UART_PC_Send((char *)"ERROR\r\n");
/*			if (((buffUART3rx2[6] - 48) < 10) && ((buffUART3rx2[5] - 48) < 2) && ((buffUART3rx2[8] - 48) >= 0) && ((buffUART3rx2[8] - 48) < 5))
			{
                        Session_Load (&session_slot_aux, (((buffUART3rx2[5] - 48) * 10) + buffUART3rx2[6] - 48), (buffUART3rx2[8] - 48));
                        UART3Send((char *)"OK\r\n");
			}
			else
                        UART3Send((char *)"ERROR\r\n");
*/
        }
        //example. state_of_stage,0,1
        else if (!strncmp((const char *)&localbuff[0], (const char *)"state_of_stage,", (sizeof("state_of_stage,") - 1)))
        {

            if (((localbuff[15] - 48) == 0) || ((localbuff[15] - 48) == 1))
            {

                Session_Set_Status (&session_slot_aux, (localbuff[17] - 48),(localbuff[15] - 48));

                UART_PC_Send((char *)"OK\r\n");
            }
            else
                UART_PC_Send((char *)"ERROR\r\n");

        }

        else if (!strncmp((const char *)&localbuff[0], (const char *)"special_function,", (sizeof("special_function,") - 1)))
        {

            UART_PC_Send((char *)"OK\r\n");
        }

        else if (!strncmp((const char *)&localbuff[0], (const char *)"start,", (sizeof("start,") - 1)))
        {
/*			if ((buffUART3rx2[6] - 48) == 1)
                        Session_Channel_1_Start();
			if ((buffUART3rx2[6] - 48) == 2)
                        Session_Channel_2_Start();
			if ((buffUART3rx2[6] - 48) == 3)
                        Session_Channel_3_Start();
			if ((buffUART3rx2[6] - 48) == 4)
                        Session_Channel_4_Start();
*/
            Session_Channel_1_Start();
            Session_Channel_2_Start();
            Session_Channel_3_Start();
            Session_Channel_4_Start();

            ResetCheckGlobalErrors ();
            Current_Limit_Counter_Reset ();

            UART_PC_Send((char *)"OK\r\n");
        }

        else if (!strncmp((const char *)&localbuff[0], (const char *)"stop,", (sizeof("stop,") - 1)))
        {
/*			if((buffUART3rx2[5] - 48) == 1)
                        Session_Channel_1_Stop();
			if((buffUART3rx2[5] - 48) == 2)
                        Session_Channel_2_Stop();
			if((buffUART3rx2[5] - 48) == 3)
                        Session_Channel_3_Stop();
			if((buffUART3rx2[5] - 48) == 4)
                        Session_Channel_4_Stop();
*/
            Session_Channel_1_Stop();
            Session_Channel_2_Stop();
            Session_Channel_3_Stop();
            Session_Channel_4_Stop();

            UART_PC_Send((char *)"OK\r\n");
        }

#ifdef HARDWARE_VERSION_2_1
        else if (!strncmp((const char *)&localbuff[0], (const char *)"finish_ok,", (sizeof("finish_ok,") - 1)))
        {
            Session_Channel_1_Stop();
            Session_Channel_2_Stop();
            Session_Channel_3_Stop();
            Session_Channel_4_Stop();

            BuzzerCommands(BUZZER_MULTIPLE_SHORT, 3);
            UART_PC_Send((char *)"OK\r\n");
        }
#endif

        else if (!strncmp((const char *)&localbuff[0], (const char *)"read_channel,", (sizeof("read_channel,") - 1)))
        {
            if (((localbuff[13] - 48) < 5) && ((localbuff[13] - 48) > 0))
            {
                Channel_Load (&session_slot_aux, (localbuff[13] - 48));

                //--- Send slot content ---//
                SessionSend(&session_slot_aux);

                UART_PC_Send((char *)"OK\r\n");
            }
            else
                UART_PC_Send((char *)"ERROR\r\n");
        }

        else if (!strncmp((const char *)&localbuff[0], (const char *)"read_slot,", (sizeof("read_slot,") - 1)))
        {
            //load,slot[2],destino[1]
            if ((((localbuff[11] - 48) + (localbuff[10] - 48) * 10) < 11))
            {
                Session_Load (&session_slot_aux, (((localbuff[10] - 48) * 10) + localbuff[11] - 48), 0);

                //--- Send slot content ---//
                SessionSend(&session_slot_aux);

                UART_PC_Send((char *)"OK\r\n");
            }
            else
                UART_PC_Send((char *)"ERROR\r\n");
        }

        else
            UART_PC_Send((char *)"ERROR\r\n");
        //--- end ---//
        usart1_have_data = 0;
    }


}

//--- Channel 1 ---//
void UART2_Receive (void)
{

    unsigned short aux;
    antenna_typedef antenna_aux;

    // if (msUART2rxTimeOut == 0)
    // {

    // 	MSUART2RXTIMEOUT;

    // 	pBuffUART2rxW = &buffUART2rx[0];
    // 	pBuffUART2rxR = &buffUART2rx[0];
    // }

    if (usart2_have_data)
    {


        //--- Introduce your code here ---//
        //UART1Send((char *)&localbuff[0]);

        //temp,055.00\r\n
        if (!strncmp((const char *)&localbuff[0], (const char *)"temp", (sizeof("temp") - 1)))
        {

            if ((localbuff[4] == ',') && (localbuff[8] == '.') && (localbuff[11] == '\r'))
            {
                aux = (localbuff[5] - 48) * 100;
                aux += (localbuff[6] - 48) * 10;
                aux += (localbuff[7] - 48);

                temp_actual_channel_1_int = aux;

                if (temp_actual_channel_1_int != 25)
                    temp_actual_channel_1_int +=1;

                aux = (localbuff[9] - 48) * 10;
                aux += (localbuff[10] - 48);

                temp_actual_channel_1_dec = aux;

                strcpy((char *)&localbuff[11], (const char *)",1\r\n");
                UART_PC_Send((char *)&localbuff[0]);
                UART_CH1_Send("OK\r\n");
            }
        }

        //ant0,012.27,087.90,001.80,065.00\r\n.
        else if (!strncmp((const char *)&localbuff[0], (const char *)"ant", (sizeof("ant") - 1)))
        {

            strcpy((char *)&localbuff[32], (const char *)",1\r\n");
            UART_PC_Send((char *)&localbuff[0]);

            if ((localbuff[4] == ',') && (localbuff[11] == ',') && (localbuff[18] == ',') && (localbuff[25] == ','))
            {
                aux = (localbuff[5] - 48) * 100;
                aux += (localbuff[6] - 48) * 10;
                aux += (localbuff[7] - 48);

                antenna_aux.resistance_int = aux;

                aux = (localbuff[9] - 48) * 10;
                aux += (localbuff[10] - 48);

                antenna_aux.resistance_dec = aux;

                aux = (localbuff[12] - 48) * 100;
                aux += (localbuff[13] - 48) * 10;
                aux += (localbuff[14] - 48);

                antenna_aux.inductance_int = aux;

                aux = (localbuff[16] - 48) * 10;
                aux += (localbuff[17] - 48);

                antenna_aux.inductance_dec = aux;

                aux = (localbuff[19] - 48) * 100;
                aux += (localbuff[20] - 48) * 10;
                aux += (localbuff[21] - 48);

                antenna_aux.current_limit_int = aux;

                aux = (localbuff[23] - 48) * 10;
                aux += (localbuff[24] - 48);

                antenna_aux.current_limit_dec = aux;

                aux = (localbuff[26] - 48) * 100;
                aux += (localbuff[27] - 48) * 10;
                aux += (localbuff[28] - 48);

                antenna_aux.temp_max_int = aux;

                aux = (localbuff[30] - 48) * 10;
                aux += (localbuff[31] - 48);

                antenna_aux.temp_max_dec = aux;

                Session_Set_Antenna (&session_ch_1, aux , &antenna_aux);
                Save_Antenna_Name(CH1, (char *) localbuff);



                UART_CH1_Send((char *) "OK\r\n");
            }
            else
                UART_CH1_Send((char *) "ERROR\r\n");

        }
        //--- end ---//

        usart2_have_data = 0;
    }
}

void UART3_Receive (void)
{
    unsigned short aux;
    antenna_typedef antenna_aux;

    // if (msUART3rxTimeOut == 0)
    // {

    // 	MSUART3RXTIMEOUT;

    // 	pBuffUART3rxW = &buffUART3rx[0];
    // 	pBuffUART3rxR = &buffUART3rx[0];
    // }

    if (usart3_have_data)
    {
        //--- Introduce your code here ---//
        //UART2Send((char *)&localbuff[0]);

        //temp,055.00\r\n
        if (!strncmp((const char *)&localbuff[0], (const char *)"temp", (sizeof("temp") - 1)))
        {
            if ((localbuff[4] == ',') && (localbuff[8] == '.') && (localbuff[11] == '\r'))
            {
                aux = (localbuff[5] - 48) * 100;
                aux += (localbuff[6] - 48) * 10;
                aux += (localbuff[7] - 48);

                temp_actual_channel_2_int = aux;

                aux = (localbuff[9] - 48) * 10;
                aux += (localbuff[10] - 48);

                temp_actual_channel_2_dec = aux;

                strcpy((char *)&localbuff[11], (const char *)",2\r\n");
                UART_PC_Send((char *)&localbuff[0]);
                UART_CH2_Send("OK\r\n");
            }
        }

        //ant0,012.27,087.90,001.80,065.00\r\n.
        else if (!strncmp((const char *)&localbuff[0], (const char *)"ant", (sizeof("ant") - 1)))
        {
            strcpy((char *)&localbuff[32], (const char *)",2\r\n");
            UART_PC_Send((char *)&localbuff[0]);

            if ((localbuff[4] == ',') && (localbuff[11] == ',') && (localbuff[18] == ',') && (localbuff[25] == ','))
            {
                aux = (localbuff[5] - 48) * 100;
                aux += (localbuff[6] - 48) * 10;
                aux += (localbuff[7] - 48);

                antenna_aux.resistance_int = aux;

                aux = (localbuff[9] - 48) * 10;
                aux += (localbuff[10] - 48);

                antenna_aux.resistance_dec = aux;

                aux = (localbuff[12] - 48) * 100;
                aux += (localbuff[13] - 48) * 10;
                aux += (localbuff[14] - 48);

                antenna_aux.inductance_int = aux;

                aux = (localbuff[16] - 48) * 10;
                aux += (localbuff[17] - 48);

                antenna_aux.inductance_dec = aux;

                aux = (localbuff[19] - 48) * 100;
                aux += (localbuff[20] - 48) * 10;
                aux += (localbuff[21] - 48);

                antenna_aux.current_limit_int = aux;

                aux = (localbuff[23] - 48) * 10;
                aux += (localbuff[24] - 48);

                antenna_aux.current_limit_dec = aux;

                aux = (localbuff[26] - 48) * 100;
                aux += (localbuff[27] - 48) * 10;
                aux += (localbuff[28] - 48);

                antenna_aux.temp_max_int = aux;

                aux = (localbuff[30] - 48) * 10;
                aux += (localbuff[31] - 48);

                antenna_aux.temp_max_dec = aux;


                Session_Set_Antenna (&session_ch_2, aux , &antenna_aux);
                Save_Antenna_Name(CH2, (char *) localbuff);

                UART_CH2_Send((char *) "OK\r\n");
            }
            else
                UART_CH2_Send((char *) "ERROR\r\n");
        }
        //--- end ---//

        usart3_have_data = 0;
    }
}

void UART4_Receive (void)
{
    unsigned short aux;
    antenna_typedef antenna_aux;

    // if (msUART4rxTimeOut == 0)
    // {

    // 	MSUART4RXTIMEOUT;

    // 	pBuffUART4rxW = &buffUART4rx[0];
    // 	pBuffUART4rxR = &buffUART4rx[0];
    // }

    if (usart4_have_data)
    {
        //--- Introduce your code here ---//
        //UART4Send((char *)&buffUART4rx2[0]);

        //temp,055.00\r\n
        if (!strncmp((const char *)&localbuff[0], (const char *)"temp", (sizeof("temp") - 1)))
        {
            if ((localbuff[4] == ',') && (localbuff[8] == '.') && (localbuff[11] == '\r'))
            {
                aux = (localbuff[5] - 48) * 100;
                aux += (localbuff[6] - 48) * 10;
                aux += (localbuff[7] - 48);

                temp_actual_channel_3_int = aux;

                aux = (localbuff[9] - 48) * 10;
                aux += (localbuff[10] - 48);

                temp_actual_channel_3_dec = aux;

                strcpy((char *)&localbuff[11], (const char *)",3\r\n");
                UART_PC_Send((char *)&localbuff[0]);
                UART_CH3_Send("OK\r\n");
            }
        }

        //ant0,012.27,087.90,001.80,065.00\r\n.
        else if (!strncmp((const char *)&localbuff[0], (const char *)"ant", (sizeof("ant") - 1)))
        {

            strcpy((char *)&localbuff[32], (const char *)",3\r\n");
            UART_PC_Send((char *)&localbuff[0]);

            if ((localbuff[4] == ',') && (localbuff[11] == ',') && (localbuff[18] == ',') && (localbuff[25] == ','))
            {
                aux = (localbuff[5] - 48) * 100;
                aux += (localbuff[6] - 48) * 10;
                aux += (localbuff[7] - 48);

                antenna_aux.resistance_int = aux;

                aux = (localbuff[9] - 48) * 10;
                aux += (localbuff[10] - 48);

                antenna_aux.resistance_dec = aux;

                aux = (localbuff[12] - 48) * 100;
                aux += (localbuff[13] - 48) * 10;
                aux += (localbuff[14] - 48);

                antenna_aux.inductance_int = aux;

                aux = (localbuff[16] - 48) * 10;
                aux += (localbuff[17] - 48);

                antenna_aux.inductance_dec = aux;

                aux = (localbuff[19] - 48) * 100;
                aux += (localbuff[20] - 48) * 10;
                aux += (localbuff[21] - 48);

                antenna_aux.current_limit_int = aux;

                aux = (localbuff[23] - 48) * 10;
                aux += (localbuff[24] - 48);

                antenna_aux.current_limit_dec = aux;

                aux = (localbuff[26] - 48) * 100;
                aux += (localbuff[27] - 48) * 10;
                aux += (localbuff[28] - 48);

                antenna_aux.temp_max_int = aux;

                aux = (localbuff[30] - 48) * 10;
                aux += (localbuff[31] - 48);

                antenna_aux.temp_max_dec = aux;

                Session_Set_Antenna (&session_ch_3, aux , &antenna_aux);
                Save_Antenna_Name(CH3, (char *) localbuff);

                UART_CH3_Send((char *) "OK\r\n");
            }
            else
                UART_CH3_Send((char *) "ERROR\r\n");
        }
        //--- end ---//
        usart4_have_data = 0;
    }
}

void UART5_Receive (void)
{
    unsigned short aux;
    antenna_typedef antenna_aux;

    // if (msUART5rxTimeOut == 0)
    // {

    // 	MSUART5RXTIMEOUT;

    // 	pBuffUART5rxW = &buffUART5rx[0];
    // 	pBuffUART5rxR = &buffUART5rx[0];
    // }

    if (usart5_have_data)
    {

        //--- Introduce your code here ---//
        //UART5Send((char *)&buffUART5rx2[0]);

        //temp,055.00\r\n
        if (!strncmp((const char *)&localbuff[0], (const char *)"temp", (sizeof("temp") - 1)))
        {
            if ((localbuff[4] == ',') && (localbuff[8] == '.') && (localbuff[11] == '\r'))
            {
                aux = (localbuff[5] - 48) * 100;
                aux += (localbuff[6] - 48) * 10;
                aux += (localbuff[7] - 48);

                temp_actual_channel_4_int = aux;

                aux = (localbuff[9] - 48) * 10;
                aux += (localbuff[10] - 48);

                temp_actual_channel_4_dec = aux;

                strcpy((char *)&localbuff[11], (const char *)",4\r\n");
                UART_PC_Send((char *)&localbuff[0]);
                UART_CH4_Send("OK\r\n");
            }
        }

        //ant0,012.27,087.90,001.80,065.00\r\n.
        else if (!strncmp((const char *)&localbuff[0], (const char *)"ant", (sizeof("ant") - 1)))
        {
            strcpy((char *)&localbuff[32], (const char *)",4\r\n");
            UART_PC_Send((char *) &localbuff[0]);

            if ((localbuff[4] == ',') && (localbuff[11] == ',') && (localbuff[18] == ',') && (localbuff[25] == ','))
            {
                aux = (localbuff[5] - 48) * 100;
                aux += (localbuff[6] - 48) * 10;
                aux += (localbuff[7] - 48);

                antenna_aux.resistance_int = aux;

                aux = (localbuff[9] - 48) * 10;
                aux += (localbuff[10] - 48);

                antenna_aux.resistance_dec = aux;

                aux = (localbuff[12] - 48) * 100;
                aux += (localbuff[13] - 48) * 10;
                aux += (localbuff[14] - 48);

                antenna_aux.inductance_int = aux;

                aux = (localbuff[16] - 48) * 10;
                aux += (localbuff[17] - 48);

                antenna_aux.inductance_dec = aux;

                aux = (localbuff[19] - 48) * 100;
                aux += (localbuff[20] - 48) * 10;
                aux += (localbuff[21] - 48);

                antenna_aux.current_limit_int = aux;

                aux = (localbuff[23] - 48) * 10;
                aux += (localbuff[24] - 48);

                antenna_aux.current_limit_dec = aux;

                aux = (localbuff[26] - 48) * 100;
                aux += (localbuff[27] - 48) * 10;
                aux += (localbuff[28] - 48);

                antenna_aux.temp_max_int = aux;

                aux = (localbuff[30] - 48) * 10;
                aux += (localbuff[31] - 48);

                antenna_aux.temp_max_dec = aux;

                Session_Set_Antenna (&session_ch_4, aux , &antenna_aux);
                Save_Antenna_Name(CH4, (char *) localbuff);

                UART_CH4_Send((char *) "OK\r\n");
            }
            else
                UART_CH4_Send((char *) "ERROR\r\n");
        }
        //--- end ---//

        usart5_have_data = 0;
    }
}



//---- end of file ----//
