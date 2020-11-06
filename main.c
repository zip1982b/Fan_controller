#include "stm32f1xx.h"
#include "clock.h"
#include "gpio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "string.h"
#include "uart.h"
#include "timer.h"
#include "cayenne_lpp.h"
#include "modemRHF78-052.h"






xQueueHandle xQueue_for_send_to_APServer;
//xQueueHandle xQueue_received_data_from_app_server;
xQueueHandle xQueue_data_for_Fan;



void vFan(void *arg);
void vTemp_Humi_measurement(void *arg);
void vLoRaWAN_modem(void *arg);



static void _print_buffer(cayenne_lpp_t *lpp)
{
    for (uint8_t i = 0; i < lpp->cursor; ++i) {
        printf("%02X ", lpp->buffer[i]);
    }
    puts("");
}


static void get_payload_string(cayenne_lpp_t *lpp, char *payload_byte, char *payload_str)
{
    for (uint8_t i = 0; i < lpp->cursor; ++i) {
        sprintf(payload_byte, "%02X", lpp->buffer[i]);
		strcat(payload_str, payload_byte);
    }
}



static void _sprint_buffer(cayenne_lpp_t *lpp, char *payload_string, char *dst)
{
    for (uint8_t i = 0; i < lpp->cursor; ++i) {
        sprintf(payload_string, "%02X", lpp->buffer[i]);
		strcat(dst, payload_string);
    }
	strcat(dst, "\"\r\n");
}



//AT commands for modem
const  char check_link[] = "AT\r\n";
const  char answer_check_link[] = "+AT: OK\r\n";

#if defined CLASS_A || DEBUG
const  char set_DEVADDR[] = "AT+ID=DevAddr, \"014373d8\"\r\n";
const  char answer_DEVADDR[] = "+ID: DevAddr, 01:43:73:D8\r\n";

const  char set_devEUI[] = "AT+ID=DevEui, \"E49C8F9C70F2CD95\"\r\n";
const  char answer_devEUI[] = "+ID: DevEui, E4:9C:8F:9C:70:F2:CD:95\r\n";
#endif


#if defined CLASS_C || DEBUG_CLASS_C
const  char set_DEVADDR[] = "AT+ID=DevAddr, \"01855d9a\"\r\n";
const  char answer_DEVADDR[] = "+ID: DevAddr, 01:85:5D:9A\r\n";

const  char set_devEUI[] = "AT+ID=DevEui, \"797b87eb82453254\"\r\n";
const  char answer_devEUI[] = "+ID: DevEui, 79:7B:87:EB:82:45:32:54\r\n";
#endif


const  char EU433[] = "AT+DR=EU433\r\n";
const  char answer_EU433[] = "+DR: EU433\r\n";


const char set_channel0[] = "AT+CH=0,433.175,DR0,DR5\r\n";
const char answer_set_channel0[] = "+CH: 0,433175000,DR0:DR5\r\n";


const  char set_channel1[] = "AT+CH=1,433.375,DR0,DR5\r\n";
const  char answer_set_channel1[] = "+CH: 1,433375000,DR0:DR5\r\n";


const  char set_channel2[] = "AT+CH=2,433.575,DR0,DR5\r\n";
const  char answer_set_channel2[] = "+CH: 2,433575000,DR0:DR5\r\n";


const  char set_channel3[] = "AT+CH=3,433.775,DR0,DR5\r\n";
const  char answer_set_channel3[] = "+CH: 3,433775000,DR0:DR5\r\n";


const  char set_channel4[] = "AT+CH=4,433.975,DR0,DR5\r\n";
const  char answer_set_channel4[] = "+CH: 4,433975000,DR0:DR5\r\n";


const  char set_channel5[] = "AT+CH=5,434.175,DR0,DR5\r\n";
const  char answer_set_channel5[] = "+CH: 5,434175000,DR0:DR5\r\n";


const  char set_channel6[] = "AT+CH=6,434.375,DR0,DR5\r\n";
const  char answer_set_channel6[] = "+CH: 6,434375000,DR0:DR5\r\n";


const  char set_channel7[] = "AT+CH=7,434.575,DR0,DR5\r\n";
const  char answer_set_channel7[] = "+CH: 7,434575000,DR0:DR5\r\n";


const  char RXWIN2[] = "AT+RXWIN2=434.665,DR0\r\n";
const  char answer_RXWIN2[] = "+RXWIN2: 434665000,DR0\r\n";


