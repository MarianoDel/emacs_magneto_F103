
#include "uart.h"
#include "GTK_Estructura.h"
#include "stm32f10x_usart.h"
#include "misc.h"
#include "GTK_Signal.h"
#include "GTK_Hard.h"
#include "flash_program.h"
#include "timer.h"

#include <stdio.h>
#include <string.h>


//External variable.
//General.
extern session_typedef session_slot_aux;

extern session_typedef session_ch_1;
extern session_typedef session_ch_2;
extern session_typedef session_ch_3;
extern session_typedef session_ch_4;

extern unsigned char temp_actual_channel_1_int;
extern unsigned char temp_actual_channel_1_dec;
extern unsigned char temp_actual_channel_2_int;
extern unsigned char temp_actual_channel_2_dec;
extern unsigned char temp_actual_channel_3_int;
extern unsigned char temp_actual_channel_3_dec;
extern unsigned char temp_actual_channel_4_int;
extern unsigned char temp_actual_channel_4_dec;

extern unsigned char channel_1_pause;
extern unsigned char channel_2_pause;
extern unsigned char channel_3_pause;
extern unsigned char channel_4_pause;

//--- USART1 ---//
#define BUFFUART1RX_DIM 256
//#define BUFFUART1TX_DIM BUFFUART1RX_DIM + 64
#define BUFFUART1TX_DIM BUFFUART1RX_DIM + 256
volatile char buffUART1rx[BUFFUART1RX_DIM];
volatile char buffUART1rx2[BUFFUART1RX_DIM];
volatile char *pBuffUART1rxW;
volatile char *pBuffUART1rxR;
volatile char PacketReadyUART1;
//TX.
volatile char buffUART1tx[BUFFUART1TX_DIM];
volatile char *pBuffUART1txW;

#define MSUART1RXTIMEOUT msUART1rxTimeOut = 3 //300ms
unsigned char msUART1rxTimeOut = 0;

//--- USART2 ---//
#define BUFFUART2RX_DIM 256
#define BUFFUART2TX_DIM BUFFUART2RX_DIM
volatile char buffUART2rx[BUFFUART2RX_DIM];
volatile char buffUART2rx2[BUFFUART2RX_DIM];
volatile char *pBuffUART2rxW;
volatile char *pBuffUART2rxR;
volatile char PacketReadyUART2;
//TX.
volatile char buffUART2tx[BUFFUART2TX_DIM];
volatile char *pBuffUART2txW;

#define MSUART2RXTIMEOUT msUART2rxTimeOut = 3 //300ms
unsigned char msUART2rxTimeOut = 0;

//--- USART3 ---//
#define BUFFUART3RX_DIM 256
#define BUFFUART3TX_DIM BUFFUART3RX_DIM
volatile char buffUART3rx[BUFFUART3RX_DIM];
volatile char buffUART3rx2[BUFFUART3RX_DIM];
volatile char *pBuffUART3rxW;
volatile char *pBuffUART3rxR;
volatile char PacketReadyUART3;
//TX.
volatile char buffUART3tx[BUFFUART3TX_DIM];
volatile char *pBuffUART3txW;

#define MSUART3RXTIMEOUT msUART3rxTimeOut = 3 //300ms
unsigned char msUART3rxTimeOut = 0;

//--- USART4 ---//
#define BUFFUART4RX_DIM 256
#define BUFFUART4TX_DIM BUFFUART4RX_DIM
volatile char buffUART4rx[BUFFUART4RX_DIM];
volatile char buffUART4rx2[BUFFUART4RX_DIM];
volatile char *pBuffUART4rxW;
volatile char *pBuffUART4rxR;
volatile char PacketReadyUART4;
//TX.
volatile char buffUART4tx[BUFFUART4TX_DIM];
volatile char *pBuffUART4txW;

#define MSUART4RXTIMEOUT msUART4rxTimeOut = 3 //300ms
unsigned char msUART4rxTimeOut = 0;

//--- USART5 ---//
#define BUFFUART5RX_DIM 256
#define BUFFUART5TX_DIM BUFFUART5RX_DIM
volatile char buffUART5rx[BUFFUART5RX_DIM];
volatile char buffUART5rx2[BUFFUART5RX_DIM];
volatile char *pBuffUART5rxW;
volatile char *pBuffUART5rxR;
//volatile char counterUART5;
volatile char PacketReadyUART5;
//TX.
volatile char buffUART5tx[BUFFUART5TX_DIM];
volatile char *pBuffUART5txW;

#define MSUART5RXTIMEOUT msUART5rxTimeOut = 3 //300ms
unsigned char msUART5rxTimeOut = 0;

void UART_Tim6 (void)
{

	if (msUART2rxTimeOut)
		msUART2rxTimeOut--;

	if (msUART4rxTimeOut)
		msUART4rxTimeOut--;

	if (msUART5rxTimeOut)
		msUART5rxTimeOut--;

	if (msUART3rxTimeOut)
		msUART3rxTimeOut--;

	if (msUART1rxTimeOut)
		msUART1rxTimeOut--;
}

void USART1_IRQHandler(void)
{
	//--- Recepcion ---//
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{

		//Lectura del dato recibido.
		*pBuffUART1rxW = USART_ReceiveData(USART1);

		//Carga el timeout.
		MSUART1RXTIMEOUT;

		if (*(pBuffUART1rxW) != 0)
		{
			if ((*(pBuffUART1rxW) == '\n') || (*(pBuffUART1rxW) == 26))
			{
				*(pBuffUART1rxW+1) = 0;

				//strncpy((char *)&buffUART1rx3[0], (const char *)&buffUART1rx[0], (counterUART1+1));
				strcpy((char *)&buffUART1rx2[0], (const char *)&buffUART1rx[0]);

				//PacketReadyUART1 = counterUART1;
				PacketReadyUART1 = 1;
				pBuffUART1rxW = &buffUART1rx[0];
				*pBuffUART1rxW = 0;
			}
			else
			{
				//counterUART1++;

				//-- Mueve el puntero ---//
				if (pBuffUART1rxW < &pBuffUART1rxW[BUFFUART1RX_DIM])
					pBuffUART1rxW++;
				else
					pBuffUART1rxW = &pBuffUART1rxW[0];

				//RN171UART_TimeOut = 10;
			}
		}
		USART1->SR &= ~USART_FLAG_RXNE;
	}
	//--- Transmision ---//
	if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)
	{
		//si el caracter es null o llegue al final del buffer CORTO
		if ((* pBuffUART1txW == '\0') || (pBuffUART1txW == &buffUART1tx[BUFFUART1TX_DIM]))
		{
			pBuffUART1txW = buffUART1tx;
			*pBuffUART1txW = '\0';

			USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
		}
		else
		{
			USART1->DR = (*pBuffUART1txW & 0xFF);
			pBuffUART1txW++;	//ya se que es menor, sino se hubiera cortado
		}

		USART1->SR &= ~USART_IT_TXE;
	}
}

