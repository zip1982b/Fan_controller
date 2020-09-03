#include <stdint.h>


#include <inttypes.h>
#include <assert.h>
#include <string.h>

#include "cayenne_lpp.h"

#include "modemRHF78-052.h"
#include "uart.h"



  
/*
send at command: AT for checking interface
return:
	1 - ok (modem RHF78-052 is answered: +AT: OK)
	0 - not ok
*/
uint8_t send_AT_command(const char *commands){
	uint8_t n = strlen(commands);
	for(uint8_t i=0; i<=n-1; i++){
		char c = UART_PutC(2, commands[i]);
		if(c == commands[i]){
			continue;
		}
		else return 0;		
	}
	return 1;
}


/*
 return:
	1 - ok (modem RHF78-052 is answered: +AT: OK)
	0 - not ok
*/

uint8_t compare_answer(const char *correct_answer){
	uint16_t n = UART_BytesToRead(2);
	for(uint8_t i=0; i<=n-1; i++){
		char c = UART_GetC(2);
		if(c == correct_answer[i]){
			continue;
		}
		else return 0;
	}
	return 1;
}


uint16_t receive_data_from_APP(cayenne_lpp_t *lpp){
	uint16_t n = UART_BytesToRead(2);
	if(n){
		for(uint8_t i=0; i<=n-1; i++){
		lpp->buffer[lpp->cursor++] = UART_GetC(2);	
	}
	}
	return n;
}


void print_RX_buffer(void)
{
	uint8_t n = UART_BytesToRead(2);
	for(uint8_t i=0; i<=n-1; i++){
		char c = UART_GetC(2);
		printf("%c", c);
		
	}
	puts("\n");
}
