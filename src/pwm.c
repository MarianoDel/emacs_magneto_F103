

#include <stm32f10x.h>
#include "GTK_Hard.h"
#include "pwm.h"
#include "stm32f10x_tim.h"

/*
#define DUTY_DEF  (1000 - duty)
#define DUTY1_DEF (1000 - duty1)
#define DUTY2_DEF (1000 - duty2)
#define DUTY3_DEF (1000 - duty3)
#define DUTY4_DEF (1000 - duty4)
*/


//#define DUTY_DEF  (duty)
#define DUTY1_DEF (duty1)
#define DUTY2_DEF (duty2)
#define DUTY3_DEF (duty3)
#define DUTY4_DEF (duty4)

/*
 * Channel 1: PA8
 * Channel 2: PA11
 */
void PWM_TIM1_CONFIG (unsigned short prescaler, unsigned short periodo, unsigned short duty1, unsigned short duty2, unsigned short duty3, unsigned short duty4, unsigned char channel_en)
{

	TIM_OCInitTypeDef  TIM_OCInitStructure;
	unsigned int temp;

	//Habilitacion de clock.
	if (!RCC_AFIO_CLK)
		RCC_AFIO_CLKEN;

	if (!RCC_TIM1_CLK)
		RCC_TIM1_CLKEN;

	if (!RCC_GPIOA_clk)
		RCC_GPIOA_clkEnable;

	//PA8 salida PWM.
	if (channel_en & PWM_CHANNEL_1)
	{
		temp = GPIOA->CRH;
		temp &= 0xFFFFFFF0;
		temp |= 0x0000000B;
		GPIOA->CRH = temp;
	}

	//PA11 salida PWM.
	if (channel_en & PWM_CHANNEL_2)
	{
		temp = GPIOA->CRH;
		temp &= 0xFFFF0FFF;
		temp |= 0x0000B000;
		GPIOA->CRH = temp;
	}

	//Configuracion del timer.
	TIM1->CR1 = 0;
	TIM1->ARR = periodo;
	TIM1->PSC = prescaler;
	TIM1->EGR = 0x0001;

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;

	if (channel_en & PWM_CHANNEL_1)
	{
		TIM_OCInitStructure.TIM_Pulse = DUTY1_DEF;
		TIM_OC1Init(TIM1, &TIM_OCInitStructure);
		//TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
	}

	if (channel_en & PWM_CHANNEL_2)
	{
		TIM_OCInitStructure.TIM_Pulse = DUTY2_DEF;
		TIM_OC4Init(TIM1, &TIM_OCInitStructure);
		//TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Enable);
	}

	TIM1->CR1 |= 0x0081; //Habilitacion de preload y timer.

	/* TIM1 Main Output Enable */
	TIM_CtrlPWMOutputs(TIM1, ENABLE);
}

void PWM_TIM1_DUTY (unsigned short duty, unsigned short channel)
{

	if (channel & PWM_CHANNEL_1)
		TIM1->CCR1 = duty;

	if (channel & PWM_CHANNEL_2)
	{
/*		if (DUTY_DEF > 0)
		{

			while (1)
			{
				Led3Toggle();
				Wait_ms(100);
			}
		}
*/
		TIM1->CCR4 = duty;
	}
}

/*
 * Channel 1: PA0
 * Channel 2: PA1
 */
