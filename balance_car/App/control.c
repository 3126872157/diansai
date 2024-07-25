#include "encoder.h"
#include "pid.h"
#include "motor.h"
#include "control.h"
#include "MPU6050.h"
#include "main.h"

int result;		//陀螺仪初始化结果
float pitch;	//三个角度
float yaw;
float roll;
short gyro;		//角加速度，用于直立环        疑问：short转float会咋样

short motor1_counter;
short motor2_counter;

//单位是rpm
float motor1_speed;		//motor1为右电机，向前speed为+
float motor2_speed;		//motor2为左电机，向前speed为-

float target_speed = 0;

float motor1_out;
float motor2_out;

//几种输出
float vertical_out;
float velocity_out;

void emergency_shut_motor()
{
	if(pitch >= 30 || pitch <= -30)
	{
		vertical_out = 0;
		motor2_out = 0;
		//HAL_GPIO_WritePin(STBY_GPIO_Port,STBY_Pin,GPIO_PIN_RESET);	//STBY置零，失能电机驱动板
	}
	else
	{
		HAL_GPIO_WritePin(STBY_GPIO_Port,STBY_Pin,GPIO_PIN_SET);
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)						//定时器回调函数，用于计算速度和PID计算
{	
    if(htim->Instance==GAP_TIM.Instance)										//间隔定时器中断，是时候计算速度了
    {
		//读取陀螺仪姿态和角加速度
		if(MPU6050_DMP_Get_Data(&pitch,&roll,&yaw,&gyro)){}						//pitch : x ; yaw : y ; roll : z
			
		//测速
		encoder_get_counter(&motor1_counter , &motor2_counter);
		motor1_speed = (float)motor1_counter / (ENCODER_MODE * MOTOR_SPEED_RERATIO * PULSE_PRE_ROUND) * GAP_FREQ;		//除以倍频数，减速比和线数，再乘上
		motor2_speed = (float)motor2_counter / (ENCODER_MODE * MOTOR_SPEED_RERATIO * PULSE_PRE_ROUND) * GAP_FREQ * -1;	//手动调整，向前转speed为正数
		
		//速度环
		velocity_out = Velocity(motor1_speed , motor2_speed , target_speed);
		
		//直立环
		vertical_out = balance(pitch, gyro, velocity_out);
		
		//防摔倒抽风
		emergency_shut_motor();
		
		//pwm输出
		setPWM(vertical_out, vertical_out);
	}
}
