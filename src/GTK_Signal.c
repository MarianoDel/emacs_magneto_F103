//#include "GTK_Header.h"

#include <stm32f10x.h>
//#include <stm32f10x_gpio.h>
//#include <stm32f10x_rcc.h>
//#include <stm32f10x_tim.h>
//#include <stm32f10x_adc.h>
//#include <stm32f10x_flash.h>
//#include <stm32f10x_usart.h>

//#include <stm32f10x_exti.h>
#include <misc.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

//#include "string.h"
//#include "stdio.h"
#include "GTK_Hard.h"
#include "timer.h"
#include "uart.h"

#include "float.h"

#include "pwm.h"
#include "adc.h"
#include "stm32f10x_adc.h"
#include "GTK_Estructura.h"
#include "GTK_Signal.h"
#include "flash_program.h"
#include "GTK_Errors.h"


//--- New code ---//
#define resistance_discharge 1175
#define capacitance_discharge 100 //uF
#define tau_discharge 0.1175 //RC

extern char buffSendErr[32];

extern unsigned char channel_1_pause;
extern unsigned char channel_2_pause;
extern unsigned char channel_3_pause;
extern unsigned char channel_4_pause;

//External variables.
extern session_typedef session_ch_1;
extern session_typedef session_ch_2;
extern session_typedef session_ch_3;
extern session_typedef session_ch_4;

// Global for Sync
unsigned char ch1_sync_state = 0;
#define SYNC_REQUIRED	0x80
#define NO_SYNC_REQUIRED_MASK	0x7F
#define SYNC_RESET			0x00
#define SYNC_IN_RISING		0x01
#define SYNC_IN_MAINTENANCE	0x02
#define SYNC_IN_FALLING		0x04
#define SYNC_IN_DOWN		0x08
#define SYNC_FINISH_WARMING_UP		0x10
#define SYNC_FINISH_PLATEAU			0x20
#define SYNC_FINISH_COOLING_DOWN	0x40
#define SYNC_ACT_MASK	(SYNC_IN_RISING | SYNC_IN_MAINTENANCE | SYNC_IN_FALLING | SYNC_IN_DOWN)

#define WITH_SYNC

//--- Time base ---//
#define SESSION_CHANNEL_ASK_TEMP 2500 //2.5 segundos
#define SESSION_CHANNEL_ANSWER_TEMP 10000 //10segundos.
//--- Channel 1 ---//
unsigned short session_channel_1_ask_temp = 0;
unsigned short session_channel_1_answer_temp = 0;
unsigned short session_plateau_channel_1_time;
unsigned int session_plateau_channel_1_stage_time = 0;

unsigned short session_warning_up_channel_1_time;
unsigned short session_warning_up_channel_1_time_2;
extern volatile unsigned int session_warning_up_channel_1_stage_time;

unsigned short session_cooling_down_channel_1_time;
unsigned short session_cooling_down_channel_1_time_2;
unsigned int session_cooling_down_channel_1_stage_time = 0;

unsigned char session_warning_up_channel_1_state = 0;

enum session_warning_up_channel_states {

	SESSION_WARMING_UP_CHANNEL_INIT = 0,
	SESSION_WARMING_UP_CHANNEL_PARAMETERS_CALCULATE,
	SESSION_WARMING_UP_CHANNEL_PARAMETERS_CALCULATE_END,
	SESSION_WARMING_UP_CHANNEL_CHANGE_LEVEL,
	SESSION_WARMING_UP_CHANNEL_RISING_EDGE,
	SESSION_WARMING_UP_CHANNEL_MAINTENANCE,
	SESSION_WARMING_UP_CHANNEL_FALLING_EDGE,
	SESSION_WARMING_UP_CHANNEL_LOW,
	SESSION_WARMING_UP_CHANNEL_END_ERROR
};

unsigned char session_warning_up_channel_1_step = 0;
unsigned short session_warning_up_channel_1_burst_cnt = 0;
unsigned char fall_type_step_ch1 = 0;

float pwm_slope_channel_1;
float pwm_channel_1;
unsigned short session_plateau_channel_1_time_2;
unsigned short session_plateau_channel_1_burst_cnt = 0;

unsigned char session_cooling_down_channel_1_state = 0;

enum session_cooling_down_channel_states {

	SESSION_COOLING_DOWN_CHANNEL_INIT = 0,
	SESSION_COOLING_DOWN_CHANNEL_PARAMETERS_CALCULATE,
	SESSION_COOLING_DOWN_CHANNEL_CHANGE_LEVEL,
	SESSION_COOLING_DOWN_CHANNEL_RISING_EDGE,
	SESSION_COOLING_DOWN_CHANNEL_MAINTENANCE,
	SESSION_COOLING_DOWN_CHANNEL_FALLING_EDGE,
	SESSION_COOLING_DOWN_CHANNEL_LOW,
	SESSION_COOLING_DOWN_CHANNEL_END_ERROR
};

unsigned char session_cooling_down_channel_1_step = 0;
unsigned short session_cooling_down_channel_1_burst_cnt = 0;

unsigned char session_plateau_channel_1_state = 0;

enum session_plateau_channel_states {

	SESSION_PLATEAU_CHANNEL_INIT = 0,
	SESSION_PLATEAU_CHANNEL_PARAMETERS_CALCULATE,
	SESSION_PLATEAU_CHANNEL_RISING_EDGE,
	SESSION_PLATEAU_CHANNEL_MAINTENANCE,
	SESSION_PLATEAU_CHANNEL_FALLING_EDGE,
	SESSION_PLATEAU_CHANNEL_LOW,
	SESSION_PLATEAU_CHANNEL_END_ERROR
};

//--- Channel 2 ---//
unsigned short session_channel_2_ask_temp = 0;
unsigned short session_channel_2_answer_temp = 0;
unsigned short session_plateau_channel_2_time;
unsigned int session_plateau_channel_2_stage_time = 0;

unsigned short session_warning_up_channel_2_time;
unsigned short session_warning_up_channel_2_time_2;
unsigned int session_warning_up_channel_2_stage_time = 0;

unsigned short session_cooling_down_channel_2_time;
unsigned short session_cooling_down_channel_2_time_2;
unsigned int session_cooling_down_channel_2_stage_time = 0;

unsigned char session_warning_up_channel_2_state = 0;
unsigned char session_warning_up_channel_2_step = 0;
unsigned short session_warning_up_channel_2_burst_cnt = 0;

unsigned char fall_type_step_ch2 = 0;

float pwm_slope_channel_2;
float pwm_channel_2;
unsigned short session_plateau_channel_2_time_2;
unsigned short session_plateau_channel_2_burst_cnt = 0;
unsigned char session_plateau_channel_2_state = 0;

unsigned char session_cooling_down_channel_2_state = 0;
unsigned char session_cooling_down_channel_2_step = 0;
unsigned short session_cooling_down_channel_2_burst_cnt = 0;


//--- Channel 3 ---//
unsigned short session_channel_3_ask_temp = 0;
unsigned short session_channel_3_answer_temp = 0;
unsigned short session_plateau_channel_3_time;
unsigned int session_plateau_channel_3_stage_time = 0;

unsigned short session_warning_up_channel_3_time;
unsigned short session_warning_up_channel_3_time_2;
unsigned int session_warning_up_channel_3_stage_time = 0;

unsigned short session_cooling_down_channel_3_time;
unsigned short session_cooling_down_channel_3_time_2;
unsigned int session_cooling_down_channel_3_stage_time = 0;

unsigned char session_warning_up_channel_3_state = 0;
unsigned char fall_type_step_ch3 = 0;

float pwm_slope_channel_3;
float pwm_channel_3;
unsigned short session_plateau_channel_3_time_2;
unsigned short session_plateau_channel_3_burst_cnt = 0;

unsigned char session_warning_up_channel_3_step = 0;
unsigned short session_warning_up_channel_3_burst_cnt = 0;

unsigned char session_cooling_down_channel_3_state = 0;
unsigned char session_cooling_down_channel_3_step = 0;
unsigned short session_cooling_down_channel_3_burst_cnt = 0;
unsigned char session_plateau_channel_3_state = 0;

//--- Channel 4 ---//
unsigned short session_channel_4_ask_temp = 0;
unsigned short session_channel_4_answer_temp = 0;
unsigned short session_plateau_channel_4_time;
unsigned int session_plateau_channel_4_stage_time = 0;

unsigned short session_warning_up_channel_4_time;
unsigned short session_warning_up_channel_4_time_2;
unsigned int session_warning_up_channel_4_stage_time = 0;

unsigned short session_cooling_down_channel_4_time;
unsigned short session_cooling_down_channel_4_time_2;
unsigned int session_cooling_down_channel_4_stage_time = 0;

unsigned char session_warning_up_channel_4_state = 0;
unsigned char fall_type_step_ch4 = 0;

float pwm_slope_channel_4;
float pwm_channel_4;
unsigned short session_plateau_channel_4_time_2;
unsigned short session_plateau_channel_4_burst_cnt = 0;

unsigned char session_warning_up_channel_4_step = 0;
unsigned short session_warning_up_channel_4_burst_cnt = 0;

unsigned char session_cooling_down_channel_4_state = 0;
unsigned char session_cooling_down_channel_4_step = 0;
unsigned short session_cooling_down_channel_4_burst_cnt = 0;


unsigned char session_plateau_channel_4_state = 0;

//--- Current limit Externals ---//
extern volatile unsigned char flagMuestreo;
extern volatile unsigned char take_current_samples;

//--- Current limit Globals ---//
enum CurrState {
	CURRENT_INIT_CHECK = 0,
	CURRENT_CH1,
	CURRENT_CH1_WAIT_SAMPLE,
	CURRENT_CH1_CHECK,
	CURRENT_CH2,
	CURRENT_CH2_WAIT_SAMPLE,
	CURRENT_CH2_CHECK,
	CURRENT_CH3,
	CURRENT_CH3_WAIT_SAMPLE,
	CURRENT_CH3_CHECK,
	CURRENT_CH4,
	CURRENT_CH4_WAIT_SAMPLE,
	CURRENT_CH4_CHECK

};
enum CurrState current_limit_state = CURRENT_CH1;
unsigned short actual_current [5] = { 0, 0, 0, 0, 0};	//CH0 es un dummy

unsigned char global_error_ch1 = 0;
unsigned char global_error_ch2 = 0;
unsigned char global_error_ch3 = 0;
unsigned char global_error_ch4 = 0;


//--- Channel 1 ---//
void TIM5_IRQHandler (void)	//100uS
{

	//Channel 1.
	if (channel_1_pause == 0)
	{

			session_plateau_channel_1_time++;

			if (session_plateau_channel_1_stage_time)
				session_plateau_channel_1_stage_time--;

			session_warning_up_channel_1_time++;

			if (session_warning_up_channel_1_stage_time)
				session_warning_up_channel_1_stage_time--;

			session_cooling_down_channel_1_time++;

			if (session_cooling_down_channel_1_stage_time)
				session_cooling_down_channel_1_stage_time--;
	}
	//Channel 2.
	if (channel_2_pause == 0)
	{

		session_plateau_channel_2_time++;

		if (session_plateau_channel_2_stage_time)
			session_plateau_channel_2_stage_time--;

		session_warning_up_channel_2_time++;

		if (session_warning_up_channel_2_stage_time)
			session_warning_up_channel_2_stage_time--;

		session_cooling_down_channel_2_time++;

		if (session_cooling_down_channel_2_stage_time)
			session_cooling_down_channel_2_stage_time--;

	}
	//Channel 3.
	if (channel_3_pause == 0)
	{

		session_plateau_channel_3_time++;

		if (session_plateau_channel_3_stage_time)
			session_plateau_channel_3_stage_time--;

		session_warning_up_channel_3_time++;

		if (session_warning_up_channel_3_stage_time)
			session_warning_up_channel_3_stage_time--;

		session_cooling_down_channel_3_time++;

		if (session_cooling_down_channel_3_stage_time)
			session_cooling_down_channel_3_stage_time--;
	}
	//Channel 4.
	if (channel_4_pause == 0)
	{

		session_plateau_channel_4_time++;

		if (session_plateau_channel_4_stage_time)
			session_plateau_channel_4_stage_time--;

		session_warning_up_channel_4_time++;

		if (session_warning_up_channel_4_stage_time)
			session_warning_up_channel_4_stage_time--;

		session_cooling_down_channel_4_time++;

		if (session_cooling_down_channel_4_stage_time)
			session_cooling_down_channel_4_stage_time--;

	}
	//bajar flag
	if (TIM5->SR & 0x01)	//bajo el flag
		TIM5->SR = 0x00;
}


void TIM5_Init (void)
{

	NVIC_InitTypeDef NVIC_InitStructure;

	if (!RCC_TIM5_CLK)
		RCC_TIM5_CLKEN;

	TIM5->CR1 = 0x0000;

	//Configuracion del timer.
	TIM5->ARR = 100; //100uS.
	TIM5->CNT = 0;
	//TIM5->PSC = 71;
	TIM5->PSC = 143;		//error en frecuencia para TIM5 me da x2
	//TIM5->EGR = TIM_EGR_UG;

	// Enable timer ver UDIS
	TIM5->DIER |= TIM_DIER_UIE;
	TIM5->CR1 |= TIM_CR1_CEN;

	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 10;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 10;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}
//--- end ---//

//--- Channel 1 ---//
unsigned char session_channel_1_state;
unsigned char session_channel_2_state;
unsigned char session_channel_3_state;
unsigned char session_channel_4_state;

//--- Warning up ---//
//--- Channel 1 ---//
#define SESSION_WUP_CH1_BUFF_DIM 50
warningup_coolingdown_typedef table_warning_up_channel_1[SESSION_WUP_CH1_BUFF_DIM];

//--- Channel 2 ---//
#define SESSION_WUP_CH2_BUFF_DIM 50
warningup_coolingdown_typedef table_warning_up_channel_2[SESSION_WUP_CH2_BUFF_DIM];

//--- Channel 3 ---//
#define SESSION_WUP_CH3_BUFF_DIM 50
warningup_coolingdown_typedef table_warning_up_channel_3[SESSION_WUP_CH3_BUFF_DIM];

//--- Channel 4 ---//
#define SESSION_WUP_CH4_BUFF_DIM 50
warningup_coolingdown_typedef table_warning_up_channel_4[SESSION_WUP_CH4_BUFF_DIM];


//--- Plateau ---//
//--- Channel 1 ---//
warningup_coolingdown_typedef table_plateau_channel_1[1];

//--- Channel 2 ---//
warningup_coolingdown_typedef table_plateau_channel_2[1];

//--- Channel 3 ---//
warningup_coolingdown_typedef table_plateau_channel_3[1];

//--- Channel 4 ---//
warningup_coolingdown_typedef table_plateau_channel_4[1];

//--- Cooling down ---//
//--- Channel 1 ---//
#define SESSION_CDWN_CH1_BUFF_DIM 50
warningup_coolingdown_typedef table_cooling_down_channel_1[SESSION_CDWN_CH1_BUFF_DIM];

//--- Channel 2 ---//
#define SESSION_CDWN_CH2_BUFF_DIM 50
warningup_coolingdown_typedef table_cooling_down_channel_2[SESSION_CDWN_CH2_BUFF_DIM];

//--- Channel 3 ---//
#define SESSION_CDWN_CH3_BUFF_DIM 50
warningup_coolingdown_typedef table_cooling_down_channel_3[SESSION_CDWN_CH3_BUFF_DIM];

//--- Channel 4 ---//
#define SESSION_CDWN_CH4_BUFF_DIM 50
warningup_coolingdown_typedef table_cooling_down_channel_4[SESSION_CDWN_CH4_BUFF_DIM];

//--- end ---//

//--- Channel 1 ---//

void Channel_1_Init(void)
{
	channel_1_pause = 0;

	session_warning_up_channel_1_time = 0;
	session_warning_up_channel_1_time_2 = 0;
	session_warning_up_channel_1_stage_time = 0;

	session_plateau_channel_1_time = 0;
	session_plateau_channel_1_stage_time = 0;

	session_cooling_down_channel_1_time = 0;
	session_cooling_down_channel_1_time_2 = 0;
	session_cooling_down_channel_1_stage_time = 0;

	session_channel_1_state = 0;

	//limpio tabla WARMING_UP
	memset(&table_warning_up_channel_1[0], '\0', sizeof(table_warning_up_channel_1));

	//limpio tabla PLATEAU
	memset(&table_plateau_channel_1[0], '\0', sizeof(table_plateau_channel_1));

	//limpio tabla COOLING_DOWN
	memset(&table_cooling_down_channel_1[0], '\0', sizeof(table_cooling_down_channel_1));

}

enum states_channel_1 {

	SESSION_CHANNEL_1_INIT = 0,
	SESSION_CHANNEL_1_VERIFY_ANTENNA,
	SESSION_CHANNEL_1_WARNING_UP,
	SESSION_CHANNEL_1_PLATEAU,
	SESSION_CHANNEL_1_COOLING_DOWN,
	SESSION_CHANNEL_1_END
};

void Session_Channel_1_Start(void)
{
	session_channel_1_state = SESSION_CHANNEL_1_INIT;
	session_ch_1.status = 1;
}
void Session_Channel_1_Stop(void)
{
	session_ch_1.status = 0;
}

unsigned char Session_Channel_1_Verify_Antenna_state = 0;

#define SESSION_CHANNEL_1_VERIFY_ANTENNA_TIME 5000			//ms
unsigned short Session_Channel_1_Verify_Antenna_time = 0;

enum Session_Channel_1_Verify_Antenna_states
{
	SESSION_CHANNEL_1_VERIFY_ANTENNA_INIT = 0,
	SESSION_CHANNEL_1_VERIFY_ANTENNA_WAIT_PARAMS,
	SESSION_CHANNEL_1_VERIFY_ANTENNA_FIN_OK,
	SESSION_CHANNEL_1_VERIFY_ANTENNA_FIN_ERROR
};
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

extern unsigned char temp_actual_channel_1_int;
extern unsigned char temp_actual_channel_1_dec;

void Session_Channel_1 (void)
{

	unsigned char i;

	//if the channel is enabled the session occurs.
	//Else the program will wait with the channel turned off.
	if (session_ch_1.status)
	{

		switch (session_channel_1_state)
		{
			case SESSION_CHANNEL_1_INIT:

				temp_actual_channel_1_int = 0;
				temp_actual_channel_1_dec = 0;

				session_channel_1_answer_temp = SESSION_CHANNEL_ANSWER_TEMP;
				session_channel_1_ask_temp = SESSION_CHANNEL_ASK_TEMP;

				PWM_CH1_TiempoSubida(0); //pwm 200V.
				PWM_CH1_TiempoMantenimiento(0);
				PWM_CH1_TiempoBajada(0);

				//Restart stages.
				Session_Warning_Up_Channel_1_Restart();
				Session_Plateau_Channel_1_Restart();
				Session_Cooling_Down_Channel_1_Restart();

				ch1_sync_state = SYNC_RESET;
				SetBitGlobalErrors (CH1, BIT_ERROR_CHECK);

				session_channel_1_state = SESSION_CHANNEL_1_VERIFY_ANTENNA;
				break;

			case SESSION_CHANNEL_1_VERIFY_ANTENNA:

#ifndef FUNC_SIN_ANTENA_CH1
				i = Session_Channel_1_Verify_Antenna(&session_ch_1);


				if (i == FIN_OK)
					session_channel_1_state = SESSION_CHANNEL_1_WARNING_UP;

				else if (i == FIN_ERROR)
				{
					session_ch_1.status = 0;

					SetBitGlobalErrors (CH1, BIT_ERROR_ANTENNA);
					sprintf(&buffSendErr[0], (const char *) "ERROR(0x%03X)\r\n", ERR_CHANNEL_ANTENNA_DISCONNECTED(1));
					UART_PC_Send(&buffSendErr[0]);
				}
#else
				//aviso que encontre la antenna y hago un hardcode de parametros
				//--- Antenna parameters ---//
				session_ch_1.stage_1_resistance_int = 10;
				session_ch_1.stage_1_resistance_dec = 0;
				session_ch_1.stage_1_inductance_int = 50;
				session_ch_1.stage_1_inductance_dec = 0;
				session_ch_1.stage_1_current_limit_int = 2;
				session_ch_1.stage_1_current_limit_dec = 0;


				UART_PC_Send("Antenna hardcoded\r\n");
				session_channel_1_state = SESSION_CHANNEL_1_WARNING_UP;
#endif
				break;

			case SESSION_CHANNEL_1_WARNING_UP:

				//Warning up.
				if (channel_1_pause == 0)
				{

					//los errores pueden ser por calculo de parametros o falta de sync cuando se necesita
					i = Session_Warming_Up_Channels(CH1);

					if (i == FIN_OK)
					{
						session_channel_1_state = SESSION_CHANNEL_1_PLATEAU;
						UART_PC_Send("End_Warning_Up,1\r\n");

						PWM_CH1_TiempoSubida(0); //pwm 200V.
						PWM_CH1_TiempoMantenimiento(0);
						PWM_CH1_TiempoBajada(0);

					}

					if ((i == TRABAJANDO) && (session_warning_up_channel_1_state > SESSION_WARMING_UP_CHANNEL_PARAMETERS_CALCULATE))
					{
						Current_Limit_CheckCh1();
//						if (LED2)
//							LED2_OFF;
//						else
//							LED2_ON;
					}

					if (i == FIN_ERROR)
					{
						session_ch_1.status = 0;
						SetBitGlobalErrors (CH1, BIT_ERROR_WARMING_UP);

						sprintf(&buffSendErr[0], (const char *) "ERROR(0x%03X)\r\n", ERR_CHANNEL_WARNING_UP(1));
						UART_PC_Send(&buffSendErr[0]);
					}
				}
				else
				{
					PWM_CH1_TiempoSubida(0);
					PWM_CH1_TiempoMantenimiento(0);
					PWM_CH1_TiempoBajada(0);
				}
				break;

			case SESSION_CHANNEL_1_PLATEAU:
				//Plateau.
				if (channel_1_pause == 0)
				{

					//i = Session_Plateau_Channel_1();
					i = Session_Plateau_Channels(CH1);

					if (i == FIN_OK)
					{
						session_channel_1_state = SESSION_CHANNEL_1_COOLING_DOWN;
						UART_PC_Send("End_Plateau,1\r\n");

						PWM_CH1_TiempoSubida(0); //pwm 200V.
						PWM_CH1_TiempoMantenimiento(0);
						PWM_CH1_TiempoBajada(0);
					}

					if ((i == TRABAJANDO) && (session_plateau_channel_1_state > SESSION_PLATEAU_CHANNEL_PARAMETERS_CALCULATE))
					{
						Current_Limit_CheckCh1();
					}

					if (i == FIN_ERROR)
					{
						session_ch_1.status = 0;

						sprintf(&buffSendErr[0], (const char *) "ERROR(0x%03X)\r\n", ERR_CHANNEL_PLATEAU(1));
						UART_PC_Send(&buffSendErr[0]);
					}
				}
				else
				{
					PWM_CH1_TiempoSubida(0);
					PWM_CH1_TiempoMantenimiento(0);
					PWM_CH1_TiempoBajada(0);
				}
				break;

			case SESSION_CHANNEL_1_COOLING_DOWN:
				//Cooling dawn.
				if (channel_1_pause == 0)
				{

					//i = Session_Cooling_Down_Channel_1();
					i = Session_Cooling_Down_Channels(CH1);

					if (i == FIN_OK)
					{
						session_channel_1_state = SESSION_CHANNEL_1_END;
						UART_PC_Send("End_Cooling_Down,1\r\n");

						UART_PC_Send("finish,1\r\n");
						PWM_CH1_TiempoSubida(0); //pwm 200V.
						PWM_CH1_TiempoMantenimiento(0);
						PWM_CH1_TiempoBajada(0);
					}

					if ((i == TRABAJANDO) && (session_cooling_down_channel_1_state > SESSION_COOLING_DOWN_CHANNEL_PARAMETERS_CALCULATE))
					{
						Current_Limit_CheckCh1();
					}

					if (i == FIN_ERROR)
					{
						session_ch_1.status = 0;

						sprintf(&buffSendErr[0], (const char *) "ERROR(0x%03X)\r\n", ERR_CHANNEL_COOLING_DOWN(1));
						UART_PC_Send(&buffSendErr[0]);
					}
				}
				else
				{
					PWM_CH1_TiempoSubida(0);
					PWM_CH1_TiempoMantenimiento(0);
					PWM_CH1_TiempoBajada(0);
				}
				break;

			case SESSION_CHANNEL_1_END:

				session_ch_1.status = 0;
				break;

			default:
				session_ch_1.status = 0;
				break;
		}

		if (session_channel_1_state >= SESSION_CHANNEL_1_WARNING_UP)
		{
			if (session_channel_1_ask_temp == 0)
			{
				UART_CH1_Send("get_temp\r\n");

				session_channel_1_ask_temp = SESSION_CHANNEL_ASK_TEMP;
			}

#ifdef WITH_TEMP_CH1
			if ((temp_actual_channel_1_int != 0) || (temp_actual_channel_1_dec != 0))
			{
				if ((temp_actual_channel_1_int > session_ch_1.stage_1_temp_max_int)
						|| ((temp_actual_channel_1_int == session_ch_1.stage_1_temp_max_int) && (temp_actual_channel_1_dec > session_ch_1.stage_1_temp_max_dec)))
				{
					session_ch_1.status = 0;
					sprintf(&buffSendErr[0], (const char *) "ERROR(0x%03X)\r\n", ERR_CHANNEL_ANTENNA_TMP_OUT_OF_RANGE(1));
					UART_PC_Send(&buffSendErr[0]);

					temp_actual_channel_1_int = 0;
					temp_actual_channel_1_dec = 0;
				}
				else
				{
					session_channel_1_answer_temp = SESSION_CHANNEL_ANSWER_TEMP;

					temp_actual_channel_1_int = 0;
					temp_actual_channel_1_dec = 0;
				}
			}
#else
			session_channel_1_answer_temp = SESSION_CHANNEL_ANSWER_TEMP;

			temp_actual_channel_1_int = 0;
			temp_actual_channel_1_dec = 0;
#endif

#ifndef FUNC_SIN_ANTENA_CH1
			if (session_channel_1_answer_temp == 0)
			{
				session_ch_1.status = 0;

				sprintf(&buffSendErr[0], (const char *) "ERROR(0x%03X)\r\n", ERR_CHANNEL_ANTENNA_LOST(1));
				UART_PC_Send(&buffSendErr[0]);
			}
#endif
		}
	}
	else
	{
		//Initial state.
		session_channel_1_state = SESSION_CHANNEL_1_INIT;

		//Stop pwm channels.
		PWM_CH1_TiempoSubida(0);
		PWM_CH1_TiempoMantenimiento(0);
		PWM_CH1_TiempoBajada(0);

		//Stop timer interrupt.
	}
}


unsigned char sync_in_waiting = 0;

void Session_Warning_Up_Channel_1_Restart(void)
{
	ch1_sync_state = 0;
	sync_in_waiting = 0;
	session_warning_up_channel_1_state = SESSION_WARMING_UP_CHANNEL_INIT;
}

void Session_Plateau_Channel_1_Restart(void)
{
	ch1_sync_state = 0;
	sync_in_waiting = 0;
	session_plateau_channel_1_state = SESSION_PLATEAU_CHANNEL_INIT;
}

void Session_Cooling_Down_Channel_1_Restart(void)
{
	ch1_sync_state = 0;
	sync_in_waiting = 0;
	session_cooling_down_channel_1_state = SESSION_COOLING_DOWN_CHANNEL_INIT;
}

void Session_Warning_Up_Channel_2_Restart(void)
{
	session_warning_up_channel_2_state = SESSION_WARMING_UP_CHANNEL_INIT;
}

void Session_Plateau_Channel_2_Restart(void)
{
	session_plateau_channel_2_state = SESSION_PLATEAU_CHANNEL_INIT;
}

void Session_Cooling_Down_Channel_2_Restart(void)
{
	session_cooling_down_channel_2_state = SESSION_COOLING_DOWN_CHANNEL_INIT;
}

void Session_Warning_Up_Channel_3_Restart(void)
{
	session_warning_up_channel_3_state = SESSION_WARMING_UP_CHANNEL_INIT;
}

void Session_Plateau_Channel_3_Restart(void)
{
	session_plateau_channel_3_state = SESSION_PLATEAU_CHANNEL_INIT;
}

void Session_Cooling_Down_Channel_3_Restart(void)
{
	session_cooling_down_channel_3_state = SESSION_COOLING_DOWN_CHANNEL_INIT;
}

void Session_Warning_Up_Channel_4_Restart(void)
{
	session_warning_up_channel_4_state = SESSION_WARMING_UP_CHANNEL_INIT;
}

void Session_Plateau_Channel_4_Restart(void)
{
	session_plateau_channel_4_state = SESSION_PLATEAU_CHANNEL_INIT;
}

void Session_Cooling_Down_Channel_4_Restart(void)
{
	session_cooling_down_channel_4_state = SESSION_COOLING_DOWN_CHANNEL_INIT;
}

