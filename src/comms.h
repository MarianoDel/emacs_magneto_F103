//---------------------------------------------------------
// #### PROYECTO MAGNETO GAUSSTEK - MotherBoard ###########
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F103
// ##
// #### COMMS.H ###########################################
//---------------------------------------------------------
#ifndef _COMMS_H_
#define _COMMS_H_

//---- Includes to help the Defines ----------



//---- Configurations Defines --------------------



//---- Common Defines --------------------
#define UART_CH1_Receive()	UART2_Receive()
#define UART_CH2_Receive()	UART3_Receive()
#define UART_CH3_Receive()	UART4_Receive()
#define UART_CH4_Receive()	UART5_Receive()
#define UART_PC_Receive()	UART1_Receive()




//--- Exported functions ---//
void UART1_Receive (void);
void UART2_Receive (void);
void UART3_Receive (void);
void UART4_Receive (void);
void UART5_Receive (void);


#endif    /* _COMMS_H_ */

//--- end of file ---//

