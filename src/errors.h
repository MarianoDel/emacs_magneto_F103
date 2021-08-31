//---------------------------------------------------------
// #### PROYECTO MAGNETO GAUSSTEK - MotherBoard ###########
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F103
// ##
// #### ERRORS.H ##########################################
//---------------------------------------------------------
#ifndef _ERRORS_H_
#define _ERRORS_H_


// Exported Types Constants and Macros -----------------------------------------

//Antenna conection errors.
// The equipment can't find antenna. Antenna doesn't response, (probably never connected)
#define ERR_CHANNEL_ANTENNA_DISCONNECTED(err_channel)    (0x10 + err_channel)
// Antenna disconnected.
#define ERR_CHANNEL_ANTENNA_LOST(err_channel)    (0x20 + err_channel)
// Antenna is not emitting
#define ERR_CHANNEL_ANTENNA_NOT_EMITTING(err_channel)    (0x30 + err_channel)
// Antenna temperature errors,  was major than upper limit.
#define ERR_CHANNEL_ANTENNA_TMP_OUT_OF_RANGE(err_channel)    (0x40 + err_channel)
// The current is bigger than limit.
#define ERR_CHANNEL_ANTENNA_CURRENT_OUT_OF_RANGE(err_channel)    (0x50 + err_channel)

//Session errors.
//The equipment had an error in warming up section.
#define ERR_CHANNEL_WARMING_UP_PARAMETERS_CALCULATE(err_channel) (0x60 + err_channel)
#define ERR_CHANNEL_WARMING_UP(err_channel)    (0x70 + err_channel)

//The equipment had an error in plateau section.
#define ERR_CHANNEL_PLATEAU_PARAMETERS_CALCULATE(err_channel)    (0x80 + err_channel)
#define ERR_CHANNEL_PLATEAU(err_channel)    (0x90 + err_channel )

//The equipment had an error in cooling down section.
#define ERR_CHANNEL_COOLING_DOWN_PARAMETERS_CALCULATE(err_channel)    (0x100 + err_channel)
#define ERR_CHANNEL_COOLING_DOWN(err_channel)    (0x110 + err_channel )


// Module Exported Functions ---------------------------------------------------
void Error_SetString (char * p_errstr, unsigned short err_num);

#endif    /* _ERRORS_H_ */

//--- end of file ---//

