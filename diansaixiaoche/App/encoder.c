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

//电机测速，顺便测编码器计数，即距离
void encoder_get_speed(void)
{
	motor1.direct = __HAL_TIM_IS_TIM_COUNTING_DOWN(&ENCODER_TIM1);			//如果向上计数（正转），返回值为0，否则返回值为1
    motor1.totalCount = COUNTER1 + motor1.overflowNum * RELOADVALUE1;		//一个周期内的总计数值等于目前计数值加上溢出的计数值
	
	motor2.direct = __HAL_TIM_IS_TIM_COUNTING_DOWN(&ENCODER_TIM2);
    motor2.totalCount = COUNTER2 + motor2.overflowNum * RELOADVALUE2;
	
	//防溢出处理，但是这个小车的霍尔编码器应该不会溢出的
	if(motor1.lastCount - motor1.totalCount > 32767)						//这个数字有讲究的，得算速度得到，现在先随便给一个
    {
        motor1.overflowNum++;
        motor1.totalCount = COUNTER1 + motor1.overflowNum * RELOADVALUE1;	//一个周期内的总计数值等于目前计数值加上溢出的计数值
    }
    else if(motor1.totalCount - motor1.lastCount > 32797)
    {
        motor1.overflowNum--;
        motor1.totalCount = COUNTER1 + motor1.overflowNum * RELOADVALUE1;
    }
	if(motor2.lastCount - motor2.totalCount > 32767)						//这个数字有讲究的，得算速度得到，现在先随便给一个
    {
        motor2.overflowNum++;
        motor2.totalCount = COUNTER2 + motor2.overflowNum * RELOADVALUE2;	//一个周期内的总计数值等于目前计数值加上溢出的计数值
    }
    else if(motor2.totalCount - motor2.lastCount > 32797)
    {
        motor2.overflowNum--;
        motor2.totalCount = COUNTER2 + motor2.overflowNum * RELOADVALUE2;
    }
	
	motor1.speed = (float)(motor1.totalCount - motor1.lastCount) / (ENCODER_MODE * MOTOR_SPEED_RERATIO * PULSE_PRE_ROUND) * GAP_FREQ;			//除以倍频数，减速比和线数，再乘上计数器频率
	motor2.speed = (float)(motor2.totalCount - motor2.lastCount) / (ENCODER_MODE * MOTOR_SPEED_RERATIO * PULSE_PRE_ROUND) * GAP_FREQ * -1;    //手动调整，向前转speed为正数
	
	//记录这一次的计数值
	motor1.lastCount = motor1.totalCount;
	motor2.lastCount = motor2.totalCount;
}

