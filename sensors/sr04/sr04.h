#ifndef _SR04_H
#define _SR04_H

#include "stm32f1xx_hal.h"
#include "main.h"

#define TRIG_H  HAL_GPIO_WritePin(Trig_GPIO_Port,Trig_Pin,GPIO_PIN_SET)
#define TRIG_L  HAL_GPIO_WritePin(Trig_GPIO_Port,Trig_Pin,GPIO_PIN_RESET)

#define ECHO_TIM htim2

/*************
一个外部中断来接受超声波发的高电平，这里是PA2，用户标签Echo
一个推挽输出的引脚让超声波工作，PA3，用户标签Trig
一个定时器作为计数器，这里是TIM2

另一种方法，输入捕获
*************/
extern TIM_HandleTypeDef htim2;

void Get_Distance(void);

#endif
