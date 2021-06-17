#include "Scheduler.h"
#include "Button.h"

extern sh_type user_selection;				// aka current sheduler type, provided by user in StateManager.c
extern SemaphoreHandle_t xSchedulerLock;
TaskHandle_t xSchedulerHandler = NULL;
int SRTFrunning = 0;
int timer;												// imitates the length of the brewing process. 1 second is a period

SH_Task_handle espresso;
SH_Task_handle latte;
SH_Task_handle cappuccino;
SH_Task_handle mocha;
SH_Task_handle *task_arr[4];

// Generic function that is run by each of the coffee threads
// It represents a process itself and manages it's state
void vBrewer(void *pvParameters){
	printf("TICK: %d ", timer);
	if(xSemaphoreTake(xSchedulerLock, (TickType_t) 10) == pdTRUE){
		printf("Brewing ");
		
		SH_Task_handle* this = (SH_Task_handle*) pvParameters;
		
		// set servo
		SR_Set_position(this->servo_position);
	
		// provide the output
		char *name = (char*) malloc(20 * sizeof(char));
		switch(this->servo_position){
			case 0:
				name = "Espresso";
				break;
			
			case 1:
				name = "Latte";
				break;
			
			case 2:
				name = "Cappuccino";
				break;
			
			case 3:
				name = "Mocha";
		}
		
		printf("%s <- ", name);
		
		// decrease execution time and manage task
		this->time_left -= 1;
		printf("Time left: %d\n", this->time_left);
		
		
		// check if the deadline was passed
		if(timer > this->time_deadline) {
			printf("Deadline is missed for %s\n", name);
			this->num_missed++;
			// deadline is only passed once
			this->time_deadline = this->time_abort+1;
		}
		
		// check if coffee is no longer allowed to brew
		if(timer > this->time_abort) {
			printf("%s must abort now\n", name);
			this->state = suspended;
		}
		
		// check if coffee is finished
		if(this->time_left <= 0){
			printf("%s has finished\n", name);
			this->state = suspended;
			SN_Beep();
		}
		else this->state = ready;
		xSemaphoreGive(xSchedulerLock);
		vTaskDelete(NULL);
	}
}

void SH_print_results(){
	char * scheduler_name;
	
	switch(user_selection){
		case FPS:
			scheduler_name = "FPS";
			break;
		
		case EDF:
			scheduler_name = "EDF";
			break;
		
		case LLF:
			scheduler_name = "LLF";
			break;
		
		case CUSTOM:
			scheduler_name = "LET";
			break;
	}
	
	printf("\n\nDuring %d ticks of %s scheduler...\n", timer, scheduler_name);
	printf("\tEspresso has\t\t%d missed deadlines\n", espresso.num_missed);
	printf("\tLatte has\t\t%d missed deadlines\n", latte.num_missed);
	printf("\tCappuccino has\t\t%d missed deadlines\n", cappuccino.num_missed);
	printf("\tMocha has\t\t%d missed deadlines\n\n\n", mocha.num_missed);
}

// Returns a task that should be run next according to FPS Schedule
SH_Task_handle* FPS_Schedule()
{
	// find the closest deadline
	SH_Task_handle *next_to_run = NULL;
  int found  = 0;
  //Search highest priority 3 task
	for(int i=0; i<4; i++)
	{
		if(task_arr[i]->state == ready && task_arr[i]->priority == 3)
		{
		  found = 1;
			next_to_run = task_arr[i];
			next_to_run->state = running;		
		}
	}
	if (found == 0)
	{
        //Search highest priority 2 task
		for(int i=0; i<4; i++)
		{
				if(task_arr[i]->state == ready && task_arr[i]->priority == 2)
				{
						found = 1;
						next_to_run = task_arr[i];
						next_to_run->state = running;	
						i = 4;
				}
		}
	}
	if (found == 0)
	{
		//Search highest priority 1 task
		for(int i=0; i<4; i++)
		{
				if(task_arr[i]->state == ready && task_arr[i]->priority == 1)
				{
						found = 1;
						next_to_run = task_arr[i];
						next_to_run->state = running;		
				}
		}
	}

	return next_to_run;
}
 

// Returns a task that should be run next according to EDF Schedule
SH_Task_handle* EDF_Schedule(){
	// find the closest deadline
	SH_Task_handle *next_to_run = NULL;
	int smallest_deadline = 10000;

	for(int i=0; i<4; i++)
	{
		if(task_arr[i]->state == ready && task_arr[i]->deadline < smallest_deadline)
		{
			next_to_run = task_arr[i];
			smallest_deadline = task_arr[i]->deadline;
		}
	}
	
	return next_to_run;
}

