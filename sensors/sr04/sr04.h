#ifndef _SR04_H
#define _SR04_H

#include "stm32f1xx_hal.h"
#include "main.h"

#define TRIG_H  HAL_GPIO_WritePin(Trig_GPIO_Port,Trig_Pin,GPIO_PIN_SET)
#define TRIG_L  HAL_GPIO_WritePin(Trig_GPIO_Port,Trig_Pin,GPIO_PIN_RESET)

#define ECHO_TIM htim2

/*************
һ���ⲿ�ж������ܳ��������ĸߵ�ƽ��������PA2���û���ǩEcho
һ����������������ó�����������PA3���û���ǩTrig
һ����ʱ����Ϊ��������������TIM2

��һ�ַ��������벶��
*************/
extern TIM_HandleTypeDef htim2;

void Get_Distance(void);

#endif
