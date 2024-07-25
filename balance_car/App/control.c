#include "encoder.h"
#include "pid.h"
#include "motor.h"
#include "control.h"
#include "MPU6050.h"
#include "main.h"

int result;		//�����ǳ�ʼ�����
float pitch;	//�����Ƕ�
float yaw;
float roll;
short gyro;		//�Ǽ��ٶȣ�����ֱ����        ���ʣ�shortתfloat��զ��

short motor1_counter;
short motor2_counter;

//��λ��rpm
float motor1_speed;		//motor1Ϊ�ҵ������ǰspeedΪ+
float motor2_speed;		//motor2Ϊ��������ǰspeedΪ-

float target_speed = 0;

float motor1_out;
float motor2_out;

//�������
float vertical_out;
float velocity_out;

void emergency_shut_motor()
{
	if(pitch >= 30 || pitch <= -30)
	{
		vertical_out = 0;
		motor2_out = 0;
		//HAL_GPIO_WritePin(STBY_GPIO_Port,STBY_Pin,GPIO_PIN_RESET);	//STBY���㣬ʧ�ܵ��������
	}
	else
	{
		HAL_GPIO_WritePin(STBY_GPIO_Port,STBY_Pin,GPIO_PIN_SET);
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)						//��ʱ���ص����������ڼ����ٶȺ�PID����
{	
    if(htim->Instance==GAP_TIM.Instance)										//�����ʱ���жϣ���ʱ������ٶ���
    {
		//��ȡ��������̬�ͽǼ��ٶ�
		if(MPU6050_DMP_Get_Data(&pitch,&roll,&yaw,&gyro)){}						//pitch : x ; yaw : y ; roll : z
			
		//����
		encoder_get_counter(&motor1_counter , &motor2_counter);
		motor1_speed = (float)motor1_counter / (ENCODER_MODE * MOTOR_SPEED_RERATIO * PULSE_PRE_ROUND) * GAP_FREQ;		//���Ա�Ƶ�������ٱȺ��������ٳ���
		motor2_speed = (float)motor2_counter / (ENCODER_MODE * MOTOR_SPEED_RERATIO * PULSE_PRE_ROUND) * GAP_FREQ * -1;	//�ֶ���������ǰתspeedΪ����
		
		//�ٶȻ�
		velocity_out = Velocity(motor1_speed , motor2_speed , target_speed);
		
		//ֱ����
		vertical_out = balance(pitch, gyro, velocity_out);
		
		//��ˤ�����
		emergency_shut_motor();
		
		//pwm���
		setPWM(vertical_out, vertical_out);
	}
}