void USART2_IRQHandler(void)
{
	//--- Recepcion ---//
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{

		//Lectura del dato recibido.
		*pBuffUART2rxW = USART_ReceiveData(USART2);

		//Carga el timeout.
		MSUART2RXTIMEOUT;

		if (*(pBuffUART2rxW) != 0)
		{
			if ((*(pBuffUART2rxW) == '\n') || (*(pBuffUART2rxW) == 26))
			{
				*(pBuffUART2rxW+1) = 0;

				//strncpy((char *)&buffUART2rx3[0], (const char *)&buffUART2rx[0], (counterUART2+1));
				strcpy((char *)&buffUART2rx2[0], (const char *)&buffUART2rx[0]);

				//PacketReadyUART2 = counterUART2;
				PacketReadyUART2 = 1;
				pBuffUART2rxW = &buffUART2rx[0];
				*pBuffUART2rxW = 0;
			}
			else
			{
				//-- Mueve el puntero ---//
				if (pBuffUART2rxW < &pBuffUART2rxW[BUFFUART2RX_DIM])
					pBuffUART2rxW++;
				else
					pBuffUART2rxW = &pBuffUART2rxW[0];

				//RN171UART_TimeOut = 10;
			}
		}
		USART2->SR &= ~USART_FLAG_RXNE;
	}
	//--- Transmision ---//
	if(USART_GetITStatus(USART2, USART_IT_TXE) != RESET)
	{
		//si el caracter es null o llegue al final del buffer CORTO
		if ((* pBuffUART2txW == '\0') || (pBuffUART2txW == &buffUART2tx[BUFFUART2TX_DIM]))
		{
			pBuffUART2txW = buffUART2tx;
			*pBuffUART2txW = '\0';

			USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
		}
		else
		{
			USART2->DR = (*pBuffUART2txW & 0xFF);
			pBuffUART2txW++;	//ya se que es menor, sino se hubiera cortado
		}

		USART2->SR &= ~USART_IT_TXE;
	}
}

void UART4_IRQHandler(void)
{
	//--- Recepcion ---//
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)
	{

		//Lectura del dato recibido.
		*pBuffUART4rxW = USART_ReceiveData(UART4);

		//Carga el timeout.
		MSUART4RXTIMEOUT;

		if (*(pBuffUART4rxW) != 0)
		{
			if ((*(pBuffUART4rxW) == '\n') || (*(pBuffUART4rxW) == 26))
			{
				*(pBuffUART4rxW+1) = 0;

				//strncpy((char *)&buffUART4rx3[0], (const char *)&buffUART4rx[0], (counterUART4+1));
				strcpy((char *)&buffUART4rx2[0], (const char *)&buffUART4rx[0]);

				//PacketReadyUART4 = counterUART4;
				PacketReadyUART4 = 1;
				pBuffUART4rxW = &buffUART4rx[0];
				*pBuffUART4rxW = 0;
			}
			else
			{
				//counterUART4++;

				//-- Mueve el puntero ---//
				if (pBuffUART4rxW < &pBuffUART4rxW[BUFFUART4RX_DIM])
					pBuffUART4rxW++;
				else
					pBuffUART4rxW = &pBuffUART4rxW[0];

				//RN171UART_TimeOut = 10;
			}
		}
		UART4->SR &= ~USART_FLAG_RXNE;
	}
	//--- Transmision ---//
	if(USART_GetITStatus(UART4, USART_IT_TXE) != RESET)
	{
		//si el caracter es null o llegue al final del buffer CORTO
		if ((* pBuffUART4txW == '\0') || (pBuffUART4txW == &buffUART4tx[BUFFUART4TX_DIM]))
		{
			pBuffUART4txW = buffUART4tx;
			*pBuffUART4txW = '\0';

			USART_ITConfig(UART4, USART_IT_TXE, DISABLE);
		}
		else
		{
			UART4->DR = (*pBuffUART4txW & 0xFF);
			pBuffUART4txW++;	//ya se que es menor, sino se hubiera cortado
		}

		UART4->SR &= ~USART_IT_TXE;
	}
}

void UART5_IRQHandler(void)
{
	//--- Recepcion ---//
	if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)
	{

		//Lectura del dato recibido.
		*pBuffUART5rxW = USART_ReceiveData(UART5);

		//Carga el timeout.
		MSUART5RXTIMEOUT;

		if (*(pBuffUART5rxW) != 0)
		{
			if ((*(pBuffUART5rxW) == '\n') || (*(pBuffUART5rxW) == 26))
			{
				*(pBuffUART5rxW+1) = 0;

				//strncpy((char *)&buffUART5rx3[0], (const char *)&buffUART5rx[0], (counterUART5+1));
				strcpy((char *)&buffUART5rx2[0], (const char *)&buffUART5rx[0]);

				//PacketReadyUART5 = counterUART5;
				PacketReadyUART5 = 1;
				pBuffUART5rxW = &buffUART5rx[0];
				*pBuffUART5rxW = 0;
			}
			else
			{
				//counterUART5++;

				//-- Mueve el puntero ---//
				if (pBuffUART5rxW < &pBuffUART5rxW[BUFFUART5RX_DIM])
					pBuffUART5rxW++;
				else
					pBuffUART5rxW = &pBuffUART5rxW[0];

				//RN171UART_TimeOut = 10;
			}
		}
		UART5->SR &= ~USART_FLAG_RXNE;
	}
	//--- Transmision ---//
	if(USART_GetITStatus(UART5, USART_IT_TXE) != RESET)
	{
		//si el caracter es null o llegue al final del buffer CORTO
		if ((* pBuffUART5txW == '\0') || (pBuffUART5txW == &buffUART5tx[BUFFUART5TX_DIM]))
		{
			pBuffUART5txW = buffUART5tx;
			*pBuffUART5txW = '\0';

			USART_ITConfig(UART5, USART_IT_TXE, DISABLE);
		}
		else
		{
			UART5->DR = (*pBuffUART5txW & 0xFF);
			pBuffUART5txW++;	//ya se que es menor, sino se hubiera cortado
		}

		UART5->SR &= ~USART_IT_TXE;
	}
}
void USART3_IRQHandler(void)
{
	//--- Recepcion ---//
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
	{

		//Lectura del dato recibido.
		*pBuffUART3rxW = USART_ReceiveData(USART3);

		//Carga el timeout.
		MSUART3RXTIMEOUT;

		if (*(pBuffUART3rxW) != 0)
		{
			if ((*(pBuffUART3rxW) == '\n') || (*(pBuffUART3rxW) == 26))
			{
				*(pBuffUART3rxW+1) = 0;

				//strncpy((char *)&buffUART3rx3[0], (const char *)&buffUART3rx[0], (counterUART3+1));
				strcpy((char *)&buffUART3rx2[0], (const char *)&buffUART3rx[0]);

				//PacketReadyUART3 = counterUART3;
				PacketReadyUART3 = 1;
				pBuffUART3rxW = &buffUART3rx[0];
				*pBuffUART3rxW = 0;
			}
			else
			{
				//counterUART3++;

				//-- Mueve el puntero ---//
				if (pBuffUART3rxW < &pBuffUART3rxW[BUFFUART3RX_DIM])
					pBuffUART3rxW++;
				else
					pBuffUART3rxW = &pBuffUART3rxW[0];

				//RN171UART_TimeOut = 10;
			}
		}
		USART3->SR &= ~USART_FLAG_RXNE;
	}
	//--- Transmision ---//
	if(USART_GetITStatus(USART3, USART_IT_TXE) != RESET)
	{
		//si el caracter es null o llegue al final del buffer CORTO
		if ((* pBuffUART3txW == '\0') || (pBuffUART3txW == &buffUART3tx[BUFFUART3TX_DIM]))
		{
			pBuffUART3txW = buffUART3tx;
			*pBuffUART3txW = '\0';

			USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
		}
		else
		{
			USART3->DR = (*pBuffUART3txW & 0xFF);
			pBuffUART3txW++;	//ya se que es menor, sino se hubiera cortado
		}

		USART3->SR &= ~USART_IT_TXE;
	}
}

