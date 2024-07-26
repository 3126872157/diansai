#include "encoder.h"
#include "pid.h"
#include "motor.h"
#include "control.h"
//#include "MPU6050.h"
#include "main.h"
#include "pid.h"

pid_type_def speed_pid;
pid_type_def distance_pid;
//!
pid_type_def follow_line_pid;

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
float target_speed1 = 0;
float target_speed2 = 0;

//巡线环目标(!)
float follow_line_target =0;

//单位是rpm
float motor1_speed;		//motor1为右电机，向前speed为+
float motor2_speed;		//motor2为左电机，向前speed为-

//pid输出
float distance_out1;
float distance_out2;
float motor1_out;
float motor2_out;
//巡线环pid输出（！）
float follow_line_turn_out;

//几种pwm
float vertical_pwm;
float velocity_pwm;

void control_init(void)
{
	Motor_Init();
	my_pid_init(&speed_pid, &distance_pid, &follow_line_pid, 8000, 10000, 5000, 10000,5000, 10000);		//后面两个参数是pwm限幅和积分限幅
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
		distance_out1 = PID_calc(&distance_pid, motor1.distance, target_distance);
		distance_out2 = PID_calc(&distance_pid, motor2.distance, target_distance);
		motor1_out = PID_calc(&speed_pid, motor1.speed, distance_out1);
		motor2_out = PID_calc(&speed_pid, motor2.speed, distance_out2);
		
		//巡线环pid环！
		follow_line_turn_out = PID_calc(&follow_line_pid, openmv_receive[DELTA_X], follow_line_target);
		//pwm输出
		setPWM(motor1_out-follow_line_turn_out, motor2_out+follow_line_turn_out);
	}
}
