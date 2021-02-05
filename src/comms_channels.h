//---------------------------------------------------------
// #### PROYECTO MAGNETO GAUSSTEK - MotherBoard ###########
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F103
// ##
// #### COMMS_CHANNELS.H ##################################
//---------------------------------------------------------
#ifndef _COMMS_CHANNELS_H_
#define _COMMS_CHANNELS_H_


// Exported Types Constants and Macros -----------------------------------------
typedef enum {
    ok_answer = 0,
    ok_no_answer,
    nok_answer,
    nok_no_answer

} comms_ch_answer_e;



// Configuration Defines -------------------------------------------------------
#define UART_CH1_Receive()	UART2_Receive()
#define UART_CH2_Receive()	UART3_Receive()
#define UART_CH3_Receive()	UART4_Receive()
#define UART_CH4_Receive()	UART5_Receive()



// Module Exported Functions ---------------------------------------------------
void UART2_Receive (void);
void UART3_Receive (void);
void UART4_Receive (void);
void UART5_Receive (void);

comms_ch_answer_e ParseCommsWithChannels (char * str, unsigned char channel);


#endif    /* _COMMS_CHANNELS_H_ */

//--- end of file ---//

