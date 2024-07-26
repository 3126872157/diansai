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

void anti_overflow(Motor *motor)
{
	if(motor->lastCount - motor->totalCount > 32767)						//这个数字有讲究的，得算速度得到，现在先随便给一个
    {
        motor->overflowNum++;
    }
    else if(motor1.totalCount - motor1.lastCount > 32797)
    {
        motor->overflowNum--;
    }
}

//电机测速，顺便测编码器计数，即距离
void encoder_get_speed(void)
{
	motor1.direct = __HAL_TIM_IS_TIM_COUNTING_DOWN(&ENCODER_TIM1);			//如果向上计数（正转），返回值为0，否则返回值为1
	motor2.direct = __HAL_TIM_IS_TIM_COUNTING_DOWN(&ENCODER_TIM2);
	
	//防溢出处理，但是这个小车的霍尔编码器应该不会溢出的
	anti_overflow(&motor1);
	anti_overflow(&motor2);
	
	motor1.totalCount = COUNTER1 + motor1.overflowNum * RELOADVALUE1;		//一个周期内的总计数值等于目前计数值加上溢出的计数值
	motor2.totalCount = COUNTER2 + motor2.overflowNum * RELOADVALUE2;
	
	//算速度
	motor1.speed = (float)(motor1.totalCount - motor1.lastCount) / (ENCODER_MODE * MOTOR_SPEED_RERATIO * PULSE_PRE_ROUND) * GAP_FREQ * -1;		//除以倍频数，减速比和线数，再乘上计数器频率
	motor2.speed = (float)(motor2.totalCount - motor2.lastCount) / (ENCODER_MODE * MOTOR_SPEED_RERATIO * PULSE_PRE_ROUND) * GAP_FREQ;    		//手动调整，向前转speed为正数
	
	//低通滤波，另一个平均滤波计算量大，懒得弄了
	motor1.speed = motor1.speed * 0.8f + motor1.last_speed * (1 - 0.8f);
	motor2.speed = motor2.speed * 0.8f + motor2.last_speed * (1 - 0.8f);
	
	motor1.distance = (float)(motor1.totalCount - CNT_RESET) / (ENCODER_MODE * MOTOR_SPEED_RERATIO * PULSE_PRE_ROUND) * LINE_SPEED_C * -1;
	motor2.distance = (float)(motor2.totalCount - CNT_RESET) / (ENCODER_MODE * MOTOR_SPEED_RERATIO * PULSE_PRE_ROUND) * LINE_SPEED_C;
	
	//记录这一次的计数值
	motor1.lastCount = motor1.totalCount;
	motor2.lastCount = motor2.totalCount;
}

