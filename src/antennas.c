//---------------------------------------------------------
// #### PROYECTO MAGNETO GAUSSTEK - MotherBoard ###########
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F103
// ##
// #### ANTENNAS.C ###########################################
//---------------------------------------------------------

/* Includes ------------------------------------------------------------------*/
#include "antennas.h"
#include "usart.h"
#include "GTK_Estructura.h"
// #include "GTK_Signal.h"
// #include "flash_program.h"
// #include "GTK_Hard.h"
// #include "timer.h"

// #include <string.h>
// #include <stdio.h>


//--- Used for Antenna Sync - Time base ---//
#define SESSION_CHANNEL_ASK_TEMP 2500 //2.5 segundos
#define SESSION_CHANNEL_ANSWER_TEMP 10000 //10segundos.

#define SESSION_CHANNEL_1_VERIFY_ANTENNA_TIME 5000			//ms
#define SESSION_CHANNEL_2_VERIFY_ANTENNA_TIME 5000			//ms
#define SESSION_CHANNEL_3_VERIFY_ANTENNA_TIME 5000			//ms
#define SESSION_CHANNEL_4_VERIFY_ANTENNA_TIME 5000			//ms

/* Externals ------------------------------------------------------------------*/
extern volatile unsigned short antenna_timer;

// extern session_typedef session_ch_1;
// extern session_typedef session_ch_2;
// extern session_typedef session_ch_3;
// extern session_typedef session_ch_4;


/* Globals ----------------------------------------------------------------------------*/
antenna_state_t antenna_state;

unsigned char Session_Channel_1_Verify_Antenna_state = 0;
unsigned short Session_Channel_1_Verify_Antenna_time = 0;
unsigned char Session_Channel_2_Verify_Antenna_state = 0;
unsigned short Session_Channel_2_Verify_Antenna_time = 0;
unsigned char Session_Channel_3_Verify_Antenna_state = 0;
unsigned short Session_Channel_3_Verify_Antenna_time = 0;
unsigned char Session_Channel_4_Verify_Antenna_state = 0;
unsigned short Session_Channel_4_Verify_Antenna_time = 0;

unsigned short session_channel_1_ask_temp = 0;
unsigned short session_channel_1_answer_temp = 0;
unsigned short session_channel_2_ask_temp = 0;
unsigned short session_channel_2_answer_temp = 0;
unsigned short session_channel_3_ask_temp = 0;
unsigned short session_channel_3_answer_temp = 0;
unsigned short session_channel_4_ask_temp = 0;
unsigned short session_channel_4_answer_temp = 0;

/* Module Private Functions -----------------------------------------------------------*/



/* Module Exported Functions ----------------------------------------------------------*/
void AntennaUpdateStates (void)
{
    switch (antenna_state)
    {
    case ANTENNA_INIT:
        break;

    case ANTENNA_IN_STANDBY:
        break;

    case ANTENNA_IN_TREATMENT:
        break;

    case ANTENNA_IN_PAUSE:
        break;

    default:
        antenna_state = ANTENNA_INIT;
        break;
    }
    
    if (!antenna_timer)
    {
        // if (session_ch_1.status == 0)
            UART_CH1_Send("keepalive\r\n");
        // if (session_ch_2.status == 0)
            UART_CH2_Send("keepalive\r\n");
        // if (session_ch_3.status == 0)
            UART_CH3_Send("keepalive\r\n");
        // if (session_ch_4.status == 0)
            UART_CH4_Send("keepalive\r\n");

        antenna_timer = ANTENNA_KEEP_ALIVE_TIMER;
    }
}


void Session_Clear_Antenna (session_typedef * ptr_session, unsigned char stage)
{
	//Resistance.
	ptr_session->stage_1_resistance_int = 0;
	ptr_session->stage_1_resistance_dec = 0;

	//Inductance.
	ptr_session->stage_1_inductance_int = 0;
	ptr_session->stage_1_inductance_dec = 0;

	//Current limit.
	ptr_session->stage_1_current_limit_int = 0;
	ptr_session->stage_1_current_limit_dec = 0;

}

