//---------------------------------------------------------
// #### PROYECTO MAGNETO GAUSSTEK - MotherBoard ###########
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F103
// ##
// #### FIRST_PULSE.C #####################################
//---------------------------------------------------------

// Includes --------------------------------------------------------------------
#include "first_pulse.h"
#include "channels_defs.h"
#include "answers_defs.h"
#include "timer.h"
// #include "usart.h"
// #include "stm32f10x.h"
// #include "adc.h"
// #include "GTK_Estructura.h"
// #include "GTK_Signal.h"
// #include "flash_program.h"
// #include "errors.h"
#include "antennas.h"

// #include <float.h>
// #include <math.h>
#include <stdio.h>
// #include <string.h>


// Module Private Types Constants and Macros -----------------------------------
#define PSU_40    48

typedef enum {
    FP_INIT = 0,
    FP_GET_BASE_CURRENT,
    FP_ANTENNA_PARAMS,
    FP_5_TAU_PULSE_CHECK_CURRENT,
    FP_TAU_ENDING

} fp_state_e;


// Externals -------------------------------------------------------------------
extern unsigned short session_warming_up_channel_1_time;
extern unsigned short session_warming_up_channel_2_time;
extern unsigned short session_warming_up_channel_3_time;
extern unsigned short session_warming_up_channel_4_time;

extern unsigned short session_warming_up_channel_1_time_2;
extern unsigned short session_warming_up_channel_2_time_2;
extern unsigned short session_warming_up_channel_3_time_2;
extern unsigned short session_warming_up_channel_4_time_2;

// for current meas
extern unsigned short actual_current[];
extern unsigned char new_current_sample_ch1;
extern unsigned char new_current_sample_ch2;
extern unsigned char new_current_sample_ch3;
extern unsigned char new_current_sample_ch4;



// Globals ---------------------------------------------------------------------
fp_state_e fp_state_ch1 = FP_INIT;
fp_state_e fp_state_ch2 = FP_INIT;
fp_state_e fp_state_ch3 = FP_INIT;
fp_state_e fp_state_ch4 = FP_INIT;

// for current meas
unsigned short current_threshold [5];    //dummy on pos 0
// for detect the current base in channels
unsigned short current_samples_ch1 [8];
unsigned short current_samples_ch2 [8];
unsigned short current_samples_ch3 [8];
unsigned short current_samples_ch4 [8];
unsigned char current_samples_cnt_ch1 = 0;
unsigned char current_samples_cnt_ch2 = 0;
unsigned char current_samples_cnt_ch3 = 0;
unsigned char current_samples_cnt_ch4 = 0;
unsigned short current_base_ch1 = 0;
unsigned short current_base_ch2 = 0;
unsigned short current_base_ch3 = 0;
unsigned short current_base_ch4 = 0;



// Module Private Functions ----------------------------------------------------
void FP_Reset_PWM_CH (unsigned char ch);
void FP_Apply_PWM_CH (unsigned char ch, unsigned short pwm);


