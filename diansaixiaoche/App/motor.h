#ifndef _MOTOR_H_
#define _MOTOR_H_

#include "main.h"

//PWM输出
#define MOTOR1_TIM	htim3
#define MOTOR1_CHANNEL TIM_CHANNEL_1	//PA6
#define MOTOR2_TIM	htim3
#define MOTOR2_CHANNEL TIM_CHANNEL_2	//PA7

//
//AIN1 PB12
//AIN2 PB13

//BIN1 PB15
//BIN2 PB14

typedef struct _Motor
{
    int32_t lastCount;   	//上一次计数值
    int32_t totalCount;  	//总计数值
    int16_t overflowNum; 	//溢出次数
    float speed;         	//电机转速
    uint8_t direct;      	//旋转方向
	int offset;				//给pid死区加的偏置
	float distance;
}Motor;

void Motor_Init(void);
void setPWM(float motor1_out , float motor2_out);

#endif
