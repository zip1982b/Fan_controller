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
	char c;
	for(uint8_t i=0; i<=n-1; i++){
		c = UART_GetC(2);
		if(c == correct_answer[i]){
			continue;
		}
		else return 0;
	}
	return 1;
}


uint8_t receive_data_from_APP(cayenne_lpp_t *lpp, const char *message){
	uint16_t n = 0;
	char received_symbol;
	n = UART_BytesToRead(2);
	if(n){
		for(uint8_t i=0; i<=n-1; i++){
			received_symbol = (char)UART_GetC(2);
			if(message[i] == received_symbol){
				if(i <= 19){
					continue;
				}
				else if(received_symbol != '\"')
					lpp->buffer[lpp->cursor++] = (uint8_t)received_symbol;
				return 1;
			}
			else continue;	
		}
	}
	return 0;
}





void print_RX_buffer(void)
{
	uint8_t n = UART_BytesToRead(2);
	
	if(n){
		printf("length = %d\n", n);
		for(uint8_t i=0; i<=n; i++){ //n-1 ???
			uint8_t ascii_code = UART_GetC(2);
			if((ascii_code > 0) && (ascii_code != 255)){
				printf("%d ", ascii_code);
			}
		}
		puts("\n");	//???
	}
}




void UART_GetS(char *string, uint8_t length_string){
    uint8_t bytes_to_read = UART_BytesToRead(2);
    uint8_t ascii_code = 0;
    if(bytes_to_read){
        for(uint8_t i=0; i<bytes_to_read; i++){
            ascii_code = UART_GetC(2);
            if((ascii_code > 0) && (ascii_code < 128)){
                string[i+length_string] = (char)ascii_code;
            }
            if(ascii_code == 10){
                break;
            }
        }
    }
}

































