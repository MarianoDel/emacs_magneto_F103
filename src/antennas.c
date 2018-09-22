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
#include "GTK_Hard.h"        //define el nombre de los canales
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

extern session_typedef session_ch_1;
extern session_typedef session_ch_2;
extern session_typedef session_ch_3;
extern session_typedef session_ch_4;


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

unsigned char ch1_ant_current_temp_int = 0;
unsigned char ch1_ant_current_temp_dec = 0;
unsigned char ch1_ant_conn = 0;
unsigned char ch2_ant_current_temp_int = 0;
unsigned char ch2_ant_current_temp_dec = 0;
unsigned char ch2_ant_conn = 0;
unsigned char ch3_ant_current_temp_int = 0;
unsigned char ch3_ant_current_temp_dec = 0;
unsigned char ch3_ant_conn = 0;
unsigned char ch4_ant_current_temp_int = 0;
unsigned char ch4_ant_current_temp_dec = 0;
unsigned char ch4_ant_conn = 0;

/* Module Private Functions -----------------------------------------------------------*/
void AntennaLock (unsigned char ch);


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

//la llaman desde el comms para setear los valores actuales de temperatura en la antenas
void AntennaSetCurrentTemp (unsigned char ch, unsigned char t_int, unsigned char t_dec)
{
    switch (ch)
    {
    case CH1:
        ch1_ant_current_temp_int = t_int;
        ch1_ant_current_temp_dec = t_dec;
        ch1_ant_conn = 1;        
        break;

    case CH2:
        ch2_ant_current_temp_int = t_int;
        ch2_ant_current_temp_dec = t_dec;
        ch2_ant_conn = 1;                
        break;

    case CH3:
        ch3_ant_current_temp_int = t_int;
        ch3_ant_current_temp_dec = t_dec;
        ch3_ant_conn = 1;                
        break;

    case CH4:
        ch4_ant_current_temp_int = t_int;
        ch4_ant_current_temp_dec = t_dec;
        ch4_ant_conn = 1;                
        break;
    }    
}

//la llaman desde comms o GTK_Signal para conocer los parametros de antenas
void AntennaGetParams (unsigned char ch,
                       unsigned short *res_int, unsigned char *res_dec,
                       unsigned short *ind_int, unsigned char *ind_dec,
                       unsigned char *cur_int, unsigned char *cur_dec)
{
    switch (ch)
    {
    case CH1:
        *res_int = session_ch_1.ant_resistance_int;
        *res_dec = session_ch_1.ant_resistance_dec;
        *ind_int = session_ch_1.ant_inductance_int;
        *ind_dec = session_ch_1.ant_inductance_dec;
        *cur_int = session_ch_1.ant_current_limit_int;
        *cur_dec = session_ch_1.ant_current_limit_dec;
        break;

    case CH2:
        *res_int = session_ch_2.ant_resistance_int;
        *res_dec = session_ch_2.ant_resistance_dec;
        *ind_int = session_ch_2.ant_inductance_int;
        *ind_dec = session_ch_2.ant_inductance_dec;
        *cur_int = session_ch_2.ant_current_limit_int;
        *cur_dec = session_ch_2.ant_current_limit_dec;        
        break;

    case CH3:
        *res_int = session_ch_3.ant_resistance_int;
        *res_dec = session_ch_3.ant_resistance_dec;
        *ind_int = session_ch_3.ant_inductance_int;
        *ind_dec = session_ch_3.ant_inductance_dec;
        *cur_int = session_ch_3.ant_current_limit_int;
        *cur_dec = session_ch_3.ant_current_limit_dec;        
        break;

    case CH4:
        *res_int = session_ch_4.ant_resistance_int;
        *res_dec = session_ch_4.ant_resistance_dec;
        *ind_int = session_ch_4.ant_inductance_int;
        *ind_dec = session_ch_4.ant_inductance_dec;
        *cur_int = session_ch_4.ant_current_limit_int;
        *cur_dec = session_ch_4.ant_current_limit_dec;        
        break;
    }    
}

