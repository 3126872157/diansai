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

int result;		//�����ǳ�ʼ�����
float pitch;	//�����Ƕ�
float yaw;
float roll;
short gyro;		//�Ǽ��ٶȣ�����ֱ����������Ǵ�mpu6050.c�����        ���ʣ�shortתfloat��զ��

//�ϵĲ��ٷ�ʽ
//short motor1_counter;
//short motor2_counter;

//������
float target_distance;

//��λ��rpm
float motor1_speed;		//motor1Ϊ�ҵ������ǰspeedΪ+
float motor2_speed;		//motor2Ϊ��������ǰspeedΪ-

float target_speed = 0;

float motor1_out;
float motor2_out;

//����pwm
float vertical_pwm;
float velocity_pwm;

void control_init(void)
{
	Motor_Init();
	my_pid_init(&pid1, &pid2, 8000, 10000);		//��������������pwm�޷��ͻ����޷�
}

void emergency_shut_motor()
{
//	if(pitch >= 30 || pitch <= -30)
//	{
//		motor1_out = 0;
//		motor2_out = 0;
		//HAL_GPIO_WritePin(STBY_GPIO_Port,STBY_Pin,GPIO_PIN_RESET);	//STBY���㣬ʧ�ܵ��������
//	}
//	else
//	{
//		HAL_GPIO_WritePin(STBY_GPIO_Port,STBY_Pin,GPIO_PIN_SET);
//	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)						//��ʱ���ص����������ڼ����ٶȺ�PID����
{	
    if(htim->Instance==GAP_TIM.Instance)										//�����ʱ���жϣ���ʱ������ٶ���
    {
		//��ȡ��������̬�ͽǼ��ٶ�
//		if(MPU6050_DMP_Get_Data(&pitch,&roll,&yaw,&gyro)){}						//pitch : x ; yaw : y ; roll : z
			
		//����
		encoder_get_speed();
		
		//�����
		emergency_shut_motor();
		
		//�����ٶ�λ��pid��
		motor1_out = PID_calc(&pid1, motor1.speed, PID_calc(&pid1, motor1.distance, target_distance));
		motor2_out = PID_calc(&pid2, motor2.speed, PID_calc(&pid2, motor2.distance, target_distance));
		
		//pwm���
		setPWM(motor1_out, motor2_out);
	}
}