//carga el valor de antenna que recibe por la uart en la estructura de session
//los parametros le llegan por el *ptr_antenna y donde guardarlo en *ptr_session
void Session_Set_Antenna (session_typedef * ptr_session, unsigned char stage , antenna_typedef * ptr_antenna)
{
	unsigned char a;
	unsigned short aa;

	//Resistance.
	aa = ptr_antenna->resistance_int;
	if (aa <= 999)
		ptr_session->stage_1_resistance_int = aa;
	else
		ptr_session->stage_1_resistance_int = 999;

	a = ptr_antenna->resistance_dec;
	ptr_session->stage_1_resistance_dec = a;

	//Inductance.
	aa = ptr_antenna->inductance_int;
	if (aa <= 999)
		ptr_session->stage_1_inductance_int = aa;
	else
		ptr_session->stage_1_inductance_int = 999;

	a = ptr_antenna->inductance_dec;
	ptr_session->stage_1_inductance_dec = a;

	//Current limit.
	a = ptr_antenna->current_limit_int;
	ptr_session->stage_1_current_limit_int = a;
	a = ptr_antenna->current_limit_dec;
	ptr_session->stage_1_current_limit_dec = a;

	//Temperature limit.
	a = ptr_antenna->temp_max_int;
	ptr_session->stage_1_temp_max_int = a;
	a = ptr_antenna->temp_max_dec;
	ptr_session->stage_1_temp_max_dec = a;

}

//Preguntaa la sesion los valores de parametros antenna conectada
//devuelve info por el puntero *ptr_antenna
void Session_Get_Antenna (session_typedef * ptr_session, unsigned char stage , antenna_typedef * ptr_antenna)
{
	unsigned char a;
	unsigned short aa;

	//Resistance.
	aa = ptr_session->stage_1_resistance_int;
	ptr_antenna->resistance_int = aa;
	a = ptr_session->stage_1_resistance_dec;
	ptr_antenna->resistance_dec = a;

	//Inductance.
	aa = ptr_session->stage_1_inductance_int;
	ptr_antenna->inductance_int = aa;
	a = ptr_session->stage_1_inductance_dec;
	ptr_antenna->inductance_dec = a;

	//Current limit.
	a = ptr_session->stage_1_current_limit_int;
	ptr_antenna->current_limit_int = a;
	a = ptr_session->stage_1_current_limit_dec;
	ptr_antenna->current_limit_dec = a;

	//Temperature limit.
	a = ptr_session->stage_1_temp_max_int;
	ptr_antenna->temp_max_int = a;
	a = ptr_session->stage_1_temp_max_dec;
	ptr_antenna->temp_max_dec = a;

}

unsigned char Session_Channel_1_Verify_Antenna (session_typedef * ptr_session)
{
	switch (Session_Channel_1_Verify_Antenna_state)
	{
		case SESSION_CHANNEL_1_VERIFY_ANTENNA_INIT:

			Session_Clear_Antenna (ptr_session, SESSION_STAGE_1);
//			//Resistance.
//			ptr_session->stage_1_resistance_int = 0;
//			ptr_session->stage_1_resistance_dec = 0;
//
//			//Inductance.
//			ptr_session->stage_1_inductance_int = 0;
//			ptr_session->stage_1_inductance_dec = 0;
//
//			//Current limit.
//			ptr_session->stage_1_current_limit_int = 0;
//			ptr_session->stage_1_current_limit_dec = 0;


			UART_PC_Send("Getting antenna parameters of channel 1\r\n");
			UART_CH1_Send("get_params\r\n");

			Session_Channel_1_Verify_Antenna_time = SESSION_CHANNEL_1_VERIFY_ANTENNA_TIME;
			Session_Channel_1_Verify_Antenna_state = SESSION_CHANNEL_1_VERIFY_ANTENNA_WAIT_PARAMS;
			break;

		case SESSION_CHANNEL_1_VERIFY_ANTENNA_WAIT_PARAMS:

			if (Session_Channel_1_Verify_Antenna_time == 0)
				Session_Channel_1_Verify_Antenna_state = SESSION_CHANNEL_1_VERIFY_ANTENNA_FIN_ERROR;
			else
			{
				if 	(((ptr_session->stage_1_resistance_int != 0) || (ptr_session->stage_1_resistance_dec != 0))
					&& ((ptr_session->stage_1_inductance_int != 0) || (ptr_session->stage_1_inductance_dec != 0))
					&& ((ptr_session->stage_1_current_limit_int != 0) || (ptr_session->stage_1_current_limit_dec != 0)))
				{
					Session_Channel_1_Verify_Antenna_state = SESSION_CHANNEL_1_VERIFY_ANTENNA_FIN_OK;
				}
			}
			break;

		case SESSION_CHANNEL_1_VERIFY_ANTENNA_FIN_OK:

			UART_PC_Send("Antenna detected on CH1\r\n");
			Session_Channel_1_Verify_Antenna_state = SESSION_CHANNEL_1_VERIFY_ANTENNA_INIT;
			return FIN_OK;
			break;

		case SESSION_CHANNEL_1_VERIFY_ANTENNA_FIN_ERROR:

			Session_Channel_1_Verify_Antenna_state = SESSION_CHANNEL_1_VERIFY_ANTENNA_INIT;
			return FIN_ERROR;
			break;

		default:
			Session_Channel_1_Verify_Antenna_state = SESSION_CHANNEL_1_VERIFY_ANTENNA_INIT;
			break;
	}

	return TRABAJANDO;
}


