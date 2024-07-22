#ifndef __BSP_KEY_H__
#define __BSP_KEY_H__

#include "stm32f1xx.h"
#include "gpio.h"

//KEY1,KEY2,KEY3 用定时器消抖并不断扫描的方法检测，  其他的都用都用外部中断的方式弄吧。  （一搞好K1~K3的定时器消抖）

#define KEY_UP  HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0)
#define KEY0   HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_4)

#define KEY1   HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_1)
#define KEY2   HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_2)
#define KEY3   HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_3)

#define KEY4   HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_6)
#define KEY5   HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_0)

#define KEY1_PRES 1
#define KEY2_PRES 2
#define KEY3_PRES 3

#define KEY0_DOWN_LEVEL 1

typedef enum
{
  KEY0_UP   = 0,
  KEY0_DOWN = 1,
}KEYState_TypeDef;
 
extern char g_nButton;   //声明外部变量，方便其他地方引用。当使用定时器消抖的时候用这个,这个记录了有效按下了什么键
extern int iButtonFlag;  //记录确实按键被按下，即有效

void KEY_GPIO_Init(void);
uint8_t KEY_Scan(void);

void ButtonScan(void);

KEYState_TypeDef KEY0_StateRead(void);

#endif




