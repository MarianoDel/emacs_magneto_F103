//---------------------------------------------------------
// #### PROYECTO MAGNETO GAUSSTEK - MotherBoard ###########
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F103
// ##
// #### GTK_SIGNAL.C ######################################
//---------------------------------------------------------

/* Includes ------------------------------------------------------------------*/
#include "GTK_Hard.h"
#include "channels_defs.h"
#include "answers_defs.h"
#include "timer.h"
#include "usart.h"
#include "stm32f10x.h"
#include "adc.h"
#include "GTK_Estructura.h"
#include "GTK_Signal.h"
#include "flash_program.h"
#include "GTK_Errors.h"
#include "antennas.h"

#include <float.h>
#include <math.h>
#include <stdio.h>
#include <string.h>


/* Private Defines ------------------------------------------------------------*/
//--- Used for Signal Sync ---//
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



//--- Used for Signal Pwm Tables ---//
#define SESSION_WUP_CH1_BUFF_DIM 50
#define SESSION_WUP_CH2_BUFF_DIM 50
#define SESSION_WUP_CH3_BUFF_DIM 50
#define SESSION_WUP_CH4_BUFF_DIM 50

#define SESSION_CDWN_CH1_BUFF_DIM 50
#define SESSION_CDWN_CH2_BUFF_DIM 50
#define SESSION_CDWN_CH3_BUFF_DIM 50
#define SESSION_CDWN_CH4_BUFF_DIM 50

//--- Used for Antenna Overcurrent ---//
#define MAX_CURRENT_COUNTER        5


/* Private Typedefs -----------------------------------------------------------*/
enum states_channel_1 {

	SESSION_CHANNEL_1_INIT = 0,
	SESSION_CHANNEL_1_VERIFY_ANTENNA,
	SESSION_CHANNEL_1_WARMING_UP,
	SESSION_CHANNEL_1_PLATEAU,
	SESSION_CHANNEL_1_COOLING_DOWN,
	SESSION_CHANNEL_1_END
};

enum states_channel_2 {

	SESSION_CHANNEL_2_INIT = 0,
	SESSION_CHANNEL_2_VERIFY_ANTENNA,
	SESSION_CHANNEL_2_WARMING_UP,
	SESSION_CHANNEL_2_PLATEAU,
	SESSION_CHANNEL_2_COOLING_DOWN,
	SESSION_CHANNEL_2_END
};

enum states_channel_3 {

	SESSION_CHANNEL_3_INIT = 0,
	SESSION_CHANNEL_3_VERIFY_ANTENNA,
	SESSION_CHANNEL_3_WARMING_UP,
	SESSION_CHANNEL_3_PLATEAU,
	SESSION_CHANNEL_3_COOLING_DOWN,
	SESSION_CHANNEL_3_END
};

enum states_channel_4 {

	SESSION_CHANNEL_4_INIT = 0,
	SESSION_CHANNEL_4_VERIFY_ANTENNA,
	SESSION_CHANNEL_4_WARMING_UP,
	SESSION_CHANNEL_4_PLATEAU,
	SESSION_CHANNEL_4_COOLING_DOWN,
	SESSION_CHANNEL_4_END
};


enum session_warming_up_channel_states {

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

enum session_plateau_channel_states {

