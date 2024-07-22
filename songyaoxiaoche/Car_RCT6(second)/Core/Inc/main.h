/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define L2_Pin GPIO_PIN_3
#define L2_GPIO_Port GPIOC
#define LOAD_Pin GPIO_PIN_1
#define LOAD_GPIO_Port GPIOA
#define L1_Pin GPIO_PIN_4
#define L1_GPIO_Port GPIOC
#define M_Pin GPIO_PIN_5
#define M_GPIO_Port GPIOC
#define BIN1_Pin GPIO_PIN_10
#define BIN1_GPIO_Port GPIOB
#define BIN2_Pin GPIO_PIN_11
#define BIN2_GPIO_Port GPIOB
#define red_sig_Pin GPIO_PIN_15
#define red_sig_GPIO_Port GPIOB
#define R1_Pin GPIO_PIN_8
#define R1_GPIO_Port GPIOC
#define R2_Pin GPIO_PIN_9
#define R2_GPIO_Port GPIOC
#define blue_sig_Pin GPIO_PIN_11
#define blue_sig_GPIO_Port GPIOA
#define yellow_sig_Pin GPIO_PIN_12
#define yellow_sig_GPIO_Port GPIOA
#define AIN1_Pin GPIO_PIN_8
#define AIN1_GPIO_Port GPIOB
#define AIN2_Pin GPIO_PIN_9
#define AIN2_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