unsigned char Session_Channels_Parameters_Calculate(unsigned char channel, unsigned char session_stage)
{

	//Antenna parameters.
	float resistance = 0.0;
	float inductance = 0.0;
	float current_limit = 0.0;

	//Signal parameters.
	float initial_current = 0.0;
	float final_current = 0.0;
	float current = 0.0;

	float auxiliar_duty = 0.0;
	float voltage = 0.0;
	float voltage2 = 0.0;

	float LR_tau = 0.0;
//	float Wp;
	float Td = 0.0;
	float Vsnubber = 0.0;
	float falling_time = 0.0;

	//Peak Current
	float peak_c = 0.0;

	unsigned int time;
	unsigned char i;

	unsigned char table_lenght = 0;

	//los punteros a las estructuras correspondientes (que voy a usar)
	warningup_coolingdown_typedef * p_table;
	session_typedef * p_session;

	//--- Primero debo saber que canal y que estado/escenario de sesion estoy calculando ---//
	switch (channel)
	{
		case 1:
			switch (session_stage)
			{
				case WARMING_UP:
					p_table = &table_warning_up_channel_1[0];
					break;

				case PLATEAU:
					p_table = &table_plateau_channel_1[0];
					break;

				case COOLING_DOWN:
					p_table = &table_cooling_down_channel_1[0];
					break;

				default:
					channel = 5;
					break;
			}
			p_session = &session_ch_1;
			break;

		case 2:
			switch (session_stage)
			{
				case WARMING_UP:
					p_table = &table_warning_up_channel_2[0];
					break;

				case PLATEAU:
					p_table = &table_plateau_channel_2[0];
					break;

				case COOLING_DOWN:
					p_table = &table_cooling_down_channel_2[0];
					break;

				default:
					channel = 5;
					break;
			}
			p_session = &session_ch_2;
			break;

		case 3:
			switch (session_stage)
			{
				case WARMING_UP:
					p_table = &table_warning_up_channel_3[0];
					break;

				case PLATEAU:
					p_table = &table_plateau_channel_3[0];
					break;

				case COOLING_DOWN:
					p_table = &table_cooling_down_channel_3[0];
					break;

				default:
					channel = 5;
					break;
			}
			p_session = &session_ch_3;
			break;

		case 4:
			switch (session_stage)
			{
				case WARMING_UP:
					p_table = &table_warning_up_channel_4[0];
					break;

				case PLATEAU:
					p_table = &table_plateau_channel_4[0];
					break;

				case COOLING_DOWN:
					p_table = &table_cooling_down_channel_4[0];
					break;

				default:
					channel = 5;
					break;
			}
			p_session = &session_ch_4;
			break;

		default:
			channel = 5;
			break;
	}

	if (channel == 5)
		return FIN_ERROR;

	//--- Limpieza de las señales ---//
	if (session_stage == PLATEAU)
		table_lenght = 1;
	else
		table_lenght = SESSION_WUP_CH1_BUFF_DIM;

	//--- Rising ---//
	for (i = 0; i < table_lenght; i++)
	{
		(p_table + i)->rising_pwm_200_final = 0;
		(p_table + i)->rising_pwm_200_initial = 0;
		(p_table + i)->rising_pwm_40_final = 0;
		(p_table + i)->rising_pwm_40_initial = 0;
		(p_table + i)->rising_pwm_n_final = 0;
		(p_table + i)->rising_pwm_n_initial = 0;
		(p_table + i)->rising_step_number = 0;
	}

	//--- Maintenance ---//
	p_table->maintenance_pwm_200 = 0;
	p_table->maintenance_pwm_40 = 0;
	p_table->maintenance_pwm_n = 0;
	p_table->maintenance_step_number = 0;

	//--- Falling ---//
	for (i = 0; i < table_lenght; i++)
	{
		(p_table + i)->falling_pwm_200_final = 0;
		(p_table + i)->falling_pwm_200_initial = 0;
		(p_table + i)->falling_pwm_40_final = 0;
		(p_table + i)->falling_pwm_40_initial = 0;
		(p_table + i)->falling_pwm_n_final = 0;
		(p_table + i)->falling_pwm_n_initial = 0;
		(p_table + i)->falling_step_number = 0;
		(p_table + i)->falling_type = 0;
	}

	//Resistance.
	resistance = (float)p_session->stage_1_resistance_dec;
	resistance /= 100;
	resistance += (float)p_session->stage_1_resistance_int;

	//Inductance.
	inductance = (float)p_session->stage_1_inductance_dec;
	inductance /= 100;
	inductance += (float)p_session->stage_1_inductance_int;

	//Current limit.
	current_limit = (float)p_session->stage_1_current_limit_dec;
	current_limit /= 100;
	current_limit += (float)p_session->stage_1_current_limit_int;

	//tengo 196mV / A + offset 456mV
	peak_c = (current_limit * 1.5) * 0.196 + 0.46;		//convierto corriente max a tensión con 50% de margen
	peak_c = peak_c * 0.303;	//divido 3.3V
	peak_c = peak_c * 4095;		//valor pico permitido en ADC

	p_session->peak_current_limit = (unsigned short) peak_c;

	//TODO: OJO ESTOS DOS VER QUE PASA EN MAINTENANCE

	//Initial current & Final current
	initial_current = (float)current_limit / 100;
	final_current = (float)current_limit / 100;

	switch (session_stage)
	{
		case WARMING_UP:
			initial_current *= p_session->stage_1_initial_power;
			final_current *= p_session->stage_1_final_power;

			//Time of each power step
			time = p_session->stage_1_time_hours * 3600 + p_session->stage_1_time_minutes * 60 + p_session->stage_1_time_seconds;
			time *= 10000; //Considerando 100uS.
			time /= SESSION_WUP_CH1_BUFF_DIM;
			p_session->stage_1_time_per_step = (unsigned int) time;
			break;

		case PLATEAU:
			initial_current *= p_session->stage_2_initial_power;
			final_current *= p_session->stage_2_final_power;		//Plateau no usa final_current
			break;

		case COOLING_DOWN:
			initial_current *= p_session->stage_3_initial_power;
			final_current *= p_session->stage_3_final_power;

			//Time of each power step
			time = p_session->stage_3_time_hours * 3600 + p_session->stage_3_time_minutes * 60 + p_session->stage_3_time_seconds;
			time *= 10000; //Considerando 100uS.
			time /= SESSION_WUP_CH1_BUFF_DIM;
			p_session->stage_3_time_per_step = (unsigned int) time;
			break;
	}

	for (i = 0; i < table_lenght; i++)
	{
		//--- Rising ---//
		//--- Nivel de señal ---//
		current = (float) final_current - (float) initial_current;
		//current /= SESSION_WUP_CH1_BUFF_DIM;
		current /= table_lenght;
		current *= i;
		current += initial_current;

		//Duty.
		//Minimum voltage.
		voltage = current;
		voltage *= (float)inductance;
		switch(session_stage)
		{
			case WARMING_UP:
				voltage /= (float)p_session->stage_1_rising_time;
				break;

			case PLATEAU:
				voltage /= (float)p_session->stage_2_rising_time;
				break;

			case COOLING_DOWN:
				voltage /= (float)p_session->stage_3_rising_time;
				break;
		}


		if (voltage > 200)
			return FIN_ERROR;
		//Maximum voltage.
		voltage2 = voltage + (float)(current * resistance);

		if (voltage2 > 200)
			return FIN_ERROR;

		if (voltage2 < 40)
		{
			//Initial voltage.
			auxiliar_duty = (float)voltage * 25;
			(p_table + i)->rising_pwm_40_initial = (unsigned short)auxiliar_duty;

			//Final voltage.
			auxiliar_duty = (float)voltage2 * 25; //*1000/40.
			(p_table + i)->rising_pwm_40_final = (unsigned short)auxiliar_duty;

			//Steps.
			switch(session_stage)
			{
				case WARMING_UP:
					auxiliar_duty = (float)p_session->stage_1_rising_time * 10;
					break;

				case PLATEAU:
					auxiliar_duty = (float)p_session->stage_2_rising_time * 10;
					break;

				case COOLING_DOWN:
					auxiliar_duty = (float)p_session->stage_3_rising_time * 10;
					break;
			}
			(p_table + i)->rising_step_number = auxiliar_duty;
		}
		else if ((voltage2 >= 40) && (voltage2 < 200))
		{
			//Initial voltage.
			auxiliar_duty = (float)voltage * 5;
			(p_table + i)->rising_pwm_200_initial = (unsigned short)auxiliar_duty;

			//Final voltage.
			auxiliar_duty = (float)voltage2 * 5; //*1000/200.
			(p_table + i)->rising_pwm_200_final = (unsigned short)auxiliar_duty;

			//Steps.
			switch(session_stage)
			{
				case WARMING_UP:
					auxiliar_duty = (float)p_session->stage_1_rising_time * 10;
					break;

				case PLATEAU:
					auxiliar_duty = (float)p_session->stage_2_rising_time * 10;
					break;

				case COOLING_DOWN:
					auxiliar_duty = (float)p_session->stage_3_rising_time * 10;
					break;
			}
			(p_table + i)->rising_step_number = auxiliar_duty;
		}
		else
			return FIN_ERROR;

		//--- Maintenance ---//
		voltage = resistance;
		voltage *= (float)current;

		switch(session_stage)
		{
			case WARMING_UP:
				(p_table + i)->maintenance_step_number = (float) p_session->stage_1_maintenance_time * 10;
				break;

			case PLATEAU:
				(p_table + i)->maintenance_step_number = (float) p_session->stage_2_maintenance_time * 10;
				break;

			case COOLING_DOWN:
				(p_table + i)->maintenance_step_number = (float) p_session->stage_3_maintenance_time * 10;
				break;
		}

		if (voltage > 40)
			return FIN_ERROR;

		if (voltage <= 40)
		{
			auxiliar_duty = (float)voltage * 25;
			(p_table + i)->maintenance_pwm_40 = (unsigned short)auxiliar_duty;
		}
		else if ((voltage > 40) && (voltage < 200))
		{
			auxiliar_duty = (float)voltage * 5;
			(p_table + i)->maintenance_pwm_200 = (unsigned short)auxiliar_duty;
		}
		else
			return FIN_ERROR;

		//--- Falling ---//
		//Tau.
		//cuentas nuevas
		LR_tau = (float) (inductance / (1000 * resistance));	//no tocar
		switch(session_stage)
		{
			case WARMING_UP:
				falling_time = p_session->stage_1_rising_time + p_session->stage_1_maintenance_time + p_session->stage_1_falling_time + p_session->stage_1_low_time;
				break;

			case PLATEAU:
				falling_time = p_session->stage_2_rising_time + p_session->stage_2_maintenance_time + p_session->stage_2_falling_time + p_session->stage_2_low_time;
				break;

			case COOLING_DOWN:
				falling_time = p_session->stage_3_rising_time + p_session->stage_3_maintenance_time + p_session->stage_3_falling_time + p_session->stage_3_low_time;
				break;
		}

		Vsnubber = (float) 0.5 * inductance * final_current * final_current * resistance_discharge / falling_time;
		Vsnubber = sqrt(Vsnubber);

		//corrijo snubber en Ipeak * Rserie
		Vsnubber += final_current * resistance;

		//Primer cruce por cero.
		Td = 1.7175;
		Td *= 1 - final_current * resistance / Vsnubber;
		Td -= 1.597;
		Td *= -LR_tau;

		//fin cuentas nuevas

		//Nuevo programa bajada
		Td *= 1000;
		Td += 0.25;

		switch(session_stage)
		{
			case WARMING_UP:
				//Slow discharge.
				if (p_session->stage_1_falling_time > (1000 * LR_tau))	//desde 1 tau
				{
					(p_table + i)->falling_time = p_session->stage_1_falling_time;
					(p_table + i)->falling_type = FALLING_SLOW_DISCHARGE;

					voltage =  resistance - ((float)inductance / p_session->stage_1_falling_time);
					voltage *= current;

					if (voltage < 0)
						voltage = 0;

					if (voltage > 200)
						return FIN_ERROR;

					voltage2 = (float) resistance * current;
					voltage2 = voltage - voltage2;

					if (voltage2 < 0)
						voltage2 = 0;

					if (voltage2 > 200)
						return FIN_ERROR;

					if (voltage2 < 40)
					{
						auxiliar_duty = (float)voltage * 25;
						(p_table + i)->falling_pwm_40_initial = (unsigned short)auxiliar_duty;

						auxiliar_duty = (float)voltage2 * 25; //*1000/40.
						(p_table + i)->falling_pwm_40_final = (unsigned short)auxiliar_duty;

						//Steps.
						auxiliar_duty = (float)(p_session->stage_1_falling_time + 0) * 10;
						(p_table + i)->falling_step_number = auxiliar_duty;

					}
					else if ((voltage2 >= 40) && (voltage2 < 200))
					{
						auxiliar_duty = (float)voltage * 5;
						(p_table + i)->falling_pwm_40_initial = (unsigned short)auxiliar_duty;

						auxiliar_duty = (float)voltage2 * 5; //*1000/200.
						(p_table + i)->falling_pwm_40_final = (unsigned short)auxiliar_duty;

						//Steps.
						auxiliar_duty = (float)(p_session->stage_1_falling_time + 0) * 10;
						(p_table + i)->falling_step_number = auxiliar_duty;
					}
					else
						return FIN_ERROR;
				}
				//LR discharge.
				else if (p_session->stage_1_falling_time > (Td * 2))	//es myor que 2 veces la descarga rapida
				{
					(p_table + i)->falling_time = p_session->stage_1_falling_time;
					auxiliar_duty = (float)(p_table + i)->falling_time * 10;
					(p_table + i)->falling_step_number = auxiliar_duty;
					(p_table + i)->falling_type = FALLING_LR;
				}
				//Fast discharge.
				else
		 		{
					(p_table + i)->falling_pwm_200_final = 0;
					(p_table + i)->falling_pwm_200_initial = 0;
					(p_table + i)->falling_pwm_40_final = 0;
					(p_table + i)->falling_pwm_40_initial = 0;
					(p_table + i)->falling_pwm_n_final = 1000;
					(p_table + i)->falling_pwm_n_initial = 1000;
					(p_table + i)->falling_time = (unsigned short)Td; //mS.
					(p_table + i)->falling_type = FALLING_FAST_DISCHARGE;

					auxiliar_duty = (float) Td * 10;
					(p_table + i)->falling_step_number = auxiliar_duty;
				}
				//Fin nuevo programa bajada

				//--- LOW ---//
				(p_table + i)->low_step_number = p_session->stage_1_low_time * 10;
				(p_table + i)->burst_mode_off = p_session->stage_1_burst_mode_off;
				(p_table + i)->burst_mode_on = p_session->stage_1_burst_mode_on;

				break;

			case PLATEAU:
				//Slow discharge.
				if (p_session->stage_2_falling_time > (1000 * LR_tau))	//desde 1 tau
				{
					(p_table + i)->falling_time = p_session->stage_2_falling_time;
					(p_table + i)->falling_type = FALLING_SLOW_DISCHARGE;

					voltage =  resistance - ((float)inductance / p_session->stage_2_falling_time);
					voltage *= current;

					if (voltage < 0)
						voltage = 0;

					if (voltage > 200)
						return FIN_ERROR;

					voltage2 = (float) resistance * current;
					voltage2 = voltage - voltage2;

					if (voltage2 < 0)
						voltage2 = 0;

					if (voltage2 > 200)
						return FIN_ERROR;

					if (voltage2 < 40)
					{
						auxiliar_duty = (float)voltage * 25;
						(p_table + i)->falling_pwm_40_initial = (unsigned short)auxiliar_duty;

						auxiliar_duty = (float)voltage2 * 25; //*1000/40.
						(p_table + i)->falling_pwm_40_final = (unsigned short)auxiliar_duty;

						//Steps.
						auxiliar_duty = (float)(p_session->stage_2_falling_time + 0) * 10;
						(p_table + i)->falling_step_number = auxiliar_duty;

					}
					else if ((voltage2 >= 40) && (voltage2 < 200))
					{
						auxiliar_duty = (float)voltage * 5;
						(p_table + i)->falling_pwm_40_initial = (unsigned short)auxiliar_duty;

						auxiliar_duty = (float)voltage2 * 5; //*1000/200.
						(p_table + i)->falling_pwm_40_final = (unsigned short)auxiliar_duty;

						//Steps.
						auxiliar_duty = (float)(p_session->stage_2_falling_time + 0) * 10;
						(p_table + i)->falling_step_number = auxiliar_duty;
					}
					else
						return FIN_ERROR;
				}
				//LR discharge.
				else if (p_session->stage_2_falling_time > (Td * 2))	//es myor que 2 veces la descarga rapida
				{
					(p_table + i)->falling_time = p_session->stage_2_falling_time;
					auxiliar_duty = (float)(p_table + i)->falling_time * 10;
					(p_table + i)->falling_step_number = auxiliar_duty;
					(p_table + i)->falling_type = FALLING_LR;
				}
				//Fast discharge.
				else
		 		{
					(p_table + i)->falling_pwm_200_final = 0;
					(p_table + i)->falling_pwm_200_initial = 0;
					(p_table + i)->falling_pwm_40_final = 0;
					(p_table + i)->falling_pwm_40_initial = 0;
					(p_table + i)->falling_pwm_n_final = 1000;
					(p_table + i)->falling_pwm_n_initial = 1000;
					(p_table + i)->falling_time = (unsigned short)Td; //mS.
					(p_table + i)->falling_type = FALLING_FAST_DISCHARGE;

					auxiliar_duty = (float) Td * 10;
					(p_table + i)->falling_step_number = auxiliar_duty;
				}
				//Fin nuevo programa bajada

				//--- LOW ---//
				(p_table + i)->low_step_number = p_session->stage_2_low_time * 10;
				(p_table + i)->burst_mode_off = p_session->stage_2_burst_mode_off;
				(p_table + i)->burst_mode_on = p_session->stage_2_burst_mode_on;

				break;

			case COOLING_DOWN:
				//Slow discharge.
				if (p_session->stage_3_falling_time > (1000 * LR_tau))	//desde 1 tau
				{
					(p_table + i)->falling_time = p_session->stage_3_falling_time;
					(p_table + i)->falling_type = FALLING_SLOW_DISCHARGE;

					voltage =  resistance - ((float)inductance / p_session->stage_3_falling_time);
					voltage *= current;

					if (voltage < 0)
						voltage = 0;

					if (voltage > 200)
						return FIN_ERROR;

					voltage2 = (float) resistance * current;
					voltage2 = voltage - voltage2;

					if (voltage2 < 0)
						voltage2 = 0;

					if (voltage2 > 200)
						return FIN_ERROR;

					if (voltage2 < 40)
					{
						auxiliar_duty = (float)voltage * 25;
						(p_table + i)->falling_pwm_40_initial = (unsigned short)auxiliar_duty;

						auxiliar_duty = (float)voltage2 * 25; //*1000/40.
						(p_table + i)->falling_pwm_40_final = (unsigned short)auxiliar_duty;

						//Steps.
						auxiliar_duty = (float)(p_session->stage_3_falling_time + 0) * 10;
						(p_table + i)->falling_step_number = auxiliar_duty;

					}
					else if ((voltage2 >= 40) && (voltage2 < 200))
					{
						auxiliar_duty = (float)voltage * 5;
						(p_table + i)->falling_pwm_40_initial = (unsigned short)auxiliar_duty;

						auxiliar_duty = (float)voltage2 * 5; //*1000/200.
						(p_table + i)->falling_pwm_40_final = (unsigned short)auxiliar_duty;

						//Steps.
						auxiliar_duty = (float)(p_session->stage_3_falling_time + 0) * 10;
						(p_table + i)->falling_step_number = auxiliar_duty;
					}
					else
						return FIN_ERROR;
				}
				//LR discharge.
				else if (p_session->stage_3_falling_time > (Td * 2))	//es myor que 2 veces la descarga rapida
				{
					(p_table + i)->falling_time = p_session->stage_3_falling_time;
					auxiliar_duty = (float)(p_table + i)->falling_time * 10;
					(p_table + i)->falling_step_number = auxiliar_duty;
					(p_table + i)->falling_type = FALLING_LR;
				}
				//Fast discharge.
				else
		 		{
					(p_table + i)->falling_pwm_200_final = 0;
					(p_table + i)->falling_pwm_200_initial = 0;
					(p_table + i)->falling_pwm_40_final = 0;
					(p_table + i)->falling_pwm_40_initial = 0;
					(p_table + i)->falling_pwm_n_final = 1000;
					(p_table + i)->falling_pwm_n_initial = 1000;
					(p_table + i)->falling_time = (unsigned short)Td; //mS.
					(p_table + i)->falling_type = FALLING_FAST_DISCHARGE;

					auxiliar_duty = (float) Td * 10;
					(p_table + i)->falling_step_number = auxiliar_duty;
				}
				//Fin nuevo programa bajada

				//--- LOW ---//
				(p_table + i)->low_step_number = p_session->stage_3_low_time * 10;
				(p_table + i)->burst_mode_off = p_session->stage_3_burst_mode_off;
				(p_table + i)->burst_mode_on = p_session->stage_3_burst_mode_on;

				break;
		}

	}

	//--- end ---//
	return FIN_OK;
}

