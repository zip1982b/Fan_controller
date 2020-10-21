#include "stm32f1xx.h"
#include "timer.h"



void TIM3_Init(void)
{
  SET_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM3EN);
  NVIC_EnableIRQ(TIM3_IRQn);
  WRITE_REG(TIM3->PSC, 1200);
  WRITE_REG(TIM3->ARR, 60606);
  WRITE_REG(TIM3->CCR1, 545); // T=0.000033s, f=30000Hz, need 18ms
  //SET_BIT(TIM3->DIER, TIM_DIER_CC1IE); // Capture/Compare 1 interrupt enable
  
}


void TIM3_IRQHandler(void)
{
  if(READ_BIT(TIM3->SR, TIM_SR_UIF)){
    CLEAR_BIT(TIM3->SR, TIM_SR_UIF);
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