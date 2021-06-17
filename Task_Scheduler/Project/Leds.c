#include "Leds.h"

void LD_Init(){
	GPIO_InitTypeDef GPIO_InitStruct;
	__HAL_RCC_GPIOD_CLK_ENABLE();  // Enable clock to GPIO-D for LEDs
	// Set GPIOD Pins Parameters 
	GPIO_InitStruct.Pin     = GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;  // I/O PD12, PD13, PD14, and PD15
	GPIO_InitStruct.Mode    = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull    = GPIO_PULLDOWN;
	GPIO_InitStruct.Speed   = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);    // Init GPIOD 
}

void LD_Set(Led_color i, int state){
	
	switch(i){
		case 0:
			// green led
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, state);
			break;
		
		case 1:
			// Orange led
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, state);
			break;
		
		case 2:
			// red led
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, state);
			break;
		
		case 3:
			// blue led
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, state);
			break;
		
	}
}

void LD_Reset(){
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, 0);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, 0);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, 0);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, 0);
}

