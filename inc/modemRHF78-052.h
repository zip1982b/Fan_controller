#ifndef MODEM_RHF78_H
#define MODEM_RHF78_H

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif
  
  
uint8_t send_AT_command(const char *commands);

uint8_t compare_answer(const char *correct_answer);

void print_RX_buffer(void);

uint8_t receive_data_from_APP(cayenne_lpp_t *lpp, const char *message);


#endif /* MODEM_RHF78_H */