void UART1_Config(void){

	unsigned long temp;
	NVIC_InitTypeDef NVIC_InitStructure;

	//---- Clk USART1 ----//
	if (!RCC_USART1_CLK)
		RCC_USART1_CLKEN;

	if (!RCC_GPIOA_clk)
		RCC_GPIOA_clkEnable;

	//----GPIOA----//
	//----TX:PA9 RX:PA10----//
	temp = GPIOA->CRH;
	temp &= 0xFFFFF00F;
	temp |= 0x000004B0;
	GPIOA->CRH = temp;

	//Configuracion NVIC.
	//Interrupcion UART.
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	//buffer DEBUG.
	//RX.
	pBuffUART1rxW = &buffUART1rx[0];
	pBuffUART1rxR = &buffUART1rx[0];
	PacketReadyUART1 = 0;
	//TX.
//	pBuffUART1txR = &buffUART1tx[0];
	pBuffUART1txW = &buffUART1tx[0];

	while (pBuffUART1rxW == &buffUART1rx[BUFFUART1RX_DIM])
	{
		*pBuffUART1rxW = 0;
		pBuffUART1rxW++;
	}

	pBuffUART1rxW = &buffUART1rx[0];

	while (pBuffUART1txW == &buffUART1tx[BUFFUART1TX_DIM])
	{
		*pBuffUART1txW = 0;
		pBuffUART1txW++;
	}

	pBuffUART1txW = &buffUART1tx[0];

	USART1->BRR = 0x1D4C;	//baud: 9600. PCLK1 @ 72MHz
	//USART1->BRR = 0x0EA6; //baud: 9600. PCLK1 @ 36MHz
	USART1->CR1 = 0x202C; //8N1
}

void UART2_Config(void){

	unsigned long temp;
	NVIC_InitTypeDef NVIC_InitStructure;

	//---- Clk USART2 ----//
	if (!RCC_USART2_CLK)
		RCC_USART2_CLKEN;

	if (!RCC_GPIOA_clk)
		RCC_GPIOA_clkEnable;

	//----GPIOA----//
	//----TX:PA2 RX:PA3----//
	temp = GPIOA->CRL;
	temp &= 0xFFFF00FF;
	temp |= 0x00004B00;
	GPIOA->CRL = temp;

	//Configuracion NVIC.
	//Interrupcion UART.
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	//buffer DEBUG.
	//RX.
	pBuffUART2rxW = &buffUART2rx[0];
	pBuffUART2rxR = &buffUART2rx[0];
	PacketReadyUART2 = 0;
	//TX.
	pBuffUART2txW = &buffUART2tx[0];

	while (pBuffUART2rxW == &buffUART2rx[BUFFUART2RX_DIM])
	{
		*pBuffUART2rxW = 0;
		pBuffUART2rxW++;
	}

	pBuffUART2rxW = &buffUART2rx[0];

	while (pBuffUART2txW == &buffUART2tx[BUFFUART2TX_DIM])
	{
		*pBuffUART2txW = 0;
		pBuffUART2txW++;
	}

	pBuffUART2txW = &buffUART2tx[0];

	USART2->BRR = 0x0EA6; //baud: 9600.
	USART2->CR1 = 0x202C; //8N1
}


void UART3_Config(void){

	unsigned long temp;
	NVIC_InitTypeDef NVIC_InitStructure;

	//---- Clk USART3 ----//
	if (!(RCC->APB1ENR & 0x00040000))
		RCC->APB1ENR |= 0x00040000;

	if (!(RCC->APB2ENR & 0x00000008))
		RCC->APB2ENR |= 0x00000008;

	//----GPIOB----//
	//----TX:PB10 RX:PB11----//
	temp = GPIOB->CRH;
	temp &= 0xFFFF00FF;
	temp |= 0x00004B00;
	GPIOB->CRH = temp;

	//Configuracion NVIC.
	//Interrupcion UART.
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	//buffer DEBUG.
	//RX.
	pBuffUART3rxW = &buffUART3rx[0];
	pBuffUART3rxR = &buffUART3rx[0];
	PacketReadyUART3 = 0;
	//TX.
	pBuffUART3txW = &buffUART3tx[0];

	while (pBuffUART3rxW == &buffUART3rx[BUFFUART3RX_DIM])
	{
		*pBuffUART3rxW = 0;
		pBuffUART3rxW++;
	}

	pBuffUART3rxW = &buffUART3rx[0];

	while (pBuffUART3txW == &buffUART3tx[BUFFUART3TX_DIM])
	{
		*pBuffUART3txW = 0;
		pBuffUART3txW++;
	}

	pBuffUART3txW = &buffUART3tx[0];

	USART3->BRR = 0x0EA6; //baud: 9600.
	USART3->CR1 = 0x202C; //8N1

}

void UART4_Config(void){

	unsigned long temp;
	NVIC_InitTypeDef NVIC_InitStructure;

	//---- Clk USART1 ----//
	if (!RCC_UART4_CLK)
		RCC_UART4_CLKEN;

	if (!RCC_GPIOC_clk)
		RCC_GPIOC_clkEnable;

	//----GPIOA----//
	//----TX:PC10 RX:PC11----//
	temp = GPIOC->CRH;
	temp &= 0xFFFF00FF;
	temp |= 0x00004B00;
	GPIOC->CRH = temp;

	//Configuracion NVIC.
	//Interrupcion UART.
	NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	//buffer DEBUG.
	//RX.
	pBuffUART4rxW = &buffUART4rx[0];
	pBuffUART4rxR = &buffUART4rx[0];
	PacketReadyUART4 = 0;
	//TX.
	pBuffUART4txW = &buffUART4tx[0];

	while (pBuffUART4rxW == &buffUART4rx[BUFFUART4RX_DIM])
	{
		*pBuffUART4rxW = 0;
		pBuffUART4rxW++;
	}

	pBuffUART4rxW = &buffUART4rx[0];

	while (pBuffUART4txW == &buffUART4tx[BUFFUART4TX_DIM])
	{
		*pBuffUART4txW = 0;
		pBuffUART4txW++;
	}

	pBuffUART4txW = &buffUART4tx[0];

	UART4->BRR = 0x0EA6; //baud: 9600.
	UART4->CR1 = 0x202C; //8N1
}

void UART5_Config(void){

	unsigned long temp;
	NVIC_InitTypeDef NVIC_InitStructure;

	//---- Clk USART1 ----//
	if (!RCC_UART5_CLK)
		RCC_UART5_CLKEN;

	if (!RCC_GPIOC_clk)
		RCC_GPIOC_clkEnable;

	if (!RCC_GPIOD_clk)
		RCC_GPIOD_clkEnable;


	//----GPIOC/D----//
	//----TX:PC12 RX:PD3----//
	temp = GPIOC->CRH;
	temp &= 0xFFF0FFFF;
	temp |= 0x000B0000;
	GPIOC->CRH = temp;

	temp = GPIOD->CRL;
	temp &= 0xFFFFF0FF;
	temp |= 0x00000400;
	GPIOD->CRL = temp;

	//Configuracion NVIC.
	//Interrupcion UART.
	NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	//buffer DEBUG.
	//RX.
	pBuffUART5rxW = &buffUART5rx[0];
	pBuffUART5rxR = &buffUART5rx[0];
	PacketReadyUART5 = 0;
	//TX.
	pBuffUART5txW = &buffUART5tx[0];

	while (pBuffUART5rxW == &buffUART5rx[BUFFUART5RX_DIM])
	{
		*pBuffUART5rxW = 0;
		pBuffUART5rxW++;
	}

	pBuffUART5rxW = &buffUART5rx[0];

	while (pBuffUART5txW == &buffUART5tx[BUFFUART5TX_DIM])
	{
		*pBuffUART5txW = 0;
		pBuffUART5txW++;
	}

	pBuffUART5txW = &buffUART5tx[0];

	UART5->BRR = 0x0EA6; //baud: 9600.
	UART5->CR1 = 0x202C; //8N1
}

