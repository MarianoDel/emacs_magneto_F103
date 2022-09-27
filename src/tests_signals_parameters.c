//---------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    TEST PLATFORM FOR FIRMWARE
// ##
// #### TESTS_SIGNALS_PARAMETERS.C ############
//---------------------------------------------

// Includes Modules for tests --------------------------------------------------
#include "GTK_Signal.h"
#include "answers_defs.h"

// Include Auxiliary modules ---------------------------------------------------
#include "tests_ok.h"

#include <stdio.h>
#include <string.h>
#include <math.h>


// Module Private Types Constants and Macros -----------------------------------
typedef enum {
    FREQ_7_83_HZ,
    FREQ_11_79_HZ,
    FREQ_16_67_HZ,
    FREQ_23_58_HZ,
    FREQ_30_80_HZ,
    FREQ_62_64_HZ
    
} freq_value_e;


typedef enum {
    SQUARE_SIGNAL,
    TRIANGULAR_SIGNAL,
    SINUSOIDAL_SIGNAL
    
} type_of_signal_e;


// Externals -------------------------------------------------------------------
extern warmingup_coolingdown_typedef table_warming_up_channel_1[];    //acceder a la tabla wup en ch1


// Globals ---------------------------------------------------------------------
char buffSendErr[64];

unsigned char channel_1_pause;
unsigned char channel_2_pause;
unsigned char channel_3_pause;
unsigned char channel_4_pause;

session_typedef session_ch_1;
session_typedef session_ch_2;
session_typedef session_ch_3;
session_typedef session_ch_4;

//la uso  para comprobar el funcionamiento de TIM5 desde el main
volatile unsigned int session_warming_up_channel_1_stage_time;

//--- Current limit Externals ---//
volatile unsigned char take_current_samples;


// Module Private Functions ----------------------------------------------------
void TSP_Get_Signal_Times_From_Screen (type_of_signal_e signal, freq_value_e freq, session_typedef * psession);
void TSP_Get_Signal_Times_From_Screen_Square (freq_value_e freq, session_typedef * psession);
void TSP_Get_Signal_Times_From_Screen_Triangular (freq_value_e freq, session_typedef * psession);
void TSP_Get_Signal_Times_From_Screen_Sinusoidal (freq_value_e freq, session_typedef * psession);
void TSP_Show_Table_Parameters (warmingup_coolingdown_typedef * ptable);


// Module Tester Functions -----------------------------------------------------
void Tests_Parameters_Sequence (void);
void Tests_Show_Parameters (void);
void Tests_Always_Generate_Parameters_Calculate (void);

void Tests_Local_Calc_Parameters (void);
void CalcParamsForSignal (int show_parcial_results);

void Tests_Compare_Parameters (void);

int Compare_Two_Params_With_Tolerance (unsigned short param1,
                                       unsigned short param2,
                                       unsigned short tolerance);

// Module Mocked Functions -----------------------------------------------------
void Update_TIM1_CH1 (unsigned short a);
void Update_TIM1_CH4 (unsigned short a);

void Update_TIM2_CH1 (unsigned short a);
void Update_TIM2_CH2 (unsigned short a);

void Update_TIM3_CH1 (unsigned short a);
void Update_TIM3_CH2 (unsigned short a);
void Update_TIM3_CH3 (unsigned short a);
void Update_TIM3_CH4 (unsigned short a);

void Update_TIM4_CH1 (unsigned short a);
void Update_TIM4_CH2 (unsigned short a);
void Update_TIM4_CH3 (unsigned short a);
void Update_TIM4_CH4 (unsigned short a);

void AntennaEndTreatment (unsigned char a);
void Usart1Send (char * send);
void Wait_ms (unsigned short a);
void BuzzerCommands (unsigned char a, unsigned char b);

void Error_SetString (char * p_errstr, unsigned short err_num);
unsigned char ConvertSingleChannelFinishFlag (void);
void ConvertChannel (unsigned char a);

unsigned char AntennaVerifyForTreatment (unsigned char a);
unsigned char AntennaGetCurrentTemp (unsigned char a);
unsigned char AntennaGetConnection (unsigned char a);
void AntennaGetParamsStruct (unsigned char ch, antenna_typedef *ant);

// Module Functions ------------------------------------------------------------
int main (int argc, char *argv[])
{
    // Tests_Parameters_Sequence ();
    // Tests_Show_Parameters ();
    Tests_Always_Generate_Parameters_Calculate ();
    // Tests_Local_Calc_Parameters ();
    // Tests_Compare_Parameters ();

    return 0;
}


// Tester Functions ------------------------------------------------------------
void Tests_Parameters_Sequence (void)
{
    printf("Testing Parameters Calculations...\n");

    printf("Parameters Calculation sequence: ");

    unsigned char i = 0;
    unsigned char channel = 1;
    // answers with FIN_OK or FIN_ERROR
    i = Session_Channels_Parameters_Calculate(channel, WARMING_UP);

    if (i == FIN_OK)
    {
        PrintOK();
    }
    else if (i == FIN_ERROR)
    {
        PrintERR();
        printf("sequence ended with FIN_ERROR\n");
    }
    
}


