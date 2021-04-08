#include "FreeRTOS.h"
#include "string.h"
#include "task.h"
#include "stm32f1xx.h"
#include "timer.h"



uint8_t delay[42] = {0};
uint8_t dt[5];
uint8_t n = 0;




void print_array(uint8_t *array, size_t size_array){
    for(uint8_t i=0; i<size_array; i++){
        printf("%d ", array[i]);
    }
	printf("\n");
}



void clear_array(uint8_t *array, size_t size_array){
    for(uint8_t i=0; i<size_array; i++){
        array[i] = 0;
        //printf("clear array %d\n", array[i]);
    }
}




uint8_t calc_crc(uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4){
    return byte1 + byte2 + byte3 + byte4;
}




float calc_humi(uint8_t byte1, uint8_t byte2){
	uint16_t sum = (byte1 * 256) + byte2;
     return  (float)(sum * 0.1);
}

float calc_temp(uint8_t byte3, uint8_t byte4){
    if(byte3 == 128){
        return (((byte3*256) + byte4) * 0.1) * -1;
    }
    else return ((byte3*256) + byte4) * 0.1;
}



uint8_t analis_data(uint8_t *data, uint8_t *delay){
	uint8_t i, j, k = 0;
	//rcv bytes
	for (j=0; j<5; j++){
		//data[j]=0;
		
		//analis bits
		for(i=0; i<8; i++){
		    //printf("delay[k+i+2] = %d\n", delay[k+i+2]);
		    //printf("k=%d, i=%d \n", k, i);
		   
			if(delay[k+i+2]>30){
				data[j] |= (1<<(7-i));
			}
			else if(delay[k+i+2]==0){
			    //printf("return, data from dht22 is not correct\n");
			    return 0;
			}
			
		}
		
		k = k + 8;
		
		//printf("byte%d = %d\n", j+1, data[j]);
		//printf("_____________________________\n");
	}
	return 1;
}



void data_conversion(float *hu, float *te){
	float h = 0.0;
	float t = 0.0;
	uint8_t crc = 0;
	if(analis_data(dt, delay))
    {
        //printf("data from dht22 is ok\n");
		crc = calc_crc(dt[0], dt[1], dt[2], dt[3]);
        if(dt[4] == crc){
            //printf("crc is correct\n");
			//printf("crc = %d\n", crc);
            h = calc_humi(dt[0], dt[1]);
            t = calc_temp(dt[2], dt[3]);
			*hu = h;
			*te = t;
			
            printf("H = %.1f \%\n", h);
            printf("T = %.1f C\n", t);
        }
        else {
			printf("crc is not correct\n");
			printf("crc = %d\n", crc);
			printf("dt[4] = %d\n", dt[4]);
		}
        
        
        
/*******************************************************************************/        
        //printf("clear delay\n");
        size_t n = sizeof(delay) / sizeof(uint8_t);
        //printf("size arry = %zd\n", n);
        //print_array(delay, n);
        //printf("____________\n");
        clear_array(delay, n);
/*******************************************************************************/        
        //printf("clear dt\n");
        n = sizeof(dt) / sizeof(uint8_t);
        //printf("size arry = %zd\n", n);
        //print_array(dt, n);
        //printf("__________\n");
        clear_array(dt, n);
    }
    else printf("data from dht22 is NOK\n");
}



void TIM3_Init()
{
  SET_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM3EN); 
  NVIC_EnableIRQ(TIM3_IRQn);
  WRITE_REG(TIM3->PSC, 71); //f=1MHz
  WRITE_REG(TIM3->ARR, 65000); // T=1uS, 
  
  
  
  
  //pwm input
  SET_BIT(TIM3->CCMR1, TIM_CCMR1_CC1S_0); //
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
		
		enableINT(); ///////!!!!!!!!!!!!!!!!!!!!!!!!!!
		
		SET_BIT(DBGMCU->CR, DBGMCU_CR_DBG_TIM3_STOP); //TIM3 counter stopped when core is halted
}

void enableINT(void){
	SET_BIT(TIM3->DIER, TIM_DIER_UIE); //enable interrupt
	SET_BIT(TIM3->DIER, TIM_DIER_CC1IE); //enable interrupt
	SET_BIT(TIM3->DIER, TIM_DIER_CC2IE); //enable interrupt
}

void disableINT(void){
	CLEAR_BIT(TIM3->DIER, TIM_DIER_UIE); //disable interrupt
	CLEAR_BIT(TIM3->DIER, TIM_DIER_CC1IE); //disable interrupt
	CLEAR_BIT(TIM3->DIER, TIM_DIER_CC2IE); //disable interrupt
}


void DHT22_START(void){
	GPIOA->BRR = (1<<6);// set low
	vTaskDelay(5 / portTICK_RATE_MS);
	GPIOA->BSRR = (1<<6);// release line - high
}


void get_data_from_DHT22(void){
	outputOpenDrainPA6(); 	// output open-drain PA6
	DHT22_START(); 			// 5 ms low signal for dht22
	inputFloatingPA6(); 	// input floating PA6
	enableINT();
	TIM_EnableCounter(TIM3);// receive data from DHT22
	vTaskDelay(300 / portTICK_RATE_MS);
	
}




void TIM3_IRQHandler(void)
{
  uint8_t data = 0;
  if(READ_BIT(TIM3->SR, TIM_SR_UIF)){
    CLEAR_BIT(TIM3->SR, TIM_SR_UIF);
  }


  if(READ_BIT(TIM3->SR, TIM_SR_CC1IF)){
    CLEAR_BIT(TIM3->SR, TIM_SR_CC1IF);

  }
  
  

  if(READ_BIT(TIM3->SR, TIM_SR_CC2IF)){
    CLEAR_BIT(TIM3->SR, TIM_SR_CC2IF);
	data = READ_REG(TIM3->CCR2);
	if(data){
		delay[n] = READ_REG(TIM3->CCR2);
		n++;
	}
	
	if(n > 42){ 
		n = 0;
		TIM_DisableCounter(TIM3);
		disableINT();
	}
  }
}





