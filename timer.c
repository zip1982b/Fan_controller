#include "stm32f1xx.h"
#include "timer.h"



void TIM2_Init(void)
{
  SET_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM2EN);
  NVIC_EnableIRQ(TIM2_IRQn);
  WRITE_REG(TIM2->PSC, 1200);
  WRITE_REG(TIM2->ARR, 60606);
}


void TIM2_IRQHandler(void)
{
  if(READ_BIT(TIM2->SR, TIM_SR_UIF))
  {
    CLEAR_BIT(TIM2->SR, TIM_SR_UIF);
	if(READ_BIT(GPIOC->ODR, GPIO_ODR_ODR13)){
		PortSetLow();
	}
	else PortSetHi();
	
  }
}