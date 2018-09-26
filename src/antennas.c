//---------------------------------------------------------
// #### PROYECTO MAGNETO GAUSSTEK - MotherBoard ###########
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F103
// ##
// #### ANTENNAS.C ###########################################
//---------------------------------------------------------

/* Includes ------------------------------------------------------------------*/
#include "antennas.h"
#include "usart.h"
#include "GTK_Estructura.h"
#include "GTK_Hard.h"        //define el nombre de los canales

#include <stdio.h>
#include <string.h>

//--- Used for Antenna Sync - Time base ---//
#define SESSION_CHANNEL_ASK_TEMP 2500 //2.5 segundos
#define SESSION_CHANNEL_ANSWER_TEMP 10000 //10segundos.

// #define SESSION_CHANNEL_1_VERIFY_ANTENNA_TIME 5000			//ms
// #define SESSION_CHANNEL_2_VERIFY_ANTENNA_TIME 5000			//ms
// #define SESSION_CHANNEL_3_VERIFY_ANTENNA_TIME 5000			//ms
// #define SESSION_CHANNEL_4_VERIFY_ANTENNA_TIME 5000			//ms

#define ANT_LOCKED    0x01
#define ANT_CONNECTED    0x02
#define ANT_HAVE_PARAMS    0x04
#define ANT_NEW_PARAMS     0x08
#define ANT_IN_TREATMENT    0x10
#define ANT_HAVE_NAME    0x20

#define AntennaNewParamsCh1    (ch1_ant_conn_status |= ANT_NEW_PARAMS)
#define AntennaNewParamsCh2    (ch2_ant_conn_status |= ANT_NEW_PARAMS)
#define AntennaNewParamsCh3    (ch3_ant_conn_status |= ANT_NEW_PARAMS)
#define AntennaNewParamsCh4    (ch4_ant_conn_status |= ANT_NEW_PARAMS)

#define AntennaNoNewParamsCh1    (ch1_ant_conn_status &= ~ANT_NEW_PARAMS)
#define AntennaNoNewParamsCh2    (ch2_ant_conn_status &= ~ANT_NEW_PARAMS)
#define AntennaNoNewParamsCh3    (ch3_ant_conn_status &= ~ANT_NEW_PARAMS)
#define AntennaNoNewParamsCh4    (ch4_ant_conn_status &= ~ANT_NEW_PARAMS)

#define AntennaCheckNewParamsCh1    (ch1_ant_conn_status & ANT_NEW_PARAMS)
#define AntennaCheckNewParamsCh2    (ch2_ant_conn_status & ANT_NEW_PARAMS)
#define AntennaCheckNewParamsCh3    (ch3_ant_conn_status & ANT_NEW_PARAMS)
#define AntennaCheckNewParamsCh4    (ch4_ant_conn_status & ANT_NEW_PARAMS)

#define AntennaParamsCh1    (ch1_ant_conn_status |= ANT_HAVE_PARAMS)
#define AntennaParamsCh2    (ch2_ant_conn_status |= ANT_HAVE_PARAMS)
#define AntennaParamsCh3    (ch3_ant_conn_status |= ANT_HAVE_PARAMS)
#define AntennaParamsCh4    (ch4_ant_conn_status |= ANT_HAVE_PARAMS)

#define AntennaNoParamsCh1    (ch1_ant_conn_status &= ~ANT_HAVE_PARAMS)
#define AntennaNoParamsCh2    (ch2_ant_conn_status &= ~ANT_HAVE_PARAMS)
#define AntennaNoParamsCh3    (ch3_ant_conn_status &= ~ANT_HAVE_PARAMS)
#define AntennaNoParamsCh4    (ch4_ant_conn_status &= ~ANT_HAVE_PARAMS)

#define AntennaCheckParamsCh1    (ch1_ant_conn_status & ANT_HAVE_PARAMS)
#define AntennaCheckParamsCh2    (ch2_ant_conn_status & ANT_HAVE_PARAMS)
#define AntennaCheckParamsCh3    (ch3_ant_conn_status & ANT_HAVE_PARAMS)
#define AntennaCheckParamsCh4    (ch4_ant_conn_status & ANT_HAVE_PARAMS)

#define AntennaConnectionCh1    (ch1_ant_conn_status |= ANT_CONNECTED)
#define AntennaConnectionCh2    (ch2_ant_conn_status |= ANT_CONNECTED)
#define AntennaConnectionCh3    (ch3_ant_conn_status |= ANT_CONNECTED)
#define AntennaConnectionCh4    (ch4_ant_conn_status |= ANT_CONNECTED)

#define AntennaNoConnectionCh1    (ch1_ant_conn_status &= ~ANT_CONNECTED)
#define AntennaNoConnectionCh2    (ch2_ant_conn_status &= ~ANT_CONNECTED)
#define AntennaNoConnectionCh3    (ch3_ant_conn_status &= ~ANT_CONNECTED)
#define AntennaNoConnectionCh4    (ch4_ant_conn_status &= ~ANT_CONNECTED)

#define AntennaCheckConnectionCh1    (ch1_ant_conn_status & ANT_CONNECTED)
#define AntennaCheckConnectionCh2    (ch2_ant_conn_status & ANT_CONNECTED)
#define AntennaCheckConnectionCh3    (ch3_ant_conn_status & ANT_CONNECTED)
#define AntennaCheckConnectionCh4    (ch4_ant_conn_status & ANT_CONNECTED)

