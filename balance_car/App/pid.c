#include "pid.h"

float balance_kp = 350;
float balance_kd = 1;
float Velocity_kp = 1000;
float Velocity_ki = 0.2;
float Velocity_kd = 0;

float zhongzhi = -0.5;

extern float target_speed;
extern float last_speed;
extern float last_encoder;

float balance(float Angle, float Gyro, float velocity_pid)     //直立环
{  
	float Bias;
	int balance;
	Bias = Angle - velocity_pid;       		
	balance=balance_kp * Bias + Gyro * balance_kd;   		//计算平衡控制的电机PWM  PD控制   kp是P系数 kd是D系数 
	return balance;
}

extern float pitch;
float Velocity(float Encoder_A,float Encoder_B,float Mechanical_velocity)
{
	float velocity,Encoder_Least;										//速度环控制PWM,获取最新速度偏差
	static float Encoder,Encoder_Integral; 								//一阶低通滤波后的速度值，速度的积分；因为积分累加和滤波的需要，故设置为静态量，存储在全局区域类似于全局变量

	Encoder_Least =( Encoder_A + Encoder_B ) - Mechanical_velocity; 	//获取最新速度偏差==测量速度（左右编码器之和）-目标速度                                            
	Encoder = Encoder * 0.8 + Encoder_Least * 0.2; 						//一阶低通滤波器,减小速度环对于直立环的负面影响
	
	Encoder_Integral += Encoder;               							//积分，就是累加
	Encoder_Integral = Encoder_Integral > 10000 ? 10000 : ( Encoder_Integral < -10000 ? -10000 : Encoder_Integral ); //积分限幅
//	velocity = Velocity_kp * Encoder + Velocity_ki * Encoder_Integral + Velocity_kd * (Encoder - last_encoder);  //速度控制 PI控制   Velocity_KP是P系数,Velocity_KI是I系数
	velocity = Velocity_kp * Encoder + Velocity_ki * Encoder_Integral;
	
	//if(target_speed != last_speed) Encoder_Integral=0;					//速度时改变清除积分
	if(pitch <= -30.0 || pitch >= 30.0) Encoder_Integral=0;   			//电机关闭后清除积分
	
	//更新数据
//	last_speed = target_speed;
//	last_encoder = Encoder;
	
	return velocity;
}
