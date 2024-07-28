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

//pid�ṹ��
pid_type_def speed_pid;
pid_type_def distance_pid;
pid_type_def follow_line_pid;

extern Motor motor1;
extern Motor motor2;
extern Car car;

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
float max_speed_error = 1.5;	//Ŀ���ٶȺ�ʵ���ٶ�����ֵ
float slow_start_k = 0.25;		//����ϵ��

//Ѳ�߻�Ŀ��
float follow_line_target = 0;

//��λ��rpm
float motor1_speed;		//motor1Ϊ�ҵ������ǰspeedΪ+
float motor2_speed;		//motor2Ϊ��������ǰspeedΪ-

//pid���
float distance_out1;
float distance_out2;
float motor1_out;
float motor2_out;
float follow_line_turn_out;		//Ѳ�߻�pid���

//����pwm
float vertical_pwm;
float velocity_pwm;

void control_init(void)
{
	Motor_Init();
	my_pid_init(&speed_pid, &distance_pid, &follow_line_pid, 8000, 10000, 5000, 10000,5000, 10000);		//��������������pwm�޷��ͻ����޷�
	sr04_init();
	openmv_init();
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
		emergency_shut_motor();
		
		//���ó�����ģʽmode��ģʽת��Ϊ������Ϊ
		car_chassis_set(&car);
		
		//���ó��Ŀ��Ʊ���
		task_set_control(&car);
		
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
