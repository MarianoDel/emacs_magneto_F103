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
#include <stm32f10x.h>
#include "GTK_Hard.h"

/* Externals -----------------------------------------------------------------*/
#ifdef USE_BUZZER_ON_BOARD
extern unsigned short buzzer_timeout;
#endif

/* Globals -------------------------------------------------------------------*/
#ifdef USE_BUZZER_ON_BOARD
tBuzzer buzzer_state = BUZZER_INIT;
unsigned char buzz_multiple = 0;
#endif


/* Module Exported Functions -------------------------------------------------*/
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

