/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "math.h"
#include "string.h"
#include "y_servo.h"
#include "y_global.h"
#include "y_kinematics.h"
#include "stdio.h"
#include "HMI.h"
#include "Openmv_Rx_Tx.h"
#include "flow_task.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

extern uint8_t revbuf[1];
extern uint8_t val1,val2,val3,val4,val5,val6;

extern uint8_t Rx_Openmv_mode;
extern uint8_t Tx_Openmv_mode;
extern uint8_t USART2_Rxbuffer;

extern int8_t Chess_State[9];//格子中棋子的状态(无棋子/黑棋子/白棋子)。-1,0,1
extern uint8_t Chess_Location[2][9];//格子的坐标(x, y)。

extern position blackStack[5];		//存放黑棋的坐标
extern position whiteStack[5];		//存放白棋的坐标

extern position chess_center[9];	//棋盘格子中心坐标

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

//到末端磁铁的关节长度，标定得出
float my_L3 = 20;
	
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM1_Init();
  MX_USART6_UART_Init();
  MX_USART1_UART_Init();
  MX_TIM8_Init();
  /* USER CODE BEGIN 2 */
	//初始化
	HAL_TIM_Base_Start(&htim1);
	HAL_TIM_Base_Start(&htim8);
	HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_4);
	HAL_TIM_PWM_Start(&htim8,TIM_CHANNEL_1);
	servo_init();		//机械臂归位
	HAL_Delay(2000);//等待归为完成
	//设置逆运动学必要参数：关节长度
	setup_kinematics(90, 105, 98, my_L3, &kinematics);
	
	//初始化，使能串口接收中断，使能一次进一次中断
	HAL_UART_Receive_IT(&huart1, revbuf, sizeof(revbuf));
	HAL_UART_Receive_IT(&huart6,&USART2_Rxbuffer,1);
	
	blackStack[0].x = -81;
	blackStack[0].y = 55;
	blackStack[1].x = -83;
	blackStack[1].y = 80;
	blackStack[2].x = -80;
	blackStack[2].y = 107;
	blackStack[3].x = -81;
	blackStack[3].y = 129;
	blackStack[4].x = -87;
	blackStack[4].y = 153;
	
	whiteStack[0].x = 79;
	whiteStack[0].y = 57;
	whiteStack[1].x = 79;
	whiteStack[1].y = 82;
	whiteStack[2].x = 79;
	whiteStack[2].y = 107;
	whiteStack[3].x = 79;
	whiteStack[3].y = 133;
	whiteStack[4].x = 79;
	whiteStack[4].y = 159;
	
	chess_center[0].x = 30;
	chess_center[0].y = 95;
	chess_center[1].x = -3;
	chess_center[1].y = 95;
	chess_center[2].x = -36;
	chess_center[2].y = 95;
	chess_center[3].x = 30;
	chess_center[3].y = 126;
	chess_center[4].x = -3;
	chess_center[4].y = 126;
	chess_center[5].x = -36;
	chess_center[5].y = 126;
	chess_center[6].x = 30;
	chess_center[6].y = 160;
	chess_center[7].x = -3;
	chess_center[7].y = 160;
	chess_center[8].x = -36;
	chess_center[8].y = 160;
	
  /* USER CODE END 2 */

  /* Call init function for freertos objects (in cmsis_os2.c) */
  MX_FREERTOS_Init();

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  //下列函数移到freeRTOS里的task完成了
//	servo_inc_offset(duoji_index);
//	__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,duoji_doing[duoji_index].cur);
//	duoji_index++;
//	servo_inc_offset(duoji_index);
//	__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_2,duoji_doing[duoji_index].cur);
//	duoji_index++;		
//	servo_inc_offset(duoji_index);
//	__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_3,duoji_doing[duoji_index].cur);
//	duoji_index++;
//	servo_inc_offset(duoji_index);
//	__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_4,duoji_doing[duoji_index].cur);
//	duoji_index=0;
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

////使用串口函数printf
//#ifdef __GNUC__

//  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)

//#else

//  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)

//#endif /* __GNUC__ */
//	
////串口1中断，与串口屏进行通信
//int fputc(int ch,FILE *f)
//{
//	HAL_UART_Transmit(&huart1,(uint8_t *)&ch,1,0xFFFF);
//	return ch;
//}

/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