#define AntennaLockCh1    (ch1_ant_conn_status |= ANT_LOCKED)
#define AntennaLockCh2    (ch2_ant_conn_status |= ANT_LOCKED)
#define AntennaLockCh3    (ch3_ant_conn_status |= ANT_LOCKED)
#define AntennaLockCh4    (ch4_ant_conn_status |= ANT_LOCKED)

#define AntennaUnlockCh1    (ch1_ant_conn_status &= ~ANT_LOCKED)
#define AntennaUnlockCh2    (ch2_ant_conn_status &= ~ANT_LOCKED)
#define AntennaUnlockCh3    (ch3_ant_conn_status &= ~ANT_LOCKED)
#define AntennaUnlockCh4    (ch4_ant_conn_status &= ~ANT_LOCKED)

#define AntennaCheckLockCh1    (ch1_ant_conn_status & ANT_LOCKED)
#define AntennaCheckLockCh2    (ch2_ant_conn_status & ANT_LOCKED)
#define AntennaCheckLockCh3    (ch3_ant_conn_status & ANT_LOCKED)
#define AntennaCheckLockCh4    (ch4_ant_conn_status & ANT_LOCKED)

#define AntennaInTreatmentCh1    (ch1_ant_conn_status |= ANT_IN_TREATMENT)
#define AntennaInTreatmentCh2    (ch2_ant_conn_status |= ANT_IN_TREATMENT)
#define AntennaInTreatmentCh3    (ch3_ant_conn_status |= ANT_IN_TREATMENT)
#define AntennaInTreatmentCh4    (ch4_ant_conn_status |= ANT_IN_TREATMENT)

#define AntennaNoInTreatmentCh1    (ch1_ant_conn_status &= ~ANT_IN_TREATMENT)
#define AntennaNoInTreatmentCh2    (ch2_ant_conn_status &= ~ANT_IN_TREATMENT)
#define AntennaNoInTreatmentCh3    (ch3_ant_conn_status &= ~ANT_IN_TREATMENT)
#define AntennaNoInTreatmentCh4    (ch4_ant_conn_status &= ~ANT_IN_TREATMENT)

#define AntennaCheckInTreatmentCh1    (ch1_ant_conn_status & ANT_IN_TREATMENT)
#define AntennaCheckInTreatmentCh2    (ch2_ant_conn_status & ANT_IN_TREATMENT)
#define AntennaCheckInTreatmentCh3    (ch3_ant_conn_status & ANT_IN_TREATMENT)
#define AntennaCheckInTreatmentCh4    (ch4_ant_conn_status & ANT_IN_TREATMENT)

#define AntennaNameCh1    (ch1_ant_conn_status |= ANT_HAVE_NAME)
#define AntennaNameCh2    (ch2_ant_conn_status |= ANT_HAVE_NAME)
#define AntennaNameCh3    (ch3_ant_conn_status |= ANT_HAVE_NAME)
#define AntennaNameCh4    (ch4_ant_conn_status |= ANT_HAVE_NAME)

#define AntennaNoNameCh1    (ch1_ant_conn_status &= ~ANT_HAVE_NAME)
#define AntennaNoNameCh2    (ch2_ant_conn_status &= ~ANT_HAVE_NAME)
#define AntennaNoNameCh3    (ch3_ant_conn_status &= ~ANT_HAVE_NAME)
#define AntennaNoNameCh4    (ch4_ant_conn_status &= ~ANT_HAVE_NAME)

#define AntennaCheckNameCh1    (ch1_ant_conn_status & ANT_HAVE_NAME)
#define AntennaCheckNameCh2    (ch2_ant_conn_status & ANT_HAVE_NAME)
#define AntennaCheckNameCh3    (ch3_ant_conn_status & ANT_HAVE_NAME)
#define AntennaCheckNameCh4    (ch4_ant_conn_status & ANT_HAVE_NAME)

/* Externals ------------------------------------------------------------------*/
extern volatile unsigned short antenna_timer;

extern session_typedef session_ch_1;
extern session_typedef session_ch_2;
extern session_typedef session_ch_3;
extern session_typedef session_ch_4;

#ifdef SOFTWARE_VERSION_1_3
extern volatile unsigned short antenna_info_timer;
#endif


/* Globals ----------------------------------------------------------------------------*/
antenna_state_t antenna_state;

unsigned char ch1_ant_current_temp_int = 0;
unsigned char ch1_ant_current_temp_dec = 0;
unsigned char ch2_ant_current_temp_int = 0;
unsigned char ch2_ant_current_temp_dec = 0;
unsigned char ch3_ant_current_temp_int = 0;
unsigned char ch3_ant_current_temp_dec = 0;
unsigned char ch4_ant_current_temp_int = 0;
unsigned char ch4_ant_current_temp_dec = 0;

unsigned char ch1_ant_conn_status = 0;
unsigned char ch2_ant_conn_status = 0;
unsigned char ch3_ant_conn_status = 0;
unsigned char ch4_ant_conn_status = 0;

unsigned char keepalive_ch1 = 0;
unsigned char keepalive_ch2 = 0;
unsigned char keepalive_ch3 = 0;
unsigned char keepalive_ch4 = 0;

unsigned char keepalive_name_ch1 = 0;
unsigned char keepalive_name_ch2 = 0;
unsigned char keepalive_name_ch3 = 0;
unsigned char keepalive_name_ch4 = 0;

#ifdef SOFTWARE_VERSION_1_3
unsigned char antenna_info_sended = 0;
unsigned char antenna_send_info_with_timer = 0;
#endif