//------ NUEVA WARMING UP ------//
//esta rutina calcula los parametros y luego dibuja la señal con los PWM en cada canal
//para el stage de Warming UP
//ademas va cambiando los niveles de señal generados
unsigned char Session_Warming_Up_Channels (unsigned char channel)
{

	unsigned char i;
	unsigned short time_aux = 0;
	unsigned char * p_session_state;
	unsigned char * p_session_channel_step;
	session_typedef * p_session_ch;
	unsigned short * p_session_burst_cnt;
	warningup_coolingdown_typedef * p_table;
	float * p_pwm_slope;
	float * p_pwm_ch;
	volatile unsigned int * p_stage_time;
	unsigned short * p_session_time;
	unsigned short * p_session_time_2;
	unsigned char * p_fall_type;

	//primero acomodo los punteros
	switch (channel)
	{
		case CH1:
			p_session_state = &session_warning_up_channel_1_state;
			p_session_channel_step = &session_warning_up_channel_1_step;
			p_session_ch = &session_ch_1;
			p_session_burst_cnt = &session_warning_up_channel_1_burst_cnt;
			p_table = &table_warning_up_channel_1[0];
			p_pwm_slope = &pwm_slope_channel_1;
			p_stage_time = &session_warning_up_channel_1_stage_time;	//lo actualiza cada 100us TIM5 (decrementa)
			p_session_time = &session_warning_up_channel_1_time;		//lo actualiza cada 100us TIM5 (incrementa)
			p_session_time_2 = &session_warning_up_channel_1_time_2;
			p_pwm_ch = &pwm_channel_1;
			p_fall_type = &fall_type_step_ch1;
			break;

		case CH2:
			p_session_state = &session_warning_up_channel_2_state;
			p_session_channel_step = &session_warning_up_channel_2_step;
			p_session_ch = &session_ch_2;
			p_session_burst_cnt = &session_warning_up_channel_2_burst_cnt;
			p_table = &table_warning_up_channel_2[0];
			p_pwm_slope = &pwm_slope_channel_2;
			p_stage_time = &session_warning_up_channel_2_stage_time;
			p_session_time = &session_warning_up_channel_2_time;
			p_session_time_2 = &session_warning_up_channel_2_time_2;
			p_pwm_ch = &pwm_channel_2;
			p_fall_type = &fall_type_step_ch2;
			break;

		case CH3:
			p_session_state = &session_warning_up_channel_3_state;
			p_session_channel_step = &session_warning_up_channel_3_step;
			p_session_ch = &session_ch_3;
			p_session_burst_cnt = &session_warning_up_channel_3_burst_cnt;
			p_table = &table_warning_up_channel_3[0];
			p_pwm_slope = &pwm_slope_channel_3;
			p_stage_time = &session_warning_up_channel_3_stage_time;
			p_session_time = &session_warning_up_channel_3_time;
			p_session_time_2 = &session_warning_up_channel_3_time_2;
			p_pwm_ch = &pwm_channel_3;
			p_fall_type = &fall_type_step_ch3;
			break;

		case CH4:
			p_session_state = &session_warning_up_channel_4_state;
			p_session_channel_step = &session_warning_up_channel_4_step;
			p_session_ch = &session_ch_4;
			p_session_burst_cnt = &session_warning_up_channel_4_burst_cnt;
			p_table = &table_warning_up_channel_4[0];
			p_pwm_slope = &pwm_slope_channel_4;
			p_stage_time = &session_warning_up_channel_4_stage_time;
			p_session_time = &session_warning_up_channel_4_time;
			p_session_time_2 = &session_warning_up_channel_4_time_2;
			p_pwm_ch = &pwm_channel_4;
			p_fall_type = &fall_type_step_ch4;
			break;

		default:
			return FIN_ERROR;
			break;
	}

	switch (*p_session_state)
	{
		case SESSION_WARMING_UP_CHANNEL_INIT:

			*p_session_channel_step = 0;

			if (p_session_ch->stage_1_status)
			{
#ifdef WITH_SYNC
				//reviso si tengo que utilizar SYNC
				if (p_session_ch->sync_on)
				{
					//debo tener canal 1 presente
					if (channel == CH1)
					{
						ch1_sync_state |= SYNC_REQUIRED;
						*p_session_state = SESSION_WARMING_UP_CHANNEL_PARAMETERS_CALCULATE;
					}
					else
					{
						//reviso que este presente CH1 y que no se hy agotado su timer
						if (Session_Channel_1_Verify_Antenna_time != 0)
						{
							if (ch1_sync_state & SYNC_REQUIRED)
							{
								//CH1 presente
								*p_session_state = SESSION_WARMING_UP_CHANNEL_PARAMETERS_CALCULATE;
								sync_in_waiting = 0;
							}
							else
							{
								if (sync_in_waiting)
								{
									sprintf(&buffSendErr[0], "\r\nCH%d waiting for sync", channel);	//TODO espero SYNC y no aparece CH1 salgo por timeout??
									UART_PC_Send(&buffSendErr[0]);
									sync_in_waiting = 1;
								}
							}
						}
						else
						{
							//tengo timeout de CH1
							*p_session_state = SESSION_WARMING_UP_CHANNEL_END_ERROR;

							sprintf(&buffSendErr[0],"ERROR in CH%d: not CH1 detected and sync is needed\r\n", channel);
							UART_PC_Send(&buffSendErr[0]);
							sync_in_waiting = 0;
							return FIN_ERROR;
						}
					}
				}
				else
				{
					sprintf(&buffSendErr[0], "CH%d no sync needed\r\n", channel);
					UART_PC_Send(&buffSendErr[0]);
					ch1_sync_state &= NO_SYNC_REQUIRED_MASK;
					*p_session_state = SESSION_WARMING_UP_CHANNEL_PARAMETERS_CALCULATE;
				}
#else
				*p_session_state = SESSION_WARMING_UP_CHANNEL_PARAMETERS_CALCULATE;
#endif
			}
			else
				return FIN_OK;
			break;

		case SESSION_WARMING_UP_CHANNEL_PARAMETERS_CALCULATE:

			i = Session_Channels_Parameters_Calculate(channel, WARMING_UP);	//retorna FIN_OK o FIN_ERROR rutina nueva 19-03-15

			if (i == FIN_OK)
			{
				*p_session_state = SESSION_WARMING_UP_CHANNEL_PARAMETERS_CALCULATE_END;
				SetBitGlobalErrors (channel, BIT_ERROR_PARAMS_FINISH);
			}

			else if (i == FIN_ERROR)
			{
				*p_session_state = SESSION_WARMING_UP_CHANNEL_END_ERROR;

				sprintf(&buffSendErr[0], (const char *) "ERROR(0x%03X)\r\n", ERR_CHANNEL_WARNING_UP_PARAMETERS_CALCULATE(channel));
				UART_PC_Send(&buffSendErr[0]);
			}
			break;

		case SESSION_WARMING_UP_CHANNEL_PARAMETERS_CALCULATE_END:
			//si todos los canales estan listos empiezo con las señales

			if (((global_error_ch1 & BIT_ERROR_PARAMS_FINISH) || (global_error_ch1 & BIT_ERROR_CHECK_MASK)) &&
					((global_error_ch2 & BIT_ERROR_PARAMS_FINISH) || (global_error_ch2 & BIT_ERROR_CHECK_MASK)) &&
					((global_error_ch3 & BIT_ERROR_PARAMS_FINISH) || (global_error_ch3 & BIT_ERROR_CHECK_MASK)) &&
					((global_error_ch4 & BIT_ERROR_PARAMS_FINISH) || (global_error_ch4 & BIT_ERROR_CHECK_MASK)))
			{

				*p_session_burst_cnt = 0;

				//--- Slope ---//
				*p_pwm_slope = (p_table + (*p_session_channel_step))->rising_pwm_200_final + (p_table + (*p_session_channel_step))->rising_pwm_40_final - (p_table + (*p_session_channel_step))->rising_pwm_200_initial - (p_table + (*p_session_channel_step))->rising_pwm_40_initial;
				*p_pwm_slope /= (float)10;
				*p_pwm_slope /= p_session_ch->stage_1_rising_time;

				//--- Next state ---//
				*p_session_state = SESSION_WARMING_UP_CHANNEL_RISING_EDGE;

				//--- Time of the next step ---//
				*p_stage_time = p_session_ch->stage_1_time_per_step;

				*p_session_time = 0;
				*p_session_time_2 = 0;

				//--- Next step ---//
				*p_session_channel_step = 0;

				//--- PWM initial values ---//
				switch (channel)
				{
					case CH1:
						if (*p_session_burst_cnt < (p_table + (*p_session_channel_step))->burst_mode_on)
						{
							PWM_CH1_TiempoSubida((p_table + (*p_session_channel_step))->rising_pwm_200_initial); //pwm 200V.
							PWM_CH1_TiempoMantenimiento((p_table + (*p_session_channel_step))->rising_pwm_40_initial);
							PWM_CH1_TiempoBajada((p_table + (*p_session_channel_step))->rising_pwm_n_initial);
						}
						else
						{
							PWM_CH1_TiempoSubida(0); //pwm 200V.
							PWM_CH1_TiempoMantenimiento(0);
							PWM_CH1_TiempoBajada(0);
						}
						break;

					case CH2:
						if (*p_session_burst_cnt < (p_table + (*p_session_channel_step))->burst_mode_on)
						{
							PWM_CH2_TiempoSubida((p_table + (*p_session_channel_step))->rising_pwm_200_initial); //pwm 200V.
							PWM_CH2_TiempoMantenimiento((p_table + (*p_session_channel_step))->rising_pwm_40_initial);
							PWM_CH2_TiempoBajada((p_table + (*p_session_channel_step))->rising_pwm_n_initial);
						}
						else
						{
							PWM_CH2_TiempoSubida(0); //pwm 200V.
							PWM_CH2_TiempoMantenimiento(0);
							PWM_CH2_TiempoBajada(0);
						}
						break;

					case CH3:
						if (*p_session_burst_cnt < (p_table + (*p_session_channel_step))->burst_mode_on)
						{
							PWM_CH3_TiempoSubida((p_table + (*p_session_channel_step))->rising_pwm_200_initial); //pwm 200V.
							PWM_CH3_TiempoMantenimiento((p_table + (*p_session_channel_step))->rising_pwm_40_initial);
							PWM_CH3_TiempoBajada((p_table + (*p_session_channel_step))->rising_pwm_n_initial);
						}
						else
						{
							PWM_CH3_TiempoSubida(0); //pwm 200V.
							PWM_CH3_TiempoMantenimiento(0);
							PWM_CH3_TiempoBajada(0);
						}
						break;

					case CH4:
						if (*p_session_burst_cnt < (p_table + (*p_session_channel_step))->burst_mode_on)
						{
							PWM_CH4_TiempoSubida((p_table + (*p_session_channel_step))->rising_pwm_200_initial); //pwm 200V.
							PWM_CH4_TiempoMantenimiento((p_table + (*p_session_channel_step))->rising_pwm_40_initial);
							PWM_CH4_TiempoBajada((p_table + (*p_session_channel_step))->rising_pwm_n_initial);
						}
						else
						{
							PWM_CH4_TiempoSubida(0); //pwm 200V.
							PWM_CH4_TiempoMantenimiento(0);
							PWM_CH4_TiempoBajada(0);
						}
						break;
				}
			}
			break;

		case SESSION_WARMING_UP_CHANNEL_CHANGE_LEVEL:

			if (*p_stage_time == 0)		//TODO: esto no tine mucho sentido, se agoto el tiempo de change level???
			{							//o ser que el buffer tiene menos posiciones y pas muchas veces por aca
				if (*p_session_channel_step < (SESSION_WUP_CH1_BUFF_DIM - 1))
				{
					//--- Time per step ---//
					*p_stage_time = p_session_ch->stage_1_time_per_step;

					//--- Next state ---//
					*p_session_state = SESSION_WARMING_UP_CHANNEL_RISING_EDGE;

					//--- Next step ---//
					*p_session_channel_step += 1;

					//--- Slope ---//
					*p_pwm_slope = (p_table + (*p_session_channel_step))->rising_pwm_200_final + (p_table + (*p_session_channel_step))->rising_pwm_40_final - (p_table + (*p_session_channel_step))->rising_pwm_200_initial - (p_table + (*p_session_channel_step))->rising_pwm_40_initial;
					*p_pwm_slope /= (float)10;
					*p_pwm_slope /= p_session_ch->stage_1_rising_time;

					switch (channel)
					{
						case CH1:
							if ((*p_session_burst_cnt < (p_table + (*p_session_channel_step))->burst_mode_on) ||
									(	((p_table + (*p_session_channel_step))->burst_mode_on == 0) && ((p_table + (*p_session_channel_step))->burst_mode_off == 0)))
							{
								PWM_CH1_TiempoSubida((p_table + (*p_session_channel_step))->rising_pwm_200_initial); //pwm 200V.
								PWM_CH1_TiempoMantenimiento((p_table + (*p_session_channel_step))->rising_pwm_40_initial);
								PWM_CH1_TiempoBajada((p_table + (*p_session_channel_step))->rising_pwm_n_initial);
							}
							else
							{
								PWM_CH1_TiempoSubida(0); //pwm 200V.
								PWM_CH1_TiempoMantenimiento(0);
								PWM_CH1_TiempoBajada(0);
							}
							break;

						case CH2:
							if ((*p_session_burst_cnt < (p_table + (*p_session_channel_step))->burst_mode_on) ||
									(	((p_table + (*p_session_channel_step))->burst_mode_on == 0) && ((p_table + (*p_session_channel_step))->burst_mode_off == 0)))
							{
								PWM_CH2_TiempoSubida((p_table + (*p_session_channel_step))->rising_pwm_200_initial); //pwm 200V.
								PWM_CH2_TiempoMantenimiento((p_table + (*p_session_channel_step))->rising_pwm_40_initial);
								PWM_CH2_TiempoBajada((p_table + (*p_session_channel_step))->rising_pwm_n_initial);
							}
							else
							{
								PWM_CH2_TiempoSubida(0); //pwm 200V.
								PWM_CH2_TiempoMantenimiento(0);
								PWM_CH2_TiempoBajada(0);
							}
							break;

						case CH3:
							if ((*p_session_burst_cnt < (p_table + (*p_session_channel_step))->burst_mode_on) ||
									(	((p_table + (*p_session_channel_step))->burst_mode_on == 0) && ((p_table + (*p_session_channel_step))->burst_mode_off == 0)))
							{
								PWM_CH3_TiempoSubida((p_table + (*p_session_channel_step))->rising_pwm_200_initial); //pwm 200V.
								PWM_CH3_TiempoMantenimiento((p_table + (*p_session_channel_step))->rising_pwm_40_initial);
								PWM_CH3_TiempoBajada((p_table + (*p_session_channel_step))->rising_pwm_n_initial);
							}
							else
							{
								PWM_CH3_TiempoSubida(0); //pwm 200V.
								PWM_CH3_TiempoMantenimiento(0);
								PWM_CH3_TiempoBajada(0);
							}
							break;

						case CH4:
							if ((*p_session_burst_cnt < (p_table + (*p_session_channel_step))->burst_mode_on) ||
									(	((p_table + (*p_session_channel_step))->burst_mode_on == 0) && ((p_table + (*p_session_channel_step))->burst_mode_off == 0)))
							{
								PWM_CH4_TiempoSubida((p_table + (*p_session_channel_step))->rising_pwm_200_initial); //pwm 200V.
								PWM_CH4_TiempoMantenimiento((p_table + (*p_session_channel_step))->rising_pwm_40_initial);
								PWM_CH4_TiempoBajada((p_table + (*p_session_channel_step))->rising_pwm_n_initial);
							}
							else
							{
								PWM_CH4_TiempoSubida(0); //pwm 200V.
								PWM_CH4_TiempoMantenimiento(0);
								PWM_CH4_TiempoBajada(0);
							}
							break;
					}

					*p_session_time = 0;
					*p_session_time_2 = 0;

				}
				else
				{
					//termino el stage, aviso a los canales
					if ((channel == CH1) && (ch1_sync_state & SYNC_REQUIRED))
					{
						ch1_sync_state &= SYNC_REQUIRED;
						ch1_sync_state |= SYNC_FINISH_WARMING_UP;
					}
					return FIN_OK;
				}
			}
			else
			{
				//--- Next state ---//
				*p_session_state = SESSION_WARMING_UP_CHANNEL_RISING_EDGE;

				*p_pwm_slope = (p_table + (*p_session_channel_step))->rising_pwm_200_final + (p_table + (*p_session_channel_step))->rising_pwm_40_final - (p_table + (*p_session_channel_step))->rising_pwm_200_initial - (p_table + (*p_session_channel_step))->rising_pwm_40_initial;
				*p_pwm_slope /= (float)10;
				*p_pwm_slope /= p_session_ch->stage_1_rising_time;

				switch (channel)
				{
					case CH1:
						if ((*p_session_burst_cnt < (p_table + (*p_session_channel_step))->burst_mode_on) ||
								(	((p_table + (*p_session_channel_step))->burst_mode_on == 0) && ((p_table + (*p_session_channel_step))->burst_mode_off == 0)))
						{
							PWM_CH1_TiempoSubida((p_table + (*p_session_channel_step))->rising_pwm_200_initial); //pwm 200V.
							PWM_CH1_TiempoMantenimiento((p_table + (*p_session_channel_step))->rising_pwm_40_initial);
							PWM_CH1_TiempoBajada((p_table + (*p_session_channel_step))->rising_pwm_n_initial);
						}
						else
						{
							PWM_CH1_TiempoSubida(0); //pwm 200V.
							PWM_CH1_TiempoMantenimiento(0);
							PWM_CH1_TiempoBajada(0);
						}
						break;

					case CH2:
						if ((*p_session_burst_cnt < (p_table + (*p_session_channel_step))->burst_mode_on) ||
								(	((p_table + (*p_session_channel_step))->burst_mode_on == 0) && ((p_table + (*p_session_channel_step))->burst_mode_off == 0)))
						{
							PWM_CH2_TiempoSubida((p_table + (*p_session_channel_step))->rising_pwm_200_initial); //pwm 200V.
							PWM_CH2_TiempoMantenimiento((p_table + (*p_session_channel_step))->rising_pwm_40_initial);
							PWM_CH2_TiempoBajada((p_table + (*p_session_channel_step))->rising_pwm_n_initial);
						}
						else
						{
							PWM_CH2_TiempoSubida(0); //pwm 200V.
							PWM_CH2_TiempoMantenimiento(0);
							PWM_CH2_TiempoBajada(0);
						}
						break;

					case CH3:
						if ((*p_session_burst_cnt < (p_table + (*p_session_channel_step))->burst_mode_on) ||
								(	((p_table + (*p_session_channel_step))->burst_mode_on == 0) && ((p_table + (*p_session_channel_step))->burst_mode_off == 0)))
						{
							PWM_CH3_TiempoSubida((p_table + (*p_session_channel_step))->rising_pwm_200_initial); //pwm 200V.
							PWM_CH3_TiempoMantenimiento((p_table + (*p_session_channel_step))->rising_pwm_40_initial);
							PWM_CH3_TiempoBajada((p_table + (*p_session_channel_step))->rising_pwm_n_initial);
						}
						else
						{
							PWM_CH3_TiempoSubida(0); //pwm 200V.
							PWM_CH3_TiempoMantenimiento(0);
							PWM_CH3_TiempoBajada(0);
						}
						break;

					case CH4:
						if ((*p_session_burst_cnt < (p_table + (*p_session_channel_step))->burst_mode_on) ||
								(	((p_table + (*p_session_channel_step))->burst_mode_on == 0) && ((p_table + (*p_session_channel_step))->burst_mode_off == 0)))
						{
							PWM_CH4_TiempoSubida((p_table + (*p_session_channel_step))->rising_pwm_200_initial); //pwm 200V.
							PWM_CH4_TiempoMantenimiento((p_table + (*p_session_channel_step))->rising_pwm_40_initial);
							PWM_CH4_TiempoBajada((p_table + (*p_session_channel_step))->rising_pwm_n_initial);
						}
						else
						{
							PWM_CH4_TiempoSubida(0); //pwm 200V.
							PWM_CH4_TiempoMantenimiento(0);
							PWM_CH4_TiempoBajada(0);
						}
						break;
				}

				*p_session_time = 0;
				*p_session_time_2 = 0;

			}

			break;

		case SESSION_WARMING_UP_CHANNEL_RISING_EDGE:
#ifdef WITH_SYNC
			if ((channel == CH1) && (ch1_sync_state & SYNC_REQUIRED))  //modificacion no arranca sin CH1 5-9-16
			{
				ch1_sync_state &= ~SYNC_ACT_MASK;
				ch1_sync_state |= SYNC_IN_RISING;
			}
#endif

			if (*p_session_time <= (p_table + (*p_session_channel_step))->rising_step_number)
			{
				if (*p_session_time_2 != *p_session_time)		//para no ejecutar demasiado seguido
				{
					*p_session_time_2 = *p_session_time;

					*p_pwm_ch = (p_table + (*p_session_channel_step))->rising_pwm_200_initial + (p_table + (*p_session_channel_step))->rising_pwm_40_initial + *p_pwm_slope * *p_session_time;

					switch (channel)
					{
						case CH1:
							if ((*p_session_burst_cnt < (p_table + (*p_session_channel_step))->burst_mode_on) ||
									(	((p_table + (*p_session_channel_step))->burst_mode_on == 0) && ((p_table + (*p_session_channel_step))->burst_mode_off == 0)))
							{
								if (((p_table + (*p_session_channel_step))->rising_pwm_200_initial + (p_table + (*p_session_channel_step))->rising_pwm_200_final) != 0)
									PWM_CH1_TiempoSubida(*p_pwm_ch); //pwm 200V.
								else if (((p_table + (*p_session_channel_step))->rising_pwm_40_initial + (p_table + (*p_session_channel_step))->rising_pwm_40_final) != 0)
									PWM_CH1_TiempoMantenimiento(*p_pwm_ch); //pwm 40V.
							}
							else
							{
								PWM_CH1_TiempoSubida(0); //pwm 200V.
								PWM_CH1_TiempoMantenimiento(0);
								PWM_CH1_TiempoBajada(0);
							}
							break;

						case CH2:
							if ((*p_session_burst_cnt < (p_table + (*p_session_channel_step))->burst_mode_on) ||
									(	((p_table + (*p_session_channel_step))->burst_mode_on == 0) && ((p_table + (*p_session_channel_step))->burst_mode_off == 0)))
							{
								if (((p_table + (*p_session_channel_step))->rising_pwm_200_initial + (p_table + (*p_session_channel_step))->rising_pwm_200_final) != 0)
									PWM_CH2_TiempoSubida(*p_pwm_ch); //pwm 200V.
								else if (((p_table + (*p_session_channel_step))->rising_pwm_40_initial + (p_table + (*p_session_channel_step))->rising_pwm_40_final) != 0)
									PWM_CH2_TiempoMantenimiento(*p_pwm_ch); //pwm 40V.
							}
							else
							{
								PWM_CH2_TiempoSubida(0); //pwm 200V.
								PWM_CH2_TiempoMantenimiento(0);
								PWM_CH2_TiempoBajada(0);
							}
							break;

						case CH3:
							if ((*p_session_burst_cnt < (p_table + (*p_session_channel_step))->burst_mode_on) ||
									(	((p_table + (*p_session_channel_step))->burst_mode_on == 0) && ((p_table + (*p_session_channel_step))->burst_mode_off == 0)))
							{
								if (((p_table + (*p_session_channel_step))->rising_pwm_200_initial + (p_table + (*p_session_channel_step))->rising_pwm_200_final) != 0)
									PWM_CH3_TiempoSubida(*p_pwm_ch); //pwm 200V.
								else if (((p_table + (*p_session_channel_step))->rising_pwm_40_initial + (p_table + (*p_session_channel_step))->rising_pwm_40_final) != 0)
									PWM_CH3_TiempoMantenimiento(*p_pwm_ch); //pwm 40V.
							}
							else
							{
								PWM_CH3_TiempoSubida(0); //pwm 200V.
								PWM_CH3_TiempoMantenimiento(0);
								PWM_CH3_TiempoBajada(0);
							}
							break;

						case CH4:
							if ((*p_session_burst_cnt < (p_table + (*p_session_channel_step))->burst_mode_on) ||
									(	((p_table + (*p_session_channel_step))->burst_mode_on == 0) && ((p_table + (*p_session_channel_step))->burst_mode_off == 0)))
							{
								if (((p_table + (*p_session_channel_step))->rising_pwm_200_initial + (p_table + (*p_session_channel_step))->rising_pwm_200_final) != 0)
									PWM_CH4_TiempoSubida(*p_pwm_ch); //pwm 200V.
								else if (((p_table + (*p_session_channel_step))->rising_pwm_40_initial + (p_table + (*p_session_channel_step))->rising_pwm_40_final) != 0)
									PWM_CH4_TiempoMantenimiento(*p_pwm_ch); //pwm 40V.
							}
							else
							{
								PWM_CH4_TiempoSubida(0); //pwm 200V.
								PWM_CH4_TiempoMantenimiento(0);
								PWM_CH4_TiempoBajada(0);
							}
							break;
					}

				}
			}
			else
			{
				//--- Next state ---//
				*p_session_state = SESSION_WARMING_UP_CHANNEL_MAINTENANCE;
				*p_session_time = 0;
				*p_session_time_2 = 0;

				//--- PWM initial values ---//
				switch (channel)
				{
					case CH1:
						if ((*p_session_burst_cnt < (p_table + (*p_session_channel_step))->burst_mode_on) ||
								(	((p_table + (*p_session_channel_step))->burst_mode_on == 0) && ((p_table + (*p_session_channel_step))->burst_mode_off == 0)))
						{
							PWM_CH1_TiempoSubida((p_table + (*p_session_channel_step))->maintenance_pwm_200); //pwm 200V.
							PWM_CH1_TiempoMantenimiento((p_table + (*p_session_channel_step))->maintenance_pwm_40); //pwm 40V.
							PWM_CH1_TiempoBajada((p_table + (*p_session_channel_step))->maintenance_pwm_n);
						}
						else
						{
							PWM_CH1_TiempoSubida(0); //pwm 200V.
							PWM_CH1_TiempoMantenimiento(0);
							PWM_CH1_TiempoBajada(0);
						}
						break;

					case CH2:
						if ((*p_session_burst_cnt < (p_table + (*p_session_channel_step))->burst_mode_on) ||
								(	((p_table + (*p_session_channel_step))->burst_mode_on == 0) && ((p_table + (*p_session_channel_step))->burst_mode_off == 0)))
						{
							PWM_CH2_TiempoSubida((p_table + (*p_session_channel_step))->maintenance_pwm_200); //pwm 200V.
							PWM_CH2_TiempoMantenimiento((p_table + (*p_session_channel_step))->maintenance_pwm_40); //pwm 40V.
							PWM_CH2_TiempoBajada((p_table + (*p_session_channel_step))->maintenance_pwm_n);
						}
						else
						{
							PWM_CH2_TiempoSubida(0); //pwm 200V.
							PWM_CH2_TiempoMantenimiento(0);
							PWM_CH2_TiempoBajada(0);
						}
						break;

					case CH3:
						if ((*p_session_burst_cnt < (p_table + (*p_session_channel_step))->burst_mode_on) ||
								(	((p_table + (*p_session_channel_step))->burst_mode_on == 0) && ((p_table + (*p_session_channel_step))->burst_mode_off == 0)))
						{
							PWM_CH3_TiempoSubida((p_table + (*p_session_channel_step))->maintenance_pwm_200); //pwm 200V.
							PWM_CH3_TiempoMantenimiento((p_table + (*p_session_channel_step))->maintenance_pwm_40); //pwm 40V.
							PWM_CH3_TiempoBajada((p_table + (*p_session_channel_step))->maintenance_pwm_n);
						}
						else
						{
							PWM_CH3_TiempoSubida(0); //pwm 200V.
							PWM_CH3_TiempoMantenimiento(0);
							PWM_CH3_TiempoBajada(0);
						}
						break;

					case CH4:
						if ((*p_session_burst_cnt < (p_table + (*p_session_channel_step))->burst_mode_on) ||
								(	((p_table + (*p_session_channel_step))->burst_mode_on == 0) && ((p_table + (*p_session_channel_step))->burst_mode_off == 0)))
						{
							PWM_CH4_TiempoSubida((p_table + (*p_session_channel_step))->maintenance_pwm_200); //pwm 200V.
							PWM_CH4_TiempoMantenimiento((p_table + (*p_session_channel_step))->maintenance_pwm_40); //pwm 40V.
							PWM_CH4_TiempoBajada((p_table + (*p_session_channel_step))->maintenance_pwm_n);
						}
						else
						{
							PWM_CH4_TiempoSubida(0); //pwm 200V.
							PWM_CH4_TiempoMantenimiento(0);
							PWM_CH4_TiempoBajada(0);
						}
						break;
				}

			}
			break;

		case SESSION_WARMING_UP_CHANNEL_MAINTENANCE:
#ifdef WITH_SYNC
			if ((channel == CH1) && (ch1_sync_state & SYNC_REQUIRED))
			{
				ch1_sync_state &= ~SYNC_ACT_MASK;
				ch1_sync_state |= SYNC_IN_MAINTENANCE;
			}
#endif

			if (*p_session_time >= (p_table + (*p_session_channel_step))->maintenance_step_number)
			{
				//--- Next state ---//
				*p_session_state = SESSION_WARMING_UP_CHANNEL_FALLING_EDGE;

				//--- Slope ---//
				*p_pwm_slope = (p_table + (*p_session_channel_step))->falling_pwm_200_initial + (p_table + (*p_session_channel_step))->falling_pwm_40_initial - (p_table + (*p_session_channel_step))->falling_pwm_200_final - (p_table + (*p_session_channel_step))->falling_pwm_40_final;
				*p_pwm_slope /= (float)10;
				*p_pwm_slope /= p_session_ch->stage_1_falling_time;

				*p_session_time = 0;
				*p_session_time_2 = 0;

				//--- PWM initial values ---//
				switch (channel)
				{
					case CH1:
						if ((*p_session_burst_cnt < (p_table + (*p_session_channel_step))->burst_mode_on) ||
								(	((p_table + (*p_session_channel_step))->burst_mode_on == 0) && ((p_table + (*p_session_channel_step))->burst_mode_off == 0)))
						{
							PWM_CH1_TiempoSubida((p_table + (*p_session_channel_step))->falling_pwm_200_initial); //pwm 200V.
							PWM_CH1_TiempoMantenimiento((p_table + (*p_session_channel_step))->falling_pwm_40_initial); //pwm 40V.
							PWM_CH1_TiempoBajada((p_table + (*p_session_channel_step))->falling_pwm_n_initial);
						}
						else
						{
							PWM_CH1_TiempoSubida(0); //pwm 200V.
							PWM_CH1_TiempoMantenimiento(0);
							PWM_CH1_TiempoBajada(0);
						}
						break;

					case CH2:
						if ((*p_session_burst_cnt < (p_table + (*p_session_channel_step))->burst_mode_on) ||
								(	((p_table + (*p_session_channel_step))->burst_mode_on == 0) && ((p_table + (*p_session_channel_step))->burst_mode_off == 0)))
						{
							PWM_CH2_TiempoSubida((p_table + (*p_session_channel_step))->falling_pwm_200_initial); //pwm 200V.
							PWM_CH2_TiempoMantenimiento((p_table + (*p_session_channel_step))->falling_pwm_40_initial); //pwm 40V.
							PWM_CH2_TiempoBajada((p_table + (*p_session_channel_step))->falling_pwm_n_initial);
						}
						else
						{
							PWM_CH2_TiempoSubida(0); //pwm 200V.
							PWM_CH2_TiempoMantenimiento(0);
							PWM_CH2_TiempoBajada(0);
						}
						break;

					case CH3:
						if ((*p_session_burst_cnt < (p_table + (*p_session_channel_step))->burst_mode_on) ||
								(	((p_table + (*p_session_channel_step))->burst_mode_on == 0) && ((p_table + (*p_session_channel_step))->burst_mode_off == 0)))
						{
							PWM_CH3_TiempoSubida((p_table + (*p_session_channel_step))->falling_pwm_200_initial); //pwm 200V.
							PWM_CH3_TiempoMantenimiento((p_table + (*p_session_channel_step))->falling_pwm_40_initial); //pwm 40V.
							PWM_CH3_TiempoBajada((p_table + (*p_session_channel_step))->falling_pwm_n_initial);
						}
						else
						{
							PWM_CH3_TiempoSubida(0); //pwm 200V.
							PWM_CH3_TiempoMantenimiento(0);
							PWM_CH3_TiempoBajada(0);
						}
						break;

					case CH4:
						if ((*p_session_burst_cnt < (p_table + (*p_session_channel_step))->burst_mode_on) ||
								(	((p_table + (*p_session_channel_step))->burst_mode_on == 0) && ((p_table + (*p_session_channel_step))->burst_mode_off == 0)))
						{
							PWM_CH4_TiempoSubida((p_table + (*p_session_channel_step))->falling_pwm_200_initial); //pwm 200V.
							PWM_CH4_TiempoMantenimiento((p_table + (*p_session_channel_step))->falling_pwm_40_initial); //pwm 40V.
							PWM_CH4_TiempoBajada((p_table + (*p_session_channel_step))->falling_pwm_n_initial);
						}
						else
						{
							PWM_CH4_TiempoSubida(0); //pwm 200V.
							PWM_CH4_TiempoMantenimiento(0);
							PWM_CH4_TiempoBajada(0);
						}
						break;
				}
			}
			break;

		case SESSION_WARMING_UP_CHANNEL_FALLING_EDGE:
#ifdef WITH_SYNC
			if ((channel == CH1) && (ch1_sync_state & SYNC_REQUIRED))
			{
				ch1_sync_state &= ~SYNC_ACT_MASK;
				ch1_sync_state |= SYNC_IN_FALLING;
			}
#endif
			//primero reviso en que tipo de bajada estoy
			//BAJADA FALLING_LR
			if ((p_table + (*p_session_channel_step))->falling_type == FALLING_LR)
			{
				switch(*p_fall_type)
				{
					case FALL_START:
						time_aux = (p_table + (*p_session_channel_step))->falling_step_number;
						time_aux >>= 2;
						if (*p_session_time > ((p_table + (*p_session_channel_step))->falling_step_number - time_aux))
						{
							switch(channel)
							{
								case CH1:
									PWM_CH1_TiempoBajada(1000);
									break;

								case CH2:
									PWM_CH2_TiempoBajada(1000);
									break;

								case CH3:
									PWM_CH3_TiempoBajada(1000);
									break;

								case CH4:
									PWM_CH4_TiempoBajada(1000);
									break;
							}
							*p_fall_type = FALL_MED;
						}
						break;

					case FALL_MED:
						if (*p_session_time >= (p_table + (*p_session_channel_step))->falling_step_number)
							*p_fall_type = FALL_FAST;
						break;

					case FALL_FAST:
						*p_session_state = SESSION_WARMING_UP_CHANNEL_LOW;

						switch(channel)
						{
							case CH1:
								PWM_CH1_TiempoSubida(0); //pwm 200V.
								PWM_CH1_TiempoMantenimiento(0);
								PWM_CH1_TiempoBajada(1000);
								break;

							case CH2:
								PWM_CH2_TiempoSubida(0); //pwm 200V.
								PWM_CH2_TiempoMantenimiento(0);
								PWM_CH2_TiempoBajada(1000);
								break;

							case CH3:
								PWM_CH3_TiempoSubida(0); //pwm 200V.
								PWM_CH3_TiempoMantenimiento(0);
								PWM_CH3_TiempoBajada(1000);
								break;

							case CH4:
								PWM_CH4_TiempoSubida(0); //pwm 200V.
								PWM_CH4_TiempoMantenimiento(0);
								PWM_CH4_TiempoBajada(1000);
								break;
						}

						*p_session_time = 0;
						*p_session_time_2 = 0;
						*p_fall_type = FALL_START;
						break;

					default:
						*p_fall_type = FALL_START;
						break;
				}
			}

			//BAJADA FALLING_FAST_DISCHARGE
			if ((p_table + (*p_session_channel_step))->falling_type == FALLING_FAST_DISCHARGE)
			{
				switch(*p_fall_type)
				{
					case FALL_START:

						switch(channel)
						{
							case CH1:
								PWM_CH1_TiempoSubida(0); //pwm 200V.
								PWM_CH1_TiempoMantenimiento(0);
								PWM_CH1_TiempoBajada(1000);
								break;

							case CH2:
								PWM_CH2_TiempoSubida(0); //pwm 200V.
								PWM_CH2_TiempoMantenimiento(0);
								PWM_CH2_TiempoBajada(1000);
								break;

							case CH3:
								PWM_CH3_TiempoSubida(0); //pwm 200V.
								PWM_CH3_TiempoMantenimiento(0);
								PWM_CH3_TiempoBajada(1000);
								break;

							case CH4:
								PWM_CH4_TiempoSubida(0); //pwm 200V.
								PWM_CH4_TiempoMantenimiento(0);
								PWM_CH4_TiempoBajada(1000);
								break;
						}
						*p_fall_type = FALL_MED;
						break;

					case FALL_MED:
						if (*p_session_time >= (p_table + (*p_session_channel_step))->falling_step_number)
							*p_fall_type = FALL_FAST;
						break;

					case FALL_FAST:
						*p_session_state = SESSION_WARMING_UP_CHANNEL_LOW;

						switch(channel)
						{
							case CH1:
								PWM_CH1_TiempoSubida(0); //pwm 200V.
								PWM_CH1_TiempoMantenimiento(0);
								PWM_CH1_TiempoBajada(1000);
								break;

							case CH2:
								PWM_CH2_TiempoSubida(0); //pwm 200V.
								PWM_CH2_TiempoMantenimiento(0);
								PWM_CH2_TiempoBajada(1000);
								break;

							case CH3:
								PWM_CH3_TiempoSubida(0); //pwm 200V.
								PWM_CH3_TiempoMantenimiento(0);
								PWM_CH3_TiempoBajada(1000);
								break;

							case CH4:
								PWM_CH4_TiempoSubida(0); //pwm 200V.
								PWM_CH4_TiempoMantenimiento(0);
								PWM_CH4_TiempoBajada(1000);
								break;
						}

						*p_session_time = 0;
						*p_session_time_2 = 0;

						*p_fall_type = FALL_START;
						break;

					default:
						*p_fall_type = FALL_START;
						break;
				}
			}

			//BAJADA FALLING_SLOW_DISCHARGE
			if ((p_table + (*p_session_channel_step))->falling_type == FALLING_SLOW_DISCHARGE)
			{
				if (*p_session_time <= (p_table + (*p_session_channel_step))->falling_step_number)
				{
					if (*p_session_time_2 >= (*p_session_time - 5))
					{
						*p_session_time_2 = *p_session_time;

						//esta es el pwm de este instante el slope lo calculo antes
						*p_pwm_ch = (p_table + (*p_session_channel_step))->falling_pwm_200_initial + (p_table + (*p_session_channel_step))->falling_pwm_40_initial - *p_pwm_slope * *p_session_time;

						//modificacion bajada LR
						if ((*p_pwm_ch < 10) && ((p_table + (*p_session_channel_step))->falling_type != FALLING_LR))
						{
							*p_pwm_ch = 0;
							switch(channel)
							{
								case CH1:
									PWM_CH1_TiempoBajada(1000);
									break;

								case CH2:
									PWM_CH2_TiempoBajada(1000);
									break;

								case CH3:
									PWM_CH3_TiempoBajada(1000);
									break;

								case CH4:
									PWM_CH4_TiempoBajada(1000);
									break;
							}
						}

						//esto representa la funcion lineal de bajada lento
						switch (channel)
						{
							case CH1:
								if ((*p_session_burst_cnt < (p_table + (*p_session_channel_step))->burst_mode_on) ||
										(	((p_table + (*p_session_channel_step))->burst_mode_on == 0) && ((p_table + (*p_session_channel_step))->burst_mode_off == 0)))
								{
									if (((p_table + (*p_session_channel_step))->falling_pwm_200_initial + (p_table + (*p_session_channel_step))->falling_pwm_200_final) != 0)
										PWM_CH1_TiempoSubida(*p_pwm_ch); //pwm 200V.
									else if (((p_table + (*p_session_channel_step))->falling_pwm_40_initial + (p_table + (*p_session_channel_step))->falling_pwm_40_final) != 0)
										PWM_CH1_TiempoMantenimiento(*p_pwm_ch); //pwm 40V.
								}
								else
								{
									PWM_CH1_TiempoSubida(0); //pwm 200V.
									PWM_CH1_TiempoMantenimiento(0);
									PWM_CH1_TiempoBajada(0);
								}
								break;

							case CH2:
								if ((*p_session_burst_cnt < (p_table + (*p_session_channel_step))->burst_mode_on) ||
										(	((p_table + (*p_session_channel_step))->burst_mode_on == 0) && ((p_table + (*p_session_channel_step))->burst_mode_off == 0)))
								{
									if (((p_table + (*p_session_channel_step))->falling_pwm_200_initial + (p_table + (*p_session_channel_step))->falling_pwm_200_final) != 0)
										PWM_CH2_TiempoSubida(*p_pwm_ch); //pwm 200V.
									else if (((p_table + (*p_session_channel_step))->falling_pwm_40_initial + (p_table + (*p_session_channel_step))->falling_pwm_40_final) != 0)
										PWM_CH2_TiempoMantenimiento(*p_pwm_ch); //pwm 40V.
								}
								else
								{
									PWM_CH2_TiempoSubida(0); //pwm 200V.
									PWM_CH2_TiempoMantenimiento(0);
									PWM_CH2_TiempoBajada(0);
								}
								break;

							case CH3:
								if ((*p_session_burst_cnt < (p_table + (*p_session_channel_step))->burst_mode_on) ||
										(	((p_table + (*p_session_channel_step))->burst_mode_on == 0) && ((p_table + (*p_session_channel_step))->burst_mode_off == 0)))
								{
									if (((p_table + (*p_session_channel_step))->falling_pwm_200_initial + (p_table + (*p_session_channel_step))->falling_pwm_200_final) != 0)
										PWM_CH3_TiempoSubida(*p_pwm_ch); //pwm 200V.
									else if (((p_table + (*p_session_channel_step))->falling_pwm_40_initial + (p_table + (*p_session_channel_step))->falling_pwm_40_final) != 0)
										PWM_CH3_TiempoMantenimiento(*p_pwm_ch); //pwm 40V.
								}
								else
								{
									PWM_CH3_TiempoSubida(0); //pwm 200V.
									PWM_CH3_TiempoMantenimiento(0);
									PWM_CH3_TiempoBajada(0);
								}
								break;

							case CH4:
								if ((*p_session_burst_cnt < (p_table + (*p_session_channel_step))->burst_mode_on) ||
										(	((p_table + (*p_session_channel_step))->burst_mode_on == 0) && ((p_table + (*p_session_channel_step))->burst_mode_off == 0)))
								{
									if (((p_table + (*p_session_channel_step))->falling_pwm_200_initial + (p_table + (*p_session_channel_step))->falling_pwm_200_final) != 0)
										PWM_CH4_TiempoSubida(*p_pwm_ch); //pwm 200V.
									else if (((p_table + (*p_session_channel_step))->falling_pwm_40_initial + (p_table + (*p_session_channel_step))->falling_pwm_40_final) != 0)
										PWM_CH4_TiempoMantenimiento(*p_pwm_ch); //pwm 40V.
								}
								else
								{
									PWM_CH4_TiempoSubida(0); //pwm 200V.
									PWM_CH4_TiempoMantenimiento(0);
									PWM_CH4_TiempoBajada(0);
								}
								break;
						}
					}
				}
				else
				{
					*p_session_state = SESSION_WARMING_UP_CHANNEL_LOW;


					switch(channel)
					{
						case CH1:
							PWM_CH1_TiempoSubida(0); //pwm 200V.
							PWM_CH1_TiempoMantenimiento(0);
							PWM_CH1_TiempoBajada(1000);
							break;

						case CH2:
							PWM_CH2_TiempoSubida(0); //pwm 200V.
							PWM_CH2_TiempoMantenimiento(0);
							PWM_CH2_TiempoBajada(1000);
							break;

						case CH3:
							PWM_CH3_TiempoSubida(0); //pwm 200V.
							PWM_CH3_TiempoMantenimiento(0);
							PWM_CH3_TiempoBajada(1000);
							break;

						case CH4:
							PWM_CH4_TiempoSubida(0); //pwm 200V.
							PWM_CH4_TiempoMantenimiento(0);
							PWM_CH4_TiempoBajada(1000);
							break;
					}
					*p_session_time = 0;
					*p_session_time_2 = 0;
				}
			}
			break;

		case SESSION_WARMING_UP_CHANNEL_LOW:
#ifdef WITH_SYNC
			if (ch1_sync_state & SYNC_REQUIRED)
			{
				if (channel == CH1)
				{
					ch1_sync_state &= ~SYNC_ACT_MASK;
					ch1_sync_state |= SYNC_IN_DOWN;

					if (*p_session_time >= (p_table + (*p_session_channel_step))->low_step_number)
					{
						//*p_session_burst_cnt++;
						*p_session_burst_cnt = *p_session_burst_cnt + 1;

						if (*p_session_burst_cnt == ((p_table + (*p_session_channel_step))->burst_mode_on + (p_table + (*p_session_channel_step))->burst_mode_off))
							*p_session_burst_cnt = 0;

						*p_session_state = SESSION_WARMING_UP_CHANNEL_CHANGE_LEVEL;
					}
				}
				else
				{
					switch (channel)
					{
						case CH2:
							if (ch1_sync_state & SYNC_FINISH_WARMING_UP)
								return FIN_OK;

							//me quedo en LOW hasta que CH1 este en LOW
							if (ch1_sync_state & SYNC_IN_DOWN)
								*p_session_state = SESSION_WARMING_UP_CHANNEL_CHANGE_LEVEL;
							break;

						case CH3:
							if (ch1_sync_state & SYNC_FINISH_WARMING_UP)
								return FIN_OK;

							//me quedo en LOW mientras CH1 este en LOW, lo atraso un poquito para evitar dobles pulsos
							//if (!(ch1_sync_state & SYNC_IN_DOWN))
							if (ch1_sync_state & SYNC_IN_RISING)
								*p_session_state = SESSION_WARMING_UP_CHANNEL_CHANGE_LEVEL;
							break;

						case CH4:
							if (ch1_sync_state & SYNC_FINISH_WARMING_UP)
								return FIN_OK;

							//me quedo en LOW hasta que CH1 este en LOW
							if (ch1_sync_state & SYNC_IN_DOWN)
								*p_session_state = SESSION_WARMING_UP_CHANNEL_CHANGE_LEVEL;
							break;
					}
				}
			}
			else
			{
				if (*p_session_time >= (p_table + (*p_session_channel_step))->low_step_number)
				{
					//*p_session_burst_cnt++;

					*p_session_burst_cnt = *p_session_burst_cnt + 1;

					if (*p_session_burst_cnt == ((p_table + (*p_session_channel_step))->burst_mode_on + (p_table + (*p_session_channel_step))->burst_mode_off))
						*p_session_burst_cnt = 0;

					*p_session_state = SESSION_WARMING_UP_CHANNEL_CHANGE_LEVEL;
				}
			}
#else
			if (*p_session_time >= (p_table + (*p_session_channel_step))->low_step_number)
			{
				*p_session_burst_cnt++;

				if (*p_session_burst_cnt == ((p_table + (*p_session_channel_step))->burst_mode_on + (p_table + (*p_session_channel_step))->burst_mode_off))
					*p_session_burst_cnt = 0;

				*p_session_state = SESSION_WARMING_UP_CHANNEL_CHANGE_LEVEL;
			}
#endif
			break;

		case SESSION_WARMING_UP_CHANNEL_END_ERROR:
			*p_session_state = SESSION_WARMING_UP_CHANNEL_INIT;
			return FIN_ERROR;
			break;

		default:
			*p_session_state = SESSION_WARMING_UP_CHANNEL_INIT;
			break;

	}
	return TRABAJANDO;
}