void Tests_Show_Parameters (void)
{
    printf("Showing Parameters Calculations...\n");

    printf("Fixing antenna params...\n");
// const char s_antena [] = { "ant0,012.27,087.90,001.80,065.00\r\n" };        
    session_ch_1.ant_resistance_int = 24;
    session_ch_1.ant_resistance_dec = 0;
    session_ch_1.ant_inductance_int = 141;
    session_ch_1.ant_inductance_dec = 0;
    session_ch_1.ant_current_limit_int = 1;
    session_ch_1.ant_current_limit_dec = 80;
    session_ch_1.ant_temp_max_int = 65;
    session_ch_1.ant_temp_max_dec = 0;

    // session_ch_1.ant_resistance_int = 12;
    // session_ch_1.ant_resistance_dec = 27;
    // session_ch_1.ant_inductance_int = 87;
    // session_ch_1.ant_inductance_dec = 90;
    // session_ch_1.ant_current_limit_int = 1;
    // session_ch_1.ant_current_limit_dec = 80;
    // session_ch_1.ant_temp_max_int = 65;
    // session_ch_1.ant_temp_max_dec = 0;
    
    printf("Fixing stage params...\n");
    session_ch_1.stage_1_time_hours = 1;
    session_ch_1.stage_1_time_minutes = 0;
    session_ch_1.stage_1_time_seconds = 0;

    session_ch_1.stage_1_initial_power = 90;
    session_ch_1.stage_1_final_power = 90;
    session_ch_1.stage_1_time_per_step = 0;

    TSP_Get_Signal_Times_From_Screen(SQUARE_SIGNAL, FREQ_7_83_HZ, &session_ch_1);    

    unsigned char i = 0;
    unsigned char channel = 1;
    // answers with FIN_OK or FIN_ERROR
    i = Session_Channels_Parameters_Calculate(channel, WARMING_UP);

    if (i == FIN_ERROR)
    {
        PrintERR();
        printf("sequence ended with FIN_ERROR\n");
    }

    TSP_Show_Table_Parameters (&table_warming_up_channel_1[0]);

}


void Tests_Always_Generate_Parameters_Calculate (void)
{
    printf("Parameters Calculations with Power Control...\n");

    printf("Fixing antenna params...\n");
    session_ch_1.ant_resistance_int = 10;
    session_ch_1.ant_resistance_dec = 10;
    session_ch_1.ant_inductance_int = 128;
    session_ch_1.ant_inductance_dec = 0;
    session_ch_1.ant_current_limit_int = 3;
    session_ch_1.ant_current_limit_dec = 50;
    session_ch_1.ant_temp_max_int = 65;
    session_ch_1.ant_temp_max_dec = 0;

    printf("Fixing stage params...\n");
    session_ch_1.stage_1_time_hours = 1;
    session_ch_1.stage_1_time_minutes = 0;
    session_ch_1.stage_1_time_seconds = 0;

    session_ch_1.stage_1_initial_power = 100;
    session_ch_1.stage_1_final_power = 100;
    session_ch_1.stage_1_time_per_step = 0;

    TSP_Get_Signal_Times_From_Screen(SQUARE_SIGNAL, FREQ_62_64_HZ, &session_ch_1);
    // TSP_Get_Signal_Times_From_Screen(TRIANGULAR_SIGNAL, FREQ_62_64_HZ, &session_ch_1);
    // TSP_Get_Signal_Times_From_Screen(SINUSOIDAL_SIGNAL, FREQ_62_64_HZ, &session_ch_1);
    // TSP_Get_Signal_Times_From_Screen(SINUSOIDAL_SIGNAL, FREQ_7_83_HZ, &session_ch_1);
    // TSP_Get_Signal_Times_From_Screen(SINUSOIDAL_SIGNAL, FREQ_11_79_HZ, &session_ch_1);
    // TSP_Get_Signal_Times_From_Screen(SINUSOIDAL_SIGNAL, FREQ_16_67_HZ, &session_ch_1);
    // TSP_Get_Signal_Times_From_Screen(SINUSOIDAL_SIGNAL, FREQ_23_58_HZ, &session_ch_1);
    // TSP_Get_Signal_Times_From_Screen(SINUSOIDAL_SIGNAL, FREQ_30_80_HZ, &session_ch_1);

    unsigned char i = 0;

    // get the setted power
    unsigned char ini = session_ch_1.stage_1_initial_power;
    unsigned char fin = session_ch_1.stage_1_final_power;

    i = Session_Channels_Parameters_Calculate_Generate_Always (1, WARMING_UP);

    if (i == FIN_OK)
    {
        printf ("sequence ended ok with initial_power: %d final_power: %d\n",
                session_ch_1.stage_1_initial_power,
                session_ch_1.stage_1_final_power);
    }

    TSP_Show_Table_Parameters (&table_warming_up_channel_1[0]);    

    printf ("Test result: ");
    if (i == FIN_OK)
        PrintOK();
    else
        PrintERR ();

}




// Mocked Functions for module compilation -------------------------------------
void Update_TIM1_CH1 (unsigned short a)
{
}

void Update_TIM1_CH4 (unsigned short a)
{
}

void Update_TIM2_CH1 (unsigned short a)
{
}

void Update_TIM2_CH2 (unsigned short a)
{
}

void Update_TIM3_CH1 (unsigned short a)
{
}

void Update_TIM3_CH2 (unsigned short a)
{
}

void Update_TIM3_CH3 (unsigned short a)
{
}

void Update_TIM3_CH4 (unsigned short a)
{
}

void Update_TIM4_CH1 (unsigned short a)
{
}

void Update_TIM4_CH2 (unsigned short a)
{
}

void Update_TIM4_CH3 (unsigned short a)
{
}