/* Module Private Functions -----------------------------------------------------------*/
void AntennaBackupParams (antenna_typedef *, session_typedef *);
void AntennaSendKnowParams (void);

void AntennaFlushParamsInLock (antenna_typedef *);
void AntennaFlushParams (unsigned char);


/* Module Exported Functions ----------------------------------------------------------*/
void AntennaUpdateStates (void)
{
    switch (antenna_state)
    {
    case ANTENNA_INIT:
#ifdef SOFTWARE_VERSION_1_3
        antenna_info_timer = ANTENNA_INFO_TIMER_FIRST_START;
        antenna_info_sended = 0;
#endif
        antenna_state++;
        break;

    case ANTENNA_IN_STANDBY:
        if (!antenna_timer)
        {
            if (AntennaCheckParamsCh1)
            {
                if (AntennaCheckConnectionCh1)    //tengo parametros y contesta
                {
                    if ((!AntennaCheckNameCh1) && (keepalive_name_ch1))
                    {
                        keepalive_name_ch1--;
                        UART_CH1_Send("get_name\r\n");
                    }
                    else    //tengo el nombre o me canse de preguntar
                        UART_CH1_Send("keepalive\r\n");

                    keepalive_ch1 = KEEP_ALIVE_COUNTER;                    
                    AntennaNoConnectionCh1;
                }
                else 
                {
                    if (keepalive_ch1)
                    {
                        keepalive_ch1--;
                        UART_CH1_Send("keepalive\r\n");
                    }
                    else    //tenia parametros y los perdio
                    {
                        AntennaNewParamsCh1;
                        AntennaNoParamsCh1;
                        AntennaNoNameCh1;
                    }
                }
            }
            else
                UART_CH1_Send("get_params\r\n");

            if (AntennaCheckParamsCh2)
            {
                if (AntennaCheckConnectionCh2)    //tengo parametros y contesta
                {
                    if ((!AntennaCheckNameCh2) && (keepalive_name_ch2))
                    {
                        keepalive_name_ch2--;
                        UART_CH2_Send("get_name\r\n");
                    }
                    else    //tengo el nombre o me canse de preguntar
                        UART_CH2_Send("keepalive\r\n");

                    keepalive_ch2 = KEEP_ALIVE_COUNTER;                    
                    AntennaNoConnectionCh2;                    
                }
                else 
                {
                    if (keepalive_ch2)
                    {
                        keepalive_ch2--;
                        UART_CH2_Send("keepalive\r\n");
                    }
                    else    //tenia parametros y los perdio
                    {
                        AntennaNewParamsCh2;
                        AntennaNoParamsCh2;
                        AntennaNoNameCh2;
                    }
                }
            }
            else
                UART_CH2_Send("get_params\r\n");

            if (AntennaCheckParamsCh3)
            {
                if (AntennaCheckConnectionCh3)    //tengo parametros y contesta
                {
                    if ((!AntennaCheckNameCh3) && (keepalive_name_ch3))
                    {
                        keepalive_name_ch3--;
                        UART_CH3_Send("get_name\r\n");
                    }
                    else    //tengo el nombre o me canse de preguntar
                        UART_CH3_Send("keepalive\r\n");

                    keepalive_ch3 = KEEP_ALIVE_COUNTER;                    
                    AntennaNoConnectionCh3;                    
                }
                else 
                {
                    if (keepalive_ch3)
                    {
                        keepalive_ch3--;
                        UART_CH3_Send("keepalive\r\n");
                    }
                    else    //tenia parametros y los perdio
                    {
                        AntennaNewParamsCh3;
                        AntennaNoParamsCh3;
                        AntennaNoNameCh3;
                    }
                }
            }
            else
                UART_CH3_Send("get_params\r\n");

            if (AntennaCheckParamsCh4)
            {
                if (AntennaCheckConnectionCh4)    //tengo parametros y contesta
                {
                    if ((!AntennaCheckNameCh4) && (keepalive_name_ch4))
                    {
                        keepalive_name_ch4--;
                        UART_CH4_Send("get_name\r\n");
                    }
                    else    //tengo el nombre o me canse de preguntar
                        UART_CH4_Send("keepalive\r\n");

                    keepalive_ch4 = KEEP_ALIVE_COUNTER;                    
                    AntennaNoConnectionCh4;                    
                }
                else 
                {
                    if (keepalive_ch4)
                    {
                        keepalive_ch4--;
                        UART_CH4_Send("keepalive\r\n");
                    }
                    else    //tenia parametros y los perdio
                    {
                        AntennaNewParamsCh4;
                        AntennaNoParamsCh4;
                        AntennaNameCh4;
                    }
                }
            }
            else
                UART_CH4_Send("get_params\r\n");
            
            antenna_timer = ANTENNA_KEEP_ALIVE_TIMER;
        }

        //si tengo todos los canales en lock voy a tratamiento
        //y aviso cuales antenas esta presentes
        if (AntennaCheckLockCh1 && AntennaCheckLockCh2 && AntennaCheckLockCh3 && AntennaCheckLockCh4)
        {
            if (AntennaGetConnection(CH1))
                AntennaInTreatmentCh1;

            if (AntennaGetConnection(CH2))
                AntennaInTreatmentCh2;
 
            if (AntennaGetConnection(CH3))
                AntennaInTreatmentCh3;

            if (AntennaGetConnection(CH4))
                AntennaInTreatmentCh4;
                        
            antenna_state++;
        }

#ifdef SOFTWARE_VERSION_1_3
        //si alguna tuvo update doy algo de tiempo y luego envio info a la PC
        if (AntennaCheckNewParamsCh1 || AntennaCheckNewParamsCh2 ||
            AntennaCheckNewParamsCh3 || AntennaCheckNewParamsCh4)
        {
            AntennaNoNewParamsCh1;
            AntennaNoNewParamsCh2;
            AntennaNoNewParamsCh3;
            AntennaNoNewParamsCh4;
            if (!antenna_info_timer)
                antenna_info_timer = ANTENNA_INFO_TIMER_UPDATE;
            
            antenna_info_sended = 0;
        }

        //si paso este timer envio la info de las antenas que conozco a la PC
        if (!antenna_info_timer)
        {
            if (!antenna_info_sended)
            {
                AntennaSendKnowParams();
                antenna_info_sended = 1;
            }
        }

        if (antenna_send_info_with_timer)
        {
            if (!antenna_info_timer)
                antenna_info_timer = ANTENNA_INFO_TIMER_UPDATE;
            
            antenna_info_sended = 0;
            antenna_send_info_with_timer = 0;
        }            
#endif
        break;

    case ANTENNA_IN_TREATMENT:
        if (!antenna_timer)
        {
            if (AntennaCheckInTreatmentCh1)    //si es una de las que esta en tratamiento
            {
                if (AntennaCheckParamsCh1)
                {
                    if (AntennaCheckConnectionCh1)    //tengo parametros y contesta
                    {
                        UART_CH1_Send("get_temp\r\n");
                        keepalive_ch1 = KEEP_ALIVE_COUNTER;                    
                        AntennaNoConnectionCh1;
                    }
                    else 
                    {
                        if (keepalive_ch1)
                        {
                            keepalive_ch1--;
                            UART_CH1_Send("get_temp\r\n");
                        }
                        else    //estaba conectada pero se perdio
                        {
                            AntennaNoParamsCh1;
                            antenna_send_info_with_timer = 1;
                        }
                    }
                }
            }

            if (AntennaCheckInTreatmentCh2)    //si es una de las que esta en tratamiento
            {                            
                if (AntennaCheckParamsCh2)
                {
                    if (AntennaCheckConnectionCh2)    //tengo parametros y contesta
                    {
                        UART_CH2_Send("get_temp\r\n");
                        keepalive_ch2 = KEEP_ALIVE_COUNTER;                    
                        AntennaNoConnectionCh2;
                    }
                    else 
                    {
                        if (keepalive_ch2)
                        {
                            keepalive_ch2--;
                            UART_CH2_Send("get_temp\r\n");
                        }
                        else    //estaba conectada pero se perdio
                        {
                            AntennaNoParamsCh2;
                            antenna_send_info_with_timer = 1;
                        }
                    }
                }
            }

            if (AntennaCheckInTreatmentCh3)    //si es una de las que esta en tratamiento
            {                                            
                if (AntennaCheckParamsCh3)
                {
                    if (AntennaCheckConnectionCh3)    //tengo parametros y contesta
                    {
                        UART_CH3_Send("get_temp\r\n");
                        keepalive_ch3 = KEEP_ALIVE_COUNTER;                    
                        AntennaNoConnectionCh3;
                    }
                    else 
                    {
                        if (keepalive_ch3)
                        {
                            keepalive_ch3--;
                            UART_CH3_Send("get_temp\r\n");
                        }
                        else    //estaba conectada pero se perdio
                        {                            
                            AntennaNoParamsCh3;
                            antenna_send_info_with_timer = 1;
                        }
                    }
                }
            }

            if (AntennaCheckInTreatmentCh4)    //si es una de las que esta en tratamiento
            {                                                        
                if (AntennaCheckParamsCh4)
                {
                    if (AntennaCheckConnectionCh4)    //tengo parametros y contesta
                    {
                        UART_CH4_Send("get_temp\r\n");
                        keepalive_ch4 = KEEP_ALIVE_COUNTER;                    
                        AntennaNoConnectionCh4;
                    }
                    else 
                    {
                        if (keepalive_ch4)
                        {
                            keepalive_ch4--;
                            UART_CH4_Send("get_temp\r\n");
                        }
                        else    //estaba conectada pero se perdio
                        {
                            AntennaNoParamsCh4;
                            antenna_send_info_with_timer = 1;
                        }
                    }
                }
            }

            antenna_timer = ANTENNA_TEMP_TIMER;
        }

        //si tengo todos los canales en unlock salgo del tratamiento
        if ((!AntennaCheckLockCh1) && (!AntennaCheckLockCh2) && (!AntennaCheckLockCh3) && (!AntennaCheckLockCh4))
        {
            AntennaNoInTreatmentCh1;
            AntennaNoInTreatmentCh2;
            AntennaNoInTreatmentCh3;
            AntennaNoInTreatmentCh4;
            
            antenna_state = ANTENNA_IN_STANDBY;
        }

#ifdef SOFTWARE_VERSION_1_3
        //si alguna tuvo update, se desenchufo y enchufo
        if (AntennaCheckNewParamsCh1 && AntennaCheckInTreatmentCh1)
        {
            AntennaNoNewParamsCh1;
            AntennaNoInTreatmentCh1;
            AntennaNoParamsCh1;
            antenna_send_info_with_timer = 1;
        }

        if (AntennaCheckNewParamsCh2 && AntennaCheckInTreatmentCh2)
        {
            AntennaNoNewParamsCh2;
            AntennaNoInTreatmentCh2;
            AntennaNoParamsCh2;
            antenna_send_info_with_timer = 1;
        }

        if (AntennaCheckNewParamsCh3 && AntennaCheckInTreatmentCh3)
        {
            AntennaNoNewParamsCh3;
            AntennaNoInTreatmentCh3;
            AntennaNoParamsCh3;
            antenna_send_info_with_timer = 1;
        }

        if (AntennaCheckNewParamsCh4 && AntennaCheckInTreatmentCh4)
        {
            AntennaNoNewParamsCh4;
            AntennaNoInTreatmentCh4;
            AntennaNoParamsCh4;
            antenna_send_info_with_timer = 1;
        }

        //si estoy en tratamiento puedo enviar info sin esperar
        if (antenna_send_info_with_timer)
        {
            AntennaSendKnowParams();            
            antenna_send_info_with_timer = 0;
        }
#endif        
        break;

    case ANTENNA_IN_PAUSE:
        break;

    default:
        antenna_state = ANTENNA_INIT;
        break;
    }
}