//------------ NUEVA FUNCION PARAMETERS CALCULATE 18-03-15 ----------------//

//------ NUEVA PLATEAU ------//
unsigned char Session_Plateau_Channels(unsigned char channel)
{
	unsigned char i;
	unsigned short time_aux = 0;
	unsigned char * p_session_state;

	session_typedef * p_session_ch;
	unsigned short * p_session_burst_cnt;
	warningup_coolingdown_typedef * p_table;
	float * p_pwm_slope;
	float * p_pwm_ch;
	unsigned int * p_stage_time;
	unsigned short * p_session_time;
	unsigned short * p_session_time_2;
	unsigned char * p_fall_type;

	//primero acomodo los punteros
	switch (channel)
	{
		case CH1:
			p_session_state = &session_plateau_channel_1_state;

			p_session_ch = &session_ch_1;
			p_session_burst_cnt = &session_plateau_channel_1_burst_cnt;
			p_table = &table_plateau_channel_1[0];
			p_pwm_slope = &pwm_slope_channel_1;
			p_stage_time = &session_plateau_channel_1_stage_time;
			p_session_time = &session_plateau_channel_1_time;
			p_session_time_2 = &session_plateau_channel_1_time_2;
			p_pwm_ch = &pwm_channel_1;
			p_fall_type = &fall_type_step_ch1;
			break;

		case CH2:
			p_session_state = &session_plateau_channel_2_state;

			p_session_ch = &session_ch_2;
			p_session_burst_cnt = &session_plateau_channel_2_burst_cnt;
			p_table = &table_plateau_channel_2[0];
			p_pwm_slope = &pwm_slope_channel_2;
			p_stage_time = &session_plateau_channel_2_stage_time;
			p_session_time = &session_plateau_channel_2_time;
			p_session_time_2 = &session_plateau_channel_2_time_2;
			p_pwm_ch = &pwm_channel_2;
			p_fall_type = &fall_type_step_ch2;
			break;

		case CH3:
			p_session_state = &session_plateau_channel_3_state;

			p_session_ch = &session_ch_3;
			p_session_burst_cnt = &session_plateau_channel_3_burst_cnt;
			p_table = &table_plateau_channel_3[0];
			p_pwm_slope = &pwm_slope_channel_3;
			p_stage_time = &session_plateau_channel_3_stage_time;
			p_session_time = &session_plateau_channel_3_time;
			p_session_time_2 = &session_plateau_channel_3_time_2;
			p_pwm_ch = &pwm_channel_3;
			p_fall_type = &fall_type_step_ch3;
			break;

		case CH4:
			p_session_state = &session_plateau_channel_4_state;

			p_session_ch = &session_ch_4;
			p_session_burst_cnt = &session_plateau_channel_4_burst_cnt;
			p_table = &table_plateau_channel_4[0];
			p_pwm_slope = &pwm_slope_channel_4;
			p_stage_time = &session_plateau_channel_4_stage_time;
			p_session_time = &session_plateau_channel_4_time;
			p_session_time_2 = &session_plateau_channel_4_time_2;
			p_pwm_ch = &pwm_channel_4;
			p_fall_type = &fall_type_step_ch4;
			break;

		default:
			return FIN_ERROR;
			break;
	}

	//--- Plateau stage ---//
	switch(*p_session_state)
	{
		case SESSION_PLATEAU_CHANNEL_INIT:

			if (p_session_ch->stage_2_status)
			{
#ifdef WITH_SYNC
				//reviso si tengo que utilizar SYNC
				if (p_session_ch->sync_on)
				{
					//debo tener canal 1 presente
					if (channel == CH1)
					{
						ch1_sync_state |= SYNC_REQUIRED;
						*p_session_state = SESSION_PLATEAU_CHANNEL_PARAMETERS_CALCULATE;
					}
					else
					{
						//reviso que este presente CH1 l antena y deberia estar revisada
						if (ch1_sync_state & SYNC_REQUIRED)
						{
							//CH1 presente
							*p_session_state = SESSION_PLATEAU_CHANNEL_PARAMETERS_CALCULATE;
							sync_in_waiting = 0;
						}
						else
						{
							//no esta CH1
							*p_session_state = SESSION_PLATEAU_CHANNEL_END_ERROR;

							sprintf(&buffSendErr[0], "ERROR in CH%d: not CH1 detected and sync is needed\r\n", channel);
							UART_PC_Send(&buffSendErr[0]);
							sync_in_waiting = 0;
							return FIN_ERROR;
						}
					}
				}
				else
				{
					sprintf(&buffSendErr[0], "\r\nCH%d no sync needed", channel);
					UART_PC_Send(&buffSendErr[0]);
					ch1_sync_state &= NO_SYNC_REQUIRED_MASK;
					*p_session_state = SESSION_PLATEAU_CHANNEL_PARAMETERS_CALCULATE;
				}
#else
				*p_session_state = SESSION_PLATEAU_CHANNEL_PARAMETERS_CALCULATE;
#endif

			}
			else
				return FIN_OK;
			break;

		case SESSION_PLATEAU_CHANNEL_PARAMETERS_CALCULATE:

			i = Session_Channels_Parameters_Calculate(channel, PLATEAU);

			if (i == FIN_OK)
			{
				*p_session_burst_cnt = 0;

				//--- Slope ---//
				*p_pwm_slope = p_table->rising_pwm_200_final + p_table->rising_pwm_40_final - p_table->rising_pwm_200_initial - p_table->rising_pwm_40_initial;
				*p_pwm_slope /= (float)10;
				*p_pwm_slope /= p_session_ch->stage_2_rising_time;

				//--- Next state ---//
				*p_session_state = SESSION_PLATEAU_CHANNEL_RISING_EDGE;

				//--- Time of the next step ---//
				*p_stage_time = (p_session_ch->stage_2_time_hours * 3600 + p_session_ch->stage_2_time_minutes * 60 + p_session_ch->stage_2_time_seconds) * 10000;
				*p_session_time = 0;
				*p_session_time_2 = 0;

				//--- PWM initial values ---//
				switch (channel)
				{
					case CH1:
						if (*p_session_burst_cnt < p_table->burst_mode_on)
						{
							PWM_CH1_TiempoSubida(p_table->rising_pwm_200_initial); //pwm 200V.
							PWM_CH1_TiempoMantenimiento(p_table->rising_pwm_40_initial);
							PWM_CH1_TiempoBajada(p_table->rising_pwm_n_initial);
						}
						else
						{
							PWM_CH1_TiempoSubida(0); //pwm 200V.
							PWM_CH1_TiempoMantenimiento(0);
							PWM_CH1_TiempoBajada(0);
						}
						break;

					case CH2:
						if (*p_session_burst_cnt < p_table->burst_mode_on)
						{
							PWM_CH2_TiempoSubida(p_table->rising_pwm_200_initial); //pwm 200V.
							PWM_CH2_TiempoMantenimiento(p_table->rising_pwm_40_initial);
							PWM_CH2_TiempoBajada(p_table->rising_pwm_n_initial);
						}
						else
						{
							PWM_CH2_TiempoSubida(0); //pwm 200V.
							PWM_CH2_TiempoMantenimiento(0);
							PWM_CH2_TiempoBajada(0);
						}
						break;

					case CH3:
						if (*p_session_burst_cnt < p_table->burst_mode_on)
						{
							PWM_CH3_TiempoSubida(p_table->rising_pwm_200_initial); //pwm 200V.
							PWM_CH3_TiempoMantenimiento(p_table->rising_pwm_40_initial);
							PWM_CH3_TiempoBajada(p_table->rising_pwm_n_initial);
						}
						else
						{
							PWM_CH3_TiempoSubida(0); //pwm 200V.
							PWM_CH3_TiempoMantenimiento(0);
							PWM_CH3_TiempoBajada(0);
						}
						break;

					case CH4:
						if (*p_session_burst_cnt < p_table->burst_mode_on)
						{
							PWM_CH4_TiempoSubida(p_table->rising_pwm_200_initial); //pwm 200V.
							PWM_CH4_TiempoMantenimiento(p_table->rising_pwm_40_initial);
							PWM_CH4_TiempoBajada(p_table->rising_pwm_n_initial);
						}
						else
						{
							PWM_CH4_TiempoSubida(0); //pwm 200V.
							PWM_CH4_TiempoMantenimiento(0);
							PWM_CH4_TiempoBajada(0);
						}
						break;
				}
			}

			else if (i == FIN_ERROR)
			{
				*p_session_state = SESSION_PLATEAU_CHANNEL_END_ERROR;

				sprintf(&buffSendErr[0], (const char *) "ERROR(0x%03X)\r\n", ERR_CHANNEL_PLATEAU_PARAMETERS_CALCULATE(2));
				UART_PC_Send(&buffSendErr[0]);
			}

			break;

		case SESSION_PLATEAU_CHANNEL_RISING_EDGE:
#ifdef WITH_SYNC
			if ((channel == CH1) && (ch1_sync_state & SYNC_REQUIRED))
			{
				ch1_sync_state &= ~SYNC_ACT_MASK;
				ch1_sync_state |= SYNC_IN_RISING;
			}
#endif

			if (*p_session_time <= p_table->rising_step_number)
			{
				if (*p_session_time_2 != *p_session_time)		//va haciendo un update del pwm para dibujar la subida rising
				{
					*p_session_time_2 = *p_session_time;

					*p_pwm_ch = p_table->rising_pwm_200_initial + p_table->rising_pwm_40_initial + (*p_pwm_slope) * (*p_session_time);

					switch (channel)
					{
						case CH1:
							if ((*p_session_burst_cnt < p_table->burst_mode_on) || ((p_table->burst_mode_on == 0) && (p_table->burst_mode_off == 0)))
							{
								if ((p_table->rising_pwm_200_initial + p_table->rising_pwm_200_final) != 0)
									PWM_CH1_TiempoSubida(*p_pwm_ch); //pwm 200V.
								else if ((p_table->rising_pwm_40_initial + p_table->rising_pwm_40_final) != 0)
									PWM_CH1_TiempoMantenimiento(*p_pwm_ch); //pwm 40V.
							}
							else
							{
								PWM_CH1_TiempoSubida(0); //pwm 200V.
								PWM_CH1_TiempoMantenimiento(0);
								PWM_CH1_TiempoBajada(0);
							}
							break;

						case CH2:
							if ((*p_session_burst_cnt < p_table->burst_mode_on) || ((p_table->burst_mode_on == 0) && (p_table->burst_mode_off == 0)))
							{
								if ((p_table->rising_pwm_200_initial + p_table->rising_pwm_200_final) != 0)
									PWM_CH2_TiempoSubida(*p_pwm_ch); //pwm 200V.
								else if ((p_table->rising_pwm_40_initial + p_table->rising_pwm_40_final) != 0)
									PWM_CH2_TiempoMantenimiento(*p_pwm_ch); //pwm 40V.
							}
							else
							{
								PWM_CH2_TiempoSubida(0); //pwm 200V.
								PWM_CH2_TiempoMantenimiento(0);
								PWM_CH2_TiempoBajada(0);
							}
							break;

						case CH3:
							if ((*p_session_burst_cnt < p_table->burst_mode_on) || ((p_table->burst_mode_on == 0) && (p_table->burst_mode_off == 0)))
							{
								if ((p_table->rising_pwm_200_initial + p_table->rising_pwm_200_final) != 0)
									PWM_CH3_TiempoSubida(*p_pwm_ch); //pwm 200V.
								else if ((p_table->rising_pwm_40_initial + p_table->rising_pwm_40_final) != 0)
									PWM_CH3_TiempoMantenimiento(*p_pwm_ch); //pwm 40V.
							}
							else
							{
								PWM_CH3_TiempoSubida(0); //pwm 200V.
								PWM_CH3_TiempoMantenimiento(0);
								PWM_CH3_TiempoBajada(0);
							}
							break;

						case CH4:
							if ((*p_session_burst_cnt < p_table->burst_mode_on) || ((p_table->burst_mode_on == 0) && (p_table->burst_mode_off == 0)))
							{
								if ((p_table->rising_pwm_200_initial + p_table->rising_pwm_200_final) != 0)
									PWM_CH4_TiempoSubida(*p_pwm_ch); //pwm 200V.
								else if ((p_table->rising_pwm_40_initial + p_table->rising_pwm_40_final) != 0)
									PWM_CH4_TiempoMantenimiento(*p_pwm_ch); //pwm 40V.
							}
							else
							{
								PWM_CH4_TiempoSubida(0); //pwm 200V.
								PWM_CH4_TiempoMantenimiento(0);
								PWM_CH4_TiempoBajada(0);
							}
							break;
					}
				}
			}
			else
			{
				//--- Next state ---//
				*p_session_state = SESSION_PLATEAU_CHANNEL_MAINTENANCE;
				*p_session_time = 0;
				*p_session_time_2 = 0;

				//--- PWM initial values ---//
				switch (channel)
				{
					case CH1:
						if ((*p_session_burst_cnt < p_table->burst_mode_on) || ((p_table->burst_mode_on == 0) && (p_table->burst_mode_off == 0)))
						{
							PWM_CH1_TiempoSubida(p_table->maintenance_pwm_200); //pwm 200V.
							PWM_CH1_TiempoMantenimiento(p_table->maintenance_pwm_40); //pwm 40V.
							PWM_CH1_TiempoBajada(p_table->maintenance_pwm_n);
						}
						else
						{
							PWM_CH1_TiempoSubida(0); //pwm 200V.
							PWM_CH1_TiempoMantenimiento(0);
							PWM_CH1_TiempoBajada(0);
						}
						break;

					case CH2:
						if ((*p_session_burst_cnt < p_table->burst_mode_on) || ((p_table->burst_mode_on == 0) && (p_table->burst_mode_off == 0)))
						{
							PWM_CH2_TiempoSubida(p_table->maintenance_pwm_200); //pwm 200V.
							PWM_CH2_TiempoMantenimiento(p_table->maintenance_pwm_40); //pwm 40V.
							PWM_CH2_TiempoBajada(p_table->maintenance_pwm_n);
						}
						else
						{
							PWM_CH2_TiempoSubida(0); //pwm 200V.
							PWM_CH2_TiempoMantenimiento(0);
							PWM_CH2_TiempoBajada(0);
						}
						break;

					case CH3:
						if ((*p_session_burst_cnt < p_table->burst_mode_on) || ((p_table->burst_mode_on == 0) && (p_table->burst_mode_off == 0)))
						{
							PWM_CH3_TiempoSubida(p_table->maintenance_pwm_200); //pwm 200V.
							PWM_CH3_TiempoMantenimiento(p_table->maintenance_pwm_40); //pwm 40V.
							PWM_CH3_TiempoBajada(p_table->maintenance_pwm_n);
						}
						else
						{
							PWM_CH3_TiempoSubida(0); //pwm 200V.
							PWM_CH3_TiempoMantenimiento(0);
							PWM_CH3_TiempoBajada(0);
						}
						break;

					case CH4:
						if ((*p_session_burst_cnt < p_table->burst_mode_on) || ((p_table->burst_mode_on == 0) && (p_table->burst_mode_off == 0)))
						{
							PWM_CH4_TiempoSubida(p_table->maintenance_pwm_200); //pwm 200V.
							PWM_CH4_TiempoMantenimiento(p_table->maintenance_pwm_40); //pwm 40V.
							PWM_CH4_TiempoBajada(p_table->maintenance_pwm_n);
						}
						else
						{
							PWM_CH4_TiempoSubida(0); //pwm 200V.
							PWM_CH4_TiempoMantenimiento(0);
							PWM_CH4_TiempoBajada(0);
						}
						break;
					}
			}
			break;

		case SESSION_PLATEAU_CHANNEL_MAINTENANCE:
#ifdef WITH_SYNC
			if ((channel == CH1) && (ch1_sync_state & SYNC_REQUIRED))
			{
				ch1_sync_state &= ~SYNC_ACT_MASK;
				ch1_sync_state |= SYNC_IN_MAINTENANCE;
			}
#endif

			if (*p_session_time >= p_table->maintenance_step_number)
			{
				//--- Next state ---//
				*p_session_state = SESSION_PLATEAU_CHANNEL_FALLING_EDGE;

				//--- Slope ---//
				*p_pwm_slope = (p_table->falling_pwm_200_initial + p_table->falling_pwm_40_initial - p_table->falling_pwm_200_final - p_table->falling_pwm_40_final);
				*p_pwm_slope /= (float)10;
//CAMBIAR X P_SESSION_CH	*p_pwm_slope /= p_table->falling_time;
				*p_pwm_slope /= p_session_ch->stage_2_falling_time;		//TODO revisar que parameters calculate lo este cargando

				*p_session_time = 0;
				*p_session_time_2 = 0;

				//--- PWM initial values ---//
				switch (channel)
				{
					case CH1:
						if ((*p_session_burst_cnt < p_table->burst_mode_on) || ((p_table->burst_mode_on == 0) && (p_table->burst_mode_off == 0)))
						{
							PWM_CH1_TiempoSubida(p_table->falling_pwm_200_initial); 		//pwm 200V.
							PWM_CH1_TiempoMantenimiento(p_table->falling_pwm_40_initial); 	//pwm 40V.
							PWM_CH1_TiempoBajada(p_table->falling_pwm_n_initial);			//pwm n.
						}
						else
						{
							PWM_CH1_TiempoSubida(0); //pwm 200V.
							PWM_CH1_TiempoMantenimiento(0);
							PWM_CH1_TiempoBajada(0);
						}
						break;

					case CH2:
						if ((*p_session_burst_cnt < p_table->burst_mode_on) || ((p_table->burst_mode_on == 0) && (p_table->burst_mode_off == 0)))
						{
							PWM_CH2_TiempoSubida(p_table->falling_pwm_200_initial); 		//pwm 200V.
							PWM_CH2_TiempoMantenimiento(p_table->falling_pwm_40_initial); 	//pwm 40V.
							PWM_CH2_TiempoBajada(p_table->falling_pwm_n_initial);			//pwm n.
						}
						else
						{
							PWM_CH2_TiempoSubida(0); //pwm 200V.
							PWM_CH2_TiempoMantenimiento(0);
							PWM_CH2_TiempoBajada(0);
						}
						break;

					case CH3:
						if ((*p_session_burst_cnt < p_table->burst_mode_on) || ((p_table->burst_mode_on == 0) && (p_table->burst_mode_off == 0)))
						{
							PWM_CH3_TiempoSubida(p_table->falling_pwm_200_initial); 		//pwm 200V.
							PWM_CH3_TiempoMantenimiento(p_table->falling_pwm_40_initial); 	//pwm 40V.
							PWM_CH3_TiempoBajada(p_table->falling_pwm_n_initial);			//pwm n.
						}
						else
						{
							PWM_CH3_TiempoSubida(0); //pwm 200V.
							PWM_CH3_TiempoMantenimiento(0);
							PWM_CH3_TiempoBajada(0);
						}
						break;

					case CH4:
						if ((*p_session_burst_cnt < p_table->burst_mode_on) || ((p_table->burst_mode_on == 0) && (p_table->burst_mode_off == 0)))
						{
							PWM_CH4_TiempoSubida(p_table->falling_pwm_200_initial); 		//pwm 200V.
							PWM_CH4_TiempoMantenimiento(p_table->falling_pwm_40_initial); 	//pwm 40V.
							PWM_CH4_TiempoBajada(p_table->falling_pwm_n_initial);			//pwm n.
						}
						else
						{
							PWM_CH4_TiempoSubida(0); //pwm 200V.
							PWM_CH4_TiempoMantenimiento(0);
							PWM_CH4_TiempoBajada(0);
						}
						break;
				}
			}
			break;

		case SESSION_PLATEAU_CHANNEL_FALLING_EDGE:
#ifdef WITH_SYNC
			if ((channel == CH1) && (ch1_sync_state & SYNC_REQUIRED))
			{
				ch1_sync_state &= ~SYNC_ACT_MASK;
				ch1_sync_state |= SYNC_IN_FALLING;
			}
#endif

			//primero reviso en que tipo de bajada estoy
			//BAJADA FALLING_LR
			if (p_table->falling_type == FALLING_LR)
			{
				switch(*p_fall_type)
				{
					case FALL_START:
						time_aux = p_table->falling_step_number;
						time_aux >>= 2;
						if (*p_session_time > (p_table->falling_step_number - time_aux))
						{
							switch(channel)
							{
								case CH1:
									PWM_CH1_TiempoBajada(1000);
									break;

								case CH2:
									PWM_CH2_TiempoBajada(1000);
									break;

								case CH3:
									PWM_CH3_TiempoBajada(1000);
									break;

								case CH4:
									PWM_CH4_TiempoBajada(1000);
									break;
							}
							*p_fall_type = FALL_MED;
						}
						break;

					case FALL_MED:
						if (*p_session_time >= (p_table->falling_step_number))
							*p_fall_type = FALL_FAST;
						break;

					case FALL_FAST:
						*p_session_state = SESSION_PLATEAU_CHANNEL_LOW;

						switch(channel)
						{
							case CH1:
								PWM_CH1_TiempoSubida(0); //pwm 200V.
								PWM_CH1_TiempoMantenimiento(0);
								PWM_CH1_TiempoBajada(1000);
								break;

							case CH2:
								PWM_CH2_TiempoSubida(0); //pwm 200V.
								PWM_CH2_TiempoMantenimiento(0);
								PWM_CH2_TiempoBajada(1000);
								break;

							case CH3:
								PWM_CH3_TiempoSubida(0); //pwm 200V.
								PWM_CH3_TiempoMantenimiento(0);
								PWM_CH3_TiempoBajada(1000);
								break;

							case CH4:
								PWM_CH4_TiempoSubida(0); //pwm 200V.
								PWM_CH4_TiempoMantenimiento(0);
								PWM_CH4_TiempoBajada(1000);
								break;
						}

						*p_session_time = 0;
						*p_session_time_2 = 0;
						*p_fall_type = FALL_START;
						break;

					default:
						*p_fall_type = FALL_START;
						break;
				}
			}

			//BAJADA FALLING_FAST_DISCHARGE
			if (p_table->falling_type == FALLING_FAST_DISCHARGE)
			{
				switch(*p_fall_type)
				{
					case FALL_START:

						switch(channel)
						{
							case CH1:
								PWM_CH1_TiempoSubida(0); //pwm 200V.
								PWM_CH1_TiempoMantenimiento(0);
								PWM_CH1_TiempoBajada(1000);
								break;

							case CH2:
								PWM_CH2_TiempoSubida(0); //pwm 200V.
								PWM_CH2_TiempoMantenimiento(0);
								PWM_CH2_TiempoBajada(1000);
								break;

							case CH3:
								PWM_CH3_TiempoSubida(0); //pwm 200V.
								PWM_CH3_TiempoMantenimiento(0);
								PWM_CH3_TiempoBajada(1000);
								break;

							case CH4:
								PWM_CH4_TiempoSubida(0); //pwm 200V.
								PWM_CH4_TiempoMantenimiento(0);
								PWM_CH4_TiempoBajada(1000);
								break;
						}
						*p_fall_type = FALL_MED;
						break;

					case FALL_MED:
						if (*p_session_time >= (p_table->falling_step_number))
							*p_fall_type = FALL_FAST;
						break;

					case FALL_FAST:
						*p_session_state = SESSION_PLATEAU_CHANNEL_LOW;

						switch(channel)
						{
							case CH1:
								PWM_CH1_TiempoSubida(0); //pwm 200V.
								PWM_CH1_TiempoMantenimiento(0);
								PWM_CH1_TiempoBajada(1000);
								break;

							case CH2:
								PWM_CH2_TiempoSubida(0); //pwm 200V.
								PWM_CH2_TiempoMantenimiento(0);
								PWM_CH2_TiempoBajada(1000);
								break;

							case CH3:
								PWM_CH3_TiempoSubida(0); //pwm 200V.
								PWM_CH3_TiempoMantenimiento(0);
								PWM_CH3_TiempoBajada(1000);
								break;

							case CH4:
								PWM_CH4_TiempoSubida(0); //pwm 200V.
								PWM_CH4_TiempoMantenimiento(0);
								PWM_CH4_TiempoBajada(1000);
								break;
						}

						*p_session_time = 0;
						*p_session_time_2 = 0;

						*p_fall_type = FALL_START;
						break;

					default:
						*p_fall_type = FALL_START;
						break;
				}
			}

			//BAJADA FALLING_SLOW_DISCHARGE
			if (p_table->falling_type == FALLING_SLOW_DISCHARGE)
			{
				if (*p_session_time <= (p_table->falling_step_number))
				{
					if (*p_session_time_2 >= (*p_session_time - 5))
					{
						*p_session_time_2 = *p_session_time;

						//esta es el pwm de este instante el slope lo calculo antes
						*p_pwm_ch = p_table->falling_pwm_200_initial + p_table->falling_pwm_40_initial - *p_pwm_slope * *p_session_time;

						//modificacion bajada LR
						if ((*p_pwm_ch < 10) && (p_table->falling_type != FALLING_LR))
						{
							*p_pwm_ch = 0;
							switch(channel)
							{
								case CH1:
									PWM_CH1_TiempoBajada(1000);
									break;

								case CH2:
									PWM_CH2_TiempoBajada(1000);
									break;

								case CH3:
									PWM_CH3_TiempoBajada(1000);
									break;

								case CH4:
									PWM_CH4_TiempoBajada(1000);
									break;
							}
						}

						//esto representa la funcion lineal de bajada lento
						switch (channel)
						{
							case CH1:
								if ((*p_session_burst_cnt < p_table->burst_mode_on) || ((p_table->burst_mode_on == 0) && (p_table->burst_mode_off == 0)))
								{
									if ((p_table->falling_pwm_200_initial + p_table->falling_pwm_200_final) != 0)
										PWM_CH1_TiempoSubida(*p_pwm_ch); //pwm 200V.
									else if ((p_table->falling_pwm_40_initial + p_table->falling_pwm_40_final) != 0)
										PWM_CH1_TiempoMantenimiento(*p_pwm_ch); //pwm 40V.
								}
								else
								{
									PWM_CH1_TiempoSubida(0); //pwm 200V.
									PWM_CH1_TiempoMantenimiento(0);
									PWM_CH1_TiempoBajada(0);
								}
								break;

							case CH2:
								if ((*p_session_burst_cnt < p_table->burst_mode_on) || ((p_table->burst_mode_on == 0) && (p_table->burst_mode_off == 0)))
								{
									if ((p_table->falling_pwm_200_initial + p_table->falling_pwm_200_final) != 0)
										PWM_CH2_TiempoSubida(*p_pwm_ch); //pwm 200V.
									else if ((p_table->falling_pwm_40_initial + p_table->falling_pwm_40_final) != 0)
										PWM_CH2_TiempoMantenimiento(*p_pwm_ch); //pwm 40V.
								}
								else
								{
									PWM_CH2_TiempoSubida(0); //pwm 200V.
									PWM_CH2_TiempoMantenimiento(0);
									PWM_CH2_TiempoBajada(0);
								}
								break;

							case CH3:
								if ((*p_session_burst_cnt < p_table->burst_mode_on) || ((p_table->burst_mode_on == 0) && (p_table->burst_mode_off == 0)))
								{
									if ((p_table->falling_pwm_200_initial + p_table->falling_pwm_200_final) != 0)
										PWM_CH3_TiempoSubida(*p_pwm_ch); //pwm 200V.
									else if ((p_table->falling_pwm_40_initial + p_table->falling_pwm_40_final) != 0)
										PWM_CH3_TiempoMantenimiento(*p_pwm_ch); //pwm 40V.
								}
								else
								{
									PWM_CH3_TiempoSubida(0); //pwm 200V.
									PWM_CH3_TiempoMantenimiento(0);
									PWM_CH3_TiempoBajada(0);
								}
								break;

							case CH4:
								if ((*p_session_burst_cnt < p_table->burst_mode_on) || ((p_table->burst_mode_on == 0) && (p_table->burst_mode_off == 0)))
								{
									if ((p_table->falling_pwm_200_initial + p_table->falling_pwm_200_final) != 0)
										PWM_CH4_TiempoSubida(*p_pwm_ch); //pwm 200V.
									else if ((p_table->falling_pwm_40_initial + p_table->falling_pwm_40_final) != 0)
										PWM_CH4_TiempoMantenimiento(*p_pwm_ch); //pwm 40V.
								}
								else
								{
									PWM_CH4_TiempoSubida(0); //pwm 200V.
									PWM_CH4_TiempoMantenimiento(0);
									PWM_CH4_TiempoBajada(0);
								}
								break;
						}
					}
				}
				else
				{
					*p_session_state = SESSION_PLATEAU_CHANNEL_LOW;


					switch(channel)
					{
						case CH1:
							PWM_CH1_TiempoSubida(0); //pwm 200V.
							PWM_CH1_TiempoMantenimiento(0);
							PWM_CH1_TiempoBajada(1000);
							break;

						case CH2:
							PWM_CH2_TiempoSubida(0); //pwm 200V.
							PWM_CH2_TiempoMantenimiento(0);
							PWM_CH2_TiempoBajada(1000);
							break;

						case CH3:
							PWM_CH3_TiempoSubida(0); //pwm 200V.
							PWM_CH3_TiempoMantenimiento(0);
							PWM_CH3_TiempoBajada(1000);
							break;

						case CH4:
							PWM_CH4_TiempoSubida(0); //pwm 200V.
							PWM_CH4_TiempoMantenimiento(0);
							PWM_CH4_TiempoBajada(1000);
							break;
					}
					*p_session_time = 0;
					*p_session_time_2 = 0;
				}
			}


			break;

		case SESSION_PLATEAU_CHANNEL_LOW:
#ifdef WITH_SYNC
			//CH1 es el que toma el tiempo
			if (ch1_sync_state & SYNC_REQUIRED)
			{
				if (channel == CH1)
				{
					ch1_sync_state &= ~SYNC_ACT_MASK;
					ch1_sync_state |= SYNC_IN_DOWN;

					if (*p_session_time >= p_table->low_step_number)
					{
						//End of cycle.
						//*p_session_burst_cnt++;
						*p_session_burst_cnt = *p_session_burst_cnt + 1;
						if (*p_session_burst_cnt == (p_table->burst_mode_on + p_table->burst_mode_off))
							*p_session_burst_cnt = 0;

						*p_session_state = SESSION_PLATEAU_CHANNEL_RISING_EDGE;

						*p_pwm_slope = (p_table->rising_pwm_200_final + p_table->rising_pwm_40_final - p_table->rising_pwm_200_initial - p_table->rising_pwm_40_initial);
						*p_pwm_slope /= (float)10;
						*p_pwm_slope /= p_session_ch->stage_2_rising_time;

						if ((*p_session_burst_cnt < p_table->burst_mode_on) || ((p_table->burst_mode_on == 0) && (p_table->burst_mode_off == 0)))
						{
							PWM_CH1_TiempoSubida(p_table->rising_pwm_200_initial); //pwm 200V.
							PWM_CH1_TiempoMantenimiento(p_table->rising_pwm_40_initial);
							PWM_CH1_TiempoBajada(p_table->rising_pwm_n_initial);
						}
						else
						{
							PWM_CH1_TiempoSubida(0); //pwm 200V.
							PWM_CH1_TiempoMantenimiento(0);
							PWM_CH1_TiempoBajada(0);
						}
						*p_session_time = 0;
						*p_session_time_2 = 0;
					}
				}
				else
				{
					switch (channel)
					{
						case CH2:
							if (ch1_sync_state & SYNC_FINISH_PLATEAU)
								return FIN_OK;

							//me quedo en LOW hasta que CH1 este en LOW
							if (ch1_sync_state & SYNC_IN_DOWN)
							{
								//End of cycle.
								//*p_session_burst_cnt++;
								*p_session_burst_cnt = *p_session_burst_cnt + 1;
								if (*p_session_burst_cnt == (p_table->burst_mode_on + p_table->burst_mode_off))
									*p_session_burst_cnt = 0;

								*p_session_state = SESSION_PLATEAU_CHANNEL_RISING_EDGE;

								*p_pwm_slope = (p_table->rising_pwm_200_final + p_table->rising_pwm_40_final - p_table->rising_pwm_200_initial - p_table->rising_pwm_40_initial);
								*p_pwm_slope /= (float)10;
								*p_pwm_slope /= p_session_ch->stage_2_rising_time;

								if ((*p_session_burst_cnt < p_table->burst_mode_on) || ((p_table->burst_mode_on == 0) && (p_table->burst_mode_off == 0)))
								{
									PWM_CH2_TiempoSubida(p_table->rising_pwm_200_initial); //pwm 200V.
									PWM_CH2_TiempoMantenimiento(p_table->rising_pwm_40_initial);
									PWM_CH2_TiempoBajada(p_table->rising_pwm_n_initial);
								}
								else
								{
									PWM_CH2_TiempoSubida(0); //pwm 200V.
									PWM_CH2_TiempoMantenimiento(0);
									PWM_CH2_TiempoBajada(0);
								}
								*p_session_time = 0;
								*p_session_time_2 = 0;
							}
							break;

						case CH3:
							if (ch1_sync_state & SYNC_FINISH_PLATEAU)
								return FIN_OK;

							//me quedo en LOW mientras CH1 este en LOW, lo atraso un poquito para evitar dobles pulsos
							//if (!(ch1_sync_state & SYNC_IN_DOWN))
							if (ch1_sync_state & SYNC_IN_RISING)
							{
								//End of cycle.
								//*p_session_burst_cnt++;
								*p_session_burst_cnt = *p_session_burst_cnt + 1;
								if (*p_session_burst_cnt == (p_table->burst_mode_on + p_table->burst_mode_off))
									*p_session_burst_cnt = 0;

								*p_session_state = SESSION_PLATEAU_CHANNEL_RISING_EDGE;

								*p_pwm_slope = (p_table->rising_pwm_200_final + p_table->rising_pwm_40_final - p_table->rising_pwm_200_initial - p_table->rising_pwm_40_initial);
								*p_pwm_slope /= (float)10;
								*p_pwm_slope /= p_session_ch->stage_2_rising_time;

								if ((*p_session_burst_cnt < p_table->burst_mode_on) || ((p_table->burst_mode_on == 0) && (p_table->burst_mode_off == 0)))
								{
									PWM_CH3_TiempoSubida(p_table->rising_pwm_200_initial); //pwm 200V.
									PWM_CH3_TiempoMantenimiento(p_table->rising_pwm_40_initial);
									PWM_CH3_TiempoBajada(p_table->rising_pwm_n_initial);
								}
								else
								{
									PWM_CH3_TiempoSubida(0); //pwm 200V.
									PWM_CH3_TiempoMantenimiento(0);
									PWM_CH3_TiempoBajada(0);
								}
								*p_session_time = 0;
								*p_session_time_2 = 0;
							}
							break;

						case CH4:
							if (ch1_sync_state & SYNC_FINISH_PLATEAU)
								return FIN_OK;

							//me quedo en LOW hasta que CH1 este en LOW
							if (ch1_sync_state & SYNC_IN_DOWN)
							{
								//End of cycle.
								//*p_session_burst_cnt++;
								*p_session_burst_cnt = *p_session_burst_cnt + 1;
								if (*p_session_burst_cnt == (p_table->burst_mode_on + p_table->burst_mode_off))
									*p_session_burst_cnt = 0;

								*p_session_state = SESSION_PLATEAU_CHANNEL_RISING_EDGE;

								*p_pwm_slope = (p_table->rising_pwm_200_final + p_table->rising_pwm_40_final - p_table->rising_pwm_200_initial - p_table->rising_pwm_40_initial);
								*p_pwm_slope /= (float)10;
								*p_pwm_slope /= p_session_ch->stage_2_rising_time;

								if ((*p_session_burst_cnt < p_table->burst_mode_on) || ((p_table->burst_mode_on == 0) && (p_table->burst_mode_off == 0)))
								{
									PWM_CH4_TiempoSubida(p_table->rising_pwm_200_initial); //pwm 200V.
									PWM_CH4_TiempoMantenimiento(p_table->rising_pwm_40_initial);
									PWM_CH4_TiempoBajada(p_table->rising_pwm_n_initial);
								}
								else
								{
									PWM_CH4_TiempoSubida(0); //pwm 200V.
									PWM_CH4_TiempoMantenimiento(0);
									PWM_CH4_TiempoBajada(0);
								}
								*p_session_time = 0;
								*p_session_time_2 = 0;
							}
							break;
					}
				}
			}
			else
			{
				if (*p_session_time >= p_table->low_step_number)
				{
					//End of cycle.
					//*p_session_burst_cnt++;
					*p_session_burst_cnt = *p_session_burst_cnt + 1;
					if (*p_session_burst_cnt == (p_table->burst_mode_on + p_table->burst_mode_off))
						*p_session_burst_cnt = 0;

					*p_session_state = SESSION_PLATEAU_CHANNEL_RISING_EDGE;

					*p_pwm_slope = (p_table->rising_pwm_200_final + p_table->rising_pwm_40_final - p_table->rising_pwm_200_initial - p_table->rising_pwm_40_initial);
					*p_pwm_slope /= (float)10;
					*p_pwm_slope /= p_session_ch->stage_2_rising_time;

					switch (channel)
					{
						case CH1:
							if ((*p_session_burst_cnt < p_table->burst_mode_on) || ((p_table->burst_mode_on == 0) && (p_table->burst_mode_off == 0)))
							{
								PWM_CH1_TiempoSubida(p_table->rising_pwm_200_initial); //pwm 200V.
								PWM_CH1_TiempoMantenimiento(p_table->rising_pwm_40_initial);
								PWM_CH1_TiempoBajada(p_table->rising_pwm_n_initial);
							}
							else
							{
								PWM_CH1_TiempoSubida(0); //pwm 200V.
								PWM_CH1_TiempoMantenimiento(0);
								PWM_CH1_TiempoBajada(0);
							}
							break;

						case CH2:
							if ((*p_session_burst_cnt < p_table->burst_mode_on) || ((p_table->burst_mode_on == 0) && (p_table->burst_mode_off == 0)))
							{
								PWM_CH2_TiempoSubida(p_table->rising_pwm_200_initial); //pwm 200V.
								PWM_CH2_TiempoMantenimiento(p_table->rising_pwm_40_initial);
								PWM_CH2_TiempoBajada(p_table->rising_pwm_n_initial);
							}
							else
							{
								PWM_CH2_TiempoSubida(0); //pwm 200V.
								PWM_CH2_TiempoMantenimiento(0);
								PWM_CH2_TiempoBajada(0);
							}
							break;

						case CH3:
							if ((*p_session_burst_cnt < p_table->burst_mode_on) || ((p_table->burst_mode_on == 0) && (p_table->burst_mode_off == 0)))
							{
								PWM_CH3_TiempoSubida(p_table->rising_pwm_200_initial); //pwm 200V.
								PWM_CH3_TiempoMantenimiento(p_table->rising_pwm_40_initial);
								PWM_CH3_TiempoBajada(p_table->rising_pwm_n_initial);
							}
							else
							{
								PWM_CH3_TiempoSubida(0); //pwm 200V.
								PWM_CH3_TiempoMantenimiento(0);
								PWM_CH3_TiempoBajada(0);
							}
							break;

						case CH4:
							if ((*p_session_burst_cnt < p_table->burst_mode_on) || ((p_table->burst_mode_on == 0) && (p_table->burst_mode_off == 0)))
							{
								PWM_CH4_TiempoSubida(p_table->rising_pwm_200_initial); //pwm 200V.
								PWM_CH4_TiempoMantenimiento(p_table->rising_pwm_40_initial);
								PWM_CH4_TiempoBajada(p_table->rising_pwm_n_initial);
							}
							else
							{
								PWM_CH4_TiempoSubida(0); //pwm 200V.
								PWM_CH4_TiempoMantenimiento(0);
								PWM_CH4_TiempoBajada(0);
							}
							break;
					}
					*p_session_time = 0;
					*p_session_time_2 = 0;
				}

			}
#else
			if (*p_session_time >= p_table->low_step_number)
			{
				//End of cycle.
				*p_session_burst_cnt++;
				if (*p_session_burst_cnt == (p_table->burst_mode_on + p_table->burst_mode_off))
					*p_session_burst_cnt = 0;

				*p_session_state = SESSION_PLATEAU_CHANNEL_RISING_EDGE;

				*p_pwm_slope = (p_table->rising_pwm_200_final + p_table->rising_pwm_40_final - p_table->rising_pwm_200_initial - p_table->rising_pwm_40_initial);
				*p_pwm_slope /= (float)10;
				*p_pwm_slope /= p_session_ch->stage_2_rising_time;

				switch (channel)
				{
					case CH1:
						if ((*p_session_burst_cnt < p_table->burst_mode_on) || ((p_table->burst_mode_on == 0) && (p_table->burst_mode_off == 0)))
						{
							PWM_CH1_TiempoSubida(p_table->rising_pwm_200_initial); //pwm 200V.
							PWM_CH1_TiempoMantenimiento(p_table->rising_pwm_40_initial);
							PWM_CH1_TiempoBajada(p_table->rising_pwm_n_initial);
						}
						else
						{
							PWM_CH1_TiempoSubida(0); //pwm 200V.
							PWM_CH1_TiempoMantenimiento(0);
							PWM_CH1_TiempoBajada(0);
						}
						break;

					case CH2:
						if ((*p_session_burst_cnt < p_table->burst_mode_on) || ((p_table->burst_mode_on == 0) && (p_table->burst_mode_off == 0)))
						{
							PWM_CH2_TiempoSubida(p_table->rising_pwm_200_initial); //pwm 200V.
							PWM_CH2_TiempoMantenimiento(p_table->rising_pwm_40_initial);
							PWM_CH2_TiempoBajada(p_table->rising_pwm_n_initial);
						}
						else
						{
							PWM_CH2_TiempoSubida(0); //pwm 200V.
							PWM_CH2_TiempoMantenimiento(0);
							PWM_CH2_TiempoBajada(0);
						}
						break;

					case CH3:
						if ((*p_session_burst_cnt < p_table->burst_mode_on) || ((p_table->burst_mode_on == 0) && (p_table->burst_mode_off == 0)))
						{
							PWM_CH3_TiempoSubida(p_table->rising_pwm_200_initial); //pwm 200V.
							PWM_CH3_TiempoMantenimiento(p_table->rising_pwm_40_initial);
							PWM_CH3_TiempoBajada(p_table->rising_pwm_n_initial);
						}
						else
						{
							PWM_CH3_TiempoSubida(0); //pwm 200V.
							PWM_CH3_TiempoMantenimiento(0);
							PWM_CH3_TiempoBajada(0);
						}
						break;

					case CH4:
						if ((*p_session_burst_cnt < p_table->burst_mode_on) || ((p_table->burst_mode_on == 0) && (p_table->burst_mode_off == 0)))
						{
							PWM_CH4_TiempoSubida(p_table->rising_pwm_200_initial); //pwm 200V.
							PWM_CH4_TiempoMantenimiento(p_table->rising_pwm_40_initial);
							PWM_CH4_TiempoBajada(p_table->rising_pwm_n_initial);
						}
						else
						{
							PWM_CH4_TiempoSubida(0); //pwm 200V.
							PWM_CH4_TiempoMantenimiento(0);
							PWM_CH4_TiempoBajada(0);
						}
						break;
				}
				*p_session_time = 0;
				*p_session_time_2 = 0;
			}

#endif

			if (*p_stage_time == 0)
			{
				*p_session_state = SESSION_PLATEAU_CHANNEL_INIT;
				*p_session_burst_cnt = 0;
				//termino el stage, aviso a los canales
				if (channel == CH1)
				{
					ch1_sync_state &= SYNC_REQUIRED;
					ch1_sync_state |= SYNC_FINISH_PLATEAU;
				}

				return FIN_OK;
			}
			break;

		case SESSION_PLATEAU_CHANNEL_END_ERROR:

			*p_session_state = SESSION_PLATEAU_CHANNEL_INIT;
			return FIN_ERROR;
			break;
	}

	//Stage enable.
	//Signal parameters calculate.
	//Signal form.

	//--- end ---//


	return TRABAJANDO;
}