void PWM_TIM2_CONFIG (unsigned short prescaler, unsigned short periodo, unsigned short duty1, unsigned short duty2, unsigned short duty3, unsigned short duty4, unsigned char channel_en)
{

	TIM_OCInitTypeDef  TIM_OCInitStructure;
	unsigned int temp;

	//Habilitacion de clock.
	if (!RCC_AFIO_CLK)
		RCC_AFIO_CLKEN;

	if (!RCC_TIM2_CLK)
		RCC_TIM2_CLKEN;

	if (!RCC_GPIOA_clk)
		RCC_GPIOA_clkEnable;

	//PA0 salida PWM.
	if (channel_en & PWM_CHANNEL_1)
	{
		temp = GPIOA->CRL;
		temp &= 0xFFFFFFF0;
		temp |= 0x0000000B;
		GPIOA->CRL = temp;
	}

	//PA1 salida PWM.
	if (channel_en & PWM_CHANNEL_2)
	{
		temp = GPIOA->CRL;
		temp &= 0xFFFFFF0F;
		temp |= 0x000000B0;
		GPIOA->CRL = temp;
	}

	//Configuracion del timer.
	TIM2->CR1 = 0;
	TIM2->ARR = periodo;
	TIM2->PSC = prescaler;
	TIM2->EGR = 0x0001;

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;


	if (channel_en & PWM_CHANNEL_1)
	{
		TIM_OCInitStructure.TIM_Pulse = DUTY1_DEF;
		TIM_OC1Init(TIM2, &TIM_OCInitStructure);
		TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);
	}

	if (channel_en & PWM_CHANNEL_2)
	{
		TIM_OCInitStructure.TIM_Pulse = DUTY2_DEF;
		TIM_OC2Init(TIM2, &TIM_OCInitStructure);
		TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);
	}

	TIM2->CR1 |= 0x0081; //Habilitacion de preload y timer.
}

void PWM_TIM2_DUTY (unsigned short duty, unsigned short channel)
{

	if (channel & PWM_CHANNEL_1)
		TIM2->CCR1 = duty;

	if (channel & PWM_CHANNEL_2)
		TIM2->CCR2 = duty;

	if (channel & PWM_CHANNEL_3)
		TIM2->CCR3 = duty;

	if (channel & PWM_CHANNEL_4)
		TIM2->CCR4 = duty;
}

/*
 * Channel 1: PC6
 * Channel 2: PC7
 * Channel 3: PC8
 * Channel 4: PC9
 */
void PWM_TIM3_CONFIG (unsigned short prescaler, unsigned short periodo, unsigned short duty1, unsigned short duty2, unsigned short duty3, unsigned short duty4, unsigned char channel_en)
{

	TIM_OCInitTypeDef  TIM_OCInitStructure;
	unsigned int temp;

	//Habilitacion de clock.
	if (!RCC_AFIO_CLK)
		RCC_AFIO_CLKEN;

	if (!RCC_TIM3_CLK)
		RCC_TIM3_CLKEN;

	if (!RCC_GPIOC_clk)
		RCC_GPIOC_clkEnable;

	//Remap.
	AFIO->MAPR |= 0x00000C00;

	//PA0 salida PWM.
	if (channel_en & PWM_CHANNEL_1)
	{
		temp = GPIOC->CRL;
		temp &= 0xF0FFFFFF;
		temp |= 0x0B000000;
		GPIOC->CRL = temp;
	}

	//PA1 salida PWM.
	if (channel_en & PWM_CHANNEL_2)
	{
		temp = GPIOC->CRL;
		temp &= 0x0FFFFFFF;
		temp |= 0xB0000000;
		GPIOC->CRL = temp;
	}

	if (channel_en & PWM_CHANNEL_3)
	{
		temp = GPIOC->CRH;
		temp &= 0xFFFFFFF0;
		temp |= 0x0000000B;
		GPIOC->CRH = temp;
	}

	if (channel_en & PWM_CHANNEL_4)
	{
		temp = GPIOC->CRH;
		temp &= 0xFFFFFF0F;
		temp |= 0x000000B0;
		GPIOC->CRH = temp;
	}

	//Configuracion del timer.
	TIM3->CR1 = 0;
	TIM3->ARR = periodo;
	TIM3->PSC = prescaler;
	TIM3->EGR = 0x0001;

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;


	if (channel_en & PWM_CHANNEL_1)
	{
		TIM_OCInitStructure.TIM_Pulse = DUTY1_DEF;
		TIM_OC1Init(TIM3, &TIM_OCInitStructure);
		TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
	}

	if (channel_en & PWM_CHANNEL_2)
	{
		TIM_OCInitStructure.TIM_Pulse = DUTY2_DEF;
		TIM_OC2Init(TIM3, &TIM_OCInitStructure);
		TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);
	}

	if (channel_en & PWM_CHANNEL_3)
	{
		TIM_OCInitStructure.TIM_Pulse = DUTY3_DEF;
		TIM_OC3Init(TIM3, &TIM_OCInitStructure);
		TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);
	}

	if (channel_en & PWM_CHANNEL_4)
	{
		TIM_OCInitStructure.TIM_Pulse = DUTY4_DEF;
		TIM_OC4Init(TIM3, &TIM_OCInitStructure);
		TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);
	}

	TIM3->CR1 |= 0x0081; //Habilitacion de preload y timer.
}