//la llaman desde el comms para setear los valores actuales de temperatura en la antenas
void AntennaSetCurrentTemp (unsigned char ch, unsigned char t_int, unsigned char t_dec)
{
    switch (ch)
    {
    case CH1:
        ch1_ant_current_temp_int = t_int;
        ch1_ant_current_temp_dec = t_dec;
        AntennaConnectionCh1;        
        break;

    case CH2:
        ch2_ant_current_temp_int = t_int;
        ch2_ant_current_temp_dec = t_dec;
        AntennaConnectionCh2;                
        break;

    case CH3:
        ch3_ant_current_temp_int = t_int;
        ch3_ant_current_temp_dec = t_dec;
        AntennaConnectionCh3;                
        break;

    case CH4:
        ch4_ant_current_temp_int = t_int;
        ch4_ant_current_temp_dec = t_dec;
        AntennaConnectionCh4;                
        break;
    }    
}

//la llaman desde el comms para respuestas del keepalive de antenna
void AntennaIsAnswering (unsigned char ch)
{
    if (ch == CH1)
        AntennaConnectionCh1;

    if (ch == CH2)
        AntennaConnectionCh2;

    if (ch == CH3)
        AntennaConnectionCh3;

    if (ch == CH4)
        AntennaConnectionCh4;
    
}

