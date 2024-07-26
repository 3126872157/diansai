#include "encoder.h"
#include "pid.h"
#include "motor.h"
#include "control.h"
//#include "MPU6050.h"
#include "main.h"
#include "pid.h"
#include "sr04.h"

pid_type_def speed_pid;
pid_type_def distance_pid;

extern Motor motor1;
extern Motor motor2;

int result;		//�����ǳ�ʼ�����
float pitch;	//�����Ƕ�
float yaw;
float roll;
short gyro;		//�Ǽ��ٶȣ�����ֱ����������Ǵ�mpu6050.c�����        ���ʣ�shortתfloat��զ��

//������
extern float target_distance;
extern float target_speed1;
extern float target_speed2;

//�������
float max_speed_error = 1;	//Ŀ���ٶȺ�ʵ���ٶ�����ֵ
float slow_start_k = 0.1;	//����ϵ��

//��λ��rpm
float motor1_speed;		//motor1Ϊ�ҵ������ǰspeedΪ+
float motor2_speed;		//motor2Ϊ��������ǰspeedΪ-

//pid���
float distance_out1;
float distance_out2;
float motor1_out;
float motor2_out;

//����pwm
float vertical_pwm;
float velocity_pwm;

void control_init(void)
{
	Motor_Init();
	sr04_init();
	my_pid_init(&speed_pid, &distance_pid, 4000, 10000, 5000, 10000);		//��������������pwm�޷��ͻ����޷�
}

void emergency_shut_motor()
{
//	if()
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
	//С��������ؿ���
    if(htim->Instance == GAP_TIM.Instance)										//�����ʱ���жϣ���ʱ������ٶ���
    {
		//��ȡ��������̬�ͽǼ��ٶ�
//		if(MPU6050_DMP_Get_Data(&pitch,&roll,&yaw,&gyro)){}						//pitch : x ; yaw : y ; roll : z
		
		//����
		encoder_get_speed();
		
		//�����
//		emergency_shut_motor();
		
		//�����ٶ�λ��pid��
		distance_out1 = PID_calc(&distance_pid, motor1.distance, target_distance);
		distance_out2 = PID_calc(&distance_pid, motor2.distance, target_distance);
		
		//����
		if(distance_out1 - motor1.speed > max_speed_error)
		{
			distance_out1 = motor1.speed + (distance_out1 - motor1.speed) * slow_start_k;
		}
		if(distance_out2 - motor2.speed > max_speed_error)
		{
			distance_out2 = motor2.speed + (distance_out2 - motor2.speed) * slow_start_k;
		}
		
		motor1_out = PID_calc(&speed_pid, motor1.speed, distance_out1);
		motor2_out = PID_calc(&speed_pid, motor2.speed, distance_out2);
		
		//pwm���
		setPWM(motor1_out, motor2_out);
	}
	
	//��������ؿ���
	if(htim->Instance == SENSOR_TIM.Instance)
	{
		//������ʹ��
		TRIG_ON;
		delay_us(15); 	//��������10us
		TRIG_OFF;
		
		__HAL_TIM_SET_CAPTUREPOLARITY(&SR04_TIM,TIM_CHANNEL_1,TIM_ICPOLARITY_RISING);
		HAL_TIM_IC_Start_IT(&SR04_TIM,TIM_CHANNEL_1);

	}
}
