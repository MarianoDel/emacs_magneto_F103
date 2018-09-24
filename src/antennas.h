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
#include "GTK_Estructura.h"        //por antenna_typedef


//---- Configurations Defines --------------------
#define ANTENNA_KEEP_ALIVE_TIMER        2500    //2.5 segundos
#define ANTENNA_TEMP_TIMER        1000    //1 segundo
#define KEEP_ALIVE_COUNTER    3
#define ANTENNA_INFO_TIMER_FIRST_START        10000

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
void AntennaSetCurrentTemp (unsigned char, unsigned char, unsigned char);
unsigned char AntennaGetConnection (unsigned char);
unsigned char AntennaGetCurrentTemp (unsigned char);
void AntennaGetParamsStruct (unsigned char, antenna_typedef *);
void AntennaSetParamsStruct (unsigned char, antenna_typedef *);
unsigned char AntennaVerifyForTreatment (unsigned char);
void AntennaIsAnswering (unsigned char);
void Signal_TIM1MS (void);
void AntennaEndTreatment (unsigned char);


// void Session_Set_Antenna (session_typedef * ptr_session, unsigned char stage , antenna_typedef * ptr_antenna);
// void Session_Clear_Antenna (session_typedef * ptr_session, unsigned char stage);
// void Session_Get_Antenna (session_typedef * ptr_session, unsigned char stage , antenna_typedef * ptr_antenna);


#endif    /* _ANTENNAS_H_ */

//--- end of file ---//

