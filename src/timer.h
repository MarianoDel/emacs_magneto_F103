#ifndef TIMER_H_
#define TIMER_H_

#define EnableTimer6 TIM6->CR1 |= TIM_CR1_CEN
#define DisableTimer6 TIM6->CR1 &= ~TIM_CR1_CEN

#define EnableTimer7 TIM7->CR1 |= TIM_CR1_CEN
#define DisableTimer7 TIM7->CR1 &= ~TIM_CR1_CEN

//funciones
void TIM7_IRQHandler (void);
void TIM7_Init(void);
void TIM6_IRQHandler (void);
void TIM6_Init(void);
void TIM2_Init(void);
void Wait_ms (unsigned short a);

#endif /* TIMER_H_ */