const  char default_Data_Rate[] = "AT+DR=DR0\r\n";
const  char answer_default_Data_Rate[] = "+DR: EU433 DR0 SF12 BW125K\r\n";


const  char default_Power[] = "AT+POWER=12\r\n";
const  char answer_default_Power[] = "+POWER: 12\r\n";
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#if defined CLASS_A || DEBUG
const  char Port[] = "AT+PORT=1\r\n";
const  char answer_Port[] = "+PORT: 1\r\n";
#endif

#if defined CLASS_C || DEBUG_CLASS_C
const  char Port[] = "AT+PORT=2\r\n";
const  char answer_Port[] = "+PORT: 2\r\n";
#endif



const  char ADR[] = "AT+ADR=ON\r\n";
const  char answer_ADR[] = "+ADR: ON\r\n";


//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#if defined CLASS_A || DEBUG
const  char NwkSKey[] = "AT+KEY=NwkSKey, \"a3b6555a4525d72976ee2aa991fa4c5e\"\r\n";
const  char answer_NwkSKey[] = "+KEY: NWKSKEY A3B6555A4525D72976EE2AA991FA4C5E\r\n";

const  char AppSKey[] = "AT+KEY=APPSKEY, \"f8adb8ac7fea829dd5cefc00a72c55d7\"\r\n";
const  char answer_AppSKey[] = "+KEY: APPSKEY F8ADB8AC7FEA829DD5CEFC00A72C55D7\r\n";

const  char Set_Class[] = "AT+CLASS=A\r\n";
const  char answer_Class[] = "+CLASS: A\r\n";
#endif

#if defined CLASS_C || DEBUG_CLASS_C
const  char NwkSKey[] = "AT+KEY=NwkSKey, \"1a5399cc6af803d2b86202989607a226\"\r\n";
const  char answer_NwkSKey[] = "+KEY: NWKSKEY 1A5399CC6AF803D2B86202989607A226\r\n";

const  char AppSKey[] = "AT+KEY=APPSKEY, \"a8754af152c1000f949d8c0d57813d57\"\r\n";
const  char answer_AppSKey[] = "+KEY: APPSKEY A8754AF152C1000F949D8C0D57813D57\r\n";

const  char Set_Class[] = "AT+CLASS=C\r\n";
const  char answer_Class[] = "+CLASS: C\r\n";
#endif

const char messages_from_APP[] = "+MSG: PORT: 2; RX: \"\n";


static const UARTInitStructure_t UARTInitStr = 
{
  .bus_freq = 36000000,
  .baud = 9600,
  .data_bits = 8,
  .stop_bits = 1,
  .parity = 0,
};



