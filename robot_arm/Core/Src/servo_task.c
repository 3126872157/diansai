#include "y_servo.h"
#include "tim.h"
#include "cmsis_os.h"
#include "servo_task.h"

extern servo_t duoji_doing[DJ_NUM];

void servo_Task(void const * argument)
{
	while(1)
	{
//		for(int duoji_index = 0; duoji_index < 4; ++duoji_index)
//		{
//			
//		}
		int duoji_index = 0;
		servo_inc_offset(duoji_index);
		__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,duoji_doing[duoji_index].cur);
		duoji_index++;
		servo_inc_offset(duoji_index);
		__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_2,duoji_doing[duoji_index].cur);
		duoji_index++;		
		servo_inc_offset(duoji_index);
		__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_3,duoji_doing[duoji_index].cur);
		duoji_index++;
		servo_inc_offset(duoji_index);
		__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_4,duoji_doing[duoji_index].cur);
		duoji_index=0;
		osDelay(20);
	}
}
