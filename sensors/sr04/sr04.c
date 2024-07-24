#include "sr04.h"

uint16_t highTime;
float distance;		//单位㎝

//===============================================us延时函数
void delay1_us(uint32_t us)//主频72M
{
    uint32_t delay = (HAL_RCC_GetHCLKFreq() / 4000000 * us);
    while (delay--)
	{
		;
	}
}

void Get_Distance(void)
{
	TRIG_H;
	delay1_us(30);	//超过10us的高电平触发超声波开始工作
	TRIG_L;
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(HAL_GPIO_ReadPin(Echo_GPIO_Port,Echo_Pin) == GPIO_PIN_SET)
	{
		__HAL_TIM_SetCounter(&ECHO_TIM,0);
		HAL_TIM_Base_Start(&ECHO_TIM);
	}
	else
	{
		HAL_TIM_Base_Stop(&ECHO_TIM);
		highTime = __HAL_TIM_GetCounter(&ECHO_TIM);
		distance = highTime * 0.017;	//定时器频率为1MHz，0.000,001 * 340 * 0.01 / 2 = 0.017
	}
}