//------ NUEVA COOLING DOWN ------//
unsigned char Session_Cooling_Down_Channels (unsigned char channel)
{

	unsigned char i;
	unsigned short time_aux = 0;
	unsigned char * p_session_state;
	unsigned char * p_session_channel_step;
	session_typedef * p_session_ch;
	unsigned short * p_session_burst_cnt;
	warningup_coolingdown_typedef * p_table;
	float * p_pwm_slope;
	float * p_pwm_ch;
	unsigned int * p_stage_time;
	unsigned short * p_session_time;
	unsigned short * p_session_time_2;
	unsigned char * p_fall_type;

	//primero acomodo los punteros
	switch (channel)
	{
		case CH1:
			p_session_state = &session_cooling_down_channel_1_state;
			p_session_channel_step = &session_cooling_down_channel_1_step;
			p_session_ch = &session_ch_1;
			p_session_burst_cnt = &session_cooling_down_channel_1_burst_cnt;
			p_table = &table_cooling_down_channel_1[0];
			p_pwm_slope = &pwm_slope_channel_1;
			p_stage_time = &session_cooling_down_channel_1_stage_time;
			p_session_time = &session_cooling_down_channel_1_time;
			p_session_time_2 = &session_cooling_down_channel_1_time_2;
			p_pwm_ch = &pwm_channel_1;
			p_fall_type = &fall_type_step_ch1;
			break;

		case CH2:
			p_session_state = &session_cooling_down_channel_2_state;
			p_session_channel_step = &session_cooling_down_channel_2_step;
			p_session_ch = &session_ch_2;
			p_session_burst_cnt = &session_cooling_down_channel_2_burst_cnt;
			p_table = &table_cooling_down_channel_2[0];
			p_pwm_slope = &pwm_slope_channel_2;
			p_stage_time = &session_cooling_down_channel_2_stage_time;
			p_session_time = &session_cooling_down_channel_2_time;
			p_session_time_2 = &session_cooling_down_channel_2_time_2;
			p_pwm_ch = &pwm_channel_2;
			p_fall_type = &fall_type_step_ch2;
			break;

		case CH3:
			p_session_state = &session_cooling_down_channel_3_state;
			p_session_channel_step = &session_cooling_down_channel_3_step;
			p_session_ch = &session_ch_3;
			p_session_burst_cnt = &session_cooling_down_channel_3_burst_cnt;
			p_table = &table_cooling_down_channel_3[0];
			p_pwm_slope = &pwm_slope_channel_3;
			p_stage_time = &session_cooling_down_channel_3_stage_time;
			p_session_time = &session_cooling_down_channel_3_time;
			p_session_time_2 = &session_cooling_down_channel_3_time_2;
			p_pwm_ch = &pwm_channel_3;
			p_fall_type = &fall_type_step_ch3;
			break;

		case CH4:
			p_session_state = &session_cooling_down_channel_4_state;
			p_session_channel_step = &session_cooling_down_channel_4_step;
			p_session_ch = &session_ch_4;
			p_session_burst_cnt = &session_cooling_down_channel_4_burst_cnt;
			p_table = &table_cooling_down_channel_4[0];
			p_pwm_slope = &pwm_slope_channel_4;
			p_stage_time = &session_cooling_down_channel_4_stage_time;
			p_session_time = &session_cooling_down_channel_4_time;
			p_session_time_2 = &session_cooling_down_channel_4_time_2;
			p_pwm_ch = &pwm_channel_4;
			p_fall_type = &fall_type_step_ch4;
			break;

		default:
			return FIN_ERROR;
			break;
	}

	switch (*p_session_state)
	{
		case SESSION_COOLING_DOWN_CHANNEL_INIT:

			*p_session_channel_step = 0;

			if (p_session_ch->stage_3_status)
			{
#ifdef WITH_SYNC
				//reviso si tengo que utilizar SYNC
				if (p_session_ch->sync_on)
				{
					//debo tener canal 1 presente
					if (channel == CH1)
					{
						ch1_sync_state |= SYNC_REQUIRED;
						*p_session_state = SESSION_COOLING_DOWN_CHANNEL_PARAMETERS_CALCULATE;
					}
					else
					{
						//reviso que este presente CH1 l antena y deberia estar revisada
						if (ch1_sync_state & SYNC_REQUIRED)
						{
							//CH1 presente
							*p_session_state = SESSION_COOLING_DOWN_CHANNEL_PARAMETERS_CALCULATE;
							sync_in_waiting = 0;
						}
						else
						{
							//no esta CH1
							*p_session_state = SESSION_COOLING_DOWN_CHANNEL_END_ERROR;

							sprintf(&buffSendErr[0], "ERROR in CH%d: not CH1 detected and sync is needed\r\n", channel);
							UART_PC_Send(&buffSendErr[0]);
							sync_in_waiting = 0;
							return FIN_ERROR;
						}
					}
				}
				else
				{
					sprintf(&buffSendErr[0], "\r\nCH%d no sync needed", channel);
					UART_PC_Send(&buffSendErr[0]);
					ch1_sync_state &= NO_SYNC_REQUIRED_MASK;
					*p_session_state = SESSION_COOLING_DOWN_CHANNEL_PARAMETERS_CALCULATE;
				}
#else
				*p_session_state = SESSION_COOLING_DOWN_CHANNEL_PARAMETERS_CALCULATE;
#endif
			}
			else
				return FIN_OK;
			break;

		case SESSION_COOLING_DOWN_CHANNEL_PARAMETERS_CALCULATE:

			i = Session_Channels_Parameters_Calculate(channel, COOLING_DOWN);	//retorna FIN_OK o FIN_ERROR rutina nueva 19-03-15

			if (i == FIN_OK)
			{
				*p_session_burst_cnt = 0;

				//--- Slope ---//
				*p_pwm_slope = (p_table + (*p_session_channel_step))->rising_pwm_200_final + (p_table + (*p_session_channel_step))->rising_pwm_40_final - (p_table + (*p_session_channel_step))->rising_pwm_200_initial - (p_table + (*p_session_channel_step))->rising_pwm_40_initial;
				*p_pwm_slope /= (float)10;
				*p_pwm_slope /= p_session_ch->stage_3_rising_time;

				//--- Next state ---//
				*p_session_state = SESSION_COOLING_DOWN_CHANNEL_RISING_EDGE;

				//--- Time of the next step ---//
				*p_stage_time = p_session_ch->stage_3_time_per_step;

				*p_session_time = 0;
				*p_session_time_2 = 0;

				//--- Next step ---//
				*p_session_channel_step = 0;

				//--- PWM initial values ---//
				switch (channel)
				{
					case CH1:
						if (*p_session_burst_cnt < (p_table + (*p_session_channel_step))->burst_mode_on)
						{
							PWM_CH1_TiempoSubida((p_table + (*p_session_channel_step))->rising_pwm_200_initial); //pwm 200V.
							PWM_CH1_TiempoMantenimiento((p_table + (*p_session_channel_step))->rising_pwm_40_initial);
							PWM_CH1_TiempoBajada((p_table + (*p_session_channel_step))->rising_pwm_n_initial);
						}
						else
						{
							PWM_CH1_TiempoSubida(0); //pwm 200V.
							PWM_CH1_TiempoMantenimiento(0);
							PWM_CH1_TiempoBajada(0);
						}
						break;

					case CH2:
						if (*p_session_burst_cnt < (p_table + (*p_session_channel_step))->burst_mode_on)
						{
							PWM_CH2_TiempoSubida((p_table + (*p_session_channel_step))->rising_pwm_200_initial); //pwm 200V.
							PWM_CH2_TiempoMantenimiento((p_table + (*p_session_channel_step))->rising_pwm_40_initial);
							PWM_CH2_TiempoBajada((p_table + (*p_session_channel_step))->rising_pwm_n_initial);
						}
						else
						{
							PWM_CH2_TiempoSubida(0); //pwm 200V.
							PWM_CH2_TiempoMantenimiento(0);
							PWM_CH2_TiempoBajada(0);
						}
						break;

					case CH3:
						if (*p_session_burst_cnt < (p_table + (*p_session_channel_step))->burst_mode_on)
						{
							PWM_CH3_TiempoSubida((p_table + (*p_session_channel_step))->rising_pwm_200_initial); //pwm 200V.
							PWM_CH3_TiempoMantenimiento((p_table + (*p_session_channel_step))->rising_pwm_40_initial);
							PWM_CH3_TiempoBajada((p_table + (*p_session_channel_step))->rising_pwm_n_initial);
						}
						else
						{
							PWM_CH3_TiempoSubida(0); //pwm 200V.
							PWM_CH3_TiempoMantenimiento(0);
							PWM_CH3_TiempoBajada(0);
						}
						break;

					case CH4:
						if (*p_session_burst_cnt < (p_table + (*p_session_channel_step))->burst_mode_on)
						{
							PWM_CH4_TiempoSubida((p_table + (*p_session_channel_step))->rising_pwm_200_initial); //pwm 200V.
							PWM_CH4_TiempoMantenimiento((p_table + (*p_session_channel_step))->rising_pwm_40_initial);
							PWM_CH4_TiempoBajada((p_table + (*p_session_channel_step))->rising_pwm_n_initial);
						}
						else
						{
							PWM_CH4_TiempoSubida(0); //pwm 200V.
							PWM_CH4_TiempoMantenimiento(0);
							PWM_CH4_TiempoBajada(0);
						}
						break;
				}
			}

			else if (i == FIN_ERROR)
			{
				*p_session_state = SESSION_COOLING_DOWN_CHANNEL_END_ERROR;

				sprintf(&buffSendErr[0], (const char *) "ERROR(0x%03X)\r\n", ERR_CHANNEL_COOLING_DOWN_PARAMETERS_CALCULATE(channel));
				UART_PC_Send(&buffSendErr[0]);
			}
			break;

		case SESSION_COOLING_DOWN_CHANNEL_CHANGE_LEVEL:

			if (*p_stage_time == 0)
			{
				if (*p_session_channel_step < (SESSION_WUP_CH1_BUFF_DIM - 1))
				{
					//--- Time per step ---//
					*p_stage_time = p_session_ch->stage_3_time_per_step;

					//--- Next state ---//
					*p_session_state = SESSION_COOLING_DOWN_CHANNEL_RISING_EDGE;

					//--- Next step ---//
					*p_session_channel_step += 1;

					//--- Slope ---//
					*p_pwm_slope = (p_table + (*p_session_channel_step))->rising_pwm_200_final + (p_table + (*p_session_channel_step))->rising_pwm_40_final - (p_table + (*p_session_channel_step))->rising_pwm_200_initial - (p_table + (*p_session_channel_step))->rising_pwm_40_initial;
					*p_pwm_slope /= (float)10;
					*p_pwm_slope /= p_session_ch->stage_3_rising_time;

					switch (channel)
					{
						case CH1:
							if ((*p_session_burst_cnt < (p_table + (*p_session_channel_step))->burst_mode_on) ||
									(	((p_table + (*p_session_channel_step))->burst_mode_on == 0) && ((p_table + (*p_session_channel_step))->burst_mode_off == 0)))
							{
								PWM_CH1_TiempoSubida((p_table + (*p_session_channel_step))->rising_pwm_200_initial); //pwm 200V.
								PWM_CH1_TiempoMantenimiento((p_table + (*p_session_channel_step))->rising_pwm_40_initial);
								PWM_CH1_TiempoBajada((p_table + (*p_session_channel_step))->rising_pwm_n_initial);
							}
							else
							{
								PWM_CH1_TiempoSubida(0); //pwm 200V.
								PWM_CH1_TiempoMantenimiento(0);
								PWM_CH1_TiempoBajada(0);
							}
							break;

						case CH2:
							if ((*p_session_burst_cnt < (p_table + (*p_session_channel_step))->burst_mode_on) ||
									(	((p_table + (*p_session_channel_step))->burst_mode_on == 0) && ((p_table + (*p_session_channel_step))->burst_mode_off == 0)))
							{
								PWM_CH2_TiempoSubida((p_table + (*p_session_channel_step))->rising_pwm_200_initial); //pwm 200V.
								PWM_CH2_TiempoMantenimiento((p_table + (*p_session_channel_step))->rising_pwm_40_initial);
								PWM_CH2_TiempoBajada((p_table + (*p_session_channel_step))->rising_pwm_n_initial);
							}
							else
							{
								PWM_CH2_TiempoSubida(0); //pwm 200V.
								PWM_CH2_TiempoMantenimiento(0);
								PWM_CH2_TiempoBajada(0);
							}
							break;

						case CH3:
							if ((*p_session_burst_cnt < (p_table + (*p_session_channel_step))->burst_mode_on) ||
									(	((p_table + (*p_session_channel_step))->burst_mode_on == 0) && ((p_table + (*p_session_channel_step))->burst_mode_off == 0)))
							{
								PWM_CH3_TiempoSubida((p_table + (*p_session_channel_step))->rising_pwm_200_initial); //pwm 200V.
								PWM_CH3_TiempoMantenimiento((p_table + (*p_session_channel_step))->rising_pwm_40_initial);
								PWM_CH3_TiempoBajada((p_table + (*p_session_channel_step))->rising_pwm_n_initial);
							}
							else
							{
								PWM_CH3_TiempoSubida(0); //pwm 200V.
								PWM_CH3_TiempoMantenimiento(0);
								PWM_CH3_TiempoBajada(0);
							}
							break;

						case CH4:
							if ((*p_session_burst_cnt < (p_table + (*p_session_channel_step))->burst_mode_on) ||
									(	((p_table + (*p_session_channel_step))->burst_mode_on == 0) && ((p_table + (*p_session_channel_step))->burst_mode_off == 0)))
							{
								PWM_CH4_TiempoSubida((p_table + (*p_session_channel_step))->rising_pwm_200_initial); //pwm 200V.
								PWM_CH4_TiempoMantenimiento((p_table + (*p_session_channel_step))->rising_pwm_40_initial);
								PWM_CH4_TiempoBajada((p_table + (*p_session_channel_step))->rising_pwm_n_initial);
							}
							else
							{
								PWM_CH4_TiempoSubida(0); //pwm 200V.
								PWM_CH4_TiempoMantenimiento(0);
								PWM_CH4_TiempoBajada(0);
							}
							break;
					}

					*p_session_time = 0;
					*p_session_time_2 = 0;

				}
				else
				{
					//termino el stage, aviso a los canales
					if ((channel == CH1) && (ch1_sync_state & SYNC_REQUIRED))
					{
						ch1_sync_state &= SYNC_REQUIRED;
						ch1_sync_state |= SYNC_FINISH_COOLING_DOWN;
					}
					return FIN_OK;
				}
			}
			else
			{
				//--- Next state ---//
				*p_session_state = SESSION_COOLING_DOWN_CHANNEL_RISING_EDGE;

				*p_pwm_slope = (p_table + (*p_session_channel_step))->rising_pwm_200_final + (p_table + (*p_session_channel_step))->rising_pwm_40_final - (p_table + (*p_session_channel_step))->rising_pwm_200_initial - (p_table + (*p_session_channel_step))->rising_pwm_40_initial;
				*p_pwm_slope /= (float)10;
				*p_pwm_slope /= p_session_ch->stage_3_rising_time;

				switch (channel)
				{
					case CH1:
						if ((*p_session_burst_cnt < (p_table + (*p_session_channel_step))->burst_mode_on) ||
								(	((p_table + (*p_session_channel_step))->burst_mode_on == 0) && ((p_table + (*p_session_channel_step))->burst_mode_off == 0)))
						{
							PWM_CH1_TiempoSubida((p_table + (*p_session_channel_step))->rising_pwm_200_initial); //pwm 200V.
							PWM_CH1_TiempoMantenimiento((p_table + (*p_session_channel_step))->rising_pwm_40_initial);
							PWM_CH1_TiempoBajada((p_table + (*p_session_channel_step))->rising_pwm_n_initial);
						}
						else
						{
							PWM_CH1_TiempoSubida(0); //pwm 200V.
							PWM_CH1_TiempoMantenimiento(0);
							PWM_CH1_TiempoBajada(0);
						}
						break;

					case CH2:
						if ((*p_session_burst_cnt < (p_table + (*p_session_channel_step))->burst_mode_on) ||
								(	((p_table + (*p_session_channel_step))->burst_mode_on == 0) && ((p_table + (*p_session_channel_step))->burst_mode_off == 0)))
						{
							PWM_CH2_TiempoSubida((p_table + (*p_session_channel_step))->rising_pwm_200_initial); //pwm 200V.
							PWM_CH2_TiempoMantenimiento((p_table + (*p_session_channel_step))->rising_pwm_40_initial);
							PWM_CH2_TiempoBajada((p_table + (*p_session_channel_step))->rising_pwm_n_initial);
						}
						else
						{
							PWM_CH2_TiempoSubida(0); //pwm 200V.
							PWM_CH2_TiempoMantenimiento(0);
							PWM_CH2_TiempoBajada(0);
						}
						break;

					case CH3:
						if ((*p_session_burst_cnt < (p_table + (*p_session_channel_step))->burst_mode_on) ||
								(	((p_table + (*p_session_channel_step))->burst_mode_on == 0) && ((p_table + (*p_session_channel_step))->burst_mode_off == 0)))
						{
							PWM_CH3_TiempoSubida((p_table + (*p_session_channel_step))->rising_pwm_200_initial); //pwm 200V.
							PWM_CH3_TiempoMantenimiento((p_table + (*p_session_channel_step))->rising_pwm_40_initial);
							PWM_CH3_TiempoBajada((p_table + (*p_session_channel_step))->rising_pwm_n_initial);
						}
						else
						{
							PWM_CH3_TiempoSubida(0); //pwm 200V.
							PWM_CH3_TiempoMantenimiento(0);
							PWM_CH3_TiempoBajada(0);
						}
						break;

					case CH4:
						if ((*p_session_burst_cnt < (p_table + (*p_session_channel_step))->burst_mode_on) ||
								(	((p_table + (*p_session_channel_step))->burst_mode_on == 0) && ((p_table + (*p_session_channel_step))->burst_mode_off == 0)))
						{
							PWM_CH4_TiempoSubida((p_table + (*p_session_channel_step))->rising_pwm_200_initial); //pwm 200V.
							PWM_CH4_TiempoMantenimiento((p_table + (*p_session_channel_step))->rising_pwm_40_initial);
							PWM_CH4_TiempoBajada((p_table + (*p_session_channel_step))->rising_pwm_n_initial);
						}
						else
						{
							PWM_CH4_TiempoSubida(0); //pwm 200V.
							PWM_CH4_TiempoMantenimiento(0);
							PWM_CH4_TiempoBajada(0);
						}
						break;
				}

				*p_session_time = 0;
				*p_session_time_2 = 0;

			}

			break;

		case SESSION_COOLING_DOWN_CHANNEL_RISING_EDGE:
#ifdef WITH_SYNC
			if ((channel == CH1) && (ch1_sync_state & SYNC_REQUIRED))
			{
				ch1_sync_state &= ~SYNC_ACT_MASK;
				ch1_sync_state |= SYNC_IN_RISING;
			}
#endif

			if (*p_session_time <= (p_table + (*p_session_channel_step))->rising_step_number)
			{
				if (*p_session_time_2 != *p_session_time)		//para ejecutar un vez sola
				{
					*p_session_time_2 = *p_session_time;

					*p_pwm_ch = (p_table + (*p_session_channel_step))->rising_pwm_200_initial + (p_table + (*p_session_channel_step))->rising_pwm_40_initial + *p_pwm_slope * *p_session_time;

					switch (channel)
					{
						case CH1:
							if ((*p_session_burst_cnt < (p_table + (*p_session_channel_step))->burst_mode_on) ||
									(	((p_table + (*p_session_channel_step))->burst_mode_on == 0) && ((p_table + (*p_session_channel_step))->burst_mode_off == 0)))
							{
								if (((p_table + (*p_session_channel_step))->rising_pwm_200_initial + (p_table + (*p_session_channel_step))->rising_pwm_200_final) != 0)
									PWM_CH1_TiempoSubida(*p_pwm_ch); //pwm 200V.
								else if (((p_table + (*p_session_channel_step))->rising_pwm_40_initial + (p_table + (*p_session_channel_step))->rising_pwm_40_final) != 0)
									PWM_CH1_TiempoMantenimiento(*p_pwm_ch); //pwm 40V.
							}
							else
							{
								PWM_CH1_TiempoSubida(0); //pwm 200V.
								PWM_CH1_TiempoMantenimiento(0);
								PWM_CH1_TiempoBajada(0);
							}
							break;

						case CH2:
							if ((*p_session_burst_cnt < (p_table + (*p_session_channel_step))->burst_mode_on) ||
									(	((p_table + (*p_session_channel_step))->burst_mode_on == 0) && ((p_table + (*p_session_channel_step))->burst_mode_off == 0)))
							{
								if (((p_table + (*p_session_channel_step))->rising_pwm_200_initial + (p_table + (*p_session_channel_step))->rising_pwm_200_final) != 0)
									PWM_CH2_TiempoSubida(*p_pwm_ch); //pwm 200V.
								else if (((p_table + (*p_session_channel_step))->rising_pwm_40_initial + (p_table + (*p_session_channel_step))->rising_pwm_40_final) != 0)
									PWM_CH2_TiempoMantenimiento(*p_pwm_ch); //pwm 40V.
							}
							else
							{
								PWM_CH2_TiempoSubida(0); //pwm 200V.
								PWM_CH2_TiempoMantenimiento(0);
								PWM_CH2_TiempoBajada(0);
							}
							break;

						case CH3:
							if ((*p_session_burst_cnt < (p_table + (*p_session_channel_step))->burst_mode_on) ||
									(	((p_table + (*p_session_channel_step))->burst_mode_on == 0) && ((p_table + (*p_session_channel_step))->burst_mode_off == 0)))
							{
								if (((p_table + (*p_session_channel_step))->rising_pwm_200_initial + (p_table + (*p_session_channel_step))->rising_pwm_200_final) != 0)
									PWM_CH3_TiempoSubida(*p_pwm_ch); //pwm 200V.
								else if (((p_table + (*p_session_channel_step))->rising_pwm_40_initial + (p_table + (*p_session_channel_step))->rising_pwm_40_final) != 0)
									PWM_CH3_TiempoMantenimiento(*p_pwm_ch); //pwm 40V.
							}
							else
							{
								PWM_CH3_TiempoSubida(0); //pwm 200V.
								PWM_CH3_TiempoMantenimiento(0);
								PWM_CH3_TiempoBajada(0);
							}
							break;

						case CH4:
							if ((*p_session_burst_cnt < (p_table + (*p_session_channel_step))->burst_mode_on) ||
									(	((p_table + (*p_session_channel_step))->burst_mode_on == 0) && ((p_table + (*p_session_channel_step))->burst_mode_off == 0)))
							{
								if (((p_table + (*p_session_channel_step))->rising_pwm_200_initial + (p_table + (*p_session_channel_step))->rising_pwm_200_final) != 0)
									PWM_CH4_TiempoSubida(*p_pwm_ch); //pwm 200V.
								else if (((p_table + (*p_session_channel_step))->rising_pwm_40_initial + (p_table + (*p_session_channel_step))->rising_pwm_40_final) != 0)
									PWM_CH4_TiempoMantenimiento(*p_pwm_ch); //pwm 40V.
							}
							else
							{
								PWM_CH4_TiempoSubida(0); //pwm 200V.
								PWM_CH4_TiempoMantenimiento(0);
								PWM_CH4_TiempoBajada(0);
							}
							break;
					}

				}
			}
			else
			{
				//--- Next state ---//
				*p_session_state = SESSION_COOLING_DOWN_CHANNEL_MAINTENANCE;
				*p_session_time = 0;
				*p_session_time_2 = 0;

				//--- PWM initial values ---//
				switch (channel)
				{
					case CH1:
						if ((*p_session_burst_cnt < (p_table + (*p_session_channel_step))->burst_mode_on) ||
								(	((p_table + (*p_session_channel_step))->burst_mode_on == 0) && ((p_table + (*p_session_channel_step))->burst_mode_off == 0)))
						{
							PWM_CH1_TiempoSubida((p_table + (*p_session_channel_step))->maintenance_pwm_200); //pwm 200V.
							PWM_CH1_TiempoMantenimiento((p_table + (*p_session_channel_step))->maintenance_pwm_40); //pwm 40V.
							PWM_CH1_TiempoBajada((p_table + (*p_session_channel_step))->maintenance_pwm_n);
						}
						else
						{
							PWM_CH1_TiempoSubida(0); //pwm 200V.
							PWM_CH1_TiempoMantenimiento(0);
							PWM_CH1_TiempoBajada(0);
						}
						break;

					case CH2:
						if ((*p_session_burst_cnt < (p_table + (*p_session_channel_step))->burst_mode_on) ||
								(	((p_table + (*p_session_channel_step))->burst_mode_on == 0) && ((p_table + (*p_session_channel_step))->burst_mode_off == 0)))
						{
							PWM_CH2_TiempoSubida((p_table + (*p_session_channel_step))->maintenance_pwm_200); //pwm 200V.
							PWM_CH2_TiempoMantenimiento((p_table + (*p_session_channel_step))->maintenance_pwm_40); //pwm 40V.
							PWM_CH2_TiempoBajada((p_table + (*p_session_channel_step))->maintenance_pwm_n);
						}
						else
						{
							PWM_CH2_TiempoSubida(0); //pwm 200V.
							PWM_CH2_TiempoMantenimiento(0);
							PWM_CH2_TiempoBajada(0);
						}
						break;

					case CH3:
						if ((*p_session_burst_cnt < (p_table + (*p_session_channel_step))->burst_mode_on) ||
								(	((p_table + (*p_session_channel_step))->burst_mode_on == 0) && ((p_table + (*p_session_channel_step))->burst_mode_off == 0)))
						{
							PWM_CH3_TiempoSubida((p_table + (*p_session_channel_step))->maintenance_pwm_200); //pwm 200V.
							PWM_CH3_TiempoMantenimiento((p_table + (*p_session_channel_step))->maintenance_pwm_40); //pwm 40V.
							PWM_CH3_TiempoBajada((p_table + (*p_session_channel_step))->maintenance_pwm_n);
						}
						else
						{
							PWM_CH3_TiempoSubida(0); //pwm 200V.
							PWM_CH3_TiempoMantenimiento(0);
							PWM_CH3_TiempoBajada(0);
						}
						break;

					case CH4:
						if ((*p_session_burst_cnt < (p_table + (*p_session_channel_step))->burst_mode_on) ||
								(	((p_table + (*p_session_channel_step))->burst_mode_on == 0) && ((p_table + (*p_session_channel_step))->burst_mode_off == 0)))
						{
							PWM_CH4_TiempoSubida((p_table + (*p_session_channel_step))->maintenance_pwm_200); //pwm 200V.
							PWM_CH4_TiempoMantenimiento((p_table + (*p_session_channel_step))->maintenance_pwm_40); //pwm 40V.
							PWM_CH4_TiempoBajada((p_table + (*p_session_channel_step))->maintenance_pwm_n);
						}
						else
						{
							PWM_CH4_TiempoSubida(0); //pwm 200V.
							PWM_CH4_TiempoMantenimiento(0);
							PWM_CH4_TiempoBajada(0);
						}
						break;
				}

			}
			break;

		case SESSION_COOLING_DOWN_CHANNEL_MAINTENANCE:
#ifdef WITH_SYNC
			if ((channel == CH1) && (ch1_sync_state & SYNC_REQUIRED))
			{
				ch1_sync_state &= ~SYNC_ACT_MASK;
				ch1_sync_state |= SYNC_IN_MAINTENANCE;
			}
#endif

			if (*p_session_time >= (p_table + (*p_session_channel_step))->maintenance_step_number)
			{
				//--- Next state ---//
				*p_session_state = SESSION_COOLING_DOWN_CHANNEL_FALLING_EDGE;

				//--- Slope ---//
				*p_pwm_slope = (p_table + (*p_session_channel_step))->falling_pwm_200_initial + (p_table + (*p_session_channel_step))->falling_pwm_40_initial - (p_table + (*p_session_channel_step))->falling_pwm_200_final - (p_table + (*p_session_channel_step))->falling_pwm_40_final;
				*p_pwm_slope /= (float)10;
				*p_pwm_slope /= p_session_ch->stage_3_falling_time;

				*p_session_time = 0;
				*p_session_time_2 = 0;

				//--- PWM initial values ---//
				switch (channel)
				{
					case CH1:
						if ((*p_session_burst_cnt < (p_table + (*p_session_channel_step))->burst_mode_on) ||
								(	((p_table + (*p_session_channel_step))->burst_mode_on == 0) && ((p_table + (*p_session_channel_step))->burst_mode_off == 0)))
						{
							PWM_CH1_TiempoSubida((p_table + (*p_session_channel_step))->falling_pwm_200_initial); //pwm 200V.
							PWM_CH1_TiempoMantenimiento((p_table + (*p_session_channel_step))->falling_pwm_40_initial); //pwm 40V.
							PWM_CH1_TiempoBajada((p_table + (*p_session_channel_step))->falling_pwm_n_initial);
						}
						else
						{
							PWM_CH1_TiempoSubida(0); //pwm 200V.
							PWM_CH1_TiempoMantenimiento(0);
							PWM_CH1_TiempoBajada(0);
						}
						break;

					case CH2:
						if ((*p_session_burst_cnt < (p_table + (*p_session_channel_step))->burst_mode_on) ||
								(	((p_table + (*p_session_channel_step))->burst_mode_on == 0) && ((p_table + (*p_session_channel_step))->burst_mode_off == 0)))
						{
							PWM_CH2_TiempoSubida((p_table + (*p_session_channel_step))->falling_pwm_200_initial); //pwm 200V.
							PWM_CH2_TiempoMantenimiento((p_table + (*p_session_channel_step))->falling_pwm_40_initial); //pwm 40V.
							PWM_CH2_TiempoBajada((p_table + (*p_session_channel_step))->falling_pwm_n_initial);
						}
						else
						{
							PWM_CH2_TiempoSubida(0); //pwm 200V.
							PWM_CH2_TiempoMantenimiento(0);
							PWM_CH2_TiempoBajada(0);
						}
						break;

					case CH3:
						if ((*p_session_burst_cnt < (p_table + (*p_session_channel_step))->burst_mode_on) ||
								(	((p_table + (*p_session_channel_step))->burst_mode_on == 0) && ((p_table + (*p_session_channel_step))->burst_mode_off == 0)))
						{
							PWM_CH3_TiempoSubida((p_table + (*p_session_channel_step))->falling_pwm_200_initial); //pwm 200V.
							PWM_CH3_TiempoMantenimiento((p_table + (*p_session_channel_step))->falling_pwm_40_initial); //pwm 40V.
							PWM_CH3_TiempoBajada((p_table + (*p_session_channel_step))->falling_pwm_n_initial);
						}
						else
						{
							PWM_CH3_TiempoSubida(0); //pwm 200V.
							PWM_CH3_TiempoMantenimiento(0);
							PWM_CH3_TiempoBajada(0);
						}
						break;

					case CH4:
						if ((*p_session_burst_cnt < (p_table + (*p_session_channel_step))->burst_mode_on) ||
								(	((p_table + (*p_session_channel_step))->burst_mode_on == 0) && ((p_table + (*p_session_channel_step))->burst_mode_off == 0)))
						{
							PWM_CH4_TiempoSubida((p_table + (*p_session_channel_step))->falling_pwm_200_initial); //pwm 200V.
							PWM_CH4_TiempoMantenimiento((p_table + (*p_session_channel_step))->falling_pwm_40_initial); //pwm 40V.
							PWM_CH4_TiempoBajada((p_table + (*p_session_channel_step))->falling_pwm_n_initial);
						}
						else
						{
							PWM_CH4_TiempoSubida(0); //pwm 200V.
							PWM_CH4_TiempoMantenimiento(0);
							PWM_CH4_TiempoBajada(0);
						}
						break;
				}
			}
			break;

		case SESSION_COOLING_DOWN_CHANNEL_FALLING_EDGE:
#ifdef WITH_SYNC
			if ((channel == CH1) && (ch1_sync_state & SYNC_REQUIRED))
			{
				ch1_sync_state &= ~SYNC_ACT_MASK;
				ch1_sync_state |= SYNC_IN_FALLING;
			}
#endif

			//primero reviso en que tipo de bajada estoy
			//BAJADA FALLING_LR
			if ((p_table + (*p_session_channel_step))->falling_type == FALLING_LR)
			{
				switch(*p_fall_type)
				{
					case FALL_START:
						time_aux = (p_table + (*p_session_channel_step))->falling_step_number;
						time_aux >>= 2;
						if (*p_session_time > ((p_table + (*p_session_channel_step))->falling_step_number - time_aux))
						{
							switch(channel)
							{
								case CH1:
									PWM_CH1_TiempoBajada(1000);
									break;

								case CH2:
									PWM_CH2_TiempoBajada(1000);
									break;

								case CH3:
									PWM_CH3_TiempoBajada(1000);
									break;

								case CH4:
									PWM_CH4_TiempoBajada(1000);
									break;
							}
							*p_fall_type = FALL_MED;
						}
						break;

					case FALL_MED:
						if (*p_session_time >= (p_table + (*p_session_channel_step))->falling_step_number)
							*p_fall_type = FALL_FAST;
						break;

					case FALL_FAST:
						*p_session_state = SESSION_COOLING_DOWN_CHANNEL_LOW;

						switch(channel)
						{
							case CH1:
								PWM_CH1_TiempoSubida(0); //pwm 200V.
								PWM_CH1_TiempoMantenimiento(0);
								PWM_CH1_TiempoBajada(1000);
								break;

							case CH2:
								PWM_CH2_TiempoSubida(0); //pwm 200V.
								PWM_CH2_TiempoMantenimiento(0);
								PWM_CH2_TiempoBajada(1000);
								break;

							case CH3:
								PWM_CH3_TiempoSubida(0); //pwm 200V.
								PWM_CH3_TiempoMantenimiento(0);
								PWM_CH3_TiempoBajada(1000);
								break;

							case CH4:
								PWM_CH4_TiempoSubida(0); //pwm 200V.
								PWM_CH4_TiempoMantenimiento(0);
								PWM_CH4_TiempoBajada(1000);
								break;
						}

						*p_session_time = 0;
						*p_session_time_2 = 0;
						*p_fall_type = FALL_START;
						break;

					default:
						*p_fall_type = FALL_START;
						break;
				}
			}

			//BAJADA FALLING_FAST_DISCHARGE
			if ((p_table + (*p_session_channel_step))->falling_type == FALLING_FAST_DISCHARGE)
			{
				switch(*p_fall_type)
				{
					case FALL_START:

						switch(channel)
						{
							case CH1:
								PWM_CH1_TiempoSubida(0); //pwm 200V.
								PWM_CH1_TiempoMantenimiento(0);
								PWM_CH1_TiempoBajada(1000);
								break;

							case CH2:
								PWM_CH2_TiempoSubida(0); //pwm 200V.
								PWM_CH2_TiempoMantenimiento(0);
								PWM_CH2_TiempoBajada(1000);
								break;

							case CH3:
								PWM_CH3_TiempoSubida(0); //pwm 200V.
								PWM_CH3_TiempoMantenimiento(0);
								PWM_CH3_TiempoBajada(1000);
								break;

							case CH4:
								PWM_CH4_TiempoSubida(0); //pwm 200V.
								PWM_CH4_TiempoMantenimiento(0);
								PWM_CH4_TiempoBajada(1000);
								break;
						}
						*p_fall_type = FALL_MED;
						break;

					case FALL_MED:
						if (*p_session_time >= (p_table + (*p_session_channel_step))->falling_step_number)
							*p_fall_type = FALL_FAST;
						break;

					case FALL_FAST:
						*p_session_state = SESSION_COOLING_DOWN_CHANNEL_LOW;

						switch(channel)
						{
							case CH1:
								PWM_CH1_TiempoSubida(0); //pwm 200V.
								PWM_CH1_TiempoMantenimiento(0);
								PWM_CH1_TiempoBajada(1000);
								break;

							case CH2:
								PWM_CH2_TiempoSubida(0); //pwm 200V.
								PWM_CH2_TiempoMantenimiento(0);
								PWM_CH2_TiempoBajada(1000);
								break;

							case CH3:
								PWM_CH3_TiempoSubida(0); //pwm 200V.
								PWM_CH3_TiempoMantenimiento(0);
								PWM_CH3_TiempoBajada(1000);
								break;

							case CH4:
								PWM_CH4_TiempoSubida(0); //pwm 200V.
								PWM_CH4_TiempoMantenimiento(0);
								PWM_CH4_TiempoBajada(1000);
								break;
						}

						*p_session_time = 0;
						*p_session_time_2 = 0;

						*p_fall_type = FALL_START;
						break;

					default:
						*p_fall_type = FALL_START;
						break;
				}
			}

			//BAJADA FALLING_SLOW_DISCHARGE
			if ((p_table + (*p_session_channel_step))->falling_type == FALLING_SLOW_DISCHARGE)
			{
				if (*p_session_time <= (p_table + (*p_session_channel_step))->falling_step_number)
				{
					if (*p_session_time_2 >= (*p_session_time - 5))
					{
						*p_session_time_2 = *p_session_time;

						//esta es el pwm de este instante el slope lo calculo antes
						*p_pwm_ch = (p_table + (*p_session_channel_step))->falling_pwm_200_initial + (p_table + (*p_session_channel_step))->falling_pwm_40_initial - *p_pwm_slope * *p_session_time;

						//modificacion bajada LR
						if ((*p_pwm_ch < 10) && ((p_table + (*p_session_channel_step))->falling_type != FALLING_LR))
						{
							*p_pwm_ch = 0;
							switch(channel)
							{
								case CH1:
									PWM_CH1_TiempoBajada(1000);
									break;

								case CH2:
									PWM_CH2_TiempoBajada(1000);
									break;

								case CH3:
									PWM_CH3_TiempoBajada(1000);
									break;

								case CH4:
									PWM_CH4_TiempoBajada(1000);
									break;
							}
						}

						//esto representa la funcion lineal de bajada lento
						switch (channel)
						{
							case CH1:
								if ((*p_session_burst_cnt < (p_table + (*p_session_channel_step))->burst_mode_on) ||
										(	((p_table + (*p_session_channel_step))->burst_mode_on == 0) && ((p_table + (*p_session_channel_step))->burst_mode_off == 0)))
								{
									if (((p_table + (*p_session_channel_step))->falling_pwm_200_initial + (p_table + (*p_session_channel_step))->falling_pwm_200_final) != 0)
										PWM_CH1_TiempoSubida(*p_pwm_ch); //pwm 200V.
									else if (((p_table + (*p_session_channel_step))->falling_pwm_40_initial + (p_table + (*p_session_channel_step))->falling_pwm_40_final) != 0)
										PWM_CH1_TiempoMantenimiento(*p_pwm_ch); //pwm 40V.
								}
								else
								{
									PWM_CH1_TiempoSubida(0); //pwm 200V.
									PWM_CH1_TiempoMantenimiento(0);
									PWM_CH1_TiempoBajada(0);
								}
								break;

							case CH2:
								if ((*p_session_burst_cnt < (p_table + (*p_session_channel_step))->burst_mode_on) ||
										(	((p_table + (*p_session_channel_step))->burst_mode_on == 0) && ((p_table + (*p_session_channel_step))->burst_mode_off == 0)))
								{
									if (((p_table + (*p_session_channel_step))->falling_pwm_200_initial + (p_table + (*p_session_channel_step))->falling_pwm_200_final) != 0)
										PWM_CH2_TiempoSubida(*p_pwm_ch); //pwm 200V.
									else if (((p_table + (*p_session_channel_step))->falling_pwm_40_initial + (p_table + (*p_session_channel_step))->falling_pwm_40_final) != 0)
										PWM_CH2_TiempoMantenimiento(*p_pwm_ch); //pwm 40V.
								}
								else
								{
									PWM_CH2_TiempoSubida(0); //pwm 200V.
									PWM_CH2_TiempoMantenimiento(0);
									PWM_CH2_TiempoBajada(0);
								}
								break;

							case CH3:
								if ((*p_session_burst_cnt < (p_table + (*p_session_channel_step))->burst_mode_on) ||
										(	((p_table + (*p_session_channel_step))->burst_mode_on == 0) && ((p_table + (*p_session_channel_step))->burst_mode_off == 0)))
								{
									if (((p_table + (*p_session_channel_step))->falling_pwm_200_initial + (p_table + (*p_session_channel_step))->falling_pwm_200_final) != 0)
										PWM_CH3_TiempoSubida(*p_pwm_ch); //pwm 200V.
									else if (((p_table + (*p_session_channel_step))->falling_pwm_40_initial + (p_table + (*p_session_channel_step))->falling_pwm_40_final) != 0)
										PWM_CH3_TiempoMantenimiento(*p_pwm_ch); //pwm 40V.
								}
								else
								{
									PWM_CH3_TiempoSubida(0); //pwm 200V.
									PWM_CH3_TiempoMantenimiento(0);
									PWM_CH3_TiempoBajada(0);
								}
								break;

							case CH4:
								if ((*p_session_burst_cnt < (p_table + (*p_session_channel_step))->burst_mode_on) ||
										(	((p_table + (*p_session_channel_step))->burst_mode_on == 0) && ((p_table + (*p_session_channel_step))->burst_mode_off == 0)))
								{
									if (((p_table + (*p_session_channel_step))->falling_pwm_200_initial + (p_table + (*p_session_channel_step))->falling_pwm_200_final) != 0)
										PWM_CH4_TiempoSubida(*p_pwm_ch); //pwm 200V.
									else if (((p_table + (*p_session_channel_step))->falling_pwm_40_initial + (p_table + (*p_session_channel_step))->falling_pwm_40_final) != 0)
										PWM_CH4_TiempoMantenimiento(*p_pwm_ch); //pwm 40V.
								}
								else
								{
									PWM_CH4_TiempoSubida(0); //pwm 200V.
									PWM_CH4_TiempoMantenimiento(0);
									PWM_CH4_TiempoBajada(0);
								}
								break;
						}
					}
				}
				else
				{
					*p_session_state = SESSION_COOLING_DOWN_CHANNEL_LOW;


					switch(channel)
					{
						case CH1:
							PWM_CH1_TiempoSubida(0); //pwm 200V.
							PWM_CH1_TiempoMantenimiento(0);
							PWM_CH1_TiempoBajada(1000);
							break;

						case CH2:
							PWM_CH2_TiempoSubida(0); //pwm 200V.
							PWM_CH2_TiempoMantenimiento(0);
							PWM_CH2_TiempoBajada(1000);
							break;

						case CH3:
							PWM_CH3_TiempoSubida(0); //pwm 200V.
							PWM_CH3_TiempoMantenimiento(0);
							PWM_CH3_TiempoBajada(1000);
							break;

						case CH4:
							PWM_CH4_TiempoSubida(0); //pwm 200V.
							PWM_CH4_TiempoMantenimiento(0);
							PWM_CH4_TiempoBajada(1000);
							break;
					}
					*p_session_time = 0;
					*p_session_time_2 = 0;
				}
			}
			break;

		case SESSION_COOLING_DOWN_CHANNEL_LOW:
#ifdef WITH_SYNC
			if (ch1_sync_state & SYNC_REQUIRED)
			{
				if (channel == CH1)
				{
					ch1_sync_state &= ~SYNC_ACT_MASK;
					ch1_sync_state |= SYNC_IN_DOWN;

					if (*p_session_time >= (p_table + (*p_session_channel_step))->low_step_number)
					{
						//*p_session_burst_cnt++;
						*p_session_burst_cnt = *p_session_burst_cnt + 1;

						if (*p_session_burst_cnt == ((p_table + (*p_session_channel_step))->burst_mode_on + (p_table + (*p_session_channel_step))->burst_mode_off))
							*p_session_burst_cnt = 0;

						*p_session_state = SESSION_COOLING_DOWN_CHANNEL_CHANGE_LEVEL;

					}
				}
				else
				{
					switch (channel)
					{
						case CH2:
							if (ch1_sync_state & SYNC_FINISH_COOLING_DOWN)
								return FIN_OK;

							//me quedo en LOW hasta que CH1 este en LOW
							if (ch1_sync_state & SYNC_IN_DOWN)
								*p_session_state = SESSION_COOLING_DOWN_CHANNEL_CHANGE_LEVEL;
							break;

						case CH3:
							if (ch1_sync_state & SYNC_FINISH_COOLING_DOWN)
								return FIN_OK;

							//me quedo en LOW mientras CH1 este en LOW, lo atraso un poquito para evitar dobles pulsos
							//if (!(ch1_sync_state & SYNC_IN_DOWN))
							if (ch1_sync_state & SYNC_IN_RISING)
								*p_session_state = SESSION_COOLING_DOWN_CHANNEL_CHANGE_LEVEL;
							break;

						case CH4:
							if (ch1_sync_state & SYNC_FINISH_COOLING_DOWN)
								return FIN_OK;

							//me quedo en LOW hasta que CH1 este en LOW
							if (ch1_sync_state & SYNC_IN_DOWN)
								*p_session_state = SESSION_COOLING_DOWN_CHANNEL_CHANGE_LEVEL;
							break;
					}
				}
			}
			else
			{
				if (*p_session_time >= (p_table + (*p_session_channel_step))->low_step_number)
				{
					//*p_session_burst_cnt++;
					*p_session_burst_cnt = *p_session_burst_cnt + 1;

					if (*p_session_burst_cnt == ((p_table + (*p_session_channel_step))->burst_mode_on + (p_table + (*p_session_channel_step))->burst_mode_off))
						*p_session_burst_cnt = 0;

					*p_session_state = SESSION_COOLING_DOWN_CHANNEL_CHANGE_LEVEL;

				}
			}
#else
			if (*p_session_time >= (p_table + (*p_session_channel_step))->low_step_number)
			{
				*p_session_burst_cnt++;

				if (*p_session_burst_cnt == ((p_table + (*p_session_channel_step))->burst_mode_on + (p_table + (*p_session_channel_step))->burst_mode_off))
					*p_session_burst_cnt = 0;

				*p_session_state = SESSION_COOLING_DOWN_CHANNEL_CHANGE_LEVEL;

			}
#endif
			break;

		case SESSION_COOLING_DOWN_CHANNEL_END_ERROR:
			*p_session_state = SESSION_COOLING_DOWN_CHANNEL_INIT;
			return FIN_ERROR;
			break;

		default:
			*p_session_state = SESSION_COOLING_DOWN_CHANNEL_INIT;
			break;

	}
	return TRABAJANDO;
}



