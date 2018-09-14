//---------------------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F103
// ##
// #### COMMS.C ############################################
//---------------------------------------------------------

/* Includes ------------------------------------------------------------------*/
// #include "stm32f10x.h"
// #include "hard.h"

#include "comms.h"
#include "uart.h"
#include "GTK_Estructura.h"
#include "GTK_Signal.h"
#include "flash_program.h"
#include "GTK_Hard.h"
#include "timer.h"

#include <string.h>
#include <stdio.h>



/* Externals ------------------------------------------------------------------*/
//desde uart.c
extern volatile char PacketReadyUART1;
extern volatile char PacketReadyUART2;
extern volatile char PacketReadyUART3;
extern volatile char PacketReadyUART4;
extern volatile char PacketReadyUART5;
//TODO: quitar esta mierda despues!!!!
extern unsigned char msUART1rxTimeOut;
extern unsigned char msUART2rxTimeOut;
extern unsigned char msUART3rxTimeOut;
extern unsigned char msUART4rxTimeOut;
extern unsigned char msUART5rxTimeOut;

extern volatile char *pBuffUART1rxW;
extern volatile char *pBuffUART2rxW;
extern volatile char *pBuffUART3rxW;
extern volatile char *pBuffUART4rxW;
extern volatile char *pBuffUART5rxW;

extern volatile char *pBuffUART1rxR;
extern volatile char *pBuffUART2rxR;
extern volatile char *pBuffUART3rxR;
extern volatile char *pBuffUART4rxR;
extern volatile char *pBuffUART5rxR;

extern volatile char buffUART1rx [];
extern volatile char buffUART2rx [];
extern volatile char buffUART3rx [];
extern volatile char buffUART4rx [];
extern volatile char buffUART5rx [];

extern volatile char buffUART1rx2[];
extern volatile char buffUART2rx2[];
extern volatile char buffUART3rx2[];
extern volatile char buffUART4rx2[];
extern volatile char buffUART5rx2[];

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



