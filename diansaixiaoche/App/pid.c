#include "pid.h"
#include "main.h"

#define abs(x)	( (x>0) ? (x) : (-x) )
#define LimitMax(input, max)   \
    {                          \
        if (input > max)       \
        {                      \
            input = max;       \
        }                      \
        else if (input < -max) \
        {                      \
            input = -max;      \
        }                      \
    }

float balance_kp = 350;
float balance_kd = 1;
float Velocity_kp = 1000;
float Velocity_ki = 0.2;

//�����ã�pid��������
float k1[3] = {1500, 250, 5};	//�ٶȻ�
float k2[3] = {0.06, 0, 0};		//λ�û�
//!
float k3[3] = {0, 0, 0};		//Ѳ�߻�



float zhongzhi = -0.5;

float balance(float Angle,float Gyro)     //ֱ����
{  
	float Bias;
	int balance;
	Bias=Angle-zhongzhi;       		
	balance=balance_kp*Bias+Gyro*balance_kd;   		//����ƽ����Ƶĵ��PWM  PD����   kp��Pϵ�� kd��Dϵ�� 
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
	velocity = Velocity_kp * Encoder + Velocity_ki * Encoder_Integral;  //�ٶȿ��� PI����   Velocity_KP��Pϵ��,Velocity_KI��Iϵ��
	if(pitch <= -30.0 || pitch >= 30.0) Encoder_Integral=0;   			//����رպ��������
	return velocity;
}

/**
  * @brief          pid struct data init
  * @param[out]     pid: PID�ṹ����ָ��
  * @param[in]      mode: PID_POSITION:��ͨPID
  *                 PID_DELTA: ���PID
  * @param[in]      PID: 0: kp, 1: ki, 2:kd
  * @param[in]      max_out: pid������
  * @param[in]      max_iout: pid���������
  * @retval         none
  */
void PID_init(pid_type_def *pid, uint8_t mode, const fp32 PID[3], fp32 max_out, fp32 max_iout)
{
    if (pid == NULL || PID == NULL)
    {
        return;
    }
    pid->mode = mode;
    pid->Kp = PID[0];
    pid->Ki = PID[1];
    pid->Kd = PID[2];
    pid->max_out = max_out;
    pid->max_iout = max_iout;
    pid->Dbuf[0] = pid->Dbuf[1] = pid->Dbuf[2] = 0.0f;
    pid->error[0] = pid->error[1] = pid->error[2] = pid->Pout = pid->Iout = pid->Dout = pid->out = 0.0f;
}

void my_pid_init(pid_type_def *pid1, pid_type_def *pid2,pid_type_def *pid3, fp32 max_out1, fp32 max_iout1, fp32 max_out2, fp32 max_iout2, fp32 max_out3, fp32 max_iout3)
{
	PID_init(pid1, PID_POSITION, k1, max_out1, max_iout1); 
	PID_init(pid2, PID_POSITION, k2, max_out2, max_iout2);
    //!
    PID_init(pid3, PID_POSITION, k3, max_out3, max_iout3);
}

/**
  * @brief          pid����
  * @param[out]     pid: PID�ṹ����ָ��
  * @param[in]      ref: ��������
  * @param[in]      set: �趨ֵ
  * @retval         pid���
  */
fp32 PID_calc(pid_type_def *pid, fp32 ref, fp32 set)
{
    if (pid == NULL)
    {
        return 0.0f;
    }

    pid->error[2] = pid->error[1];
    pid->error[1] = pid->error[0];
    pid->set = set;
    pid->fdb = ref;
    pid->error[0] = set - ref;
    if (pid->mode == PID_POSITION)
    {
        pid->Pout = pid->Kp * pid->error[0];
        pid->Iout += pid->Ki * pid->error[0];
        pid->Dbuf[2] = pid->Dbuf[1];
        pid->Dbuf[1] = pid->Dbuf[0];
        pid->Dbuf[0] = (pid->error[0] - pid->error[1]);
        pid->Dout = pid->Kd * pid->Dbuf[0];
        LimitMax(pid->Iout, pid->max_iout);
        pid->out = pid->Pout + pid->Iout + pid->Dout;
        LimitMax(pid->out, pid->max_out);
    }
    else if (pid->mode == PID_DELTA)
    {
        pid->Pout = pid->Kp * (pid->error[0] - pid->error[1]);
        pid->Iout = pid->Ki * pid->error[0];
        pid->Dbuf[2] = pid->Dbuf[1];
        pid->Dbuf[1] = pid->Dbuf[0];
        pid->Dbuf[0] = (pid->error[0] - 2.0f * pid->error[1] + pid->error[2]);
        pid->Dout = pid->Kd * pid->Dbuf[0];
        pid->out += pid->Pout + pid->Iout + pid->Dout;
        LimitMax(pid->out, pid->max_out);
    }
    return pid->out;
}

/**
  * @brief          pid ������
  * @param[out]     pid: PID�ṹ����ָ��
  * @retval         none
  */
void PID_clear(pid_type_def *pid)
{
    if (pid == NULL)
    {
        return;
    }

    pid->error[0] = pid->error[1] = pid->error[2] = 0.0f;
    pid->Dbuf[0] = pid->Dbuf[1] = pid->Dbuf[2] = 0.0f;
    pid->out = pid->Pout = pid->Iout = pid->Dout = 0.0f;
    pid->fdb = pid->set = 0.0f;
}