int main()
{
  RCC_DeInit();
  if(!ClockInit()){
    GPIO_EXTI_Init(); 
	UART_Init(2, &UARTInitStr);
	TIM3_Init(); // 
	
	//xQueue_for_send_to_APServer = xQueueCreate(5, sizeof(cayenne_lpp_t));
	//xQueue_received_data_from_app_server = xQueueCreate(5, sizeof(cayenne_lpp_t));
	//xQueue_data_for_Fan = xQueueCreate(5, sizeof(cayenne_lpp_t));
	
    xTaskCreate(vTemp_Humi_measurement, "temperature and humidity measurement", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
    xTaskCreate(vFan, "fan operation", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
    //xTaskCreate(vLoRaWAN_modem, "RHF78-052 operation", 512, NULL, 2, NULL);
    
    vTaskStartScheduler();
  }
  else return 0;
  
  while(1){
    
  }
}



/**********TASKS***************/
void vFan(void *arg){
  while(1){
    PortSetHi();
    vTaskDelay(500 / portTICK_RATE_MS);
    PortSetLow();
    vTaskDelay(500 / portTICK_RATE_MS);
  }
}



void vTemp_Humi_measurement(void *arg){
	
	//cayenne_lpp_t temp_humi = { 0 }; //starting init
	//float celsius = 0.2;
	//float humidity = 20.5;
	
	//vTaskDelay(10000 / portTICK_RATE_MS);
	
	
	GPIO_PA6_Mode(OUTPUT); // 1 - hi-z
	vTaskDelay(2000 / portTICK_RATE_MS);
	TIM3_Mode(OUTPUT);
	GPIOA->BRR = (1<<6); // 0 - Start signal
	TIM_EnableCounter(TIM3); // 18 mSec delay
	
  while(1){
	/*  
	cayenne_lpp_add_temperature(&temp_humi, 1, celsius); //  |1|103|d|d|dec or |1|67|x|x|hex
	cayenne_lpp_add_relative_humidity(&temp_humi, 1, humidity); //  |1|104|d|d|dec or |1|68|x|x|hex
	xQueueSendToBack(xQueue_for_send_to_APServer, &temp_humi, 100/portTICK_RATE_MS); // to APP server
	//xQueueSendToBack(xQueue_data_for_Fan, &temp_humi, 100/portTICK_RATE_MS); // to FAN
	cayenne_lpp_reset(&temp_humi); */
	vTaskDelay(10000 / portTICK_RATE_MS);
	
	
          GPIO_PA6_Mode(OUTPUT); // 1 - hi-z
          vTaskDelay(3000 / portTICK_RATE_MS);
          TIM3_Mode(OUTPUT);
          vTaskDelay(2000 / portTICK_RATE_MS);
          GPIOA->BRR = (1<<6); // 0 - Start signal
          TIM_EnableCounter(TIM3); // 18 mSec delay
       
        
	
	
	//celsius = celsius + 0.1;	// measurement temp
	//humidity = humidity + 2.5;	// measurement rh
  }
}





void vLoRaWAN_modem(void *arg){
	uint8_t settings_is_ok = 0;
	
    //setting up the modem, only once.
	
	UART_ReadBuffClear(2);
	UART_WriteBuffClear(2);
	if(send_AT_command(check_link)){
		vTaskDelay(500);
		if(compare_answer(answer_check_link)){
            #if defined DEBUG || DEBUG_CLASS_C
			printf("UART link is ok\n");
            #endif
            #if defined CLASS_A || CLASS_C
			settings_is_ok++; // 1
            #endif
		}
		else {
			#if defined DEBUG || DEBUG_CLASS_C
			printf("link is not ok\n");
			#endif
			#if defined CLASS_A || CLASS_C
			asm("nop");
			#endif
		}
	}
	
	
	UART_ReadBuffClear(2);
	UART_WriteBuffClear(2);
	if(send_AT_command(set_DEVADDR)){
		vTaskDelay(500);
		if(compare_answer(answer_DEVADDR)){
			#if defined DEBUG || DEBUG_CLASS_C
			printf("device address is ok\n");
			#endif
			#if defined CLASS_A || CLASS_C
			settings_is_ok++; // 2
			#endif
		}
        else {
			#if defined DEBUG || DEBUG_CLASS_C
			printf("device address is not ok\n");
			#endif
			#if defined CLASS_A || CLASS_C
			asm("nop");
			#endif
		}
	}

	UART_ReadBuffClear(2);
	UART_WriteBuffClear(2);
	if(send_AT_command(set_devEUI)){
		vTaskDelay(500);
		if(compare_answer(answer_devEUI)){
			#if defined DEBUG || DEBUG_CLASS_C
			printf("dev EUI is ok\n");
			#endif
			#if defined CLASS_A || CLASS_C
			settings_is_ok++; // 3
			asm("nop");
			#endif
		}
        else {
			#if defined DEBUG || DEBUG_CLASS_C
			printf("dev EUI is not ok\n");
			#endif
			#if defined CLASS_A || CLASS_C
			asm("nop");
			#endif
		}
	}
	
	UART_ReadBuffClear(2);
	UART_WriteBuffClear(2);  
	if(send_AT_command(EU433)){
		vTaskDelay(500);
		if(compare_answer(answer_EU433)){
			#if defined DEBUG || DEBUG_CLASS_C
			printf("EU433 is ok\n");
			#endif
			#if defined CLASS_A || CLASS_C
			settings_is_ok++; // 4
			asm("nop");
			#endif
		}
        else {
			#if defined DEBUG || DEBUG_CLASS_C
			printf("EU433 is not ok\n");
			#endif
			#if defined CLASS_A || CLASS_C
			asm("nop");
			#endif
		}
	}
   
	UART_ReadBuffClear(2);
	UART_WriteBuffClear(2);  
	if(send_AT_command(set_channel0)){
		vTaskDelay(500);
		if(compare_answer(answer_set_channel0)){
			#if defined DEBUG || DEBUG_CLASS_C
			printf("set_channel0 is ok\n");
			#endif
			#if defined CLASS_A || CLASS_C
			settings_is_ok++; // 5
			asm("nop");
			#endif
			
		}
        else {
			#if defined DEBUG || DEBUG_CLASS_C
			printf("set_channel0 is not ok\n");
			#endif
			#if defined CLASS_A || CLASS_C
			asm("nop");
			#endif
		}
	}
	
	UART_ReadBuffClear(2);
	UART_WriteBuffClear(2);  
	if(send_AT_command(set_channel1)){
		vTaskDelay(500);
		if(compare_answer(answer_set_channel1)){
			#if defined DEBUG || DEBUG_CLASS_C
			printf("set_channel1 is ok\n");
			#endif
			#if defined CLASS_A || CLASS_C
			settings_is_ok++; // 6
			asm("nop");
			#endif
		}
        else {
			#if defined DEBUG || DEBUG_CLASS_C
			printf("set_channel1 is not ok\n");
			#endif
			#if defined CLASS_A || CLASS_C
			settings_is_ok++; 
			asm("nop");
			#endif
		}
	}
	
	UART_ReadBuffClear(2);
	UART_WriteBuffClear(2);  
	if(send_AT_command(set_channel2)){
		vTaskDelay(500);
		if(compare_answer(answer_set_channel2)){
			#if defined DEBUG || DEBUG_CLASS_C
			printf("set_channel2 is ok\n");
			#endif
			#if defined CLASS_A || CLASS_C
			settings_is_ok++; // 7
			asm("nop");
			#endif
		}
        else{
			#if defined DEBUG || DEBUG_CLASS_C
			printf("set_channel2 is not ok\n");
			#endif
			#if defined CLASS_A || CLASS_C
			asm("nop");
			#endif
		}
	}
	
	UART_ReadBuffClear(2);
	UART_WriteBuffClear(2);  
	if(send_AT_command(set_channel3)){
		vTaskDelay(500);
		if(compare_answer(answer_set_channel3)){
			#if defined DEBUG || DEBUG_CLASS_C
			printf("set_channel3 is ok\n");
			#endif
			#if defined CLASS_A || CLASS_C
			settings_is_ok++; // 8
			asm("nop");
			#endif
		}
        else {
			#if defined DEBUG || DEBUG_CLASS_C
			printf("set_channel3 is not ok\n");
			#endif
			#if defined CLASS_A || CLASS_C
			asm("nop");
			#endif
		}
	}
	
	UART_ReadBuffClear(2);
	UART_WriteBuffClear(2);  
	if(send_AT_command(set_channel4)){
		vTaskDelay(500);
		if(compare_answer(answer_set_channel4)){
			#if defined DEBUG || DEBUG_CLASS_C
			printf("set_channel4 is ok\n");
			#endif
			#if defined CLASS_A || CLASS_C
			settings_is_ok++; // 9
			asm("nop");
			#endif
		}
        else {
			#if defined DEBUG || DEBUG_CLASS_C
			printf("set_channel4 is not ok\n");
			#endif
			#if defined CLASS_A || CLASS_C
			asm("nop");
			#endif
		}
	}
	
	UART_ReadBuffClear(2);
	UART_WriteBuffClear(2);  
	if(send_AT_command(set_channel5)){
		vTaskDelay(500);
		if(compare_answer(answer_set_channel5)){
			#if defined DEBUG || DEBUG_CLASS_C
			printf("set_channel5 is ok\n");
			#endif
			#if defined CLASS_A || CLASS_C
			settings_is_ok++; // 10
			asm("nop");
			#endif
		}
        else {
			#if defined DEBUG || DEBUG_CLASS_C
			printf("set_channel5 is not ok\n");
			#endif
			#if defined CLASS_A || CLASS_C
			asm("nop");
			#endif
		}
	}

	UART_ReadBuffClear(2);
	UART_WriteBuffClear(2);
	if(send_AT_command(set_channel6)){
		vTaskDelay(500);
		if(compare_answer(answer_set_channel6)){
			#if defined DEBUG || DEBUG_CLASS_C
			printf("set_channel6 is ok\n");
			#endif
			#if defined CLASS_A || CLASS_C
			settings_is_ok++; // 11
			asm("nop");
			#endif
		}
        else{
			#if defined DEBUG || DEBUG_CLASS_C
			printf("set_channel6 is not ok\n");
			#endif
			#if defined CLASS_A || CLASS_C
			asm("nop");
			#endif
		}
	}
	
	UART_ReadBuffClear(2);
	UART_WriteBuffClear(2);
	if(send_AT_command(set_channel7)){
		vTaskDelay(500);
		if(compare_answer(answer_set_channel7)){
			#if defined DEBUG || DEBUG_CLASS_C
			printf("set_channel7 is ok\n");
			#endif
			#if defined CLASS_A || CLASS_C
			settings_is_ok++; // 12
			asm("nop");
			#endif
		}
        else {
			#if defined DEBUG || DEBUG_CLASS_C
			printf("set_channel7 is not ok\n");
			#endif
			#if defined CLASS_A || CLASS_C
			asm("nop");
			#endif
		}
	}
	
	
	UART_ReadBuffClear(2);
	UART_WriteBuffClear(2);	
	if(send_AT_command(RXWIN2)){
		vTaskDelay(500);
		if(compare_answer(answer_RXWIN2)){
			#if defined DEBUG || DEBUG_CLASS_C
			printf("RXWIN2 is ok\n");
			#endif
			#if defined CLASS_A || CLASS_C
			settings_is_ok++; // 13
			asm("nop");
			#endif
		}
        else {
			#if defined DEBUG || DEBUG_CLASS_C
			printf("RXWIN2 is not ok\n");
			#endif
			#if defined CLASS_A || CLASS_C
			asm("nop");
			#endif
		}
	}
  
	UART_ReadBuffClear(2);
	UART_WriteBuffClear(2);
	if(send_AT_command(default_Data_Rate)){
		vTaskDelay(500);
		if(compare_answer(answer_default_Data_Rate)){
			#if defined DEBUG || DEBUG_CLASS_C
			printf("default_Data_Rate is ok\n");
			#endif
			#if defined CLASS_A || CLASS_C
			settings_is_ok++; // 14
			asm("nop");
			#endif
		}
        else {
			#if defined DEBUG || DEBUG_CLASS_C
			printf("default_Data_Rate is not ok\n");
			#endif
			#if defined CLASS_A || CLASS_C
			asm("nop");
			#endif
		}
	}

	UART_ReadBuffClear(2);
	UART_WriteBuffClear(2);
	if(send_AT_command(default_Power)){
		vTaskDelay(500);
		if(compare_answer(answer_default_Power)){
			#if defined DEBUG || DEBUG_CLASS_C
			printf("default_Power is ok\n");
			#endif
			#if defined CLASS_A || CLASS_C
			settings_is_ok++; // 15
			asm("nop");
			#endif
		}
        else {
			#if defined DEBUG || DEBUG_CLASS_C
			printf("default_Power is not ok\n");
			#endif
			#if defined CLASS_A || CLASS_C
			asm("nop");
			#endif
		}
	}
	
	UART_ReadBuffClear(2);
	UART_WriteBuffClear(2);
	if(send_AT_command(ADR)){
		vTaskDelay(500);
		if(compare_answer(answer_ADR)){
			#if defined DEBUG || DEBUG_CLASS_C
			printf("ADR is ok\n");
			#endif
			#if defined CLASS_A || CLASS_C
			settings_is_ok++; // 16
			asm("nop");
			#endif
		}
        else {
			#if defined DEBUG || DEBUG_CLASS_C
			printf("ADR is not ok\n");
			#endif
			#if defined CLASS_A || CLASS_C
			asm("nop");
			#endif
		}
	}
	
	UART_ReadBuffClear(2);
	UART_WriteBuffClear(2);
	if(send_AT_command(NwkSKey)){
		vTaskDelay(500);
		if(compare_answer(answer_NwkSKey)){
			#if defined DEBUG || DEBUG_CLASS_C
			printf("NwkSKey is ok\n");
			#endif
			#if defined CLASS_A || CLASS_C
			settings_is_ok++; // 17
			asm("nop");
			#endif
		}
        else {
			#if defined DEBUG || DEBUG_CLASS_C
			printf("NwkSKey is not ok\n");
			#endif
			#if defined CLASS_A || CLASS_C
			asm("nop");
			#endif
		}
	}	
 
 
	UART_ReadBuffClear(2);
	UART_WriteBuffClear(2);
	if(send_AT_command(AppSKey)){
		vTaskDelay(500);
		if(compare_answer(answer_AppSKey)){
			#if defined DEBUG || DEBUG_CLASS_C
			printf("AppSKey is ok\n");
			#endif
			#if defined CLASS_A || CLASS_C
			settings_is_ok++; // 18
			asm("nop");
			#endif
			
		}
        else {
			#if defined DEBUG || DEBUG_CLASS_C
			printf("AppSKey is not ok\n");
			#endif
			#if defined CLASS_A || CLASS_C
			asm("nop");
			#endif
			
		}
	}
  
  
	UART_ReadBuffClear(2);
	UART_WriteBuffClear(2);
	if(send_AT_command(Port)){
		vTaskDelay(500);
		if(compare_answer(answer_Port)){
			#if defined DEBUG || DEBUG_CLASS_C
			printf("Port is ok\n");
			#endif
			#if defined CLASS_A || CLASS_C
			settings_is_ok++; // 19
			#endif
		}
        else {
			#if defined DEBUG || DEBUG_CLASS_C
			printf("Port is not ok\n");
			#endif
			#if defined CLASS_A || CLASS_C
			asm("nop");
			#endif
		}
	}
  
	UART_ReadBuffClear(2);
	UART_WriteBuffClear(2);
	if(send_AT_command(Set_Class)){
		vTaskDelay(500);
		if(compare_answer(answer_Class)){
			#if defined DEBUG || DEBUG_CLASS_C
			printf("Class set is ok\n");
			#endif
			#if defined CLASS_A || CLASS_C
			settings_is_ok++; // 20
			asm("nop");
			#endif
			
		}
        else {
			#if defined DEBUG || DEBUG_CLASS_C
			printf("Class set is not ok\n");
			#endif
			#if defined CLASS_A || CLASS_C
			asm("nop");
			#endif
		}
	}
	
// end set up
	
	cayenne_lpp_t payload_for_send_to_app = { 0 };
	char payload_byte[1] = {0};
	char payload_str[20] = {0};
	char result[CAYENNE_LPP_MAX_BUFFER_SIZE] = {0};
	char receivied_string[50] = {0};
	cayenne_lpp_t payload = { 0 };
	uint8_t data_from_APP = 0;
	uint8_t len = 0;
	uint8_t value = 0;
	UART_ReadBuffClear(2);
	UART_WriteBuffClear(2);
	
  while(1){
	  // waiting for data
	if(xQueueReceive(xQueue_for_send_to_APServer, &payload_for_send_to_app, 50/portTICK_RATE_MS) == pdPASS){
		#if defined DEBUG || DEBUG_CLASS_C
		_print_buffer(&payload_for_send_to_app);
		#endif
		//_print_buffer(&payload_for_send_to_app);
		
		get_payload_string(&payload_for_send_to_app, payload_byte, payload_str);
	    //printf("payload_str - %s\n", payload_str);
		
		//_sprint_buffer(&payload_for_send_to_app, payload_str, AT_CMSGHEX);
		cayenne_lpp_reset(&payload_for_send_to_app); 
		
		sprintf(result, "AT+CMSGHEX=\"%s\"\r\n", payload_str);
	    //printf("result - %s\n", result);
		send_AT_command(result);
		
	    memset (payload_str, '\0', 20);
	    memset (result, '\0', CAYENNE_LPP_MAX_BUFFER_SIZE);
	}
	//print_RX_buffer();
	vTaskDelay(1500 / portTICK_RATE_MS);
	
	
	len = strlen(receivied_string);
	if((len>=15) && (receivied_string[len-1]=='\n')){
		//printf("String is received!\n");
		printf("%s\n", receivied_string);
		//UART_ReadBuffClear(2);
		if((receivied_string[12] == '2') && (receivied_string[19] == '\"') && receivied_string[len-3]=='\"'){
			//channel
			if(receivied_string[20] == '0' && receivied_string[21] == '2' && receivied_string[22] == '0' && receivied_string[23] == '1'){
				value = receivied_string[25] - '0';
				cayenne_lpp_add_digital_output(&payload, 2, value);
			}
			
			_print_buffer(&payload);
			xQueueSendToBack(xQueue_data_for_Fan, &payload, 100/portTICK_RATE_MS);
			cayenne_lpp_reset(&payload);
			
		}
		//UART_ReadBuffClear(2);
		memset (receivied_string, '\0', 50);
	}
	else{
		UART_GetS(receivied_string, (uint8_t)strlen(receivied_string));
		//printf("line filling = %s\n", receivied_string);
	}
	//vTaskDelay(3000 / portTICK_RATE_MS);
  }
}