void Update_TIM4_CH4 (unsigned short a)
{
}

void AntennaEndTreatment (unsigned char a)
{
}

void Usart1Send (char * send)
{
}

void Wait_ms (unsigned short a)
{
}

void BuzzerCommands (unsigned char a, unsigned char b)
{
}

void Error_SetString (char * p_errstr, unsigned short err_num)
{
}

unsigned char ConvertSingleChannelFinishFlag (void)
{
    return 1;
}

void ConvertChannel (unsigned char a)
{
}

unsigned char AntennaVerifyForTreatment (unsigned char a)
{
    return 1;
}

unsigned char AntennaGetCurrentTemp (unsigned char a)
{
    return 1;
}

unsigned char AntennaGetConnection (unsigned char a)
{
    return 1;
}

void AntennaGetParamsStruct (unsigned char ch, antenna_typedef *ant)
{
    switch (ch)
    {
    case 1:
        ant->resistance_int = session_ch_1.ant_resistance_int;
        ant->resistance_dec = session_ch_1.ant_resistance_dec;
        ant->inductance_int = session_ch_1.ant_inductance_int;
        ant->inductance_dec = session_ch_1.ant_inductance_dec;
        ant->current_limit_int = session_ch_1.ant_current_limit_int;
        ant->current_limit_dec = session_ch_1.ant_current_limit_dec;
        ant->temp_max_int = session_ch_1.ant_temp_max_int;
        ant->temp_max_dec = session_ch_1.ant_temp_max_dec;        
        break;

    case 2:
        ant->resistance_int = session_ch_2.ant_resistance_int;
        ant->resistance_dec = session_ch_2.ant_resistance_dec;
        ant->inductance_int = session_ch_2.ant_inductance_int;
        ant->inductance_dec = session_ch_2.ant_inductance_dec;
        ant->current_limit_int = session_ch_2.ant_current_limit_int;
        ant->current_limit_dec = session_ch_2.ant_current_limit_dec;
        ant->temp_max_int = session_ch_2.ant_temp_max_int;
        ant->temp_max_dec = session_ch_2.ant_temp_max_dec;                
        break;

    case 3:
        ant->resistance_int = session_ch_3.ant_resistance_int;
        ant->resistance_dec = session_ch_3.ant_resistance_dec;
        ant->inductance_int = session_ch_3.ant_inductance_int;
        ant->inductance_dec = session_ch_3.ant_inductance_dec;
        ant->current_limit_int = session_ch_3.ant_current_limit_int;
        ant->current_limit_dec = session_ch_3.ant_current_limit_dec;
        ant->temp_max_int = session_ch_3.ant_temp_max_int;
        ant->temp_max_dec = session_ch_3.ant_temp_max_dec;                
        break;

    case 4:
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
// End of Module Mocked Functions for module compilation -----------------------


warmingup_coolingdown_typedef pwm_outputs;
void Tests_Local_Calc_Parameters (void)
{
    printf("Verify Local Parameters Calculations...\n");

    printf("Fixing antenna params...\n");
// const char s_antena [] = { "ant0,012.27,087.90,001.80,065.00\r\n" };            
    session_ch_1.ant_resistance_int = 10;
    session_ch_1.ant_resistance_dec = 1;
    session_ch_1.ant_inductance_int = 128;
    session_ch_1.ant_inductance_dec = 0;
    session_ch_1.ant_current_limit_int = 3;
    session_ch_1.ant_current_limit_dec = 50;
    session_ch_1.ant_temp_max_int = 65;
    session_ch_1.ant_temp_max_dec = 0;

    printf("Fixing stage params...\n");
    session_ch_1.stage_1_time_hours = 1;
    session_ch_1.stage_1_time_minutes = 0;
    session_ch_1.stage_1_time_seconds = 0;

    session_ch_1.stage_1_initial_power = 100;
    session_ch_1.stage_1_final_power = 100;
    session_ch_1.stage_1_time_per_step = 0;

    TSP_Get_Signal_Times_From_Screen(SINUSOIDAL_SIGNAL, FREQ_30_80_HZ, &session_ch_1);
    
    CalcParamsForSignal (1);
    
}


void Tests_Compare_Parameters (void)
{
    int errors = 0;
    printf("Compare Parameters Calculations...\n");

    printf("Fixing antenna params...\n");
// const char s_antena [] = { "ant0,012.27,087.90,001.80,065.00\r\n" };    
    session_ch_1.ant_resistance_int = 24;
    session_ch_1.ant_resistance_dec = 0;
    session_ch_1.ant_inductance_int = 141;
    session_ch_1.ant_inductance_dec = 0;
    session_ch_1.ant_current_limit_int = 1;
    session_ch_1.ant_current_limit_dec = 80;
    session_ch_1.ant_temp_max_int = 65;
    session_ch_1.ant_temp_max_dec = 0;

    printf("Fixing stage params...\n");
    session_ch_1.stage_1_time_hours = 1;
    session_ch_1.stage_1_time_minutes = 0;
    session_ch_1.stage_1_time_seconds = 0;

    session_ch_1.stage_1_initial_power = 100;
    session_ch_1.stage_1_final_power = 100;
    session_ch_1.stage_1_time_per_step = 0;

    // times for square signal 7.83Hz
    TSP_Get_Signal_Times_From_Screen (SQUARE_SIGNAL, FREQ_7_83_HZ, &session_ch_1);

    unsigned char i = 0;
    unsigned char channel = 1;
    // answers with FIN_OK or FIN_ERROR
    i = Session_Channels_Parameters_Calculate(channel, WARMING_UP);

    if (i == FIN_ERROR)
    {
        PrintERR();
        printf("sequence ended with FIN_ERROR\n");
        errors = 1;
    }

    CalcParamsForSignal (1);

    printf("Comparing local and embedded results: ");
    warmingup_coolingdown_typedef * pt = &table_warming_up_channel_1[0];
    
    if ((!Compare_Two_Params_With_Tolerance(pt->rising_pwm_200_initial, pwm_outputs.rising_pwm_200_initial, 1)) ||
        (!Compare_Two_Params_With_Tolerance(pt->rising_pwm_200_final, pwm_outputs.rising_pwm_200_final, 1)) ||
        (!Compare_Two_Params_With_Tolerance(pt->rising_pwm_40_initial, pwm_outputs.rising_pwm_40_initial, 1)) ||
        (!Compare_Two_Params_With_Tolerance(pt->rising_pwm_40_final, pwm_outputs.rising_pwm_40_final, 1)) ||
        (!Compare_Two_Params_With_Tolerance(pt->rising_pwm_n_initial, pwm_outputs.rising_pwm_n_initial, 1)) ||
        (!Compare_Two_Params_With_Tolerance(pt->rising_pwm_n_final, pwm_outputs.rising_pwm_n_final, 1)) ||
        (!Compare_Two_Params_With_Tolerance(pt->rising_step_number, pwm_outputs.rising_step_number, 1)))
    {
        PrintERR();
        printf(" Errors on rising parameters\n");

        printf("  rising pwm 200 initial: %d %d\n",
               pt->rising_pwm_200_initial,
               pwm_outputs.rising_pwm_200_initial);

        printf("  rising pwm 200 final: %d %d\n",
               pt->rising_pwm_200_final,
               pwm_outputs.rising_pwm_200_final);
        
        printf("  rising pwm 40 initial: %d %d\n",
               pt->rising_pwm_40_initial,
               pwm_outputs.rising_pwm_40_initial);

        printf("  rising pwm 40 final: %d %d\n",
               pt->rising_pwm_40_final,
               pwm_outputs.rising_pwm_40_final);
        
        printf("  rising pwm n initial: %d %d\n",
               pt->rising_pwm_n_initial,
               pwm_outputs.rising_pwm_n_initial);

        printf("  rising pwm n final: %d %d\n",
               pt->rising_pwm_n_final,
               pwm_outputs.rising_pwm_n_final);
        
        printf("  rising step number: %d %d\n",
               pt->rising_step_number,
               pwm_outputs.rising_step_number);        

        errors = 1;
    }


    if ((!Compare_Two_Params_With_Tolerance(pt->maintenance_pwm_200, pwm_outputs.maintenance_pwm_200, 1)) ||
        (!Compare_Two_Params_With_Tolerance(pt->maintenance_pwm_40, pwm_outputs.maintenance_pwm_40, 1)) ||
        (!Compare_Two_Params_With_Tolerance(pt->maintenance_pwm_n, pwm_outputs.maintenance_pwm_n, 1)) ||
        (!Compare_Two_Params_With_Tolerance(pt->maintenance_step_number, pwm_outputs.maintenance_step_number, 1)))
    {
        PrintERR();
        printf(" Errors on maintenance parameters\n");

        printf("  maintenance pwm 200: %d %d\n",
               pt->maintenance_pwm_200,
               pwm_outputs.maintenance_pwm_200);
        
        printf("  maintenance pwm 40: %d %d\n",
               pt->maintenance_pwm_40,
               pwm_outputs.maintenance_pwm_40);
        
        printf("  maintenance pwm n: %d %d\n",
               pt->maintenance_pwm_n,
               pwm_outputs.maintenance_pwm_n);
        
        printf("  maintenance step number: %d %d\n",
               pt->maintenance_step_number,
               pwm_outputs.maintenance_step_number);        

        errors = 1;        
    }

    
    if ((!Compare_Two_Params_With_Tolerance(pt->falling_pwm_200_initial, pwm_outputs.falling_pwm_200_initial, 1)) ||
        (!Compare_Two_Params_With_Tolerance(pt->falling_pwm_200_final, pwm_outputs.falling_pwm_200_final, 1)) ||
        (!Compare_Two_Params_With_Tolerance(pt->falling_pwm_40_initial, pwm_outputs.falling_pwm_40_initial, 1)) ||
        (!Compare_Two_Params_With_Tolerance(pt->falling_pwm_40_final, pwm_outputs.falling_pwm_40_final, 1)) ||
        (!Compare_Two_Params_With_Tolerance(pt->falling_pwm_n_initial, pwm_outputs.falling_pwm_n_initial, 1)) ||
        (!Compare_Two_Params_With_Tolerance(pt->falling_pwm_n_final, pwm_outputs.falling_pwm_n_final, 1)) ||
        (!Compare_Two_Params_With_Tolerance(pt->falling_time, pwm_outputs.falling_time, 1)) ||
        (!Compare_Two_Params_With_Tolerance(pt->falling_step_number, pwm_outputs.falling_step_number, 1)) ||
        (!Compare_Two_Params_With_Tolerance(pt->falling_type, pwm_outputs.falling_type, 1)))
    {
        PrintERR();
        printf(" Errors on falling parameters\n");

        printf("  falling pwm 200 initial: %d %d\n",
               pt->falling_pwm_200_initial,
               pwm_outputs.falling_pwm_200_initial);

        printf("  falling pwm 200 final: %d %d\n",
               pt->falling_pwm_200_final,
               pwm_outputs.falling_pwm_200_final);
        
        printf("  falling pwm 40 initial: %d %d\n",
               pt->falling_pwm_40_initial,
               pwm_outputs.falling_pwm_40_initial);

        printf("  falling pwm 40 final: %d %d\n",
               pt->falling_pwm_40_final,
               pwm_outputs.falling_pwm_40_final);
        
        printf("  falling pwm n initial: %d %d\n",
               pt->falling_pwm_n_initial,
               pwm_outputs.falling_pwm_n_initial);

        printf("  falling pwm n final: %d %d\n",
               pt->falling_pwm_n_final,
               pwm_outputs.falling_pwm_n_final);
        
        printf("  falling time: %d %d\n",
               pt->falling_time,
               pwm_outputs.falling_time);

        printf("  falling step number: %d %d\n",
               pt->falling_step_number,
               pwm_outputs.falling_step_number);        

        printf("  falling type: %d %d\n",
               pt->falling_type,
               pwm_outputs.falling_type);        
        
        errors = 1;
    }

    if (pt->low_step_number != pwm_outputs.low_step_number)
    {
        PrintERR();
        printf(" Errors on low steps parameter\n");

        printf("  low step number: %d %d\n",
               pt->low_step_number,
               pwm_outputs.low_step_number);        
        
        errors = 1;
    }

    if (!errors)
        PrintOK();
}



#define PSU_40		48
#define PSU_200	200
#define RSNUBBER_POWER_MAX		15
#define resistance_discharge 1175
#define capacitance_discharge 100 //uF
#define tau_discharge 0.1175 //RC
void CalcParamsForSignal (int show_parcial_results)
{
    antenna_typedef ant;
    AntennaGetParamsStruct(1, &ant);

    float voltage = 0.0;
    float voltage2 = 0.0;
    float current = 0.0;
    float inductance = 0.0;
    float resistance = 0.0;
    float power = 0.0;

    if (show_parcial_results)
    {
        printf("antenna params R: %d.%d L: %d.%d I: %d.%d\n",
               ant.resistance_int,
               ant.resistance_dec,
               ant.inductance_int,
               ant.inductance_dec,
               ant.current_limit_int,
               ant.current_limit_dec
            );

        printf("session rising params rising: %d maintenance: %d falling: %d low: %d\n",
               session_ch_1.stage_1_rising_time,
               session_ch_1.stage_1_maintenance_time,
               session_ch_1.stage_1_falling_time,
               session_ch_1.stage_1_low_time
            );

        printf("session power: %d\n",
               session_ch_1.stage_1_initial_power
            );
        
    }

    resistance = ant.resistance_int + ant.resistance_dec / 100.0;
    inductance = (ant.inductance_int + ant.inductance_dec / 100.0) / 1000.0;
    current = ant.current_limit_int + ant.current_limit_dec / 100.0;
    power = session_ch_1.stage_1_initial_power;

    if (show_parcial_results)
    {
        printf("float params  R: %f ohms  L: %f Hy  I:%f A\n",
               resistance,
               inductance,
               current
            );
    }

    // Power adjustment
    current = current * power / 100.0;

    if (show_parcial_results)
        printf("local current: %f\n", current);

    // Rising calcs
    voltage = (current * inductance * 1000.0) / session_ch_1.stage_1_rising_time;    //rising in ms
    voltage2 = voltage + current * resistance;

    if (show_parcial_results)
        printf("\nRising init voltage: %f end voltage2: %f\n", voltage, voltage2);

    float auxiliar_duty = 0.0;    
    if (voltage2 > PSU_200)
    {
        //si da mal PWM al 100%
        auxiliar_duty = 1000;
        pwm_outputs.rising_pwm_200_initial = (unsigned short)auxiliar_duty;

        //Final voltage.
        auxiliar_duty = 1000;
        pwm_outputs.rising_pwm_200_final = (unsigned short)auxiliar_duty;
        
    }

    else if (voltage2 > PSU_40)
    {
        //si da bien, hace la cuenta como siempre
        //Initial voltage.
        auxiliar_duty = (float)voltage * 1000 / PSU_200;
        pwm_outputs.rising_pwm_200_initial = (unsigned short)auxiliar_duty;

        //Final voltage.
        auxiliar_duty = (float)voltage2 * 1000 / PSU_200;
        pwm_outputs.rising_pwm_200_final = (unsigned short)auxiliar_duty;

    }

    else    //voltage2 < PSU_40
    {
        //Initial voltage.
        auxiliar_duty = (float)voltage * 1000 / PSU_40;
        pwm_outputs.rising_pwm_40_initial = (unsigned short)auxiliar_duty;

        //Final voltage.
        auxiliar_duty = (float)voltage2 * 1000 / PSU_40;
        pwm_outputs.rising_pwm_40_final = (unsigned short)auxiliar_duty;
    }

    auxiliar_duty = session_ch_1.stage_1_rising_time * 10;
    pwm_outputs.rising_step_number = auxiliar_duty;

    if (show_parcial_results)
    {
        printf("200_ini\t200_fin\t\t40_ini\t40_fin\t\tn_ini\tn_fin\tstep\n");
        printf("%d\t%d\t\t%d\t%d\t\t%d\t%d\t%d\n",
               pwm_outputs.rising_pwm_200_initial,
               pwm_outputs.rising_pwm_200_final,
               pwm_outputs.rising_pwm_40_initial,
               pwm_outputs.rising_pwm_40_final,
               pwm_outputs.rising_pwm_n_initial,
               pwm_outputs.rising_pwm_n_final,
               pwm_outputs.rising_step_number
            );
    }

    // Maintenance calcs
    voltage = resistance * current;

    if (show_parcial_results)
        printf("\nMaintenance voltage: %f\n", voltage);    

    auxiliar_duty = (float)voltage * 1000 / PSU_40;
    pwm_outputs.maintenance_pwm_40 = (unsigned short)auxiliar_duty;

    pwm_outputs.maintenance_step_number = session_ch_1.stage_1_maintenance_time * 10;
    
    if (show_parcial_results)
    {
        printf("pwm 200\t\tpwm 40\t\tpwm n\tstep\n");
        printf("%d\t\t%d\t\t%d\t%d\n",
               pwm_outputs.maintenance_pwm_200,
               pwm_outputs.maintenance_pwm_40,
               pwm_outputs.maintenance_pwm_n,
               pwm_outputs.maintenance_step_number
            );
    }

    // Falling calcs
    float falling_time = 0.0;
    float period = 0.0;
    float LR_tau = 0.0;
    float power_inductance = 0.0;
    float power_r_inductance = 0.0;
    float power_r_snubber = 0.0;
    float Vsnubber = 0.0;
    float Td = 0.0;
    
    LR_tau = (float) (inductance / (resistance));

    falling_time = session_ch_1.stage_1_falling_time;
    period = session_ch_1.stage_1_rising_time +
        session_ch_1.stage_1_maintenance_time +
        session_ch_1.stage_1_falling_time +
        session_ch_1.stage_1_low_time;

    falling_time /= 1000;	//a segundos
    period /= 1000;	//a segundos

    //potencia en el inductor
    power_inductance = (float) 0.5 * inductance * current * current / period;
    //tension rms estimada en RL
    voltage = current * resistance * sqrt(falling_time / (3.0 * period));
    //potencia en RL
    power_r_inductance = voltage * voltage / resistance;
    //potencia en Rsnubber
    power_r_snubber = power_inductance - power_r_inductance;

    if (show_parcial_results)
    {
        printf("\nFalling LR_tau: %f Power L: %f VRL: %f PRL: %f PRSnub: %f\n",
               LR_tau,
               power_inductance,
               voltage,
               power_r_inductance,
               power_r_snubber
            );
    }
    
    if (power_r_snubber > RSNUBBER_POWER_MAX)
        printf("ERROR on falling, power_r_snubber > RSNUBBER_POWER_MAX\n");

    //tension rms estimada en Rsnubber (esta es la real del circuito)
    Vsnubber = sqrt(power_r_snubber * resistance_discharge);

    //ajusto Vsnubber para calcular descarga rapida lo corrijo en Ipeak * Rserie
    Vsnubber += current * resistance;

    // modificacion 10-11-2021 reviso no tener logaritmos negativos
    auxiliar_duty = (current * resistance) / Vsnubber;
    if (auxiliar_duty > 0.99)
        Td = 0.0;
    else
        Td = (-LR_tau) * log(1 - (current * resistance) / Vsnubber);
    
    //Nuevo programa bajada
    Td *= 1000;		//paso a ms

    if (Td > (session_ch_1.stage_1_falling_time + session_ch_1.stage_1_low_time))
        printf("ERROR on falling, Td > falling + low time\n");
    
    Td += 0.25;		//ajusto un poquito

    //Slow discharge.
    if (session_ch_1.stage_1_falling_time  > (1000 * LR_tau))	//desde 1 tau utilizo descarga lenta; LR_tau lo paso a ms
    {
        pwm_outputs.falling_time = session_ch_1.stage_1_falling_time;
        pwm_outputs.falling_type = FALLING_SLOW_DISCHARGE;

        voltage =  resistance - (((float)inductance * 1000) / session_ch_1.stage_1_falling_time);
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
            pwm_outputs.falling_pwm_40_initial = (unsigned short)auxiliar_duty;

            auxiliar_duty = (float)voltage2 * 1000 / PSU_40;
            pwm_outputs.falling_pwm_40_final = (unsigned short)auxiliar_duty;

            //Steps.
            auxiliar_duty = (float)(session_ch_1.stage_1_falling_time + 0) * 10;
            pwm_outputs.falling_step_number = auxiliar_duty;

        }
        else
            printf("ERROR on falling, voltage > PSU_40 or voltage2 > PSU_40\n");
    }
    //LR discharge.
    else if (session_ch_1.stage_1_falling_time > (float)(Td * 1.2))	//es mayor que 1.2 veces la descarga rapida
    {
        pwm_outputs.falling_time = session_ch_1.stage_1_falling_time;	//aca le paso el valor elegido
        //auxiliar_duty = (float)pwm_outputs.falling_time * 10;
        auxiliar_duty = Td * 10;
        pwm_outputs.falling_step_number = auxiliar_duty;				//aca le paso el valor fast discharge
        pwm_outputs.falling_type = FALLING_LR;
    }
    //Fast discharge.
    else
    {
        pwm_outputs.falling_pwm_200_final = 0;
        pwm_outputs.falling_pwm_200_initial = 0;
        pwm_outputs.falling_pwm_40_final = 0;
        pwm_outputs.falling_pwm_40_initial = 0;
        pwm_outputs.falling_pwm_n_final = 1000;
        pwm_outputs.falling_pwm_n_initial = 1000;
        pwm_outputs.falling_type = FALLING_FAST_DISCHARGE;

        pwm_outputs.falling_time = session_ch_1.stage_1_falling_time;	//aca le paso el valor elegido
        
        printf("local Td: %f\n", Td);
        if (Td < session_ch_1.stage_1_falling_time)
        {
            auxiliar_duty = session_ch_1.stage_1_falling_time;
            auxiliar_duty *= 10;
            pwm_outputs.falling_step_number = auxiliar_duty;		//descargo mas rapido que el tiempo pedido
            auxiliar_duty = 0;
        }
        else	//Td es mayor que el tiempo disponible
        {
            auxiliar_duty = (float) Td * 10;
            pwm_outputs.falling_step_number = auxiliar_duty;		//aca le paso el valor fast discharge
            auxiliar_duty -= (session_ch_1.stage_1_falling_time) * 10;
        }
    }
    //Fin nuevo programa bajada

    //--- LOW ---//
    if (pwm_outputs.falling_type == FALLING_FAST_DISCHARGE)
        pwm_outputs.low_step_number = session_ch_1.stage_1_low_time * 10 - auxiliar_duty;
    else
        pwm_outputs.low_step_number = session_ch_1.stage_1_low_time * 10;

    // (p_table + i)->burst_mode_off = p_session->stage_1_burst_mode_off;
    // (p_table + i)->burst_mode_on = p_session->stage_1_burst_mode_on;
	// FALLING_SLOW_DISCHARGE = 1,
	// FALLING_LR,
	// FALLING_FAST_DISCHARGE
    
    if (show_parcial_results)
    {
        printf("200_ini\t200_fin\t\t40_ini\t40_fin\t\tn_ini\tn_fin\tfall_time\tfall_type\tstep\n");
        printf("%d\t%d\t\t%d\t%d\t\t%d\t%d\t%d\t\t%d\t\t%d\n",
               pwm_outputs.falling_pwm_200_initial,
               pwm_outputs.falling_pwm_200_final,
               pwm_outputs.falling_pwm_40_initial,
               pwm_outputs.falling_pwm_40_final,
               pwm_outputs.falling_pwm_n_initial,
               pwm_outputs.falling_pwm_n_final,
               pwm_outputs.falling_time,
               pwm_outputs.falling_type,           
               pwm_outputs.falling_step_number
            );
    }

    if (show_parcial_results)
    {
        printf("\nLow step 0\n");
        printf("step\n");
        printf("%d\n", pwm_outputs.low_step_number);
    }
}


void FirstPulseCheck (unsigned char ch)
{
}


int Compare_Two_Params_With_Tolerance (unsigned short param1,
                                       unsigned short param2,
                                       unsigned short tolerance)
{
    int answer = 0;
    
    if (param1 > param2)
    {
        if ((param1 - param2) <= tolerance)
            answer = 1;
    }
    else
    {
        if ((param2 - param1) <= tolerance)
            answer = 1;        
    }

    return answer;
}


void TSP_Get_Signal_Times_From_Screen (type_of_signal_e signal, freq_value_e freq, session_typedef * psession)
{
    switch (signal)
    {
    case SQUARE_SIGNAL:
        TSP_Get_Signal_Times_From_Screen_Square (freq, psession);
        break;

    case TRIANGULAR_SIGNAL:
        TSP_Get_Signal_Times_From_Screen_Triangular (freq, psession);        
        break;

    case SINUSOIDAL_SIGNAL:
        TSP_Get_Signal_Times_From_Screen_Sinusoidal (freq, psession);
        break;

    default:
        printf("ERROR on signal select - Get_Signal_Timer_From_Screen -\n");
        break;
    }
}


void TSP_Get_Signal_Times_From_Screen_Square (freq_value_e freq, session_typedef * psession)
{
    switch (freq)
    {
    case FREQ_7_83_HZ:
        psession->stage_1_rising_time = 1;
        psession->stage_1_maintenance_time = 61;
        psession->stage_1_falling_time = 1;
        psession->stage_1_low_time = 64;            
        break;

    case FREQ_11_79_HZ:
        psession->stage_1_rising_time = 1;
        psession->stage_1_maintenance_time = 40;
        psession->stage_1_falling_time = 1;
        psession->stage_1_low_time = 43;
        break;

    case FREQ_16_67_HZ:
        psession->stage_1_rising_time = 1;
        psession->stage_1_maintenance_time = 28;
        psession->stage_1_falling_time = 1;
        psession->stage_1_low_time = 30;
        break;

    case FREQ_23_58_HZ:
        psession->stage_1_rising_time = 1;
        psession->stage_1_maintenance_time = 19;
        psession->stage_1_falling_time = 1;
        psession->stage_1_low_time = 21;
        break;

    case FREQ_30_80_HZ:
        psession->stage_1_rising_time = 1;
        psession->stage_1_maintenance_time = 14;
        psession->stage_1_falling_time = 1;
        psession->stage_1_low_time = 16;
        break;

    case FREQ_62_64_HZ:
        psession->stage_1_rising_time = 1;
        psession->stage_1_maintenance_time = 6;
        psession->stage_1_falling_time = 1;
        psession->stage_1_low_time = 8;
        break;
    
    default:
        printf("ERROR on frequency select - Get_Signal_Timer_From_Screen_Square -\n");
        break;
    }
}


void TSP_Get_Signal_Times_From_Screen_Triangular (freq_value_e freq, session_typedef * psession)
{
    switch (freq)
    {
    case FREQ_7_83_HZ:
        psession->stage_1_rising_time = 61;
        psession->stage_1_maintenance_time = 1;
        psession->stage_1_falling_time = 1;
        psession->stage_1_low_time = 64;            
        break;

    case FREQ_11_79_HZ:
        psession->stage_1_rising_time = 40;
        psession->stage_1_maintenance_time = 1;
        psession->stage_1_falling_time = 1;
        psession->stage_1_low_time = 43;
        break;

    case FREQ_16_67_HZ:
        psession->stage_1_rising_time = 28;
        psession->stage_1_maintenance_time = 1;
        psession->stage_1_falling_time = 1;
        psession->stage_1_low_time = 30;
        break;

    case FREQ_23_58_HZ:
        psession->stage_1_rising_time = 19;
        psession->stage_1_maintenance_time = 1;
        psession->stage_1_falling_time = 1;
        psession->stage_1_low_time = 21;
        break;

    case FREQ_30_80_HZ:
        psession->stage_1_rising_time = 14;
        psession->stage_1_maintenance_time = 1;
        psession->stage_1_falling_time = 1;
        psession->stage_1_low_time = 16;
        break;

    case FREQ_62_64_HZ:
        psession->stage_1_rising_time = 6;
        psession->stage_1_maintenance_time = 1;
        psession->stage_1_falling_time = 1;
        psession->stage_1_low_time = 8;
        break;

    default:
        printf("ERROR on frequency select - Get_Signal_Timer_From_Screen_Triangular -\n");
        break;
    }
    
}


void TSP_Get_Signal_Times_From_Screen_Sinusoidal (freq_value_e freq, session_typedef * psession)
{
    switch (freq)
    {
    case FREQ_7_83_HZ:
        psession->stage_1_rising_time = 21;
        psession->stage_1_maintenance_time = 21;
        psession->stage_1_falling_time = 21;
        psession->stage_1_low_time = 64;
        break;

    case FREQ_11_79_HZ:
        psession->stage_1_rising_time = 14;
        psession->stage_1_maintenance_time = 14;
        psession->stage_1_falling_time = 14;
        psession->stage_1_low_time = 43;
        break;

    case FREQ_16_67_HZ:
        psession->stage_1_rising_time = 10;
        psession->stage_1_maintenance_time = 10;
        psession->stage_1_falling_time = 10;
        psession->stage_1_low_time = 30;
        break;

    case FREQ_23_58_HZ:
        psession->stage_1_rising_time = 7;
        psession->stage_1_maintenance_time = 7;
        psession->stage_1_falling_time = 7;
        psession->stage_1_low_time = 21;
        break;

    case FREQ_30_80_HZ:
        psession->stage_1_rising_time = 5;
        psession->stage_1_maintenance_time = 6;
        psession->stage_1_falling_time = 5;
        psession->stage_1_low_time = 16;
        break;

    case FREQ_62_64_HZ:
        psession->stage_1_rising_time = 3;
        psession->stage_1_maintenance_time = 2;
        psession->stage_1_falling_time = 3;
        psession->stage_1_low_time = 8;
        break;

    default:
        printf("ERROR on frequency select - Get_Signal_Timer_From_Screen_Sinusoidal -\n");
        break;
    }
    
}


void TSP_Show_Table_Parameters (warmingup_coolingdown_typedef * ptable)
{
    printf("\nRising first step 0\n");
    printf("200_ini\t200_fin\t\t40_ini\t40_fin\t\tn_ini\tn_fin\tstep\n");
    printf("%d\t%d\t\t%d\t%d\t\t%d\t%d\t%d\n",
           ptable->rising_pwm_200_initial,
           ptable->rising_pwm_200_final,
           ptable->rising_pwm_40_initial,
           ptable->rising_pwm_40_final,
           ptable->rising_pwm_n_initial,
           ptable->rising_pwm_n_final,
           ptable->rising_step_number
        );        

    printf("\nMaintenance first step 0\n");
    printf("200_pwm\t\t40_pwm\t\tn_pwm\tstep\n");
    printf("%d\t\t%d\t\t%d\t%d\n",
           ptable->maintenance_pwm_200,
           ptable->maintenance_pwm_40,
           ptable->maintenance_pwm_n,
           ptable->maintenance_step_number
        );        
    
    printf("\nFalling first step 0\n");
    printf("200_ini\t200_fin\t\t40_ini\t40_fin\t\tn_ini\tn_fin\ttime\tstep\ttype\n");
    printf("%d\t%d\t\t%d\t%d\t\t%d\t%d\t%d\t%d\t%d\n",
           ptable->falling_pwm_200_initial,
           ptable->falling_pwm_200_final,
           ptable->falling_pwm_40_initial,
           ptable->falling_pwm_40_final,
           ptable->falling_pwm_n_initial,
           ptable->falling_pwm_n_final,
           ptable->falling_time,
           ptable->falling_step_number,
           ptable->falling_type
        );        

    printf("\nLow step 0\n");
    printf("step\n");
    printf("%d\n", ptable->low_step_number);
}

//--- end of file ---//


