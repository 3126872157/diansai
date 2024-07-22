#ifndef __BSP_KEY_H__
#define __BSP_KEY_H__

#include "stm32f1xx.h"

//#define K1   HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0)   板载按键用外部中断
//#define K1_PRES  1


#define KEY1   HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_0)
#define KEY2   HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_1)
#define KEY3   HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_2)

#define KEY1_PRES 1
#define KEY2_PRES 2
#define KEY3_PRES 3

#define K1_DOWN_LEVEL 0

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


KEYState_TypeDef K1_StateRead(void);

#endif



