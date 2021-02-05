//---------------------------------------------------------
// #### PROYECTO MAGNETO GAUSSTEK - MotherBoard ###########
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F103
// ##
// #### GTK_HARD.H ########################################
//---------------------------------------------------------
#ifndef _GTK_HARD_H_
#define _GTK_HARD_H_

//---- Includes to help the Defines ----------
#include "stm32f10x.h"

//---- Configurations Defines --------------------
//----- Board Configuration -------------------//
//--- Hardware ------------------//
#define HARDWARE_VERSION_2_1
// #define HARDWARE_VERSION_2_0    //placas viejas con wire-rap ydisipador color aluminio SIN BUZZER
#define SOFTWARE_VERSION_1_3        //funciona con la nueva interface grafica
// #define SOFTWARE_VERSION_1_2		//Agrega buzzer en la placa, la version 2.0 no lo tiene
// #define SOFTWARE_VERSION_1_1			//Agrega posibilidad de usar antenas harcodeadas
//#define SOFTWARE_VERSION_1_0

// #define ANTENNA_HARDCODED        //para no pedir comunicacion con la placa de antenna
#define NO_ERRORS_IN_RISING      //en rising edge a veces necesita mucha tension, Param Calc no tira error
#define USE_CURRENT_SLOT_WITHOUT_LOAD_CMD    //se puede dar start, sin hacer el load previo

//---- Internal configs depending on the prevous versions -------------//
#ifdef HARDWARE_VERSION_2_1
#define USE_BUZZER_ON_BOARD
#endif

#ifdef SOFTWARE_VERSION_1_3
#define USE_ADC_SAMPLE_BY_SAMPLE
#define USE_DEVICE_ID_4BYTES
// #define USE_DEVICE_ID_12BYTES
#endif
#ifdef SOFTWARE_VERSION_1_2
#define USE_ADC_SAMPLE_BY_SAMPLE
#endif
//---- end of Inernals configs ----------------------------------------//
#ifdef HARDWARE_VERSION_2_1
#define HARD "Hardware Version: 2.1\r\n"
#endif
#ifdef HARDWARE_VERSION_2_0
#define HARD "Hardware Version: 2.0\r\n"
#endif

//--- Software ------------------//
#ifdef SOFTWARE_VERSION_1_3
#define SOFT "Software Version: 1.3\r\n"
#endif
#ifdef SOFTWARE_VERSION_1_2
#define SOFT "Software Version: 1.2\r\n"
#endif
#ifdef SOFTWARE_VERSION_1_1
#define SOFT "Software Version: 1.1\r\n"
#endif
#ifdef SOFTWARE_VERSION_1_0
#define SOFT "Software Version: 1.0\r\n"
#endif

//--- Clock frequency and Crystal selection ---//
//#define sysFREC48
//#define sysFREC72_XTAL_8
#define sysFREC72_XTAL_12
//--- Voltage of the Powers Supply used ---//
#define PSU_40		48
#define PSU_200	200
#define RSNUBBER_POWER_MAX		15
#define resistance_discharge 1175
#define capacitance_discharge 100 //uF
#define tau_discharge 0.1175 //RC

//----- End of Board Configuration ------------//

//----- Use it whithout Antenna Board Configuration ----------//
#ifdef ANTENNA_HARDCODED
#define WITHOUT_ANTENNA_BOARD_CH1
#define WITHOUT_ANTENNA_BOARD_CH2
#define WITHOUT_ANTENNA_BOARD_CH3
#define WITHOUT_ANTENNA_BOARD_CH4
#endif
//valores decimales son enteros/100, 0.9 es 90
// #define HARDCODE_R_INT_CH1	3
// #define HARDCODE_R_DEC_CH1 0		//estos los divide por 10 y los suma al int
// #define HARDCODE_L_INT_CH1 18
// #define HARDCODE_L_DEC_CH1 0		//estos los divide por 10 y los suma al int
// #define HARDCODE_I_INT_CH1 3
// #define HARDCODE_I_DEC_CH1 5		//estos los divide por 10 y los suma al int

#define HARDCODE_R_INT_CH1	42
#define HARDCODE_R_DEC_CH1 80
#define HARDCODE_L_INT_CH1 400
#define HARDCODE_L_DEC_CH1 0
#define HARDCODE_I_INT_CH1 0
#define HARDCODE_I_DEC_CH1 90

// #define HARDCODE_R_INT_CH1	42
// #define HARDCODE_R_DEC_CH1 8
// #define HARDCODE_L_INT_CH1 400
// #define HARDCODE_L_DEC_CH1 0
// #define HARDCODE_I_INT_CH1 1		//de 0.9 en adelante no va con 3 3 3 6 de senial
// #define HARDCODE_I_DEC_CH1 0

#define HARDCODE_R_INT_CH2	42
#define HARDCODE_R_DEC_CH2 80
#define HARDCODE_L_INT_CH2 400
#define HARDCODE_L_DEC_CH2 0
#define HARDCODE_I_INT_CH2 0
#define HARDCODE_I_DEC_CH2 90

// #define HARDCODE_R_INT_CH2	42
// #define HARDCODE_R_DEC_CH2 8
// #define HARDCODE_L_INT_CH2 400
// #define HARDCODE_L_DEC_CH2 0
// #define HARDCODE_I_INT_CH2 1
// #define HARDCODE_I_DEC_CH2 0

#define HARDCODE_R_INT_CH3	47
#define HARDCODE_R_DEC_CH3 50
#define HARDCODE_L_INT_CH3 500
#define HARDCODE_L_DEC_CH3 0
#define HARDCODE_I_INT_CH3 0
#define HARDCODE_I_DEC_CH3 80