//info que mandan del modulo comms cuando se conecta una antenna o responde a get_params
void AntennaSetParamsStruct (unsigned char ch, antenna_typedef *ant)
{
    switch (ch)
    {
    case CH1:
        session_ch_1.ant_resistance_int = ant->resistance_int;
        session_ch_1.ant_resistance_dec = ant->resistance_dec;
        session_ch_1.ant_inductance_int = ant->inductance_int;
        session_ch_1.ant_inductance_dec = ant->inductance_dec;
        session_ch_1.ant_current_limit_int = ant->current_limit_int;
        session_ch_1.ant_current_limit_dec = ant->current_limit_dec;
        session_ch_1.ant_temp_max_int = ant->temp_max_int;
        session_ch_1.ant_temp_max_dec = ant->temp_max_dec;
        break;

    case CH2:
        session_ch_2.ant_resistance_int = ant->resistance_int;
        session_ch_2.ant_resistance_dec = ant->resistance_dec;
        session_ch_2.ant_inductance_int = ant->inductance_int;
        session_ch_2.ant_inductance_dec = ant->inductance_dec;
        session_ch_2.ant_current_limit_int = ant->current_limit_int;
        session_ch_2.ant_current_limit_dec = ant->current_limit_dec;
        session_ch_2.ant_temp_max_int = ant->temp_max_int;
        session_ch_2.ant_temp_max_dec = ant->temp_max_dec;
        break;

    case CH3:
        session_ch_3.ant_resistance_int = ant->resistance_int;
        session_ch_3.ant_resistance_dec = ant->resistance_dec;
        session_ch_3.ant_inductance_int = ant->inductance_int;
        session_ch_3.ant_inductance_dec = ant->inductance_dec;
        session_ch_3.ant_current_limit_int = ant->current_limit_int;
        session_ch_3.ant_current_limit_dec = ant->current_limit_dec;
        session_ch_3.ant_temp_max_int = ant->temp_max_int;
        session_ch_3.ant_temp_max_dec = ant->temp_max_dec;
        break;

    case CH4:
        session_ch_4.ant_resistance_int = ant->resistance_int;
        session_ch_4.ant_resistance_dec = ant->resistance_dec;
        session_ch_4.ant_inductance_int = ant->inductance_int;
        session_ch_4.ant_inductance_dec = ant->inductance_dec;
        session_ch_4.ant_current_limit_int = ant->current_limit_int;
        session_ch_4.ant_current_limit_dec = ant->current_limit_dec;
        session_ch_4.ant_temp_max_int = ant->temp_max_int;
        session_ch_4.ant_temp_max_dec = ant->temp_max_dec;
        break;
    }

    //reviso un poco los parametros
    if (((ant->resistance_int) || (ant->resistance_dec)) &&
        ((ant->inductance_int) || (ant->inductance_dec)) &&
        ((ant->current_limit_int) || (ant->current_limit_dec)) &&
        ((ant->temp_max_int) || (ant->temp_max_dec)))
    {
        if (ch == CH1)
        {
            AntennaParamsCh1;
            AntennaNewParamsCh1;
            AntennaConnectionCh1;
            keepalive_name_ch1 = KEEP_ALIVE_ANTENNA_NAME_COUNTER;
        }
        if (ch == CH2)
        {
            AntennaParamsCh2;
            AntennaNewParamsCh2;
            AntennaConnectionCh2;
            keepalive_name_ch2 = KEEP_ALIVE_ANTENNA_NAME_COUNTER;            
        }
        if (ch == CH3)
        {
            AntennaParamsCh3;
            AntennaNewParamsCh3;
            AntennaConnectionCh3;
            keepalive_name_ch3 = KEEP_ALIVE_ANTENNA_NAME_COUNTER;            
        }
        if (ch == CH4)
        {
            AntennaParamsCh4;
            AntennaNewParamsCh4;
            AntennaConnectionCh4;
            keepalive_name_ch4 = KEEP_ALIVE_ANTENNA_NAME_COUNTER;            
        }
    }    
}

