//#include "GTK_Header.h"

#include "GTK_Estructura.h"
#include "uart.h"
#include "flash_program.h"
#include "GTK_Signal.h"
#include "GTK_Hard.h"
#include <string.h>
#include <stdio.h>

//stage 1:	warning up.
//stage 2: 	plateau.
//stage 3:	cooling down.

//Channels.
extern session_typedef session_ch_1;
extern session_typedef session_ch_2;
extern session_typedef session_ch_3;
extern session_typedef session_ch_4;

unsigned char Get_Antenna_Name(unsigned char channel, char * p_buff)
{
	unsigned char name_finded = 0;

	switch (channel)
	{
		case CH1:
			if (strlen(session_ch_1.connected_ant_name) > 0)
			{
				strcpy(p_buff, session_ch_1.connected_ant_name);
				name_finded = 1;
			}
			break;

		case CH2:
			if (strlen(session_ch_2.connected_ant_name) > 0)
			{
				strcpy(p_buff, session_ch_2.connected_ant_name);
				name_finded = 1;
			}

			break;

		case CH3:
			if (strlen(session_ch_3.connected_ant_name) > 0)
			{
				strcpy(p_buff, session_ch_3.connected_ant_name);
				name_finded = 1;
			}

			break;

		case CH4:
			if (strlen(session_ch_4.connected_ant_name) > 0)
			{
				strcpy(p_buff, session_ch_4.connected_ant_name);
				name_finded = 1;
			}

			break;

		default:
			break;

	}
	return name_finded;
}

#define EFECTIVO_EN_CODIGO
//#define EFECTIVO_EN_VELOCIDAD

void Save_Antenna_Name(unsigned char channel, char * p_buff)
{
#ifdef EFECTIVO_EN_CODIGO
	char buff[20];

	memset(buff, '\0', sizeof(buff));

	if (strncmp((const char *)"ant0", p_buff, 4) == 0)
		strcpy(buff, (const char *) "Big size Toroid");

	else if (strncmp((const char *)"ant1", p_buff, 4) == 0)
		strcpy(buff, (const char *) "Med size Toroid");

	else if (strncmp((const char *)"ant2", p_buff, 4) == 0)
		strcpy(buff, (const char *) "Small Cylinder");

	else if (strncmp((const char *)"ant3", p_buff, 4) == 0)
		strcpy(buff, (const char *) "Med Cylinder");

	else if (strncmp((const char *)"ant4", p_buff, 4) == 0)
		strcpy(buff, (const char *) "Big Cylinder");

	else if (strncmp((const char *)"ant5", p_buff, 4) == 0)
		strcpy(buff, (const char *) "Small Toroid");

	else if (strncmp((const char *)"ant6", p_buff, 4) == 0)
		strcpy(buff, (const char *) "Old Toroid");

	else if (strncmp((const char *)"ant7", p_buff, 4) == 0)
		strcpy(buff, (const char *) "Pencil Solenoid");

	else if (strncmp((const char *)"ant8", p_buff, 4) == 0)
		strcpy(buff, (const char *) "Pencil Dish");

	//else if (strncmp((const char *)"antA", p_buff, 4) == 0)
	else
		strcpy(buff, (const char *) "Not Finded");

	switch (channel)
	{
		case CH1:
			strcpy(session_ch_1.connected_ant_name, buff);
			break;

		case CH2:
			strcpy(session_ch_2.connected_ant_name, buff);
			break;

		case CH3:
			strcpy(session_ch_3.connected_ant_name, buff);
			break;

		case CH4:
			strcpy(session_ch_4.connected_ant_name, buff);
			break;

		default:
			break;

	}
#endif

#ifdef EFECTIVO_EN_VELOCIDAD

#endif
}

//--- Set functions ---//
void Session_Set_Status (session_typedef * ptr_session, unsigned char stage , unsigned char new_state) //Stage status.
{
	switch(stage)
	{
		case SESSION_STAGE_1:
			ptr_session->stage_1_status = new_state;
			break;

		case SESSION_STAGE_2:
			ptr_session->stage_2_status = new_state;
			break;

		case SESSION_STAGE_3:
			ptr_session->stage_3_status = new_state;
			break;
	}
}

void Session_Set_Special_Function (session_typedef * ptr_session, unsigned char stage , unsigned char value)
{

	switch(stage)
	{
		case SESSION_STAGE_1:
			ptr_session->stage_1_special_function = value;
			break;

		case SESSION_STAGE_2:
			ptr_session->stage_2_special_function = value;
			break;

		case SESSION_STAGE_3:
			ptr_session->stage_3_special_function = value;
			break;
	}
}

