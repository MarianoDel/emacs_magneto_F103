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
#define ANTENNA_KEEP_ALIVE_TIMER        1250    //1.25 segundos
#define ANTENNA_TEMP_TIMER        1000    //1 segundo
#define KEEP_ALIVE_COUNTER    3
#define KEEP_ALIVE_ANTENNA_NAME_COUNTER    3
#define ANTENNA_INFO_TIMER_FIRST_START        10000
#define ANTENNA_INFO_TIMER_UPDATE        (KEEP_ALIVE_COUNTER * ANTENNA_KEEP_ALIVE_TIMER)

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
unsigned char AntennaGetCurrentTemp (unsigned char);

void AntennaSetParamsStruct (unsigned char, antenna_typedef *);
void AntennaGetParamsStruct (unsigned char, antenna_typedef *);

unsigned char AntennaVerifyForTreatment (unsigned char);
unsigned char AntennaGetConnection (unsigned char);

void AntennaIsAnswering (unsigned char);
void AntennaEndTreatment (unsigned char);
void AntennaSendKnowInfoWithTimer (void);
void AntennaSetName (unsigned char, char *);


#endif    /* _ANTENNAS_H_ */

//--- end of file ---//

