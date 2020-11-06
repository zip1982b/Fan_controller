#include "stm32f1xx.h"
#include "gpio.h"
void GPIO_EXTI_Init(void)
{
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN; //Тактирование GPIOA = PA6-DHT22
  RCC->APB2ENR |= RCC_APB2ENR_IOPBEN; //Тактирование GPIOB = PB12-Zero_Sensor, PB6-FAN
  RCC->APB2ENR |= RCC_APB2ENR_IOPCEN; // enable GPIOC LED
  RCC->APB2ENR |= RCC_APB2ENR_AFIOEN; //Тактирование AFIO
  
  /*
	Пин: PC13 - LED
	MODE: output 2 MHz
    CNF: push-pull mode
  */
  GPIOC->CRH &= ~(GPIO_CRH_MODE13 | GPIO_CRH_CNF13); //reset PC13
  GPIOC->CRH |= (0x02 << GPIO_CRH_MODE13_Pos) | (0x00 << GPIO_CRH_CNF13_Pos);
  
  
  /*
    Настройка EXTI
  */
  //AFIO->EXTICR[13] &= ~(AFIO_EXTICR4_EXTI13); //13 канал EXTI подключен к порту PB13
  
  //EXTI->RTSR |= EXTI_RTSR_TR13; //Прерывание по нарастанию импульса
  //EXTI->FTSR |= EXTI_FTSR_TR13; //Прерывание по спаду импульса
  
  //EXTI->PR = EXTI_PR_PR13;      //Сбрасываем флаг прерывания 
                               //перед включением самого прерывания
  //EXTI->IMR |= EXTI_IMR_MR13;   //Включаем прерывание 13-го канала EXTI
  
  //NVIC_EnableIRQ(EXTI13_IRQn);  //Разрешаем прерывание в контроллере прерываний 13 канал
}

void GPIO_PA6_Mode(eMode mode){
	if(mode == OUTPUT){
		
		/*
		Настройка GPIO
		Пин: PA6 - DHT22
		MODE: output 2 MHz
		CNF: alternate Open-drain mode
		(external pull-up 10KOhm)
		*/
		//SET_BIT(GPIOA->CRL, GPIO_CRL_CNF6_0); // alternate open-drain
		//SET_BIT(GPIOA->CRL, GPIO_CRL_CNF6_1); // alternate open-drain
  
		SET_BIT(GPIOA->CRL, GPIO_CRL_CNF6_0); // CNF0 = 1 Open-drain
		CLEAR_BIT(GPIOA->CRL, GPIO_CRL_CNF6_1); // CNF1 = 0
  
		SET_BIT(GPIOA->CRL, GPIO_CRL_MODE6_0); // 1
		SET_BIT(GPIOA->CRL, GPIO_CRL_MODE6_1); // 1 MODE: output 50 MHz
  
		GPIOA->BSRR = (1<<6); //hi-z (1 - external pull-up 10KOhm)
	}
	else if(mode == INPUT){
		/*PA6 input floating*/
		SET_BIT(GPIOA->CRL, GPIO_CRL_CNF6_0); // 1
		CLEAR_BIT(GPIOA->CRL, GPIO_CRL_CNF6_1); // 0
  
		CLEAR_BIT(GPIOA->CRL, GPIO_CRL_MODE6_0);
		CLEAR_BIT(GPIOA->CRL, GPIO_CRL_MODE6_1); //input
	}
	
}


void PortSetHi(void)
{
  GPIOC->BSRR = (1<<13); //GPIOB->ODR |= (1<<13);
}


void PortSetLow(void)
{
  GPIOC->BRR = (1<<13);//GPIOB->ODR &= ~(1<<13);
}








/*
void EXTI13_IRQHandler(void)
{
  EXTI->PR = EXTI_PR_PR13; //Сбрасываем флаг прерывания 
  asm("nop");
  asm("nop");
  asm("nop");
  asm("nop");
  asm("nop");
   
}
*/