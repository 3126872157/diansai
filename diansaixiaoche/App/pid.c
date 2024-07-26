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

//调试用，pid三个参数
float k1[3] = {1500, 250, 5};	//速度环
float k2[3] = {0.06, 0, 0};		//位置环
//!
float k3[3] = {0, 0, 0};		//巡线环



float zhongzhi = -0.5;

float balance(float Angle,float Gyro)     //直立环
{  
	float Bias;
	int balance;
	Bias=Angle-zhongzhi;       		
	balance=balance_kp*Bias+Gyro*balance_kd;   		//计算平衡控制的电机PWM  PD控制   kp是P系数 kd是D系数 
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
	velocity = Velocity_kp * Encoder + Velocity_ki * Encoder_Integral;  //速度控制 PI控制   Velocity_KP是P系数,Velocity_KI是I系数
	if(pitch <= -30.0 || pitch >= 30.0) Encoder_Integral=0;   			//电机关闭后清除积分
	return velocity;
}

/**
  * @brief          pid struct data init
  * @param[out]     pid: PID结构数据指针
  * @param[in]      mode: PID_POSITION:普通PID
  *                 PID_DELTA: 差分PID
  * @param[in]      PID: 0: kp, 1: ki, 2:kd
  * @param[in]      max_out: pid最大输出
  * @param[in]      max_iout: pid最大积分输出
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
  * @brief          pid计算
  * @param[out]     pid: PID结构数据指针
  * @param[in]      ref: 反馈数据
  * @param[in]      set: 设定值
  * @retval         pid输出
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
  * @brief          pid 输出清除
  * @param[out]     pid: PID结构数据指针
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
