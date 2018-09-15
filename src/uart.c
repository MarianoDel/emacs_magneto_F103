
#include "uart.h"
#include "GTK_Estructura.h"
#include "stm32f10x_usart.h"
#include "misc.h"
#include "GTK_Signal.h"
#include "GTK_Hard.h"
#include "flash_program.h"
#include "timer.h"

#include "comms.h"

#include <stdio.h>
#include <string.h>


//External variable.
//General.
// extern session_typedef session_slot_aux;

// extern session_typedef session_ch_1;
// extern session_typedef session_ch_2;
// extern session_typedef session_ch_3;
// extern session_typedef session_ch_4;

// extern unsigned char temp_actual_channel_1_int;
// extern unsigned char temp_actual_channel_1_dec;
// extern unsigned char temp_actual_channel_2_int;
// extern unsigned char temp_actual_channel_2_dec;
// extern unsigned char temp_actual_channel_3_int;
// extern unsigned char temp_actual_channel_3_dec;
// extern unsigned char temp_actual_channel_4_int;
// extern unsigned char temp_actual_channel_4_dec;

// extern unsigned char channel_1_pause;
// extern unsigned char channel_2_pause;
// extern unsigned char channel_3_pause;
// extern unsigned char channel_4_pause;

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

//End.
