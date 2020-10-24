#include "stm32f1xx.h"
#include "timer.h"



void TIM3_Init(void)
{
  SET_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM3EN); 
  NVIC_EnableIRQ(TIM3_IRQn);
  WRITE_REG(TIM3->PSC, 1200);
  WRITE_REG(TIM3->ARR, 60606);
  SET_BIT(TIM3->CCMR1, TIM_CCMR1_OC1M_0); //OC1M = 011  Toggle - OC1REF toggles when TIM3_CNT=TIM3_CCR1.
  SET_BIT(TIM3->CCMR1, TIM_CCMR1_OC1M_1);
  CLEAR_BIT(TIM3->CCMR1, TIM_CCMR1_OC1M_2);
  
  CLEAR_BIT(TIM3->CCMR1, TIM_CCMR1_OC1PE);//Preload register on TIMx_CCR1 disabled. 										//TIMx_CCR1 can be written at anytime.
  CLEAR_BIT(TIM3->CCER, TIM_CCER_CC1P); 	//output polarity
											//0: OC1 active high.
											//1: OC1 active low.
  SET_BIT(TIM3->CCER, TIM_CCER_CC1E); //output enable. 1: On - OC1 signal is output on the corresponding output pin.
  
  SET_BIT(DBGMCU->CR, DBGMCU_CR_DBG_TIM3_STOP); //TIM3 counter stopped when core is halted
}


void DHT22_Start(void){
	WRITE_REG(TIM3->CCR1, 5); // T=0.000033s, f=30000Hz, need 1sec delay (5-30308)
	TIM_EnableCounter(TIM3);
}



void TIM3_IRQHandler(void)
{
  if(READ_BIT(TIM3->SR, TIM_SR_UIF)){
    CLEAR_BIT(TIM3->SR, TIM_SR_UIF);
	TIM_DisableCounter(TIM3);
  }
  
  if(READ_BIT(TIM3->SR, TIM_SR_CC1IF)){
    CLEAR_BIT(TIM3->SR, TIM_SR_CC1IF);
	WRITE_REG(TIM3->CCR1, 30308); // T=0.000033s, f=30000Hz, need 1sec delay (5-30308)
  }
  
}