void PWM_TIM3_DUTY (unsigned short duty, unsigned short channel)
{

	if (channel & PWM_CHANNEL_1)
		TIM3->CCR1 = duty;

	if (channel & PWM_CHANNEL_2)
		TIM3->CCR2 = duty;

	if (channel & PWM_CHANNEL_3)
		TIM3->CCR3 = duty;

	if (channel & PWM_CHANNEL_4)
		TIM3->CCR4 = duty;
}

/*
 * Channel 1: PB6
 * Channel 2: PB7
 * Channel 3: PB8
 * Channel 4: PB9
 */
void PWM_TIM4_CONFIG (unsigned short prescaler, unsigned short periodo, unsigned short duty1, unsigned short duty2, unsigned short duty3, unsigned short duty4, unsigned char channel_en)
{

	TIM_OCInitTypeDef  TIM_OCInitStructure;
	unsigned int temp;

	//Habilitacion de clock.
	if (!RCC_AFIO_CLK)
		RCC_AFIO_CLKEN;

	if (!RCC_TIM4_CLK)
		RCC_TIM4_CLKEN;

	if (!RCC_GPIOB_clk)
		RCC_GPIOB_clkEnable;

	//Remap.
	AFIO->MAPR |= 0x00000C00;

	//PA0 salida PWM.
	if (channel_en & PWM_CHANNEL_1)
	{
		temp = GPIOB->CRL;
		temp &= 0xF0FFFFFF;
		temp |= 0x0B000000;
		GPIOB->CRL = temp;
	}

	//PA1 salida PWM.
	if (channel_en & PWM_CHANNEL_2)
	{
		temp = GPIOB->CRL;
		temp &= 0x0FFFFFFF;
		temp |= 0xB0000000;
		GPIOB->CRL = temp;
	}

	if (channel_en & PWM_CHANNEL_3)
	{
		temp = GPIOB->CRH;
		temp &= 0xFFFFFFF0;
		temp |= 0x0000000B;
		GPIOB->CRH = temp;
	}

	if (channel_en & PWM_CHANNEL_4)
	{
		temp = GPIOB->CRH;
		temp &= 0xFFFFFF0F;
		temp |= 0x000000B0;
		GPIOB->CRH = temp;
	}

	//Configuracion del timer.
	TIM4->CR1 = 0;
	TIM4->ARR = periodo;
	TIM4->PSC = prescaler;
	TIM4->EGR = 0x0001;

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;


	if (channel_en & PWM_CHANNEL_1)
	{
		TIM_OCInitStructure.TIM_Pulse = DUTY1_DEF;
		TIM_OC1Init(TIM4, &TIM_OCInitStructure);
		TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);
	}

	if (channel_en & PWM_CHANNEL_2)
	{
		TIM_OCInitStructure.TIM_Pulse = DUTY2_DEF;
		TIM_OC2Init(TIM4, &TIM_OCInitStructure);
		TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);
	}

	if (channel_en & PWM_CHANNEL_3)
	{
		TIM_OCInitStructure.TIM_Pulse = DUTY3_DEF;
		TIM_OC3Init(TIM4, &TIM_OCInitStructure);
		TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);
	}

	if (channel_en & PWM_CHANNEL_4)
	{
		TIM_OCInitStructure.TIM_Pulse = DUTY4_DEF;
		TIM_OC4Init(TIM4, &TIM_OCInitStructure);
		TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);
	}

	TIM4->CR1 |= 0x0081; //Habilitacion de preload y timer.
}

void PWM_TIM4_DUTY (unsigned short duty, unsigned short channel)
{

	if (channel & PWM_CHANNEL_1)
		TIM4->CCR1 = duty;

	if (channel & PWM_CHANNEL_2)
		TIM4->CCR2 = duty;

	if (channel & PWM_CHANNEL_3)
		TIM4->CCR3 = duty;

	if (channel & PWM_CHANNEL_4)
		TIM4->CCR4 = duty;
}