//--- Channel 2 ---//
void Channel_2_Init(void)
{
	channel_2_pause = 0;

	session_plateau_channel_2_time = 0;
	session_plateau_channel_2_stage_time = 0;

	session_warning_up_channel_2_time = 0;
	session_warning_up_channel_2_time_2 = 0;
	session_warning_up_channel_2_stage_time = 0;

	session_cooling_down_channel_2_time = 0;
	session_cooling_down_channel_2_time_2 = 0;
	session_cooling_down_channel_2_stage_time = 0;

	session_channel_2_state = 0;

	//limpio tabla WARMING_UP
	memset(&table_warning_up_channel_2[0], '\0', sizeof(table_warning_up_channel_2));

	//limpio tabla PLATEAU
	memset(&table_plateau_channel_2[0], '\0', sizeof(table_plateau_channel_2));

	//limpio tabla COOLING_DOWN
	memset(&table_cooling_down_channel_2[0], '\0', sizeof(table_cooling_down_channel_2));

}

enum states_channel_2 {

	SESSION_CHANNEL_2_INIT = 0,
	SESSION_CHANNEL_2_VERIFY_ANTENNA,
	SESSION_CHANNEL_2_WARNING_UP,
	SESSION_CHANNEL_2_PLATEAU,
	SESSION_CHANNEL_2_COOLING_DOWN,
	SESSION_CHANNEL_2_END
};

