#include "stm32f1xx.h"

void GPIO_EXTI_Init(void)
{
  RCC->APB2ENR |= RCC_APB2ENR_IOPBEN; //Тактирование GPIOB DHT22, Zero_Sensor
  RCC->APB2ENR |= RCC_APB2ENR_IOPCEN; // enable GPIOC LED
  RCC->APB2ENR |= RCC_APB2ENR_AFIOEN; //Тактирование AFIO
  
  GPIOC->CRH &= ~(GPIO_CRH_MODE13 | GPIO_CRH_CNF13); //reset all
  
  /*
    Настройка GPIO
      Пин: PB13
      Режим: Input floating (external pull-up 5KOhm)
	  
	  Пин: PC13 - LED
  */
  GPIOB->CRH &= ~(GPIO_CRL_MODE13 | GPIO_CRL_CNF13);
  GPIOB->CRH |= (0x01 << GPIO_CRH_CNF13_Pos); //Input floating (external pull-up 5KOhm)
  //GPIOB->ODR |= (1 << 0); //Подтяжка вверх
  
  //MODE: output 2 MHz
  //CNF: push-pull mode
  GPIOC->CRH |= (0x02 << GPIO_CRH_MODE13_Pos) | (0x00 << GPIO_CRH_CNF13_Pos);
  
  
  
  /*
    Настройка EXTI
  */
  AFIO->EXTICR[13] &= ~(AFIO_EXTICR4_EXTI13); //13 канал EXTI подключен к порту PB13
  
  EXTI->RTSR |= EXTI_RTSR_TR13; //Прерывание по нарастанию импульса
  EXTI->FTSR |= EXTI_FTSR_TR13; //Прерывание по спаду импульса
  
  EXTI->PR = EXTI_PR_PR13;      //Сбрасываем флаг прерывания 
                               //перед включением самого прерывания
  EXTI->IMR |= EXTI_IMR_MR13;   //Включаем прерывание 13-го канала EXTI
  
  NVIC_EnableIRQ(EXTI13_IRQn);  //Разрешаем прерывание в контроллере прерываний 13 канал
}




void PortSetHi(void)
{
  GPIOC->BSRR = (1<<13); //GPIOB->ODR |= (1<<13);
}


void PortSetLow(void)
{
  GPIOC->BRR = (1<<13);//GPIOB->ODR &= ~(1<<13);
}



void EXTI13_IRQHandler(void)
{
  EXTI->PR = EXTI_PR_PR13; //Сбрасываем флаг прерывания 
  asm("nop");
  asm("nop");
  asm("nop");
  asm("nop");
  asm("nop");
}