// Module Functions ------------------------------------------------------------
unsigned char FirstPulseCheck (unsigned char ch)
{
    unsigned char answer = TRABAJANDO;
    fp_state_e * p_state;
    unsigned short * p_pulse_time;
    unsigned short * p_pulse_time_end;
    unsigned short * p_current;
    unsigned short * p_current_threshold;
    unsigned char * p_current_flag;
    unsigned short * p_current_samples;
    unsigned char * p_current_samples_cnt;
    unsigned short * p_current_base;


    //Antenna parameters.
    antenna_typedef ant_params;
    float resistance = 0.0;
    float inductance = 0.0;
    float current = 0.0;    
    
    float LR_tau = 0.0;
    float voltage = 0.0;
    float c_threshold = 0.0;    

    // the pwm to apply
    unsigned short applied_pwm = 0;
    
    
    // get states for each channel
    switch (ch)
    {
    case CH1:
        p_state = &fp_state_ch1;
        p_pulse_time = &session_warming_up_channel_1_time;
        p_pulse_time_end = &session_warming_up_channel_1_time_2;
        p_current = &actual_current[CH1];
        p_current_threshold = &current_threshold[CH1];
        p_current_flag = &new_current_sample_ch1;
        p_current_samples = current_samples_ch1;
        p_current_samples_cnt = &current_samples_cnt_ch1;
        p_current_base = &current_base_ch1;
        break;

    case CH2:
        p_state = &fp_state_ch2;
        p_pulse_time = &session_warming_up_channel_2_time;
        p_pulse_time_end = &session_warming_up_channel_2_time_2;
        p_current = &actual_current[CH2];
        p_current_threshold = &current_threshold[CH2];
        p_current_flag = &new_current_sample_ch2;
        p_current_samples = current_samples_ch2;
        p_current_samples_cnt = &current_samples_cnt_ch2;
        p_current_base = &current_base_ch2;        
        break;

    case CH3:
        p_state = &fp_state_ch3;
        p_pulse_time = &session_warming_up_channel_3_time;
        p_pulse_time_end = &session_warming_up_channel_3_time_2;
        p_current = &actual_current[CH3];
        p_current_threshold = &current_threshold[CH3];
        p_current_flag = &new_current_sample_ch3;
        p_current_samples = current_samples_ch3;
        p_current_samples_cnt = &current_samples_cnt_ch3;
        p_current_base = &current_base_ch3;        
        break;

    case CH4:
        p_state = &fp_state_ch4;
        p_pulse_time = &session_warming_up_channel_4_time;
        p_pulse_time_end = &session_warming_up_channel_4_time_2;
        p_current = &actual_current[CH4];
        p_current_threshold = &current_threshold[CH4];        
        p_current_flag = &new_current_sample_ch4;
        p_current_samples = current_samples_ch4;
        p_current_samples_cnt = &current_samples_cnt_ch4;
        p_current_base = &current_base_ch4;        
        break;
    }

    switch (*p_state)
    {
    case FP_INIT:
        FP_Reset_PWM_CH (ch);

        for (unsigned char i = 0; i < 8; i++)
            *(p_current_samples + i) = 0;

        *p_current_samples_cnt = 0;
        
        *p_state = FP_GET_BASE_CURRENT;
        break;

    case FP_GET_BASE_CURRENT:
        if (*p_current_samples_cnt < 8)
        {
            //new current sample arrived
            if (*p_current_flag)
            {
                *(p_current_samples + *p_current_samples_cnt) = *p_current;
                *p_current_samples_cnt += 1;
                *p_current_flag = 0;
            }
        }
        else
        {
            unsigned short current_base = 0;
            for (unsigned char i = 0; i < 8; i++)
                current_base += *(p_current_samples + i);

            current_base >>= 3;
            *p_current_base = current_base;
            *p_state = FP_ANTENNA_PARAMS;
        }
        break;

    case FP_ANTENNA_PARAMS:
        AntennaGetParamsStruct(ch, &ant_params);

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
	current = (float) ant_params.current_limit_dec;
	current /= 100;
	current += (float) ant_params.current_limit_int;

        // printf("antena params R: %f ohms L: %f Hy I: %f A\n",
        //        resistance,
        //        inductance,
        //        current
        //     );

        LR_tau = inductance / resistance;
        LR_tau = LR_tau * 5 * 10000;    //5 tau en centenas de us

        voltage = resistance * current;
        voltage = voltage * 1000 / PSU_40;
        applied_pwm = (unsigned short) voltage;

        FP_Apply_PWM_CH(ch, applied_pwm);

        *p_pulse_time_end = (unsigned short) LR_tau;
        *p_pulse_time = 0;

        c_threshold = current * 0.6;    //600mV / A
        c_threshold = c_threshold * 0.303;    //divided by 3.3V
        c_threshold = c_threshold * 4095;    //convert to adc samples
        c_threshold = c_threshold / 2.0;    //50% of current for threshold
        
        *p_current_threshold = (unsigned short) c_threshold;
        *p_current_threshold = *p_current_threshold + *p_current_base;

        // printf("5 Tau step: %d (100us / step) voltage: %f V pwm: %d I: %f thres: %d\n",
        //        *p_pulse_time_end,
        //        (resistance * current),
        //        applied_pwm,
        //        current,
        //        *p_current_threshold
        //     );

        *p_state = FP_5_TAU_PULSE_CHECK_CURRENT;
        break;

    case FP_5_TAU_PULSE_CHECK_CURRENT:
        if (*p_pulse_time < *p_pulse_time_end)
        {
            //new current sample arrived
            if (*p_current_flag)
            {
                *p_current_flag = 0;

                // current threshold getted, reset PWM and wait for 1 Tau
                if (*p_current > *p_current_threshold)
                {
                    FP_Reset_PWM_CH (ch);
                    *p_pulse_time_end = *p_pulse_time_end / 5;
                    *p_pulse_time = 0;
                    *p_state = FP_TAU_ENDING;
                }
            }            
        }
        else
        {           
            // all 5 tau time pass and no current
            FP_Reset_PWM_CH (ch);
            *p_state = FP_INIT;            
            answer = FIN_ERROR;
        }
        break;

    case FP_TAU_ENDING:
        if (*p_pulse_time > *p_pulse_time_end)
        {
            *p_state = FP_INIT;
            answer = FIN_OK;
        }
        break;
    }

    return answer;
}


void FP_Reset_PWM_CH (unsigned char ch)
{
    switch (ch)
    {
    case CH1:
        PWM_CH1_TiempoSubida(0);
        PWM_CH1_TiempoMantenimiento(0);
        PWM_CH1_TiempoBajada(0);
        break;

    case CH2:
        PWM_CH2_TiempoSubida(0);
        PWM_CH2_TiempoMantenimiento(0);
        PWM_CH2_TiempoBajada(0);
        break;

    case CH3:
        PWM_CH3_TiempoSubida(0);
        PWM_CH3_TiempoMantenimiento(0);
        PWM_CH3_TiempoBajada(0);
        break;

    case CH4:
        PWM_CH4_TiempoSubida(0);
        PWM_CH4_TiempoMantenimiento(0);
        PWM_CH4_TiempoBajada(0);
        break;
    }
}


void FP_Apply_PWM_CH (unsigned char ch, unsigned short pwm)
{
    switch (ch)
    {
    case CH1:
        PWM_CH1_TiempoMantenimiento(pwm);
        break;

    case CH2:
        PWM_CH2_TiempoMantenimiento(pwm);
        break;

    case CH3:
        PWM_CH3_TiempoMantenimiento(pwm);
        break;

    case CH4:
        PWM_CH4_TiempoMantenimiento(pwm);
        break;
    }
}


//--- end of file ---//