void UART1Send(char * ptrSend)
{
	unsigned short datos = 0;

	datos = strlen (ptrSend);

	if (datos < BUFFUART1TX_DIM)
	{
//		USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
		if ((pBuffUART1txW + datos + 1) < &buffUART1tx[BUFFUART1TX_DIM])
		{
			strcat((char *)pBuffUART1txW, ptrSend);
		}
		else
			pBuffUART1txW  = buffUART1tx;

		USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
	}
}

void UART2Send(char * ptrSend)
{
	unsigned char datos = strlen ((const char *) &ptrSend[0]);

	if (datos < BUFFUART2TX_DIM)
	{
		if ((pBuffUART2txW + datos + 1) < &buffUART2tx[BUFFUART2TX_DIM])
		{
			strcat((char *)pBuffUART2txW, ptrSend);
		}
		else
			pBuffUART2txW  = buffUART2tx;

		USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
	}
}

void UART3Send(char * ptrSend)
{
	unsigned char datos = strlen ((const char *) &ptrSend[0]);

	if (datos < BUFFUART3TX_DIM)
	{
		if ((pBuffUART3txW + datos + 1) < &buffUART3tx[BUFFUART3TX_DIM])
		{
			strcat((char *)pBuffUART3txW, ptrSend);
		}
		else
			pBuffUART3txW  = buffUART3tx;

		USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
	}
}

void UART4Send(char * ptrSend)
{
	unsigned char datos = strlen ((const char *) &ptrSend[0]);

	if (datos < BUFFUART4TX_DIM)
	{
		if ((pBuffUART4txW + datos + 1) < &buffUART4tx[BUFFUART4TX_DIM])
		{
			strcat((char *)pBuffUART4txW, ptrSend);
		}
		else
			pBuffUART4txW  = buffUART4tx;

		USART_ITConfig(UART4, USART_IT_TXE, ENABLE);
	}
}

void UART5Send(char * ptrSend)
{
	unsigned char datos = strlen ((const char *) &ptrSend[0]);

	if (datos < BUFFUART5TX_DIM)
	{
		if ((pBuffUART5txW + datos) < &buffUART5tx[BUFFUART5TX_DIM])
		{
			strcat((char *)pBuffUART5txW, ptrSend);
		}
		else
			pBuffUART5txW  = buffUART5tx;

		USART_ITConfig(UART5, USART_IT_TXE, ENABLE);
	}
}

