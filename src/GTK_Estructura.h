#ifndef ESTRUCTURA_H_
#define ESTRUCTURA_H_

//---- Nuevas estructuras ---//
//--- Sesion ---//
typedef struct
{

	//unsigned char channel; //Number of channel used by the antenna.

	//--- --- --- --- --- Stage 1 --- --- --- --- ---//
	//--- General ---//
	unsigned char stage_1_status; //Enable/Disable.
	unsigned char stage_1_special_function;

	//--- Duration ---//
	unsigned char stage_1_time_hours;
	unsigned char stage_1_time_minutes;
	unsigned char stage_1_time_seconds;

	//--- Antenna parameters ---//
	unsigned char stage_1_resistance_int;
	unsigned char stage_1_resistance_dec;
	unsigned char stage_1_inductance_int;
	unsigned char stage_1_inductance_dec;
	unsigned char stage_1_current_limit_int;
	unsigned char stage_1_current_limit_dec;

	//--- Signal ---//
	unsigned char stage_1_initial_power;
	unsigned char stage_1_final_power;
	unsigned char stage_1_temp_max_int;
	unsigned char stage_1_temp_max_dec;

	unsigned int stage_1_time_per_step;

	unsigned short stage_1_rising_time;
	unsigned short stage_1_maintenance_time;
	unsigned short stage_1_falling_time;
	unsigned short stage_1_low_time;

	unsigned short stage_1_burst_mode_on;
	unsigned short stage_1_burst_mode_off;

	//--- --- --- --- --- Stage 2 --- --- --- --- ---//
	//--- General ---//
	unsigned char stage_2_status; //Enable/Disable.
	unsigned char stage_2_special_function;

	//--- Duration ---//
	unsigned char stage_2_time_hours;
	unsigned char stage_2_time_minutes;
	unsigned char stage_2_time_seconds;

	//--- Signal ---//
	unsigned char stage_2_initial_power;
	unsigned char stage_2_final_power;
	//unsigned short stage_2_step_number;
	unsigned char stage_2_temp_max_int;
	unsigned char stage_2_temp_max_dec;

	unsigned int stage_2_time_per_step;

	unsigned short stage_2_rising_time;
	unsigned short stage_2_maintenance_time;
	unsigned short stage_2_falling_time;
	unsigned short stage_2_low_time;

	unsigned short stage_2_burst_mode_on;
	unsigned short stage_2_burst_mode_off;

	//--- --- --- --- --- Stage 3 --- --- --- --- ---//
	//--- General ---//
	unsigned char stage_3_status; //Enable/Disable.
	unsigned char stage_3_special_function;


	//--- Duration ---//
	unsigned char stage_3_time_hours;
	unsigned char stage_3_time_minutes;
	unsigned char stage_3_time_seconds;

	//--- Signal ---//
	unsigned char stage_3_initial_power;
	unsigned char stage_3_final_power;
	//unsigned short stage_3_step_number;
	unsigned char stage_3_temp_max_int;
	unsigned char stage_3_temp_max_dec;

	unsigned int stage_3_time_per_step;

	unsigned short stage_3_rising_time;
	unsigned short stage_3_maintenance_time;
	unsigned short stage_3_falling_time;
	unsigned short stage_3_low_time;

	unsigned short stage_3_burst_mode_on;
	unsigned short stage_3_burst_mode_off;

	//--- --- --- --- --- General Parameters --- --- --- --- ---//
	unsigned char status;
	char connected_ant_name [17];
	unsigned char sync_on;
	unsigned short peak_current_limit;


} session_typedef;

typedef struct {

	unsigned char hours;
	unsigned char minutes;
	unsigned char seconds;

} duration_typedef;

typedef struct {

	unsigned char initial_power;
	unsigned char final_power;
	unsigned char sync_on;
	unsigned short step_number;

	unsigned short rising_time;
	unsigned short maintenance_time;
	unsigned short falling_time;
	unsigned short low_time;

	unsigned short burst_mode_on;
	unsigned short burst_mode_off;
} signal_typedef;

typedef struct {

	unsigned char resistance_int;
	unsigned char resistance_dec;
	unsigned char inductance_int;
	unsigned char inductance_dec;
	unsigned char current_limit_int;
	unsigned char current_limit_dec;
	unsigned char temp_max_int;
	unsigned char temp_max_dec;

} antenna_typedef;
//--- Fin de nuevas estructuras ---//

enum stages
{
	SESSION_STAGE_1 = 1,
	SESSION_STAGE_2,
	SESSION_STAGE_3
};