//me llaman desde comms y GTK_Signal para conocer los parametros de antena
void AntennaGetParamsStruct (unsigned char ch, antenna_typedef *ant)
{
    switch (ch)
    {
    case CH1:
        ant->resistance_int = session_ch_1.ant_resistance_int;
        ant->resistance_dec = session_ch_1.ant_resistance_dec;
        ant->inductance_int = session_ch_1.ant_inductance_int;
        ant->inductance_dec = session_ch_1.ant_inductance_dec;
        ant->current_limit_int = session_ch_1.ant_current_limit_int;
        ant->current_limit_dec = session_ch_1.ant_current_limit_dec;
        ant->temp_max_int = session_ch_1.ant_temp_max_int;
        ant->temp_max_dec = session_ch_1.ant_temp_max_dec;        
        break;

    case CH2:
        ant->resistance_int = session_ch_2.ant_resistance_int;
        ant->resistance_dec = session_ch_2.ant_resistance_dec;
        ant->inductance_int = session_ch_2.ant_inductance_int;
        ant->inductance_dec = session_ch_2.ant_inductance_dec;
        ant->current_limit_int = session_ch_2.ant_current_limit_int;
        ant->current_limit_dec = session_ch_2.ant_current_limit_dec;
        ant->temp_max_int = session_ch_2.ant_temp_max_int;
        ant->temp_max_dec = session_ch_2.ant_temp_max_dec;                
        break;

    case CH3:
        ant->resistance_int = session_ch_3.ant_resistance_int;
        ant->resistance_dec = session_ch_3.ant_resistance_dec;
        ant->inductance_int = session_ch_3.ant_inductance_int;
        ant->inductance_dec = session_ch_3.ant_inductance_dec;
        ant->current_limit_int = session_ch_3.ant_current_limit_int;
        ant->current_limit_dec = session_ch_3.ant_current_limit_dec;
        ant->temp_max_int = session_ch_3.ant_temp_max_int;
        ant->temp_max_dec = session_ch_3.ant_temp_max_dec;                
        break;

    case CH4:
        ant->resistance_int = session_ch_4.ant_resistance_int;
        ant->resistance_dec = session_ch_4.ant_resistance_dec;
        ant->inductance_int = session_ch_4.ant_inductance_int;
        ant->inductance_dec = session_ch_4.ant_inductance_dec;
        ant->current_limit_int = session_ch_4.ant_current_limit_int;
        ant->current_limit_dec = session_ch_4.ant_current_limit_dec;
        ant->temp_max_int = session_ch_4.ant_temp_max_int;
        ant->temp_max_dec = session_ch_4.ant_temp_max_dec;                
        break;
    }        
}

//contesta a la sesion si la antena esta presente/contestando y tiene parametros
unsigned char AntennaGetConnection (unsigned char ch)
{
    unsigned char conn = 0;

    if (ch == CH1)
        if (AntennaCheckParamsCh1 && keepalive_ch1)
            conn = 1;

    if (ch == CH2)
        if (AntennaCheckParamsCh2 && keepalive_ch2)
            conn = 1;

    if (ch == CH3)
        if (AntennaCheckParamsCh3 && keepalive_ch3)
            conn = 1;

    if (ch == CH4)
        if (AntennaCheckParamsCh4 && keepalive_ch4)
            conn = 1;

    return conn;
    
}

//contesta a la sesion la ultima temperatura conocida de la antena
unsigned char AntennaGetCurrentTemp (unsigned char ch)
{
    unsigned char temp = 0;

    if (ch == CH1)
        temp = ch1_ant_current_temp_int;

    if (ch == CH2)
        temp = ch2_ant_current_temp_int;

    if (ch == CH3)
        temp = ch3_ant_current_temp_int;

    if (ch == CH4)
        temp = ch4_ant_current_temp_int;

    return temp;
    
}

//llaman desde la sesion para saber si tiene antena presente y avisar del comienzo
//de un nuevo tratamiento
unsigned char AntennaVerifyForTreatment (unsigned char ch)
{
    unsigned char resp = FIN_ERROR;

    switch (ch)
    {
    case CH1:
        if (AntennaCheckParamsCh1)
        {
            keepalive_ch1 = KEEP_ALIVE_COUNTER;
            resp = FIN_OK;
        }

        AntennaLockCh1;
        break;

    case CH2:
        if (AntennaCheckParamsCh2)
        {
            keepalive_ch2 = KEEP_ALIVE_COUNTER;            
            resp = FIN_OK;
        }

        AntennaLockCh2;
        break;

    case CH3:
        if (AntennaCheckParamsCh3)
        {
            keepalive_ch3 = KEEP_ALIVE_COUNTER;            
            resp = FIN_OK;
        }

        AntennaLockCh3;
        break;

    case CH4:
        if (AntennaCheckParamsCh4)
        {
            keepalive_ch4 = KEEP_ALIVE_COUNTER;
            resp = FIN_OK;
        }

        AntennaLockCh4;
        break;

    default:
        break;
    }

    return resp;
}

