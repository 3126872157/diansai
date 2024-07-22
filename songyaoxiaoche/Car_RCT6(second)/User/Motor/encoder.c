/*视频教程链接 https://www.bilibili.com/video/BV1UL411V7XK?p=2&share_source=copy_web   昵称：海喂喂喂*/
#include "encoder.h"
#include "stm32f1xx.h"
#include "tim.h"//包含tim头文件
#include "usart.h"

long g_lMotorPulseSigma =0;
long g_lMotor2PulseSigma=0;
short g_nMotorPulse=0,g_nMotor2Pulse=0;//全局变量， 保存电机脉冲数值


//如果编码器有时候捕获的数值会跳变的话，可以在cubemx的编码器设置里面加上滤波，可以设置1~16的值


/*******************实际运行时读取编码器数值************************/   //当两个轮子转向不一致的时候，要同时调这里编码器捕获脉冲的正负和control.h里的PWM通道宏定义
void GetMotorPulse(void)//读取电机脉冲
{
	g_nMotorPulse = (short)(__HAL_TIM_GET_COUNTER(&htim3));//获取计数器值   为什么现在可以两个都是正？
	
	__HAL_TIM_SET_COUNTER(&htim3,0);//TIM3计数器清零
	
	g_nMotor2Pulse = (short)(__HAL_TIM_GET_COUNTER(&htim8));//获取计数器值
	g_nMotor2Pulse = -g_nMotor2Pulse;
	__HAL_TIM_SET_COUNTER(&htim8,0);//TIM8计数器清零
	
	
	g_lMotorPulseSigma += g_nMotorPulse;//位置外环使用的脉冲累积      //完成一次位置控制之后才清除。
	g_lMotor2PulseSigma += g_nMotor2Pulse;//位置外环使用的脉冲累积   //记录了之后也要及时清零呀！！！

}