/* Module Private Functions -----------------------------------------------------------*/
//TODO: por favor!!!!!!!!!!!
#define MSUART1RXTIMEOUT msUART1rxTimeOut = 3 //300ms
#define MSUART2RXTIMEOUT msUART2rxTimeOut = 3 //300ms
#define MSUART3RXTIMEOUT msUART3rxTimeOut = 3 //300ms
#define MSUART4RXTIMEOUT msUART4rxTimeOut = 3 //300ms
#define MSUART5RXTIMEOUT msUART5rxTimeOut = 3 //300ms



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

	if (msUART1rxTimeOut == 0)
	{

		MSUART1RXTIMEOUT;

		pBuffUART1rxW = &buffUART1rx[0];
		pBuffUART1rxR = &buffUART1rx[0];
	}

	if (PacketReadyUART1)
	{
		//--- Introduce your code here ---//
		UART1Send((char *)&buffUART1rx2[0]);

		if (!strncmp((const char *)&buffUART1rx2[0], (const char *)"get_temp,", (sizeof("get_temp,") - 1)))
		{
			switch ((buffUART1rx2[9] - 48))
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

		else if (!strncmp((const char *)&buffUART1rx2[0], (const char *)"get_antenna_con,", (sizeof("get_antenna_con,") - 1)))
		{
			//0 si no tengo, 1 si esta 1, 2 si est 2 "ant_con,0,2,0,0\r\n"
			/*
			sprintf((const char *)&buffUART1rx2[0], "ant_con,0,2,0,0\r\n",
						antenna.resistance_int,
						antenna.resistance_dec,
						antenna.inductance_int,
						antenna.inductance_dec,
						antenna.current_limit_int,
						antenna.current_limit_dec,
						antenna.temp_max_int,
						antenna.temp_max_dec);
UART_PC_Send(buffUART1rx2);
buffUART1rx2[0] = '\0';
			*/
			UART_PC_Send((char *) "ant_con,0,2,0,0\r\n");
			buffUART1rx2[0] = '\0';
		}

		else if (!strncmp((const char *)&buffUART1rx2[0], (const char *)"get_antenna,", (sizeof("get_antenna,") - 1)))
		{
			//--- Get antenna parameters ---//
			Session_Get_Antenna (&session_ch_1, 1, &antenna);
			//strcpy((const char *)&buffUART1rx2[0], (const char *)"ant0,");
			//sprintf((const char *)&buffUART1rx2[0], "ant0,%03d.%02d,%03d.%02d,%03d.%02d,%03d.%02d,1\r\n",
			sprintf((char *)&buffUART1rx2[0], "ant0,%03d.%02d,%03d.%02d,%03d.%02d,%03d.%02d,1\r\n",
						antenna.resistance_int,
						antenna.resistance_dec,
						antenna.inductance_int,
						antenna.inductance_dec,
						antenna.current_limit_int,
						antenna.current_limit_dec,
						antenna.temp_max_int,
						antenna.temp_max_dec);

			//UART_PC_Send((const char *) buffUART1rx2);
			UART_PC_Send((char *) buffUART1rx2);
			buffUART1rx2[0] = '\0';

			Session_Get_Antenna (&session_ch_2, 1, &antenna);
			//strcpy((const char *)&buffUART1rx2[0], (const char *)"ant0,");
			sprintf((char *)&buffUART1rx2[0], "ant0,%03d.%02d,%03d.%02d,%03d.%02d,%03d.%02d,2\r\n",
						antenna.resistance_int,
						antenna.resistance_dec,
						antenna.inductance_int,
						antenna.inductance_dec,
						antenna.current_limit_int,
						antenna.current_limit_dec,
						antenna.temp_max_int,
						antenna.temp_max_dec);

			//UART_PC_Send((const char *) buffUART1rx2);
			UART_PC_Send((char *) buffUART1rx2);
			buffUART1rx2[0] = '\0';

			Session_Get_Antenna (&session_ch_3, 1, &antenna);
			//strcpy((const char *)&buffUART1rx2[0], (const char *)"ant0,");
			sprintf((char *)&buffUART1rx2[0], "ant0,%03d.%02d,%03d.%02d,%03d.%02d,%03d.%02d,3\r\n",
						antenna.resistance_int,
						antenna.resistance_dec,
						antenna.inductance_int,
						antenna.inductance_dec,
						antenna.current_limit_int,
						antenna.current_limit_dec,
						antenna.temp_max_int,
						antenna.temp_max_dec);

			UART_PC_Send((char *) buffUART1rx2);
			buffUART1rx2[0] = '\0';

			Session_Get_Antenna (&session_ch_4, 1, &antenna);
			//strcpy((const char *)&buffUART1rx2[0], (const char *)"ant0,");
			sprintf((char *)&buffUART1rx2[0], "ant0,%03d.%02d,%03d.%02d,%03d.%02d,%03d.%02d,4\r\n",
						antenna.resistance_int,
						antenna.resistance_dec,
						antenna.inductance_int,
						antenna.inductance_dec,
						antenna.current_limit_int,
						antenna.current_limit_dec,
						antenna.temp_max_int,
						antenna.temp_max_dec);

			UART_PC_Send((char *) buffUART1rx2);
			buffUART1rx2[0] = '\0';

			if (Get_Antenna_Name(CH1, (char *) &buffUART1rx2[0]) != 0)
			{
				UART_PC_Send((char *) (const char *) "ant_name,1,");
				UART_PC_Send((char *) buffUART1rx2);
				UART_PC_Send((char *) (const char *) "\r\n");
			}

			if (Get_Antenna_Name(CH2, (char *) &buffUART1rx2[0]) != 0)
			{
				UART_PC_Send((char *) (const char *) "ant_name,2,");
				UART_PC_Send((char *) buffUART1rx2);
				UART_PC_Send((char *) (const char *) "\r\n");
			}

			if (Get_Antenna_Name(CH3, (char *) &buffUART1rx2[0]) != 0)
			{
				UART_PC_Send((char *) (const char *) "ant_name,3,");
				UART_PC_Send((char *) buffUART1rx2);
				UART_PC_Send((char *) (const char *) "\r\n");
			}

			if (Get_Antenna_Name(CH4, (char *) &buffUART1rx2[0]) != 0)
			{
				UART_PC_Send((char *) (const char *) "ant_name,4,");
				UART_PC_Send((char *) buffUART1rx2);
				UART_PC_Send((char *) (const char *) "\r\n");
			}

			buffUART1rx2[0] = '\0';
		}

		else if (!strncmp((const char *)&buffUART1rx2[0], (const char *)"antenna,", (sizeof("antenna,") - 1)))
		{

			if (((buffUART1rx2[28] - 48) > 0) && ((buffUART1rx2[28] - 48) < 4) && (buffUART1rx2[27] == ','))
			{
				antenna.resistance_int = 	(buffUART1rx2[8] - 48) * 100 + (buffUART1rx2[9] - 48) * 10 + (buffUART1rx2[10] - 48);
				antenna.resistance_dec = 	(buffUART1rx2[12] - 48) * 10 + (buffUART1rx2[13] - 48);
				antenna.inductance_int = 	(buffUART1rx2[15] - 48) * 100 + (buffUART1rx2[16] - 48) * 10 + (buffUART1rx2[17] - 48);
				antenna.inductance_dec = 	(buffUART1rx2[19] - 48) * 10 + (buffUART1rx2[20] - 48);
				antenna.current_limit_int = (buffUART1rx2[22] - 48) * 10 + (buffUART1rx2[23] - 48);
				antenna.current_limit_dec = (buffUART1rx2[25] - 48) * 10 + (buffUART1rx2[26] - 48);

				//--- Save antenna parameters ---//
				Session_Set_Antenna (&session_slot_aux, (buffUART1rx2[28] - 48), &antenna);

				UART_PC_Send((char *)"OK\r\n");
			}
			else
				UART_PC_Send((char *)"ERROR\r\n");
		}
		//example. signal,100,100,0000,0003,0003,0003,0006,0000,0000,1
		else if (!strncmp((const char *)&buffUART1rx2[0], (const char *)"signal,", (sizeof("signal,") - 1)))
		{
			//signal,%03d,%03d,0%x%x%d,%04d,%04d,%04d,%04d,%04d,%04d,1\r\n",
			if (((buffUART1rx2[50] - 48) > 0) && ((buffUART1rx2[50] - 48) < 4) && (buffUART1rx2[49] == ','))
			{
				signal.initial_power = 		(buffUART1rx2[7] - 48) * 100 + (buffUART1rx2[8] - 48) * 10 + (buffUART1rx2[9] - 48);
				signal.final_power = 		(buffUART1rx2[11] - 48) * 100 + (buffUART1rx2[12] - 48) * 10 + (buffUART1rx2[13] - 48);
				signal.sync_on =  			(buffUART1rx2[18] - 48);
				//signal.step_number = 		(buffUART1rx2[15] - 48) * 1000 + (buffUART1rx2[16] - 48) * 100 + (buffUART1rx2[17] - 48) * 10 + (buffUART1rx2[18] - 48);
				signal.rising_time = 		(buffUART1rx2[20] - 48) * 1000 + (buffUART1rx2[21] - 48) * 100 + (buffUART1rx2[22] - 48) * 10 + (buffUART1rx2[23] - 48);
				signal.maintenance_time = 	(buffUART1rx2[25] - 48) * 1000 + (buffUART1rx2[26] - 48) * 100 + (buffUART1rx2[27] - 48) * 10 + (buffUART1rx2[28] - 48);
				signal.falling_time = 		(buffUART1rx2[30] - 48) * 1000 + (buffUART1rx2[31] - 48) * 100 + (buffUART1rx2[32] - 48) * 10 + (buffUART1rx2[33] - 48);
				signal.low_time = 			(buffUART1rx2[35] - 48) * 1000 + (buffUART1rx2[36] - 48) * 100 + (buffUART1rx2[37] - 48) * 10 + (buffUART1rx2[38] - 48);
				signal.burst_mode_on = 		(buffUART1rx2[40] - 48) * 1000 + (buffUART1rx2[41] - 48) * 100 + (buffUART1rx2[42] - 48) * 10 + (buffUART1rx2[43] - 48);
				signal.burst_mode_off = 	(buffUART1rx2[45] - 48) * 1000 + (buffUART1rx2[46] - 48) * 100 + (buffUART1rx2[47] - 48) * 10 + (buffUART1rx2[48] - 48);
				Session_Set_Signal (&session_slot_aux, (buffUART1rx2[50] - 48), &signal);

				UART_PC_Send((char *)"OK\r\n");
			}
			else
				UART_PC_Send((char *)"ERROR\r\n");

		}
		//example.	duration,00,10,00,1
		else if (!strncmp((const char *)&buffUART1rx2[0], (const char *)"duration,", (sizeof("duration,") - 1)))
		{

			if (((buffUART1rx2[18] - 48) > 0) && ((buffUART1rx2[18] - 48) < 4) && (buffUART1rx2[17] == ','))
			{
				duration.hours =  	(buffUART1rx2[9] - 48) * 10 + (buffUART1rx2[10] - 48);
				duration.minutes =	(buffUART1rx2[12] - 48) * 10 + (buffUART1rx2[13] - 48);
				duration.seconds =	(buffUART1rx2[15] - 48) * 10 + (buffUART1rx2[16] - 48);

				Session_Set_Duration (&session_slot_aux, (buffUART1rx2[18] - 48), &duration);

				UART_PC_Send((char *)"OK\r\n");
			}
			else
				UART_PC_Send((char *)"ERROR\r\n");
		}

		else if (!strncmp((const char *)&buffUART1rx2[0], (const char *)"save,", (sizeof("save,") - 1)))
		{

			if (((buffUART1rx2[5] - 48) < 2) && (buffUART1rx2[6] - 48 < 10))
			{
				//Session_Save (&session_slot_aux, buffUART3rx2[5] - 48);
				i = FLASH_Program(&session_slot_aux, ((buffUART1rx2[5] - 48) * 10 + (buffUART1rx2[6] - 48)));

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

		else if (!strncmp((const char *)&buffUART1rx2[0], (const char *)"reset", (sizeof("reset") - 1)))
		{
			UART_PC_Send("Restarting...\r\n");
			Wait_ms(1000);
			NVIC_SystemReset();
		}

		else if (!strncmp((const char *)&buffUART1rx2[0], (const char *)"pause,", (sizeof("pause,") - 1)))
		{
			if (buffUART1rx2[6] - 48)
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

		else if (!strncmp((const char *)&buffUART1rx2[0], (const char *)"load,", (sizeof("load,") - 1)))
		{
			//load,slot[2],destino[1]
			if (((buffUART1rx2[6] - 48) < 10) && ((buffUART1rx2[5] - 48) < 2))
			{
				Session_Load (&session_slot_aux, (((buffUART1rx2[5] - 48) * 10) + buffUART1rx2[6] - 48), 0);
				Session_Load (&session_slot_aux, (((buffUART1rx2[5] - 48) * 10) + buffUART1rx2[6] - 48), 1);
				Session_Load (&session_slot_aux, (((buffUART1rx2[5] - 48) * 10) + buffUART1rx2[6] - 48), 2);
				Session_Load (&session_slot_aux, (((buffUART1rx2[5] - 48) * 10) + buffUART1rx2[6] - 48), 3);
				Session_Load (&session_slot_aux, (((buffUART1rx2[5] - 48) * 10) + buffUART1rx2[6] - 48), 4);
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
		else if (!strncmp((const char *)&buffUART1rx2[0], (const char *)"state_of_stage,", (sizeof("state_of_stage,") - 1)))
		{

			if (((buffUART1rx2[15] - 48) == 0) || ((buffUART1rx2[15] - 48) == 1))
			{

				Session_Set_Status (&session_slot_aux, (buffUART1rx2[17] - 48),(buffUART1rx2[15] - 48));

				UART_PC_Send((char *)"OK\r\n");
			}
			else
				UART_PC_Send((char *)"ERROR\r\n");

		}

		else if (!strncmp((const char *)&buffUART1rx2[0], (const char *)"special_function,", (sizeof("special_function,") - 1)))
		{

			UART_PC_Send((char *)"OK\r\n");
		}

		else if (!strncmp((const char *)&buffUART1rx2[0], (const char *)"start,", (sizeof("start,") - 1)))
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

		else if (!strncmp((const char *)&buffUART1rx2[0], (const char *)"stop,", (sizeof("stop,") - 1)))
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
		else if (!strncmp((const char *)&buffUART1rx2[0], (const char *)"finish_ok,", (sizeof("finish_ok,") - 1)))
		{
			Session_Channel_1_Stop();
			Session_Channel_2_Stop();
			Session_Channel_3_Stop();
			Session_Channel_4_Stop();

			BuzzerCommands(BUZZER_MULTIPLE_SHORT, 3);
			UART_PC_Send((char *)"OK\r\n");
		}
#endif

		else if (!strncmp((const char *)&buffUART1rx2[0], (const char *)"read_channel,", (sizeof("read_channel,") - 1)))
		{
			if (((buffUART1rx2[13] - 48) < 5) && ((buffUART1rx2[13] - 48) > 0))
			{
				Channel_Load (&session_slot_aux, (buffUART1rx2[13] - 48));

				//--- Send slot content ---//
				SessionSend(&session_slot_aux);

				UART_PC_Send((char *)"OK\r\n");
			}
			else
				UART_PC_Send((char *)"ERROR\r\n");
		}

		else if (!strncmp((const char *)&buffUART1rx2[0], (const char *)"read_slot,", (sizeof("read_slot,") - 1)))
		{
			//load,slot[2],destino[1]
			if ((((buffUART1rx2[11] - 48) + (buffUART1rx2[10] - 48) * 10) < 11))
			{
				Session_Load (&session_slot_aux, (((buffUART1rx2[10] - 48) * 10) + buffUART1rx2[11] - 48), 0);

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
		PacketReadyUART1 = 0;
	}


}

//--- Channel 1 ---//
void UART2_Receive (void)
{

	unsigned short aux;
	antenna_typedef antenna_aux;

	if (msUART2rxTimeOut == 0)
	{

		MSUART2RXTIMEOUT;

		pBuffUART2rxW = &buffUART2rx[0];
		pBuffUART2rxR = &buffUART2rx[0];
	}

	if (PacketReadyUART2)
	{


		//--- Introduce your code here ---//
		//UART1Send((char *)&buffUART1rx2[0]);

		//temp,055.00\r\n
		if (!strncmp((const char *)&buffUART2rx2[0], (const char *)"temp", (sizeof("temp") - 1)))
		{

			if ((buffUART2rx2[4] == ',') && (buffUART2rx2[8] == '.') && (buffUART2rx2[11] == '\r'))
			{
				aux = (buffUART2rx2[5] - 48) * 100;
				aux += (buffUART2rx2[6] - 48) * 10;
				aux += (buffUART2rx2[7] - 48);

				temp_actual_channel_1_int = aux;

				if (temp_actual_channel_1_int != 25)
					temp_actual_channel_1_int +=1;

				aux = (buffUART2rx2[9] - 48) * 10;
				aux += (buffUART2rx2[10] - 48);

				temp_actual_channel_1_dec = aux;

				strcpy((char *)&buffUART2rx2[11], (const char *)",1\r\n");
				UART_PC_Send((char *)&buffUART2rx2[0]);
				UART_CH1_Send("OK\r\n");
			}
		}

		//ant0,012.27,087.90,001.80,065.00\r\n.
		else if (!strncmp((const char *)&buffUART2rx2[0], (const char *)"ant", (sizeof("ant") - 1)))
		{

			strcpy((char *)&buffUART2rx2[32], (const char *)",1\r\n");
			UART_PC_Send((char *)&buffUART2rx2[0]);

			if ((buffUART2rx2[4] == ',') && (buffUART2rx2[11] == ',') && (buffUART2rx2[18] == ',') && (buffUART2rx2[25] == ','))
			{
				aux = (buffUART2rx2[5] - 48) * 100;
				aux += (buffUART2rx2[6] - 48) * 10;
				aux += (buffUART2rx2[7] - 48);

				antenna_aux.resistance_int = aux;

				aux = (buffUART2rx2[9] - 48) * 10;
				aux += (buffUART2rx2[10] - 48);

				antenna_aux.resistance_dec = aux;

				aux = (buffUART2rx2[12] - 48) * 100;
				aux += (buffUART2rx2[13] - 48) * 10;
				aux += (buffUART2rx2[14] - 48);

				antenna_aux.inductance_int = aux;

				aux = (buffUART2rx2[16] - 48) * 10;
				aux += (buffUART2rx2[17] - 48);

				antenna_aux.inductance_dec = aux;

				aux = (buffUART2rx2[19] - 48) * 100;
				aux += (buffUART2rx2[20] - 48) * 10;
				aux += (buffUART2rx2[21] - 48);

				antenna_aux.current_limit_int = aux;

				aux = (buffUART2rx2[23] - 48) * 10;
				aux += (buffUART2rx2[24] - 48);

				antenna_aux.current_limit_dec = aux;

				aux = (buffUART2rx2[26] - 48) * 100;
				aux += (buffUART2rx2[27] - 48) * 10;
				aux += (buffUART2rx2[28] - 48);

				antenna_aux.temp_max_int = aux;

				aux = (buffUART2rx2[30] - 48) * 10;
				aux += (buffUART2rx2[31] - 48);

				antenna_aux.temp_max_dec = aux;

				Session_Set_Antenna (&session_ch_1, aux , &antenna_aux);
				Save_Antenna_Name(CH1, (char *) buffUART2rx2);



				UART_CH1_Send((char *) "OK\r\n");
			}
			else
				UART_CH1_Send((char *) "ERROR\r\n");

		}
		//--- end ---//

		PacketReadyUART2 = 0;
	}
}

void UART3_Receive (void)
{
	unsigned short aux;
	antenna_typedef antenna_aux;

	if (msUART3rxTimeOut == 0)
	{

		MSUART3RXTIMEOUT;

		pBuffUART3rxW = &buffUART3rx[0];
		pBuffUART3rxR = &buffUART3rx[0];
	}

	if (PacketReadyUART3)
	{
		//--- Introduce your code here ---//
		//UART2Send((char *)&buffUART2rx2[0]);

		//temp,055.00\r\n
		if (!strncmp((const char *)&buffUART3rx2[0], (const char *)"temp", (sizeof("temp") - 1)))
		{
			if ((buffUART3rx2[4] == ',') && (buffUART3rx2[8] == '.') && (buffUART3rx2[11] == '\r'))
			{
				aux = (buffUART3rx2[5] - 48) * 100;
				aux += (buffUART3rx2[6] - 48) * 10;
				aux += (buffUART3rx2[7] - 48);

				temp_actual_channel_2_int = aux;

				aux = (buffUART3rx2[9] - 48) * 10;
				aux += (buffUART3rx2[10] - 48);

				temp_actual_channel_2_dec = aux;

				strcpy((char *)&buffUART3rx2[11], (const char *)",2\r\n");
				UART_PC_Send((char *)&buffUART3rx2[0]);
				UART_CH2_Send("OK\r\n");
			}
		}

		//ant0,012.27,087.90,001.80,065.00\r\n.
		else if (!strncmp((const char *)&buffUART3rx2[0], (const char *)"ant", (sizeof("ant") - 1)))
		{
			strcpy((char *)&buffUART3rx2[32], (const char *)",2\r\n");
			UART_PC_Send((char *)&buffUART3rx2[0]);

			if ((buffUART3rx2[4] == ',') && (buffUART3rx2[11] == ',') && (buffUART3rx2[18] == ',') && (buffUART3rx2[25] == ','))
			{
				aux = (buffUART3rx2[5] - 48) * 100;
				aux += (buffUART3rx2[6] - 48) * 10;
				aux += (buffUART3rx2[7] - 48);

				antenna_aux.resistance_int = aux;

				aux = (buffUART3rx2[9] - 48) * 10;
				aux += (buffUART3rx2[10] - 48);

				antenna_aux.resistance_dec = aux;

				aux = (buffUART3rx2[12] - 48) * 100;
				aux += (buffUART3rx2[13] - 48) * 10;
				aux += (buffUART3rx2[14] - 48);

				antenna_aux.inductance_int = aux;

				aux = (buffUART3rx2[16] - 48) * 10;
				aux += (buffUART3rx2[17] - 48);

				antenna_aux.inductance_dec = aux;

				aux = (buffUART3rx2[19] - 48) * 100;
				aux += (buffUART3rx2[20] - 48) * 10;
				aux += (buffUART3rx2[21] - 48);

				antenna_aux.current_limit_int = aux;

				aux = (buffUART3rx2[23] - 48) * 10;
				aux += (buffUART3rx2[24] - 48);

				antenna_aux.current_limit_dec = aux;

				aux = (buffUART3rx2[26] - 48) * 100;
				aux += (buffUART3rx2[27] - 48) * 10;
				aux += (buffUART3rx2[28] - 48);

				antenna_aux.temp_max_int = aux;

				aux = (buffUART3rx2[30] - 48) * 10;
				aux += (buffUART3rx2[31] - 48);

				antenna_aux.temp_max_dec = aux;


				Session_Set_Antenna (&session_ch_2, aux , &antenna_aux);
				Save_Antenna_Name(CH2, (char *) buffUART3rx2);

				UART_CH2_Send((char *) "OK\r\n");
			}
			else
				UART_CH2_Send((char *) "ERROR\r\n");
		}
		//--- end ---//

		PacketReadyUART3 = 0;
	}
}

void UART4_Receive (void)
{
	unsigned short aux;
	antenna_typedef antenna_aux;

	if (msUART4rxTimeOut == 0)
	{

		MSUART4RXTIMEOUT;

		pBuffUART4rxW = &buffUART4rx[0];
		pBuffUART4rxR = &buffUART4rx[0];
	}

	if (PacketReadyUART4)
	{
		//--- Introduce your code here ---//
		//UART4Send((char *)&buffUART4rx2[0]);

		//temp,055.00\r\n
		if (!strncmp((const char *)&buffUART4rx2[0], (const char *)"temp", (sizeof("temp") - 1)))
		{
			if ((buffUART4rx2[4] == ',') && (buffUART4rx2[8] == '.') && (buffUART4rx2[11] == '\r'))
			{
				aux = (buffUART4rx2[5] - 48) * 100;
				aux += (buffUART4rx2[6] - 48) * 10;
				aux += (buffUART4rx2[7] - 48);

				temp_actual_channel_3_int = aux;

				aux = (buffUART4rx2[9] - 48) * 10;
				aux += (buffUART4rx2[10] - 48);

				temp_actual_channel_3_dec = aux;

				strcpy((char *)&buffUART4rx2[11], (const char *)",3\r\n");
				UART_PC_Send((char *)&buffUART4rx2[0]);
				UART_CH3_Send("OK\r\n");
			}
		}

		//ant0,012.27,087.90,001.80,065.00\r\n.
		else if (!strncmp((const char *)&buffUART4rx2[0], (const char *)"ant", (sizeof("ant") - 1)))
		{

			strcpy((char *)&buffUART4rx2[32], (const char *)",3\r\n");
			UART_PC_Send((char *)&buffUART4rx2[0]);

			if ((buffUART4rx2[4] == ',') && (buffUART4rx2[11] == ',') && (buffUART4rx2[18] == ',') && (buffUART4rx2[25] == ','))
			{
				aux = (buffUART4rx2[5] - 48) * 100;
				aux += (buffUART4rx2[6] - 48) * 10;
				aux += (buffUART4rx2[7] - 48);

				antenna_aux.resistance_int = aux;

				aux = (buffUART4rx2[9] - 48) * 10;
				aux += (buffUART4rx2[10] - 48);

				antenna_aux.resistance_dec = aux;

				aux = (buffUART4rx2[12] - 48) * 100;
				aux += (buffUART4rx2[13] - 48) * 10;
				aux += (buffUART4rx2[14] - 48);

				antenna_aux.inductance_int = aux;

				aux = (buffUART4rx2[16] - 48) * 10;
				aux += (buffUART4rx2[17] - 48);

				antenna_aux.inductance_dec = aux;

				aux = (buffUART4rx2[19] - 48) * 100;
				aux += (buffUART4rx2[20] - 48) * 10;
				aux += (buffUART4rx2[21] - 48);

				antenna_aux.current_limit_int = aux;

				aux = (buffUART4rx2[23] - 48) * 10;
				aux += (buffUART4rx2[24] - 48);

				antenna_aux.current_limit_dec = aux;

				aux = (buffUART4rx2[26] - 48) * 100;
				aux += (buffUART4rx2[27] - 48) * 10;
				aux += (buffUART4rx2[28] - 48);

				antenna_aux.temp_max_int = aux;

				aux = (buffUART4rx2[30] - 48) * 10;
				aux += (buffUART4rx2[31] - 48);

				antenna_aux.temp_max_dec = aux;

				Session_Set_Antenna (&session_ch_3, aux , &antenna_aux);
				Save_Antenna_Name(CH3, (char *) buffUART4rx2);

				UART4Send((char *) "OK\r\n");
			}
			else
				UART4Send((char *) "ERROR\r\n");
		}
		//--- end ---//
		PacketReadyUART4 = 0;
	}
}

void UART5_Receive (void)
{
	unsigned short aux;
	antenna_typedef antenna_aux;

	if (msUART5rxTimeOut == 0)
	{

		MSUART5RXTIMEOUT;

		pBuffUART5rxW = &buffUART5rx[0];
		pBuffUART5rxR = &buffUART5rx[0];
	}

	if (PacketReadyUART5)
	{

		//--- Introduce your code here ---//
		//UART5Send((char *)&buffUART5rx2[0]);

		//temp,055.00\r\n
		if (!strncmp((const char *)&buffUART5rx2[0], (const char *)"temp", (sizeof("temp") - 1)))
		{
			if ((buffUART5rx2[4] == ',') && (buffUART5rx2[8] == '.') && (buffUART5rx2[11] == '\r'))
			{
				aux = (buffUART5rx2[5] - 48) * 100;
				aux += (buffUART5rx2[6] - 48) * 10;
				aux += (buffUART5rx2[7] - 48);

				temp_actual_channel_4_int = aux;

				aux = (buffUART5rx2[9] - 48) * 10;
				aux += (buffUART5rx2[10] - 48);

				temp_actual_channel_4_dec = aux;

				strcpy((char *)&buffUART5rx2[11], (const char *)",4\r\n");
				UART_PC_Send((char *)&buffUART5rx2[0]);
				UART_CH4_Send("OK\r\n");
			}
		}

		//ant0,012.27,087.90,001.80,065.00\r\n.
		else if (!strncmp((const char *)&buffUART5rx2[0], (const char *)"ant", (sizeof("ant") - 1)))
		{
			strcpy((char *)&buffUART5rx2[32], (const char *)",4\r\n");
			UART_PC_Send((char *) &buffUART5rx2[0]);

			if ((buffUART5rx2[4] == ',') && (buffUART5rx2[11] == ',') && (buffUART5rx2[18] == ',') && (buffUART5rx2[25] == ','))
			{
				aux = (buffUART5rx2[5] - 48) * 100;
				aux += (buffUART5rx2[6] - 48) * 10;
				aux += (buffUART5rx2[7] - 48);

				antenna_aux.resistance_int = aux;

				aux = (buffUART5rx2[9] - 48) * 10;
				aux += (buffUART5rx2[10] - 48);

				antenna_aux.resistance_dec = aux;

				aux = (buffUART5rx2[12] - 48) * 100;
				aux += (buffUART5rx2[13] - 48) * 10;
				aux += (buffUART5rx2[14] - 48);

				antenna_aux.inductance_int = aux;

				aux = (buffUART5rx2[16] - 48) * 10;
				aux += (buffUART5rx2[17] - 48);

				antenna_aux.inductance_dec = aux;

				aux = (buffUART5rx2[19] - 48) * 100;
				aux += (buffUART5rx2[20] - 48) * 10;
				aux += (buffUART5rx2[21] - 48);

				antenna_aux.current_limit_int = aux;

				aux = (buffUART5rx2[23] - 48) * 10;
				aux += (buffUART5rx2[24] - 48);

				antenna_aux.current_limit_dec = aux;

				aux = (buffUART5rx2[26] - 48) * 100;
				aux += (buffUART5rx2[27] - 48) * 10;
				aux += (buffUART5rx2[28] - 48);

				antenna_aux.temp_max_int = aux;

				aux = (buffUART5rx2[30] - 48) * 10;
				aux += (buffUART5rx2[31] - 48);

				antenna_aux.temp_max_dec = aux;

				Session_Set_Antenna (&session_ch_4, aux , &antenna_aux);
				Save_Antenna_Name(CH4, (char *) buffUART5rx2);

				UART5Send((char *) "OK\r\n");
			}
			else
				UART5Send((char *) "ERROR\r\n");
		}
		//--- end ---//

		PacketReadyUART5 = 0;
	}
}



//---- end of file ----//