//MODIFICADO MAGNETO V2.0 28-02-15 ANTES ERA USART3
//--- Recepcion de la PC
void UART1_Receive (void)
{
	unsigned char i;
	antenna_typedef 	antenna;
	signal_typedef 		signal;
	duration_typedef 	duration;
	//char buf[10];

	if (msUART1rxTimeOut == 0)
	{

		MSUART1RXTIMEOUT;

		pBuffUART1rxW = &buffUART1rx[0];
		pBuffUART1rxR = &buffUART1rx[0];
	}

	if (PacketReadyUART1)
	{
		//--- Introduce your code here ---//
		UART1Send((char *)&buffUART1rx2[0]);

		if (!strncmp((const char *)&buffUART1rx2[0], (const char *)"get_temp,", (sizeof("get_temp,") - 1)))
		{
			switch ((buffUART1rx2[9] - 48))
			{
				case 1:
					UART_CH1_Send("get_temp\r\n");
					UART_PC_Send("OK\r\n");
					break;

				case 2:
					UART_CH2_Send("get_temp\r\n");
					UART_PC_Send("OK\r\n");
					break;

				case 3:
					UART_CH3_Send("get_temp\r\n");
					UART_PC_Send("OK\r\n");
					break;

				case 4:
					UART_CH4_Send("get_temp\r\n");
					UART_PC_Send("OK\r\n");
					break;

				default:
					UART_PC_Send("ERROR\r\n");
					break;
			}
		}

		else if (!strncmp((const char *)&buffUART1rx2[0], (const char *)"get_antenna_con,", (sizeof("get_antenna_con,") - 1)))
		{
			//0 si no tengo, 1 si esta 1, 2 si est 2 "ant_con,0,2,0,0\r\n"
			/*
			sprintf((const char *)&buffUART1rx2[0], "ant_con,0,2,0,0\r\n",
						antenna.resistance_int,
						antenna.resistance_dec,
						antenna.inductance_int,
						antenna.inductance_dec,
						antenna.current_limit_int,
						antenna.current_limit_dec,
						antenna.temp_max_int,
						antenna.temp_max_dec);
UART_PC_Send(buffUART1rx2);
buffUART1rx2[0] = '\0';
			*/
			UART_PC_Send((char *) "ant_con,0,2,0,0\r\n");
			buffUART1rx2[0] = '\0';
		}

		else if (!strncmp((const char *)&buffUART1rx2[0], (const char *)"get_antenna,", (sizeof("get_antenna,") - 1)))
		{
			//--- Get antenna parameters ---//
			Session_Get_Antenna (&session_ch_1, 1, &antenna);
			//strcpy((const char *)&buffUART1rx2[0], (const char *)"ant0,");
			//sprintf((const char *)&buffUART1rx2[0], "ant0,%03d.%02d,%03d.%02d,%03d.%02d,%03d.%02d,1\r\n",
			sprintf((char *)&buffUART1rx2[0], "ant0,%03d.%02d,%03d.%02d,%03d.%02d,%03d.%02d,1\r\n",
						antenna.resistance_int,
						antenna.resistance_dec,
						antenna.inductance_int,
						antenna.inductance_dec,
						antenna.current_limit_int,
						antenna.current_limit_dec,
						antenna.temp_max_int,
						antenna.temp_max_dec);

			//UART_PC_Send((const char *) buffUART1rx2);
			UART_PC_Send((char *) buffUART1rx2);
			buffUART1rx2[0] = '\0';

			Session_Get_Antenna (&session_ch_2, 1, &antenna);
			//strcpy((const char *)&buffUART1rx2[0], (const char *)"ant0,");
			sprintf((char *)&buffUART1rx2[0], "ant0,%03d.%02d,%03d.%02d,%03d.%02d,%03d.%02d,2\r\n",
						antenna.resistance_int,
						antenna.resistance_dec,
						antenna.inductance_int,
						antenna.inductance_dec,
						antenna.current_limit_int,
						antenna.current_limit_dec,
						antenna.temp_max_int,
						antenna.temp_max_dec);

			//UART_PC_Send((const char *) buffUART1rx2);
			UART_PC_Send((char *) buffUART1rx2);
			buffUART1rx2[0] = '\0';

			Session_Get_Antenna (&session_ch_3, 1, &antenna);
			//strcpy((const char *)&buffUART1rx2[0], (const char *)"ant0,");
			sprintf((char *)&buffUART1rx2[0], "ant0,%03d.%02d,%03d.%02d,%03d.%02d,%03d.%02d,3\r\n",
						antenna.resistance_int,
						antenna.resistance_dec,
						antenna.inductance_int,
						antenna.inductance_dec,
						antenna.current_limit_int,
						antenna.current_limit_dec,
						antenna.temp_max_int,
						antenna.temp_max_dec);

			UART_PC_Send((char *) buffUART1rx2);
			buffUART1rx2[0] = '\0';

			Session_Get_Antenna (&session_ch_4, 1, &antenna);
			//strcpy((const char *)&buffUART1rx2[0], (const char *)"ant0,");
			sprintf((char *)&buffUART1rx2[0], "ant0,%03d.%02d,%03d.%02d,%03d.%02d,%03d.%02d,4\r\n",
						antenna.resistance_int,
						antenna.resistance_dec,
						antenna.inductance_int,
						antenna.inductance_dec,
						antenna.current_limit_int,
						antenna.current_limit_dec,
						antenna.temp_max_int,
						antenna.temp_max_dec);

			UART_PC_Send((char *) buffUART1rx2);
			buffUART1rx2[0] = '\0';

			if (Get_Antenna_Name(CH1, (char *) &buffUART1rx2[0]) != 0)
			{
				UART_PC_Send((char *) (const char *) "ant_name,1,");
				UART_PC_Send((char *) buffUART1rx2);
				UART_PC_Send((char *) (const char *) "\r\n");
			}

			if (Get_Antenna_Name(CH2, (char *) &buffUART1rx2[0]) != 0)
			{
				UART_PC_Send((char *) (const char *) "ant_name,2,");
				UART_PC_Send((char *) buffUART1rx2);
				UART_PC_Send((char *) (const char *) "\r\n");
			}

			if (Get_Antenna_Name(CH3, (char *) &buffUART1rx2[0]) != 0)
			{
				UART_PC_Send((char *) (const char *) "ant_name,3,");
				UART_PC_Send((char *) buffUART1rx2);
				UART_PC_Send((char *) (const char *) "\r\n");
			}

			if (Get_Antenna_Name(CH4, (char *) &buffUART1rx2[0]) != 0)
			{
				UART_PC_Send((char *) (const char *) "ant_name,4,");
				UART_PC_Send((char *) buffUART1rx2);
				UART_PC_Send((char *) (const char *) "\r\n");
			}

			buffUART1rx2[0] = '\0';
		}

		else if (!strncmp((const char *)&buffUART1rx2[0], (const char *)"antenna,", (sizeof("antenna,") - 1)))
		{

			if (((buffUART1rx2[28] - 48) > 0) && ((buffUART1rx2[28] - 48) < 4) && (buffUART1rx2[27] == ','))
			{
				antenna.resistance_int = 	(buffUART1rx2[8] - 48) * 100 + (buffUART1rx2[9] - 48) * 10 + (buffUART1rx2[10] - 48);
				antenna.resistance_dec = 	(buffUART1rx2[12] - 48) * 10 + (buffUART1rx2[13] - 48);
				antenna.inductance_int = 	(buffUART1rx2[15] - 48) * 100 + (buffUART1rx2[16] - 48) * 10 + (buffUART1rx2[17] - 48);
				antenna.inductance_dec = 	(buffUART1rx2[19] - 48) * 10 + (buffUART1rx2[20] - 48);
				antenna.current_limit_int = (buffUART1rx2[22] - 48) * 10 + (buffUART1rx2[23] - 48);
				antenna.current_limit_dec = (buffUART1rx2[25] - 48) * 10 + (buffUART1rx2[26] - 48);

				//--- Save antenna parameters ---//
				Session_Set_Antenna (&session_slot_aux, (buffUART1rx2[28] - 48), &antenna);

				UART_PC_Send((char *)"OK\r\n");
			}
			else
				UART_PC_Send((char *)"ERROR\r\n");
		}

		else if (!strncmp((const char *)&buffUART1rx2[0], (const char *)"signal,", (sizeof("signal,") - 1)))
		{
			//signal,%03d,%03d,0%x%x%d,%04d,%04d,%04d,%04d,%04d,%04d,1\r\n",
			if (((buffUART1rx2[50] - 48) > 0) && ((buffUART1rx2[50] - 48) < 4) && (buffUART1rx2[49] == ','))
			{
				signal.initial_power = 		(buffUART1rx2[7] - 48) * 100 + (buffUART1rx2[8] - 48) * 10 + (buffUART1rx2[9] - 48);
				signal.final_power = 		(buffUART1rx2[11] - 48) * 100 + (buffUART1rx2[12] - 48) * 10 + (buffUART1rx2[13] - 48);
				signal.sync_on =  			(buffUART1rx2[18] - 48);
				//signal.step_number = 		(buffUART1rx2[15] - 48) * 1000 + (buffUART1rx2[16] - 48) * 100 + (buffUART1rx2[17] - 48) * 10 + (buffUART1rx2[18] - 48);
				signal.rising_time = 		(buffUART1rx2[20] - 48) * 1000 + (buffUART1rx2[21] - 48) * 100 + (buffUART1rx2[22] - 48) * 10 + (buffUART1rx2[23] - 48);
				signal.maintenance_time = 	(buffUART1rx2[25] - 48) * 1000 + (buffUART1rx2[26] - 48) * 100 + (buffUART1rx2[27] - 48) * 10 + (buffUART1rx2[28] - 48);
				signal.falling_time = 		(buffUART1rx2[30] - 48) * 1000 + (buffUART1rx2[31] - 48) * 100 + (buffUART1rx2[32] - 48) * 10 + (buffUART1rx2[33] - 48);
				signal.low_time = 			(buffUART1rx2[35] - 48) * 1000 + (buffUART1rx2[36] - 48) * 100 + (buffUART1rx2[37] - 48) * 10 + (buffUART1rx2[38] - 48);
				signal.burst_mode_on = 		(buffUART1rx2[40] - 48) * 1000 + (buffUART1rx2[41] - 48) * 100 + (buffUART1rx2[42] - 48) * 10 + (buffUART1rx2[43] - 48);
				signal.burst_mode_off = 	(buffUART1rx2[45] - 48) * 1000 + (buffUART1rx2[46] - 48) * 100 + (buffUART1rx2[47] - 48) * 10 + (buffUART1rx2[48] - 48);
				Session_Set_Signal (&session_slot_aux, (buffUART1rx2[50] - 48), &signal);

				UART_PC_Send((char *)"OK\r\n");
			}
			else
				UART_PC_Send((char *)"ERROR\r\n");

		}

		else if (!strncmp((const char *)&buffUART1rx2[0], (const char *)"duration,", (sizeof("duration,") - 1)))
		{

			if (((buffUART1rx2[18] - 48) > 0) && ((buffUART1rx2[18] - 48) < 4) && (buffUART1rx2[17] == ','))
			{
				duration.hours =  	(buffUART1rx2[9] - 48) * 10 + (buffUART1rx2[10] - 48);
				duration.minutes =	(buffUART1rx2[12] - 48) * 10 + (buffUART1rx2[13] - 48);
				duration.seconds =	(buffUART1rx2[15] - 48) * 10 + (buffUART1rx2[16] - 48);

				Session_Set_Duration (&session_slot_aux, (buffUART1rx2[18] - 48), &duration);

				UART_PC_Send((char *)"OK\r\n");
			}
			else
				UART_PC_Send((char *)"ERROR\r\n");
		}

		else if (!strncmp((const char *)&buffUART1rx2[0], (const char *)"save,", (sizeof("save,") - 1)))
		{

			if (((buffUART1rx2[5] - 48) < 2) && (buffUART1rx2[6] - 48 < 10))
			{
				//Session_Save (&session_slot_aux, buffUART3rx2[5] - 48);
				i = FLASH_Program(&session_slot_aux, ((buffUART1rx2[5] - 48) * 10 + (buffUART1rx2[6] - 48)));

				if (i == FIN_OK)
				{
					UART_PC_Send("Guardado OK\r\n");
					Wait_ms(1000);
					//Reset.
					NVIC_SystemReset();
				}

				if (i == FIN_ERROR)
				{
					UART_PC_Send("Guardado ERROR\r\n");
					Wait_ms(1000);
					//Reset.
					NVIC_SystemReset();
				}


				UART_PC_Send((char *)"OK\r\n");
			}
			else
				UART_PC_Send((char *)"ERROR\r\n");
		}

		else if (!strncmp((const char *)&buffUART1rx2[0], (const char *)"reset", (sizeof("reset") - 1)))
		{
			UART_PC_Send("Restarting...\r\n");
			Wait_ms(1000);
			NVIC_SystemReset();
		}

		else if (!strncmp((const char *)&buffUART1rx2[0], (const char *)"pause,", (sizeof("pause,") - 1)))
		{
			if (buffUART1rx2[6] - 48)
			{
				channel_1_pause = 1;
				channel_2_pause = 1;
				channel_3_pause = 1;
				channel_4_pause = 1;
			}
			else
			{
				channel_1_pause = 0;
				channel_2_pause = 0;
				channel_3_pause = 0;
				channel_4_pause = 0;
			}
/*			switch(buffUART3rx2[6] - 48)
			{
				case 1:
					if (buffUART3rx2[8] - 48)
						channel_1_pause = 1;
					else
						channel_1_pause = 0;

					break;

				case 2:
					if (buffUART3rx2[8] - 48)
						channel_2_pause = 1;
					else
						channel_2_pause = 0;
					break;

				case 3:
					if (buffUART3rx2[8] - 48)
						channel_3_pause = 1;
					else
						channel_3_pause = 0;
					break;

				case 4:
					if (buffUART3rx2[8] - 48)
						channel_4_pause = 1;
					else
						channel_4_pause = 0;
					break;

				default:
					UART3Send((char *)"ERROR\r\n");
					break;
			}
			*/
		}

		else if (!strncmp((const char *)&buffUART1rx2[0], (const char *)"load,", (sizeof("load,") - 1)))
		{
			//load,slot[2],destino[1]
			if (((buffUART1rx2[6] - 48) < 10) && ((buffUART1rx2[5] - 48) < 2))
			{
				Session_Load (&session_slot_aux, (((buffUART1rx2[5] - 48) * 10) + buffUART1rx2[6] - 48), 0);
				Session_Load (&session_slot_aux, (((buffUART1rx2[5] - 48) * 10) + buffUART1rx2[6] - 48), 1);
				Session_Load (&session_slot_aux, (((buffUART1rx2[5] - 48) * 10) + buffUART1rx2[6] - 48), 2);
				Session_Load (&session_slot_aux, (((buffUART1rx2[5] - 48) * 10) + buffUART1rx2[6] - 48), 3);
				Session_Load (&session_slot_aux, (((buffUART1rx2[5] - 48) * 10) + buffUART1rx2[6] - 48), 4);
				UART_PC_Send((char *)"OK\r\n");
			}
			else
				UART_PC_Send((char *)"ERROR\r\n");
/*			if (((buffUART3rx2[6] - 48) < 10) && ((buffUART3rx2[5] - 48) < 2) && ((buffUART3rx2[8] - 48) >= 0) && ((buffUART3rx2[8] - 48) < 5))
			{
				Session_Load (&session_slot_aux, (((buffUART3rx2[5] - 48) * 10) + buffUART3rx2[6] - 48), (buffUART3rx2[8] - 48));
				UART3Send((char *)"OK\r\n");
			}
			else
				UART3Send((char *)"ERROR\r\n");
*/
		}

		else if (!strncmp((const char *)&buffUART1rx2[0], (const char *)"state_of_stage,", (sizeof("state_of_stage,") - 1)))
		{

			if (((buffUART1rx2[15] - 48) == 0) || ((buffUART1rx2[15] - 48) == 1))
			{

				Session_Set_Status (&session_slot_aux, (buffUART1rx2[17] - 48),(buffUART1rx2[15] - 48));

				UART_PC_Send((char *)"OK\r\n");
			}
			else
				UART_PC_Send((char *)"ERROR\r\n");

		}

		else if (!strncmp((const char *)&buffUART1rx2[0], (const char *)"special_function,", (sizeof("special_function,") - 1)))
		{

			UART_PC_Send((char *)"OK\r\n");
		}

		else if (!strncmp((const char *)&buffUART1rx2[0], (const char *)"start,", (sizeof("start,") - 1)))
		{
/*			if ((buffUART3rx2[6] - 48) == 1)
				Session_Channel_1_Start();
			if ((buffUART3rx2[6] - 48) == 2)
				Session_Channel_2_Start();
			if ((buffUART3rx2[6] - 48) == 3)
				Session_Channel_3_Start();
			if ((buffUART3rx2[6] - 48) == 4)
				Session_Channel_4_Start();
*/
			Session_Channel_1_Start();
			Session_Channel_2_Start();
			Session_Channel_3_Start();
			Session_Channel_4_Start();

			ResetCheckGlobalErrors ();
			Current_Limit_Counter_Reset ();

			UART_PC_Send((char *)"OK\r\n");
		}

		else if (!strncmp((const char *)&buffUART1rx2[0], (const char *)"stop,", (sizeof("stop,") - 1)))
		{
/*			if((buffUART3rx2[5] - 48) == 1)
				Session_Channel_1_Stop();
			if((buffUART3rx2[5] - 48) == 2)
				Session_Channel_2_Stop();
			if((buffUART3rx2[5] - 48) == 3)
				Session_Channel_3_Stop();
			if((buffUART3rx2[5] - 48) == 4)
				Session_Channel_4_Stop();
*/
			Session_Channel_1_Stop();
			Session_Channel_2_Stop();
			Session_Channel_3_Stop();
			Session_Channel_4_Stop();

			UART_PC_Send((char *)"OK\r\n");
		}

		else if (!strncmp((const char *)&buffUART1rx2[0], (const char *)"read_channel,", (sizeof("read_channel,") - 1)))
		{
			if (((buffUART1rx2[13] - 48) < 5) && ((buffUART1rx2[13] - 48) > 0))
			{
				Channel_Load (&session_slot_aux, (buffUART1rx2[13] - 48));

				//--- Send slot content ---//
				SessionSend(&session_slot_aux);

				UART_PC_Send((char *)"OK\r\n");
			}
			else
				UART_PC_Send((char *)"ERROR\r\n");
		}

		else if (!strncmp((const char *)&buffUART1rx2[0], (const char *)"read_slot,", (sizeof("read_slot,") - 1)))
		{
			//load,slot[2],destino[1]
			if ((((buffUART1rx2[11] - 48) + (buffUART1rx2[10] - 48) * 10) < 11))
			{
				Session_Load (&session_slot_aux, (((buffUART1rx2[10] - 48) * 10) + buffUART1rx2[11] - 48), 0);

				//--- Send slot content ---//
				SessionSend(&session_slot_aux);

				UART_PC_Send((char *)"OK\r\n");
			}
			else
				UART_PC_Send((char *)"ERROR\r\n");
		}

		else
			UART_PC_Send((char *)"ERROR\r\n");
		//--- end ---//
		PacketReadyUART1 = 0;
	}


}

