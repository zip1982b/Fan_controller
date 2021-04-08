#ifndef __TIMER_H__
#define __TIMER_H__

//#define TIM_EnableIT_UPDATE(TIMx) SET_BIT(TIMx->DIER, TIM_DIER_UIE) // Update interrupt enable
//#define TIM_EnableIT_COMPARE(TIMx) SET_BIT(TIMx->DIER, TIM_DIER_CC1IE); // Capture/Compare 1 interrupt enable

#define TIM_EnableCounter(TIMx) SET_BIT(TIMx->CR1, TIM_CR1_CEN)
#define TIM_DisableCounter(TIMx) CLEAR_BIT(TIMx->CR1, TIM_CR1_CEN)

typedef enum {
	OUTPUT,
	INPUT
} eMode;


void print_array(uint8_t *array, size_t size_array);
void clear_array(uint8_t *array, size_t size_array);
uint8_t calc_crc(uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4);
float calc_humi(uint8_t byte1, uint8_t byte2);
float calc_temp(uint8_t byte3, uint8_t byte4);
uint8_t analis_data(uint8_t *data, uint8_t *delay);
void data_conversion(float *h, float *t);






void TIM3_Init(void);
void DHT22_START(void);
void enableINT(void);
void disableINT(void);
void get_data_from_DHT22(void);

#endif