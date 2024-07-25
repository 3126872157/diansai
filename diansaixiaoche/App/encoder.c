#include "encoder.h"
#include "motor.h"
#include "control.h"

extern Motor motor1;
extern Motor motor2;

void encoder_get_counter(short *encoder1_counter , short *encoder2_counter)
{
	*encoder1_counter = COUNTER1;
	*encoder2_counter = COUNTER2;
	
	__HAL_TIM_SET_COUNTER(&ENCODER_TIM1, CNT_RESET);
	__HAL_TIM_SET_COUNTER(&ENCODER_TIM2, CNT_RESET);
}

//������٣�˳��������������������
void encoder_get_speed(void)
{
	motor1.direct = __HAL_TIM_IS_TIM_COUNTING_DOWN(&ENCODER_TIM1);			//������ϼ�������ת��������ֵΪ0�����򷵻�ֵΪ1
    motor1.totalCount = COUNTER1 + motor1.overflowNum * RELOADVALUE1;		//һ�������ڵ��ܼ���ֵ����Ŀǰ����ֵ��������ļ���ֵ
	
	motor2.direct = __HAL_TIM_IS_TIM_COUNTING_DOWN(&ENCODER_TIM2);
    motor2.totalCount = COUNTER2 + motor2.overflowNum * RELOADVALUE2;
	
	//����������������С���Ļ���������Ӧ�ò��������
	if(motor1.lastCount - motor1.totalCount > 32767)						//��������н����ģ������ٶȵõ�������������һ��
    {
        motor1.overflowNum++;
        motor1.totalCount = COUNTER1 + motor1.overflowNum * RELOADVALUE1;	//һ�������ڵ��ܼ���ֵ����Ŀǰ����ֵ��������ļ���ֵ
    }
    else if(motor1.totalCount - motor1.lastCount > 32797)
    {
        motor1.overflowNum--;
        motor1.totalCount = COUNTER1 + motor1.overflowNum * RELOADVALUE1;
    }
	if(motor2.lastCount - motor2.totalCount > 32767)						//��������н����ģ������ٶȵõ�������������һ��
    {
        motor2.overflowNum++;
        motor2.totalCount = COUNTER2 + motor2.overflowNum * RELOADVALUE2;	//һ�������ڵ��ܼ���ֵ����Ŀǰ����ֵ��������ļ���ֵ
    }
    else if(motor2.totalCount - motor2.lastCount > 32797)
    {
        motor2.overflowNum--;
        motor2.totalCount = COUNTER2 + motor2.overflowNum * RELOADVALUE2;
    }
	
	motor1.speed = (float)(motor1.totalCount - motor1.lastCount) / (ENCODER_MODE * MOTOR_SPEED_RERATIO * PULSE_PRE_ROUND) * GAP_FREQ;			//���Ա�Ƶ�������ٱȺ��������ٳ��ϼ�����Ƶ��
	motor2.speed = (float)(motor2.totalCount - motor2.lastCount) / (ENCODER_MODE * MOTOR_SPEED_RERATIO * PULSE_PRE_ROUND) * GAP_FREQ * -1;    //�ֶ���������ǰתspeedΪ����
	
	//��¼��һ�εļ���ֵ
	motor1.lastCount = motor1.totalCount;
	motor2.lastCount = motor2.totalCount;
}

