#ifndef _USARTFV_H_
#define _USARTFV_H_

//CAMBIO MAGNETO V2.0 28-02-15
#define UART_PC_Init() 		UART1_Config()
#define UART_PC_Send(x) 	UART1Send(x)

#define UART_CH1_Init()		UART2_Config()
#define UART_CH1_Send(x)	UART2Send(x)

#define UART_CH2_Init()		UART3_Config()
#define UART_CH2_Send(x)	UART3Send(x)

#define UART_CH3_Init()		UART4_Config()
#define UART_CH3_Send(x)	UART4Send(x)

#define UART_CH4_Init()		UART5_Config()
#define UART_CH4_Send(x)	UART5Send(x)
//FIN CAMBIO 28-02-15



void UART1_Config(void);
void UART1Send(char * ptrSend);
void UART2_Config(void);
void UART2Send(char * ptrSend);
void UART3_Config(void);
void UART3Send(char * ptrSend);
void UART4_Config(void);
void UART4Send(char * ptrSend);
void UART5_Config(void);
void UART5Send(char * ptrSend);
void UART_Tim6 (void);

#endif