void AntennaGetParamsStruct (unsigned char ch, antenna_typedef *ant)
{
    switch (ch)
    {
    case CH1:
        ant->resistance_int = session_ch_1.ant_resistance_int;
        ant->resistance_dec = session_ch_1.ant_resistance_dec;
        ant->inductance_int = session_ch_1.ant_inductance_int;
        ant->inductance_dec = session_ch_1.ant_inductance_dec;
        ant->current_limit_int = session_ch_1.ant_current_limit_int;
        ant->current_limit_dec = session_ch_1.ant_current_limit_dec;
        ant->temp_max_int = session_ch_1.ant_temp_max_int;
        ant->temp_max_dec = session_ch_1.ant_temp_max_dec;        
        break;

    case CH2:
        ant->resistance_int = session_ch_2.ant_resistance_int;
        ant->resistance_dec = session_ch_2.ant_resistance_dec;
        ant->inductance_int = session_ch_2.ant_inductance_int;
        ant->inductance_dec = session_ch_2.ant_inductance_dec;
        ant->current_limit_int = session_ch_2.ant_current_limit_int;
        ant->current_limit_dec = session_ch_2.ant_current_limit_dec;
        ant->temp_max_int = session_ch_2.ant_temp_max_int;
        ant->temp_max_dec = session_ch_2.ant_temp_max_dec;                
        break;

    case CH3:
        ant->resistance_int = session_ch_3.ant_resistance_int;
        ant->resistance_dec = session_ch_3.ant_resistance_dec;
        ant->inductance_int = session_ch_3.ant_inductance_int;
        ant->inductance_dec = session_ch_3.ant_inductance_dec;
        ant->current_limit_int = session_ch_3.ant_current_limit_int;
        ant->current_limit_dec = session_ch_3.ant_current_limit_dec;
        ant->temp_max_int = session_ch_3.ant_temp_max_int;
        ant->temp_max_dec = session_ch_3.ant_temp_max_dec;                
        break;

    case CH4:
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

void AntennaSetParamsStruct (unsigned char ch, antenna_typedef *ant)
{
    switch (ch)
    {
    case CH1:
        session_ch_1.ant_resistance_int = ant->resistance_int;
        session_ch_1.ant_resistance_dec = ant->resistance_dec;
        session_ch_1.ant_inductance_int = ant->inductance_int;
        session_ch_1.ant_inductance_dec = ant->inductance_dec;
        session_ch_1.ant_current_limit_int = ant->current_limit_int;
        session_ch_1.ant_current_limit_dec = ant->current_limit_dec;
        session_ch_1.ant_temp_max_int = ant->temp_max_int;
        session_ch_1.ant_temp_max_dec = ant->temp_max_dec;
        break;

    case CH2:
        session_ch_2.ant_resistance_int = ant->resistance_int;
        session_ch_2.ant_resistance_dec = ant->resistance_dec;
        session_ch_2.ant_inductance_int = ant->inductance_int;
        session_ch_2.ant_inductance_dec = ant->inductance_dec;
        session_ch_2.ant_current_limit_int = ant->current_limit_int;
        session_ch_2.ant_current_limit_dec = ant->current_limit_dec;
        session_ch_2.ant_temp_max_int = ant->temp_max_int;
        session_ch_2.ant_temp_max_dec = ant->temp_max_dec;
        break;

    case CH3:
        session_ch_3.ant_resistance_int = ant->resistance_int;
        session_ch_3.ant_resistance_dec = ant->resistance_dec;
        session_ch_3.ant_inductance_int = ant->inductance_int;
        session_ch_3.ant_inductance_dec = ant->inductance_dec;
        session_ch_3.ant_current_limit_int = ant->current_limit_int;
        session_ch_3.ant_current_limit_dec = ant->current_limit_dec;
        session_ch_3.ant_temp_max_int = ant->temp_max_int;
        session_ch_3.ant_temp_max_dec = ant->temp_max_dec;
        break;

    case CH4:
        session_ch_4.ant_resistance_int = ant->resistance_int;
        session_ch_4.ant_resistance_dec = ant->resistance_dec;
        session_ch_4.ant_inductance_int = ant->inductance_int;
        session_ch_4.ant_inductance_dec = ant->inductance_dec;
        session_ch_4.ant_current_limit_int = ant->current_limit_int;
        session_ch_4.ant_current_limit_dec = ant->current_limit_dec;
        session_ch_4.ant_temp_max_int = ant->temp_max_int;
        session_ch_4.ant_temp_max_dec = ant->temp_max_dec;
        break;
    }        
}

unsigned char AntennaGetConnection (unsigned char ch)
{
    unsigned char conn = 0;

    if (ch == CH1)
        conn = ch1_ant_conn;

    if (ch == CH2)
        conn = ch2_ant_conn;

    if (ch == CH3)
        conn = ch3_ant_conn;

    if (ch == CH4)
        conn = ch4_ant_conn;

    return conn;
    
}

unsigned char AntennaGetCurrentTemp (unsigned char ch)
{
    unsigned char temp = 0;

    if (ch == CH1)
        temp = ch1_ant_current_temp_int;

    if (ch == CH2)
        temp = ch2_ant_current_temp_int;

    if (ch == CH3)
        temp = ch3_ant_current_temp_int;

    if (ch == CH4)
        temp = ch4_ant_current_temp_int;

    return temp;
    
}

unsigned char AntennaVerifyForTreatment (unsigned char ch)
{
    unsigned char resp = FIN_ERROR;

    switch (ch)
    {
    case CH1:
        if (ch1_ant_conn)
            resp = FIN_OK;

        AntennaLock(CH1);        
        break;

    case CH2:
        if (ch2_ant_conn)
            resp = FIN_OK;

        AntennaLock(CH2);        
        break;

    case CH3:
        if (ch3_ant_conn)
            resp = FIN_OK;

        AntennaLock(CH3);        
        break;

    case CH4:
        if (ch4_ant_conn)
            resp = FIN_OK;

        AntennaLock(CH4);                
        break;

    default:
        break;
    }

    return resp;
}

// void Session_Clear_Antenna (session_typedef * ptr_session, unsigned char stage)
// {
// 	//Resistance.
// 	ptr_session->stage_1_resistance_int = 0;
// 	ptr_session->stage_1_resistance_dec = 0;

// 	//Inductance.
// 	ptr_session->stage_1_inductance_int = 0;
// 	ptr_session->stage_1_inductance_dec = 0;

// 	//Current limit.
// 	ptr_session->stage_1_current_limit_int = 0;
// 	ptr_session->stage_1_current_limit_dec = 0;

// }

// //carga el valor de antenna que recibe por la uart en la estructura de session
// //los parametros le llegan por el *ptr_antenna y donde guardarlo en *ptr_session
// void Session_Set_Antenna (session_typedef * ptr_session, unsigned char stage , antenna_typedef * ptr_antenna)
// {
// 	unsigned char a;
// 	unsigned short aa;

// 	//Resistance.
// 	aa = ptr_antenna->resistance_int;
// 	if (aa <= 999)
// 		ptr_session->stage_1_resistance_int = aa;
// 	else
// 		ptr_session->stage_1_resistance_int = 999;

// 	a = ptr_antenna->resistance_dec;
// 	ptr_session->stage_1_resistance_dec = a;

// 	//Inductance.
// 	aa = ptr_antenna->inductance_int;
// 	if (aa <= 999)
// 		ptr_session->stage_1_inductance_int = aa;
// 	else
// 		ptr_session->stage_1_inductance_int = 999;

// 	a = ptr_antenna->inductance_dec;
// 	ptr_session->stage_1_inductance_dec = a;

// 	//Current limit.
// 	a = ptr_antenna->current_limit_int;
// 	ptr_session->stage_1_current_limit_int = a;
// 	a = ptr_antenna->current_limit_dec;
// 	ptr_session->stage_1_current_limit_dec = a;

// 	//Temperature limit.
// 	a = ptr_antenna->temp_max_int;
// 	ptr_session->stage_1_temp_max_int = a;
// 	a = ptr_antenna->temp_max_dec;
// 	ptr_session->stage_1_temp_max_dec = a;

// }

// //Preguntaa la sesion los valores de parametros antenna conectada
// //devuelve info por el puntero *ptr_antenna
// void Session_Get_Antenna (session_typedef * ptr_session, unsigned char stage , antenna_typedef * ptr_antenna)
// {
// 	unsigned char a;
// 	unsigned short aa;

// 	//Resistance.
// 	aa = ptr_session->stage_1_resistance_int;
// 	ptr_antenna->resistance_int = aa;
// 	a = ptr_session->stage_1_resistance_dec;
// 	ptr_antenna->resistance_dec = a;

// 	//Inductance.
// 	aa = ptr_session->stage_1_inductance_int;
// 	ptr_antenna->inductance_int = aa;
// 	a = ptr_session->stage_1_inductance_dec;
// 	ptr_antenna->inductance_dec = a;

// 	//Current limit.
// 	a = ptr_session->stage_1_current_limit_int;
// 	ptr_antenna->current_limit_int = a;
// 	a = ptr_session->stage_1_current_limit_dec;
// 	ptr_antenna->current_limit_dec = a;

// 	//Temperature limit.
// 	a = ptr_session->stage_1_temp_max_int;
// 	ptr_antenna->temp_max_int = a;
// 	a = ptr_session->stage_1_temp_max_dec;
// 	ptr_antenna->temp_max_dec = a;

// }

// unsigned char Session_Channel_1_Verify_Antenna (session_typedef * ptr_session)
// {
// 	switch (Session_Channel_1_Verify_Antenna_state)
// 	{
// 		case SESSION_CHANNEL_1_VERIFY_ANTENNA_INIT:

// 			Session_Clear_Antenna (ptr_session, SESSION_STAGE_1);
// //			//Resistance.
// //			ptr_session->stage_1_resistance_int = 0;
// //			ptr_session->stage_1_resistance_dec = 0;
// //
// //			//Inductance.
// //			ptr_session->stage_1_inductance_int = 0;
// //			ptr_session->stage_1_inductance_dec = 0;
// //
// //			//Current limit.
// //			ptr_session->stage_1_current_limit_int = 0;
// //			ptr_session->stage_1_current_limit_dec = 0;


// 			UART_PC_Send("Getting antenna parameters of channel 1\r\n");
// 			UART_CH1_Send("get_params\r\n");

// 			Session_Channel_1_Verify_Antenna_time = SESSION_CHANNEL_1_VERIFY_ANTENNA_TIME;
// 			Session_Channel_1_Verify_Antenna_state = SESSION_CHANNEL_1_VERIFY_ANTENNA_WAIT_PARAMS;
// 			break;

// 		case SESSION_CHANNEL_1_VERIFY_ANTENNA_WAIT_PARAMS:

// 			if (Session_Channel_1_Verify_Antenna_time == 0)
// 				Session_Channel_1_Verify_Antenna_state = SESSION_CHANNEL_1_VERIFY_ANTENNA_FIN_ERROR;
// 			else
// 			{
// 				if 	(((ptr_session->stage_1_resistance_int != 0) || (ptr_session->stage_1_resistance_dec != 0))
// 					&& ((ptr_session->stage_1_inductance_int != 0) || (ptr_session->stage_1_inductance_dec != 0))
// 					&& ((ptr_session->stage_1_current_limit_int != 0) || (ptr_session->stage_1_current_limit_dec != 0)))
// 				{
// 					Session_Channel_1_Verify_Antenna_state = SESSION_CHANNEL_1_VERIFY_ANTENNA_FIN_OK;
// 				}
// 			}
// 			break;

// 		case SESSION_CHANNEL_1_VERIFY_ANTENNA_FIN_OK:

// 			UART_PC_Send("Antenna detected on CH1\r\n");
// 			Session_Channel_1_Verify_Antenna_state = SESSION_CHANNEL_1_VERIFY_ANTENNA_INIT;
// 			return FIN_OK;
// 			break;

// 		case SESSION_CHANNEL_1_VERIFY_ANTENNA_FIN_ERROR:

// 			Session_Channel_1_Verify_Antenna_state = SESSION_CHANNEL_1_VERIFY_ANTENNA_INIT;
// 			return FIN_ERROR;
// 			break;

// 		default:
// 			Session_Channel_1_Verify_Antenna_state = SESSION_CHANNEL_1_VERIFY_ANTENNA_INIT;
// 			break;
// 	}

// 	return TRABAJANDO;
// }


// unsigned char Session_Channel_2_Verify_Antenna (session_typedef * ptr_session)
// {
// 	switch (Session_Channel_2_Verify_Antenna_state)
// 	{
// 		case SESSION_CHANNEL_2_VERIFY_ANTENNA_INIT:

// 			Session_Clear_Antenna (ptr_session, SESSION_STAGE_1);
// 			//Session_Clear_Antenna (ptr_session, SESSION_STAGE_2);
// 			//Session_Clear_Antenna (ptr_session, SESSION_STAGE_3);

// 			UART_PC_Send("Getting antenna parameters of channel 2\r\n");
// 			UART_CH2_Send("get_params\r\n");

// 			Session_Channel_2_Verify_Antenna_time = SESSION_CHANNEL_2_VERIFY_ANTENNA_TIME;
// 			Session_Channel_2_Verify_Antenna_state = SESSION_CHANNEL_2_VERIFY_ANTENNA_WAIT_PARAMS;
// 			break;

// 		case SESSION_CHANNEL_2_VERIFY_ANTENNA_WAIT_PARAMS:

// 			if (Session_Channel_2_Verify_Antenna_time == 0)
// 				Session_Channel_2_Verify_Antenna_state = SESSION_CHANNEL_2_VERIFY_ANTENNA_FIN_ERROR;
// 			else
// 			{
// 				if 	(((ptr_session->stage_1_resistance_int != 0) || (ptr_session->stage_1_resistance_dec != 0))
// 					&& ((ptr_session->stage_1_inductance_int != 0) || (ptr_session->stage_1_inductance_dec != 0))
// 					&& ((ptr_session->stage_1_current_limit_int != 0) || (ptr_session->stage_1_current_limit_dec != 0)))
// 				{
// 					Session_Channel_2_Verify_Antenna_state = SESSION_CHANNEL_2_VERIFY_ANTENNA_FIN_OK;
// 				}
// 			}
// 			break;

// 		case SESSION_CHANNEL_2_VERIFY_ANTENNA_FIN_OK:

// 			UART_PC_Send("Antenna detected on CH2\r\n");
// 			Session_Channel_2_Verify_Antenna_state = SESSION_CHANNEL_2_VERIFY_ANTENNA_INIT;
// 			return FIN_OK;
// 			break;

// 		case SESSION_CHANNEL_2_VERIFY_ANTENNA_FIN_ERROR:

// 			Session_Channel_2_Verify_Antenna_state = SESSION_CHANNEL_2_VERIFY_ANTENNA_INIT;
// 			return FIN_ERROR;
// 			break;

// 		default:
// 			Session_Channel_2_Verify_Antenna_state = SESSION_CHANNEL_2_VERIFY_ANTENNA_INIT;
// 			break;
// 	}

// 	return TRABAJANDO;
// }

// unsigned char Session_Channel_3_Verify_Antenna (session_typedef * ptr_session)
// {
// 	switch (Session_Channel_3_Verify_Antenna_state)
// 	{
// 		case SESSION_CHANNEL_3_VERIFY_ANTENNA_INIT:

// 			Session_Clear_Antenna (ptr_session, SESSION_STAGE_1);
// 			//Session_Clear_Antenna (ptr_session, SESSION_STAGE_2);
// 			//Session_Clear_Antenna (ptr_session, SESSION_STAGE_3);

// 			UART_PC_Send("Getting antenna parameters of channel 3\r\n");
// 			UART_CH3_Send("get_params\r\n");

// 			Session_Channel_3_Verify_Antenna_time = SESSION_CHANNEL_3_VERIFY_ANTENNA_TIME;
// 			Session_Channel_3_Verify_Antenna_state = SESSION_CHANNEL_3_VERIFY_ANTENNA_WAIT_PARAMS;
// 			break;

// 		case SESSION_CHANNEL_3_VERIFY_ANTENNA_WAIT_PARAMS:

// 			if (Session_Channel_3_Verify_Antenna_time == 0)
// 				Session_Channel_3_Verify_Antenna_state = SESSION_CHANNEL_3_VERIFY_ANTENNA_FIN_ERROR;
// 			else
// 			{
// 				if 	(((ptr_session->stage_1_resistance_int != 0) || (ptr_session->stage_1_resistance_dec != 0))
// 					&& ((ptr_session->stage_1_inductance_int != 0) || (ptr_session->stage_1_inductance_dec != 0))
// 					&& ((ptr_session->stage_1_current_limit_int != 0) || (ptr_session->stage_1_current_limit_dec != 0)))
// 				{
// 					Session_Channel_3_Verify_Antenna_state = SESSION_CHANNEL_3_VERIFY_ANTENNA_FIN_OK;
// 				}
// 			}
// 			break;

// 		case SESSION_CHANNEL_3_VERIFY_ANTENNA_FIN_OK:

// 			UART_PC_Send("Antenna detected on CH3\r\n");
// 			Session_Channel_3_Verify_Antenna_state = SESSION_CHANNEL_3_VERIFY_ANTENNA_INIT;
// 			return FIN_OK;
// 			break;

// 		case SESSION_CHANNEL_3_VERIFY_ANTENNA_FIN_ERROR:

// 			Session_Channel_3_Verify_Antenna_state = SESSION_CHANNEL_3_VERIFY_ANTENNA_INIT;
// 			return FIN_ERROR;
// 			break;

// 		default:
// 			Session_Channel_3_Verify_Antenna_state = SESSION_CHANNEL_3_VERIFY_ANTENNA_INIT;
// 			break;
// 	}

// 	return TRABAJANDO;
// }

// unsigned char Session_Channel_4_Verify_Antenna (session_typedef * ptr_session)
// {
// 	switch (Session_Channel_4_Verify_Antenna_state)
// 	{
// 		case SESSION_CHANNEL_4_VERIFY_ANTENNA_INIT:

// 			Session_Clear_Antenna (ptr_session, SESSION_STAGE_1);
// 			//Session_Clear_Antenna (ptr_session, SESSION_STAGE_2);
// 			//Session_Clear_Antenna (ptr_session, SESSION_STAGE_3);

// 			UART_PC_Send("Getting antenna parameters of channel 4\r\n");
// 			UART_CH4_Send("get_params\r\n");

// 			Session_Channel_4_Verify_Antenna_time = SESSION_CHANNEL_4_VERIFY_ANTENNA_TIME;
// 			Session_Channel_4_Verify_Antenna_state = SESSION_CHANNEL_4_VERIFY_ANTENNA_WAIT_PARAMS;
// 			break;

// 		case SESSION_CHANNEL_4_VERIFY_ANTENNA_WAIT_PARAMS:

// 			if (Session_Channel_4_Verify_Antenna_time == 0)
// 				Session_Channel_4_Verify_Antenna_state = SESSION_CHANNEL_4_VERIFY_ANTENNA_FIN_ERROR;
// 			else
// 			{
// 				if 	(((ptr_session->stage_1_resistance_int != 0) || (ptr_session->stage_1_resistance_dec != 0))
// 					&& ((ptr_session->stage_1_inductance_int != 0) || (ptr_session->stage_1_inductance_dec != 0))
// 					&& ((ptr_session->stage_1_current_limit_int != 0) || (ptr_session->stage_1_current_limit_dec != 0)))
// 				{
// 					Session_Channel_4_Verify_Antenna_state = SESSION_CHANNEL_4_VERIFY_ANTENNA_FIN_OK;
// 				}
// 			}
// 			break;

// 		case SESSION_CHANNEL_4_VERIFY_ANTENNA_FIN_OK:

// 			UART_PC_Send("Antenna detected on CH4\r\n");
// 			Session_Channel_4_Verify_Antenna_state = SESSION_CHANNEL_4_VERIFY_ANTENNA_INIT;
// 			return FIN_OK;
// 			break;

// 		case SESSION_CHANNEL_4_VERIFY_ANTENNA_FIN_ERROR:
// 			Session_Channel_4_Verify_Antenna_state = SESSION_CHANNEL_4_VERIFY_ANTENNA_INIT;
// 			return FIN_ERROR;
// 			break;

// 		default:
// 			Session_Channel_4_Verify_Antenna_state = SESSION_CHANNEL_2_VERIFY_ANTENNA_INIT;
// 			break;
// 	}

// 	return TRABAJANDO;
// }

void Signal_TIM1MS (void)
{

// 	if (channel_1_pause == 0)
// 	{
// 		if (Session_Channel_1_Verify_Antenna_time)
// 			Session_Channel_1_Verify_Antenna_time--;

// 		if (session_channel_1_ask_temp)
// 			session_channel_1_ask_temp--;

// 		if (session_channel_1_answer_temp)
// 			session_channel_1_answer_temp--;
// 	}

// 	if (channel_2_pause == 0)
// 	{

// 		if (Session_Channel_2_Verify_Antenna_time)
// 			Session_Channel_2_Verify_Antenna_time--;

// 		if (session_channel_2_ask_temp)
// 			session_channel_2_ask_temp--;

// 		if (session_channel_2_answer_temp)
// 			session_channel_2_answer_temp--;
// 	}

// 	if (channel_3_pause == 0)
// 	{

// 		if (Session_Channel_3_Verify_Antenna_time)
// 			Session_Channel_3_Verify_Antenna_time--;

// 		if (session_channel_3_ask_temp)
// 			session_channel_3_ask_temp--;

// 		if (session_channel_3_answer_temp)
// 			session_channel_3_answer_temp--;

// 	}

// 	if (channel_4_pause == 0)
// 	{

// 		if (Session_Channel_4_Verify_Antenna_time)
// 			Session_Channel_4_Verify_Antenna_time--;

// 		if (session_channel_4_ask_temp)
// 			session_channel_4_ask_temp--;

// 		if (session_channel_4_answer_temp)
// 			session_channel_4_answer_temp--;
// 	}
}




// 		if (session_channel_1_state >= SESSION_CHANNEL_1_WARMING_UP)
// 		{
// 			if (session_channel_1_ask_temp == 0)
// 			{
// 				UART_CH1_Send("get_temp\r\n");
// 				session_channel_1_ask_temp = SESSION_CHANNEL_ASK_TEMP;
// 			}

// 			if ((temp_actual_channel_1_int != 0) || (temp_actual_channel_1_dec != 0))
// 			{
// 				if ((temp_actual_channel_1_int > session_ch_1.stage_1_temp_max_int)
// 						|| ((temp_actual_channel_1_int == session_ch_1.stage_1_temp_max_int) && (temp_actual_channel_1_dec > session_ch_1.stage_1_temp_max_dec)))
// 				{
// 					session_ch_1.status = 0;
// 					SetBitGlobalErrors (CH1, BIT_ERROR_ANTENNA);
// 					sprintf(&buffSendErr[0], (const char *) "ERROR(0x%03X)\r\n", ERR_CHANNEL_ANTENNA_TMP_OUT_OF_RANGE(1));
// 					UART_PC_Send(&buffSendErr[0]);
// 				}
// 				else
// 					session_channel_1_answer_temp = SESSION_CHANNEL_ANSWER_TEMP;

// 				temp_actual_channel_1_int = 0;
// 				temp_actual_channel_1_dec = 0;
// 			}

// 			if (session_channel_1_answer_temp == 0)
// 			{
// 				session_ch_1.status = 0;
// 				SetBitGlobalErrors (CH1, BIT_ERROR_ANTENNA);
// 				sprintf(&buffSendErr[0], (const char *) "ERROR(0x%03X)\r\n", ERR_CHANNEL_ANTENNA_LOST(1));
// 				UART_PC_Send(&buffSendErr[0]);
// 			}
// 		}

// extern unsigned char temp_actual_channel_1_int;
// extern unsigned char temp_actual_channel_1_dec;
// extern unsigned char temp_actual_channel_2_int;
// extern unsigned char temp_actual_channel_2_dec;
// extern unsigned char temp_actual_channel_3_int;
// extern unsigned char temp_actual_channel_3_dec;
// extern unsigned char temp_actual_channel_4_int;
// extern unsigned char temp_actual_channel_4_dec;

//---- end of file ----//
