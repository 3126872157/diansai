#ifndef _MOTOR_H_
#define _MOTOR_H_

#include "main.h"

//PWM���
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
    int32_t lastCount;   	//��һ�μ���ֵ
    int32_t totalCount;  	//�ܼ���ֵ
    int16_t overflowNum; 	//�������
    float speed;         	//���ת��
    uint8_t direct;      	//��ת����
	int offset;				//��pid�����ӵ�ƫ��
	float distance;
}Motor;

void Motor_Init(void);
void setPWM(float motor1_out , float motor2_out);

#endif
