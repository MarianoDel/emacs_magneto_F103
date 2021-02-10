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

// Includes --------------------------------------------------------------------
#include "comms.h"
#include "channels_defs.h"
#include "usart.h"
// #include "GTK_Estructura.h"
#include "GTK_Signal.h"
// #include "flash_program.h"
#include "GTK_Hard.h"
// #include "timer.h"
#include "antennas.h"
#include "utils.h"

#include <string.h>
#include <stdio.h>



// Externals -------------------------------------------------------------------
//main.c para usart.c
extern volatile unsigned char usart1_have_data;
// extern volatile unsigned char usart2_have_data;
// extern volatile unsigned char usart3_have_data;
// extern volatile unsigned char usart4_have_data;
// extern volatile unsigned char usart5_have_data;

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

// desde main.c buffer local para todas las comms
extern char s_ok [];
extern char s_nok [];


// Globals ---------------------------------------------------------------------
unsigned char localbuff [SIZEOF_RXDATA] = { '\0' };


const char s_buzzer_short [] = {"buzzer short"};
const char s_buzzer_half [] = {"buzzer half"};
const char s_buzzer_long [] = {"buzzer long"};


// Module Private Functions ----------------------------------------------------
// unsigned char ParseCommsWithChannels (char * str, unsigned char channel);
// static void ParseCurrentTemp (char * str, unsigned char * t_int, unsigned char * t_dec);
// static void ParseAntennaParams (char * str, antenna_typedef * antenna);