void Session_Channel_2_Start(void)
{
	session_channel_2_state = SESSION_CHANNEL_2_INIT;
	session_ch_2.status = 1;
}
void Session_Channel_2_Stop(void)
{
	session_ch_2.status = 0;
}

unsigned char Session_Channel_2_Verify_Antenna_state = 0;

#define SESSION_CHANNEL_2_VERIFY_ANTENNA_TIME 5000			//ms
unsigned short Session_Channel_2_Verify_Antenna_time = 0;

enum Session_Channel_2_Verify_Antenna_states
{
	SESSION_CHANNEL_2_VERIFY_ANTENNA_INIT = 0,
	SESSION_CHANNEL_2_VERIFY_ANTENNA_WAIT_PARAMS,
	SESSION_CHANNEL_2_VERIFY_ANTENNA_FIN_OK,
	SESSION_CHANNEL_2_VERIFY_ANTENNA_FIN_ERROR
};
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

extern unsigned char temp_actual_channel_2_int;
extern unsigned char temp_actual_channel_2_dec;

void Session_Channel_2 (void)
{

	unsigned char i;

	//if the channel is enabled the session occurs.
	//Else the program will wait with the channel turned off.
	if (session_ch_2.status)
	{

		switch (session_channel_2_state)
		{
			case SESSION_CHANNEL_2_INIT:

				temp_actual_channel_2_int = 0;
				temp_actual_channel_2_dec = 0;

				session_channel_2_answer_temp = SESSION_CHANNEL_ANSWER_TEMP;
				session_channel_2_ask_temp = SESSION_CHANNEL_ASK_TEMP;

				PWM_CH2_TiempoSubida(0); //pwm 200V.
				PWM_CH2_TiempoMantenimiento(0);
				PWM_CH2_TiempoBajada(0);

				//Restart stages.
				Session_Warning_Up_Channel_2_Restart();
				Session_Plateau_Channel_2_Restart();
				Session_Cooling_Down_Channel_2_Restart();

				SetBitGlobalErrors (CH2, BIT_ERROR_CHECK);

				session_channel_2_state = SESSION_CHANNEL_2_VERIFY_ANTENNA;
				break;

			case SESSION_CHANNEL_2_VERIFY_ANTENNA:

				i = Session_Channel_2_Verify_Antenna(&session_ch_2);


				if (i == FIN_OK)
					session_channel_2_state = SESSION_CHANNEL_2_WARNING_UP;

				else if (i == FIN_ERROR)
				{
					session_ch_2.status = 0;
					SetBitGlobalErrors (CH2, BIT_ERROR_ANTENNA);

					sprintf(&buffSendErr[0], (const char *) "ERROR(0x%03X)\r\n", ERR_CHANNEL_ANTENNA_DISCONNECTED(2));
					UART_PC_Send(&buffSendErr[0]);
				}
				break;

			case SESSION_CHANNEL_2_WARNING_UP:
				//Warning up.
				if (channel_2_pause == 0)
				{

					//los errores pueden ser por calculo de parametros o falta de sync cuando se necesita
					i = Session_Warming_Up_Channels(CH2);

					if (i == FIN_OK)
					{
						session_channel_2_state = SESSION_CHANNEL_2_PLATEAU;
						UART_PC_Send("End_Warning_Up,2\r\n");

						PWM_CH2_TiempoSubida(0); //pwm 200V.
						PWM_CH2_TiempoMantenimiento(0);
						PWM_CH2_TiempoBajada(0);

					}

					if ((i == TRABAJANDO) && (session_warning_up_channel_2_state > SESSION_WARMING_UP_CHANNEL_PARAMETERS_CALCULATE))
					{
						Current_Limit_CheckCh2();
					}

					if (i == FIN_ERROR)
					{
						session_ch_2.status = 0;
						SetBitGlobalErrors (CH2, BIT_ERROR_WARMING_UP);

						sprintf(&buffSendErr[0], (const char *) "ERROR(0x%03X)\r\n", ERR_CHANNEL_WARNING_UP(2));
						UART_PC_Send(&buffSendErr[0]);
					}
				}
				else
				{
					PWM_CH2_TiempoSubida(0);
					PWM_CH2_TiempoMantenimiento(0);
					PWM_CH2_TiempoBajada(0);
				}
				break;

			case SESSION_CHANNEL_2_PLATEAU:
				//Plateau.
				if (channel_2_pause == 0)
				{

					//i = Session_Plateau_Channel_2();
					i = Session_Plateau_Channels(CH2);

					if (i == FIN_OK)
					{
						session_channel_2_state = SESSION_CHANNEL_2_COOLING_DOWN;
						UART_PC_Send("End_Plateau,2\r\n");

						PWM_CH2_TiempoSubida(0); //pwm 200V.
						PWM_CH2_TiempoMantenimiento(0);
						PWM_CH2_TiempoBajada(0);
					}

					if ((i == TRABAJANDO) && (session_plateau_channel_2_state > SESSION_PLATEAU_CHANNEL_PARAMETERS_CALCULATE))
					{
						Current_Limit_CheckCh2();
					}

					if (i == FIN_ERROR)
					{
						session_ch_2.status = 0;

						sprintf(&buffSendErr[0], (const char *) "ERROR(0x%03X)\r\n", ERR_CHANNEL_PLATEAU(2));
						UART_PC_Send(&buffSendErr[0]);
					}
				}
				else
				{
					PWM_CH2_TiempoSubida(0);
					PWM_CH2_TiempoMantenimiento(0);
					PWM_CH2_TiempoBajada(0);
				}
				break;

			case SESSION_CHANNEL_2_COOLING_DOWN:
				//Cooling dawn.
				if (channel_2_pause == 0)
				{

					//i = Session_Cooling_Down_Channel_2();
					i = Session_Cooling_Down_Channels(CH2);

					if (i == FIN_OK)
					{
						session_channel_2_state = SESSION_CHANNEL_2_END;
						UART_PC_Send("End_Cooling_Down,2\r\n");

						UART_PC_Send("finish,2\r\n");
						PWM_CH2_TiempoSubida(0); //pwm 200V.
						PWM_CH2_TiempoMantenimiento(0);
						PWM_CH2_TiempoBajada(0);
					}

					if ((i == TRABAJANDO) && (session_cooling_down_channel_2_state > SESSION_COOLING_DOWN_CHANNEL_PARAMETERS_CALCULATE))
					{
						Current_Limit_CheckCh2();
					}

					if (i == FIN_ERROR)
					{
						session_ch_2.status = 0;

						sprintf(&buffSendErr[0], (const char *) "ERROR(0x%03X)\r\n", ERR_CHANNEL_COOLING_DOWN(2));
						UART_PC_Send(&buffSendErr[0]);
					}
				}
				else
				{
					PWM_CH2_TiempoSubida(0);
					PWM_CH2_TiempoMantenimiento(0);
					PWM_CH2_TiempoBajada(0);
				}
				break;

			case SESSION_CHANNEL_2_END:
				session_ch_2.status = 0;
				break;

			default:
				session_ch_2.status = 0;
				break;
		}

		if (session_channel_2_state >= SESSION_CHANNEL_2_WARNING_UP)
		{

			if (session_channel_2_ask_temp == 0)
			{
				UART_CH2_Send("get_temp\r\n");

				session_channel_2_ask_temp = SESSION_CHANNEL_ASK_TEMP;
			}

			if ((temp_actual_channel_2_int != 0) || (temp_actual_channel_2_dec != 0))
			{
				if ((temp_actual_channel_2_int > session_ch_2.stage_1_temp_max_int)
						|| ((temp_actual_channel_2_int == session_ch_2.stage_1_temp_max_int) && (temp_actual_channel_2_dec > session_ch_2.stage_1_temp_max_dec)))
				{
					session_ch_2.status = 0;
					sprintf(&buffSendErr[0], (const char *) "ERROR(0x%03X)\r\n", ERR_CHANNEL_ANTENNA_TMP_OUT_OF_RANGE(2));
					UART_PC_Send(&buffSendErr[0]);

					temp_actual_channel_2_int = 0;
					temp_actual_channel_2_dec = 0;
				}
				else
				{
					session_channel_2_answer_temp = SESSION_CHANNEL_ANSWER_TEMP;

					temp_actual_channel_2_int = 0;
					temp_actual_channel_2_dec = 0;
				}
			}

			if (session_channel_2_answer_temp == 0)
			{
				session_ch_2.status = 0;

				sprintf(&buffSendErr[0], (const char *) "ERROR(0x%03X)\r\n", ERR_CHANNEL_ANTENNA_LOST(2));
				UART_PC_Send(&buffSendErr[0]);
			}
		}
	}
	else
	{
		//Initial state.
		session_channel_2_state = SESSION_CHANNEL_2_INIT;

		//Stop pwm channels.
		PWM_CH2_TiempoSubida(0);
		PWM_CH2_TiempoMantenimiento(0);
		PWM_CH2_TiempoBajada(0);

		//Stop timer interrupt.
	}
}







//--- Channel 3 ---//
void Channel_3_Init(void)
{
	channel_3_pause = 0;

	session_plateau_channel_3_time = 0;
	session_plateau_channel_3_stage_time = 0;

	session_warning_up_channel_3_time = 0;
	session_warning_up_channel_3_time_2 = 0;
	session_warning_up_channel_3_stage_time = 0;

	session_cooling_down_channel_3_time = 0;
	session_cooling_down_channel_3_time_2 = 0;
	session_cooling_down_channel_3_stage_time = 0;

	session_channel_3_state = 0;

	//limpio tabla WARMING_UP
	memset(&table_warning_up_channel_3[0], '\0', sizeof(table_warning_up_channel_3));

	//limpio tabla PLATEAU
	memset(&table_plateau_channel_3[0], '\0', sizeof(table_plateau_channel_3));

	//limpio tabla COOLING_DOWN
	memset(&table_cooling_down_channel_3[0], '\0', sizeof(table_cooling_down_channel_3));
}

enum states_channel_3 {

	SESSION_CHANNEL_3_INIT = 0,
	SESSION_CHANNEL_3_VERIFY_ANTENNA,
	SESSION_CHANNEL_3_WARNING_UP,
	SESSION_CHANNEL_3_PLATEAU,
	SESSION_CHANNEL_3_COOLING_DOWN,
	SESSION_CHANNEL_3_END
};

void Session_Channel_3_Start(void)
{
	session_channel_3_state = SESSION_CHANNEL_3_INIT;
	session_ch_3.status = 1;
}
void Session_Channel_3_Stop(void)
{
	session_ch_3.status = 0;
}

unsigned char Session_Channel_3_Verify_Antenna_state = 0;

#define SESSION_CHANNEL_3_VERIFY_ANTENNA_TIME 5000			//ms
unsigned short Session_Channel_3_Verify_Antenna_time = 0;

enum Session_Channel_3_Verify_Antenna_states
{
	SESSION_CHANNEL_3_VERIFY_ANTENNA_INIT = 0,
	SESSION_CHANNEL_3_VERIFY_ANTENNA_WAIT_PARAMS,
	SESSION_CHANNEL_3_VERIFY_ANTENNA_FIN_OK,
	SESSION_CHANNEL_3_VERIFY_ANTENNA_FIN_ERROR
};

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

extern unsigned char temp_actual_channel_3_int;
extern unsigned char temp_actual_channel_3_dec;