	SESSION_PLATEAU_CHANNEL_INIT = 0,
	SESSION_PLATEAU_CHANNEL_PARAMETERS_CALCULATE,
	SESSION_PLATEAU_CHANNEL_RISING_EDGE,
	SESSION_PLATEAU_CHANNEL_MAINTENANCE,
	SESSION_PLATEAU_CHANNEL_FALLING_EDGE,
	SESSION_PLATEAU_CHANNEL_LOW,
	SESSION_PLATEAU_CHANNEL_END_ERROR
};

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

//--- Current limit Enum ---//
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



/* Externals ------------------------------------------------------------------*/
extern char buffSendErr[64];

extern unsigned char channel_1_pause;
extern unsigned char channel_2_pause;
extern unsigned char channel_3_pause;
extern unsigned char channel_4_pause;

extern session_typedef session_ch_1;
extern session_typedef session_ch_2;
extern session_typedef session_ch_3;
extern session_typedef session_ch_4;

//la uso externa para comprobar el funcionamiento de TIM5 desde el main
extern volatile unsigned int session_warming_up_channel_1_stage_time;

//--- Current limit Externals ---//
extern volatile unsigned char take_current_samples;



/* Globals ---------------------------------------------------------------------*/
//--- Signals PWM tables ---//
//--- Warming-Up ---//
warmingup_coolingdown_typedef table_warming_up_channel_1[SESSION_WUP_CH1_BUFF_DIM];
warmingup_coolingdown_typedef table_warming_up_channel_2[SESSION_WUP_CH2_BUFF_DIM];
warmingup_coolingdown_typedef table_warming_up_channel_3[SESSION_WUP_CH3_BUFF_DIM];
warmingup_coolingdown_typedef table_warming_up_channel_4[SESSION_WUP_CH4_BUFF_DIM];
//--- Plateau ---//
warmingup_coolingdown_typedef table_plateau_channel_1[1];
warmingup_coolingdown_typedef table_plateau_channel_2[1];
warmingup_coolingdown_typedef table_plateau_channel_3[1];
warmingup_coolingdown_typedef table_plateau_channel_4[1];
//--- Cooling down ---//
warmingup_coolingdown_typedef table_cooling_down_channel_1[SESSION_CDWN_CH1_BUFF_DIM];
warmingup_coolingdown_typedef table_cooling_down_channel_2[SESSION_CDWN_CH2_BUFF_DIM];
warmingup_coolingdown_typedef table_cooling_down_channel_3[SESSION_CDWN_CH3_BUFF_DIM];
warmingup_coolingdown_typedef table_cooling_down_channel_4[SESSION_CDWN_CH4_BUFF_DIM];
//--- end ---//

// Global for Sync
unsigned char ch1_sync_state = 0;
unsigned char sync_in_waiting = 0;

//--- States of each channel ---//
unsigned char session_channel_1_state;
unsigned char session_channel_2_state;
unsigned char session_channel_3_state;
unsigned char session_channel_4_state;

//--- Channel 1 ---//


unsigned char session_warming_up_channel_1_state = 0;
unsigned char session_warming_up_channel_1_step = 0;
unsigned short session_warming_up_channel_1_burst_cnt = 0;
unsigned short session_warming_up_channel_1_time;
unsigned short session_warming_up_channel_1_time_2;

unsigned char session_plateau_channel_1_state = 0;
unsigned short session_plateau_channel_1_burst_cnt = 0;
unsigned short session_plateau_channel_1_time;
unsigned short session_plateau_channel_1_time_2;
unsigned int session_plateau_channel_1_stage_time = 0;

unsigned int session_cooling_down_channel_1_stage_time = 0;
unsigned char session_cooling_down_channel_1_state = 0;
unsigned char session_cooling_down_channel_1_step = 0;
unsigned short session_cooling_down_channel_1_burst_cnt = 0;
unsigned short session_cooling_down_channel_1_time;
unsigned short session_cooling_down_channel_1_time_2;

unsigned char fall_type_step_ch1 = 0;

float pwm_slope_channel_1;
float pwm_channel_1;

//--- Channel 2 ---//


unsigned char session_warming_up_channel_2_state = 0;
unsigned char session_warming_up_channel_2_step = 0;
unsigned short session_warming_up_channel_2_burst_cnt = 0;
unsigned short session_warming_up_channel_2_time;
unsigned short session_warming_up_channel_2_time_2;
unsigned int session_warming_up_channel_2_stage_time = 0;

unsigned char session_plateau_channel_2_state = 0;
unsigned short session_plateau_channel_2_burst_cnt = 0;
unsigned short session_plateau_channel_2_time;
unsigned short session_plateau_channel_2_time_2;
unsigned int session_plateau_channel_2_stage_time = 0;

unsigned char session_cooling_down_channel_2_state = 0;
unsigned char session_cooling_down_channel_2_step = 0;
unsigned short session_cooling_down_channel_2_burst_cnt = 0;
unsigned short session_cooling_down_channel_2_time;
unsigned short session_cooling_down_channel_2_time_2;
unsigned int session_cooling_down_channel_2_stage_time = 0;

unsigned char fall_type_step_ch2 = 0;

float pwm_slope_channel_2;
float pwm_channel_2;

//--- Channel 3 ---//


unsigned char session_warming_up_channel_3_state = 0;
unsigned char session_warming_up_channel_3_step = 0;
unsigned short session_warming_up_channel_3_burst_cnt = 0;
unsigned short session_warming_up_channel_3_time;
unsigned short session_warming_up_channel_3_time_2;
unsigned int session_warming_up_channel_3_stage_time = 0;

unsigned char session_plateau_channel_3_state = 0;
unsigned short session_plateau_channel_3_burst_cnt = 0;
unsigned short session_plateau_channel_3_time;
unsigned short session_plateau_channel_3_time_2;
unsigned int session_plateau_channel_3_stage_time = 0;

unsigned char session_cooling_down_channel_3_state = 0;
unsigned char session_cooling_down_channel_3_step = 0;
unsigned short session_cooling_down_channel_3_burst_cnt = 0;
unsigned short session_cooling_down_channel_3_time;
unsigned short session_cooling_down_channel_3_time_2;
unsigned int session_cooling_down_channel_3_stage_time = 0;

unsigned char fall_type_step_ch3 = 0;

float pwm_slope_channel_3;
float pwm_channel_3;

//--- Channel 4 ---//


unsigned char session_warming_up_channel_4_state = 0;
unsigned char session_warming_up_channel_4_step = 0;
unsigned short session_warming_up_channel_4_burst_cnt = 0;
unsigned short session_warming_up_channel_4_time;
unsigned short session_warming_up_channel_4_time_2;
unsigned int session_warming_up_channel_4_stage_time = 0;

unsigned char session_plateau_channel_4_state = 0;
unsigned short session_plateau_channel_4_burst_cnt = 0;
unsigned short session_plateau_channel_4_time;
unsigned short session_plateau_channel_4_time_2;
unsigned int session_plateau_channel_4_stage_time = 0;

unsigned char session_cooling_down_channel_4_state = 0;
unsigned char session_cooling_down_channel_4_step = 0;
unsigned short session_cooling_down_channel_4_burst_cnt = 0;
unsigned short session_cooling_down_channel_4_time;
unsigned short session_cooling_down_channel_4_time_2;
unsigned int session_cooling_down_channel_4_stage_time = 0;

unsigned char fall_type_step_ch4 = 0;

float pwm_slope_channel_4;
float pwm_channel_4;


//--- Current limit Globals ---//
enum CurrState current_limit_state = CURRENT_CH1;
unsigned short actual_current [5] = { 0, 0, 0, 0, 0};	//CH0 es un dummy

unsigned char global_error_ch1 = 0;
unsigned char global_error_ch2 = 0;
unsigned char global_error_ch3 = 0;
unsigned char global_error_ch4 = 0;

//--- Relativo al exceso de corriente en los canales
unsigned char current_limit_cnt_ch1 = 0;
unsigned char current_limit_cnt_ch2 = 0;
unsigned char current_limit_cnt_ch3 = 0;
unsigned char current_limit_cnt_ch4 = 0;

unsigned char new_current_sample_ch1 = 0;
unsigned char new_current_sample_ch2 = 0;
unsigned char new_current_sample_ch3 = 0;
unsigned char new_current_sample_ch4 = 0;



/* Module Exported Functions --------------------------------------------------*/
//--- Timer 5 Callback ---//
void TIM5_IRQ_Callback (void)
{
    //Channel 1.
    if (channel_1_pause == 0)
    {

        session_plateau_channel_1_time++;

        if (session_plateau_channel_1_stage_time)
            session_plateau_channel_1_stage_time--;

        session_warming_up_channel_1_time++;

        if (session_warming_up_channel_1_stage_time)
            session_warming_up_channel_1_stage_time--;

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

        session_warming_up_channel_2_time++;

        if (session_warming_up_channel_2_stage_time)
            session_warming_up_channel_2_stage_time--;

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

        session_warming_up_channel_3_time++;

        if (session_warming_up_channel_3_stage_time)
            session_warming_up_channel_3_stage_time--;

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

        session_warming_up_channel_4_time++;

        if (session_warming_up_channel_4_stage_time)
            session_warming_up_channel_4_stage_time--;

        session_cooling_down_channel_4_time++;

        if (session_cooling_down_channel_4_stage_time)
            session_cooling_down_channel_4_stage_time--;

    }

}
//--- end ---//



//--- Channel 1 ---//
void Channel_1_Init(void)
{
	channel_1_pause = 0;

	session_warming_up_channel_1_time = 0;
	session_warming_up_channel_1_time_2 = 0;
	session_warming_up_channel_1_stage_time = 0;

	session_plateau_channel_1_time = 0;
	session_plateau_channel_1_stage_time = 0;

	session_cooling_down_channel_1_time = 0;
	session_cooling_down_channel_1_time_2 = 0;
	session_cooling_down_channel_1_stage_time = 0;

	session_channel_1_state = 0;

	//limpio tabla WARMING_UP
	memset(&table_warming_up_channel_1[0], '\0', sizeof(table_warming_up_channel_1));

	//limpio tabla PLATEAU
	memset(&table_plateau_channel_1[0], '\0', sizeof(table_plateau_channel_1));

	//limpio tabla COOLING_DOWN
	memset(&table_cooling_down_channel_1[0], '\0', sizeof(table_cooling_down_channel_1));

}

void Session_Channel_1_Start(void)
{
	session_channel_1_state = SESSION_CHANNEL_1_INIT;
	session_ch_1.status = 1;
}

void Session_Channel_1_Stop(void)
{
	session_ch_1.status = 0;
}


void Session_Channel_1 (void)
{
    unsigned char i;

#ifndef WITHOUT_ANTENNA_BOARD_CH1    
    unsigned char actual_antenna_temp;
#endif

    //if the channel is enabled the session occurs.
    //Else the program will wait with the channel turned off.
    if (session_ch_1.status)
    {

        switch (session_channel_1_state)
        {
        case SESSION_CHANNEL_1_INIT:

            PWM_CH1_TiempoSubida(0); //pwm 200V.
            PWM_CH1_TiempoMantenimiento(0);
            PWM_CH1_TiempoBajada(0);

            //Restart stages.
            Session_warming_Up_Channel_1_Restart();
            Session_Plateau_Channel_1_Restart();
            Session_Cooling_Down_Channel_1_Restart();

            ch1_sync_state = SYNC_RESET;
            SetBitGlobalErrors (CH1, BIT_ERROR_CHECK);

#ifdef USE_BUZZER_ON_BOARD
            //Buzzer empezar tratamiento USO ESTE PARA TODOS LOS CANALES
            BuzzerCommands(BUZZER_MULTIPLE_HALF, 1);
#endif

            session_channel_1_state = SESSION_CHANNEL_1_VERIFY_ANTENNA;
            break;

        case SESSION_CHANNEL_1_VERIFY_ANTENNA:

#ifdef WITHOUT_ANTENNA_BOARD_CH1
            //aviso que encontre la antenna y hago un hardcode de parametros
            //--- Antenna parameters ---//
            session_ch_1.ant_resistance_int = HARDCODE_R_INT_CH1;
            session_ch_1.ant_resistance_dec = HARDCODE_R_DEC_CH1;
            session_ch_1.ant_inductance_int = HARDCODE_L_INT_CH1;
            session_ch_1.ant_inductance_dec = HARDCODE_L_DEC_CH1;
            session_ch_1.ant_current_limit_int = HARDCODE_I_INT_CH1;
            session_ch_1.ant_current_limit_dec = HARDCODE_I_DEC_CH1;

            UART_PC_Send("Antenna hardcoded on CH1\r\n");
            session_channel_1_state = SESSION_CHANNEL_1_WARMING_UP;
#else            
            // i = Session_Channel_1_Verify_Antenna(&session_ch_1);
            i = AntennaVerifyForTreatment(CH1);

            if (i == FIN_OK)
            {
                session_channel_1_state = SESSION_CHANNEL_1_WARMING_UP;
            }
                

            else if (i == FIN_ERROR)
            {
                session_channel_1_state = SESSION_CHANNEL_1_END;
                SetBitGlobalErrors (CH1, BIT_ERROR_ANTENNA);
                sprintf(&buffSendErr[0], (const char *) "ERROR(0x%03X)\r\n", ERR_CHANNEL_ANTENNA_DISCONNECTED(1));
                UART_PC_Send(&buffSendErr[0]);
            }
#endif
            break;

        case SESSION_CHANNEL_1_WARMING_UP:

            //Warming up.
            if (channel_1_pause == 0)
            {
                //los errores pueden ser por calculo de parametros o falta de sync cuando se necesita
                i = Session_Warming_Up_Channels(CH1);

                if (i == FIN_OK)
                {
                    session_channel_1_state = SESSION_CHANNEL_1_PLATEAU;
                    UART_PC_Send("End_warming_Up,1\r\n");

                    PWM_CH1_TiempoSubida(0); //pwm 200V.
                    PWM_CH1_TiempoMantenimiento(0);
                    PWM_CH1_TiempoBajada(0);

                }

                if ((i == TRABAJANDO) && (session_warming_up_channel_1_state > SESSION_WARMING_UP_CHANNEL_PARAMETERS_CALCULATE))
                {
                    //i = Current_Limit_CheckCh1();	//TODO: tengo que leer error para que salga la func. abajo
                    Current_Limit_CheckCh1();
                }

                if (i == FIN_ERROR)
                {
                    SetBitGlobalErrors (CH1, BIT_ERROR_WARMING_UP);
                    sprintf(&buffSendErr[0], (const char *) "ERROR(0x%03X)\r\n", ERR_CHANNEL_WARMING_UP(1));
                    UART_PC_Send(&buffSendErr[0]);
                    session_channel_1_state = SESSION_CHANNEL_1_END;
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
                    sprintf(&buffSendErr[0], (const char *) "ERROR(0x%03X)\r\n", ERR_CHANNEL_PLATEAU(1));
                    UART_PC_Send(&buffSendErr[0]);
                    session_channel_1_state = SESSION_CHANNEL_1_END;                    
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
                i = Session_Cooling_Down_Channels(CH1);

                if (i == FIN_OK)
                {
                    session_channel_1_state = SESSION_CHANNEL_1_END;

                    PWM_CH1_TiempoSubida(0); //pwm 200V.
                    PWM_CH1_TiempoMantenimiento(0);
                    PWM_CH1_TiempoBajada(0);

                    UART_PC_Send("End_Cooling_Down,1\r\n");
                    UART_PC_Send("finish,1\r\n");
#ifdef USE_BUZZER_ON_BOARD
                    BuzzerCommands(BUZZER_MULTIPLE_SHORT, 3);
#endif
                }

                if ((i == TRABAJANDO) && (session_cooling_down_channel_1_state > SESSION_COOLING_DOWN_CHANNEL_PARAMETERS_CALCULATE))
                {
                    Current_Limit_CheckCh1();
                }

                if (i == FIN_ERROR)
                {
                    sprintf(&buffSendErr[0], (const char *) "ERROR(0x%03X)\r\n", ERR_CHANNEL_COOLING_DOWN(1));
                    UART_PC_Send(&buffSendErr[0]);
                    session_channel_1_state = SESSION_CHANNEL_1_END;
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
            AntennaEndTreatment(CH1);
            break;

        default:
            session_ch_1.status = 0;
            break;
        }

#ifndef WITHOUT_ANTENNA_BOARD_CH1
        if ((session_channel_1_state >= SESSION_CHANNEL_1_WARMING_UP) &&
            (session_channel_1_state != SESSION_CHANNEL_1_END))
        {
            //reviso que la temperatura actual no sea mayor que la seteada
            actual_antenna_temp = AntennaGetCurrentTemp (CH1);
            if (actual_antenna_temp > session_ch_1.ant_temp_max_int)
            {
                session_channel_1_state = SESSION_CHANNEL_1_END;
                SetBitGlobalErrors (CH1, BIT_ERROR_ANTENNA);
                // sprintf(&buffSendErr[0], (const char *) "ERROR(0x%03X)\r\n",
                        // ERR_CHANNEL_ANTENNA_TMP_OUT_OF_RANGE(1));
                sprintf(buffSendErr, "current: %d max: %d\r\n",
                        actual_antenna_temp,
                        session_ch_1.ant_temp_max_int);
                
                UART_PC_Send(buffSendErr);
            }

            //reviso no haber tenido una perdida de antena
            if (!AntennaGetConnection (CH1))
            {
                session_channel_1_state = SESSION_CHANNEL_1_END;
                SetBitGlobalErrors (CH1, BIT_ERROR_ANTENNA);
                sprintf(&buffSendErr[0], (const char *) "ERROR(0x%03X)\r\n",
                        ERR_CHANNEL_ANTENNA_LOST(1));
                
                UART_PC_Send(buffSendErr);
            }
        }
#endif
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


void Session_warming_Up_Channel_1_Restart(void)
{
	ch1_sync_state = 0;
	sync_in_waiting = 0;
	session_warming_up_channel_1_state = SESSION_WARMING_UP_CHANNEL_INIT;
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

void Session_warming_Up_Channel_2_Restart(void)
{
	session_warming_up_channel_2_state = SESSION_WARMING_UP_CHANNEL_INIT;
}

void Session_Plateau_Channel_2_Restart(void)
{
	session_plateau_channel_2_state = SESSION_PLATEAU_CHANNEL_INIT;
}

void Session_Cooling_Down_Channel_2_Restart(void)
{
	session_cooling_down_channel_2_state = SESSION_COOLING_DOWN_CHANNEL_INIT;
}

void Session_warming_Up_Channel_3_Restart(void)
{
	session_warming_up_channel_3_state = SESSION_WARMING_UP_CHANNEL_INIT;
}

void Session_Plateau_Channel_3_Restart(void)
{
	session_plateau_channel_3_state = SESSION_PLATEAU_CHANNEL_INIT;
}

void Session_Cooling_Down_Channel_3_Restart(void)
{
	session_cooling_down_channel_3_state = SESSION_COOLING_DOWN_CHANNEL_INIT;
}

void Session_warming_Up_Channel_4_Restart(void)
{
	session_warming_up_channel_4_state = SESSION_WARMING_UP_CHANNEL_INIT;
}

void Session_Plateau_Channel_4_Restart(void)
{
	session_plateau_channel_4_state = SESSION_PLATEAU_CHANNEL_INIT;
}

void Session_Cooling_Down_Channel_4_Restart(void)
{
	session_cooling_down_channel_4_state = SESSION_COOLING_DOWN_CHANNEL_INIT;
}

//Funcion que carga la tabla de PWM con parametros fijos para pruebas de bobinas
//utilizar con cuidado, no verifica maximos
//Tambien incluye una breve explicacion del funcionamiento del programa / sesion / tratamiento
unsigned char Session_Channels_Fixed_Parameters (unsigned char channel, unsigned char session_stage)
{
	unsigned char i;
	unsigned char table_length;
	//la sesion (tratamiento) se divide en tres partes Warming UP, Plateau, Cooling Down
	//sobre Warming Up y Cooling Down se puede modificar la potencia entregada Wm Up sube, Cool Dwn baja
	//estructura session_typedef; en particular se llaman session_ch_1; session_ch_2; session_ch_3; session_ch_4
	//stage_1 Warming Up
	//stage_2 Plateau
	//stage_3 Cooling Down
	//incluye mas parametros como: tiempo de cada parte y etapa, parametros de antena conectada, señal elegida

	//se cuenta con una tabla de 50 posiciones para ir cambiando la potencia de las señales
	//si se necesitara en Wm Up y Cool Dwn
	//estructura warmingup_coolingdown_typedef; tabla 50 pos table_warming_up_channel_1; table_warming_up_channel_2
	//table_warming_up_channel_3; table_warming_up_channel_4
	//que incluye todos los pwm y tiempos necesarios para armar la señal, y
	//la tabla contiene estos pwm para las variaciones de potencia
	//el tiempo de avance de cada paso de potencia (50 pasos) se guarda en la estructura de sesion
	//session_ch_1.stage_1_time_per_step; pasos de 100us incremental con TIM5


	//se avanza en cada posicion de la tabla por un timer que se verifica luego de cada señal
	//todos los calculos y carga de la tabla se realiza antes de empezar el tratamiento
	//con la funcion Session_Channels_Parameters_Calculate(channel, session_stage);



	//cada señal dispone de 4 partes Rising, Maintenance, Falling, Stop
	//estas partes tambien se calculan en la funcion Session_Channels_Parameters_Calculate(channel, session_stage);

	//para dibujar una señal entoces necesito: por ejemplo a Wm Up
	//session_ch_1.
	//rising_step_number timer con tick de 100us donde termina el rising time	(10 = 1ms)
	//RISING pwm_200 o pwm_40 inicial y final + step
	//warmingup_coolingdown_typedef.rising_step_number = 30;				//3m
	//MAINTENANCE pwm_40 + step
	//warmingup_coolingdown_typedef.maintenance_step_number = 100;		//10m
	//FALLING_DOWN pwm_n inicial final + step + falling type + falling time + falling step number
	//warmingup_coolingdown_typedef.falling_step_number = 30;				//3m
	//CHANNEL_LOW low step number
	//warmingup_coolingdown_typedef.low_step_number = 60;				//6m

	//los punteros a las estructuras correspondientes (que voy a usar)
	warmingup_coolingdown_typedef * p_table;
	session_typedef * p_session;

	//por ahora solo CH1 y WM UP
	p_table = &table_warming_up_channel_1[0];
	p_session = &session_ch_1;

	//--- Signal Cleaning ---//
	//--- Rising ---//
	for (i = 0; i < SESSION_WUP_CH1_BUFF_DIM; i++)
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
	for (i = 0; i < SESSION_WUP_CH1_BUFF_DIM; i++)
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

	//corriente maxima que voy a permitir
	p_session->peak_current_limit = 3780;	//5A aprox.

	//tiempo de avance de la tabla (como ticks de 100us)
	p_session->stage_1_time_per_step = 2200;	//le doy tiempo de una señal completa

	table_length = 1;		//no pasarse de SESSION_WUP_CH1_BUFF_DIM

	//tiempo de a sesion va a ser (p_session->stage_1_time_per_step) * tble lenght
	for (i = 0; i < table_length; i++)
	{
		//cargo pwm de las señales por ahora un solo paso de tabla
		//Rising
		p_table->rising_pwm_40_initial = 0;
		p_table->rising_pwm_40_final = 0;
		p_table->rising_pwm_200_initial = 261;
		p_table->rising_pwm_200_final = 367;

		p_table->rising_step_number = 30;		//(como ticks de 100us)

		//Maintenance
		p_table->maintenance_pwm_40 = 530;

		p_table->maintenance_step_number = 100;	//(como ticks de 100us)

		//Falling Down
		//LR discharge.
		p_table->falling_time = 3;								//aca le paso el valor elegido (en ms)
		p_table->falling_step_number = 17;					//aca le paso el valor fast discharge (como ticks de 100us)
		p_table->falling_type = FALLING_LR;

		// //Fast discharge.
		// p_table->falling_time = 3;							//aca le paso el valor elegido
		// p_table->falling_step_number = auxiliar_duty;		//descargo mas rapido que el tiempo pedido
		// p_table->falling_type = FALLING_FAST_DISCHARGE;

		//Stop Time
		p_table->low_step_number = 60;		//(como ticks de 100us)
	}

	//--- end ---//
	return FIN_OK;
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

	//usados para calculos intermedios
	float auxiliar_duty = 0.0;
	float voltage = 0.0;
	float voltage2 = 0.0;

	float LR_tau = 0.0;
	float Td = 0.0;
	float Vsnubber = 0.0;
	float period = 0.0;
	float falling_time = 0.0;
	float power_inductance = 0.0;
	float power_r_inductance = 0.0;
	float power_r_snubber = 0.0;

	//Peak Current
	float peak_c = 0.0;

	unsigned int time;
	unsigned char i;

	unsigned char table_lenght = 0;

	//los punteros a las estructuras correspondientes (que voy a usar)
	warmingup_coolingdown_typedef * p_table;
	session_typedef * p_session;

	//--- Primero debo saber que canal y que estado/escenario de sesion estoy calculando ---//
	switch (channel)
	{
		case 1:
			switch (session_stage)
			{
				case WARMING_UP:
					p_table = &table_warming_up_channel_1[0];
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
					p_table = &table_warming_up_channel_2[0];
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
					p_table = &table_warming_up_channel_3[0];
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
					p_table = &table_warming_up_channel_4[0];
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

	//--- Signal Cleaning ---//
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

        antenna_typedef ant_params;
        AntennaGetParamsStruct(channel, &ant_params);

        //new code
	//Resistance.
	resistance = (float) ant_params.resistance_dec;
	resistance /= 100;
	resistance += (float) ant_params.resistance_int;

	//Inductance.
	inductance = (float) ant_params.inductance_dec;
	inductance /= 100;
	inductance += (float) ant_params.inductance_int;
	inductance /=1000;	//ahora este en [Hy]

	//Current limit.
	current_limit = (float) ant_params.current_limit_dec;
	current_limit /= 100;
	current_limit += (float) ant_params.current_limit_int;

        // //old code
	// //Resistance.
	// resistance = (float)p_session->stage_1_resistance_dec;
	// resistance /= 100;
	// resistance += (float)p_session->stage_1_resistance_int;

	// //Inductance.
	// inductance = (float)p_session->stage_1_inductance_dec;
	// inductance /= 100;
	// inductance += (float)p_session->stage_1_inductance_int;
	// inductance /=1000;	//ahora este en [Hy]

	// //Current limit.
	// current_limit = (float)p_session->stage_1_current_limit_dec;
	// current_limit /= 100;
	// current_limit += (float)p_session->stage_1_current_limit_int;
        
	//tengo 196mV / A + offset 456mV
	// peak_c = (current_limit * 1.5) * 0.196 + 0.46;		//convierto corriente max a tension con 50% de margen
	// peak_c = peak_c * 0.303;	//divido 3.3V
	// peak_c = peak_c * 4095;		//valor pico permitido en ADC

	peak_c = (current_limit * 1.2) * 0.6;		//600mV / A + 20%
	// peak_c = (current_limit * 1.08) * 0.6;		//600mV / A + 8%
	peak_c += 0.3;										//sumo 300mV de offet
	peak_c = peak_c * 0.303;	//divido 3.3V
	peak_c = peak_c * 4095;		//valor pico permitido en ADC

	//#PRUEBAS PARA VERSION 2.1 600mV / A		//TODO: pruebas de corriente max 11-07-17
	// //corte en 4A
	//peak_c = 2978;
	//corte en 5A
	//peak_c = 3722;
	//Ahora tengo aprox. 600mV/A o 744./A
	//pruebo exceso de I
	//peak_c = 1116;	//1.5A aprox

	//
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
			//TODO: y stage_2_time_per_step tampoco????
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
		voltage *= (float)inductance * 1000;	//ajusto los ms de lo que viene
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

		//Maximum voltage.
		voltage2 = voltage + (float)(current * resistance);

#ifdef NO_ERRORS_IN_RISING
		// if (voltage2 > PSU_200)
		// 	return FIN_ERROR;
#else
		if (voltage2 > PSU_200)
			return FIN_ERROR;                
#endif

		if (voltage2 < PSU_40)
		{
			//Initial voltage.
			auxiliar_duty = (float)voltage * 1000 / PSU_40;
			(p_table + i)->rising_pwm_40_initial = (unsigned short)auxiliar_duty;

			//Final voltage.
			auxiliar_duty = (float)voltage2 * 1000 / PSU_40;
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
		else	//ya se que es mayor a 40 y menor a 200
		{
#ifdef NO_ERRORS_IN_RISING
                    if (voltage2 > PSU_200)
                    {
                        //si da mal PWM al 100%
			auxiliar_duty = 1000;
			(p_table + i)->rising_pwm_200_initial = (unsigned short)auxiliar_duty;

			//Final voltage.
			auxiliar_duty = 1000;
			(p_table + i)->rising_pwm_200_final = (unsigned short)auxiliar_duty;
                    }
                    else
                    {
                        //si da bien, hace la cuenta como siempre
			//Initial voltage.
			auxiliar_duty = (float)voltage * 1000 / PSU_200;
			(p_table + i)->rising_pwm_200_initial = (unsigned short)auxiliar_duty;

			//Final voltage.
			auxiliar_duty = (float)voltage2 * 1000 / PSU_200;
			(p_table + i)->rising_pwm_200_final = (unsigned short)auxiliar_duty;
                    }


#else
			//Initial voltage.
			auxiliar_duty = (float)voltage * 1000 / PSU_200;
			(p_table + i)->rising_pwm_200_initial = (unsigned short)auxiliar_duty;

			//Final voltage.
			auxiliar_duty = (float)voltage2 * 1000 / PSU_200;
			(p_table + i)->rising_pwm_200_final = (unsigned short)auxiliar_duty;                        
#endif
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

		if (voltage > PSU_40)
			return FIN_ERROR;

		//voltage <= PSU_40
		auxiliar_duty = (float)voltage * 1000 / PSU_40;
		(p_table + i)->maintenance_pwm_40 = (unsigned short)auxiliar_duty;

		//--- Falling ---//
		//Tau.
		//cuentas nuevas
		//LR_tau = (float) (inductance / (1000 * resistance));	//no tocar !LO TOQUE inductance ahora me llega en Hy
		LR_tau = (float) (inductance / (resistance));	//no tocar

		// sprintf(&buffSendErr[0], "\r\nLR_tau: %dms", (int) (LR_tau * 1000));		//TODO:debug 9-7-17
		// UART_PC_Send(&buffSendErr[0]);
		// Wait_ms(30);

		switch(session_stage)
		{
			case WARMING_UP:
				falling_time = p_session->stage_1_falling_time;
				period = p_session->stage_1_rising_time + p_session->stage_1_maintenance_time + p_session->stage_1_falling_time + p_session->stage_1_low_time;
				break;

			case PLATEAU:
				falling_time = p_session->stage_2_falling_time;
				period = p_session->stage_2_rising_time + p_session->stage_2_maintenance_time + p_session->stage_2_falling_time + p_session->stage_2_low_time;
				break;

			case COOLING_DOWN:
				falling_time = p_session->stage_3_falling_time;
				period = p_session->stage_3_rising_time + p_session->stage_3_maintenance_time + p_session->stage_3_falling_time + p_session->stage_3_low_time;
				break;
		}
		falling_time /= 1000;	//a segundos
		period /= 1000;	//a segundos

		//potencia en el inductor
		power_inductance = (float) 0.5 * inductance * final_current * final_current / period;
		//tension rms estimada en RL
		voltage = final_current * resistance * sqrt(falling_time / (3.0 * period));
		//potencia en RL
		power_r_inductance = voltage * voltage / resistance;
		//potencia en Rsnubber
		power_r_snubber = power_inductance - power_r_inductance;

		if (power_r_snubber > RSNUBBER_POWER_MAX)
			return FIN_ERROR;

		//tension rms estimada en Rsnubber (esta es la real del circuito)
		Vsnubber = sqrt(power_r_snubber * resistance_discharge);
		// sprintf(&buffSendErr[0], "\r\nVsnubber: %dV", (int) Vsnubber);		//TODO:debug 9-7-17
		// UART_PC_Send(&buffSendErr[0]);
		// Wait_ms(30);

		//ajusto Vsnubber para calcular descarga rapida lo corrijo en Ipeak * Rserie
		Vsnubber += final_current * resistance;
		Td = (-LR_tau) * log(1 - (current_limit * resistance) / Vsnubber);
		// sprintf(&buffSendErr[0], "\r\ndescarga rapida: %dms", (int) (Td * 1000));		//TODO:debug 9-7-17
		// UART_PC_Send(&buffSendErr[0]);
		// Wait_ms(30);

		//fin cuentas nuevas

		//Nuevo programa bajada
		Td *= 1000;		//paso a ms

		//reviso que Td no supere falling_time + low_time
		switch(session_stage)
		{
			case WARMING_UP:
				if (Td > (p_session->stage_1_falling_time + p_session->stage_1_low_time))
					return FIN_ERROR;
				break;

			case PLATEAU:
				if (Td > (p_session->stage_2_falling_time + p_session->stage_2_low_time))
					return FIN_ERROR;
				break;

			case COOLING_DOWN:
				if (Td > (p_session->stage_3_falling_time + p_session->stage_3_low_time))
					return FIN_ERROR;
				break;
		}

		Td += 0.25;		//ajusto un poquito

		switch(session_stage)
		{
			case WARMING_UP:
				//Slow discharge.
				if (p_session->stage_1_falling_time  > (1000 * LR_tau))	//desde 1 tau utilizo descarga lenta; LR_tau lo paso a ms
				{
					(p_table + i)->falling_time = p_session->stage_1_falling_time;
					(p_table + i)->falling_type = FALLING_SLOW_DISCHARGE;

					voltage =  resistance - (((float)inductance * 1000) / p_session->stage_1_falling_time);
					voltage *= current;

					if (voltage < 0)
						voltage = 0;

					voltage2 = (float) resistance * current;
					voltage2 = voltage - voltage2;

					if (voltage2 < 0)
						voltage2 = 0;

					if ((voltage < PSU_40) && (voltage2 < PSU_40))
					{
						auxiliar_duty = (float)voltage * 1000 / PSU_40;
						(p_table + i)->falling_pwm_40_initial = (unsigned short)auxiliar_duty;

						auxiliar_duty = (float)voltage2 * 1000 / PSU_40;
						(p_table + i)->falling_pwm_40_final = (unsigned short)auxiliar_duty;

						//Steps.
						auxiliar_duty = (float)(p_session->stage_1_falling_time + 0) * 10;
						(p_table + i)->falling_step_number = auxiliar_duty;

					}
					else
						return FIN_ERROR;
				}
				//LR discharge.
				//else if (p_session->stage_1_falling_time > (Td * 2))	//es mayor que 2 veces la descarga rapida
				else if (p_session->stage_1_falling_time > (float)(Td * 1.2))	//es mayor que 1.2 veces la descarga rapida
				{
					(p_table + i)->falling_time = p_session->stage_1_falling_time;	//aca le paso el valor elegido
					//auxiliar_duty = (float)(p_table + i)->falling_time * 10;
					auxiliar_duty = Td * 10;
					(p_table + i)->falling_step_number = auxiliar_duty;				//aca le paso el valor fast discharge
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
					(p_table + i)->falling_type = FALLING_FAST_DISCHARGE;

					(p_table + i)->falling_time = p_session->stage_1_falling_time;	//aca le paso el valor elegido

					if (Td < p_session->stage_1_falling_time)
					{
						auxiliar_duty = p_session->stage_1_falling_time;
						auxiliar_duty *= 10;
						(p_table + i)->falling_step_number = auxiliar_duty;		//descargo mas rapido que el tiempo pedido
						auxiliar_duty = 0;
					}
					else	//Td es mayor que el tiempo disponible
					{
						auxiliar_duty = (float) Td * 10;
						(p_table + i)->falling_step_number = auxiliar_duty;		//aca le paso el valor fast discharge
						auxiliar_duty -= (p_session->stage_1_falling_time) * 10;
					}
				}
				//Fin nuevo programa bajada

				//--- LOW ---//
				if ((p_table + i)->falling_type == FALLING_FAST_DISCHARGE)
					(p_table + i)->low_step_number = (p_session->stage_1_low_time * 10) - auxiliar_duty;
				else
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

					voltage =  resistance - (((float)inductance * 1000) / p_session->stage_2_falling_time);
					voltage *= current;

					if (voltage < 0)
						voltage = 0;

					voltage2 = (float) resistance * current;
					voltage2 = voltage - voltage2;

					if (voltage2 < 0)
						voltage2 = 0;

					if ((voltage < PSU_40) && (voltage2 < PSU_40))
					{
						auxiliar_duty = (float)voltage * 1000 / PSU_40;
						(p_table + i)->falling_pwm_40_initial = (unsigned short)auxiliar_duty;

						auxiliar_duty = (float)voltage2 * 1000 / PSU_40;
						(p_table + i)->falling_pwm_40_final = (unsigned short)auxiliar_duty;

						//Steps.
						auxiliar_duty = (float)(p_session->stage_2_falling_time + 0) * 10;
						(p_table + i)->falling_step_number = auxiliar_duty;

					}
					else
						return FIN_ERROR;
				}
				//LR discharge.
				else if (p_session->stage_2_falling_time > (float)(Td * 1.2))	//es mayor que 1.2 veces la descarga rapida
				{
					(p_table + i)->falling_time = p_session->stage_2_falling_time;	//aca le paso el valor elegido
					auxiliar_duty = Td * 10;
					(p_table + i)->falling_step_number = auxiliar_duty;				//aca le paso el valor fast discharge
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
					(p_table + i)->falling_type = FALLING_FAST_DISCHARGE;

					(p_table + i)->falling_time = p_session->stage_2_falling_time;	//aca le paso el valor elegido

					if (Td < p_session->stage_2_falling_time)
					{
						auxiliar_duty = p_session->stage_2_falling_time;
						auxiliar_duty *= 10;
						(p_table + i)->falling_step_number = auxiliar_duty;		//descargo mas rapido que el tiempo pedido
						auxiliar_duty = 0;
					}
					else	//Td es mayor que el tiempo disponible
					{
						auxiliar_duty = (float) Td * 10;
						(p_table + i)->falling_step_number = auxiliar_duty;		//aca le paso el valor fast discharge
						auxiliar_duty -= (p_session->stage_2_falling_time) * 10;
					}
				}
				//Fin nuevo programa bajada

				//--- LOW ---//
				if ((p_table + i)->falling_type == FALLING_FAST_DISCHARGE)
					(p_table + i)->low_step_number = (p_session->stage_2_low_time * 10) - auxiliar_duty;
				else
					(p_table + i)->low_step_number = p_session->stage_2_low_time * 10;

				(p_table + i)->burst_mode_off = p_session->stage_2_burst_mode_off;
				(p_table + i)->burst_mode_on = p_session->stage_2_burst_mode_on;

				break;

			case COOLING_DOWN:	//TODO: cambiar descarga como WARMING_UP
				//Slow discharge.
				if (p_session->stage_3_falling_time > (1000 * LR_tau))	//desde 1 tau
				{
					(p_table + i)->falling_time = p_session->stage_3_falling_time;
					(p_table + i)->falling_type = FALLING_SLOW_DISCHARGE;

					voltage =  resistance - (((float)inductance * 1000) / p_session->stage_3_falling_time);
					voltage *= current;

					if (voltage < 0)
						voltage = 0;

					voltage2 = (float) resistance * current;
					voltage2 = voltage - voltage2;

					if (voltage2 < 0)
						voltage2 = 0;

					if ((voltage < PSU_40) && (voltage2 < PSU_40))
					{
						auxiliar_duty = (float)voltage * 1000 / PSU_40;
						(p_table + i)->falling_pwm_40_initial = (unsigned short)auxiliar_duty;

						auxiliar_duty = (float)voltage2 * 1000 / PSU_40;
						(p_table + i)->falling_pwm_40_final = (unsigned short)auxiliar_duty;

						//Steps.
						auxiliar_duty = (float)(p_session->stage_3_falling_time + 0) * 10;
						(p_table + i)->falling_step_number = auxiliar_duty;

					}
					else
						return FIN_ERROR;
				}
				//LR discharge.
				else if (p_session->stage_3_falling_time > (float)(Td * 1.2))	//es mayor que 1.2 veces la descarga rapida
				{
					(p_table + i)->falling_time = p_session->stage_3_falling_time;
					auxiliar_duty = Td * 10;
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
					(p_table + i)->falling_type = FALLING_FAST_DISCHARGE;

					(p_table + i)->falling_time = p_session->stage_3_falling_time;	//aca le paso el valor elegido

					if (Td < p_session->stage_3_falling_time)
					{
						auxiliar_duty = p_session->stage_3_falling_time;
						auxiliar_duty *= 10;
						(p_table + i)->falling_step_number = auxiliar_duty;		//descargo mas rapido que el tiempo pedido
						auxiliar_duty = 0;
					}
					else	//Td es mayor que el tiempo disponible
					{
						auxiliar_duty = (float) Td * 10;
						(p_table + i)->falling_step_number = auxiliar_duty;		//aca le paso el valor fast discharge
						auxiliar_duty -= (p_session->stage_3_falling_time) * 10;
					}
				}
				//Fin nuevo programa bajada

				//--- LOW ---//
				if ((p_table + i)->falling_type == FALLING_FAST_DISCHARGE)
					(p_table + i)->low_step_number = (p_session->stage_3_low_time * 10) - auxiliar_duty;
				else
					(p_table + i)->low_step_number = p_session->stage_3_low_time * 10;

				(p_table + i)->burst_mode_off = p_session->stage_3_burst_mode_off;
				(p_table + i)->burst_mode_on = p_session->stage_3_burst_mode_on;

				break;
		}	//fin switch session_stage

	}	//fin for table_lenght

	//--- end ---//
	return FIN_OK;
}

//------ NUEVA WARMING UP ------//
//esta rutina calcula los parametros y luego dibuja la senial con los PWM en cada canal
//para el stage de Warming UP
//ademas va cambiando los niveles de senial generados
unsigned char Session_Warming_Up_Channels (unsigned char channel)
{

	unsigned char i;
	unsigned short time_aux = 0;
	unsigned char * p_session_state;
	unsigned char * p_session_channel_step;
	session_typedef * p_session_ch;
	unsigned short * p_session_burst_cnt;
	warmingup_coolingdown_typedef * p_table;
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
			p_session_state = &session_warming_up_channel_1_state;
			p_session_channel_step = &session_warming_up_channel_1_step;
			p_session_ch = &session_ch_1;
			p_session_burst_cnt = &session_warming_up_channel_1_burst_cnt;
			p_table = &table_warming_up_channel_1[0];
			p_pwm_slope = &pwm_slope_channel_1;
			p_stage_time = &session_warming_up_channel_1_stage_time;	//lo actualiza cada 100us TIM5 (decrementa)
			p_session_time = &session_warming_up_channel_1_time;		//lo actualiza cada 100us TIM5 (incrementa)
			p_session_time_2 = &session_warming_up_channel_1_time_2;
			p_pwm_ch = &pwm_channel_1;
			p_fall_type = &fall_type_step_ch1;
			break;

		case CH2:
			p_session_state = &session_warming_up_channel_2_state;
			p_session_channel_step = &session_warming_up_channel_2_step;
			p_session_ch = &session_ch_2;
			p_session_burst_cnt = &session_warming_up_channel_2_burst_cnt;
			p_table = &table_warming_up_channel_2[0];
			p_pwm_slope = &pwm_slope_channel_2;
			p_stage_time = &session_warming_up_channel_2_stage_time;
			p_session_time = &session_warming_up_channel_2_time;
			p_session_time_2 = &session_warming_up_channel_2_time_2;
			p_pwm_ch = &pwm_channel_2;
			p_fall_type = &fall_type_step_ch2;
			break;

		case CH3:
			p_session_state = &session_warming_up_channel_3_state;
			p_session_channel_step = &session_warming_up_channel_3_step;
			p_session_ch = &session_ch_3;
			p_session_burst_cnt = &session_warming_up_channel_3_burst_cnt;
			p_table = &table_warming_up_channel_3[0];
			p_pwm_slope = &pwm_slope_channel_3;
			p_stage_time = &session_warming_up_channel_3_stage_time;
			p_session_time = &session_warming_up_channel_3_time;
			p_session_time_2 = &session_warming_up_channel_3_time_2;
			p_pwm_ch = &pwm_channel_3;
			p_fall_type = &fall_type_step_ch3;
			break;

		case CH4:
			p_session_state = &session_warming_up_channel_4_state;
			p_session_channel_step = &session_warming_up_channel_4_step;
			p_session_ch = &session_ch_4;
			p_session_burst_cnt = &session_warming_up_channel_4_burst_cnt;
			p_table = &table_warming_up_channel_4[0];
			p_pwm_slope = &pwm_slope_channel_4;
			p_stage_time = &session_warming_up_channel_4_stage_time;
			p_session_time = &session_warming_up_channel_4_time;
			p_session_time_2 = &session_warming_up_channel_4_time_2;
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
                                            //debo tener CH1 presente y me llaman desde otro canal
                                            if (AntennaGetConnection(CH1))
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
                                    // sprintf(&buffSendErr[0], "CH%d no sync needed %d\r\n",
                                    //         channel,
                                    //         p_session_ch->ant_temp_max_int);
                                    
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
//			i = Session_Channels_Fixed_Parameters(channel, WARMING_UP);

			if (i == FIN_OK)
			{
				*p_session_state = SESSION_WARMING_UP_CHANNEL_PARAMETERS_CALCULATE_END;
				SetBitGlobalErrors (channel, BIT_ERROR_PARAMS_FINISH);
			}

			else if (i == FIN_ERROR)
			{
				*p_session_state = SESSION_WARMING_UP_CHANNEL_END_ERROR;

				sprintf(&buffSendErr[0], (const char *) "ERROR(0x%03X)\r\n", ERR_CHANNEL_WARMING_UP_PARAMETERS_CALCULATE(channel));
				UART_PC_Send(&buffSendErr[0]);
			}
			break;

		case SESSION_WARMING_UP_CHANNEL_PARAMETERS_CALCULATE_END:
			//si todos los canales estan listos empiezo con las seniales

			if (((global_error_ch1 & BIT_ERROR_PARAMS_FINISH) || (global_error_ch1 & BIT_ERROR_CHECK_MASK)) &&
					((global_error_ch2 & BIT_ERROR_PARAMS_FINISH) || (global_error_ch2 & BIT_ERROR_CHECK_MASK)) &&
					((global_error_ch3 & BIT_ERROR_PARAMS_FINISH) || (global_error_ch3 & BIT_ERROR_CHECK_MASK)) &&
					((global_error_ch4 & BIT_ERROR_PARAMS_FINISH) || (global_error_ch4 & BIT_ERROR_CHECK_MASK)))
			{

				*p_session_burst_cnt = 0;

				//--- Slope ---//		//slope del primer rising edge
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

			if (*p_stage_time == 0)		//reviso el timer del stage despues de haber terminado la señal completa
			{									//avanzo en la tabla si tengo mas posiciones
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
			//BAJADA FALLING_LR; en falling_time recibo lo elgido y en step_number la descarga rapida
			if ((p_table + (*p_session_channel_step))->falling_type == FALLING_LR)
			{
				switch(*p_fall_type)
				{
					case FALL_START:
						//time_aux = (p_table + (*p_session_channel_step))->falling_step_number;
						time_aux = (p_table + (*p_session_channel_step))->falling_time;
						time_aux *= 10;
						time_aux -= (p_table + (*p_session_channel_step))->falling_step_number;	//le resto la parte de descarga rapida
						//if (*p_session_time > ((p_table + (*p_session_channel_step))->falling_step_number - time_aux))
						if (*p_session_time > time_aux)
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
						//if (*p_session_time >= (p_table + (*p_session_channel_step))->falling_step_number)
						time_aux = (p_table + (*p_session_channel_step))->falling_time;
						time_aux *= 10;
						if (*p_session_time >= time_aux)
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
	warmingup_coolingdown_typedef * p_table;
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
	warmingup_coolingdown_typedef * p_table;
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

	session_warming_up_channel_2_time = 0;
	session_warming_up_channel_2_time_2 = 0;
	session_warming_up_channel_2_stage_time = 0;

	session_cooling_down_channel_2_time = 0;
	session_cooling_down_channel_2_time_2 = 0;
	session_cooling_down_channel_2_stage_time = 0;

	session_channel_2_state = 0;

	//limpio tabla WARMING_UP
	memset(&table_warming_up_channel_2[0], '\0', sizeof(table_warming_up_channel_2));

	//limpio tabla PLATEAU
	memset(&table_plateau_channel_2[0], '\0', sizeof(table_plateau_channel_2));

	//limpio tabla COOLING_DOWN
	memset(&table_cooling_down_channel_2[0], '\0', sizeof(table_cooling_down_channel_2));

}

void Session_Channel_2_Start(void)
{
	session_channel_2_state = SESSION_CHANNEL_2_INIT;
	session_ch_2.status = 1;
}

void Session_Channel_2_Stop(void)
{
	session_ch_2.status = 0;
}


void Session_Channel_2 (void)
{
    unsigned char i;

#ifndef WITHOUT_ANTENNA_BOARD_CH2
    unsigned char actual_antenna_temp;
#endif
    
    //if the channel is enabled the session occurs.
    //Else the program will wait with the channel turned off.
    if (session_ch_2.status)
    {

        switch (session_channel_2_state)
        {
        case SESSION_CHANNEL_2_INIT:

            PWM_CH2_TiempoSubida(0); //pwm 200V.
            PWM_CH2_TiempoMantenimiento(0);
            PWM_CH2_TiempoBajada(0);

            //Restart stages.
            Session_warming_Up_Channel_2_Restart();
            Session_Plateau_Channel_2_Restart();
            Session_Cooling_Down_Channel_2_Restart();

            SetBitGlobalErrors (CH2, BIT_ERROR_CHECK);

            session_channel_2_state = SESSION_CHANNEL_2_VERIFY_ANTENNA;
            break;

        case SESSION_CHANNEL_2_VERIFY_ANTENNA:
#ifdef WITHOUT_ANTENNA_BOARD_CH2
            //aviso que encontre la antenna y hago un hardcode de parametros
            //--- Antenna parameters ---//
            session_ch_2.ant_resistance_int = HARDCODE_R_INT_CH2;
            session_ch_2.ant_resistance_dec = HARDCODE_R_DEC_CH2;
            session_ch_2.ant_inductance_int = HARDCODE_L_INT_CH2;
            session_ch_2.ant_inductance_dec = HARDCODE_L_DEC_CH2;
            session_ch_2.ant_current_limit_int = HARDCODE_I_INT_CH2;
            session_ch_2.ant_current_limit_dec = HARDCODE_I_DEC_CH2;

            UART_PC_Send("Antenna hardcoded on CH2\r\n");
            session_channel_2_state = SESSION_CHANNEL_2_WARMING_UP;
#else
            // i = Session_Channel_2_Verify_Antenna(&session_ch_2);
            i = AntennaVerifyForTreatment(CH2);

            if (i == FIN_OK)
                session_channel_2_state = SESSION_CHANNEL_2_WARMING_UP;

            else if (i == FIN_ERROR)
            {
                SetBitGlobalErrors (CH2, BIT_ERROR_ANTENNA);
                session_channel_2_state = SESSION_CHANNEL_2_END;
                sprintf(&buffSendErr[0], (const char *) "ERROR(0x%03X)\r\n", ERR_CHANNEL_ANTENNA_DISCONNECTED(2));
                UART_PC_Send(&buffSendErr[0]);
            }
#endif
            break;

        case SESSION_CHANNEL_2_WARMING_UP:
            //warming up.
            if (channel_2_pause == 0)
            {

                //los errores pueden ser por calculo de parametros o falta de sync cuando se necesita
                i = Session_Warming_Up_Channels(CH2);

                if (i == FIN_OK)
                {
                    session_channel_2_state = SESSION_CHANNEL_2_PLATEAU;
                    UART_PC_Send("End_warming_Up,2\r\n");

                    PWM_CH2_TiempoSubida(0); //pwm 200V.
                    PWM_CH2_TiempoMantenimiento(0);
                    PWM_CH2_TiempoBajada(0);

                }

                if ((i == TRABAJANDO) && (session_warming_up_channel_2_state > SESSION_WARMING_UP_CHANNEL_PARAMETERS_CALCULATE))
                {
                    Current_Limit_CheckCh2();
                }

                if (i == FIN_ERROR)
                {
                    SetBitGlobalErrors (CH2, BIT_ERROR_WARMING_UP);
                    session_channel_2_state = SESSION_CHANNEL_2_END;
                    sprintf(&buffSendErr[0], (const char *) "ERROR(0x%03X)\r\n", ERR_CHANNEL_WARMING_UP(2));
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
                    session_channel_2_state = SESSION_CHANNEL_2_END;
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
                i = Session_Cooling_Down_Channels(CH2);

                if (i == FIN_OK)
                {
                    session_channel_2_state = SESSION_CHANNEL_2_END;
                    PWM_CH2_TiempoSubida(0); //pwm 200V.
                    PWM_CH2_TiempoMantenimiento(0);
                    PWM_CH2_TiempoBajada(0);

                    UART_PC_Send("End_Cooling_Down,2\r\n");
                    UART_PC_Send("finish,2\r\n");
#ifdef USE_BUZZER_ON_BOARD
                    BuzzerCommands(BUZZER_MULTIPLE_SHORT, 3);
#endif
                }

                if ((i == TRABAJANDO) && (session_cooling_down_channel_2_state > SESSION_COOLING_DOWN_CHANNEL_PARAMETERS_CALCULATE))
                {
                    Current_Limit_CheckCh2();
                }

                if (i == FIN_ERROR)
                {
                    session_channel_2_state = SESSION_CHANNEL_2_END;
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
            AntennaEndTreatment(CH2);
            break;

        default:
            session_ch_2.status = 0;
            break;
        }

#ifndef WITHOUT_ANTENNA_BOARD_CH2
        if ((session_channel_2_state >= SESSION_CHANNEL_2_WARMING_UP) &&
            (session_channel_2_state != SESSION_CHANNEL_2_END))
        {
            //reviso que la temperatura actual no sea mayor que la seteada
            actual_antenna_temp = AntennaGetCurrentTemp (CH2);
            if (actual_antenna_temp > session_ch_2.ant_temp_max_int)
            {
                session_channel_2_state = SESSION_CHANNEL_2_END;
                SetBitGlobalErrors (CH2, BIT_ERROR_ANTENNA);
                sprintf(&buffSendErr[0], (const char *) "ERROR(0x%03X)\r\n",
                        ERR_CHANNEL_ANTENNA_TMP_OUT_OF_RANGE(2));
                
                UART_PC_Send(buffSendErr);
            }

            //reviso no haber tenido una perdida de antena
            if (!AntennaGetConnection (CH2))
            {
                session_channel_2_state = SESSION_CHANNEL_2_END;                
                SetBitGlobalErrors (CH2, BIT_ERROR_ANTENNA);
                sprintf(&buffSendErr[0], (const char *) "ERROR(0x%03X)\r\n",
                        ERR_CHANNEL_ANTENNA_LOST(2));
                
                UART_PC_Send(buffSendErr);
            }
        }
#endif
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

	session_warming_up_channel_3_time = 0;
	session_warming_up_channel_3_time_2 = 0;
	session_warming_up_channel_3_stage_time = 0;

	session_cooling_down_channel_3_time = 0;
	session_cooling_down_channel_3_time_2 = 0;
	session_cooling_down_channel_3_stage_time = 0;

	session_channel_3_state = 0;

	//limpio tabla WARMING_UP
	memset(&table_warming_up_channel_3[0], '\0', sizeof(table_warming_up_channel_3));

	//limpio tabla PLATEAU
	memset(&table_plateau_channel_3[0], '\0', sizeof(table_plateau_channel_3));

	//limpio tabla COOLING_DOWN
	memset(&table_cooling_down_channel_3[0], '\0', sizeof(table_cooling_down_channel_3));
}

void Session_Channel_3_Start(void)
{
	session_channel_3_state = SESSION_CHANNEL_3_INIT;
	session_ch_3.status = 1;
}

void Session_Channel_3_Stop(void)
{
	session_ch_3.status = 0;
}
    

void Session_Channel_3 (void)
{
    unsigned char i;

#ifndef WITHOUT_ANTENNA_BOARD_CH3    
    unsigned char actual_antenna_temp;
#endif

    //if the channel is enabled the session occurs.
    //Else the program will wait with the channel turned off.
    if (session_ch_3.status)
    {

        switch (session_channel_3_state)
        {
        case SESSION_CHANNEL_3_INIT:

            PWM_CH3_TiempoSubida(0); //pwm 200V.
            PWM_CH3_TiempoMantenimiento(0);
            PWM_CH3_TiempoBajada(0);

            //Restart stages.
            Session_warming_Up_Channel_3_Restart();
            Session_Plateau_Channel_3_Restart();
            Session_Cooling_Down_Channel_3_Restart();

            SetBitGlobalErrors (CH3, BIT_ERROR_CHECK);
            session_channel_3_state = SESSION_CHANNEL_3_VERIFY_ANTENNA;
            break;

        case SESSION_CHANNEL_3_VERIFY_ANTENNA:
#ifdef WITHOUT_ANTENNA_BOARD_CH3
            //aviso que encontre la antenna y hago un hardcode de parametros
            //--- Antenna parameters ---//
            session_ch_3.ant_resistance_int = HARDCODE_R_INT_CH3;
            session_ch_3.ant_resistance_dec = HARDCODE_R_DEC_CH3;
            session_ch_3.ant_inductance_int = HARDCODE_L_INT_CH3;
            session_ch_3.ant_inductance_dec = HARDCODE_L_DEC_CH3;
            session_ch_3.ant_current_limit_int = HARDCODE_I_INT_CH3;
            session_ch_3.ant_current_limit_dec = HARDCODE_I_DEC_CH3;

            UART_PC_Send("Antenna hardcoded on CH3\r\n");
            session_channel_3_state = SESSION_CHANNEL_3_WARMING_UP;
#else
            // i = Session_Channel_3_Verify_Antenna(&session_ch_3);
            i = AntennaVerifyForTreatment(CH3);

            if (i == FIN_OK)
                session_channel_3_state = SESSION_CHANNEL_3_WARMING_UP;

            else if (i == FIN_ERROR)
            {
                SetBitGlobalErrors (CH3, BIT_ERROR_ANTENNA);
                session_channel_3_state = SESSION_CHANNEL_3_END;
                sprintf(&buffSendErr[0], (const char *) "ERROR(0x%03X)\r\n", ERR_CHANNEL_ANTENNA_DISCONNECTED(3));
                UART_PC_Send(&buffSendErr[0]);
            }
#endif
            break;

        case SESSION_CHANNEL_3_WARMING_UP:
            //warming up.
            if (channel_3_pause == 0)
            {

                //los errores pueden ser por calculo de parametros o falta de sync cuando se necesita
                i = Session_Warming_Up_Channels(CH3);

                if (i == FIN_OK)
                {
                    session_channel_3_state = SESSION_CHANNEL_3_PLATEAU;
                    UART_PC_Send("End_warming_Up,3\r\n");

                    PWM_CH3_TiempoSubida(0); //pwm 200V.
                    PWM_CH3_TiempoMantenimiento(0);
                    PWM_CH3_TiempoBajada(0);

                }

                if ((i == TRABAJANDO) && (session_warming_up_channel_3_state > SESSION_WARMING_UP_CHANNEL_PARAMETERS_CALCULATE))
                {
                    Current_Limit_CheckCh3();
                }

                if (i == FIN_ERROR)
                {
                    SetBitGlobalErrors (CH3, BIT_ERROR_WARMING_UP);
                    session_channel_3_state = SESSION_CHANNEL_3_END;
                    sprintf(&buffSendErr[0], (const char *) "ERROR(0x%03X)\r\n", ERR_CHANNEL_WARMING_UP(3));
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
                    session_channel_3_state = SESSION_CHANNEL_3_END;
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
                i = Session_Cooling_Down_Channels(CH3);

                if (i == FIN_OK)
                {
                    session_channel_3_state = SESSION_CHANNEL_3_END;
                    PWM_CH3_TiempoSubida(0); //pwm 200V.
                    PWM_CH3_TiempoMantenimiento(0);
                    PWM_CH3_TiempoBajada(0);

                    UART_PC_Send("End_Cooling_Down,3\r\n");
                    UART_PC_Send("finish,3\r\n");
#ifdef USE_BUZZER_ON_BOARD
                    BuzzerCommands(BUZZER_MULTIPLE_SHORT, 3);
#endif
                }

                if ((i == TRABAJANDO) && (session_cooling_down_channel_3_state > SESSION_COOLING_DOWN_CHANNEL_PARAMETERS_CALCULATE))
                {
                    Current_Limit_CheckCh3();
                }

                if (i == FIN_ERROR)
                {
                    session_channel_3_state = SESSION_CHANNEL_3_END;
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
            AntennaEndTreatment(CH3);
            break;

        default:
            session_ch_3.status = 0;
            break;
        }

#ifndef WITHOUT_ANTENNA_BOARD_CH3
        if ((session_channel_3_state >= SESSION_CHANNEL_3_WARMING_UP) &&
            (session_channel_3_state != SESSION_CHANNEL_3_END))
        {
            //reviso que la temperatura actual no sea mayor que la seteada
            actual_antenna_temp = AntennaGetCurrentTemp (CH3);
            if (actual_antenna_temp > session_ch_3.ant_temp_max_int)
            {
                session_channel_3_state = SESSION_CHANNEL_3_END;
                SetBitGlobalErrors (CH3, BIT_ERROR_ANTENNA);
                sprintf(&buffSendErr[0], (const char *) "ERROR(0x%03X)\r\n",
                        ERR_CHANNEL_ANTENNA_TMP_OUT_OF_RANGE(3));
                
                UART_PC_Send(buffSendErr);
            }

            //reviso no haber tenido una perdida de antena
            if (!AntennaGetConnection (CH3))
            {
                session_channel_3_state = SESSION_CHANNEL_3_END;
                SetBitGlobalErrors (CH3, BIT_ERROR_ANTENNA);
                sprintf(&buffSendErr[0], (const char *) "ERROR(0x%03X)\r\n",
                        ERR_CHANNEL_ANTENNA_LOST(3));
                
                UART_PC_Send(buffSendErr);
            }
        }
#endif
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

	session_warming_up_channel_4_time = 0;
	session_warming_up_channel_4_time_2 = 0;
	session_warming_up_channel_4_stage_time = 0;

	session_cooling_down_channel_4_time = 0;
	session_cooling_down_channel_4_time_2 = 0;
	session_cooling_down_channel_4_stage_time = 0;

	session_channel_4_state = 0;


	//limpio tabla WARMING_UP
	memset(&table_warming_up_channel_4[0], '\0', sizeof(table_warming_up_channel_4));

	//limpio tabla PLATEAU
	memset(&table_plateau_channel_4[0], '\0', sizeof(table_plateau_channel_4));

	//limpio tabla COOLING_DOWN
	memset(&table_cooling_down_channel_4[0], '\0', sizeof(table_cooling_down_channel_4));
}

void Session_Channel_4_Start(void)
{
	session_channel_4_state = SESSION_CHANNEL_4_INIT;
	session_ch_4.status = 1;
}

void Session_Channel_4_Stop(void)
{
	session_ch_4.status = 0;
}


void Session_Channel_4 (void)
{
    unsigned char i;

#ifndef WITHOUT_ANTENNA_BOARD_CH4
    unsigned char actual_antenna_temp;
#endif

    //if the channel is enabled the session occurs.
    //Else the program will wait with the channel turned off.

    if (session_ch_4.status)
    {

        switch (session_channel_4_state)
        {
        case SESSION_CHANNEL_4_INIT:

            PWM_CH4_TiempoSubida(0); //pwm 200V.
            PWM_CH4_TiempoMantenimiento(0);
            PWM_CH4_TiempoBajada(0);

            //Restart stages.
            Session_warming_Up_Channel_4_Restart();
            Session_Plateau_Channel_4_Restart();
            Session_Cooling_Down_Channel_4_Restart();

            SetBitGlobalErrors (CH4, BIT_ERROR_CHECK);

            session_channel_4_state = SESSION_CHANNEL_4_VERIFY_ANTENNA;
            break;

        case SESSION_CHANNEL_4_VERIFY_ANTENNA:
#ifdef WITHOUT_ANTENNA_BOARD_CH4
            //aviso que encontre la antenna y hago un hardcode de parametros
            //--- Antenna parameters ---//
            session_ch_4.ant_resistance_int = HARDCODE_R_INT_CH4;
            session_ch_4.ant_resistance_dec = HARDCODE_R_DEC_CH4;
            session_ch_4.ant_inductance_int = HARDCODE_L_INT_CH4;
            session_ch_4.ant_inductance_dec = HARDCODE_L_DEC_CH4;
            session_ch_4.ant_current_limit_int = HARDCODE_I_INT_CH4;
            session_ch_4.ant_current_limit_dec = HARDCODE_I_DEC_CH4;

            UART_PC_Send("Antenna hardcoded on CH4\r\n");
            session_channel_4_state = SESSION_CHANNEL_4_WARMING_UP;
#else
            // i = Session_Channel_4_Verify_Antenna(&session_ch_4);
            i = AntennaVerifyForTreatment(CH4);

            if (i == FIN_OK)
                session_channel_4_state = SESSION_CHANNEL_4_WARMING_UP;

            else if (i == FIN_ERROR)
            {
                SetBitGlobalErrors (CH4, BIT_ERROR_ANTENNA);
                session_channel_4_state = SESSION_CHANNEL_4_END;
                sprintf(&buffSendErr[0], (const char *) "ERROR(0x%03X)\r\n", ERR_CHANNEL_ANTENNA_DISCONNECTED(4));
                UART_PC_Send(&buffSendErr[0]);
            }
#endif
            break;

        case SESSION_CHANNEL_4_WARMING_UP:
            //warming up.
            if (channel_4_pause == 0)
            {
                //los errores pueden ser por calculo de parametros o falta de sync cuando se necesita
                i = Session_Warming_Up_Channels(CH4);

                if (i == FIN_OK)
                {
                    session_channel_4_state = SESSION_CHANNEL_4_PLATEAU;
                    UART_PC_Send("End_warming_Up,4\r\n");

                    PWM_CH4_TiempoSubida(0); //pwm 200V.
                    PWM_CH4_TiempoMantenimiento(0);
                    PWM_CH4_TiempoBajada(0);

                }

                if ((i == TRABAJANDO) && (session_warming_up_channel_4_state > SESSION_WARMING_UP_CHANNEL_PARAMETERS_CALCULATE))
                {
                    Current_Limit_CheckCh4();
                }

                if (i == FIN_ERROR)
                {
                    SetBitGlobalErrors (CH4, BIT_ERROR_WARMING_UP);
                    session_channel_4_state = SESSION_CHANNEL_4_END;
                    sprintf(&buffSendErr[0], (const char *) "ERROR(0x%03X)\r\n", ERR_CHANNEL_WARMING_UP(4));
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
                    session_channel_4_state = SESSION_CHANNEL_4_END;
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
                i = Session_Cooling_Down_Channels(CH4);

                if (i == FIN_OK)
                {
                    session_channel_4_state = SESSION_CHANNEL_4_END;
                    PWM_CH4_TiempoSubida(0); //pwm 200V.
                    PWM_CH4_TiempoMantenimiento(0);
                    PWM_CH4_TiempoBajada(0);

                    UART_PC_Send("End_Cooling_Down,4\r\n");
                    UART_PC_Send("finish,4\r\n");
#ifdef USE_BUZZER_ON_BOARD
                    BuzzerCommands(BUZZER_MULTIPLE_SHORT, 3);
#endif
                }

                if ((i == TRABAJANDO) && (session_cooling_down_channel_4_state > SESSION_COOLING_DOWN_CHANNEL_PARAMETERS_CALCULATE))
                {
                    Current_Limit_CheckCh4();
                }

                if (i == FIN_ERROR)
                {
                    session_channel_4_state = SESSION_CHANNEL_4_END;
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
            AntennaEndTreatment(CH4);
            break;

        default:
            session_ch_4.status = 0;
            break;
        }

#ifndef WITHOUT_ANTENNA_BOARD_CH4
        if ((session_channel_4_state >= SESSION_CHANNEL_4_WARMING_UP) &&
            (session_channel_4_state != SESSION_CHANNEL_4_END))
        {
            //reviso que la temperatura actual no sea mayor que la seteada
            actual_antenna_temp = AntennaGetCurrentTemp (CH4);
            if (actual_antenna_temp > session_ch_4.ant_temp_max_int)
            {
                session_channel_4_state = SESSION_CHANNEL_4_END;
                SetBitGlobalErrors (CH4, BIT_ERROR_ANTENNA);
                sprintf(&buffSendErr[0], (const char *) "ERROR(0x%03X)\r\n",
                        ERR_CHANNEL_ANTENNA_TMP_OUT_OF_RANGE(4));
                
                UART_PC_Send(buffSendErr);
            }

            //reviso no haber tenido una perdida de antena
            if (!AntennaGetConnection (CH4))
            {
                session_channel_4_state = SESSION_CHANNEL_4_END;
                SetBitGlobalErrors (CH4, BIT_ERROR_ANTENNA);
                sprintf(&buffSendErr[0], (const char *) "ERROR(0x%03X)\r\n",
                        ERR_CHANNEL_ANTENNA_LOST(4));
                
                UART_PC_Send(buffSendErr);
            }
        }
#endif
    }
    else
    {
        //Initial state.
        session_channel_4_state = SESSION_CHANNEL_4_INIT;

        //Stop pwm channels.
        PWM_CH4_TiempoSubida(0);
        PWM_CH4_TiempoMantenimiento(0);
        PWM_CH4_TiempoBajada(0);
        //Stop timer interrupt.
    }
}




//si el canal se encuentra activo
//cargo en un vector las muestras corriente de cada canal
void Session_Current_Limit_control (void)
{
#ifdef USE_ADC_SAMPLE_BY_SAMPLE
    switch (current_limit_state)
    {
    case CURRENT_INIT_CHECK:
        if (!take_current_samples)    //synchro 1ms with TIM7
        {
            take_current_samples = 1;
            current_limit_state++;
        }
        break;

    case CURRENT_CH1:
        if (session_ch_1.status)
        {
            // flagMuestreo = 0;
            // //ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 1, ADC_SampleTime_28Cycles5);
            // ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 1, ADC_SampleTime_239Cycles5);
            // ADC_SoftwareStartConvCmd(ADC1, ENABLE);

            ConvertChannel(ADC_Channel_4);
            current_limit_state = CURRENT_CH1_WAIT_SAMPLE;
        }
        else
            current_limit_state = CURRENT_CH2;


        break;

    case CURRENT_CH1_WAIT_SAMPLE:
        // if (flagMuestreo)
        if (ConvertSingleChannelFinishFlag())
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
            // flagMuestreo = 0;
            // //ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 1, ADC_SampleTime_28Cycles5);
            // ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 1, ADC_SampleTime_239Cycles5);
            // ADC_SoftwareStartConvCmd(ADC1, ENABLE);

            ConvertChannel(ADC_Channel_5);
            current_limit_state = CURRENT_CH2_WAIT_SAMPLE;
        }
        else
            current_limit_state = CURRENT_CH3;


        break;

    case CURRENT_CH2_WAIT_SAMPLE:
        // if (flagMuestreo)
        if (ConvertSingleChannelFinishFlag())
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
            // flagMuestreo = 0;
            // //ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 1, ADC_SampleTime_28Cycles5);
            // ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 1, ADC_SampleTime_239Cycles5);
            // ADC_SoftwareStartConvCmd(ADC1, ENABLE);
            
            ConvertChannel(ADC_Channel_6);
            current_limit_state = CURRENT_CH3_WAIT_SAMPLE;
        }
        else
            current_limit_state = CURRENT_CH4;


        break;

    case CURRENT_CH3_WAIT_SAMPLE:
        // if (flagMuestreo)
        if (ConvertSingleChannelFinishFlag())
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
            // flagMuestreo = 0;
            // ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 1, ADC_SampleTime_28Cycles5);
            // ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 1, ADC_SampleTime_239Cycles5);
            // ADC_SoftwareStartConvCmd(ADC1, ENABLE);
            ConvertChannel(ADC_Channel_7);
            current_limit_state = CURRENT_CH4_WAIT_SAMPLE;
        }
        else
            current_limit_state = CURRENT_INIT_CHECK;


        break;

    case CURRENT_CH4_WAIT_SAMPLE:
        // if (flagMuestreo)
        if (ConvertSingleChannelFinishFlag())
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

        //prueba envio a la PC
        // sprintf(buffSendErr, "IS1: %d IS2: %d IS3: %d IS4: %d\n",
        //         actual_current[CH1],
        //         actual_current[CH2],
        //         actual_current[CH3],
        //         actual_current[CH4]);

        // UART_PC_Send(buffSendErr);
        // Wait_ms(2000);
        //fin prueba envio a la PC                

        break;

    default:
        current_limit_state = CURRENT_INIT_CHECK;
        break;

    }
#endif    //USE_ADC_SAMPLE_BY_SAMPLE
    
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

				//sprintf(&buffSendErr[0], (const char *) "ERROR(0x%03X)\r\n", ERR_CHANNEL_ANTENNA_CURRENT_OUT_OF_RANGE(1));
				// sprintf(&buffSendErr[0], (const char *) "ERROR(0x%02X)\r\n", ERR_CHANNEL_ANTENNA_CURRENT_OUT_OF_RANGE(1));
				// UART_PC_Send(&buffSendErr[0]);
				sprintf(&buffSendErr[0], (const char *) "current was: %d\r\n", actual_current[CH1]);
				UART_PC_Send(&buffSendErr[0]);
				return FIN_ERROR;
			}
		}
		else
		{
			if (current_limit_cnt_ch1)
				current_limit_cnt_ch1--;
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

				// sprintf(&buffSendErr[0], (const char *) "ERROR(0x%03X)\r\n", ERR_CHANNEL_ANTENNA_CURRENT_OUT_OF_RANGE(2));
				// sprintf(&buffSendErr[0], (const char *) "ERROR(0x%02X)\r\n", ERR_CHANNEL_ANTENNA_CURRENT_OUT_OF_RANGE(2));
				// UART_PC_Send(&buffSendErr[0]);
				sprintf(&buffSendErr[0], (const char *) "current was: %d\r\n", actual_current[CH2]);
				UART_PC_Send(&buffSendErr[0]);
				return FIN_ERROR;
			}
		}
		else
		{
			if (current_limit_cnt_ch2)
				current_limit_cnt_ch2--;
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

//				sprintf(&buffSendErr[0], (const char *) "ERROR(0x%03X)\r\n", ERR_CHANNEL_ANTENNA_CURRENT_OUT_OF_RANGE(3));
				// sprintf(&buffSendErr[0], (const char *) "ERROR(0x%02X)\r\n", ERR_CHANNEL_ANTENNA_CURRENT_OUT_OF_RANGE(3));
				// UART_PC_Send(&buffSendErr[0]);
				sprintf(&buffSendErr[0], (const char *) "current was: %d\r\n", actual_current[CH3]);
				UART_PC_Send(&buffSendErr[0]);
				return FIN_ERROR;
			}
		}
		else
		{
			if (current_limit_cnt_ch3)
				current_limit_cnt_ch3--;
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

				//sprintf(&buffSendErr[0], (const char *) "ERROR(0x%03X)\r\n", ERR_CHANNEL_ANTENNA_CURRENT_OUT_OF_RANGE(4));
				// sprintf(&buffSendErr[0], (const char *) "ERROR(0x%02X)\r\n", ERR_CHANNEL_ANTENNA_CURRENT_OUT_OF_RANGE(4));
				// UART_PC_Send(&buffSendErr[0]);
				sprintf(&buffSendErr[0], (const char *) "current was: %d\r\n", actual_current[CH4]);
				UART_PC_Send(&buffSendErr[0]);
				return FIN_ERROR;
			}
		}
		else
		{
			if (current_limit_cnt_ch4)
				current_limit_cnt_ch4--;
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
	unsigned char dummy_stop = 0;
	//primero me fijo si tengo que revisar los canales
	//lo activa la sesion de cada canal al iniciar
	if ((global_error_ch1 & BIT_ERROR_CHECK) ||
			(global_error_ch2 & BIT_ERROR_CHECK) ||
			(global_error_ch3 & BIT_ERROR_CHECK) ||
			(global_error_ch4 & BIT_ERROR_CHECK))
	{
		if (global_error_ch1 & BIT_ERROR_CURRENT)
		{
			StopAllChannels ();
#ifdef USE_BUZZER_ON_BOARD
			BuzzerCommands(BUZZER_MULTIPLE_SHORT, 5);
#endif
			Wait_ms(100);
			sprintf(&buffSendErr[0], (const char *) "ERROR(0x%02X)\r\n", ERR_CHANNEL_ANTENNA_CURRENT_OUT_OF_RANGE(1));
			UART_PC_Send(&buffSendErr[0]);

			Wait_ms(100);
			sprintf(&buffSendErr[0], (const char *) "ERROR(0x%02X)\r\n", ERR_CHANNEL_ANTENNA_CURRENT_OUT_OF_RANGE(1));
			UART_PC_Send(&buffSendErr[0]);

			ResetCheckGlobalErrors ();
			dummy_stop = 1;
		}

		if (global_error_ch2 & BIT_ERROR_CURRENT)
		{
			StopAllChannels ();
#ifdef USE_BUZZER_ON_BOARD
			BuzzerCommands(BUZZER_MULTIPLE_SHORT, 5);
#endif
			Wait_ms(100);
			sprintf(&buffSendErr[0], (const char *) "ERROR(0x%02X)\r\n", ERR_CHANNEL_ANTENNA_CURRENT_OUT_OF_RANGE(2));
			UART_PC_Send(&buffSendErr[0]);

			Wait_ms(100);
			sprintf(&buffSendErr[0], (const char *) "ERROR(0x%02X)\r\n", ERR_CHANNEL_ANTENNA_CURRENT_OUT_OF_RANGE(2));
			UART_PC_Send(&buffSendErr[0]);

			ResetCheckGlobalErrors ();
			dummy_stop = 1;
		}

		if (global_error_ch3 & BIT_ERROR_CURRENT)
		{
			StopAllChannels ();
#ifdef USE_BUZZER_ON_BOARD
			BuzzerCommands(BUZZER_MULTIPLE_SHORT, 5);
#endif
			Wait_ms(100);
			sprintf(&buffSendErr[0], (const char *) "ERROR(0x%02X)\r\n", ERR_CHANNEL_ANTENNA_CURRENT_OUT_OF_RANGE(3));
			UART_PC_Send(&buffSendErr[0]);

			Wait_ms(100);
			sprintf(&buffSendErr[0], (const char *) "ERROR(0x%02X)\r\n", ERR_CHANNEL_ANTENNA_CURRENT_OUT_OF_RANGE(3));
			UART_PC_Send(&buffSendErr[0]);

			ResetCheckGlobalErrors ();
			dummy_stop = 1;
		}

		if (global_error_ch4 & BIT_ERROR_CURRENT)
		{
			StopAllChannels ();
#ifdef USE_BUZZER_ON_BOARD
			BuzzerCommands(BUZZER_MULTIPLE_SHORT, 5);
#endif
			Wait_ms(100);
			sprintf(&buffSendErr[0], (const char *) "ERROR(0x%02X)\r\n", ERR_CHANNEL_ANTENNA_CURRENT_OUT_OF_RANGE(4));
			UART_PC_Send(&buffSendErr[0]);

			Wait_ms(100);
			sprintf(&buffSendErr[0], (const char *) "ERROR(0x%02X)\r\n", ERR_CHANNEL_ANTENNA_CURRENT_OUT_OF_RANGE(4));
			UART_PC_Send(&buffSendErr[0]);

			ResetCheckGlobalErrors ();
			dummy_stop = 1;
		}

		if (!dummy_stop)	//revisa no haber saltado por corriente antes
		{
			if ((global_error_ch1 & (BIT_ERROR_ANTENNA | BIT_ERROR_WARMING_UP)) &&
					(global_error_ch2 & (BIT_ERROR_ANTENNA | BIT_ERROR_WARMING_UP)) &&
					(global_error_ch3 & (BIT_ERROR_ANTENNA | BIT_ERROR_WARMING_UP)) &&
					(global_error_ch4 & (BIT_ERROR_ANTENNA | BIT_ERROR_WARMING_UP)))
					{
						//tengo errores en todos los canales que no son por corriente
						StopAllChannels ();
#ifdef USE_BUZZER_ON_BOARD
						BuzzerCommands(BUZZER_MULTIPLE_LONG, 1);
#endif						
						Wait_ms(100);
						UART_PC_Send((char *) (const char *) "STOP\r\n");

						Wait_ms(100);
						UART_PC_Send((char *) (const char *) "STOP\r\n");
						ResetCheckGlobalErrors ();
					}
			}



		// if ((global_error_ch1 & BIT_ERROR_CHECK_MASK) &&
		// 		(global_error_ch2 & BIT_ERROR_CHECK_MASK) &&
		// 		(global_error_ch3 & BIT_ERROR_CHECK_MASK) &&
		// 		(global_error_ch4 & BIT_ERROR_CHECK_MASK))
		// {
		// 	//tengo error en todos los canales, aviso del error para parar display
		// 	UART_PC_Send((char *) (const char *) "STOP\r\n");
		// 	ResetCheckGlobalErrors ();
		// 	//por las dudas antes de la demora apago todos los PWM
		// 	PWM_CH1_TiempoSubida(0); //pwm 200V.
		// 	PWM_CH1_TiempoMantenimiento(0);
		// 	PWM_CH1_TiempoBajada(0);
		//
		// 	PWM_CH2_TiempoSubida(0); //pwm 200V.
		// 	PWM_CH2_TiempoMantenimiento(0);
		// 	PWM_CH2_TiempoBajada(0);
		//
		// 	PWM_CH3_TiempoSubida(0); //pwm 200V.
		// 	PWM_CH3_TiempoMantenimiento(0);
		// 	PWM_CH3_TiempoBajada(0);
		//
		// 	PWM_CH4_TiempoSubida(0); //pwm 200V.
		// 	PWM_CH4_TiempoMantenimiento(0);
		// 	PWM_CH4_TiempoBajada(0);
		//
		//
		// 	Wait_ms(1000);
		// 	UART_PC_Send((char *) (const char *) "STOP\r\n");
		// 	UART_PC_Send((char *) (const char *) "STOP\r\n");
		// }
	}
}

void StopAllChannels (void)
{
    Session_Channel_1_Stop();
    Session_Channel_2_Stop();
    Session_Channel_3_Stop();
    Session_Channel_4_Stop();

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

    //esto es terminar por error todos los canales
    //o desde modulo comms terminar con finish_ok o stop (desde PC)
    AntennaEndTreatment(CH1);
    AntennaEndTreatment(CH2);
    AntennaEndTreatment(CH3);
    AntennaEndTreatment(CH4);
        
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


//--- Fin de carga de seniales ---//

//--- end of file ---//
