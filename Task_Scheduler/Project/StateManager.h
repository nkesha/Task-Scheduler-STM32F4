#ifndef MY_STATE_MNGR
#define MY_STATE_MNGR

#include <stdio.h>
#include "FreeRTOS.h"
#include "Scheduler.h"
#include "Button.h"
#include "Leds.h"
#include "queue.h"

typedef enum {
	SELECT, PERFORM
} op_mode;

void vTaskOperator( void *pvParameters );				// Will listen on the input queue and manage the state of the program based on user input
void MN_Init();

#endif
