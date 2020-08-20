#include "stm32f1xx.h"
//Set HSE (8Mhz)
//returned:
//  0 - it is OK
//  1 - error HSE
//  2 - error PLL
int ClockInit(void)
{
  __IO int StartUpCounter;
  
  ////////////////////////////////////////////////////////////
  //Startup HSE
  ////////////////////////////////////////////////////////////
  
  RCC->CR |= (1<<RCC_CR_HSEON_Pos); // HSE on
  while (!(RCC->CR & (1<<RCC_CR_HSERDY_Pos)));
  
  /*
  for(StartUpCounter=0; ; StartUpCounter++)
  {
    if(RCC->CR & (1<<RCC_CR_HSERDY_Pos))
      break;
    
    //if not startup
    //switch off all
    //return error
    if(StartUpCounter > 0x2000)
    {
      RCC->CR &= ~(1<<RCC_CR_HSEON_Pos); // HSE
      return 1;
    }
  }
  */
  ////////////////////////////////////////////////////////////
  //Set and startup PLL
  ////////////////////////////////////////////////////////////
  
  //set PLL
  RCC->CFGR |= (0x07<<RCC_CFGR_PLLMULL_Pos) //PLL x9
            | (0x01<<RCC_CFGR_PLLSRC_Pos); //PLL from HSE
  
  
  RCC->CR |= (1<<RCC_CR_PLLON_Pos); //startup PLL
  
  //whait ...
  for(StartUpCounter=0; ; StartUpCounter++)
  {
    if(RCC->CR & (1<<RCC_CR_PLLRDY_Pos))
      break; // wery good
    
    //if not startup PLL, ??
    //switch off all
    //return error
    if(StartUpCounter > 0x2000)
    {
      RCC->CR &= ~(1<<RCC_CR_HSEON_Pos); //switch off HSE
      RCC->CR &= ~(1<<RCC_CR_PLLON_Pos); //switch off PLL
      return 2;
    }
  }
  
  ////////////////////////////////////////////////////////////
  //Set FLASH 
  ////////////////////////////////////////////////////////////
  
  //set 2 cycle waiting for Flash
  // 48 MHz < SYSCLK <= 72 MHz
  FLASH->ACR |= (0x02<<FLASH_ACR_LATENCY_Pos); 
  
  RCC->CFGR |= (0x00<<RCC_CFGR_PPRE2_Pos) // APB2 / 1 (72MHz)
            | (0x04<<RCC_CFGR_PPRE1_Pos) // APB1 / 2 (36MHz)
            | (0x00<<RCC_CFGR_HPRE_Pos); // AHB / 1 (72MHz)
  
  
  RCC->CFGR |= (0x02<<RCC_CFGR_SW_Pos); //from PLL
  //waiting
  while((RCC->CFGR & RCC_CFGR_SWS_Msk) != (0x02<<RCC_CFGR_SWS_Pos))
  {
  }
  
  //HSE-on HSI-off
  RCC->CR &= ~(1<<RCC_CR_HSION_Pos);
  
  return 0;
}



void RCC_DeInit(void){
	SET_BIT(RCC->CR, RCC_CR_HSION);
	while(READ_BIT(RCC->CR, RCC_CR_HSIRDY == RESET)) {}
	
	//reset calibration
	MODIFY_REG(RCC->CR, RCC_CR_HSITRIM, 0x80U);
	
	//clear all config
	CLEAR_REG(RCC->CFGR);
	while(READ_BIT(RCC->CFGR, RCC_CFGR_SWS) != RESET) {}
	
	CLEAR_BIT(RCC->CR, RCC_CR_PLLON);
	while(READ_BIT(RCC->CR, RCC_CR_PLLRDY) != RESET) {}
	
	
	CLEAR_BIT(RCC->CR, RCC_CR_HSEON | RCC_CR_CSSON);
	while(READ_BIT(RCC->CR, RCC_CR_HSERDY) != RESET) {}
	
	CLEAR_BIT(RCC->CR, RCC_CR_HSEBYP);
	
	SET_BIT(RCC->CSR, RCC_CSR_RMVF);
	
	//Disable all interrupts
	//CLEAR_REG(RCC->CIR);
	
}





void SetSysClockTo72(void){
	SET_BIT(RCC->CR, RCC_CR_HSEON);
	while(READ_BIT(RCC->CR, RCC_CR_HSERDY == RESET)) {}
	
	
	//Enable the Prefetch Buffer
	CLEAR_BIT(FLASH->ACR, FLASH_ACR_PRFTBE);
	SET_BIT(FLASH->ACR, FLASH_ACR_PRFTBE);
	MODIFY_REG(FLASH->ACR, FLASH_ACR_LATENCY, FLASH_ACR_LATENCY_2);
	
	
	
	MODIFY_REG(RCC->CFGR, RCC_CFGR_HPRE, RCC_CFGR_HPRE_DIV1);
	MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE2, RCC_CFGR_PPRE2_DIV1);
	MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE1, RCC_CFGR_PPRE1_DIV2);
	
	//MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE1, RCC_CFGR_PPRE1_DIV2);
	MODIFY_REG(RCC->CFGR, RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMULL,
				RCC_CFGR_PLLSRC | RCC_CFGR_PLLMULL9);
	
	SET_BIT(RCC->CR, RCC_CR_PLLON);
	while(READ_BIT(RCC->CR, RCC_CR_PLLRDY) != (RCC_CR_PLLRDY)) {}
	//while(READ_BIT(RCC->CR, RCC_CR_PLLRDY) != (RCC_CR_PLLRDY)) {}
	MODIFY_REG(RCC->CFGR, RCC_CFGR_SW, RCC_CFGR_SW_PLL);
	while(READ_BIT(RCC->CFGR, RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL) {}
 
 
 
}