/* Module Exported Functions -----------------------------------------------------------*/
//MODIFICADO MAGNETO V2.0 28-02-15 ANTES ERA USART3
//--- Recepcion de la PC
void UART1_Receive (void)
{
    antenna_typedef 	antenna;
    signal_typedef 		signal;
    duration_typedef 	duration;

    if (usart1_have_data)
    {
        ReadUsart1Buffer(localbuff, sizeof(localbuff));
        //--- Introduce your code here ---//
        // UART_PC_Send((char *)&localbuff[0]);

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

        else if (!strncmp((const char *)localbuff,
                          (const char *)"serial num",
                          (sizeof("serial num") - 1)))
        {
#ifdef USE_DEVICE_ID_4BYTES
            unsigned int device_id = *((unsigned short*)0x1FFFF7E8);
            device_id <<= 16;
            device_id |= *((unsigned short*)(0x1FFFF7E8 + 2));
            device_id ^= *((unsigned int*)(0x1FFFF7E8 + 4));
            device_id ^= *((unsigned int*)(0x1FFFF7E8 + 8));
            sprintf((char *)localbuff, "0x%08x\r\n", device_id);
            
            UART_PC_Send((char *)localbuff);            
#endif
            // sprintf((char *)localbuff, "mem: %dk\r\n", *((unsigned short*)0x1FFFF7E0));
            // sprintf((char *)localbuff, "0x%x\r\n", *((unsigned short*)0x1FFFF7E8));            
#ifdef USE_DEVICE_ID_12BYTES
            sprintf((char *)localbuff, "0x%04x%04x%08x%08x\r\n",
                    *((unsigned short*)0x1FFFF7E8),
                    *((unsigned short*)(0x1FFFF7E8 + 2)),
                    *((unsigned int*)(0x1FFFF7E8 + 4)),
                    *((unsigned int*)(0x1FFFF7E8 + 8)));

            UART_PC_Send((char *)localbuff);
#endif
        }
        else if (!strncmp((const char *)&localbuff[0],
                          (const char *)"get_antenna,",
                          (sizeof("get_antenna,") - 1)))
        {
            UART_PC_Send((char *)"OK\r\n");
            //pido envio de info de antenas conocidas hasta ahora
            AntennaSendKnowInfoWithTimer ();            
        }

        else if (!strncmp((const char *)&localbuff[0], (const char *)"get_antenna,1",
                          (sizeof("get_antenna,1") - 1)))
        {
            //--- Get antenna parameters ---//
            //TODO: despues reemplazar por nombre de antenna            
            AntennaGetParamsStruct (CH1, &antenna);
            sprintf((char *)&localbuff[0], "ch1,%03d.%02d,%03d.%02d,%03d.%02d,%03d.%02d\r\n",
                    antenna.resistance_int,
                    antenna.resistance_dec,
                    antenna.inductance_int,
                    antenna.inductance_dec,
                    antenna.current_limit_int,
                    antenna.current_limit_dec,
                    antenna.temp_max_int,
                    antenna.temp_max_dec);

            UART_PC_Send((char *) localbuff);
        }

        else if (!strncmp((const char *)&localbuff[0], (const char *)"get_antenna,2",
                          (sizeof("get_antenna,2") - 1)))
        {
            //--- Get antenna parameters ---//
            //TODO: despues reemplazar por nombre de antenna            
            AntennaGetParamsStruct (CH2, &antenna);
            sprintf((char *)&localbuff[0], "ch2,%03d.%02d,%03d.%02d,%03d.%02d,%03d.%02d\r\n",
                    antenna.resistance_int,
                    antenna.resistance_dec,
                    antenna.inductance_int,
                    antenna.inductance_dec,
                    antenna.current_limit_int,
                    antenna.current_limit_dec,
                    antenna.temp_max_int,
                    antenna.temp_max_dec);

            UART_PC_Send((char *) localbuff);
        }

        else if (!strncmp((const char *)&localbuff[0], (const char *)"get_antenna,3",
                          (sizeof("get_antenna,3") - 1)))
        {
            //--- Get antenna parameters ---//
            //TODO: despues reemplazar por nombre de antenna            
            AntennaGetParamsStruct (CH3, &antenna);
            sprintf((char *)&localbuff[0], "ch3,%03d.%02d,%03d.%02d,%03d.%02d,%03d.%02d\r\n",
                    antenna.resistance_int,
                    antenna.resistance_dec,
                    antenna.inductance_int,
                    antenna.inductance_dec,
                    antenna.current_limit_int,
                    antenna.current_limit_dec,
                    antenna.temp_max_int,
                    antenna.temp_max_dec);

            UART_PC_Send((char *) localbuff);
        }

        else if (!strncmp((const char *)&localbuff[0], (const char *)"get_antenna,4",
                          (sizeof("get_antenna,4") - 1)))
        {
            //--- Get antenna parameters ---//
            AntennaGetParamsStruct (CH4, &antenna);
            //TODO: despues reemplazar por nombre de antenna
            sprintf((char *)&localbuff[0], "ch4,%03d.%02d,%03d.%02d,%03d.%02d,%03d.%02d\r\n",
                    antenna.resistance_int,
                    antenna.resistance_dec,
                    antenna.inductance_int,
                    antenna.inductance_dec,
                    antenna.current_limit_int,
                    antenna.current_limit_dec,
                    antenna.temp_max_int,
                    antenna.temp_max_dec);

            UART_PC_Send((char *) localbuff);
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
                // Session_Set_Antenna (&session_slot_aux, (localbuff[28] - 48), &antenna);
                //TODO: por ahora guardo en las 4 estructuras lo mismo
                AntennaSetParamsStruct (CH1, &antenna);
                AntennaSetParamsStruct (CH2, &antenna);
                AntennaSetParamsStruct (CH3, &antenna);
                AntennaSetParamsStruct (CH4, &antenna);                

                UART_PC_Send((char *)"OK\r\n");
            }
            else
                UART_PC_Send((char *)"ERROR\r\n");
        }
        //example. signal,100,100,0000,0003,0003,0003,0006,0000,0000,1
        else if (!strncmp((const char *)&localbuff[0], (const char *)"signal,", (sizeof("signal,") - 1)))
        {
            //"signal,%03d,%03d,0%x%x%d,%04d,%04d,%04d,%04d,%04d,%04d,1\r\n"
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

#ifdef USE_CURRENT_SLOT_WITHOUT_LOAD_CMD
                Session_Set_Signal (&session_ch_1, (localbuff[50] - 48), &signal);
                Session_Set_Signal (&session_ch_2, (localbuff[50] - 48), &signal);
                Session_Set_Signal (&session_ch_3, (localbuff[50] - 48), &signal);
                Session_Set_Signal (&session_ch_4, (localbuff[50] - 48), &signal);
#endif
                
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
#ifdef USE_CURRENT_SLOT_WITHOUT_LOAD_CMD
                Session_Set_Duration (&session_ch_1, (localbuff[18] - 48), &duration);
                Session_Set_Duration (&session_ch_2, (localbuff[18] - 48), &duration);
                Session_Set_Duration (&session_ch_3, (localbuff[18] - 48), &duration);
                Session_Set_Duration (&session_ch_4, (localbuff[18] - 48), &duration);
#endif
                UART_PC_Send((char *)"OK\r\n");
            }
            else
                UART_PC_Send((char *)"ERROR\r\n");
        }

        else if (!strncmp((const char *)&localbuff[0], (const char *)"save,", (sizeof("save,") - 1)))
        {
            // unsigned char i;
            // if (((localbuff[5] - 48) < 2) && (localbuff[6] - 48 < 10))
            // {
            //     i = FLASH_Program(&session_slot_aux, ((localbuff[5] - 48) * 10 + (localbuff[6] - 48)));

            //     if (i == FIN_OK)
            //     {
            //         UART_PC_Send("Guardado OK\r\n");
            //         Wait_ms(1000);
            //         //Reset.
            //         NVIC_SystemReset();
            //     }

            //     if (i == FIN_ERROR)
            //     {
            //         UART_PC_Send("Guardado ERROR\r\n");
            //         Wait_ms(1000);
            //         //Reset.
            //         NVIC_SystemReset();
            //     }


            //     UART_PC_Send((char *)"OK\r\n");
            // }
            // else
            //     UART_PC_Send((char *)"ERROR\r\n");

            UART_PC_Send((char *)"OK\r\n");            
        }

        else if (!strncmp((const char *)&localbuff[0], (const char *)"reset", (sizeof("reset") - 1)))
        {
            UART_PC_Send("Restarting...\r\n");
            // Wait_ms(1000);
            // NVIC_SystemReset();
        }

        else if (!strncmp((const char *)&localbuff[0], (const char *)"pause,", (sizeof("pause,") - 1)))
        {
            if (localbuff[6] == '1')
            {
                channel_1_pause = 1;
                channel_2_pause = 1;
                channel_3_pause = 1;
                channel_4_pause = 1;
                ChangeLed(LED_TREATMENT_PAUSED);
            }
            else
            {
                channel_1_pause = 0;
                channel_2_pause = 0;
                channel_3_pause = 0;
                channel_4_pause = 0;
                ChangeLed(LED_TREATMENT_GENERATING);
            }
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
#ifdef USE_CURRENT_SLOT_WITHOUT_LOAD_CMD
                Session_Set_Status (&session_ch_1, (localbuff[17] - 48), (localbuff[15] - 48));
                Session_Set_Status (&session_ch_2, (localbuff[17] - 48), (localbuff[15] - 48));
                Session_Set_Status (&session_ch_3, (localbuff[17] - 48), (localbuff[15] - 48));
                Session_Set_Status (&session_ch_4, (localbuff[17] - 48), (localbuff[15] - 48));
#endif
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
            // someone was paused?
            channel_1_pause = 0;
            channel_2_pause = 0;
            channel_3_pause = 0;
            channel_4_pause = 0;

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
            StopAllChannels();

            UART_PC_Send((char *)"OK\r\n");
        }

        else if (!strncmp((const char *)&localbuff[0],
                          (const char *)"keepalive,",
                          (sizeof("keepalive,") - 1)))
        {
            UART_PC_Send((char *)"OK\r\n");
        }

    //-- Buzzer Actions
        else if (strncmp((const char *)localbuff, s_buzzer_short, sizeof(s_buzzer_short) - 1) == 0)
        {
            char * msg = (char *) localbuff;
            unsigned char decimales = 0;
            unsigned short bips_qtty = 0;
        
            msg += sizeof(s_buzzer_short);		//normalizo al payload, hay un espacio

            //lo que viene es un byte de 1 a 9
            decimales = StringIsANumber(msg, &bips_qtty);
            if (decimales == 1)
            {
                BuzzerCommands(BUZZER_MULTIPLE_SHORT, (unsigned char) bips_qtty);
                UART_PC_Send(s_ok);
            }
            else
                UART_PC_Send(s_nok);
        }

        else if (strncmp((const char *)localbuff, s_buzzer_half, sizeof(s_buzzer_half) - 1) == 0)
        {
            char * msg = (char *) localbuff;
            unsigned char decimales = 0;            
            unsigned short bips_qtty = 0;
        
            msg += sizeof(s_buzzer_half);		//normalizo al payload, hay un espacio

            //lo que viene es un byte de 1 a 9
            decimales = StringIsANumber(msg, &bips_qtty);
            if (decimales == 1)
            {
                BuzzerCommands(BUZZER_MULTIPLE_HALF, (unsigned char) bips_qtty);
                UART_PC_Send(s_ok);
            }
            else
                UART_PC_Send(s_nok);

        }

        else if (strncmp((const char *)localbuff, s_buzzer_long, sizeof(s_buzzer_long) - 1) == 0)
        {
            char * msg = (char *) localbuff;
            unsigned char decimales = 0;            
            unsigned short bips_qtty = 0;
        
            msg += sizeof(s_buzzer_long);		//normalizo al payload, hay un espacio

            //lo que viene es un byte de 1 a 9
            decimales = StringIsANumber(msg, &bips_qtty);
            if (decimales == 1)
            {
                BuzzerCommands(BUZZER_MULTIPLE_LONG, (unsigned char) bips_qtty);
                UART_PC_Send(s_ok);
            }
            else
                UART_PC_Send(s_nok);
        }
        
        
#ifdef HARDWARE_VERSION_2_1
        else if (!strncmp((const char *)&localbuff[0], (const char *)"finish_ok,", (sizeof("finish_ok,") - 1)))
        {
            StopAllChannels();

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



//---- end of file ----//
