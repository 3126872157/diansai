/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f1xx_it.c
  * @brief   Interrupt Service Routines.
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f1xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "control.h"
#include "bsp_sys.h"
#include "bsp_key.h"
#include "menu.h"

#include "bsp_led.h"
#include "openmv.h"
#include "bsp_sys.h"
#include "bsp_delay.h"

//#include "Fire_protocol.h"

#include "bsp_BlueTooth.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

unsigned char g_ucMainEventCount = 0;  //主事件计数，会用在中断中

uint8_t dr;
volatile uint8_t sr_status;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern TIM_HandleTypeDef htim6;
extern TIM_HandleTypeDef htim7;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;
/* USER CODE BEGIN EV */

extern u16 showOLEDTime;
extern u16 SendTime;

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M3 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
  while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Prefetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVCall_IRQn 0 */

  /* USER CODE END SVCall_IRQn 0 */
  /* USER CODE BEGIN SVCall_IRQn 1 */

  /* USER CODE END SVCall_IRQn 1 */
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */
	 g_ucMainEventCount++;
	 
	//人机交互功能获取节拍
	showOLEDTime++;
  SendTime++;

	//任务过程中的延时节拍获取
	if(FindStartFlag ==1)
  {
		FindTimeCount++;  
	}	

  if(WaitFlag == 1)
	{
		WaitTimeCount++;
	}		
	
	//严格定时执行的代码
	if(g_ucMainEventCount >= 10)   //5ms执行一次
	{
		g_ucMainEventCount = 0;
	}
	
	if(g_ucMainEventCount == 1) 
	{
	  ButtonScan();
		MENU_Item_KEY();
	}
	
	
  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */
	HAL_SYSTICK_IRQHandler();   //必须加这个语句，否则不能正常使用sysTick的更新中断

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32F1xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f1xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles EXTI line0 interrupt.
  */
void EXTI0_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI0_IRQn 0 */

  /* USER CODE END EXTI0_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
  /* USER CODE BEGIN EXTI0_IRQn 1 */

  /* USER CODE END EXTI0_IRQn 1 */
}

/**
  * @brief This function handles USART1 global interrupt.
  */
void USART1_IRQHandler(void)
{
  /* USER CODE BEGIN USART1_IRQn 0 */
	
	//使用野火上位机查看波形时的用这个
	sr_status = huart1.Instance->SR & (1<<3);  //clear SR register ORE bit status
	dr = huart1.Instance->DR;
  protocol_data_recv(&dr, 1);
	
	//RedSignal_Toggle;
	
	
  /* USER CODE END USART1_IRQn 0 */
  HAL_UART_IRQHandler(&huart1);
  /* USER CODE BEGIN USART1_IRQn 1 */

  /* USER CODE END USART1_IRQn 1 */
}

/**
  * @brief This function handles USART2 global interrupt.
  */
void USART2_IRQHandler(void)
{
  /* USER CODE BEGIN USART2_IRQn 0 */

  /* USER CODE END USART2_IRQn 0 */
  HAL_UART_IRQHandler(&huart2);
  /* USER CODE BEGIN USART2_IRQn 1 */

  /* USER CODE END USART2_IRQn 1 */
}

/**
  * @brief This function handles USART3 global interrupt.
  */
void USART3_IRQHandler(void)
{
  /* USER CODE BEGIN USART3_IRQn 0 */

  /* USER CODE END USART3_IRQn 0 */
  HAL_UART_IRQHandler(&huart3);
  /* USER CODE BEGIN USART3_IRQn 1 */

  /* USER CODE END USART3_IRQn 1 */
}

/**
  * @brief This function handles TIM6 global interrupt.
  */
void TIM6_IRQHandler(void)
{
  /* USER CODE BEGIN TIM6_IRQn 0 */

  /* USER CODE END TIM6_IRQn 0 */
  HAL_TIM_IRQHandler(&htim6);
  /* USER CODE BEGIN TIM6_IRQn 1 */

  /* USER CODE END TIM6_IRQn 1 */
}

/**
  * @brief This function handles TIM7 global interrupt.
  */
void TIM7_IRQHandler(void)
{
  /* USER CODE BEGIN TIM7_IRQn 0 */

  /* USER CODE END TIM7_IRQn 0 */
  HAL_TIM_IRQHandler(&htim7);
  /* USER CODE BEGIN TIM7_IRQn 1 */

  /* USER CODE END TIM7_IRQn 1 */
}

/* USER CODE BEGIN 1 */

//定时器中断回调函数在control.c里面


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  uint8_t tem1, tem2;// 这里的是无符号的
	
	 //RedSignal_Toggle;   //用来看是否接收数据的, 这里要随时都没效果的话就代表连串口的中断都没进来
	
	/**********接收openmv发送来的数据*****/
  if(huart->Instance== USART3)     //这里只能这样大写USART3
  {
		   
		//RedSignal_Toggle;   //用来看是否接收数据的, 这里要随时都没效果的话就代表连串口3的中断都没进来
		
    tem1=uart3_rxbuff;
    Openmv_Receive_Data(tem1);
		
		HAL_UART_Receive_IT(&huart3,&uart3_rxbuff,1);
  }	
	
	
	/*************接收蓝牙传来的数据**********/    //待完善 
	else if(huart->Instance== USART2)
	{
		//RedSignal_Toggle;   //用来看是否接收数据的, 这里要随时都没效果的话就代表连串口2的中断都没进来
		tem2=uart2_rxbuff;
		
		 BlueTooth_Receive_Data(tem2);
		
		HAL_UART_Receive_IT(&huart2,&uart2_rxbuff,1);
	}
	 
	
//	  HAL_UART_Receive_IT(&huart2,&uart2_rxbuff,1); 
//    HAL_UART_Receive_IT(&huart3,&uart3_rxbuff,1);   //这样子一起放在外边可不可以？只要抢断优先级相同就没啥大问题，两串口不会干扰到
	 
}


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)   //按键触发外部中断，这个其实与 定时器消抖  的效果差不多。编程也没那么麻烦
{
  
	if(GPIO_Pin == GPIO_PIN_0)
	{
		
			delay_ms(10);
		  if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == K1_DOWN_LEVEL)
			{
				
				 //BlueSignal_Toggle;   //测试能否进这个外部中断
				 
					static char i = 0;
				   
				  i = !i;
					
					if(i == 0)
					{
           
						//YellowSignal_Toggle;
						Car_go(-100);   //直走没问题
					}
					else
					{
				    
						Car_go(100);   //直走没问题

					}
					
				
						
			    #if defined(PID_ASSISTANT_EN) 
					   
					  // RedSignal_Toggle;   //可以进来的话就会翻转红色指示灯的电平状态
					
				    	int	temp;
							temp = set_point;    // 上位机需要整数参数，转换一下  
							set_computer_value(SEND_TARGET_CMD, CURVES_CH1, &temp, 1);     // 给通道 1 发送目标值
					#endif
				
			}
			__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_0);   // 这句话一定要加上，否则会一直进入中断，导致按键按下后触发很多次。  
	}
}



/* USER CODE END 1 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
