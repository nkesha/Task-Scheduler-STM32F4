#include "Sound.h"

#include "Sound.h"

I2C_HandleTypeDef hi2c1;
I2S_HandleTypeDef hi2s3;
DMA_HandleTypeDef hdma_spi3_tx;

uint16_t sample_N;
int16_t data_A5[100];

float mySinVal;
float sample_dt;

/* I2C1 init function */
void InitI2C1(void)
{
		hi2c1.Instance = I2C1;
		hi2c1.Init.ClockSpeed = 100000; // clock frequency <= 400kHz
		hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2; //fast mode Tlow/Thigh = 2
		hi2c1.Init.OwnAddress1 = 0; 
		hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
		hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
		hi2c1.Init.OwnAddress2 = 0; 
		hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
		hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
		HAL_I2C_Init(&hi2c1); //initialize the I2C peripheral 
}

/** This function handles DMA1 stream5 global interrupt. **/
void DMA1_Stream5_IRQHandler(void)
{
  HAL_DMA_IRQHandler(&hdma_spi3_tx);
}

void InitI2S3(void)
{
		hi2s3.Instance = SPI3;
		hi2s3.Init.Mode = I2S_MODE_MASTER_TX; //transmit in master mode
		hi2s3.Init.Standard = I2S_STANDARD_PHILIPS; //data protocol
		hi2s3.Init.DataFormat = I2S_DATAFORMAT_16B; //bits per sample
		hi2s3.Init.MCLKOutput = I2S_MCLKOUTPUT_ENABLE; //master clock signal
		hi2s3.Init.AudioFreq = I2S_AUDIOFREQ_48K; //audio frequency
		hi2s3.Init.CPOL = I2S_CPOL_LOW; //clock polarity
		hi2s3.Init.ClockSource = I2S_CLOCK_PLL;
		hi2s3.Init.FullDuplexMode = I2S_FULLDUPLEXMODE_DISABLE;
		HAL_I2S_Init(&hi2s3); //set the setting 
}

void InitDMA(void) 
{
  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);
}

void generateData(float note, int16_t arr[]){
	for(uint16_t i=0; i<sample_N; i++)
		{
			mySinVal = AMP*sinf(i*2*PI*sample_dt); 
			arr[i*2] = mySinVal;    //Right data (even: 0,2,4,6...)
			arr[i*2 + 1] = mySinVal; //Left data  (odd: 1,3,5,7...)
		}
}

void SN_Init() {
	InitDMA();
	InitI2C1();
	InitI2S3();
	
	sample_N =  F_SAMPLE/A5;
	generateData(A5, data_A5);
}

int16_t dataI2S[100];

void SN_Beep(){
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14,GPIO_PIN_SET);

	CS43_Init(hi2c1, MODE_I2S);
	CS43_SetVolume(30); //0 - 100
	CS43_Enable_RightLeft(CS43_RIGHT_LEFT);
	CS43_Start();
	//Create Sine wave
	sample_dt = A5/F_SAMPLE;
	sample_N =  F_SAMPLE/A5;
	for(uint16_t i=0; i<sample_N; i++)
	{
		mySinVal = AMP*sinf(i*2*PI*sample_dt); 
		dataI2S[i*2] = mySinVal;    //Right data (even: 0,2,4,6...)
		dataI2S[i*2 + 1] = mySinVal; //Left data  (odd: 1,3,5,7...)
	}

	// Output the sample through I2S from DMA
	HAL_I2S_Transmit_DMA(&hi2s3, (uint16_t *)dataI2S, sample_N*2);
	HAL_Delay(500);
	// Reset the DAC output pin 
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4,GPIO_PIN_RESET);
}