unsigned char Session_Channel_2_Verify_Antenna (session_typedef * ptr_session)
{
	switch (Session_Channel_2_Verify_Antenna_state)
	{
		case SESSION_CHANNEL_2_VERIFY_ANTENNA_INIT:

			Session_Clear_Antenna (ptr_session, SESSION_STAGE_1);
			//Session_Clear_Antenna (ptr_session, SESSION_STAGE_2);
			//Session_Clear_Antenna (ptr_session, SESSION_STAGE_3);

			UART_PC_Send("Getting antenna parameters of channel 2\r\n");
			UART_CH2_Send("get_params\r\n");

			Session_Channel_2_Verify_Antenna_time = SESSION_CHANNEL_2_VERIFY_ANTENNA_TIME;
			Session_Channel_2_Verify_Antenna_state = SESSION_CHANNEL_2_VERIFY_ANTENNA_WAIT_PARAMS;
			break;

		case SESSION_CHANNEL_2_VERIFY_ANTENNA_WAIT_PARAMS:

			if (Session_Channel_2_Verify_Antenna_time == 0)
				Session_Channel_2_Verify_Antenna_state = SESSION_CHANNEL_2_VERIFY_ANTENNA_FIN_ERROR;
			else
			{
				if 	(((ptr_session->stage_1_resistance_int != 0) || (ptr_session->stage_1_resistance_dec != 0))
					&& ((ptr_session->stage_1_inductance_int != 0) || (ptr_session->stage_1_inductance_dec != 0))
					&& ((ptr_session->stage_1_current_limit_int != 0) || (ptr_session->stage_1_current_limit_dec != 0)))
				{
					Session_Channel_2_Verify_Antenna_state = SESSION_CHANNEL_2_VERIFY_ANTENNA_FIN_OK;
				}
			}
			break;

		case SESSION_CHANNEL_2_VERIFY_ANTENNA_FIN_OK:

			UART_PC_Send("Antenna detected on CH2\r\n");
			Session_Channel_2_Verify_Antenna_state = SESSION_CHANNEL_2_VERIFY_ANTENNA_INIT;
			return FIN_OK;
			break;

		case SESSION_CHANNEL_2_VERIFY_ANTENNA_FIN_ERROR:

			Session_Channel_2_Verify_Antenna_state = SESSION_CHANNEL_2_VERIFY_ANTENNA_INIT;
			return FIN_ERROR;
			break;

		default:
			Session_Channel_2_Verify_Antenna_state = SESSION_CHANNEL_2_VERIFY_ANTENNA_INIT;
			break;
	}

	return TRABAJANDO;
}

unsigned char Session_Channel_3_Verify_Antenna (session_typedef * ptr_session)
{
	switch (Session_Channel_3_Verify_Antenna_state)
	{
		case SESSION_CHANNEL_3_VERIFY_ANTENNA_INIT:

			Session_Clear_Antenna (ptr_session, SESSION_STAGE_1);
			//Session_Clear_Antenna (ptr_session, SESSION_STAGE_2);
			//Session_Clear_Antenna (ptr_session, SESSION_STAGE_3);

			UART_PC_Send("Getting antenna parameters of channel 3\r\n");
			UART_CH3_Send("get_params\r\n");

			Session_Channel_3_Verify_Antenna_time = SESSION_CHANNEL_3_VERIFY_ANTENNA_TIME;
			Session_Channel_3_Verify_Antenna_state = SESSION_CHANNEL_3_VERIFY_ANTENNA_WAIT_PARAMS;
			break;

		case SESSION_CHANNEL_3_VERIFY_ANTENNA_WAIT_PARAMS:

			if (Session_Channel_3_Verify_Antenna_time == 0)
				Session_Channel_3_Verify_Antenna_state = SESSION_CHANNEL_3_VERIFY_ANTENNA_FIN_ERROR;
			else
			{
				if 	(((ptr_session->stage_1_resistance_int != 0) || (ptr_session->stage_1_resistance_dec != 0))
					&& ((ptr_session->stage_1_inductance_int != 0) || (ptr_session->stage_1_inductance_dec != 0))
					&& ((ptr_session->stage_1_current_limit_int != 0) || (ptr_session->stage_1_current_limit_dec != 0)))
				{
					Session_Channel_3_Verify_Antenna_state = SESSION_CHANNEL_3_VERIFY_ANTENNA_FIN_OK;
				}
			}
			break;

		case SESSION_CHANNEL_3_VERIFY_ANTENNA_FIN_OK:

			UART_PC_Send("Antenna detected on CH3\r\n");
			Session_Channel_3_Verify_Antenna_state = SESSION_CHANNEL_3_VERIFY_ANTENNA_INIT;
			return FIN_OK;
			break;

		case SESSION_CHANNEL_3_VERIFY_ANTENNA_FIN_ERROR:

			Session_Channel_3_Verify_Antenna_state = SESSION_CHANNEL_3_VERIFY_ANTENNA_INIT;
			return FIN_ERROR;
			break;

		default:
			Session_Channel_3_Verify_Antenna_state = SESSION_CHANNEL_3_VERIFY_ANTENNA_INIT;
			break;
	}

	return TRABAJANDO;
}

