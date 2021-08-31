//---------------------------------------------
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    TEST PLATFORM FOR FIRMWARE
// ##
// #### TESTS_ERRORS_STR.C ####################
//---------------------------------------------

// Includes Modules for tests --------------------------------------------------
#include "errors.h"


// Include Auxiliary modules ---------------------------------------------------
#include "tests_ok.h"

#include <stdio.h>
#include <string.h>


// Module Types Constants and Macros -------------------------------------------


// Externals -------------------------------------------------------------------


// Globals ---------------------------------------------------------------------


// Module Tester Functions -----------------------------------------------------
void Tests_Errors_Strings (void);
void Antenna_Errors_in_Channel (unsigned char ch);
void Warming_Errors_in_Channel (unsigned char ch);
void Plateau_Errors_in_Channel (unsigned char ch);
void Cooling_Down_Errors_in_Channel (unsigned char ch);


// Module Functions ------------------------------------------------------------
int main (int argc, char *argv[])
{
    Tests_Errors_Strings ();
        
    return 0;
}


// Tester Functions ------------------------------------------------------------
void Tests_Errors_Strings (void)
{
    printf("Testing Errors Module...\n");

    for (int i = 1; i < 5; i++)
        Antenna_Errors_in_Channel(i);

    printf("\n");
    for (int i = 1; i < 5; i++)
        Warming_Errors_in_Channel(i);

    printf("\n");    
    for (int i = 1; i < 5; i++)
        Plateau_Errors_in_Channel(i);

    printf("\n");    
    for (int i = 1; i < 5; i++)
        Cooling_Down_Errors_in_Channel(i);

    
}
    
void Antenna_Errors_in_Channel (unsigned char ch)
{
    char cmp_str [100] = { '\0' };
    char err_str [100] = { '\0' };
    char errors_str [100] = { '\0' };    
    int errors = 0;
    
    printf("Antenna Errors on CH%d: ", ch);

    Error_SetString (err_str, ERR_CHANNEL_ANTENNA_DISCONNECTED(ch));
    sprintf(cmp_str, "ERROR(0x01%d)\r\n", ch);
    if (strcmp(cmp_str, err_str) != 0)
    {
        sprintf(errors_str, "error on antenna disconnected ch%d\n", ch);
        errors = 1;
    }        

    Error_SetString (err_str, ERR_CHANNEL_ANTENNA_LOST(ch));
    sprintf(cmp_str, "ERROR(0x02%d)\r\n", ch);
    if (strcmp(cmp_str, err_str) != 0)        
    {
        sprintf(errors_str, "error on antenna lost ch%d\n", ch);
        errors = 1;
    }        

    Error_SetString (err_str, ERR_CHANNEL_ANTENNA_NOT_EMITTING(ch));
    sprintf(cmp_str, "ERROR(0x03%d)\r\n", ch);
    if (strcmp(cmp_str, err_str) != 0)        
    {
        sprintf(errors_str, "error on antenna not emitting ch%d\n", ch);
        errors = 1;
    }        

    Error_SetString (err_str, ERR_CHANNEL_ANTENNA_TMP_OUT_OF_RANGE(ch));
    sprintf(cmp_str, "ERROR(0x04%d)\r\n", ch);        
    if (strcmp(cmp_str, err_str) != 0)        
    {
        sprintf(errors_str, "error on antenna temp out of range ch%d\n", ch);
        errors = 1;
    }        

    Error_SetString (err_str, ERR_CHANNEL_ANTENNA_CURRENT_OUT_OF_RANGE(ch));
    sprintf(cmp_str, "ERROR(0x05%d)\r\n", ch);        
    if (strcmp(cmp_str, err_str) != 0)        
    {
        sprintf(errors_str, "error on antenna current out of range ch%d\n", ch);
        errors = 1;
    }

    if (errors)
    {
        PrintERR();
        printf(errors_str);
    }
    else
        PrintOK();
    
}


void Warming_Errors_in_Channel (unsigned char ch)
{
    char cmp_str [100] = { '\0' };
    char err_str [100] = { '\0' };
    char errors_str [100] = { '\0' };    
    int errors = 0;
    
    printf("Warming Errors on CH%d: ", ch);

    Error_SetString (err_str, ERR_CHANNEL_WARMING_UP_PARAMETERS_CALCULATE(ch));
    sprintf(cmp_str, "ERROR(0x06%d)\r\n", ch);
    if (strcmp(cmp_str, err_str) != 0)
    {
        sprintf(errors_str, "error on warming parameters ch%d\n", ch);
        errors = 1;
    }        

    Error_SetString (err_str, ERR_CHANNEL_WARMING_UP(ch));
    sprintf(cmp_str, "ERROR(0x07%d)\r\n", ch);
    if (strcmp(cmp_str, err_str) != 0)        
    {
        sprintf(errors_str, "error on warming up ch%d\n", ch);
        errors = 1;
    }        

    if (errors)
    {
        PrintERR();
        printf(errors_str);
    }
    else
        PrintOK();
    
}


void Plateau_Errors_in_Channel (unsigned char ch)
{
    char cmp_str [100] = { '\0' };
    char err_str [100] = { '\0' };
    char errors_str [100] = { '\0' };    
    int errors = 0;
    
    printf("Plateau Errors on CH%d: ", ch);

    Error_SetString (err_str, ERR_CHANNEL_PLATEAU_PARAMETERS_CALCULATE(ch));
    sprintf(cmp_str, "ERROR(0x08%d)\r\n", ch);
    if (strcmp(cmp_str, err_str) != 0)
    {
        sprintf(errors_str, "error on plateau parameters ch%d\n", ch);
        errors = 1;
    }        

    Error_SetString (err_str, ERR_CHANNEL_PLATEAU(ch));
    sprintf(cmp_str, "ERROR(0x09%d)\r\n", ch);
    if (strcmp(cmp_str, err_str) != 0)        
    {
        sprintf(errors_str, "error on plateau up ch%d\n", ch);
        errors = 1;
    }        

    if (errors)
    {
        PrintERR();
        printf(errors_str);
    }
    else
        PrintOK();
    
}


void Cooling_Down_Errors_in_Channel (unsigned char ch)
{
    char cmp_str [100] = { '\0' };
    char err_str [100] = { '\0' };
    char errors_str [100] = { '\0' };    
    int errors = 0;
    
    printf("Cooling Down Errors on CH%d: ", ch);

    Error_SetString (err_str, ERR_CHANNEL_COOLING_DOWN_PARAMETERS_CALCULATE(ch));
    sprintf(cmp_str, "ERROR(0x10%d)\r\n", ch);
    if (strcmp(cmp_str, err_str) != 0)
    {
        sprintf(errors_str, "error on cooling down parameters ch%d\n", ch);
        errors = 1;
    }        

    Error_SetString (err_str, ERR_CHANNEL_COOLING_DOWN(ch));
    sprintf(cmp_str, "ERROR(0x11%d)\r\n", ch);
    if (strcmp(cmp_str, err_str) != 0)        
    {
        sprintf(errors_str, "error on cooling down ch%d\n", ch);
        errors = 1;
    }        

    if (errors)
    {
        PrintERR();
        printf(errors_str);
    }
    else
        PrintOK();

}


//--- end of file ---//