// Returns a task that should be run next according to LLF Schedule
SH_Task_handle* LLF_Schedule() {
	SH_Task_handle* task_to_execute = NULL;
	int laxity = 10000;
	
	for(int i=0; i<4; i++){
		int time_from_deadline = task_arr[i]->time_deadline - timer;
		int execution_time_left = task_arr[i]->time_left;
		
		if(time_from_deadline > 0 && execution_time_left > 0){
			int curr_laxity = time_from_deadline - execution_time_left;
			
			if(curr_laxity < laxity){
				laxity = curr_laxity;
				task_to_execute = task_arr[i];
			}
		}
	}
	
	return task_to_execute;
}


//Short Remaining Time First Scheduling
SH_Task_handle* SRTF_Schedule() {

	// Find the shortest Remaining Time First
	SH_Task_handle *task_to_execute = NULL;
  
  
	//find the first ready element
	int firstElement = 0;
	for(int j= 0;j<4;j++)
	{
		if(task_arr[j]->state == ready && task_arr[j]->time_left > 0)
		{
			firstElement = j;
			j=4;
		}	
	}

	int index = firstElement;
	for(int i=index+1; i<4; i++)
	{
		if((task_arr[i]->state == ready) && (task_arr[i]->time_left < task_arr[index]->time_left) && task_arr[i]->time_left != 0)
		{
				index = i;
		}
	}
	if(task_arr[index]->time_left != 0)
		task_to_execute = task_arr[index];
	
	return task_to_execute;
}






// Checks periodicity of a coffee type provided and manages it's state accordingly
// (Initializez coffee tasks accouding to their periodicity)
void check_coffee_period(SH_Task_handle* coffee){
	if (timer % coffee->period == 0){
		// initialize new task
		coffee->state = ready;
		coffee->time_abort = timer + coffee->period;
		coffee->time_deadline = timer + coffee->deadline;
		coffee->time_left = coffee->duration;
	}
}

// Manages all the scheduling functions
void vScheduler(void *pvParameters){
	while(1){
		if(xSemaphoreTake(xSchedulerLock, (TickType_t) 10) == pdTRUE){
			
			//printf("Scheduling EDF...\n");
			
			//Coffee tasks are periodic so we need to initialize them at the right time
			check_coffee_period(&espresso);
			check_coffee_period(&latte);
			check_coffee_period(&cappuccino);
			check_coffee_period(&mocha);
			
			SH_Task_handle* task_to_execute = NULL;
			
			switch(user_selection){
				case FPS:
					task_to_execute = FPS_Schedule();
					break;
				
				case EDF:
					task_to_execute = EDF_Schedule();
					break;
				
				case LLF:
					task_to_execute = LLF_Schedule();
					break;
				
				case CUSTOM:
					task_to_execute = SRTF_Schedule();
					break;
			}
			
			//task_to_execute = FPS_Schedule();
			
					// run the task found
			if(task_to_execute != NULL){
				task_to_execute->state = running;
				xTaskCreate(vBrewer, "Brew coffee", STACK_SIZE_MIN, (void*) task_to_execute, 1, NULL);
				
			}
			timer++;
			xSemaphoreGive(xSchedulerLock);
			vTaskDelay(700);									// let a task run for one second
		}
	}
}

// Initializes tasks with the default scheduling table
void SH_init_tasks() {
	espresso.servo_position = 0;
	espresso.duration = 2;
	espresso.deadline = 5;
	espresso.period = 10;
	espresso.priority = 3;
	espresso.time_left = 2;
	espresso.time_deadline = 5;		// = deadline
	espresso.time_abort = 10; 		// = period
	espresso.state = ready;
	espresso.num_missed = 0;
	
	latte.servo_position = 1;
	latte.duration = 3;
	latte.deadline = 7;
	latte.period = 20;
	latte.priority = 1;
	latte.time_left = 3;
	latte.time_deadline = 7;
	latte.time_abort = 20;
	latte.state = ready;
	latte.num_missed = 0;
	
	cappuccino.servo_position = 2;
	cappuccino.duration = 3;
	cappuccino.deadline = 7;
	cappuccino.period = 20;
	cappuccino.priority = 2;
	cappuccino.time_left = 3;
	cappuccino.time_deadline = 7;
	cappuccino.time_abort = 20;
	cappuccino.state = ready;
	cappuccino.num_missed = 0;
	
	mocha.servo_position = 3;
	mocha.duration = 4;
	mocha.deadline = 10;
	mocha.period = 30;
	mocha.priority = 2;
	mocha.time_left = 4;
	mocha.time_deadline = 10;
	mocha.time_abort = 30;
	mocha.state = ready;
	mocha.num_missed = 0;
	
	task_arr[0] = &espresso;
	task_arr[1] = &latte;
	task_arr[2] = &cappuccino;
	task_arr[3] = &mocha;
	
	timer = 0;
}

// Initializes the scheduler. To be run from main.c
void SH_Start(){	
	xTaskCreate( vScheduler, "Scheduler", STACK_SIZE_MIN * 2, NULL, 5, &xSchedulerHandler);
	vTaskStartScheduler();
}