void AntennaEndTreatment (unsigned char ch)
{
    if (ch == CH1)
        AntennaUnlockCh1;

    if (ch == CH2)
        AntennaUnlockCh2;

    if (ch == CH3)
        AntennaUnlockCh3;

    if (ch == CH4)
        AntennaUnlockCh4;
}

void AntennaBackupParams (antenna_typedef *plock_in, session_typedef *psession)    
{
    plock_in->resistance_int = psession->ant_resistance_int;
    plock_in->resistance_dec = psession->ant_resistance_dec;
    plock_in->inductance_int = psession->ant_inductance_int;
    plock_in->inductance_dec = psession->ant_inductance_dec;
    plock_in->current_limit_int = psession->ant_current_limit_int;
    plock_in->current_limit_dec = psession->ant_current_limit_dec;
    plock_in->temp_max_int = psession->ant_temp_max_int;
    plock_in->temp_max_dec = psession->ant_temp_max_dec;    
}

void AntennaFlushParamsInLock (antenna_typedef * plock_in)
{
    plock_in->resistance_int = 0;
    plock_in->resistance_dec = 0;
    plock_in->inductance_int = 0;
    plock_in->inductance_dec = 0;
    plock_in->current_limit_int = 0;
    plock_in->current_limit_dec = 0;
    plock_in->temp_max_int = 0;
    plock_in->temp_max_dec = 0;    
}

void AntennaFlushParams (unsigned char ch)
{
    switch (ch)
    {
    case CH1:
        session_ch_1.ant_resistance_int = 0;
        session_ch_1.ant_resistance_dec = 0;
        session_ch_1.ant_inductance_int = 0;
        session_ch_1.ant_inductance_dec = 0;
        session_ch_1.ant_current_limit_int = 0;
        session_ch_1.ant_current_limit_dec = 0;
        session_ch_1.ant_temp_max_int = 0;
        session_ch_1.ant_temp_max_dec = 0;
        break;

    case CH2:
        session_ch_2.ant_resistance_int = 0;
        session_ch_2.ant_resistance_dec = 0;
        session_ch_2.ant_inductance_int = 0;
        session_ch_2.ant_inductance_dec = 0;
        session_ch_2.ant_current_limit_int = 0;
        session_ch_2.ant_current_limit_dec = 0;
        session_ch_2.ant_temp_max_int = 0;
        session_ch_2.ant_temp_max_dec = 0;
        break;

    case CH3:
        session_ch_3.ant_resistance_int = 0;
        session_ch_3.ant_resistance_dec = 0;
        session_ch_3.ant_inductance_int = 0;
        session_ch_3.ant_inductance_dec = 0;
        session_ch_3.ant_current_limit_int = 0;
        session_ch_3.ant_current_limit_dec = 0;
        session_ch_3.ant_temp_max_int = 0;
        session_ch_3.ant_temp_max_dec = 0;
        break;

    case CH4:
        session_ch_4.ant_resistance_int = 0;
        session_ch_4.ant_resistance_dec = 0;
        session_ch_4.ant_inductance_int = 0;
        session_ch_4.ant_inductance_dec = 0;
        session_ch_4.ant_current_limit_int = 0;
        session_ch_4.ant_current_limit_dec = 0;
        session_ch_4.ant_temp_max_int = 0;
        session_ch_4.ant_temp_max_dec = 0;
        break;
    }        
}