unsigned char Session_Channel_4_Verify_Antenna (session_typedef * ptr_session)
{
	switch (Session_Channel_4_Verify_Antenna_state)
	{
		case SESSION_CHANNEL_4_VERIFY_ANTENNA_INIT:

			Session_Clear_Antenna (ptr_session, SESSION_STAGE_1);
			//Session_Clear_Antenna (ptr_session, SESSION_STAGE_2);
			//Session_Clear_Antenna (ptr_session, SESSION_STAGE_3);

			UART_PC_Send("Getting antenna parameters of channel 4\r\n");
			UART_CH4_Send("get_params\r\n");

			Session_Channel_4_Verify_Antenna_time = SESSION_CHANNEL_4_VERIFY_ANTENNA_TIME;
			Session_Channel_4_Verify_Antenna_state = SESSION_CHANNEL_4_VERIFY_ANTENNA_WAIT_PARAMS;
			break;

		case SESSION_CHANNEL_4_VERIFY_ANTENNA_WAIT_PARAMS:

			if (Session_Channel_4_Verify_Antenna_time == 0)
				Session_Channel_4_Verify_Antenna_state = SESSION_CHANNEL_4_VERIFY_ANTENNA_FIN_ERROR;
			else
			{
				if 	(((ptr_session->stage_1_resistance_int != 0) || (ptr_session->stage_1_resistance_dec != 0))
					&& ((ptr_session->stage_1_inductance_int != 0) || (ptr_session->stage_1_inductance_dec != 0))
					&& ((ptr_session->stage_1_current_limit_int != 0) || (ptr_session->stage_1_current_limit_dec != 0)))
				{
					Session_Channel_4_Verify_Antenna_state = SESSION_CHANNEL_4_VERIFY_ANTENNA_FIN_OK;
				}
			}
			break;

		case SESSION_CHANNEL_4_VERIFY_ANTENNA_FIN_OK:

			UART_PC_Send("Antenna detected on CH4\r\n");
			Session_Channel_4_Verify_Antenna_state = SESSION_CHANNEL_4_VERIFY_ANTENNA_INIT;
			return FIN_OK;
			break;

		case SESSION_CHANNEL_4_VERIFY_ANTENNA_FIN_ERROR:
			Session_Channel_4_Verify_Antenna_state = SESSION_CHANNEL_4_VERIFY_ANTENNA_INIT;
			return FIN_ERROR;
			break;

		default:
			Session_Channel_4_Verify_Antenna_state = SESSION_CHANNEL_2_VERIFY_ANTENNA_INIT;
			break;
	}

	return TRABAJANDO;
}

void Signal_TIM1MS (void)
{

	if (channel_1_pause == 0)
	{
		if (Session_Channel_1_Verify_Antenna_time)
			Session_Channel_1_Verify_Antenna_time--;

		if (session_channel_1_ask_temp)
			session_channel_1_ask_temp--;

		if (session_channel_1_answer_temp)
			session_channel_1_answer_temp--;
	}

	if (channel_2_pause == 0)
	{

		if (Session_Channel_2_Verify_Antenna_time)
			Session_Channel_2_Verify_Antenna_time--;

		if (session_channel_2_ask_temp)
			session_channel_2_ask_temp--;

		if (session_channel_2_answer_temp)
			session_channel_2_answer_temp--;
	}

	if (channel_3_pause == 0)
	{

		if (Session_Channel_3_Verify_Antenna_time)
			Session_Channel_3_Verify_Antenna_time--;

		if (session_channel_3_ask_temp)
			session_channel_3_ask_temp--;

		if (session_channel_3_answer_temp)
			session_channel_3_answer_temp--;

	}

	if (channel_4_pause == 0)
	{

		if (Session_Channel_4_Verify_Antenna_time)
			Session_Channel_4_Verify_Antenna_time--;

		if (session_channel_4_ask_temp)
			session_channel_4_ask_temp--;

		if (session_channel_4_answer_temp)
			session_channel_4_answer_temp--;
	}
}

//---- end of file ----//
