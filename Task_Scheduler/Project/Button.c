#include "Button.h"
#include <stdio.h>
#include "Leds.h"

QueueHandle_t input_q;
bt_state button_state;
uint32_t now_t=0, last_event_t=0, distance=0;
int btn_pressed = 0;
button userButton;


void BT_Init()
{
		
		GPIO_InitTypeDef GPIO_InitStruct;
	  __HAL_RCC_GPIOA_CLK_ENABLE();  // Enable clock to GPIO-A for button
    // Set GPIOA Pin Parameters, pin 0 for button
    GPIO_InitStruct.Pin     = GPIO_PIN_0;
	  // Specifies the trigger signal active edge for the EXTI lines. (e.g. Rising = button is pressed)
    GPIO_InitStruct.Mode    = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull    = GPIO_NOPULL;
    GPIO_InitStruct.Speed   = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);  // Init GPIOA
		HAL_NVIC_SetPriority(EXTI0_IRQn, 10, 0);
		HAL_NVIC_EnableIRQ(EXTI0_IRQn); // Enable GPIO_PIN_0 interrupt at IRQ-Level
	
		input_q = xQueueCreate( 10, sizeof( bt_state ) );
}	


void EXTI0_IRQHandler(void) 
{
	__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_0);
	

	// Resume the button task from the ISR
	BaseType_t xYieldRequiredCheck = xTaskResumeFromISR(xButtonHandler);
	portYIELD_FROM_ISR(xYieldRequiredCheck); // Yield to avoid delayed until the next time the scheduler

}

void buttonPressTimerCallback(TimerHandle_t buttonPressTimer)
{
		
		LD_Reset();
		// Single Press Button
		if (userButton.numPresses == 1)
		{		
			button_state = SHORT_PRESS;
			xQueueSendFromISR(input_q, &button_state, 0);
		}
		// Double Press
		else if(userButton.numPresses == 2)
		{
				button_state = DOUBLE_PRESS;
				xQueueSendFromISR(input_q, &button_state, 0);
		}
		//else might be sth else
		userButton.numPresses = 0;
}

void vTaskButton( void *pvParameters ) // Button Task (interrupt)
{
	const char *pcTaskName = "vTaskButton is running\n";
	
	for(;;)
	{
		vTaskSuspend(NULL);
		if (userButton.numPresses == 0)
		{
			xTimerStart(buttonPressTimer, 0);
		}
		userButton.numPresses++;
		vTaskDelay(200);
	}
}

