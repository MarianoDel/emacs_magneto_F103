#ifndef SIGNAL_H_
#define SIGNAL_H_

#include "GTK_Estructura.h"

//--- New code ---//
void Signal_TIM1MS (void);
void TIM5_IRQHandler (void);
void TIM5_Init (void);
//--- Channel 1 ---//
void Channel_1_Init(void);
void Session_Channel_1_Start(void);
void Session_Channel_1_Stop(void);
unsigned char Session_Channel_1_Verify_Antenna (session_typedef * ptr_session);
void Session_Channel_1 (void);
//Warning up.
void Session_Warning_Up_Channel_1_Restart(void);
unsigned char Session_Warning_Up_Channel_1(void);
unsigned char Session_Warning_Up_Channel_1_Parameters_Calculate(void);
//Plateau.
void Session_Plateau_Channel_1_Restart(void);
unsigned char Session_Plateau_Channel_1(void);
unsigned char Session_Plateau_Channel_1_Parameters_Calculate(void);
//Cooling down.
unsigned char Session_Cooling_Down_Channel_1(void);
unsigned char Session_Cooling_Down_Channel_1_Parameters_Calculate(void);
void Session_Cooling_Down_Channel_1_Restart(void);

unsigned char Session_Channels_Parameters_Calculate(unsigned char channel, unsigned char session_stage);
unsigned char Session_Warming_Up_Channels (unsigned char channel);
unsigned char Session_Cooling_Down_Channels (unsigned char channel);
unsigned char Session_Plateau_Channels(unsigned char channel);

//--- Channel 2 ---//
void Channel_2_Init(void);
void Session_Channel_2_Start(void);
void Session_Channel_2_Stop(void);
unsigned char Session_Channel_2_Verify_Antenna (session_typedef * ptr_session);
void Session_Channel_2 (void);
//Warning up.
void Session_Warning_Up_Channel_2_Restart(void);
unsigned char Session_Warning_Up_Channel_2(void);
unsigned char Session_Warning_Up_Channel_2_Parameters_Calculate(void);
//Plateau.
void Session_Plateau_Channel_2_Restart(void);
unsigned char Session_Plateau_Channel_2(void);
unsigned char Session_Plateau_Channel_2_Parameters_Calculate(void);
//Cooling down.
unsigned char Session_Cooling_Down_Channel_2(void);
unsigned char Session_Cooling_Down_Channel_2_Parameters_Calculate(void);
void Session_Cooling_Down_Channel_2_Restart(void);

//--- Channel 3 ---//
void Channel_3_Init(void);
void Session_Channel_3_Start(void);
void Session_Channel_3_Stop(void);
unsigned char Session_Channel_3_Verify_Antenna (session_typedef * ptr_session);
void Session_Channel_3 (void);
//Warning up.
void Session_Warning_Up_Channel_3_Restart(void);
unsigned char Session_Warning_Up_Channel_3(void);
unsigned char Session_Warning_Up_Channel_3_Parameters_Calculate(void);
//Plateau.
void Session_Plateau_Channel_3_Restart(void);
unsigned char Session_Plateau_Channel_3(void);
unsigned char Session_Plateau_Channel_3_Parameters_Calculate(void);
//Cooling down.
unsigned char Session_Cooling_Down_Channel_3(void);
unsigned char Session_Cooling_Down_Channel_3_Parameters_Calculate(void);
void Session_Cooling_Down_Channel_3_Restart(void);

//--- Channel 4 ---//
void Channel_4_Init(void);
void Session_Channel_4_Start(void);
void Session_Channel_4_Stop(void);
unsigned char Session_Channel_4_Verify_Antenna (session_typedef * ptr_session);
void Session_Channel_4 (void);

//Warning up.
void Session_Warning_Up_Channel_4_Restart(void);
unsigned char Session_Warning_Up_Channel_4(void);
unsigned char Session_Warning_Up_Channel_4_Parameters_Calculate(void);
//Plateau.
void Session_Plateau_Channel_4_Restart(void);
unsigned char Session_Plateau_Channel_4(void);
unsigned char Session_Plateau_Channel_4_Parameters_Calculate(void);
//Cooling down.
unsigned char Session_Cooling_Down_Channel_4(void);
unsigned char Session_Cooling_Down_Channel_4_Parameters_Calculate(void);
void Session_Cooling_Down_Channel_4_Restart(void);

//-- Para control de corriente en los canales
void Session_Current_Limit_control (void);
unsigned char Current_Limit_CheckCh1 (void);
unsigned char Current_Limit_CheckCh2 (void);
unsigned char Current_Limit_CheckCh3 (void);
unsigned char Current_Limit_CheckCh4 (void);
void Current_Limit_Counter_Reset (void);

//--- Para errores globales de antena
//
#define BIT_ERROR_CHECK				0x01
#define BIT_ERROR_PARAMS_FINISH		0x02
#define BIT_ERROR_CHECK_MASK		0xF0

#define BIT_ERROR_ANTENNA			0x10
#define BIT_ERROR_WARMING_UP		0x20
#define BIT_ERROR_CURRENT			0x40
//
void CheckforGlobalErrors (void);
void SetCheckGlobalErrors (unsigned char );
void ResetCheckGlobalErrors (void);
void SetBitGlobalErrors (unsigned char, unsigned char);

//--- end ---//

//--- New code ---//
enum falling_type
{
	FALLING_SLOW_DISCHARGE = 1,
	FALLING_LR,
	FALLING_FAST_DISCHARGE
};

//estados de session_stage
#define WARMING_UP	1
#define PLATEAU		2
#define COOLING_DOWN	3

//canales
#define CH0		0
#define CH1		1
#define CH2		2
#define CH3		3
#define CH4		4

//estados de las bajadas fall_type_step_chX
#define FALL_START	0
#define FALL_MED	1
#define FALL_FAST	2



//--- end ---//
#endif
