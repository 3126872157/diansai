#include "sr04.h"
#include "stm32f1xx_hal.h"
#include "control.h"
#include "main.h"

uint32_t capture_Buf[4] = {0};
uint8_t capture_Cnt = 0;
uint32_t high_time;
float sr04_distance=0;

//��Ƶ72M��us����ʱ����
void delay_us(uint32_t us)
{
    uint32_t delay = (HAL_RCC_GetHCLKFreq() / 4000000 * us);
    while (delay --);
}

void sr04_init(void)
{
	TRIG_OFF;
	HAL_TIM_Base_Start_IT(&SENSOR_TIM);
	HAL_Delay(300);
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if(htim == &SR04_TIM){
		switch(capture_Cnt){
			case 0:      //low level
				capture_Buf[0] = HAL_TIM_ReadCapturedValue(&SR04_TIM, TIM_CHANNEL_1);
				__HAL_TIM_SET_CAPTUREPOLARITY(&SR04_TIM,TIM_CHANNEL_1, TIM_ICPOLARITY_FALLING);
				capture_Cnt++;
				break;
 
			case 1:     //high level
				capture_Buf[1] = HAL_TIM_ReadCapturedValue(&SR04_TIM, TIM_CHANNEL_1);
				HAL_TIM_IC_Stop_IT(&SR04_TIM, TIM_CHANNEL_1);
				capture_Cnt = 0;
				__HAL_TIM_SET_COUNTER(&SR04_TIM, 0);	//�����������
				high_time = capture_Buf[1] - capture_Buf[0];
				sr04_distance = high_time * 0.017; //�ٶ�0.034cm/us,������ľ���Ҫ����2,distances�ĵ�λ��cm
				break;
 
			default: break;
		}
	}
}