void AntennaSendKnowParams (void)
{
    unsigned char some_sended = 0;
    unsigned char antbuff [SIZEOF_RXDATA];

    if (AntennaGetConnection(CH1))
    {
        if (AntennaCheckNameCh1)
        {
            sprintf((char *)antbuff, "%s,%03d.%02d,%03d.%02d,%03d.%02d,%03d.%02d,1\r\n",
                    session_ch_1.connected_ant_name,
                    session_ch_1.ant_resistance_int,
                    session_ch_1.ant_resistance_dec,
                    session_ch_1.ant_inductance_int,
                    session_ch_1.ant_inductance_dec,
                    session_ch_1.ant_current_limit_int,
                    session_ch_1.ant_current_limit_dec,
                    session_ch_1.ant_temp_max_int,
                    session_ch_1.ant_temp_max_dec);            
        }
        else
        {
            sprintf((char *)antbuff, "ch1,%03d.%02d,%03d.%02d,%03d.%02d,%03d.%02d,1\r\n",
                    session_ch_1.ant_resistance_int,
                    session_ch_1.ant_resistance_dec,
                    session_ch_1.ant_inductance_int,
                    session_ch_1.ant_inductance_dec,
                    session_ch_1.ant_current_limit_int,
                    session_ch_1.ant_current_limit_dec,
                    session_ch_1.ant_temp_max_int,
                    session_ch_1.ant_temp_max_dec);
        }

        some_sended = 1;
        UART_PC_Send((char *) antbuff);
    }

    if (AntennaGetConnection(CH2))
    {
        if (AntennaCheckNameCh2)
        {
            sprintf((char *)antbuff, "%s,%03d.%02d,%03d.%02d,%03d.%02d,%03d.%02d,2\r\n",
                    session_ch_2.connected_ant_name,
                    session_ch_2.ant_resistance_int,
                    session_ch_2.ant_resistance_dec,
                    session_ch_2.ant_inductance_int,
                    session_ch_2.ant_inductance_dec,
                    session_ch_2.ant_current_limit_int,
                    session_ch_2.ant_current_limit_dec,
                    session_ch_2.ant_temp_max_int,
                    session_ch_2.ant_temp_max_dec);
            
        }
        else
        {
            sprintf((char *)antbuff, "ch2,%03d.%02d,%03d.%02d,%03d.%02d,%03d.%02d,2\r\n",
                    session_ch_2.ant_resistance_int,
                    session_ch_2.ant_resistance_dec,
                    session_ch_2.ant_inductance_int,
                    session_ch_2.ant_inductance_dec,
                    session_ch_2.ant_current_limit_int,
                    session_ch_2.ant_current_limit_dec,
                    session_ch_2.ant_temp_max_int,
                    session_ch_2.ant_temp_max_dec);
        }

        some_sended = 1;
        UART_PC_Send((char *) antbuff);
    }

    if (AntennaGetConnection(CH3))
    {
        if (AntennaCheckNameCh3)            
        {
            sprintf((char *)antbuff, "%s,%03d.%02d,%03d.%02d,%03d.%02d,%03d.%02d,3\r\n",
                    session_ch_3.connected_ant_name,
                    session_ch_3.ant_resistance_int,
                    session_ch_3.ant_resistance_dec,
                    session_ch_3.ant_inductance_int,
                    session_ch_3.ant_inductance_dec,
                    session_ch_3.ant_current_limit_int,
                    session_ch_3.ant_current_limit_dec,
                    session_ch_3.ant_temp_max_int,
                    session_ch_3.ant_temp_max_dec);            
        }
        else
        {
            sprintf((char *)antbuff, "ch3,%03d.%02d,%03d.%02d,%03d.%02d,%03d.%02d,3\r\n",
                    session_ch_3.ant_resistance_int,
                    session_ch_3.ant_resistance_dec,
                    session_ch_3.ant_inductance_int,
                    session_ch_3.ant_inductance_dec,
                    session_ch_3.ant_current_limit_int,
                    session_ch_3.ant_current_limit_dec,
                    session_ch_3.ant_temp_max_int,
                    session_ch_3.ant_temp_max_dec);
        }

        some_sended = 1;
        UART_PC_Send((char *) antbuff);
    }

    if (AntennaGetConnection(CH4))
    {
        if (AntennaCheckNameCh4)
        {
            sprintf((char *)antbuff, "%s,%03d.%02d,%03d.%02d,%03d.%02d,%03d.%02d,4\r\n",
                    session_ch_4.connected_ant_name,
                    session_ch_4.ant_resistance_int,
                    session_ch_4.ant_resistance_dec,
                    session_ch_4.ant_inductance_int,
                    session_ch_4.ant_inductance_dec,
                    session_ch_4.ant_current_limit_int,
                    session_ch_4.ant_current_limit_dec,
                    session_ch_4.ant_temp_max_int,
                    session_ch_4.ant_temp_max_dec);            
        }
        else
        {
            sprintf((char *)antbuff, "ch4,%03d.%02d,%03d.%02d,%03d.%02d,%03d.%02d,4\r\n",
                    session_ch_4.ant_resistance_int,
                    session_ch_4.ant_resistance_dec,
                    session_ch_4.ant_inductance_int,
                    session_ch_4.ant_inductance_dec,
                    session_ch_4.ant_current_limit_int,
                    session_ch_4.ant_current_limit_dec,
                    session_ch_4.ant_temp_max_int,
                    session_ch_4.ant_temp_max_dec);
        }

        some_sended = 1;
        UART_PC_Send((char *) antbuff);
    }

    if (!some_sended)
        UART_PC_Send("antenna none\r\n");

}

//me llaman desde comms para conocer las antenas conectadas
void AntennaSendKnowInfoWithTimer (void)
{
    antenna_send_info_with_timer = 1;
}

//me llaman desde comms para setear el nombre de la antena
void AntennaSetName (unsigned char ch, char * pname)
{
    unsigned char len;
    
    len = strlen(pname);
    //si tiene \r final lo quito y ajusto
    if (*(pname + len - 1) == '\r')
    {
        *(pname + len - 1) = '\0';
        len--;
    }

    if (len < (SIZEOF_ANTENNA_NAME - 1))
    {
        if (ch == CH1)
        {
            AntennaNameCh1;            
            strcpy(session_ch_1.connected_ant_name, pname);
            AntennaConnectionCh1;
        }

        if (ch == CH2)
        {
            AntennaNameCh2;            
            strcpy(session_ch_2.connected_ant_name, pname);
            AntennaConnectionCh2;
        }

        if (ch == CH3)
        {
            AntennaNameCh3;            
            strcpy(session_ch_3.connected_ant_name, pname);
            AntennaConnectionCh3;
        }

        if (ch == CH4)
        {
            AntennaNameCh4;
            strcpy(session_ch_4.connected_ant_name, pname);
            AntennaConnectionCh4;
        }
    }    
}
//---- end of file ----//