//--- Channel 1 ---//
void UART2_Receive (void)
{

	unsigned short aux;
	antenna_typedef antenna_aux;

	if (msUART2rxTimeOut == 0)
	{

		MSUART2RXTIMEOUT;

		pBuffUART2rxW = &buffUART2rx[0];
		pBuffUART2rxR = &buffUART2rx[0];
	}

	if (PacketReadyUART2)
	{


		//--- Introduce your code here ---//
		//UART1Send((char *)&buffUART1rx2[0]);

		//temp,055.00\r\n
		if (!strncmp((const char *)&buffUART2rx2[0], (const char *)"temp", (sizeof("temp") - 1)))
		{

			if ((buffUART2rx2[4] == ',') && (buffUART2rx2[8] == '.') && (buffUART2rx2[11] == '\r'))
			{
				aux = (buffUART2rx2[5] - 48) * 100;
				aux += (buffUART2rx2[6] - 48) * 10;
				aux += (buffUART2rx2[7] - 48);

				temp_actual_channel_1_int = aux;

				if (temp_actual_channel_1_int != 25)
					temp_actual_channel_1_int +=1;

				aux = (buffUART2rx2[9] - 48) * 10;
				aux += (buffUART2rx2[10] - 48);

				temp_actual_channel_1_dec = aux;

				strcpy((char *)&buffUART2rx2[11], (const char *)",1\r\n");
				UART_PC_Send((char *)&buffUART2rx2[0]);
				UART_CH1_Send("OK\r\n");
			}
		}

		//ant0,012.27,087.90,001.80,065.00\r\n.
		else if (!strncmp((const char *)&buffUART2rx2[0], (const char *)"ant", (sizeof("ant") - 1)))
		{

			strcpy((char *)&buffUART2rx2[32], (const char *)",1\r\n");
			UART_PC_Send((char *)&buffUART2rx2[0]);

			if ((buffUART2rx2[4] == ',') && (buffUART2rx2[11] == ',') && (buffUART2rx2[18] == ',') && (buffUART2rx2[25] == ','))
			{
				aux = (buffUART2rx2[5] - 48) * 100;
				aux += (buffUART2rx2[6] - 48) * 10;
				aux += (buffUART2rx2[7] - 48);

				antenna_aux.resistance_int = aux;

				aux = (buffUART2rx2[9] - 48) * 10;
				aux += (buffUART2rx2[10] - 48);

				antenna_aux.resistance_dec = aux;

				aux = (buffUART2rx2[12] - 48) * 100;
				aux += (buffUART2rx2[13] - 48) * 10;
				aux += (buffUART2rx2[14] - 48);

				antenna_aux.inductance_int = aux;

				aux = (buffUART2rx2[16] - 48) * 10;
				aux += (buffUART2rx2[17] - 48);

				antenna_aux.inductance_dec = aux;

				aux = (buffUART2rx2[19] - 48) * 100;
				aux += (buffUART2rx2[20] - 48) * 10;
				aux += (buffUART2rx2[21] - 48);

				antenna_aux.current_limit_int = aux;

				aux = (buffUART2rx2[23] - 48) * 10;
				aux += (buffUART2rx2[24] - 48);

				antenna_aux.current_limit_dec = aux;

				aux = (buffUART2rx2[26] - 48) * 100;
				aux += (buffUART2rx2[27] - 48) * 10;
				aux += (buffUART2rx2[28] - 48);

				antenna_aux.temp_max_int = aux;

				aux = (buffUART2rx2[30] - 48) * 10;
				aux += (buffUART2rx2[31] - 48);

				antenna_aux.temp_max_dec = aux;

				Session_Set_Antenna (&session_ch_1, aux , &antenna_aux);
				Save_Antenna_Name(CH1, (char *) buffUART2rx2);



				UART_CH1_Send((char *) "OK\r\n");
			}
			else
				UART_CH1_Send((char *) "ERROR\r\n");

		}
		//--- end ---//

		PacketReadyUART2 = 0;
	}
}

