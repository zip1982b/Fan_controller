#ifndef __GPIO_H__
#define __GPIO_H__

typedef enum {
	OUTPUT,
	INPUT
} eMode;


void GPIO_EXTI_Init(void);

void GPIO_PA6_Mode(eMode mode);

void PortSetHi(void);

void PortSetLow(void);

#endif