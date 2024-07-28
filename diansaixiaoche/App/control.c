#include "encoder.h"
#include "pid.h"
#include "motor.h"
#include "control.h"
//#include "MPU6050.h"
#include "main.h"
#include "pid.h"
#include "sr04.h"
#include "task.h"
#include "openmv.h"

//pid结构体
pid_type_def speed_pid;
pid_type_def distance_pid;
pid_type_def follow_line_pid;

extern Motor motor1;
extern Motor motor2;
extern Car car;

int result;		//陀螺仪初始化结果
float pitch;	//三个角度
float yaw;
float roll;
short gyro;		//角加速度，用于直立环，这个是从mpu6050.c里读出        疑问：short转float会咋样

//调试用
extern float target_distance;
extern float target_speed1;
extern float target_speed2;

//缓起相关
float max_speed_error = 1.5;	//目标速度和实际速度最大差值
float slow_start_k = 0.25;		//缓起系数

//巡线环目标
float follow_line_target = 0;

//单位是rpm
float motor1_speed;		//motor1为右电机，向前speed为+
float motor2_speed;		//motor2为左电机，向前speed为-

//pid输出
float distance_out1;
float distance_out2;
float motor1_out;
float motor2_out;
float follow_line_turn_out;		//巡线环pid输出

//几种pwm
float vertical_pwm;
float velocity_pwm;

void control_init(void)
{
	Motor_Init();
	my_pid_init(&speed_pid, &distance_pid, &follow_line_pid, 8000, 10000, 5000, 10000,5000, 10000);		//后面两个参数是pwm限幅和积分限幅
	sr04_init();
	openmv_init();
}

void emergency_shut_motor()
{
//	if()
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
	//小车底盘相关控制
    if(htim->Instance == GAP_TIM.Instance)										//间隔定时器中断，是时候计算速度了
    {
		//读取陀螺仪姿态和角加速度
//		if(MPU6050_DMP_Get_Data(&pitch,&roll,&yaw,&gyro)){}						//pitch : x ; yaw : y ; roll : z
		
		//测速
		encoder_get_speed();
		
		//防抽风
		emergency_shut_motor();
		
		//设置车车的模式mode，模式转换为底盘行为
		car_chassis_set(&car);
		
		//设置车的控制变量
		task_set_control(&car);
		
		//pwm输出
		setPWM(motor1_out, motor2_out);

	}
	
	//传感器相关控制
	if(htim->Instance == SENSOR_TIM.Instance)
	{
		//超声波使能
		TRIG_ON;
		delay_us(15); 	//持续至少10us
		TRIG_OFF;
		
		__HAL_TIM_SET_CAPTUREPOLARITY(&SR04_TIM,TIM_CHANNEL_1,TIM_ICPOLARITY_RISING);
		HAL_TIM_IC_Start_IT(&SR04_TIM,TIM_CHANNEL_1);

	}
}
