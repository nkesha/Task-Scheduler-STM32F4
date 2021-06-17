#ifndef MY_SOUND
#define MY_SOUND

#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"
#include "MY_CS43L22.h"
#include <math.h>

#define PI 3.14159f
//Sample rate and Output freq
#define F_SAMPLE		48000.0f // Sample freq
#define F_OUT				880.0f  // Output freq 
#define AMP				1000  // Output amplitute 
  
#define A5				880.0f  
#define B5				988.0f
#define C6				1047.0f
#define D6				1175.0f  
#define E6				1319.0f   
#define F6				1397.0f   
#define G6				1568.0f 
#define A6				1760.0f

void SN_Init();
void SN_Beep();


#endif
