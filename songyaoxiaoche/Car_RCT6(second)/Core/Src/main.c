/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "adc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "oled.h"
#include "bsp_key.h"
#include "bsp_sys.h"
#include "bsp_delay.h"
#include "control.h"
#include "bsp_led.h"
#include "menu.h"
#include "Fire_protocol.h"

#include "bsp_pid.h"
#include "openmv.h"
#include "bsp_GraySensor.h"

#include "bsp_BlueTooth.h"



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

u8 Do_count = 0, Do2_count = 0;

u16 showOLEDTime = 0;
u16 SendTime = 0;

extern unsigned char BMP3[];
extern unsigned char BMP4[];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	/*视频教程链接 https://www.bilibili.com/video/BV1UL411V7XK?p=2&share_source=copy_web   昵称：海喂喂喂*/
	
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
	LED_Init();
	KEY_GPIO_Init();

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
	delay_init(72);

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ADC1_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_TIM6_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  MX_TIM7_Init();
  MX_TIM8_Init();
  MX_TIM2_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
	
	
	
	HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);   //开启TIM3的编码器接口模式
  HAL_TIM_Encoder_Start(&htim8, TIM_CHANNEL_ALL);   //开启TIM8的编码器接口模式


	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_3);       //Motor 2  
	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_4);
	
	HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_3);     //Motor1
	HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_4);   
	
	
	delay_ms(100);    //延时一下，让OLED正常输出
	OLED_Init();
	delay_ms(100);    //延时一下，让OLED正常输出
	
  OLED_Clear(); //因为是直接进来的，所以清除一下比较好
	
	PID_param_init();  

  set_motor2_enable();    //上位机调试pid时才需要手动使能电机的PWM输出
	set_motor_enable();  
	
	 HAL_TIM_Base_Start_IT(&htim7);
	 HAL_TIM_Base_Start_IT(&htim6);
	 
	 
	 HAL_UART_Receive_IT(&huart3,&uart3_rxbuff,1);  //openmv通信串口
	 
	 HAL_UART_Receive_IT(&huart2,&uart2_rxbuff,1);  //蓝牙通信串口
	
	 __HAL_UART_ENABLE_IT(&huart1,UART_IT_RXNE);    //串口1接收中断，只开启接收中断，与上位机通信
	
	
		protocol_init();       //这个初始化了才能通过上位机控制呀, 再看这个注意事项
		
	 __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_0);
	 
	

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		receiving_process();   //解析野火上位机通信协议,调试电机位置速度串级PID时用
		
		
		if(SendTime >= 21)
		{
			SendTime = 0;
			
			SendDataToOpenmv();   //不能太快，否则会超过openmv的接收数据缓冲区
			
		}
		
		if(showOLEDTime >= 200)
		{
			showOLEDTime = 0;
			OLED_Display(Menu_Item);
			
//			if(OneLoadFlag == 1)
//			{
//				BlueSignal_Toggle;
//			}
			//printf("123");
		  
			/****判断是否有放药品******/  
			LoadOrNot();
		}
		
	 
		
	  /**************等待识别病房号**************/   
		if(TASK == 1)   
		{
			SetTargetRoom();
			
		}
		
		
		else if(TASK == 2)    
	  {
			 //加上发送给openmv 的数据
				if(Load_flag == 1)    //运送药物
				{
						//近端病房，靠openmv规划	， 用来测试二车性能
						if(TargetRoom == 'A')   
						{
							switch(Do_count)
							{
								case 0:
									 Do_count++;								 
									 Car_go(60-5+15+HeadToWheelCM);   //此时 Stop_Flag ==0  完成后自动置1        //近端病房不预留openmv识别的位置
								
								break;
						 
								case 1:
									 if(Stop_Flag ==1)          //直行、转弯函数交替使用时，可以不手动将标志位清零
									 {
										 Do_count++;
										 spin_Turn(left_90);   //此时Spin_succeed_flag== 0 ,完成后自动置1
									 }
								break;
								 
								case 2:
									 if(Spin_succeed_flag == 1)
									 {
										 Do_count++;
										 Car_go(40-5+15-HeadToWheelCM +3);      //Stop_Flag再次置1   +3是为了进停车区  
									 }
								break;
									 
								case	3:
									if(Stop_Flag ==1)
									{
										RedSignal_on;
										
											//点亮红色指示灯
										
									}
								break;
							}			 
						}
						
						
						else if(TargetRoom == 'B')
						{
							switch(Do_count)
							{
								case 0:
									 Do_count++;								 
																									
										Car_go(60-5+15+HeadToWheelCM);  //此时 Stop_Flag ==0  完成后自动置1        //近端病房不预留openmv识别的位置

								break;
						 
								case 1:
									 if(Stop_Flag ==1)
									 {
										 Do_count++;
										 spin_Turn(right_90);   //此时Spin_succeed_flag== 0 ,完成后自动置1
									 }
								break;
								 
								case 2:
									 if(Spin_succeed_flag == 1)
									 {
										 Do_count++;
										 Car_go(40-5+15-HeadToWheelCM +3);        //Stop_Flag再次置1   +3是为了进停车区  
									 }
								break;
									 
								case	3:
									if(Stop_Flag ==1)
									{
										
										RedSignal_on;
											//点亮红色指示灯
										
									}
								break;
							}			 
						}
						
						if(OneLoadFlag == 1)
						{
								//去中端送药，靠一车等待目标房号来确定，不用返回
							if(OneTargetRoom == 'C')
							{
								 switch(Do_count)
								 {
										case 0:
											 Do_count++;								 
											 Car_go(160+HeadToWheelCM);    //此时 Stop_Flag ==0  完成后自动置1        //近端病房不预留openmv识别的位置
											 
										break;

										case 1:
											 if(Stop_Flag ==1)      
											 {
												 Do_count++;
												 spin_Turn(right_90 );   //此时Spin_succeed_flag== 0 ,完成后自动置1
												
											 }
										break;
										 
										case 2:
											 if(Spin_succeed_flag == 1)   //这里躲入另外一个中端病房通道，  测试一下，看掉头效果好还是直接后退效果好
											 {
												 Do_count++;   
												 Car_go(HideCM);        //Stop_Flag再次置1    //如果后退的话，巡线补偿的值需要相反  
												
											 }
										break;
											 
										case 3:
											if(Stop_Flag ==1)        
											{
												
												YellowSignal_on; //等一车取药
												
											}
										break;
											 
								 }
							}
									
								
							else if(OneTargetRoom == 'D' )
							{
								 switch(Do_count)
								 {
										case 0:
											 Do_count++;								 
											 Car_go(150-5+15+HeadToWheelCM);    //此时 Stop_Flag ==0  完成后自动置1        //近端病房不预留openmv识别的位置
											 
										break;

										case 1:
											 if(Stop_Flag ==1)         
											 {
												 Do_count++;
												 spin_Turn(left_90);   //此时Spin_succeed_flag== 0 ,完成后自动置1
												
											 }
										break;
										 
										case 2:
											 if(Spin_succeed_flag == 1)   //这里躲入另外一个中端病房通道，  测试一下，看掉头效果好还是直接后退效果好
											 {
												 Do_count++;  
												 Car_go(HideCM);        //Stop_Flag再次置1   //如果后退的话，巡线补偿的值需要相反  
												
											 }
										break;
											 
										case 3:
											if(Stop_Flag ==1)          
											{
												
												YellowSignal_on;  //等一车取药
												
											}
										break;
											 
										
								 }
							}
						}
				}
			
				else if(Load_flag == 2)       // 只在近端才返回,用来测试二车控制效果  
				{
						if(TargetRoom == 'A')
						{
							switch(Do2_count)
							{
								case 0:
									Do2_count++;
									 
									spin_Turn(back_180 );
									RedSignal_off; //关闭红色指示灯
									
								break;
							
								
								case 1:
									if(Spin_succeed_flag == 1)
									{
										Do2_count++;
									
										
										Car_go(40-5+15-HeadToWheelCM+3);   //此时 Stop_Flag ==0  完成后自动置1        //近端病房不预留openmv识别的位置
									}
								break;
						 
								case 2:
									 if(Stop_Flag ==1)
									 {
										 Do2_count++;
										 spin_Turn(right_90 );   //此时Spin_succeed_flag== 0 ,完成后自动置1
									 }
								break;
								 
								case 3:
									 if(Spin_succeed_flag == 1)
									 {
										 
										 
										 Do2_count++;
										 Car_go(60-5+15-HeadToWheelCM +3 );   //Stop_Flag再次置1
									
									 }
								break;
									 
								case	4:
									if(Stop_Flag ==1)
									{
									
										GreenSignal_on; //点亮绿色指示灯
										
									}
								break;
							}	               

						}
						
						else if(TargetRoom == 'B')
						{
							switch(Do2_count)
							{
								case 0:
									Do2_count++;
									 
									spin_Turn(back_180 );
									RedSignal_off; //关闭红色指示灯
									
								break;
							
								
								case 1:
									if(Spin_succeed_flag == 1)
									{
										Do2_count++;
									
										
										Car_go(40-5+15-HeadToWheelCM+3);     //此时 Stop_Flag ==0  完成后自动置1        //近端病房不预留openmv识别的位置
									}
								break;
						 
								case 2:
									 if(Stop_Flag ==1)
									 {
										 Do2_count++;
										 spin_Turn(left_90 );   //此时Spin_succeed_flag== 0 ,完成后自动置1
									 }
								break;
								 
								case 3:
									 if(Spin_succeed_flag == 1)
									 {
										 
										 
										 Do2_count++;
										 Car_go(60-5+15-HeadToWheelCM +3);    //Stop_Flag再次置1
										
									 }
								break;
									 
								case	4:
									if(Stop_Flag ==1)
									{
									
										GreenSignal_on; //点亮绿色指示灯
										
									}
								break;
							}
						}
				}
			
			
			if(OneLoadFlag == 2)
			{

        if(OneTargetRoom == 'C' || OneTargetRoom == 'D')
				{
					switch(Do_count)
					{
						
						case 3:
							//来个延时的标志位
							 WaitFlag = 1;
						   YellowSignal_off;	 
						
							 if(WaitTimeCount >=  6000)  //延时6s
							 {
								 Do_count++;
								 
								 WaitFlag = 0;
								 WaitTimeCount = 0;
								 
								 spin_Turn(back_180);
							 
						   }
					  break;
							 
						case	4:
							if(Spin_succeed_flag ==  1)
							{
								 Do_count++;
								
								 Car_go(HideCM + 40-5+15-HeadToWheelCM +3);
							}
						break;
							
					  case 5:
							if(Stop_Flag == 1)
							{
								Do_count++;
								
								RedSignal_on;
							}
						
						break; 
					}
				}  				
			
				//去远端取药，不用管是否装载药品，先在中端等一车过了再去远端。 
			  if(OneTargetRoom == 'E' || OneTargetRoom == 'F'  ||  OneTargetRoom == 'G' ||OneTargetRoom == 'H' )   // 由一车发送的标志位控制二车启动
				{
					switch(Do_count)
					{
						 case 0:
							 Do_count++;								 
							 Car_go(150-5+15+HeadToWheelCM-2);    //此时 Stop_Flag ==0  完成后自动置1        //近端病房不预留openmv识别的位置
							 
						 break;

						 case 1:
							 if(Stop_Flag ==1)         
							 {
								 if(OneTargetRoom == 'E' || OneTargetRoom == 'F')
								 {
									 Do_count++;
									 spin_Turn(right_90);   //此时Spin_succeed_flag== 0 ,完成后自动置1
								 }
								 
								 else if(OneTargetRoom == 'G' ||OneTargetRoom == 'H')
								 {
									 Do_count++;
									 spin_Turn(left_90);   //此时Spin_succeed_flag== 0 ,完成后自动置1
								 }
								
							 }
						 break;
							 
						 case 2:					 
							 if(Spin_succeed_flag == 1)
							 {
									Do_count++;
									Car_go(HideCM);
							 }
						 break;
							 
						 case 3:
							 if(Stop_Flag == 1)
							 {
									WaitFlag = 1;
								  Do_count++;
							 }
						 break;
							 
						 case 4:
							 if(WaitTimeCount >= 3000 )  //等待3s
							{
								Do_count++;
								WaitTimeCount = 0;
								WaitFlag = 0;
								
								spin_Turn(back_180);
							}
             break;						 
							 
							 
						 case 5:
							 if(Spin_succeed_flag == 1)
							 {
								 Do_count++;
							   Car_go(HideCM );
							 }
						 break;
							
						 case	6:
							 if(Stop_Flag ==1)         
							 {
								 if(OneTargetRoom == 'E' || OneTargetRoom == 'F')
								 {
									 Do_count++;
									 spin_Turn(right_90 );   //此时Spin_succeed_flag== 0 ,完成后自动置1
								 }
								 
								 else if(OneTargetRoom == 'G' ||OneTargetRoom == 'H')
								 {
									 Do_count++;
									 spin_Turn(left_90 );   //此时Spin_succeed_flag== 0 ,完成后自动置1
								 }
								
							 }
						 break;

						 case	7:
							 if(Spin_succeed_flag == 1)
							 {
									Do_count++;
									Car_go(90- (LookCM + HeadToWheelCM) );
							 }
						 break;

						 case	8:
							 if(Stop_Flag == 1 )
							 {
									FindStartFlag = 1;
											
									if(FindTimeCount < 500)    //一定时间内识别到RoomNum, 且其位置偏左     三个条件都要满足：时间、识别成功标志位、位置
									{
										if(RoomNum == TargetNum && LoR == 1) 
										{
											 Do_count++; 
											 TargetRoom = 'E';      //先假定是E
											 Car_go(LookCM+HeadToWheelCM);      //补回给openmv识别预留的位置，同时让车轮压线，以便后续转弯
											
											//停止计时,并清零
											 FindTimeCount = 0;
											 FindStartFlag = 0;
										}
										else if(RoomNum == TargetNum && LoR == 2 )  	//识别到目标数字, 且其位置偏右
										{
											 Do_count++; 
											 TargetRoom = 'G';    //先假定是G
											 Car_go(LookCM + HeadToWheelCM);      //补回给openmv识别预留的位置，同时让车轮压线，以便后续转弯
											
											//停止计时,并清零
												 FindTimeCount = 0;
												 FindStartFlag = 0;
										}										
										
									}										 

									else if(FindTimeCount >= 500)   //仍然超时并未识别到，则保留初始化是赋予的'E'
									{
										 Do_count++; 
										 Car_go(LookCM + HeadToWheelCM);      //补回给openmv识别预留的位置，同时让车轮压线，以便后续转弯
											
											//停止计时,并清零
										 FindTimeCount = 0;
										 FindStartFlag = 0;
									}
										
								}
						 break;
						 
						 case	9:
								if(Stop_Flag == 1)
								{
									Do_count++;
									if(TargetRoom == 'E')
									{
										spin_Turn(left_90);
									}
									else if(TargetRoom == 'G')
									{
										spin_Turn(right_90);
									}
								}
						 break;
								
						 case 10:
								if(Spin_succeed_flag == 1)
								{
									Do_count++;
									Car_go(90 -HeadToWheelCM -LookCM);
								}
						 break;
						 
						 case 11:
								if(Stop_Flag == 1)
								{
									FindStartFlag = 1;
									if(FindTimeCount < 500)
									{
										if(TargetRoom == 'E')
										{
												if(RoomNum == TargetNum && LoR == 1)    //识别到RoomNum, 且其位置偏左    
											 {
												 Do_count ++;  
												 TargetRoom = 'E';
												 Car_go(LookCM+HeadToWheelCM);      //补回给openmv识别预留的位置，同时让车轮压线，以便后续转弯
												 
												 FindStartFlag = 0;
												 FindTimeCount =0;
											 }										 
											 
											 else if(RoomNum == TargetNum && LoR == 2)   //一定时间内识别到RoomNum, 且其位置偏右
											 {
												 Do_count++;  
												 TargetRoom = 'F';
												 Car_go(LookCM + HeadToWheelCM);
												 
												 FindStartFlag = 0;
												 FindTimeCount =0;
											 }
										}
										
										else if(TargetRoom == 'G')
										{
												if(RoomNum == TargetNum && LoR == 1)    //一定时间内识别到RoomNum, 且其位置偏左    
											 {
												 Do_count++; 
												 TargetRoom = 'G';
												 Car_go(LookCM+HeadToWheelCM);												 
												 
												 FindStartFlag = 0;
												 FindTimeCount =0;
											 }										 
											 
											 else if(RoomNum == TargetNum && LoR == 2)   //一定时间内识别到RoomNum, 且其位置偏右
											 {
												 Do_count++;  
												 TargetRoom = 'H';
												 Car_go(LookCM + HeadToWheelCM);
												 
												 FindStartFlag = 0;
												 FindTimeCount =0;
											 }
										}		
									}
									
									else if(FindTimeCount >= 500)   
									{
										Do_count++;  
										Car_go(LookCM + HeadToWheelCM);
										
										
										FindStartFlag=0;
										FindTimeCount=0;
									}
										
								}
						 break;
						 
						 case 12:
								if(Stop_Flag == 1)
								{
									Do_count++;
									if( TargetRoom == 'E' || TargetRoom == 'G')
									{
										spin_Turn(left_90);
									}
									else if( TargetRoom == 'F' || TargetRoom == 'H')
									{
										spin_Turn(right_90);
									}
								}
						 break;
						 
						 case 13:
								if(Spin_succeed_flag == 1)
								{
									Do_count++;
									Car_go(40-5+15-HeadToWheelCM +3);
								}
						 break;					 
						
						 case 14:
								if(Stop_Flag == 1)
									{
										
										RedSignal_on;
										 //点亮红色指示灯
									}
						 break;					 
						
					}
				}
				
			}
		}
		
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
