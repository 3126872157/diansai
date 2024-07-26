#ifndef _SR04_H_
#define _SR04_H_

#include "struct_typedef.h"
#include "tim.h"

#define TRIG_ON  	HAL_GPIO_WritePin(TRIG_GPIO_Port, TRIG_Pin, GPIO_PIN_SET)
#define TRIG_OFF  	HAL_GPIO_WritePin(TRIG_GPIO_Port, TRIG_Pin, GPIO_PIN_RESET)
#define SENSOR_TIM	htim6
#define SR04_TIM	htim1

void delay_us(uint32_t us);
void sr04_init(void);

#endif
