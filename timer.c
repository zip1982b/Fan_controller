#include "stm32f1xx.h"
#include "gpio.h" 
#include "timer.h"

uint8_t n = 0; 
uint32_t time[41] = {0};

void TIM3_Init(void)
{
  SET_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM3EN); 
  NVIC_EnableIRQ(TIM3_IRQn);
  WRITE_REG(TIM3->PSC, 36); //f=1MHz
  WRITE_REG(TIM3->ARR, 18000); // delay 18mSec
  SET_BIT(DBGMCU->CR, DBGMCU_CR_DBG_TIM3_STOP); //TIM3 counter stopped when core is halted
}


void TIM3_Mode(eMode mode){
	if(mode == OUTPUT){
		//CLEAR_BIT(TIM3->CCER, TIM_CCER_CC1E); // Note: CC1S bits are writable only when the channel is OFF (CC1E = 0 in TIMx_CCER)
		//CLEAR_BIT(TIM3->CCMR1, TIM_CCMR1_CC1S_0); 
		//CLEAR_BIT(TIM3->CCMR1, TIM_CCMR1_CC1S_1); // 00: CC1 channel is configured as output
		
		//SET_BIT(TIM3->CCMR1, TIM_CCMR1_OC1M_0); //OC1M = 011  Toggle - OC1REF toggles when TIM3_CNT=TIM3_CCR1.
		//SET_BIT(TIM3->CCMR1, TIM_CCMR1_OC1M_1); // переключение
		//CLEAR_BIT(TIM3->CCMR1, TIM_CCMR1_OC1M_2); 
  
		//CLEAR_BIT(TIM3->CCMR1, TIM_CCMR1_OC1PE);//Preload register on TIMx_CCR1 disabled. TIMx_CCR1 can be written at anytime.
		//CLEAR_BIT(TIM3->CCER, TIM_CCER_CC1P); 	//output polarity 
											//0: OC1 active high.!
											//1: OC1 active low.
		//SET_BIT(TIM3->CCER, TIM_CCER_CC1E); //output enable. 1: On - OC1 signal is output on the corresponding output pin.
		/*slave mode selection*/
		CLEAR_BIT(TIM3->SMCR, TIM_SMCR_SMS_0);
		CLEAR_BIT(TIM3->SMCR, TIM_SMCR_SMS_1);
		CLEAR_BIT(TIM3->SMCR, TIM_SMCR_SMS_2); //000: 000: Slave mode disabled
		
		
		SET_BIT(TIM3->DIER, TIM_DIER_UIE); //enable interrupt
		CLEAR_BIT(TIM3->DIER, TIM_DIER_CC1IE); //disable interrupt
		CLEAR_BIT(TIM3->DIER, TIM_DIER_CC2IE); //disable interrupt
	}
	else if(mode == INPUT){
		WRITE_REG(TIM3->ARR, 65000); // 
		
		CLEAR_BIT(TIM3->CCER, TIM_CCER_CC1E); // Note: CC1S bits are writable only when the channel is OFF (CC1E = 0 in TIMx_CCER)
		SET_BIT(TIM3->CCMR1, TIM_CCMR1_CC1S_0); // only when the channel is OFF (CC1E = 0 in TIMx_CCER)
		CLEAR_BIT(TIM3->CCMR1, TIM_CCMR1_CC1S_1); // 01: CC1 channel is configured as input, IC1 is mapped on TI1.
		
		CLEAR_BIT(TIM3->CCER, TIM_CCER_CC1P); 	//0: non-inverted: capture is done on a rising edge of IC1

		CLEAR_BIT(TIM3->CCMR1, TIM_CCMR1_CC2S_0); 
		SET_BIT(TIM3->CCMR1, TIM_CCMR1_CC2S_1); // 10: CC2 channel is configured as input, IC2 is mapped on TI1
		SET_BIT(TIM3->CCER, TIM_CCER_CC2P); 	//1:inverted: capture is done on a falling edge of IC2
		
		/*trigger selection*/
		SET_BIT(TIM3->SMCR, TIM_SMCR_TS_0); 
		CLEAR_BIT(TIM3->SMCR, TIM_SMCR_TS_1);
		SET_BIT(TIM3->SMCR, TIM_SMCR_TS_2); //101: Filtered Timer Input 1 (TI1FP1)
		
		/*slave mode selection*/
		CLEAR_BIT(TIM3->SMCR, TIM_SMCR_SMS_0);
		CLEAR_BIT(TIM3->SMCR, TIM_SMCR_SMS_1);
		SET_BIT(TIM3->SMCR, TIM_SMCR_SMS_2); //100: Reset Mode - Rising edge of the selected trigger input (TRGI) reinitializes the counter and generates an update of the registers.
		
		SET_BIT(TIM3->CCER, TIM_CCER_CC1E); // 1: Capture enabled.
		SET_BIT(TIM3->CCER, TIM_CCER_CC2E); // 1: Capture enabled.
		
		SET_BIT(TIM3->DIER, TIM_DIER_UIE); //enable interrupt
		SET_BIT(TIM3->DIER, TIM_DIER_CC1IE); //enable interrupt
		SET_BIT(TIM3->DIER, TIM_DIER_CC2IE); //enable interrupt
		
	}
}











void TIM3_IRQHandler(void)
{
  if(READ_BIT(TIM3->SR, TIM_SR_UIF)){
    CLEAR_BIT(TIM3->SR, TIM_SR_UIF);
	if(READ_REG(TIM3->ARR) == 18000){
		GPIOA->BSRR = (1<<6);
		TIM_DisableCounter(TIM3);
		GPIO_PA6_Mode(INPUT);
		TIM3_Mode(INPUT);
		TIM_EnableCounter(TIM3);
	}
  }



  if(READ_BIT(TIM3->SR, TIM_SR_CC1IF)){
    CLEAR_BIT(TIM3->SR, TIM_SR_CC1IF);
	
  }
  
  
  if(READ_BIT(TIM3->SR, TIM_SR_CC2IF)){
    CLEAR_BIT(TIM3->SR, TIM_SR_CC2IF);
	time[n] = READ_REG(TIM3->CCR2);
	n++;
	if(n >= 40){ 
		n = 0;
		TIM_DisableCounter(TIM3);
	}
  }  
}


















