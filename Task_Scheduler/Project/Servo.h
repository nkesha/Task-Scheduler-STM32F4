#ifndef MY_SERVO
#define MY_SERVO

#include "stm32f4xx_hal.h"

void SR_Init();
void SR_Set_position(int pos); // 0, 1, 2 or 3

#endif