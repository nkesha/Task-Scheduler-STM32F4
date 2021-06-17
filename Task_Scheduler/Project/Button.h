#ifndef MY_BTN
#define MY_BTN

#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "timers.h"


#define DELAY_DEBOUNCE 1000
#define LONG_PRESS_DURATION 2000
#define DOUBLE_PRESS_DELAY 2000000
#define TIMEOUT_SEC 5
extern TaskHandle_t xButtonHandler;
extern TimerHandle_t buttonPressTimer;

typedef enum {
	SHORT_PRESS,
	LONG_PRESS,
	DOUBLE_PRESS
} bt_state;

typedef struct
{
	int numPresses;
	
} button;

void BT_Init();
void EXTI0_IRQHandler(void);
void buttonPressTimerCallback(TimerHandle_t buttonPressTimer);
void vTaskButton( void *pvParameters );

#endif
