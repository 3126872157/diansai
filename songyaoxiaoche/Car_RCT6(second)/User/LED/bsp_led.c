/*视频教程链接 https://www.bilibili.com/video/BV1UL411V7XK?p=2&share_source=copy_web   昵称：海喂喂喂*/
#include "bsp_led.h"
void LED_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;

    __HAL_RCC_GPIOA_CLK_ENABLE();           	//开启GPIOB时钟
	   __HAL_RCC_GPIOB_CLK_ENABLE();  
	
    GPIO_Initure.Pin=GPIO_PIN_8; 				//PB5
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  	//推挽输出
    GPIO_Initure.Pull=GPIO_PULLUP;          	//上拉
    GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;    //高速
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);
	
	  GPIO_Initure.Pin=GPIO_PIN_11 | GPIO_PIN_12;
	  GPIO_Initure.Pull=GPIO_PULLDOWN; 
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);
	
	  GPIO_Initure.Pin=GPIO_PIN_15;
	  GPIO_Initure.Pull=GPIO_PULLDOWN; 
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);
	
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,GPIO_PIN_RESET);	//PB5置1，默认初始化后灯灭
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_11,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_12,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOB,GPIO_PIN_15,GPIO_PIN_RESET);
}
