//---------------------------------------------------------
// #### PROYECTO MAGNETO GAUSSTEK - MotherBoard ###########
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F103
// ##
// #### ERRORS.C ##########################################
//---------------------------------------------------------
#include "errors.h"

#include <stdio.h>


// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------


// Module Private Functions ----------------------------------------------------


// Module Functions ------------------------------------------------------------
void Error_SetString (char * p_errstr, unsigned short err_num)
{
    sprintf(p_errstr, "ERROR(0x%03X)\r\n", err_num);
}


//--- end of file ---//