enum channels
{
	SESSION_CHANNEL_1 = 1,
	SESSION_CHANNEL_2,
	SESSION_CHANNEL_3,
	SESSION_CHANNEL_4
};

//--- warning up/cooling dawn---//
typedef struct {

	//Rising.
	unsigned short 	rising_pwm_200_initial;
	unsigned short 	rising_pwm_200_final;
	unsigned short 	rising_pwm_40_initial;
	unsigned short 	rising_pwm_40_final;
	unsigned short 	rising_pwm_n_initial;
	unsigned short 	rising_pwm_n_final;
	unsigned short 	rising_step_number;

	//Maintenance.
	unsigned short 	maintenance_pwm_200;
	unsigned short 	maintenance_pwm_40;
	unsigned short 	maintenance_pwm_n;
	unsigned short 	maintenance_step_number;

	//Falling.
	unsigned short 	falling_pwm_200_initial;
	unsigned short 	falling_pwm_200_final;
	unsigned short 	falling_pwm_40_initial;
	unsigned short 	falling_pwm_40_final;
	unsigned short 	falling_pwm_n_initial;
	unsigned short 	falling_pwm_n_final;
	unsigned short 	falling_time;
	unsigned short 	falling_step_number;
	unsigned char 	falling_type;

	//Low.
	unsigned short 	low_step_number;

	unsigned short burst_mode_on;
	unsigned short burst_mode_off;
}warningup_coolingdown_typedef;

//Plateau.
typedef struct session_plateau_channel_rising_typedef{
	unsigned short pwm_200_initial;
	unsigned short pwm_200_final;
	unsigned short pwm_40_initial;
	unsigned short pwm_40_final;
	unsigned short pwm_n_initial;
	unsigned short pwm_n_final;
	unsigned short step_number;
}session_plateau_channel_rising_typedef;

typedef struct session_plateau_channel_maintenance_typedef{
	unsigned short pwm_200;
	unsigned short pwm_40;
	unsigned short pwm_n;
	unsigned short step_number;
}session_plateau_channel_maintenance_typedef;

typedef struct session_plateau_channel_falling_typedef{
	unsigned short pwm_200_initial;
	unsigned short pwm_200_final;
	unsigned short pwm_40_initial;
	unsigned short pwm_40_final;
	unsigned short pwm_n_initial;
	unsigned short pwm_n_final;
	unsigned short falling_time;
	unsigned short step_number;
	unsigned char type;
}session_plateau_channel_falling_typedef;

typedef struct session_plateau_channel_low_typedef{
	unsigned short step_number;
	unsigned short burst_mode_on;
	unsigned short burst_mode_off;
}session_plateau_channel_low_typedef;

//--- end ---//

unsigned char Get_Antenna_Name(unsigned char channel, char * p_buff);
void Save_Antenna_Name(unsigned char channel, char * p_buff);

//--- Set functions ---//
void Session_Set_Status 			(session_typedef * ptr_session, unsigned char stage , unsigned char new_state);
void Session_Set_Special_Function 	(session_typedef * ptr_session, unsigned char stage , unsigned char value);
void Session_Set_Duration 			(session_typedef * ptr_session, unsigned char stage , duration_typedef * ptr_duration);
void Session_Set_Antenna 			(session_typedef * ptr_session, unsigned char stage , antenna_typedef * ptr_antenna);
void Session_Clear_Antenna			(session_typedef * ptr_session, unsigned char stage);
void Session_Set_Signal 			(session_typedef * ptr_session, unsigned char stage, signal_typedef * ptr_signal);
//--- end ---//

//--- Get functions ---//
void Session_Get_Status (session_typedef * ptr_session, unsigned char stage , unsigned char *new_state);
void Session_Get_Special_Function (session_typedef * ptr_session, unsigned char stage , unsigned char *value);
void Session_Get_Duration (session_typedef * ptr_session, unsigned char stage , duration_typedef * ptr_duration);
void Session_Get_Antenna (session_typedef * ptr_session, unsigned char stage , antenna_typedef * ptr_antenna);
void Session_Get_Signal (session_typedef * ptr_session, unsigned char stage, signal_typedef * ptr_signal);
//--- end ---//

//--- Slot functions ---//
void SessionSend(session_typedef * ptr_session);
void Channel_Load (session_typedef * ptr_session, unsigned char channel);
void Session_Load (session_typedef * ptr_session, unsigned char slot, unsigned char channel);
void Session_Save (session_typedef * ptr_session, unsigned char slot);
//--- end ---//
#endif
