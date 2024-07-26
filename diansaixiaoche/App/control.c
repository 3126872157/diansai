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
float target_speed1 = 0;
float target_speed2 = 0;

//Ѳ�߻�Ŀ��(!)
float follow_line_target =0;

//��λ��rpm
float motor1_speed;		//motor1Ϊ�ҵ������ǰspeedΪ+
float motor2_speed;		//motor2Ϊ��������ǰspeedΪ-

//pid���
float distance_out1;
float distance_out2;
float motor1_out;
float motor2_out;
//Ѳ�߻�pid���������
float follow_line_turn_out;

//����pwm
float vertical_pwm;
float velocity_pwm;

void control_init(void)
{
	Motor_Init();
	my_pid_init(&speed_pid, &distance_pid, &follow_line_pid, 8000, 10000, 5000, 10000,5000, 10000);		//��������������pwm�޷��ͻ����޷�
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
		distance_out1 = PID_calc(&distance_pid, motor1.distance, target_distance);
		distance_out2 = PID_calc(&distance_pid, motor2.distance, target_distance);
		motor1_out = PID_calc(&speed_pid, motor1.speed, distance_out1);
		motor2_out = PID_calc(&speed_pid, motor2.speed, distance_out2);
		
		//Ѳ�߻�pid����
		follow_line_turn_out = PID_calc(&follow_line_pid, openmv_receive[DELTA_X], follow_line_target);
		//pwm���
		setPWM(motor1_out-follow_line_turn_out, motor2_out+follow_line_turn_out);
	}
}
