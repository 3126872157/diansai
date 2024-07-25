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

float balance(float Angle, float Gyro, float velocity_pid)     //ֱ����
{  
	float Bias;
	int balance;
	Bias = Angle - velocity_pid;       		
	balance=balance_kp * Bias + Gyro * balance_kd;   		//����ƽ����Ƶĵ��PWM  PD����   kp��Pϵ�� kd��Dϵ�� 
	return balance;
}

extern float pitch;
float Velocity(float Encoder_A,float Encoder_B,float Mechanical_velocity)
{
	float velocity,Encoder_Least;										//�ٶȻ�����PWM,��ȡ�����ٶ�ƫ��
	static float Encoder,Encoder_Integral; 								//һ�׵�ͨ�˲�����ٶ�ֵ���ٶȵĻ��֣���Ϊ�����ۼӺ��˲�����Ҫ��������Ϊ��̬�����洢��ȫ������������ȫ�ֱ���

	Encoder_Least =( Encoder_A + Encoder_B ) - Mechanical_velocity; 	//��ȡ�����ٶ�ƫ��==�����ٶȣ����ұ�����֮�ͣ�-Ŀ���ٶ�                                            
	Encoder = Encoder * 0.8 + Encoder_Least * 0.2; 						//һ�׵�ͨ�˲���,��С�ٶȻ�����ֱ�����ĸ���Ӱ��
	
	Encoder_Integral += Encoder;               							//���֣������ۼ�
	Encoder_Integral = Encoder_Integral > 10000 ? 10000 : ( Encoder_Integral < -10000 ? -10000 : Encoder_Integral ); //�����޷�
//	velocity = Velocity_kp * Encoder + Velocity_ki * Encoder_Integral + Velocity_kd * (Encoder - last_encoder);  //�ٶȿ��� PI����   Velocity_KP��Pϵ��,Velocity_KI��Iϵ��
	velocity = Velocity_kp * Encoder + Velocity_ki * Encoder_Integral;
	
	//if(target_speed != last_speed) Encoder_Integral=0;					//�ٶ�ʱ�ı��������
	if(pitch <= -30.0 || pitch >= 30.0) Encoder_Integral=0;   			//����رպ��������
	
	//��������
//	last_speed = target_speed;
//	last_encoder = Encoder;
	
	return velocity;
}
