#include "stm32f1xx.h"
#include "encoder.h"
#include "motor.h"

Motor motor1;
Motor motor2;

void Motor_Init(void)
{
    HAL_TIM_Encoder_Start(&ENCODER_TIM1, TIM_CHANNEL_ALL);      //开启编码器定时器
	HAL_TIM_Encoder_Start(&ENCODER_TIM2, TIM_CHANNEL_ALL);
    __HAL_TIM_ENABLE_IT(&ENCODER_TIM1,TIM_IT_UPDATE);           //开启编码器定时器更新中断,防溢出处理，不用这俩了
	__HAL_TIM_ENABLE_IT(&ENCODER_TIM2,TIM_IT_UPDATE);
    HAL_TIM_Base_Start_IT(&GAP_TIM);                       		//开启10ms定时器中断
    HAL_TIM_PWM_Start(&PWM_TIM, TIM_CHANNEL_2);            		//开启PWM
    HAL_TIM_PWM_Start(&PWM_TIM, TIM_CHANNEL_1);           		//开启PWM
    __HAL_TIM_SET_COUNTER(&ENCODER_TIM1, CNT_RESET);            //编码器定时器初始值设定为32767，此为编码器中值，采用反应位置的测速测距方式
	__HAL_TIM_SET_COUNTER(&ENCODER_TIM2, CNT_RESET);
	
	//设置电机pwm死区
	motor1.offset = 260;
	motor2.offset = 250;
	
	//motor结构体初始化
	motor1.lastCount = CNT_RESET;
    motor1.totalCount = CNT_RESET;
    motor1.overflowNum = 0;                                  
    motor1.speed = 0;
    motor1.direct = 0;
	motor1.distance = 0;
	motor2.lastCount = CNT_RESET;
    motor2.totalCount = CNT_RESET;
    motor2.overflowNum = 0;                                  
    motor2.speed = 0;
    motor2.direct = 0;
	motor2.distance = 0;
	
//	HAL_GPIO_WritePin(STBY_GPIO_Port,STBY_Pin,GPIO_PIN_SET);	//STBY置高电平，使能电机驱动板
   
}

//pwm的计数period是10000，所以pwm最大为10000
void pwm_limit(float *PWMA,float *PWMB)
{
	*PWMA = *PWMA > 9000 ? 9000 : (*PWMA < -9000 ? -9000 : *PWMA);
	*PWMB = *PWMB > 9000 ? 9000 : (*PWMB < -9000 ? -9000 : *PWMB);
}

void setPWM(float motor1_out , float motor2_out)
{
	pwm_limit(&motor1_out , &motor2_out);
	
	if(motor1_out >= 0)		//手动调一下接线(AIN1和AIN2对调)，使得motor1_out为正时，电机往前转  疑问：为啥在软件这里调没有用？
    {
		HAL_GPIO_WritePin(AIN1_GPIO_Port,AIN1_Pin,GPIO_PIN_SET);
		HAL_GPIO_WritePin(AIN2_GPIO_Port,AIN2_Pin,GPIO_PIN_RESET);
		__HAL_TIM_SetCompare(&MOTOR1_TIM, MOTOR1_CHANNEL, motor1_out + motor1.offset);	//pid死区
    }
    if(motor1_out < 0)
    {
		HAL_GPIO_WritePin(AIN1_GPIO_Port,AIN1_Pin,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(AIN2_GPIO_Port,AIN2_Pin,GPIO_PIN_SET);
		__HAL_TIM_SetCompare(&MOTOR1_TIM, MOTOR1_CHANNEL, - motor1_out + motor1.offset);
    }
	
	if(motor2_out >= 0)
    {
		HAL_GPIO_WritePin(BIN1_GPIO_Port,BIN1_Pin,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(BIN2_GPIO_Port,BIN2_Pin,GPIO_PIN_SET);
		__HAL_TIM_SetCompare(&MOTOR2_TIM, MOTOR2_CHANNEL, motor2_out + motor2.offset);
    }
    if(motor2_out < 0)
    {
		HAL_GPIO_WritePin(BIN1_GPIO_Port,BIN1_Pin,GPIO_PIN_SET);
		HAL_GPIO_WritePin(BIN2_GPIO_Port,BIN2_Pin,GPIO_PIN_RESET);
		__HAL_TIM_SetCompare(&MOTOR2_TIM, MOTOR2_CHANNEL, - motor2_out + motor2.offset);
    }
}
