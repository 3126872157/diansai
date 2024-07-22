#include "encoder.h"

void encoder_get_counter(short *encoder1_counter , short *encoder2_counter)
{
	*encoder1_counter = __HAL_TIM_GetCounter(&ENCODER_TIM1);
	*encoder2_counter = __HAL_TIM_GetCounter(&ENCODER_TIM2);
	
	__HAL_TIM_SET_COUNTER(&ENCODER_TIM1, 0);
	__HAL_TIM_SET_COUNTER(&ENCODER_TIM2, 0);
}