void Session_Set_Duration (session_typedef * ptr_session, unsigned char stage , duration_typedef * ptr_duration)
{

	unsigned char a;

	switch(stage)
	{
		case SESSION_STAGE_1:

			//Hours.
			a = ptr_duration->hours;
			ptr_session->stage_1_time_hours = a;

			//Minutes.
			a = ptr_duration->minutes;
			ptr_session->stage_1_time_minutes = a;

			//Seconds.
			a = ptr_duration->seconds;
			ptr_session->stage_1_time_seconds = a;
			break;

		case SESSION_STAGE_2:
			//Hours.
			a = ptr_duration->hours;
			ptr_session->stage_2_time_hours = a;

			//Minutes.
			a = ptr_duration->minutes;
			ptr_session->stage_2_time_minutes = a;

			//Seconds.
			a = ptr_duration->seconds;
			ptr_session->stage_2_time_seconds = a;
			break;

		case SESSION_STAGE_3:
			//Hours.
			a = ptr_duration->hours;
			ptr_session->stage_3_time_hours = a;

			//Minutes.
			a = ptr_duration->minutes;
			ptr_session->stage_3_time_minutes = a;

			//Seconds.
			a = ptr_duration->seconds;
			ptr_session->stage_3_time_seconds = a;
			break;
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

void Session_Set_Antenna (session_typedef * ptr_session, unsigned char stage , antenna_typedef * ptr_antenna)
{

	unsigned char a;

	//Resistance.
	a = ptr_antenna->resistance_int;
	ptr_session->stage_1_resistance_int = a;
	a = ptr_antenna->resistance_dec;
	ptr_session->stage_1_resistance_dec = a;

	//Inductance.
	a = ptr_antenna->inductance_int;
	ptr_session->stage_1_inductance_int = a;
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

void Session_Set_Signal (session_typedef * ptr_session, unsigned char stage, signal_typedef * ptr_signal)
{

	unsigned short a;

	switch(stage)
	{
		case SESSION_STAGE_1:

			//Initial power.
			a = ptr_signal->initial_power;
			ptr_session->stage_1_initial_power = a;

			//Final power.
			a = ptr_signal->final_power;
			ptr_session->stage_1_final_power = a;

			//Rising time.
			a = ptr_signal->rising_time;
			ptr_session->stage_1_rising_time = a;

			//Maintenance time.
			a = ptr_signal->maintenance_time;
			ptr_session->stage_1_maintenance_time = a;

			//Falling time.
			a = ptr_signal->falling_time;
			ptr_session->stage_1_falling_time = a;

			//Low time.
			a = ptr_signal->low_time;
			ptr_session->stage_1_low_time = a;

			//Burst on.
			a = ptr_signal->burst_mode_on;
			ptr_session->stage_1_burst_mode_on = a;

			//Burst off.
			a = ptr_signal->burst_mode_off;
			ptr_session->stage_1_burst_mode_off = a;
			break;

		case SESSION_STAGE_2:

			//Initial power.
			a = ptr_signal->initial_power;
			ptr_session->stage_2_initial_power = a;

			//Final power.
			a = ptr_signal->final_power;
			ptr_session->stage_2_final_power = a;

			//Rising time.
			a = ptr_signal->rising_time;
			ptr_session->stage_2_rising_time = a;

			//Maintenance time.
			a = ptr_signal->maintenance_time;
			ptr_session->stage_2_maintenance_time = a;

			//Falling time.
			a = ptr_signal->falling_time;
			ptr_session->stage_2_falling_time = a;

			//Low time.
			a = ptr_signal->low_time;
			ptr_session->stage_2_low_time = a;

			//Burst on.
			a = ptr_signal->burst_mode_on;
			ptr_session->stage_2_burst_mode_on = a;

			//Burst off.
			a = ptr_signal->burst_mode_off;
			ptr_session->stage_2_burst_mode_off = a;
			break;

		case SESSION_STAGE_3:

			//Initial power.
			a = ptr_signal->initial_power;
			ptr_session->stage_3_initial_power = a;

			//Final power.
			a = ptr_signal->final_power;
			ptr_session->stage_3_final_power = a;

			//Rising time.
			a = ptr_signal->rising_time;
			ptr_session->stage_3_rising_time = a;

			//Maintenance time.
			a = ptr_signal->maintenance_time;
			ptr_session->stage_3_maintenance_time = a;

			//Falling time.
			a = ptr_signal->falling_time;
			ptr_session->stage_3_falling_time = a;

			//Low time.
			a = ptr_signal->low_time;
			ptr_session->stage_3_low_time = a;

			//Burst on.
			a = ptr_signal->burst_mode_on;
			ptr_session->stage_3_burst_mode_on = a;

			//Burst off.
			a = ptr_signal->burst_mode_off;
			ptr_session->stage_3_burst_mode_off = a;
			break;
	}
	//------ Generals -----//
	//sync
	a = ptr_signal->sync_on;
	ptr_session->sync_on = a;

}
//--- end ---//

//--- Get functions ---//
void Session_Get_Status (session_typedef * ptr_session, unsigned char stage , unsigned char *new_state)
{
	switch(stage)
	{
		case SESSION_STAGE_1:
			*new_state = ptr_session->stage_1_status;
			break;

		case SESSION_STAGE_2:
			*new_state = ptr_session->stage_2_status;
			break;

		case SESSION_STAGE_3:
			*new_state = ptr_session->stage_3_status;
			break;
	}
}

void Session_Get_Special_Function (session_typedef * ptr_session, unsigned char stage , unsigned char *value)
{

	switch(stage)
	{
		case SESSION_STAGE_1:
			*value = ptr_session->stage_1_special_function;
			break;

		case SESSION_STAGE_2:
			*value = ptr_session->stage_2_special_function;
			break;

		case SESSION_STAGE_3:
			*value = ptr_session->stage_3_special_function;
			break;
	}
}

void Session_Get_Duration (session_typedef * ptr_session, unsigned char stage , duration_typedef * ptr_duration)
{

	unsigned char a;

	switch(stage)
	{
		case SESSION_STAGE_1:

			//Hours.
			a = ptr_session->stage_1_time_hours;
			ptr_duration->hours = a;

			//Minutes.
			a = ptr_session->stage_1_time_minutes;
			ptr_duration->minutes = a;

			//Seconds.
			a = ptr_session->stage_1_time_seconds;
			ptr_duration->seconds = a;
			break;

		case SESSION_STAGE_2:

			//Hours.
			a = ptr_session->stage_2_time_hours;
			ptr_duration->hours = a;

			//Minutes.
			a = ptr_session->stage_2_time_minutes;
			ptr_duration->minutes = a;

			//Seconds.
			a = ptr_session->stage_2_time_seconds;
			ptr_duration->seconds = a;
			break;

		case SESSION_STAGE_3:

			//Hours.
			a = ptr_session->stage_3_time_hours;
			ptr_duration->hours = a;

			//Minutes.
			a = ptr_session->stage_3_time_minutes;
			ptr_duration->minutes = a;

			//Seconds.
			a = ptr_session->stage_3_time_seconds;
			ptr_duration->seconds = a;
			break;
	}
}

void Session_Get_Antenna (session_typedef * ptr_session, unsigned char stage , antenna_typedef * ptr_antenna)
{

	unsigned char a;

	//Resistance.
	a = ptr_session->stage_1_resistance_int;
	ptr_antenna->resistance_int = a;
	a = ptr_session->stage_1_resistance_dec;
	ptr_antenna->resistance_dec = a;

	//Inductance.
	a = ptr_session->stage_1_inductance_int;
	ptr_antenna->inductance_int = a;
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

void Session_Get_Signal (session_typedef * ptr_session, unsigned char stage, signal_typedef * ptr_signal)
{

	unsigned char a;

	switch(stage)
	{
		case SESSION_STAGE_1:

			//Initial power.
			a = ptr_session->stage_1_initial_power;
			ptr_signal->initial_power = a;

			//Final power.
			a = ptr_session->stage_1_final_power;
			ptr_signal->final_power = a;

			//Rising time.
			a = ptr_session->stage_1_rising_time;
			ptr_signal->rising_time = a;

			//Maintenance time.
			a = ptr_session->stage_1_maintenance_time;
			ptr_signal->maintenance_time = a;

			//Falling time.
			a = ptr_session->stage_1_falling_time;
			ptr_signal->falling_time = a;

			//Low time.
			a = ptr_session->stage_1_low_time;
			ptr_signal->low_time = a;
			break;

		case SESSION_STAGE_2:
			//Initial power.
			a = ptr_session->stage_2_initial_power;
			ptr_signal->initial_power = a;

			//Final power.
			a = ptr_session->stage_2_final_power;
			ptr_signal->final_power = a;

			//Rising time.
			a = ptr_session->stage_2_rising_time;
			ptr_signal->rising_time = a;

			//Maintenance time.
			a = ptr_session->stage_2_maintenance_time;
			ptr_signal->maintenance_time = a;

			//Falling time.
			a = ptr_session->stage_2_falling_time;
			ptr_signal->falling_time = a;

			//Low time.
			a = ptr_session->stage_2_low_time;
			ptr_signal->low_time = a;
			break;

		case SESSION_STAGE_3:
			//Initial power.
			a = ptr_session->stage_3_initial_power;
			ptr_signal->initial_power = a;

			//Final power.
			a = ptr_session->stage_3_final_power;
			ptr_signal->final_power = a;

			//Rising time.
			a = ptr_session->stage_3_rising_time;
			ptr_signal->rising_time = a;

			//Maintenance time.
			a = ptr_session->stage_3_maintenance_time;
			ptr_signal->maintenance_time = a;

			//Falling time.
			a = ptr_session->stage_3_falling_time;
			ptr_signal->falling_time = a;

			//Low time.
			a = ptr_session->stage_3_low_time;
			ptr_signal->low_time = a;
			break;
	}
}
//--- end ---//

//--- Save functions ---//
/*
void Session_Save (session_typedef * ptr_session, unsigned char slot)
{
	unsigned short a;

	session_typedef * ptr_session_save;

	switch(slot)
	{
		case 1:
			ptr_session_save = &session_slot_1;
			break;
		case 2:
			ptr_session_save = &session_slot_2;
			break;
		case 3:
			ptr_session_save = &session_slot_3;
			break;
		case 4:
			ptr_session_save = &session_slot_4;
			break;
		case 5:
			ptr_session_save = &session_slot_5;
			break;
		default:
			ptr_session_save = &session_slot_1;
			break;
	}

	//Stage 1.

	//Status.
	a = ptr_session->stage_1_status;
	ptr_session_save->stage_1_status = a;

	//Special function.
	a = ptr_session->stage_1_special_function;
	ptr_session_save->stage_1_special_function = a;

	//Duration.
	a = ptr_session->stage_1_time_hours;
	ptr_session_save->stage_1_time_hours = a;

	a = ptr_session->stage_1_time_minutes;
	ptr_session_save->stage_1_time_minutes = a;

	a = ptr_session->stage_1_time_seconds;
	ptr_session_save->stage_1_time_seconds = a;

	//Antenna.
	a = ptr_session->stage_1_resistance_int;
	ptr_session_save->stage_1_resistance_int = a;

	a = ptr_session->stage_1_resistance_dec;
	ptr_session_save->stage_1_resistance_dec = a;

	a = ptr_session->stage_1_inductance_int;
	ptr_session_save->stage_1_inductance_int = a;

	a = ptr_session->stage_1_inductance_dec;
	ptr_session_save->stage_1_inductance_dec = a;

	a = ptr_session->stage_1_current_limit_int;
	ptr_session_save->stage_1_current_limit_int = a;

	a = ptr_session->stage_1_current_limit_dec;
	ptr_session_save->stage_1_current_limit_dec = a;

	//Signal.
	a = ptr_session->stage_1_initial_power;
	ptr_session_save->stage_1_initial_power = a;

	a = ptr_session->stage_1_final_power;
	ptr_session_save->stage_1_final_power = a;

	a = ptr_session->stage_1_temp_max_int;
	ptr_session_save->stage_1_temp_max_int = a;

	a = ptr_session->stage_1_temp_max_dec;
	ptr_session_save->stage_1_temp_max_dec = a;

	a = ptr_session->stage_1_rising_time;
	ptr_session_save->stage_1_rising_time = a;

	a = ptr_session->stage_1_maintenance_time;
	ptr_session_save->stage_1_maintenance_time = a;

	a = ptr_session->stage_1_falling_time;
	ptr_session_save->stage_1_falling_time = a;

	a = ptr_session->stage_1_low_time;
	ptr_session_save->stage_1_low_time = a;

	//Burst on.
	a = ptr_session->stage_1_burst_mode_on;
	ptr_session_save->stage_1_burst_mode_on = a;

	//Burst off.
	a = ptr_session->stage_1_burst_mode_off;
	ptr_session_save->stage_1_burst_mode_off = a;
	//Stage 2.

	//Status.
	a = ptr_session->stage_2_status;
	ptr_session_save->stage_2_status = a;

	//Special function.
	a = ptr_session->stage_2_special_function;
	ptr_session_save->stage_2_special_function = a;

	//Duration.
	a = ptr_session->stage_2_time_hours;
	ptr_session_save->stage_2_time_hours = a;

	a = ptr_session->stage_2_time_minutes;
	ptr_session_save->stage_2_time_minutes = a;

	a = ptr_session->stage_2_time_seconds;
	ptr_session_save->stage_2_time_seconds = a;

	//Antenna.

	a = ptr_session->stage_2_resistance_int;
	ptr_session_save->stage_2_resistance_int = a;

	a = ptr_session->stage_2_resistance_dec;
	ptr_session_save->stage_2_resistance_dec = a;

	a = ptr_session->stage_2_inductance_int;
	ptr_session_save->stage_2_inductance_int = a;

	a = ptr_session->stage_2_inductance_dec;
	ptr_session_save->stage_2_inductance_dec = a;

	a = ptr_session->stage_2_current_limit_int;
	ptr_session_save->stage_2_current_limit_int = a;

	a = ptr_session->stage_2_current_limit_dec;
	ptr_session_save->stage_2_current_limit_dec = a;


	//Signal.
	a = ptr_session->stage_2_initial_power;
	ptr_session_save->stage_2_initial_power = a;

	a = ptr_session->stage_2_final_power;
	ptr_session_save->stage_2_final_power = a;

	a = ptr_session->stage_2_temp_max_int;
	ptr_session_save->stage_2_temp_max_int = a;

	a = ptr_session->stage_2_temp_max_dec;
	ptr_session_save->stage_2_temp_max_dec = a;

	a = ptr_session->stage_2_rising_time;
	ptr_session_save->stage_2_rising_time = a;

	a = ptr_session->stage_2_maintenance_time;
	ptr_session_save->stage_2_maintenance_time = a;

	a = ptr_session->stage_2_falling_time;
	ptr_session_save->stage_2_falling_time = a;

	a = ptr_session->stage_2_low_time;
	ptr_session_save->stage_2_low_time = a;

	a = ptr_session->stage_2_burst_mode_on;
	ptr_session_save->stage_2_burst_mode_on = a;

	a = ptr_session->stage_2_burst_mode_off;
	ptr_session_save->stage_2_burst_mode_off = a;
	//Stage 3.

	//Status.
	a = ptr_session->stage_3_status;
	ptr_session_save->stage_3_status = a;

	//Special function.
	a = ptr_session->stage_3_special_function;
	ptr_session_save->stage_3_special_function = a;

	//Duration.
	a = ptr_session->stage_3_time_hours;
	ptr_session_save->stage_3_time_hours = a;

	a = ptr_session->stage_3_time_minutes;
	ptr_session_save->stage_3_time_minutes = a;

	a = ptr_session->stage_3_time_seconds;
	ptr_session_save->stage_3_time_seconds = a;

	//Antenna.

	a = ptr_session->stage_3_resistance_int;
	ptr_session_save->stage_3_resistance_int = a;

	a = ptr_session->stage_3_resistance_dec;
	ptr_session_save->stage_3_resistance_dec = a;

	a = ptr_session->stage_3_inductance_int;
	ptr_session_save->stage_3_inductance_int = a;

	a = ptr_session->stage_3_inductance_dec;
	ptr_session_save->stage_3_inductance_dec = a;

	a = ptr_session->stage_3_current_limit_int;
	ptr_session_save->stage_3_current_limit_int = a;

	a = ptr_session->stage_3_current_limit_dec;
	ptr_session_save->stage_3_current_limit_dec = a;


	//Signal.
	a = ptr_session->stage_3_initial_power;
	ptr_session_save->stage_3_initial_power = a;

	a = ptr_session->stage_3_final_power;
	ptr_session_save->stage_3_final_power = a;

	a = ptr_session->stage_3_temp_max_int;
	ptr_session_save->stage_3_temp_max_int = a;

	a = ptr_session->stage_3_temp_max_dec;
	ptr_session_save->stage_3_temp_max_dec = a;


	a = ptr_session->stage_3_rising_time;
	ptr_session_save->stage_3_rising_time = a;

	a = ptr_session->stage_3_maintenance_time;
	ptr_session_save->stage_3_maintenance_time = a;

	a = ptr_session->stage_3_falling_time;
	ptr_session_save->stage_3_falling_time = a;

	a = ptr_session->stage_3_low_time;
	ptr_session_save->stage_3_low_time = a;

	a = ptr_session->stage_3_burst_mode_on;
	ptr_session_save->stage_3_burst_mode_on = a;

	a = ptr_session->stage_3_burst_mode_off;
	ptr_session_save->stage_3_burst_mode_off = a;
}
*/

#define delay_session_send 150
void SessionSend(session_typedef * ptr_session)
{
	char buffChar[64];

	//Status off channel.
	sprintf(&buffChar[0],"status,%d\r\n", ptr_session->status);
	UART_PC_Send(&buffChar[0]);
	//Wait_ms(delay_session_send);

	//Duration.
	sprintf(&buffChar[0],"duration,%02d,%02d,%02d,1\r\n", ptr_session->stage_1_time_hours,ptr_session->stage_1_time_minutes, ptr_session->stage_1_time_seconds);
	UART_PC_Send(&buffChar[0]);
	//Wait_ms(delay_session_send);
	sprintf(&buffChar[0],"duration,%02d,%02d,%02d,2\r\n", ptr_session->stage_2_time_hours,ptr_session->stage_2_time_minutes, ptr_session->stage_2_time_seconds);
	UART_PC_Send(&buffChar[0]);
	//Wait_ms(delay_session_send);
	sprintf(&buffChar[0],"duration,%02d,%02d,%02d,3\r\n", ptr_session->stage_3_time_hours,ptr_session->stage_3_time_minutes, ptr_session->stage_3_time_seconds);
	UART_PC_Send(&buffChar[0]);
	//Wait_ms(delay_session_send);

	//Antenna.
/*	sprintf(&buffChar[0],"antenna,%03d,%02d,%03d,%02d,%02d,%02d,1\r\n", ptr_session->stage_1_resistance_int, ptr_session->stage_1_resistance_dec, ptr_session->stage_1_inductance_int, ptr_session->stage_1_inductance_dec, ptr_session->stage_1_current_limit_int, ptr_session->stage_1_current_limit_dec);
	UART3Send(&buffChar[0]);
	Wait_ms(delay_session_send);
	sprintf(&buffChar[0],"antenna,%03d,%02d,%03d,%02d,%02d,%02d,2\r\n", ptr_session->stage_2_resistance_int, ptr_session->stage_2_resistance_dec, ptr_session->stage_2_inductance_int, ptr_session->stage_2_inductance_dec, ptr_session->stage_2_current_limit_int, ptr_session->stage_2_current_limit_dec);
	UART3Send(&buffChar[0]);
	Wait_ms(delay_session_send);
	sprintf(&buffChar[0],"antenna,%03d,%02d,%03d,%02d,%02d,%02d,3\r\n", ptr_session->stage_3_resistance_int, ptr_session->stage_3_resistance_dec, ptr_session->stage_3_inductance_int, ptr_session->stage_3_inductance_dec, ptr_session->stage_3_current_limit_int, ptr_session->stage_3_current_limit_dec);
	UART3Send(&buffChar[0]);
	Wait_ms(delay_session_send);
*/
	//Signal.
	//signal,050,050,0000,0010,0030,0010,0030,0000,0000,3
	sprintf(&buffChar[0],"signal,%03d,%03d,000%d,%04d,%04d,%04d,%04d,%04d,%04d,1\r\n", ptr_session->stage_1_initial_power, ptr_session->stage_1_final_power, ptr_session->sync_on, ptr_session->stage_1_rising_time, ptr_session->stage_1_maintenance_time, ptr_session->stage_1_falling_time, ptr_session->stage_1_low_time, ptr_session->stage_1_burst_mode_on, ptr_session->stage_1_burst_mode_off);
	UART_PC_Send(&buffChar[0]);
	//Wait_ms(delay_session_send);
	sprintf(&buffChar[0],"signal,%03d,%03d,000%d,%04d,%04d,%04d,%04d,%04d,%04d,2\r\n", ptr_session->stage_2_initial_power, ptr_session->stage_2_final_power, ptr_session->sync_on, ptr_session->stage_2_rising_time, ptr_session->stage_2_maintenance_time, ptr_session->stage_2_falling_time, ptr_session->stage_2_low_time, ptr_session->stage_2_burst_mode_on, ptr_session->stage_2_burst_mode_off);
	UART_PC_Send(&buffChar[0]);
	//Wait_ms(delay_session_send);
	sprintf(&buffChar[0],"signal,%03d,%03d,000%d,%04d,%04d,%04d,%04d,%04d,%04d,3\r\n", ptr_session->stage_3_initial_power, ptr_session->stage_3_final_power, ptr_session->sync_on, ptr_session->stage_3_rising_time, ptr_session->stage_3_maintenance_time, ptr_session->stage_3_falling_time, ptr_session->stage_3_low_time, ptr_session->stage_3_burst_mode_on, ptr_session->stage_3_burst_mode_off);
	UART_PC_Send(&buffChar[0]);
	//Wait_ms(delay_session_send);

	//state of stage.
	sprintf(&buffChar[0],"state_of_stage,%01d,1\r\n", ptr_session->stage_1_status);
	UART_PC_Send(&buffChar[0]);
	//Wait_ms(delay_session_send);
	sprintf(&buffChar[0],"state_of_stage,%01d,2\r\n", ptr_session->stage_2_status);
	UART_PC_Send(&buffChar[0]);
	//Wait_ms(delay_session_send);
	sprintf(&buffChar[0],"state_of_stage,%01d,3\r\n", ptr_session->stage_3_status);
	UART_PC_Send(&buffChar[0]);
	//Wait_ms(delay_session_send);
}

void Channel_Load (session_typedef * ptr_session, unsigned char channel)
{
	unsigned short a;
	session_typedef * ptr_session_channel;

	switch (channel)
	{
		case 1:
			ptr_session_channel = &session_ch_1;
			break;

		case 2:
			ptr_session_channel = &session_ch_2;
			break;

		case 3:
			ptr_session_channel = &session_ch_3;
			break;

		case 4:
			ptr_session_channel = &session_ch_4;
			break;
	}

	//Stage 1.
	//Status.
	a = ptr_session_channel->stage_1_status;
	ptr_session->stage_1_status = a;

	//Special function.
	a = ptr_session_channel->stage_1_special_function;
	ptr_session->stage_1_special_function = a;

	//Duration.
	a = ptr_session_channel->stage_1_time_hours;
	ptr_session->stage_1_time_hours = a;

	a = ptr_session_channel->stage_1_time_minutes;
	ptr_session->stage_1_time_minutes = a;

	a = ptr_session_channel->stage_1_time_seconds;
	ptr_session->stage_1_time_seconds = a;

	//Antenna.
	a = ptr_session_channel->stage_1_resistance_int;
	ptr_session->stage_1_resistance_int = a;

	a = ptr_session_channel->stage_1_resistance_dec;
	ptr_session->stage_1_resistance_dec = a;

	a = ptr_session_channel->stage_1_inductance_int;
	ptr_session->stage_1_inductance_int = a;

	a = ptr_session_channel->stage_1_inductance_dec;
	ptr_session->stage_1_inductance_dec = a;

	a = ptr_session_channel->stage_1_current_limit_int;
	ptr_session->stage_1_current_limit_int = a;

	a = ptr_session_channel->stage_1_current_limit_dec;
	ptr_session->stage_1_current_limit_dec = a;

	//Signal.
	a = ptr_session_channel->stage_1_initial_power;
	ptr_session->stage_1_initial_power = a;

	a = ptr_session_channel->stage_1_final_power;
	ptr_session->stage_1_final_power = a;

	a = ptr_session_channel->stage_1_temp_max_int;
	ptr_session->stage_1_temp_max_int = a;

	a = ptr_session_channel->stage_1_temp_max_dec;
	ptr_session->stage_1_temp_max_dec = a;

	a = ptr_session_channel->stage_1_rising_time;
	ptr_session->stage_1_rising_time = a;

	a = ptr_session_channel->stage_1_maintenance_time;
	ptr_session->stage_1_maintenance_time = a;

	a = ptr_session_channel->stage_1_falling_time;
	ptr_session->stage_1_falling_time = a;

	a = ptr_session_channel->stage_1_low_time;
	ptr_session->stage_1_low_time = a;

	a = ptr_session_channel->stage_1_burst_mode_on;
	ptr_session->stage_1_burst_mode_on = a;

	a = ptr_session_channel->stage_1_burst_mode_off;
	ptr_session->stage_1_burst_mode_off = a;

	//Stage 2.
	//Status.
	a = ptr_session_channel->stage_2_status;
	ptr_session->stage_2_status = a;

	//Special function.
	a = ptr_session_channel->stage_2_special_function;
	ptr_session->stage_2_special_function = a;

	//Duration.
	a = ptr_session_channel->stage_2_time_hours;
	ptr_session->stage_2_time_hours = a;

	a = ptr_session_channel->stage_2_time_minutes;
	ptr_session->stage_2_time_minutes = a;

	a = ptr_session_channel->stage_2_time_seconds;
	ptr_session->stage_2_time_seconds = a;

	//Antenna.
	/*
	a = ptr_session_channel->stage_2_resistance_int;
	ptr_session->stage_2_resistance_int = a;

	a = ptr_session_channel->stage_2_resistance_dec;
	ptr_session->stage_2_resistance_dec = a;

	a = ptr_session_channel->stage_2_inductance_int;
	ptr_session->stage_2_inductance_int = a;

	a = ptr_session_channel->stage_2_inductance_dec;
	ptr_session->stage_2_inductance_dec = a;

	a = ptr_session_channel->stage_2_current_limit_int;
	ptr_session->stage_2_current_limit_int = a;

	a = ptr_session_channel->stage_2_current_limit_dec;
	ptr_session->stage_2_current_limit_dec = a;
	*/

	//Signal.
	a = ptr_session_channel->stage_2_initial_power;
	ptr_session->stage_2_initial_power = a;

	a = ptr_session_channel->stage_2_final_power;
	ptr_session->stage_2_final_power = a;

	a = ptr_session_channel->stage_2_temp_max_int;
	ptr_session->stage_1_temp_max_int = a;

	a = ptr_session_channel->stage_2_temp_max_dec;
	ptr_session->stage_1_temp_max_dec = a;

	a = ptr_session_channel->stage_2_rising_time;
	ptr_session->stage_2_rising_time = a;

	a = ptr_session_channel->stage_2_maintenance_time;
	ptr_session->stage_2_maintenance_time = a;

	a = ptr_session_channel->stage_2_falling_time;
	ptr_session->stage_2_falling_time = a;

	a = ptr_session_channel->stage_2_low_time;
	ptr_session->stage_2_low_time = a;

	a = ptr_session_channel->stage_2_burst_mode_on;
	ptr_session->stage_2_burst_mode_on = a;

	a = ptr_session_channel->stage_2_burst_mode_off;
	ptr_session->stage_2_burst_mode_off = a;

	//Stage 3.
	//Status.
	a = ptr_session_channel->stage_3_status;
	ptr_session->stage_3_status = a;

	//Special function.
	a = ptr_session_channel->stage_3_special_function;
	ptr_session->stage_3_special_function = a;

	//Duration.
	a = ptr_session_channel->stage_3_time_hours;
	ptr_session->stage_3_time_hours = a;

	a = ptr_session_channel->stage_3_time_minutes;
	ptr_session->stage_3_time_minutes = a;

	a = ptr_session_channel->stage_3_time_seconds;
	ptr_session->stage_3_time_seconds = a;

	//Antenna.
	/*
	a = ptr_session_channel->stage_3_resistance_int;
	ptr_session->stage_3_resistance_int = a;

	a = ptr_session_channel->stage_3_resistance_dec;
	ptr_session->stage_3_resistance_dec = a;

	a = ptr_session_channel->stage_3_inductance_int;
	ptr_session->stage_3_inductance_int = a;

	a = ptr_session_channel->stage_3_inductance_dec;
	ptr_session->stage_3_inductance_dec = a;

	a = ptr_session_channel->stage_3_current_limit_int;
	ptr_session->stage_3_current_limit_int = a;

	a = ptr_session_channel->stage_3_current_limit_dec;
	ptr_session->stage_3_current_limit_dec = a;
	*/

	//Signal.
	a = ptr_session_channel->stage_3_initial_power;
	ptr_session->stage_3_initial_power = a;

	a = ptr_session_channel->stage_3_final_power;
	ptr_session->stage_3_final_power = a;

	a = ptr_session_channel->stage_3_temp_max_int;
	ptr_session->stage_1_temp_max_int = a;

	a = ptr_session_channel->stage_3_temp_max_dec;
	ptr_session->stage_1_temp_max_dec = a;

	a = ptr_session_channel->stage_3_rising_time;
	ptr_session->stage_3_rising_time = a;

	a = ptr_session_channel->stage_3_maintenance_time;
	ptr_session->stage_3_maintenance_time = a;

	a = ptr_session_channel->stage_3_falling_time;
	ptr_session->stage_3_falling_time = a;

	a = ptr_session_channel->stage_3_low_time;
	ptr_session->stage_3_low_time = a;

	a = ptr_session_channel->stage_3_burst_mode_on;
	ptr_session->stage_3_burst_mode_on = a;

	a = ptr_session_channel->stage_3_burst_mode_off;
	ptr_session->stage_3_burst_mode_off = a;

	UART3Send("LOAD OK\r\n");


}

void Session_Load (session_typedef * ptr_session, unsigned char slot, unsigned char channel)
{
	unsigned short a;

	session_typedef ptr_session_slot;
	session_typedef * ptr_session_channel;

	a = readDataFromFlash(&ptr_session_slot, slot);

	if (a == FIN_ERROR)
	{
		UART3Send("LOAD ERROR\r\n");
	}
	else if (a == FIN_OK)
	{
		switch (channel)
		{
			case 0:
				ptr_session_channel = ptr_session;
				break;
			case 1:
				ptr_session_channel = &session_ch_1;
				break;

			case 2:
				ptr_session_channel = &session_ch_2;
				break;

			case 3:
				ptr_session_channel = &session_ch_3;
				break;

			case 4:
				ptr_session_channel = &session_ch_4;
				break;
		}

		//Stage 1.
		//Status.
		a = ptr_session_slot.stage_1_status;
		ptr_session_channel->stage_1_status = a;

		//Special function.
		a = ptr_session_slot.stage_1_special_function;
		ptr_session_channel->stage_1_special_function = a;

		//Duration.
		a = ptr_session_slot.stage_1_time_hours;
		ptr_session_channel->stage_1_time_hours = a;

		a = ptr_session_slot.stage_1_time_minutes;
		ptr_session_channel->stage_1_time_minutes = a;

		a = ptr_session_slot.stage_1_time_seconds;
		ptr_session_channel->stage_1_time_seconds = a;

		//Antenna.
		a = ptr_session_slot.stage_1_resistance_int;
		ptr_session_channel->stage_1_resistance_int = a;

		a = ptr_session_slot.stage_1_resistance_dec;
		ptr_session_channel->stage_1_resistance_dec = a;

		a = ptr_session_slot.stage_1_inductance_int;
		ptr_session_channel->stage_1_inductance_int = a;

		a = ptr_session_slot.stage_1_inductance_dec;
		ptr_session_channel->stage_1_inductance_dec = a;

		a = ptr_session_slot.stage_1_current_limit_int;
		ptr_session_channel->stage_1_current_limit_int = a;

		a = ptr_session_slot.stage_1_current_limit_dec;
		ptr_session_channel->stage_1_current_limit_dec = a;

		//Signal.
		a = ptr_session_slot.stage_1_initial_power;
		ptr_session_channel->stage_1_initial_power = a;

		a = ptr_session_slot.stage_1_final_power;
		ptr_session_channel->stage_1_final_power = a;

		a = ptr_session_slot.stage_1_temp_max_int;
		ptr_session_channel->stage_1_temp_max_int = a;

		a = ptr_session_slot.stage_1_temp_max_dec;
		ptr_session_channel->stage_1_temp_max_dec = a;

		a = ptr_session_slot.stage_1_rising_time;
		ptr_session_channel->stage_1_rising_time = a;

		a = ptr_session_slot.stage_1_maintenance_time;
		ptr_session_channel->stage_1_maintenance_time = a;

		a = ptr_session_slot.stage_1_falling_time;
		ptr_session_channel->stage_1_falling_time = a;

		a = ptr_session_slot.stage_1_low_time;
		ptr_session_channel->stage_1_low_time = a;

		a = ptr_session_slot.stage_1_burst_mode_on;
		ptr_session_channel->stage_1_burst_mode_on = a;

		a = ptr_session_slot.stage_1_burst_mode_off;
		ptr_session_channel->stage_1_burst_mode_off = a;

		//Stage 2.
		//Status.
		a = ptr_session_slot.stage_2_status;
		ptr_session_channel->stage_2_status = a;

		//Special function.
		a = ptr_session_slot.stage_2_special_function;
		ptr_session_channel->stage_2_special_function = a;

		//Duration.
		a = ptr_session_slot.stage_2_time_hours;
		ptr_session_channel->stage_2_time_hours = a;

		a = ptr_session_slot.stage_2_time_minutes;
		ptr_session_channel->stage_2_time_minutes = a;

		a = ptr_session_slot.stage_2_time_seconds;
		ptr_session_channel->stage_2_time_seconds = a;

		//Antenna.
		/*
		a = ptr_session_slot.stage_2_resistance_int;
		ptr_session_channel->stage_2_resistance_int = a;

		a = ptr_session_slot.stage_2_resistance_dec;
		ptr_session_channel->stage_2_resistance_dec = a;

		a = ptr_session_slot.stage_2_inductance_int;
		ptr_session_channel->stage_2_inductance_int = a;

		a = ptr_session_slot.stage_2_inductance_dec;
		ptr_session_channel->stage_2_inductance_dec = a;

		a = ptr_session_slot.stage_2_current_limit_int;
		ptr_session_channel->stage_2_current_limit_int = a;

		a = ptr_session_slot.stage_2_current_limit_dec;
		ptr_session_channel->stage_2_current_limit_dec = a;
		*/

		//Signal.
		a = ptr_session_slot.stage_2_initial_power;
		ptr_session_channel->stage_2_initial_power = a;

		a = ptr_session_slot.stage_2_final_power;
		ptr_session_channel->stage_2_final_power = a;

		a = ptr_session_slot.stage_2_temp_max_int;
		ptr_session_channel->stage_2_temp_max_int = a;

		a = ptr_session_slot.stage_2_temp_max_dec;
		ptr_session_channel->stage_2_temp_max_dec = a;

		a = ptr_session_slot.stage_2_rising_time;
		ptr_session_channel->stage_2_rising_time = a;

		a = ptr_session_slot.stage_2_maintenance_time;
		ptr_session_channel->stage_2_maintenance_time = a;

		a = ptr_session_slot.stage_2_falling_time;
		ptr_session_channel->stage_2_falling_time = a;

		a = ptr_session_slot.stage_2_low_time;
		ptr_session_channel->stage_2_low_time = a;

		a = ptr_session_slot.stage_2_burst_mode_on;
		ptr_session_channel->stage_2_burst_mode_on = a;

		a = ptr_session_slot.stage_2_burst_mode_off;
		ptr_session_channel->stage_2_burst_mode_off = a;

		//Stage 3.
		//Status.
		a = ptr_session_slot.stage_3_status;
		ptr_session_channel->stage_3_status = a;

		//Special function.
		a = ptr_session_slot.stage_3_special_function;
		ptr_session_channel->stage_3_special_function = a;

		//Duration.
		a = ptr_session_slot.stage_3_time_hours;
		ptr_session_channel->stage_3_time_hours = a;

		a = ptr_session_slot.stage_3_time_minutes;
		ptr_session_channel->stage_3_time_minutes = a;

		a = ptr_session_slot.stage_3_time_seconds;
		ptr_session_channel->stage_3_time_seconds = a;

		//Antenna.
		/*
		a = ptr_session_slot.stage_3_resistance_int;
		ptr_session_channel->stage_3_resistance_int = a;

		a = ptr_session_slot.stage_3_resistance_dec;
		ptr_session_channel->stage_3_resistance_dec = a;

		a = ptr_session_slot.stage_3_inductance_int;
		ptr_session_channel->stage_3_inductance_int = a;

		a = ptr_session_slot.stage_3_inductance_dec;
		ptr_session_channel->stage_3_inductance_dec = a;

		a = ptr_session_slot.stage_3_current_limit_int;
		ptr_session_channel->stage_3_current_limit_int = a;

		a = ptr_session_slot.stage_3_current_limit_dec;
		ptr_session_channel->stage_3_current_limit_dec = a;
		*/

		//Signal.
		a = ptr_session_slot.stage_3_initial_power;
		ptr_session_channel->stage_3_initial_power = a;

		a = ptr_session_slot.stage_3_final_power;
		ptr_session_channel->stage_3_final_power = a;

		a = ptr_session_slot.stage_3_temp_max_int;
		ptr_session_channel->stage_3_temp_max_int = a;

		a = ptr_session_slot.stage_3_temp_max_dec;
		ptr_session_channel->stage_3_temp_max_dec = a;

		a = ptr_session_slot.stage_3_rising_time;
		ptr_session_channel->stage_3_rising_time = a;

		a = ptr_session_slot.stage_3_maintenance_time;
		ptr_session_channel->stage_3_maintenance_time = a;

		a = ptr_session_slot.stage_3_falling_time;
		ptr_session_channel->stage_3_falling_time = a;

		a = ptr_session_slot.stage_3_low_time;
		ptr_session_channel->stage_3_low_time = a;

		a = ptr_session_slot.stage_3_burst_mode_on;
		ptr_session_channel->stage_3_burst_mode_on = a;

		a = ptr_session_slot.stage_3_burst_mode_off;
		ptr_session_channel->stage_3_burst_mode_off = a;

		//General Parameters
//		strcpy(&(ptr_session_channel->connected_ant_name), &ptr_session_slot.connected_ant_name);
		a = ptr_session_slot.sync_on;
		ptr_session_channel->sync_on = a;

		UART3Send("LOAD OK\r\n");
	}
}

//--- end ---//
