//---------------------------------------------
// ##
// ## @Author: Med
// ## @Editor: Emacs - ggtags
// ## @TAGS:   Global
// ## @CPU:    STM32F103
// ##
// #### TIMER.H ################################
//---------------------------------------------

#ifndef _TIMER_H_
#define _TIMER_H_

//--- Exported Defines ------------------------------------
#define DUTY_NONE      0
#define DUTY_50_PERCENT      500
#define DUTY_100_PERCENT     1000
#define DUTY_ALWAYS          (DUTY_100_PERCENT + 1)
#define PWM_PRESCALER        10

//--- Exported Macros ------------------------------------
#define RCC_TIM1_CLK     (RCC->APB2ENR & 0x00000800)
#define RCC_TIM1_CLK_ON     RCC->APB2ENR |= 0x00000800
#define RCC_TIM1_CLK_OFF     RCC->APB2ENR &= ~0x00000800

#define RCC_TIM2_CLK     (RCC->APB1ENR & 0x00000001)
#define RCC_TIM2_CLK_ON     RCC->APB1ENR |= 0x00000001
#define RCC_TIM2_CLK_OFF     RCC->APB1ENR &= ~0x00000001

#define RCC_TIM3_CLK     (RCC->APB1ENR & 0x00000002)
#define RCC_TIM3_CLK_ON     RCC->APB1ENR |= 0x00000002
#define RCC_TIM3_CLK_OFF     RCC->APB1ENR &= ~0x00000002

#define RCC_TIM4_CLK     (RCC->APB1ENR & 0x00000004)
#define RCC_TIM4_CLK_ON     RCC->APB1ENR |= 0x00000004
#define RCC_TIM4_CLK_OFF     RCC->APB1ENR &= ~0x00000004

#define RCC_TIM5_CLK     (RCC->APB1ENR & 0x00000008)
#define RCC_TIM5_CLK_ON     RCC->APB1ENR |= 0x00000008
#define RCC_TIM5_CLK_OFF     RCC->APB1ENR &= ~0x00000008

#define RCC_TIM6_CLK 		(RCC->APB1ENR & 0x00000010)
#define RCC_TIM6_CLK_ON 	RCC->APB1ENR |= 0x00000010;
#define RCC_TIM6_CLK_OFF        RCC->APB1ENR &= ~0x00000010;

#define RCC_TIM7_CLK 		(RCC->APB1ENR & 0x00000020)
#define RCC_TIM7_CLK_ON 	RCC->APB1ENR |= 0x00000020;
#define RCC_TIM7_CLK_OFF        RCC->APB1ENR &= ~0x00000020;

// #define RCC_TIM14_CLK 		(RCC->APB1ENR & 0x00000100)
// #define RCC_TIM14_CLK_ON 	RCC->APB1ENR |= 0x00000100
// #define RCC_TIM14_CLK_OFF 	RCC->APB1ENR &= ~0x00000100

// #define RCC_TIM15_CLK 		(RCC->APB2ENR & 0x00010000)
// #define RCC_TIM15_CLK_ON 	RCC->APB2ENR |= 0x00010000
// #define RCC_TIM15_CLK_OFF 	RCC->APB2ENR &= ~0x00010000

// #define RCC_TIM16_CLK 		(RCC->APB2ENR & 0x00020000)
// #define RCC_TIM16_CLK_ON 	RCC->APB2ENR |= 0x00020000
// #define RCC_TIM16_CLK_OFF 	RCC->APB2ENR &= ~0x00020000

// #define RCC_TIM17_CLK 		(RCC->APB2ENR & 0x00040000)
// #define RCC_TIM17_CLK_ON 	RCC->APB2ENR |= 0x00040000
// #define RCC_TIM17_CLK_OFF 	RCC->APB2ENR &= ~0x00040000

#define PWM_CH1_TiempoSubida(d) 	Update_TIM2_CH1(d)
#define PWM_CH1_TiempoMantenimiento(d) 	Update_TIM2_CH2(d)
#define PWM_CH1_TiempoBajada(d) 	Update_TIM1_CH1(d)

#define PWM_CH2_TiempoSubida(d) 	Update_TIM1_CH4(d)
#define PWM_CH2_TiempoMantenimiento(d) 	Update_TIM3_CH1(d)
#define PWM_CH2_TiempoBajada(d) 	Update_TIM3_CH2(d)

#define PWM_CH3_TiempoSubida(d) 	Update_TIM3_CH3(d)
#define PWM_CH3_TiempoMantenimiento(d) 	Update_TIM3_CH4(d)
#define PWM_CH3_TiempoBajada(d) 	Update_TIM4_CH1(d)

#define PWM_CH4_TiempoSubida(d) 	Update_TIM4_CH2(d)
#define PWM_CH4_TiempoMantenimiento(d) 	Update_TIM4_CH3(d)
#define PWM_CH4_TiempoBajada(d) 	Update_TIM4_CH4(d)


#define EnableTimer6 TIM6->CR1 |= TIM_CR1_CEN
#define DisableTimer6 TIM6->CR1 &= ~TIM_CR1_CEN

#define EnableTimer7 TIM7->CR1 |= TIM_CR1_CEN
#define DisableTimer7 TIM7->CR1 &= ~TIM_CR1_CEN

//--- Exported Functions ------------------------------

void TIM5_Init (void);
void TIM5_IRQHandler (void);
void TIM6_IRQHandler (void);
void TIM6_Init(void);
void TIM7_IRQHandler (void);
void TIM7_Init(void);

void Wait_ms (unsigned short);
void TIM_1_Init (void);
void TIM_2_Init (void);
void TIM_3_Init (void);
void TIM_4_Init (void);
void Update_TIM1_CH1 (unsigned short);
void Update_TIM1_CH4 (unsigned short);
void Update_TIM2_CH1 (unsigned short);
void Update_TIM2_CH2 (unsigned short);
void Update_TIM3_CH1 (unsigned short);
void Update_TIM3_CH2 (unsigned short);
void Update_TIM3_CH3 (unsigned short);
void Update_TIM3_CH4 (unsigned short);
void Update_TIM4_CH1 (unsigned short);
void Update_TIM4_CH2 (unsigned short);
void Update_TIM4_CH3 (unsigned short);
void Update_TIM4_CH4 (unsigned short);

#endif /* _TIMER_H_ */

//--- end of file ---//
