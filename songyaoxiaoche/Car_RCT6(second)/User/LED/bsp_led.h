#ifndef __BSP_LED_H
#define __BSP_LED_H
#include "stm32f1xx.h"
#include "gpio.h"
#include "main.h"

#define RedSignal_Toggle HAL_GPIO_TogglePin(red_sig_GPIO_Port, red_sig_Pin)
#define BlueSignal_Toggle HAL_GPIO_TogglePin(blue_sig_GPIO_Port, blue_sig_Pin)
#define YellowSignal_Toggle HAL_GPIO_TogglePin(yellow_sig_GPIO_Port, yellow_sig_Pin)

#define RedSignal_on  HAL_GPIO_WritePin(red_sig_GPIO_Port, red_sig_Pin,GPIO_PIN_SET)
#define RedSignal_off HAL_GPIO_WritePin(red_sig_GPIO_Port, red_sig_Pin,GPIO_PIN_RESET)

#define GreenSignal_on  HAL_GPIO_WritePin(blue_sig_GPIO_Port, blue_sig_Pin,GPIO_PIN_SET)
#define GreenSignal_off  HAL_GPIO_WritePin(blue_sig_GPIO_Port, blue_sig_Pin,GPIO_PIN_RESET)

#define YellowSignal_on  HAL_GPIO_WritePin(yellow_sig_GPIO_Port ,yellow_sig_Pin,GPIO_PIN_SET)
#define YellowSignal_off  HAL_GPIO_WritePin(yellow_sig_GPIO_Port,yellow_sig_Pin,GPIO_PIN_RESET)

#define greenLED_on  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,GPIO_PIN_RESET)
#define greenLED_off  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,GPIO_PIN_SET)

#define greenLED_Toggle HAL_GPIO_TogglePin(GPIOA,GPIO_PIN_8)

void LED_Init(void);

#endif