// #define HARDCODE_R_INT_CH3	47
// #define HARDCODE_R_DEC_CH3 5
// #define HARDCODE_L_INT_CH3 500
// #define HARDCODE_L_DEC_CH3 0
// #define HARDCODE_I_INT_CH3 1
// #define HARDCODE_I_DEC_CH3 0

#define HARDCODE_R_INT_CH4	49
#define HARDCODE_R_DEC_CH4 0
#define HARDCODE_L_INT_CH4 350
#define HARDCODE_L_DEC_CH4 0
#define HARDCODE_I_INT_CH4 0
#define HARDCODE_I_DEC_CH4 80

// #define HARDCODE_R_INT_CH4	49
// #define HARDCODE_R_DEC_CH4 0
// #define HARDCODE_L_INT_CH4 350
// #define HARDCODE_L_DEC_CH4 0
// #define HARDCODE_I_INT_CH4 1
// #define HARDCODE_I_DEC_CH4 0
//----- End of Use it whithout Antenna Board Configuration ---//



enum bool
{
	FALSE = 0,
	TRUE = !FALSE
};


//--- Configuracion de leds ---//
//--- PC0 ---//
#define LED1    ((GPIOC->ODR & 0x0001) == 0)
#define LED1_OFF    (GPIOC->BSRR = 0x00000001)
#define LED1_ON    (GPIOC->BSRR = 0x00010000)

//--- PC1 ---//
#define LED2    ((GPIOC->ODR & 0x0002) == 0)
#define LED2_OFF    (GPIOC->BSRR = 0x00000002)
#define LED2_ON    (GPIOC->BSRR = 0x00020000)

//--- PC13 ---//
#ifdef HARDWARE_VERSION_2_1
#define BUZZER    ((GPIOC->ODR & 0x2000) != 0)
#define BUZZER_ON    (GPIOC->BSRR = 0x00002000)
#define BUZZER_OFF    (GPIOC->BSRR = 0x20000000)
#endif

//--- PB12 ---//
#define LED3    ((GPIOB->ODR & 0x1000) == 0)
#define LED3_OFF    (GPIOB->BSRR = 0x00001000)
#define LED3_ON    (GPIOB->BSRR = 0x10000000)

//--- PB0 ---//
#define ENA_CH1    ((GPIOB->ODR & 0x0001) != 0)
#define ENA_CH1_ON    (GPIOB->BSRR = 0x00000001)
#define ENA_CH1_OFF    (GPIOB->BSRR = 0x00010000)

//--- PB1 ---//
#define ENA_CH2    ((GPIOB->ODR & 0x0002) != 0)
#define ENA_CH2_ON    (GPIOB->BSRR = 0x00000002)
#define ENA_CH2_OFF    (GPIOB->BSRR = 0x00020000)

//--- PB2 ---//
#define ENA_CH3    ((GPIOB->ODR & 0x0004) != 0)
#define ENA_CH3_ON    (GPIOB->BSRR = 0x00000004)
#define ENA_CH3_OFF    (GPIOB->BSRR = 0x00040000)

//--- PB13 ---//
#ifdef HARDWARE_VERSION_2_0
#define ENA_CH4 LED3
#define ENA_CH4_ON LED3_OFF
#define ENA_CH4_OFF LED3_ON
#else
#define ENA_CH4    ((GPIOB->ODR & 0x2000) != 0)
#define ENA_CH4_ON    (GPIOB->BSRR = 0x00002000)
#define ENA_CH4_OFF    (GPIOB->BSRR = 0x20000000)
#endif


#define RCC_ADC1_CLK    (RCC->APB2ENR & 0x00000200)
#define RCC_ADC1_CLKEN    (RCC->APB2ENR |= 0x00000200)
#define RCC_ADC1_CLKDIS    (RCC->APB2ENR &= ~0x00000200)

#define RCC_ADC2_CLK    (RCC->APB2ENR & 0x00000400)
#define RCC_ADC2_CLKEN    (RCC->APB2ENR |= 0x00000400)
#define RCC_ADC2_CLKDIS    (RCC->APB2ENR &= ~0x00000400)

#define RCC_ADC3_CLK    (RCC->APB2ENR & 0x00008000)
#define RCC_ADC3_CLKEN    (RCC->APB2ENR |= 0x00008000)
#define RCC_ADC3_CLKDIS    (RCC->APB2ENR &= ~0x00008000)

typedef enum
{
	BUZZER_INIT = 0,
	BUZZER_MULTIPLE_SHORT,
	BUZZER_MULTIPLE_SHORTA,
	BUZZER_MULTIPLE_SHORTB,
	BUZZER_MULTIPLE_HALF,
	BUZZER_MULTIPLE_HALFA,
	BUZZER_MULTIPLE_HALFB,
	BUZZER_MULTIPLE_LONG,
	BUZZER_MULTIPLE_LONGA,
	BUZZER_MULTIPLE_LONGB,
	BUZZER_TO_STOP

} tBuzzer;

//--- Tiempos (TIMEOUT) del buzzer
#define TIM_BIP_SHORT       300
#define TIM_BIP_SHORT_WAIT        500
#define TIM_BIP_HALF        600
#define TIM_BIP_HALF_WAIT        800
#define TIM_BIP_LONG        2000
#define TIM_BIP_LONG_WAIT        2000


//Estados Externos de LED BLINKING
#define LED_NO_BLINKING    0
#define LED_TREATMENT_STANDBY    1
#define LED_TREATMENT_GENERATING    2
#define LED_TREATMENT_PAUSED    3


//--- Exported Module Functions ----
void ChangeLed (unsigned char);
void UpdateLed (void);
void UpdateBuzzer (void);
void BuzzerCommands(unsigned char , unsigned char );

#endif    /* _GTK_HARD_H_ */

//--- end of file ---//

