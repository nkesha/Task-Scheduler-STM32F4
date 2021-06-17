#include "StateManager.h"

extern QueueHandle_t input_q;
SemaphoreHandle_t xSchedulerLock;

bt_state btn_state;
op_mode operation_mode;
sh_type user_selection;

void selectNextScheduler(){
	user_selection = (sh_type) (((int)user_selection + 1) % 4);
	LD_Reset();
	
	if(user_selection == FPS){
		LD_Set(ORANGE, 1);
		printf("First Priority Serve\n");
	}
	else if (user_selection == EDF){
		LD_Set(RED, 1);
		printf("Earliest Deadline Dirst\n");
	}
	else if (user_selection == LLF){
		LD_Set(BLUE, 1);
		printf("Least Laxity First\n");
	}
	else {
		LD_Set(GREEN, 1);
		printf("Short Remaining Time First\n");
	}
}

void applyCurrentSelection() {
	operation_mode = PERFORM;
	SH_init_tasks();
	xSemaphoreGive(xSchedulerLock);
}

void stopScheduler(){
	xSemaphoreTake(xSchedulerLock, portMAX_DELAY);
	SH_print_results();
	operation_mode = SELECT;
}

void vTaskOperator( void *pvParameters ) {
	while(1){
		if(xQueueReceive (input_q, &btn_state, portMAX_DELAY) == pdPASS){
			// we received a new input
			
			switch(btn_state){
				
				case SHORT_PRESS:
					if(operation_mode == SELECT)
						selectNextScheduler();
					break;
				
				case DOUBLE_PRESS:
					if(operation_mode == SELECT)
						applyCurrentSelection();
					else
						stopScheduler();
					break;
				
				case LONG_PRESS:
					break;
			}
			
		}
	}
}

void MN_Init(){
	operation_mode = SELECT;
	user_selection = FPS;
	//LD_Reset();
	//LD_Set(RED, 1);
	
	// Initialize semaphore for scheduler
	xSchedulerLock = xSemaphoreCreateBinary();
	xTaskCreate(vTaskOperator, "Manager", STACK_SIZE_MIN, NULL, 2, NULL);
}

