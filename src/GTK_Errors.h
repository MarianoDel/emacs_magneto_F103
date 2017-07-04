#ifndef ERRORS_H_
#define ERRORS_H_

//Antenna conection errors.
#define ERR_CHANNEL_ANTENNA_DISCONNECTED(err_channel)	0x10 + err_channel //The equipment can't find antenna. Antenna doesn't response.
#define ERR_CHANNEL_ANTENNA_LOST(err_channel)			0x20 + err_channel //Antenna disconnected.

//Antenna temperature errors.
#define ERR_CHANNEL_ANTENNA_TMP_OUT_OF_RANGE(err_channel)	0x40 + err_channel //The temperature of antenna was major than upper limit.
#define ERR_CHANNEL_ANTENNA_CURRENT_OUT_OF_RANGE(err_channel)	0x50 + err_channel //The temperature of antenna was major than upper limit.

//Session errors.
#define ERR_CHANNEL_WARNING_UP(err_channel)										0x110 + err_channel //The equipment had an error in warning up section.
#define ERR_CHANNEL_WARNING_UP_PARAMETERS_CALCULATE(err_channel)				0x120 + err_channel


#define ERR_CHANNEL_PLATEAU(err_channel)										0x130 + err_channel //The equipment had an error in plateau section.
#define ERR_CHANNEL_PLATEAU_PARAMETERS_CALCULATE(err_channel)					0x140 + err_channel


#define ERR_CHANNEL_COOLING_DOWN(err_channel)									0x150 + err_channel //The equipment had an error in cooling down section.
#define ERR_CHANNEL_COOLING_DOWN_PARAMETERS_CALCULATE(err_channel)				0x160 + err_channel



#endif
