#include "stm32f1xx.h"

void PortInit(void)
{
  RCC->APB2ENR |= RCC_APB2ENR_IOPCEN; // enable GPIOC
  
  GPIOC->CRH &= ~(GPIO_CRH_MODE13 | GPIO_CRH_CNF13); //reset all
  
  //MODE: output 2 MHz
  //CNF: push-pull mode
  GPIOC->CRH |= (0x02 << GPIO_CRH_MODE13_Pos) | (0x00 << GPIO_CRH_CNF13_Pos);
}

void PortSetHi(void)
{
  GPIOC->BSRR = (1<<13); //GPIOB->ODR |= (1<<13);
}


void PortSetLow(void)
{
  GPIOC->BRR = (1<<13);//GPIOB->ODR &= ~(1<<13);
}


