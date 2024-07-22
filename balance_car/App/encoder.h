#ifndef _ENCODER_H_
#define _ENCODER_H_

#include "stm32f1xx.h"
#include "tim.h"

//���1�ı�������������
#define MOTO1_ENCODER1_PORT GPIOA		//TIM2,PA0,PA1
#define MOTO1_ENCODER1_PIN  GPIO_PIN_0
#define MOTO1_ENCODER2_PORT GPIOA
#define MOTO1_ENCODER2_PIN  GPIO_PIN_1

//���2�ı�������������
#define MOTO2_ENCODER1_POR2T GPIOB		//TIM4,PB6,PB7
#define MOTO2_ENCODER1_PIN   GPIO_PIN_6
#define MOTO2_ENCODER2_PORT  GPIOB
#define MOTO2_ENCODER2_PIN   GPIO_PIN_7

//��ʱ����
#define ENCODER_TIM1	htim2
#define PWM_TIM     	htim3	
#define GAP_TIM 		htim3			//ͬʱ���ڶ�ʱ�����������������PWM��100Hz
#define ENCODER_TIM2   	htim4

#define COUNTER1 __HAL_TIM_GetCounter(&ENCODER_TIM1)     //��ȡ��������ʱ���еļ���ֵ
#define COUNTER2 __HAL_TIM_GetCounter(&ENCODER_TIM2)

void encoder_get_counter(short *encoder1_counter , short *encoder2_counter);

#endif