void UART3_Receive (void)
{
	unsigned short aux;
	antenna_typedef antenna_aux;

	if (msUART3rxTimeOut == 0)
	{

		MSUART3RXTIMEOUT;

		pBuffUART3rxW = &buffUART3rx[0];
		pBuffUART3rxR = &buffUART3rx[0];
	}

	if (PacketReadyUART3)
	{
		//--- Introduce your code here ---//
		//UART2Send((char *)&buffUART2rx2[0]);

		//temp,055.00\r\n
		if (!strncmp((const char *)&buffUART3rx2[0], (const char *)"temp", (sizeof("temp") - 1)))
		{
			if ((buffUART3rx2[4] == ',') && (buffUART3rx2[8] == '.') && (buffUART3rx2[11] == '\r'))
			{
				aux = (buffUART3rx2[5] - 48) * 100;
				aux += (buffUART3rx2[6] - 48) * 10;
				aux += (buffUART3rx2[7] - 48);

				temp_actual_channel_2_int = aux;

				aux = (buffUART3rx2[9] - 48) * 10;
				aux += (buffUART3rx2[10] - 48);

				temp_actual_channel_2_dec = aux;

				strcpy((char *)&buffUART3rx2[11], (const char *)",2\r\n");
				UART_PC_Send((char *)&buffUART3rx2[0]);
				UART_CH2_Send("OK\r\n");
			}
		}

		//ant0,012.27,087.90,001.80,065.00\r\n.
		else if (!strncmp((const char *)&buffUART3rx2[0], (const char *)"ant", (sizeof("ant") - 1)))
		{
			strcpy((char *)&buffUART3rx2[32], (const char *)",2\r\n");
			UART_PC_Send((char *)&buffUART3rx2[0]);

			if ((buffUART3rx2[4] == ',') && (buffUART3rx2[11] == ',') && (buffUART3rx2[18] == ',') && (buffUART3rx2[25] == ','))
			{
				aux = (buffUART3rx2[5] - 48) * 100;
				aux += (buffUART3rx2[6] - 48) * 10;
				aux += (buffUART3rx2[7] - 48);

				antenna_aux.resistance_int = aux;

				aux = (buffUART3rx2[9] - 48) * 10;
				aux += (buffUART3rx2[10] - 48);

				antenna_aux.resistance_dec = aux;

				aux = (buffUART3rx2[12] - 48) * 100;
				aux += (buffUART3rx2[13] - 48) * 10;
				aux += (buffUART3rx2[14] - 48);

				antenna_aux.inductance_int = aux;

				aux = (buffUART3rx2[16] - 48) * 10;
				aux += (buffUART3rx2[17] - 48);

				antenna_aux.inductance_dec = aux;

				aux = (buffUART3rx2[19] - 48) * 100;
				aux += (buffUART3rx2[20] - 48) * 10;
				aux += (buffUART3rx2[21] - 48);

				antenna_aux.current_limit_int = aux;

				aux = (buffUART3rx2[23] - 48) * 10;
				aux += (buffUART3rx2[24] - 48);

				antenna_aux.current_limit_dec = aux;

				aux = (buffUART3rx2[26] - 48) * 100;
				aux += (buffUART3rx2[27] - 48) * 10;
				aux += (buffUART3rx2[28] - 48);

				antenna_aux.temp_max_int = aux;

				aux = (buffUART3rx2[30] - 48) * 10;
				aux += (buffUART3rx2[31] - 48);

				antenna_aux.temp_max_dec = aux;


				Session_Set_Antenna (&session_ch_2, aux , &antenna_aux);
				Save_Antenna_Name(CH2, (char *) buffUART3rx2);

				UART_CH2_Send((char *) "OK\r\n");
			}
			else
				UART_CH2_Send((char *) "ERROR\r\n");
		}
		//--- end ---//

		PacketReadyUART3 = 0;
	}
}

