#ifndef MY_LEDS
#define MY_LEDS

#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"

typedef enum {
	GREEN ,ORANGE, RED, BLUE
} Led_color;

void LD_Init();
void LD_Set(Led_color i, int state); // state is 0 or 1
void LD_Reset(); // turn all off
void LD_Set_All(void);
#endif
