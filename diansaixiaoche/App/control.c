#include "encoder.h"
#include "pid.h"
#include "motor.h"
#include "control.h"
//#include "MPU6050.h"
#include "main.h"
#include "pid.h"

pid_type_def pid1;
pid_type_def pid2;

extern Motor motor1;
extern Motor motor2;

int result;		//陀螺仪初始化结果
float pitch;	//三个角度
float yaw;
float roll;
short gyro;		//角加速度，用于直立环，这个是从mpu6050.c里读出        疑问：short转float会咋样

//老的测速方式
//short motor1_counter;
//short motor2_counter;

//调试用
float target_distance;

//单位是rpm
float motor1_speed;		//motor1为右电机，向前speed为+
float motor2_speed;		//motor2为左电机，向前speed为-

float target_speed = 0;

float motor1_out;
float motor2_out;

//几种pwm
float vertical_pwm;
float velocity_pwm;

void control_init(void)
{
	Motor_Init();
	my_pid_init(&pid1, &pid2, 8000, 10000);		//后面两个参数是pwm限幅和积分限幅
}

void emergency_shut_motor()
{
//	if(pitch >= 30 || pitch <= -30)
//	{
//		motor1_out = 0;
//		motor2_out = 0;
		//HAL_GPIO_WritePin(STBY_GPIO_Port,STBY_Pin,GPIO_PIN_RESET);	//STBY置零，失能电机驱动板
//	}
//	else
//	{
//		HAL_GPIO_WritePin(STBY_GPIO_Port,STBY_Pin,GPIO_PIN_SET);
//	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)						//定时器回调函数，用于计算速度和PID计算
{	
    if(htim->Instance==GAP_TIM.Instance)										//间隔定时器中断，是时候计算速度了
    {
		//读取陀螺仪姿态和角加速度
//		if(MPU6050_DMP_Get_Data(&pitch,&roll,&yaw,&gyro)){}						//pitch : x ; yaw : y ; roll : z
			
		//测速
		encoder_get_speed();
		
		//防抽风
		emergency_shut_motor();
		
		//串级速度位置pid环
		motor1_out = PID_calc(&pid1, motor1.speed, PID_calc(&pid1, motor1.distance, target_distance));
		motor2_out = PID_calc(&pid2, motor2.speed, PID_calc(&pid2, motor2.distance, target_distance));
		
		//pwm输出
		setPWM(motor1_out, motor2_out);
	}
}
