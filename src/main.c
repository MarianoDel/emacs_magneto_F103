
#include "stm32f10x.h"
#include "GTK_Hard.h"
#include "timer.h"
#include "uart.h"
#include "pwm.h"
#include "adc.h"
#include "GTK_Estructura.h"
#include "GTK_Signal.h"
#include "flash_program.h"
#include "GTK_Errors.h"


//--- Externals para enviar errores en UART
char buffSendErr[64];

//--- Externals para enviar keepalive por UART
#define TIME_RUN_DEF 250
volatile unsigned short timeRun = TIME_RUN_DEF;

//--- Externals para muestreos de corriente con el ADC
volatile unsigned char flagMuestreo = 0;
volatile unsigned char take_current_samples = 0;

//--- Externals para armar señales y comprobar el TIM5 en el inicio del programa
volatile unsigned int session_warning_up_channel_1_stage_time = 0;


//Estructuras.
session_typedef session_slot_aux;

session_typedef session_ch_1;
session_typedef session_ch_2;
session_typedef session_ch_3;
session_typedef session_ch_4;

unsigned char temp_actual_channel_1_int = 0;
unsigned char temp_actual_channel_1_dec = 0;
unsigned char temp_actual_channel_2_int = 0;
unsigned char temp_actual_channel_2_dec = 0;
unsigned char temp_actual_channel_3_int = 0;
unsigned char temp_actual_channel_3_dec = 0;
unsigned char temp_actual_channel_4_int = 0;
unsigned char temp_actual_channel_4_dec = 0;

//session_typedef session_slot_1;
//session_typedef session_slot_2;
//session_typedef session_slot_3;
//session_typedef session_slot_4;
//session_typedef session_slot_5;

unsigned char channel_1_pause = 0;
unsigned char channel_2_pause = 0;
unsigned char channel_3_pause = 0;
unsigned char channel_4_pause = 0;


void SystemInit (void);		//dumb function en otro momento inicializaba clock aca

int main (void)
{
	unsigned char i = 0;
	unsigned char counter_keep_alive = 0;
	//Configuracion de clock.
	RCC_Config ();

	//Configuracion led. & Enabled Channels
	Led_Config();

	//Timer 1ms -- Wait_ms()
	TIM7_Init();

	//Timer 100ms.
	TIM6_Init();

	//Signal timer -- 100us
	TIM5_Init();

	//ADC1.
	ADC1_Init();

	//UART_Debug Config.
	UART_PC_Init();
	UART_CH1_Init();
	UART_CH2_Init();
	UART_CH3_Init();
	UART_CH4_Init();

	//Configuracion PWM.
	PWM1_Init();
	PWM2_Init();
	PWM3_Init();
	PWM4_Init();

	//Initialize channels.
	Channel_1_Init();
	Channel_2_Init();
	Channel_3_Init();
	Channel_4_Init();

	//pruebo TIM5 100us step
	for (i = 0; i < 20; i++)
	{
		if (LED1)
			LED1_OFF;
		else
			LED1_ON;

		while (session_warning_up_channel_1_stage_time != 0);
		session_warning_up_channel_1_stage_time = 1000;	//100ms
//		Wait_ms(100);
	}


	UART_PC_Send("Ready! \r\n");

	//mando ENABLE los canales
	ENA_CH1_ON;
	ENA_CH2_ON;
	ENA_CH3_ON;
	ENA_CH4_ON;
	Wait_ms(10);
	ENA_CH1_OFF;
	ENA_CH2_OFF;
	ENA_CH3_OFF;
	ENA_CH4_OFF;

	while (TRUE)
	{
		if (timeRun == 0)	//decrece cada 1ms
		{

			if (counter_keep_alive > 10)
			{
				counter_keep_alive = 0;

				//sprintf(&buffSendErr[0], (const char *) "keepalive,%d,%d,%d,%d\r\n", session_ch_1.status, session_ch_2.status, session_ch_3.status, session_ch_4.status);
				//UART_PC_Send(&buffSendErr[0]);

				//UART_PC_Send("ant0,010.60,078.20,002.00,065.00,4\r\n");

				if (session_ch_1.status == 0)
					UART_CH1_Send("keepalive\r\n");
				if (session_ch_2.status == 0)
					UART_CH2_Send("keepalive\r\n");
				if (session_ch_3.status == 0)
					UART_CH3_Send("keepalive\r\n");
				if (session_ch_4.status == 0)
					UART_CH4_Send("keepalive\r\n");
			}
			else
				counter_keep_alive++;

			//Led3Toggle();

			timeRun = TIME_RUN_DEF;
		}

		//ADC control.
		Session_Current_Limit_control();

		//Channel 1.
		Session_Channel_1 ();
		UART_CH1_Receive();

		//Channel 2.
		Session_Channel_2 ();
		UART_CH2_Receive();

		//Channel 3.
		Session_Channel_3 ();
		UART_CH3_Receive();

		//Channel 4.
		Session_Channel_4 ();	//si comento esto se cuelga saltando siempre TIM1_1MS pero parece solo con el debugger puesto PROBR QUITNDO TODOS LOS BREAKS
		UART_CH4_Receive();

		//Recepcion de la configuracion por PC.
		UART_PC_Receive();

		//Chequeo de errores globales
		CheckforGlobalErrors();

	}
}

void SystemInit (void)
{

}

