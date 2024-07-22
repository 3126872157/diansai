#ifndef __BSP_LED_H
#define __BSP_LED_H
#include "stm32f1xx.h"
#include "gpio.h"

#define RedSignal_Toggle HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_1)
#define GreenSignal_Toggle HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_2)

#define RedSignal_on  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_1,GPIO_PIN_SET)
#define RedSignal_off HAL_GPIO_WritePin(GPIOC,GPIO_PIN_1,GPIO_PIN_RESET)

#define GreenSignal_on  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_2,GPIO_PIN_SET)
#define GreenSignal_off  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_2,GPIO_PIN_RESET)



#define redLED_on  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_RESET)
#define redLED_off  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_SET)

#define greenLED_on  HAL_GPIO_WritePin(GPIOE,GPIO_PIN_5,GPIO_PIN_RESET)
#define greenLED_off  HAL_GPIO_WritePin(GPIOE,GPIO_PIN_5,GPIO_PIN_SET)

#define redLED_Toggle  HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_5)
#define greenLED_Toggle HAL_GPIO_TogglePin(GPIOE,GPIO_PIN_5)


void LED_Init(void);




#endif
