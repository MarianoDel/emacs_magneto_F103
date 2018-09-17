//---------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F103
// ##
// #### USART.H ################################
//---------------------------------------------

#ifndef _USART_H_
#define _USART_H_

//---- Includes to help the Defines ----------
#include "stm32f10x.h"


//---- Configurations Defines --------------------
#define SIZEOF_TXDATA        256
#define SIZEOF_RXDATA        256

//se pueden colocar ifdef para cada usart y habilitar o no cada una
//reduciria el codigo generado por el compilador

//---- Exported Macros and Defines --------------------
#define UART_PC_Init() 		Usart1Config()
#define UART_PC_Send(x) 	Usart1Send(x)

#define UART_CH1_Init()		Usart2Config()
#define UART_CH1_Send(x)	Usart2Send(x)

#define UART_CH2_Init()		Usart3Config()
#define UART_CH2_Send(x)	Usart3Send(x)

#define UART_CH3_Init()		Uart4Config()
#define UART_CH3_Send(x)	Uart4Send(x)

#define UART_CH4_Init()		Uart5Config()
#define UART_CH4_Send(x)	Uart5Send(x)

//---- Common Defines --------------------
// 0xMMMF    Mantissa MMM Fraction F/16
//
// Para utilizar con XTAL y freq 72MHz
#define USART_XTAL_PCKL1_9600        0x0EA6
#define USART_XTAL_PCKL2_9600        0x1D4C

// Para utilizar con RC interno (estan compensadas las diferencias)
#define USART_RC_PCKL1_9600        0x0DD3    //con rc interno da 9060, corrijo
#define USART_RC_PCKL2_9600        0x1B40    //con rc interno da 8930, corrijo


#define USART1_9600        USART_XTAL_PCKL2_9600
#define USART2_9600        USART_XTAL_PCKL1_9600
#define USART3_9600        USART_XTAL_PCKL1_9600
#define UART4_9600         USART_XTAL_PCKL1_9600
#define UART5_9600         USART_XTAL_PCKL1_9600

// #define USART1_9600        USART_RC_PCKL2_9600
// #define USART2_9600        USART_RC_PCKL1_9600
// #define USART3_9600        USART_RC_PCKL1_9600
// #define UART4_9600         USART_RC_PCKL1_9600
// #define UART5_9600         USART_RC_PCKL1_9600

#define RCC_USART1_CLK (RCC->APB2ENR & 0x00004000)
#define RCC_USART1_CLKEN RCC->APB2ENR |= 0x00004000
#define RCC_USART1_CLKDIS RCC->APB2ENR &= ~0x00004000

#define RCC_USART2_CLK (RCC->APB1ENR & 0x00020000)
#define RCC_USART2_CLKEN RCC->APB1ENR |= 0x00020000
#define RCC_USART2_CLKDIS RCC->APB1ENR &= ~0x00020000

#define RCC_USART3_CLK (RCC->APB1ENR & 0x00040000)
#define RCC_USART3_CLKEN RCC->APB1ENR |= 0x00040000
#define RCC_USART3_CLKDIS RCC->APB1ENR &= ~0x00040000

#define RCC_UART4_CLK (RCC->APB1ENR & 0x00080000)
#define RCC_UART4_CLKEN RCC->APB1ENR |= 0x00080000
#define RCC_UART4_CLKDIS RCC->APB1ENR &= ~0x00080000

#define RCC_UART5_CLK (RCC->APB1ENR & 0x00100000)
#define RCC_UART5_CLKEN RCC->APB1ENR |= 0x00100000
#define RCC_UART5_CLKDIS RCC->APB1ENR &= ~0x00100000


//--- Exported functions ---//
void Usart1Config (void);
void Usart1Send (char *);
void Usart1SendUnsigned (unsigned char *, unsigned char);
unsigned char ReadUsart1Buffer (unsigned char *, unsigned short);
void USART1_IRQHandler (void);

void Usart2Config (void);
void Usart2Send (char *);
void Usart2SendUnsigned (unsigned char *, unsigned char);
unsigned char ReadUsart2Buffer (unsigned char *, unsigned short);
void USART2_IRQHandler (void);

void Usart3Config (void);
void Usart3Send (char *);
void Usart3SendUnsigned (unsigned char *, unsigned char);
unsigned char ReadUsart3Buffer (unsigned char *, unsigned short);
void USART3_IRQHandler (void);

void Uart4Config (void);
void Uart4Send (char *);
void Uart4SendUnsigned (unsigned char *, unsigned char);
unsigned char ReadUart4Buffer (unsigned char *, unsigned short);
void UART4_IRQHandler (void);

void Uart5Config (void);
void Uart5Send (char *);
void Uart5SendUnsigned (unsigned char *, unsigned char);
unsigned char ReadUart5Buffer (unsigned char *, unsigned short);
void UART5_IRQHandler (void);

#endif
