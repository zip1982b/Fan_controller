#include "stm32f1xx.h"
#include "gpio.h"


void GPIO_EXTI_Init(void)
{
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN; //Тактирование GPIOA = PA6-TIM3 capture
	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN; //Тактирование GPIOB = PB12-Meandr
	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN; // enable GPIOC LED
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN; //Тактирование AFIO

	outputOpenDrainPA6();

	/*
	Пин: PC13 - LED
	MODE: output 2 MHz
    CNF: push-pull mode
	*/
	GPIOC->CRH &= ~(GPIO_CRH_MODE13 | GPIO_CRH_CNF13); //reset PC13
	GPIOC->CRH |= (0x02 << GPIO_CRH_MODE13_Pos) | (0x00 << GPIO_CRH_CNF13_Pos);
}


void outputOpenDrainPA6(void)
{
	/*
    Настройка GPIO
    Пин: PA6 - DHT22
	MODE: output 2 MHz
	CNF: open drain mode
    (external pull-up 5 KOhm)
	*/
  SET_BIT(GPIOA->CRL, GPIO_CRL_CNF6_0); // 1 - output open-drain
  CLEAR_BIT(GPIOA->CRL, GPIO_CRL_CNF6_1); // 0 - output open-drain
  
  SET_BIT(GPIOA->CRL, GPIO_CRL_MODE6_0); // 1
  SET_BIT(GPIOA->CRL, GPIO_CRL_MODE6_1); // 1 MODE: output 50 MHz
  
  GPIOA->BSRR = (1<<6); // high 
}



void inputFloatingPA6(void)
{
	/*input floating PA6*/
	SET_BIT(GPIOA->CRL, GPIO_CRL_CNF6_0);// 1
	CLEAR_BIT(GPIOA->CRL, GPIO_CRL_CNF6_1);// 0
  
	CLEAR_BIT(GPIOA->CRL, GPIO_CRL_MODE6_0);
	CLEAR_BIT(GPIOA->CRL, GPIO_CRL_MODE6_1);//input
}


void PortSetHi(void)
{
  GPIOC->BSRR = (1<<13); //GPIOB->ODR |= (1<<13);
}


void PortSetLow(void)
{
  GPIOC->BRR = (1<<13);//GPIOB->ODR &= ~(1<<13);
}


