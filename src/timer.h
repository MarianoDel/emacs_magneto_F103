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

//--- Exported Macros ------------------------------------
// #define RCC_TIM1_CLK 		(RCC->APB2ENR & 0x00000800)
// #define RCC_TIM1_CLK_ON 	RCC->APB2ENR |= 0x00000800
// #define RCC_TIM1_CLK_OFF 	RCC->APB2ENR &= ~0x00000800

// #define RCC_TIM3_CLK 		(RCC->APB1ENR & 0x00000002)
// #define RCC_TIM3_CLK_ON 	RCC->APB1ENR |= 0x00000002
// #define RCC_TIM3_CLK_OFF 	RCC->APB1ENR &= ~0x00000002

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


#define EnableTimer6 TIM6->CR1 |= TIM_CR1_CEN
#define DisableTimer6 TIM6->CR1 &= ~TIM_CR1_CEN

#define EnableTimer7 TIM7->CR1 |= TIM_CR1_CEN
#define DisableTimer7 TIM7->CR1 &= ~TIM_CR1_CEN

//--- Exported Functions ------------------------------
void TIM7_IRQHandler (void);
void TIM7_Init(void);
void TIM6_IRQHandler (void);
void TIM6_Init(void);
void TIM2_Init(void);
void Wait_ms (unsigned short);

#endif /* _TIMER_H_ */
