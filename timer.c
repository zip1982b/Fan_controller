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
  if(READ_BIT(TIM2->SR, TIM_SR_UIF)){
    CLEAR_BIT(TIM2->SR, TIM_SR_UIF);
	if(READ_BIT(GPIOC->ODR, GPIO_ODR_ODR13)){
		PortSetLow();
	}
	else PortSetHi();
  }
  
  if(Start){
	  GPIO_ODR_ODR13 = 1; // or BRR BHRR
	  /*
		Настройка GPIO
		Пин: PB13
		Режим: Input floating (external pull-up 5KOhm)
		*/
		GPIOB->CRH &= ~(GPIO_CRH_MODE13 | GPIO_CRH_CNF13); //reset PB13
  
		GPIOB->CRH &= ~(GPIO_CRL_MODE13 | GPIO_CRL_CNF13);
		GPIOB->CRH |= (0x01 << GPIO_CRH_CNF13_Pos); //Input floating (external pull-up 5KOhm)
		//GPIOB->ODR |= (1 << 0); //Подтяжка вверх
  }
  
  
  
  
  
  
  
  
  
}