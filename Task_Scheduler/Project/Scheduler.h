#ifndef MY_SCHEDULER
#define MY_SCHEDULER

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "Servo.h"
#include "Sound.h"

#define STACK_SIZE_MIN	128

typedef enum {
	FPS, EDF, LLF, CUSTOM
} sh_type;

typedef enum {
	running, ready, suspended
} t_state;

typedef struct {
	int servo_position;
	int duration;
	int deadline;
	int priority;
	int period;
	int time_left;					// time left until coffee is done
	int time_deadline;			// time left until deadline is passed
	int time_abort;					// time left until task is no longer allowed to run
	int num_missed;					// number of missed deadlines
	t_state state;
} SH_Task_handle;

void SH_Start();
void SH_init_tasks();
void SH_print_results();

#endif
