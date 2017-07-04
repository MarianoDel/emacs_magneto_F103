/**
  ******************************************************************************
  * @file    stm32f10x_conf.h
  * @author  MCD Application Team
  * @version V4.0.0
  * @date    21-January-2013
  * @brief   Library configuration file.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F10x_CONF_H
#define __STM32F10x_CONF_H

/* Includes ------------------------------------------------------------------*/
/* Uncomment the line below to enable peripheral header file inclusion */
#include "stm32f10x.h"
//#include "stm32f10x_adc.h"
//#include "stm32f10x_bkp.h"
//#include "stm32f10x_can.h"
//#include "stm32f10x_crc.h"
//#include "stm32f10x_dac.h"
//#include "stm32f10x_dbgmcu.h"
//#include "stm32f10x_dma.h"
//#include "stm32f10x_exti.h"
//#include "stm32f10x_flash.h"
//#include "stm32f10x_fsmc.h"
//#include "stm32f10x_gpio.h"
//#include "stm32f10x_i2c.h"
//#include "stm32f10x_iwdg.h"
//#include "stm32f10x_pwr.h"
#include "stm32f10x_rcc.h"
//#include "stm32f10x_rtc.h"
//#include "stm32f10x_sdio.h"
//#include "stm32f10x_spi.h"
//#include "stm32f10x_tim.h"
//#include "stm32f10x_usart.h"
//#include "stm32f10x_wwdg.h"
//#include "misc.h"   /* High level functions for NVIC and SysTick (add-on to CMSIS functions) */


//#include "string.h"
//#include "stdio.h"
//
//#include "uart.h"
//#include "timer.h"
//#include "pwm.h"
//
//#include "adc.h"
//#include "flash_program.h"
//#include "GTK_Errors.h"
//#include "GTK_Estructura.h"
//#include "GTK_Hard.h"
//#include "GTK_Signal.h"


//#include "calendar.h"
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Uncomment the line below to expanse the "assert_param" macro in the 
   Standard Peripheral Library drivers code */
/* #define USE_FULL_ASSERT    1 */

void TIME_1MS (void);

//--- RCC clkEnable ---//
#define RCC_GPIOA_clk (RCC->APB2ENR & 0x00000004)
#define RCC_GPIOA_clkEnable RCC->APB2ENR |= 0x00000004
#define RCC_GPIOA_clkDisable RCC->APB2ENR &= ~0x00000004

#define RCC_GPIOB_clk (RCC->APB2ENR & 0x00000008)
#define RCC_GPIOB_clkEnable RCC->APB2ENR |= 0x00000008
#define RCC_GPIOB_clkDisable RCC->APB2ENR &= ~0x00000008

#define RCC_GPIOC_clk (RCC->APB2ENR & 0x00000010)
#define RCC_GPIOC_clkEnable RCC->APB2ENR |= 0x00000010
#define RCC_GPIOC_clkDisable RCC->APB2ENR &= ~0x00000010

#define RCC_GPIOD_clk (RCC->APB2ENR & 0x00000020)
#define RCC_GPIOD_clkEnable RCC->APB2ENR |= 0x00000020
#define RCC_GPIOD_clkDisable RCC->APB2ENR &= ~0x00000020

#define RCC_AFIO_CLK (RCC->APB2ENR & 0x00000001)
#define RCC_AFIO_CLKEN RCC->APB2ENR |= 0x00000001
#define RCC_AFIO_CLKDIS RCC->APB2ENR &= ~0x00000001

#define RCC_TIM1_CLK (RCC->APB2ENR & 0x00000800)
#define RCC_TIM1_CLKEN RCC->APB2ENR |= 0x00000800
#define RCC_TIM1_CLKDIS RCC->APB2ENR &= ~0x00000800

#define RCC_TIM2_CLK (RCC->APB1ENR & 0x00000001)
#define RCC_TIM2_CLKEN RCC->APB1ENR |= 0x00000001
#define RCC_TIM2_CLKDIS RCC->APB1ENR &= ~0x00000001

#define RCC_TIM3_CLK (RCC->APB1ENR & 0x00000002)
#define RCC_TIM3_CLKEN RCC->APB1ENR |= 0x00000002
#define RCC_TIM3_CLKDIS RCC->APB1ENR &= ~0x00000002

#define RCC_TIM4_CLK (RCC->APB1ENR & 0x00000004)
#define RCC_TIM4_CLKEN RCC->APB1ENR |= 0x00000004
#define RCC_TIM4_CLKDIS RCC->APB1ENR &= ~0x00000004

#define RCC_TIM5_CLK (RCC->APB1ENR & 0x00000008)
#define RCC_TIM5_CLKEN RCC->APB1ENR |= 0x00000008
#define RCC_TIM5_CLKDIS RCC->APB1ENR &= ~0x00000008

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

#define RCC_ADC1_CLK (RCC->APB2ENR & 0x00000200)
#define RCC_ADC1_CLKEN RCC->APB2ENR |= 0x00000200
#define RCC_ADC1_CLKDIS RCC->APB2ENR &= ~0x00000200

#define RCC_ADC2_CLK (RCC->APB2ENR & 0x00000400)
#define RCC_ADC2_CLKEN RCC->APB2ENR |= 0x00000400
#define RCC_ADC2_CLKDIS RCC->APB2ENR &= ~0x00000400

#define RCC_ADC3_CLK (RCC->APB2ENR & 0x00008000)
#define RCC_ADC3_CLKEN RCC->APB2ENR |= 0x00008000
#define RCC_ADC3_CLKDIS RCC->APB2ENR &= ~0x00008000

/* Exported macro ------------------------------------------------------------*/
#ifdef  USE_FULL_ASSERT

/*******************************************************************************
* Macro Name     : assert_param
* Description    : The assert_param macro is used for function's parameters check.
* Input          : - expr: If expr is false, it calls assert_failed function
*                    which reports the name of the source file and the source
*                    line number of the call that failed. 
*                    If expr is true, it returns no value.
* Return         : None
*******************************************************************************/ 
  #define assert_param(expr) ((expr) ? (void)0 : assert_failed((uint8_t *)__FILE__, __LINE__))
/* Exported functions ------------------------------------------------------- */
  void assert_failed(uint8_t* file, uint32_t line);
#else
  #define assert_param(expr) ((void)0)
#endif /* USE_FULL_ASSERT */

#endif /* __STM32F10x_CONF_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