void Session_Channel_3 (void)
{

	unsigned char i;

	//if the channel is enabled the session occurs.
	//Else the program will wait with the channel turned off.
	if (session_ch_3.status)
	{

		switch (session_channel_3_state)
		{
			case SESSION_CHANNEL_3_INIT:

				temp_actual_channel_3_int = 0;
				temp_actual_channel_3_dec = 0;

				session_channel_3_answer_temp = SESSION_CHANNEL_ANSWER_TEMP;
				session_channel_3_ask_temp = SESSION_CHANNEL_ASK_TEMP;

				PWM_CH3_TiempoSubida(0); //pwm 200V.
				PWM_CH3_TiempoMantenimiento(0);
				PWM_CH3_TiempoBajada(0);

				//Restart stages.
				Session_Warning_Up_Channel_3_Restart();
				Session_Plateau_Channel_3_Restart();
				Session_Cooling_Down_Channel_3_Restart();

				SetBitGlobalErrors (CH3, BIT_ERROR_CHECK);
				session_channel_3_state = SESSION_CHANNEL_3_VERIFY_ANTENNA;
				break;

			case SESSION_CHANNEL_3_VERIFY_ANTENNA:

				i = Session_Channel_3_Verify_Antenna(&session_ch_3);


				if (i == FIN_OK)
					session_channel_3_state = SESSION_CHANNEL_3_WARNING_UP;

				else if (i == FIN_ERROR)
				{
					session_ch_3.status = 0;
					SetBitGlobalErrors (CH3, BIT_ERROR_ANTENNA);

					sprintf(&buffSendErr[0], (const char *) "ERROR(0x%03X)\r\n", ERR_CHANNEL_ANTENNA_DISCONNECTED(3));
					UART_PC_Send(&buffSendErr[0]);
				}
				break;

			case SESSION_CHANNEL_3_WARNING_UP:
				//Warning up.
				if (channel_3_pause == 0)
				{

					//los errores pueden ser por calculo de parametros o falta de sync cuando se necesita
					i = Session_Warming_Up_Channels(CH3);

					if (i == FIN_OK)
					{
						session_channel_3_state = SESSION_CHANNEL_3_PLATEAU;
						UART_PC_Send("End_Warning_Up,3\r\n");

						PWM_CH3_TiempoSubida(0); //pwm 200V.
						PWM_CH3_TiempoMantenimiento(0);
						PWM_CH3_TiempoBajada(0);

					}

					if ((i == TRABAJANDO) && (session_warning_up_channel_3_state > SESSION_WARMING_UP_CHANNEL_PARAMETERS_CALCULATE))
					{
						Current_Limit_CheckCh3();
					}

					if (i == FIN_ERROR)
					{
						session_ch_3.status = 0;
						SetBitGlobalErrors (CH3, BIT_ERROR_WARMING_UP);

						sprintf(&buffSendErr[0], (const char *) "ERROR(0x%03X)\r\n", ERR_CHANNEL_WARNING_UP(3));
						UART_PC_Send(&buffSendErr[0]);
					}
				}
				else
				{
					PWM_CH3_TiempoSubida(0);
					PWM_CH3_TiempoMantenimiento(0);
					PWM_CH3_TiempoBajada(0);
				}
				break;

			case SESSION_CHANNEL_3_PLATEAU:
				//Plateau.
				if (channel_3_pause == 0)
				{

					//i = Session_Plateau_Channel_3();
					i = Session_Plateau_Channels(CH3);

					if (i == FIN_OK)
					{
						session_channel_3_state = SESSION_CHANNEL_3_COOLING_DOWN;
						UART_PC_Send("End_Plateau,3\r\n");

						PWM_CH3_TiempoSubida(0); //pwm 200V.
						PWM_CH3_TiempoMantenimiento(0);
						PWM_CH3_TiempoBajada(0);
					}

					if ((i == TRABAJANDO) && (session_plateau_channel_3_state > SESSION_PLATEAU_CHANNEL_PARAMETERS_CALCULATE))
					{
						Current_Limit_CheckCh3();
					}

					if (i == FIN_ERROR)
					{
						session_ch_3.status = 0;

						sprintf(&buffSendErr[0], (const char *) "ERROR(0x%03X)\r\n", ERR_CHANNEL_PLATEAU(3));
						UART_PC_Send(&buffSendErr[0]);
					}
				}
				else
				{
					PWM_CH3_TiempoSubida(0);
					PWM_CH3_TiempoMantenimiento(0);
					PWM_CH3_TiempoBajada(0);
				}
				break;

			case SESSION_CHANNEL_3_COOLING_DOWN:
				//Cooling dawn.
				if (channel_3_pause == 0)
				{

					//i = Session_Cooling_Down_Channel_3();
					i = Session_Cooling_Down_Channels(CH3);

					if (i == FIN_OK)
					{
						session_channel_3_state = SESSION_CHANNEL_3_END;
						UART_PC_Send("End_Cooling_Down,3\r\n");

						UART_PC_Send("finish,3\r\n");
						PWM_CH3_TiempoSubida(0); //pwm 200V.
						PWM_CH3_TiempoMantenimiento(0);
						PWM_CH3_TiempoBajada(0);
					}

					if ((i == TRABAJANDO) && (session_cooling_down_channel_3_state > SESSION_COOLING_DOWN_CHANNEL_PARAMETERS_CALCULATE))
					{
						Current_Limit_CheckCh3();
					}

					if (i == FIN_ERROR)
					{
						session_ch_3.status = 0;

						sprintf(&buffSendErr[0], (const char *) "ERROR(0x%03X)\r\n", ERR_CHANNEL_COOLING_DOWN(3));
						UART_PC_Send(&buffSendErr[0]);
					}
				}
				else
				{
					PWM_CH3_TiempoSubida(0);
					PWM_CH3_TiempoMantenimiento(0);
					PWM_CH3_TiempoBajada(0);
				}
				break;

			case SESSION_CHANNEL_3_END:
				session_ch_3.status = 0;

				break;

			default:
				session_ch_3.status = 0;
				break;
		}

		if (session_channel_3_state >= SESSION_CHANNEL_3_WARNING_UP)
		{

			if (session_channel_3_ask_temp == 0)
			{
				UART_CH3_Send("get_temp\r\n");

				session_channel_3_ask_temp = SESSION_CHANNEL_ASK_TEMP;
			}

			if ((temp_actual_channel_3_int != 0) || (temp_actual_channel_3_dec != 0))
			{
				if ((temp_actual_channel_3_int > session_ch_3.stage_1_temp_max_int)
						|| ((temp_actual_channel_3_int == session_ch_3.stage_1_temp_max_int) && (temp_actual_channel_3_dec > session_ch_3.stage_1_temp_max_dec)))
				{
					session_ch_3.status = 0;
					sprintf(&buffSendErr[0], (const char *) "ERROR(0x%03X)\r\n", ERR_CHANNEL_ANTENNA_TMP_OUT_OF_RANGE(3));
					UART_PC_Send(&buffSendErr[0]);

					temp_actual_channel_3_int = 0;
					temp_actual_channel_3_dec = 0;
				}
				else
				{
					session_channel_3_answer_temp = SESSION_CHANNEL_ANSWER_TEMP;

					temp_actual_channel_3_int = 0;
					temp_actual_channel_3_dec = 0;
				}
			}

			if (session_channel_3_answer_temp == 0)
			{
				session_ch_3.status = 0;

				sprintf(&buffSendErr[0], (const char *) "ERROR(0x%03X)\r\n", ERR_CHANNEL_ANTENNA_LOST(3));
				UART_PC_Send(&buffSendErr[0]);
			}
		}
	}
	else
	{
		//Initial state.
		session_channel_3_state = SESSION_CHANNEL_3_INIT;

		//Stop pwm channels.
		PWM_CH3_TiempoSubida(0);
		PWM_CH3_TiempoMantenimiento(0);
		PWM_CH3_TiempoBajada(0);

		//Stop timer interrupt.
	}
}




//--- Channel 4 ---//
void Channel_4_Init(void)
{
	channel_4_pause = 0;

	session_plateau_channel_4_time = 0;
	session_plateau_channel_4_stage_time = 0;

	session_warning_up_channel_4_time = 0;
	session_warning_up_channel_4_time_2 = 0;
	session_warning_up_channel_4_stage_time = 0;

	session_cooling_down_channel_4_time = 0;
	session_cooling_down_channel_4_time_2 = 0;
	session_cooling_down_channel_4_stage_time = 0;

	session_channel_4_state = 0;


	//limpio tabla WARMING_UP
	memset(&table_warning_up_channel_4[0], '\0', sizeof(table_warning_up_channel_4));

	//limpio tabla PLATEAU
	memset(&table_plateau_channel_4[0], '\0', sizeof(table_plateau_channel_4));

	//limpio tabla COOLING_DOWN
	memset(&table_cooling_down_channel_4[0], '\0', sizeof(table_cooling_down_channel_4));
}

enum states_channel_4 {

	SESSION_CHANNEL_4_INIT = 0,
	SESSION_CHANNEL_4_VERIFY_ANTENNA,
	SESSION_CHANNEL_4_WARNING_UP,
	SESSION_CHANNEL_4_PLATEAU,
	SESSION_CHANNEL_4_COOLING_DOWN,
	SESSION_CHANNEL_4_END
};

void Session_Channel_4_Start(void)
{
	session_channel_4_state = SESSION_CHANNEL_4_INIT;
	session_ch_4.status = 1;
}
void Session_Channel_4_Stop(void)
{
	session_ch_4.status = 0;
}
unsigned char Session_Channel_4_Verify_Antenna_state = 0;

#define SESSION_CHANNEL_4_VERIFY_ANTENNA_TIME 5000			//ms
unsigned short Session_Channel_4_Verify_Antenna_time = 0;

enum Session_Channel_4_Verify_Antenna_states
{
	SESSION_CHANNEL_4_VERIFY_ANTENNA_INIT = 0,
	SESSION_CHANNEL_4_VERIFY_ANTENNA_WAIT_PARAMS,
	SESSION_CHANNEL_4_VERIFY_ANTENNA_FIN_OK,
	SESSION_CHANNEL_4_VERIFY_ANTENNA_FIN_ERROR
};
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

extern unsigned char temp_actual_channel_4_int;
extern unsigned char temp_actual_channel_4_dec;

void Session_Channel_4 (void)
{

	unsigned char i;

	//if the channel is enabled the session occurs.
	//Else the program will wait with the channel turned off.


	if (session_ch_4.status)
	{

		switch (session_channel_4_state)
		{
			case SESSION_CHANNEL_4_INIT:

				temp_actual_channel_4_int = 0;
				temp_actual_channel_4_dec = 0;

				session_channel_4_answer_temp = SESSION_CHANNEL_ANSWER_TEMP;
				session_channel_4_ask_temp = SESSION_CHANNEL_ASK_TEMP;

				PWM_CH4_TiempoSubida(0); //pwm 200V.
				PWM_CH4_TiempoMantenimiento(0);
				PWM_CH4_TiempoBajada(0);

				//Restart stages.
				Session_Warning_Up_Channel_4_Restart();
				Session_Plateau_Channel_4_Restart();
				Session_Cooling_Down_Channel_4_Restart();

				SetBitGlobalErrors (CH4, BIT_ERROR_CHECK);

				session_channel_4_state = SESSION_CHANNEL_4_VERIFY_ANTENNA;
				break;

			case SESSION_CHANNEL_4_VERIFY_ANTENNA:

				i = Session_Channel_4_Verify_Antenna(&session_ch_4);


				if (i == FIN_OK)
					session_channel_4_state = SESSION_CHANNEL_4_WARNING_UP;

				else if (i == FIN_ERROR)
				{
					session_ch_4.status = 0;
					SetBitGlobalErrors (CH4, BIT_ERROR_ANTENNA);

					sprintf(&buffSendErr[0], (const char *) "ERROR(0x%03X)\r\n", ERR_CHANNEL_ANTENNA_DISCONNECTED(4));
					UART_PC_Send(&buffSendErr[0]);
				}
				break;

			case SESSION_CHANNEL_4_WARNING_UP:
				//Warning up.
				if (channel_4_pause == 0)
				{

					LED1_ON;
					LED2_OFF;
					//LED3_OFF;

					//los errores pueden ser por calculo de parametros o falta de sync cuando se necesita
					i = Session_Warming_Up_Channels(CH4);

					if (i == FIN_OK)
					{
						session_channel_4_state = SESSION_CHANNEL_4_PLATEAU;
						UART_PC_Send("End_Warning_Up,4\r\n");

						PWM_CH4_TiempoSubida(0); //pwm 200V.
						PWM_CH4_TiempoMantenimiento(0);
						PWM_CH4_TiempoBajada(0);

					}

					if ((i == TRABAJANDO) && (session_warning_up_channel_4_state > SESSION_WARMING_UP_CHANNEL_PARAMETERS_CALCULATE))
					{
						Current_Limit_CheckCh4();
					}

					if (i == FIN_ERROR)
					{
						session_ch_4.status = 0;
						SetBitGlobalErrors (CH4, BIT_ERROR_WARMING_UP);

						PWM_CH4_TiempoSubida(0); //pwm 200V.
						PWM_CH4_TiempoMantenimiento(0);
						PWM_CH4_TiempoBajada(0);

						sprintf(&buffSendErr[0], (const char *) "ERROR(0x%03X)\r\n", ERR_CHANNEL_WARNING_UP(4));
						UART_PC_Send(&buffSendErr[0]);


					}
				}
				else
				{
					PWM_CH4_TiempoSubida(0);
					PWM_CH4_TiempoMantenimiento(0);
					PWM_CH4_TiempoBajada(0);
				}
				break;

			case SESSION_CHANNEL_4_PLATEAU:
				//Plateau.
				if (channel_4_pause == 0)
				{

					LED1_OFF;
					LED2_ON;
					//LED3_OFF;

					//i = Session_Plateau_Channel_4();
					i = Session_Plateau_Channels(CH4);

					if (i == FIN_OK)
					{
						session_channel_4_state = SESSION_CHANNEL_4_COOLING_DOWN;
						UART_PC_Send("End_Plateau,4\r\n");

						PWM_CH4_TiempoSubida(0); //pwm 200V.
						PWM_CH4_TiempoMantenimiento(0);
						PWM_CH4_TiempoBajada(0);
					}

					if ((i == TRABAJANDO) && (session_plateau_channel_4_state > SESSION_PLATEAU_CHANNEL_PARAMETERS_CALCULATE))
					{
						Current_Limit_CheckCh4();
					}

					if (i == FIN_ERROR)
					{
						session_ch_4.status = 0;

						PWM_CH4_TiempoSubida(0); //pwm 200V.
						PWM_CH4_TiempoMantenimiento(0);
						PWM_CH4_TiempoBajada(0);

						sprintf(&buffSendErr[0], (const char *) "ERROR(0x%03X)\r\n", ERR_CHANNEL_PLATEAU(4));
						UART_PC_Send(&buffSendErr[0]);
					}
				}
				else
				{
					PWM_CH4_TiempoSubida(0);
					PWM_CH4_TiempoMantenimiento(0);
					PWM_CH4_TiempoBajada(0);
				}
				break;

			case SESSION_CHANNEL_4_COOLING_DOWN:
				//Cooling dawn.
				if (channel_4_pause == 0)
				{

					LED1_OFF;
					LED2_OFF;
					//LED3_ON;

					//i = Session_Cooling_Down_Channel_4();
					i = Session_Cooling_Down_Channels(CH4);

					if (i == FIN_OK)
					{
						session_channel_4_state = SESSION_CHANNEL_4_END;
						UART_PC_Send("End_Cooling_Down,4\r\n");

						UART_PC_Send("finish,4\r\n");
						PWM_CH4_TiempoSubida(0); //pwm 200V.
						PWM_CH4_TiempoMantenimiento(0);
						PWM_CH4_TiempoBajada(0);
					}

					if ((i == TRABAJANDO) && (session_cooling_down_channel_4_state > SESSION_COOLING_DOWN_CHANNEL_PARAMETERS_CALCULATE))
					{
						Current_Limit_CheckCh4();
					}

					if (i == FIN_ERROR)
					{
						session_ch_4.status = 0;

						PWM_CH4_TiempoSubida(0); //pwm 200V.
						PWM_CH4_TiempoMantenimiento(0);
						PWM_CH4_TiempoBajada(0);

						sprintf(&buffSendErr[0], (const char *) "ERROR(0x%03X)\r\n", ERR_CHANNEL_COOLING_DOWN(4));
						UART_PC_Send(&buffSendErr[0]);
					}
				}
				else
				{
					PWM_CH4_TiempoSubida(0);
					PWM_CH4_TiempoMantenimiento(0);
					PWM_CH4_TiempoBajada(0);
				}
				break;

			case SESSION_CHANNEL_4_END:
				session_ch_4.status = 0;
				break;

			default:
				session_ch_4.status = 0;
				break;
		}

		if (session_channel_4_state >= SESSION_CHANNEL_4_WARNING_UP)
		{

			if (session_channel_4_ask_temp == 0)
			{
				UART_CH4_Send("get_temp\r\n");

				session_channel_4_ask_temp = SESSION_CHANNEL_ASK_TEMP;
			}

			if ((temp_actual_channel_4_int != 0) || (temp_actual_channel_4_dec != 0))
			{
				if ((temp_actual_channel_4_int > session_ch_4.stage_1_temp_max_int)
						|| ((temp_actual_channel_4_int == session_ch_4.stage_1_temp_max_int) && (temp_actual_channel_4_dec > session_ch_4.stage_1_temp_max_dec)))
				{

					session_ch_4.status = 0;


					sprintf(&buffSendErr[0], (const char *) "ERROR(0x%03X)\r\n", ERR_CHANNEL_ANTENNA_TMP_OUT_OF_RANGE(4));
					UART_PC_Send(&buffSendErr[0]);

					temp_actual_channel_4_int = 0;
					temp_actual_channel_4_dec = 0;
				}
				else
				{
					session_channel_4_answer_temp = SESSION_CHANNEL_ANSWER_TEMP;

					temp_actual_channel_4_int = 0;
					temp_actual_channel_4_dec = 0;
				}
			}

			if (session_channel_4_answer_temp == 0)
			{
				session_ch_4.status = 0;

				sprintf(&buffSendErr[0], (const char *) "ERROR(0x%03X)\r\n", ERR_CHANNEL_ANTENNA_LOST(4));
				UART_PC_Send(&buffSendErr[0]);
			}
		}
	}
	else
	{
		//Initial state.
		session_channel_4_state = SESSION_CHANNEL_4_INIT;

		//Stop pwm channels.
		PWM_CH4_TiempoSubida(0);
		PWM_CH4_TiempoMantenimiento(0);
		PWM_CH4_TiempoBajada(0);

		LED1_ON;
		LED2_OFF;
		//LED3_OFF;
		//Stop timer interrupt.
	}
}



//--- Relativo al exceso de corriente en los canales
#define MAX_CURRENT_COUNTER		5

unsigned char current_limit_cnt_ch1 = 0;
unsigned char current_limit_cnt_ch2 = 0;
unsigned char current_limit_cnt_ch3 = 0;
unsigned char current_limit_cnt_ch4 = 0;

unsigned char new_current_sample_ch1 = 0;
unsigned char new_current_sample_ch2 = 0;
unsigned char new_current_sample_ch3 = 0;
unsigned char new_current_sample_ch4 = 0;

//si el canal se encuentra activo
//cargo en un vector las muestras corriente de cada canal
void Session_Current_Limit_control (void)
{

	switch (current_limit_state)
	{
		case CURRENT_INIT_CHECK:
			if (take_current_samples)
			{
				take_current_samples = 0;
				current_limit_state++;
			}
			break;

		case CURRENT_CH1:
			if (session_ch_1.status)
			{
				flagMuestreo = 0;
				ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 1, ADC_SampleTime_28Cycles5);
				ADC_SoftwareStartConvCmd(ADC1, ENABLE);
				current_limit_state = CURRENT_CH1_WAIT_SAMPLE;
			}
			else
				current_limit_state = CURRENT_CH2;


			break;

		case CURRENT_CH1_WAIT_SAMPLE:
			if (flagMuestreo)
			{
				actual_current[CH1] = ADC1->DR;
				current_limit_state = CURRENT_CH1_CHECK;
			}
			break;

		case CURRENT_CH1_CHECK:
//			if (session_ch_1.peak_current_limit < actual_current[CH1])
//			{
//				Session_Channel_1_Stop();
//
//				sprintf(&buffSendErr[0], (const char *) "ERROR(0x%03X)\r\n", ERR_CHANNEL_ANTENNA_CURRENT_OUT_OF_RANGE(1));
//				UART_PC_Send(&buffSendErr[0]);
//				sprintf(&buffSendErr[0], (const char *) "current was: %d\r\n", actual_current[CH1]);
//				UART_PC_Send(&buffSendErr[0]);
//			}

			//se chequea en cada canal
			new_current_sample_ch1 = 1;
			current_limit_state++;

			break;

		case CURRENT_CH2:
			if (session_ch_2.status)
			{
				flagMuestreo = 0;
				ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 1, ADC_SampleTime_28Cycles5);
				ADC_SoftwareStartConvCmd(ADC1, ENABLE);
				current_limit_state = CURRENT_CH2_WAIT_SAMPLE;
			}
			else
				current_limit_state = CURRENT_CH3;


			break;

		case CURRENT_CH2_WAIT_SAMPLE:
			if (flagMuestreo)
			{
				actual_current[CH2] = ADC1->DR;
				current_limit_state = CURRENT_CH2_CHECK;
			}
			break;

		case CURRENT_CH2_CHECK:
//			if (session_ch_2.peak_current_limit < actual_current[CH2])
//			{
//				Session_Channel_2_Stop();
//
//				sprintf(&buffSendErr[0], (const char *) "ERROR(0x%03X)\r\n", ERR_CHANNEL_ANTENNA_CURRENT_OUT_OF_RANGE(2));
//				UART_PC_Send(&buffSendErr[0]);
//				sprintf(&buffSendErr[0], (const char *) "current was: %d\r\n", actual_current[CH2]);
//				UART_PC_Send(&buffSendErr[0]);
//			}

			//se chequea en cada canal
			new_current_sample_ch2 = 1;
			current_limit_state++;

			break;

		case CURRENT_CH3:
			if (session_ch_3.status)
			{
				flagMuestreo = 0;
				ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 1, ADC_SampleTime_28Cycles5);
				ADC_SoftwareStartConvCmd(ADC1, ENABLE);
				current_limit_state = CURRENT_CH3_WAIT_SAMPLE;
			}
			else
				current_limit_state = CURRENT_CH4;


			break;

		case CURRENT_CH3_WAIT_SAMPLE:
			if (flagMuestreo)
			{
				actual_current[CH3] = ADC1->DR;
				current_limit_state = CURRENT_CH3_CHECK;
			}
			break;

		case CURRENT_CH3_CHECK:
//			if (session_ch_3.peak_current_limit < actual_current[CH3])
//			{
//				Session_Channel_3_Stop();
//
//				sprintf(&buffSendErr[0], (const char *) "ERROR(0x%03X)\r\n", ERR_CHANNEL_ANTENNA_CURRENT_OUT_OF_RANGE(3));
//				UART_PC_Send(&buffSendErr[0]);
//				sprintf(&buffSendErr[0], (const char *) "current was: %d\r\n", actual_current[CH3]);
//				UART_PC_Send(&buffSendErr[0]);
//			}

			//se chequea en cada canal
			new_current_sample_ch3 = 1;
			current_limit_state++;

			break;

		case CURRENT_CH4:
			if (session_ch_4.status)
			{
				flagMuestreo = 0;
				ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 1, ADC_SampleTime_28Cycles5);
				ADC_SoftwareStartConvCmd(ADC1, ENABLE);
				current_limit_state = CURRENT_CH4_WAIT_SAMPLE;
			}
			else
				current_limit_state = CURRENT_INIT_CHECK;


			break;

		case CURRENT_CH4_WAIT_SAMPLE:
			if (flagMuestreo)
			{
				actual_current[CH4] = ADC1->DR;
				current_limit_state = CURRENT_CH4_CHECK;
			}
			break;

		case CURRENT_CH4_CHECK:
//			if (session_ch_4.peak_current_limit < actual_current[CH4])
//			{
//				Session_Channel_4_Stop();
//
//				sprintf(&buffSendErr[0], (const char *) "ERROR(0x%03X)\r\n", ERR_CHANNEL_ANTENNA_CURRENT_OUT_OF_RANGE(4));
//				UART_PC_Send(&buffSendErr[0]);
//				sprintf(&buffSendErr[0], (const char *) "current was: %d\r\n", actual_current[CH4]);
//				UART_PC_Send(&buffSendErr[0]);
//			}

			//se chequea en cada canal
			new_current_sample_ch4 = 1;
			current_limit_state = CURRENT_INIT_CHECK;

			break;

		default:
			current_limit_state = CURRENT_INIT_CHECK;
			break;

	}
}

unsigned char Current_Limit_CheckCh1 (void)
{
	if (new_current_sample_ch1)
	{
		if (session_ch_1.peak_current_limit < actual_current[CH1])
		{
			if (current_limit_cnt_ch1 < MAX_CURRENT_COUNTER)
				current_limit_cnt_ch1++;
			else
			{
				Session_Channel_1_Stop();
				SetBitGlobalErrors (CH1, BIT_ERROR_CURRENT);

				sprintf(&buffSendErr[0], (const char *) "ERROR(0x%03X)\r\n", ERR_CHANNEL_ANTENNA_CURRENT_OUT_OF_RANGE(1));
				UART_PC_Send(&buffSendErr[0]);
				sprintf(&buffSendErr[0], (const char *) "current was: %d\r\n", actual_current[CH1]);
				UART_PC_Send(&buffSendErr[0]);
				return FIN_ERROR;
			}
		}
		new_current_sample_ch1 = 0;
	}
	return TRABAJANDO;
}

unsigned char Current_Limit_CheckCh2 (void)
{
	if (new_current_sample_ch2)
	{
		if (session_ch_2.peak_current_limit < actual_current[CH2])
		{
			if (current_limit_cnt_ch2 < MAX_CURRENT_COUNTER)
				current_limit_cnt_ch2++;
			else
			{
				Session_Channel_2_Stop();
				SetBitGlobalErrors (CH2, BIT_ERROR_CURRENT);

				sprintf(&buffSendErr[0], (const char *) "ERROR(0x%03X)\r\n", ERR_CHANNEL_ANTENNA_CURRENT_OUT_OF_RANGE(2));
				UART_PC_Send(&buffSendErr[0]);
				sprintf(&buffSendErr[0], (const char *) "current was: %d\r\n", actual_current[CH2]);
				UART_PC_Send(&buffSendErr[0]);
				return FIN_ERROR;
			}
		}
		new_current_sample_ch2 = 0;
	}
	return TRABAJANDO;
}

unsigned char Current_Limit_CheckCh3 (void)
{
	if (new_current_sample_ch3)
	{
		if (session_ch_3.peak_current_limit < actual_current[CH3])
		{
			if (current_limit_cnt_ch3 < MAX_CURRENT_COUNTER)
				current_limit_cnt_ch3++;
			else
			{
				Session_Channel_3_Stop();
				SetBitGlobalErrors (CH3, BIT_ERROR_CURRENT);

				sprintf(&buffSendErr[0], (const char *) "ERROR(0x%03X)\r\n", ERR_CHANNEL_ANTENNA_CURRENT_OUT_OF_RANGE(3));
				UART_PC_Send(&buffSendErr[0]);
				sprintf(&buffSendErr[0], (const char *) "current was: %d\r\n", actual_current[CH3]);
				UART_PC_Send(&buffSendErr[0]);
				return FIN_ERROR;
			}
		}
		new_current_sample_ch3 = 0;
	}
	return TRABAJANDO;
}

unsigned char Current_Limit_CheckCh4 (void)
{
	if (new_current_sample_ch4)
	{
		if (session_ch_4.peak_current_limit < actual_current[CH4])
		{
			if (current_limit_cnt_ch4 < MAX_CURRENT_COUNTER)
				current_limit_cnt_ch4++;
			else
			{
				Session_Channel_4_Stop();
				SetBitGlobalErrors (CH4, BIT_ERROR_CURRENT);

				sprintf(&buffSendErr[0], (const char *) "ERROR(0x%03X)\r\n", ERR_CHANNEL_ANTENNA_CURRENT_OUT_OF_RANGE(4));
				UART_PC_Send(&buffSendErr[0]);
				sprintf(&buffSendErr[0], (const char *) "current was: %d\r\n", actual_current[CH4]);
				UART_PC_Send(&buffSendErr[0]);
				return FIN_ERROR;
			}
		}
		new_current_sample_ch4 = 0;
	}
	return TRABAJANDO;
}

void Current_Limit_Counter_Reset (void)
{
	current_limit_cnt_ch1 = 0;
	current_limit_cnt_ch2 = 0;
	current_limit_cnt_ch3 = 0;
	current_limit_cnt_ch4 = 0;
}

//--- Para errores globales de antena
//
void CheckforGlobalErrors (void)
{
	//primero me fijo si tengo que revisar los canales
	if ((global_error_ch1 & BIT_ERROR_CHECK) &&
			(global_error_ch2 & BIT_ERROR_CHECK) &&
			(global_error_ch3 & BIT_ERROR_CHECK) &&
			(global_error_ch4 & BIT_ERROR_CHECK))
	{
		if ((global_error_ch1 & BIT_ERROR_CHECK_MASK) &&
				(global_error_ch2 & BIT_ERROR_CHECK_MASK) &&
				(global_error_ch3 & BIT_ERROR_CHECK_MASK) &&
				(global_error_ch4 & BIT_ERROR_CHECK_MASK))
		{
			//tengo error en todos los canales, aviso del error para parar display
			UART_PC_Send((char *) (const char *) "STOP\r\n");
			ResetCheckGlobalErrors ();
			//por las dudas antes de la demora apago todos los PWM
			PWM_CH1_TiempoSubida(0); //pwm 200V.
			PWM_CH1_TiempoMantenimiento(0);
			PWM_CH1_TiempoBajada(0);

			PWM_CH2_TiempoSubida(0); //pwm 200V.
			PWM_CH2_TiempoMantenimiento(0);
			PWM_CH2_TiempoBajada(0);

			PWM_CH3_TiempoSubida(0); //pwm 200V.
			PWM_CH3_TiempoMantenimiento(0);
			PWM_CH3_TiempoBajada(0);

			PWM_CH4_TiempoSubida(0); //pwm 200V.
			PWM_CH4_TiempoMantenimiento(0);
			PWM_CH4_TiempoBajada(0);


			Wait_ms(1000);
			UART_PC_Send((char *) (const char *) "STOP\r\n");
			UART_PC_Send((char *) (const char *) "STOP\r\n");
		}
	}
}

void SetCheckGlobalErrors (unsigned char channel)
{
	switch (channel)
	{
		case CH1:
			global_error_ch1 |= BIT_ERROR_CHECK;
			break;

		case CH2:
			global_error_ch2 |= BIT_ERROR_CHECK;
			break;

		case CH3:
			global_error_ch3 |= BIT_ERROR_CHECK;
			break;

		case CH4:
			global_error_ch4 |= BIT_ERROR_CHECK;
			break;

		default:
			break;
	}
}

void ResetCheckGlobalErrors (void)
{
	global_error_ch1 = 0x00;
	global_error_ch2 = 0x00;
	global_error_ch3 = 0x00;
	global_error_ch4 = 0x00;
}

void SetBitGlobalErrors (unsigned char channel, unsigned char bit_err)
{
	switch (channel)
	{
		case CH1:
			global_error_ch1 |= bit_err;
			break;

		case CH2:
			global_error_ch2 |= bit_err;
			break;

		case CH3:
			global_error_ch3 |= bit_err;
			break;

		case CH4:
			global_error_ch4 |= bit_err;
			break;

		default:
			break;
	}
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

//--- Fin de carga de señales ---//

