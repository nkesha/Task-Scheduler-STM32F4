#include "Servo.h"

void InitServo(void)
{
		GPIO_InitTypeDef GPIO_InitStruct_S;
    __HAL_RCC_GPIOA_CLK_ENABLE();  // Enable clock to GPIO-B for PB6 and PB7
    // Set GPIOB Pins Parameters, PB6 and PB7 
		GPIO_InitStruct_S.Pin = GPIO_PIN_6;  //PB6: TIM4_CH1 and PB7: TIM4_CH2
    GPIO_InitStruct_S.Mode = GPIO_MODE_AF_PP; //Alternate Function Push Pull Mode 
    GPIO_InitStruct_S.Pull = GPIO_NOPULL; // No Pull-up or Pull-down activation
    GPIO_InitStruct_S.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct_S.Alternate = GPIO_AF2_TIM3; // Assign those pins alternate function in TIM4 
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct_S); // Init GPIOB
		
}

TIM_HandleTypeDef TIM3_InitStruct;
TIM_OC_InitTypeDef TIM3_OCInitStructure;

void InitTimer3(void)
{
		//__TIM1_CLK_ENABLE();
		__HAL_RCC_TIM3_CLK_ENABLE(); // Enable clock to TIM3 from APB2 bus (48Mhz max)xPLL_P = 84MHz
		 
	// TIM4 is configure to 50hz: 50 times in 1s or 1000000us
    TIM3_InitStruct.Instance = TIM3;
		TIM3_InitStruct.Init.Period = 20000-1;
    TIM3_InitStruct.Init.Prescaler   = 84-1;
		TIM3_InitStruct.Init.CounterMode = TIM_COUNTERMODE_UP;
		TIM3_InitStruct.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
		TIM3_InitStruct.Init.RepetitionCounter = 0;
		HAL_TIM_Base_Init(&TIM3_InitStruct); // Init TIM3
		
		/*//if you would like to enable interrupt
		//HAL_TIM_Base_Start_IT(&TIM3_InitStruct); // Enable timer-3 IRQ interrupt
		//HAL_NVIC_SetPriority(TIM3_IRQn, 0, 0);
		//HAL_NVIC_EnableIRQ(TIM3_IRQn); // Enable interrupt at IRQ-Level
		*/
    HAL_TIM_Base_Start(&TIM3_InitStruct); // Start TIM3
}

/*To control LED (only at TIM4)*/
void SetupPWM_TIM3()
{
	HAL_TIM_PWM_Init(&TIM3_InitStruct);
	
	TIM3_OCInitStructure.OCMode = TIM_OCMODE_PWM1; //Set output capture as PWM mode
  TIM3_OCInitStructure.Pulse = 0; // Initial duty cycle at 0%
  TIM3_OCInitStructure.OCPolarity = TIM_OCPOLARITY_HIGH; // HIGH output compare active
  TIM3_OCInitStructure.OCFastMode = TIM_OCFAST_DISABLE; // output compare disable
	HAL_TIM_PWM_ConfigChannel(&TIM3_InitStruct, &TIM3_OCInitStructure, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&TIM3_InitStruct, TIM_CHANNEL_1); // Start PWM at channel 1
	HAL_TIM_PWM_ConfigChannel(&TIM3_InitStruct, &TIM3_OCInitStructure, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&TIM3_InitStruct, TIM_CHANNEL_2); // Start PWM at channel 2
}

void SR_Init(){
	InitServo();
	InitTimer3();
	SetupPWM_TIM3();
}

void SR_Set_position(int pos){
	switch(pos){
		case 0:
			TIM3->CCR1 = 550;
			break;
		
		case 1:
			TIM3->CCR1 = 1000;
			break;
		
		case 2:
			TIM3->CCR1 = 1500;
			break;
		
		case 3:
			TIM3->CCR1 = 2000;
			break;
	}
}