void UART4_Receive (void)
{
	unsigned short aux;
	antenna_typedef antenna_aux;

	if (msUART4rxTimeOut == 0)
	{

		MSUART4RXTIMEOUT;

		pBuffUART4rxW = &buffUART4rx[0];
		pBuffUART4rxR = &buffUART4rx[0];
	}

	if (PacketReadyUART4)
	{
		//--- Introduce your code here ---//
		//UART4Send((char *)&buffUART4rx2[0]);

		//temp,055.00\r\n
		if (!strncmp((const char *)&buffUART4rx2[0], (const char *)"temp", (sizeof("temp") - 1)))
		{
			if ((buffUART4rx2[4] == ',') && (buffUART4rx2[8] == '.') && (buffUART4rx2[11] == '\r'))
			{
				aux = (buffUART4rx2[5] - 48) * 100;
				aux += (buffUART4rx2[6] - 48) * 10;
				aux += (buffUART4rx2[7] - 48);

				temp_actual_channel_3_int = aux;

				aux = (buffUART4rx2[9] - 48) * 10;
				aux += (buffUART4rx2[10] - 48);

				temp_actual_channel_3_dec = aux;

				strcpy((char *)&buffUART4rx2[11], (const char *)",3\r\n");
				UART_PC_Send((char *)&buffUART4rx2[0]);
				UART_CH3_Send("OK\r\n");
			}
		}

		//ant0,012.27,087.90,001.80,065.00\r\n.
		else if (!strncmp((const char *)&buffUART4rx2[0], (const char *)"ant", (sizeof("ant") - 1)))
		{

			strcpy((char *)&buffUART4rx2[32], (const char *)",3\r\n");
			UART_PC_Send((char *)&buffUART4rx2[0]);

			if ((buffUART4rx2[4] == ',') && (buffUART4rx2[11] == ',') && (buffUART4rx2[18] == ',') && (buffUART4rx2[25] == ','))
			{
				aux = (buffUART4rx2[5] - 48) * 100;
				aux += (buffUART4rx2[6] - 48) * 10;
				aux += (buffUART4rx2[7] - 48);

				antenna_aux.resistance_int = aux;

				aux = (buffUART4rx2[9] - 48) * 10;
				aux += (buffUART4rx2[10] - 48);

				antenna_aux.resistance_dec = aux;

				aux = (buffUART4rx2[12] - 48) * 100;
				aux += (buffUART4rx2[13] - 48) * 10;
				aux += (buffUART4rx2[14] - 48);

				antenna_aux.inductance_int = aux;

				aux = (buffUART4rx2[16] - 48) * 10;
				aux += (buffUART4rx2[17] - 48);

				antenna_aux.inductance_dec = aux;

				aux = (buffUART4rx2[19] - 48) * 100;
				aux += (buffUART4rx2[20] - 48) * 10;
				aux += (buffUART4rx2[21] - 48);

				antenna_aux.current_limit_int = aux;

				aux = (buffUART4rx2[23] - 48) * 10;
				aux += (buffUART4rx2[24] - 48);

				antenna_aux.current_limit_dec = aux;

				aux = (buffUART4rx2[26] - 48) * 100;
				aux += (buffUART4rx2[27] - 48) * 10;
				aux += (buffUART4rx2[28] - 48);

				antenna_aux.temp_max_int = aux;

				aux = (buffUART4rx2[30] - 48) * 10;
				aux += (buffUART4rx2[31] - 48);

				antenna_aux.temp_max_dec = aux;

				Session_Set_Antenna (&session_ch_3, aux , &antenna_aux);
				Save_Antenna_Name(CH3, (char *) buffUART4rx2);

				UART4Send((char *) "OK\r\n");
			}
			else
				UART4Send((char *) "ERROR\r\n");
		}
		//--- end ---//
		PacketReadyUART4 = 0;
	}
}

void UART5_Receive (void)
{
	unsigned short aux;
	antenna_typedef antenna_aux;

	if (msUART5rxTimeOut == 0)
	{

		MSUART5RXTIMEOUT;

		pBuffUART5rxW = &buffUART5rx[0];
		pBuffUART5rxR = &buffUART5rx[0];
	}

	if (PacketReadyUART5)
	{

		//--- Introduce your code here ---//
		//UART5Send((char *)&buffUART5rx2[0]);

		//temp,055.00\r\n
		if (!strncmp((const char *)&buffUART5rx2[0], (const char *)"temp", (sizeof("temp") - 1)))
		{
			if ((buffUART5rx2[4] == ',') && (buffUART5rx2[8] == '.') && (buffUART5rx2[11] == '\r'))
			{
				aux = (buffUART5rx2[5] - 48) * 100;
				aux += (buffUART5rx2[6] - 48) * 10;
				aux += (buffUART5rx2[7] - 48);

				temp_actual_channel_4_int = aux;

				aux = (buffUART5rx2[9] - 48) * 10;
				aux += (buffUART5rx2[10] - 48);

				temp_actual_channel_4_dec = aux;

				strcpy((char *)&buffUART5rx2[11], (const char *)",4\r\n");
				UART_PC_Send((char *)&buffUART5rx2[0]);
				UART_CH4_Send("OK\r\n");
			}
		}

		//ant0,012.27,087.90,001.80,065.00\r\n.
		else if (!strncmp((const char *)&buffUART5rx2[0], (const char *)"ant", (sizeof("ant") - 1)))
		{
			strcpy((char *)&buffUART5rx2[32], (const char *)",4\r\n");
			UART_PC_Send((char *) &buffUART5rx2[0]);

			if ((buffUART5rx2[4] == ',') && (buffUART5rx2[11] == ',') && (buffUART5rx2[18] == ',') && (buffUART5rx2[25] == ','))
			{
				aux = (buffUART5rx2[5] - 48) * 100;
				aux += (buffUART5rx2[6] - 48) * 10;
				aux += (buffUART5rx2[7] - 48);

				antenna_aux.resistance_int = aux;

				aux = (buffUART5rx2[9] - 48) * 10;
				aux += (buffUART5rx2[10] - 48);

				antenna_aux.resistance_dec = aux;

				aux = (buffUART5rx2[12] - 48) * 100;
				aux += (buffUART5rx2[13] - 48) * 10;
				aux += (buffUART5rx2[14] - 48);

				antenna_aux.inductance_int = aux;

				aux = (buffUART5rx2[16] - 48) * 10;
				aux += (buffUART5rx2[17] - 48);

				antenna_aux.inductance_dec = aux;

				aux = (buffUART5rx2[19] - 48) * 100;
				aux += (buffUART5rx2[20] - 48) * 10;
				aux += (buffUART5rx2[21] - 48);

				antenna_aux.current_limit_int = aux;

				aux = (buffUART5rx2[23] - 48) * 10;
				aux += (buffUART5rx2[24] - 48);

				antenna_aux.current_limit_dec = aux;

				aux = (buffUART5rx2[26] - 48) * 100;
				aux += (buffUART5rx2[27] - 48) * 10;
				aux += (buffUART5rx2[28] - 48);

				antenna_aux.temp_max_int = aux;

				aux = (buffUART5rx2[30] - 48) * 10;
				aux += (buffUART5rx2[31] - 48);

				antenna_aux.temp_max_dec = aux;

				Session_Set_Antenna (&session_ch_4, aux , &antenna_aux);
				Save_Antenna_Name(CH4, (char *) buffUART5rx2);

				UART5Send((char *) "OK\r\n");
			}
			else
				UART5Send((char *) "ERROR\r\n");
		}
		//--- end ---//

		PacketReadyUART5 = 0;
	}
}
//End.
