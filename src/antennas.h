//---------------------------------------------------------
// #### PROYECTO MAGNETO GAUSSTEK - MotherBoard ###########
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F103
// ##
// #### ANTENNAS.H ########################################
//---------------------------------------------------------
#ifndef _ANTENNAS_H_
#define _ANTENNAS_H_

//---- Includes to help the Defines ----------



//---- Configurations Defines --------------------
#define ANTENNA_KEEP_ALIVE_TIMER        1000    //1 segundo


//---- Common Defines --------------------

//---- Exported Types --------------------
typedef enum {
    ANTENNA_INIT = 0,
    ANTENNA_IN_STANDBY,
    ANTENNA_IN_TREATMENT,
    ANTENNA_IN_PAUSE

} antenna_state_t;

enum Session_Channel_1_Verify_Antenna_states
{
	SESSION_CHANNEL_1_VERIFY_ANTENNA_INIT = 0,
	SESSION_CHANNEL_1_VERIFY_ANTENNA_WAIT_PARAMS,
	SESSION_CHANNEL_1_VERIFY_ANTENNA_FIN_OK,
	SESSION_CHANNEL_1_VERIFY_ANTENNA_FIN_ERROR
};

enum Session_Channel_2_Verify_Antenna_states
{
	SESSION_CHANNEL_2_VERIFY_ANTENNA_INIT = 0,
	SESSION_CHANNEL_2_VERIFY_ANTENNA_WAIT_PARAMS,
	SESSION_CHANNEL_2_VERIFY_ANTENNA_FIN_OK,
	SESSION_CHANNEL_2_VERIFY_ANTENNA_FIN_ERROR
};

enum Session_Channel_3_Verify_Antenna_states
{
	SESSION_CHANNEL_3_VERIFY_ANTENNA_INIT = 0,
	SESSION_CHANNEL_3_VERIFY_ANTENNA_WAIT_PARAMS,
	SESSION_CHANNEL_3_VERIFY_ANTENNA_FIN_OK,
	SESSION_CHANNEL_3_VERIFY_ANTENNA_FIN_ERROR
};

enum Session_Channel_4_Verify_Antenna_states
{
	SESSION_CHANNEL_4_VERIFY_ANTENNA_INIT = 0,
	SESSION_CHANNEL_4_VERIFY_ANTENNA_WAIT_PARAMS,
	SESSION_CHANNEL_4_VERIFY_ANTENNA_FIN_OK,
	SESSION_CHANNEL_4_VERIFY_ANTENNA_FIN_ERROR
};


//--- Exported functions ---//
void AntennaUpdateStates (void);


// void Session_Set_Antenna (session_typedef * ptr_session, unsigned char stage , antenna_typedef * ptr_antenna);
// void Session_Clear_Antenna (session_typedef * ptr_session, unsigned char stage);
// void Session_Get_Antenna (session_typedef * ptr_session, unsigned char stage , antenna_typedef * ptr_antenna);


#endif    /* _ANTENNAS_H_ */

//--- end of file ---//

