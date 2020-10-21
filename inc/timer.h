#ifndef __TIMER_H__
#define __TIMER_H__

#define TIM_EnableIT_UPDATE(TIMx) SET_BIT(TIMx->DIER, TIM_DIER_UIE)
#define TIM_EnableIT_COMPARE(TIMx) SET_BIT(TIMx->DIER, TIM_DIER_CC1IE); // Capture/Compare 1 interrupt enable
#define TIM_EnableCounter(TIMx) SET_BIT(TIMx->CR1, TIM_CR1_CEN)
#define TIM_DisableCounter(TIMx) CLEAR_BIT(TIMx->CR1, TIM_CR1_CEN)



void TIM3_Init(void);



#endif