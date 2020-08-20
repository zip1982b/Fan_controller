#include "clock.h"
#include "gpio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"



void vMCU_worked(void *arg);



int main()
{
  RCC_DeInit();
  if(!ClockInit()){
    PortInit();
    xTaskCreate(vMCU_worked, "MCU worked", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
    
    vTaskStartScheduler();
  }
  else return 0;
  
  while(1){
    
  }
}



/**********TASKS***************/
void vMCU_worked(void *arg){
  
  while(1){
    PortSetHi();
    vTaskDelay(1000 / portTICK_RATE_MS);
    PortSetLow();
    vTaskDelay(1000 / portTICK_RATE_MS);
  }
}