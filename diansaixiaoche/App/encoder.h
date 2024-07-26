#ifndef _ENCODER_H_
#define _ENCODER_H_

#include "stm32f1xx.h"
#include "tim.h"

//电机1的编码器输入引脚
#define MOTO1_ENCODER1_PORT GPIOA		//TIM2,PA0,PA1
#define MOTO1_ENCODER1_PIN  GPIO_PIN_0
#define MOTO1_ENCODER2_PORT GPIOA
#define MOTO1_ENCODER2_PIN  GPIO_PIN_1

//电机2的编码器输入引脚
#define MOTO2_ENCODER1_POR2T GPIOD		//TIM4,PD12,PD13
#define MOTO2_ENCODER1_PIN   GPIO_PIN_12
#define MOTO2_ENCODER2_PORT  GPIOD
#define MOTO2_ENCODER2_PIN   GPIO_PIN_13

//定时器号
#define ENCODER_TIM1	htim2
#define PWM_TIM     	htim3	
#define GAP_TIM 		htim3			//同时用于定时器功能来测速与计算PWM—100Hz
#define ENCODER_TIM2   	htim4

#define COUNTER1 __HAL_TIM_GetCounter(&ENCODER_TIM1)     	//获取编码器定时器中的计数值
#define COUNTER2 __HAL_TIM_GetCounter(&ENCODER_TIM2)
#define RELOADVALUE1 __HAL_TIM_GetAutoreload(&ENCODER_TIM1) //获取自动装载值,本例中为65535
#define RELOADVALUE2 __HAL_TIM_GetAutoreload(&ENCODER_TIM2)

#define CNT_RESET 5000

void encoder_get_counter(short *encoder1_counter , short *encoder2_counter);
void encoder_get_speed(void);

#endif
