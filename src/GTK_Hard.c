//---------------------------------------------------------
// #### PROYECTO MAGNETO GAUSSTEK - MotherBoard ###########
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F103
// ##
// #### GTK_HARD.C ########################################
//---------------------------------------------------------

/* Includes ------------------------------------------------------------------*/
#include "GTK_Hard.h"
#include <stm32f10x.h>


// Module Private Types Constants and Macros -----------------------------------
typedef enum
{    
    START_BLINKING = 0,
    WAIT_TO_OFF,
    WAIT_TO_ON,
    WAIT_NEW_CYCLE

} led_state_t;



/* Externals -----------------------------------------------------------------*/
#ifdef USE_BUZZER_ON_BOARD
extern volatile unsigned short buzzer_timeout;
#endif
extern volatile unsigned short timer_led;


/* Globals -------------------------------------------------------------------*/
#ifdef USE_BUZZER_ON_BOARD
tBuzzer buzzer_state = BUZZER_INIT;
unsigned char buzz_multiple = 0;
#endif


led_state_t led_state = START_BLINKING;
unsigned char blink = 0;
unsigned char how_many_blinks = 0;


#define LED_ON    LED2_ON
#define LED_OFF    LED2_OFF

// Module Functions ------------------------------------------------------------
//cambia configuracion de bips del LED
void ChangeLed (unsigned char how_many)
{
    how_many_blinks = how_many;
    led_state = START_BLINKING;
}


//mueve el LED segun el estado del Programa
void UpdateLed (void)
{
    switch (led_state)
    {
        case START_BLINKING:
            blink = how_many_blinks;
            
            if (blink)
            {
                LED_ON;
                timer_led = 200;
                led_state++;
                blink--;
            }
            break;

        case WAIT_TO_OFF:
            if (!timer_led)
            {
                LED_OFF;
                timer_led = 200;
                led_state++;
            }
            break;

        case WAIT_TO_ON:
            if (!timer_led)
            {
                if (blink)
                {
                    blink--;
                    timer_led = 200;
                    led_state = WAIT_TO_OFF;
                    LED_ON;
                }
                else
                {
                    led_state = WAIT_NEW_CYCLE;
                    timer_led = 2000;
                }
            }
            break;

        case WAIT_NEW_CYCLE:
            if (!timer_led)
                led_state = START_BLINKING;

            break;

        default:
            led_state = START_BLINKING;
            break;
    }
}


#ifdef USE_BUZZER_ON_BOARD
void BuzzerCommands(unsigned char command, unsigned char multiple)
{
    buzzer_state = command;
    buzz_multiple = multiple;
}

void UpdateBuzzer (void)
{
    switch (buzzer_state)
    {
        case BUZZER_INIT:
            break;

        case BUZZER_MULTIPLE_SHORT:
            if (buzz_multiple > 0)
            {
                BUZZER_ON;
                buzzer_state++;
                buzzer_timeout = TIM_BIP_SHORT;
            }
            else
                buzzer_state = BUZZER_TO_STOP;
            break;

        case BUZZER_MULTIPLE_SHORTA:
            if (!buzzer_timeout)
            {
                buzzer_state++;
                BUZZER_OFF;
                buzzer_timeout = TIM_BIP_SHORT_WAIT;
            }
            break;

        case BUZZER_MULTIPLE_SHORTB:
            if (!buzzer_timeout)
            {
                if (buzz_multiple)
                    buzz_multiple--;

                buzzer_state = BUZZER_MULTIPLE_SHORT;
            }
            break;

        case BUZZER_MULTIPLE_HALF:
            if (buzz_multiple > 0)
            {
                BUZZER_ON;
                buzzer_state++;
                buzzer_timeout = TIM_BIP_HALF;
            }
            else
                buzzer_state = BUZZER_TO_STOP;
            break;

        case BUZZER_MULTIPLE_HALFA:
            if (!buzzer_timeout)
            {
                buzzer_state++;
                BUZZER_OFF;
                buzzer_timeout = TIM_BIP_HALF_WAIT;
            }
            break;

        case BUZZER_MULTIPLE_HALFB:
            if (!buzzer_timeout)
            {
                if (buzz_multiple)
                    buzz_multiple--;

                buzzer_state = BUZZER_MULTIPLE_HALF;
            }
            break;

        case BUZZER_MULTIPLE_LONG:
            if (buzz_multiple > 0)
            {
                BUZZER_ON;
                buzzer_state++;
                buzzer_timeout = TIM_BIP_LONG;
            }
            else
                buzzer_state = BUZZER_TO_STOP;
            break;

        case BUZZER_MULTIPLE_LONGA:
            if (!buzzer_timeout)
            {
                buzzer_state++;
                BUZZER_OFF;
                buzzer_timeout = TIM_BIP_LONG_WAIT;
            }
            break;

        case BUZZER_MULTIPLE_LONGB:
            if (!buzzer_timeout)
            {
                if (buzz_multiple)
                    buzz_multiple--;

                buzzer_state = BUZZER_MULTIPLE_LONG;
            }
            break;

        case BUZZER_TO_STOP:
        default:
            BUZZER_OFF;
            buzzer_state = BUZZER_INIT;
            break;
    }
}

#endif

//--- end of file ---//

