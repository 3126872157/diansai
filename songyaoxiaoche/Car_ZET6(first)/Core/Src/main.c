

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
#include "bsp_debug.h"
#include "menu.h"
#include "protocol.h"
#include "bsp_pid.h"
#include "openmv.h"
#include "bsp_GraySensor.h"
#include "BlueTooth.h"

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
int iTempTim3Encoder; //临时存放从TIM3编码器接口捕获到的脉冲数据
int iTempTim1Encoder; 


u16 showOLEDTime = 0;
u16 SendTime = 0;

u8 Do_count = 0, Do2_count = 0;

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
  MX_TIM3_Init();
  MX_TIM1_Init();
  MX_TIM5_Init();
  MX_TIM8_Init();
  MX_TIM6_Init();
  MX_TIM7_Init();
  MX_USART3_UART_Init();
  MX_ADC2_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  
	HAL_ADCEx_Calibration_Start(&hadc2);

//如果要使用L298N H桥的效果，两个定时器用来做正交解码，至少需要4个PWM作为控制输出     
//要测试出编码器与电机是否对应  （小对小，大对大）
	HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);   //开启TIM3的编码器接口模式
  HAL_TIM_Encoder_Start(&htim1, TIM_CHANNEL_ALL);   //开启TIM1的编码器接口模式


	HAL_TIM_PWM_Start(&htim5,TIM_CHANNEL_3);       //Motor 2  
	HAL_TIM_PWM_Start(&htim5,TIM_CHANNEL_4);
	
	HAL_TIM_PWM_Start(&htim8,TIM_CHANNEL_3);     //Motor1
	HAL_TIM_PWM_Start(&htim8,TIM_CHANNEL_4);   

	
	delay_ms(100);    //延时一下，让OLED正常输出
	OLED_Init();
	delay_ms(100);    //延时一下，让OLED正常输出
	
  OLED_Clear(); //因为是直接进来的，所以清除一下比较好
	
	PID_param_init();  
	 
	
	//protocol_init();       //这个初始化了才能通过上位机控制呀, 再看这个注意事项
	
	 HAL_TIM_Base_Start_IT(&htim7);
	 HAL_TIM_Base_Start_IT(&htim6);
	 
	HAL_UART_Receive_IT(&huart3,&uart3_rxbuff,1);
	
	 __HAL_UART_ENABLE_IT(&huart1,UART_IT_RXNE);    //串口1接收中断
	 
	
	
#if defined(PID_ASSISTANT_EN)
  set_computer_value(SEND_STOP_CMD, CURVES_CH1, NULL, 0);    // 同步上位机的启动按钮状态
	//set_computer_value(SEND_TARGET_CMD, CURVES_CH1,&TargetSpeed, 1); 
#endif

	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1)
	{
		if(SendTime >= 21)
		{
			SendTime = 0;
			SendDataToOpenmv();   //不能太快，否则会超过openmv的串口接收数据缓冲区
			
			//蓝牙发送数据这里发
			BlueToothTransmitData();
			
			
		}
		
		if(showOLEDTime >= 200)   //本来这里是给OLED刷新的，
		{
			showOLEDTime = 0;
		
			OLED_Display(Menu_Item);		
			
			/****判断是否有放药品******/  
			LoadOrNot();
		}
		
		//deal_serial_data();   //这个函数用来处理没有用协议的数据，可直接通过串口发送指令来控制
		
		
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
						/***********以openmv发过来的标志位为触发事件***********/ 						 //标志位用完记得及时清零
					if(TargetRoom == 'A')   //这个位置理解为A房，数字为1
					{
						switch(Do_count)
						{
							case 0:
								 Do_count++;								 
								 Car_go(60-5+15+HeadToWheelCM);     //此时 Stop_Flag ==0  完成后自动置1        //近端病房不预留openmv识别的位置
								  
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
									Car_go(60-5+15+HeadToWheelCM); 

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
									 Car_go(40-5+15-HeadToWheelCM +3);    //Stop_Flag再次置1   +3是为了进停车区  
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
					
					//中端病房和远端病房需要发送特定标志位
					
					else      //TargetRoom == 'C' || TargetRoom == 'D' || TargetRoom == 'E' || TargetRoom == 'F' || TargetRoom == 'G' || TargetRoom == 'H'
					{
						switch(Do_count)
						{
							case 0:
								 Do_count++;
								 Car_go(150-5+15 - LookCM - 1);     //此时 Stop_Flag ==0  完成后自动置1   //LookCM是预留给openmv识别的距离
							break;
							
							case 1:       //判定是否识别到数字，且该数字的位置偏左还是偏右
								if(Stop_Flag == 1)
								{
									 FindStartFlag = 1;  //这个有没有都一样呀
									
										if( FindTimeCount < WaitTime_ms )              //三个条件都要满足：数字相同、位置、不超时。才算在次位置识别到数字。
									 {
											if(RoomNum == TargetNum && LoR == 1)  //识别到了目标数字, 且其位置偏左  
											{
												 Do_count  = 6;  //后续直至左转并直行到病房门口的函数
												 TargetRoom = 'C';
												 Car_go(LookCM+HeadToWheelCM);      //补回给openmv识别预留的位置，同时让车轮压线，以便后续转弯
												 
												//停止计时,并清零
												 FindTimeCount = 0;
												 FindStartFlag = 0;
											}
											else if(RoomNum == TargetNum && LoR == 2 )  //识别到目标数字, 且其位置偏右
											{
												 Do_count  = 6;        //后续直至右转并直行到病房门口的函数
												 TargetRoom = 'D';
												 Car_go(LookCM + HeadToWheelCM);      //补回给openmv识别预留的位置，同时让车轮压线，以便后续转弯
												 
												 //停止计时,并清零
												 FindTimeCount = 0;
												 FindStartFlag = 0;
											}
										
									 }										 
									 
									 //超时还未识别到，则判定为不在目标房间不在此处
									 else if(FindTimeCount >= WaitTime_ms)     //中端病房在一定时间内并没有识别到相应数字 
									 {
										 Do_count++;
										 Car_go(90 - 1);    //直行至远端病房的第一个路口前   //-1防止过冲
										 
										 //停止计时,并清零
										 FindTimeCount = 0;
										 FindStartFlag = 0;
									 }
								}
							break;
								 
							case 2:
								if(Stop_Flag == 1)     //这里需要同时识别四个数字， openmv只识别从左往右数的那三个？如果没有识别到就默认在第四个位置
								{
									FindStartFlag = 1;
										
									if(FindTimeCount < WaitTime_ms)    //一定时间内识别到RoomNum, 且其位置偏左     三个条件都要满足：时间、识别成功标志位、位置
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

									else if(FindTimeCount >= WaitTime_ms)   //仍然超时并未识别到，则保留初始化是赋予的'G'
									{
									   Do_count++; 
										 Car_go(LookCM + HeadToWheelCM);      //补回给openmv识别预留的位置，同时让车轮压线，以便后续转弯
											
											//停止计时,并清零
										 FindTimeCount = 0;
										 FindStartFlag = 0;
									}
								}
							break;
								
							case 3:
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
									
							case 4:
								if(Spin_succeed_flag == 1)
								{
									Do_count++;
									Car_go(90 -HeadToWheelCM -LookCM - 1);  //多减1防止过冲
								}
							break;
								
							case 5:
								if(Stop_Flag == 1)
								{
									FindStartFlag = 1;
									if(FindTimeCount < WaitTime_ms)
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
									
									else if(FindTimeCount >= WaitTime_ms)   
									{
										Do_count++;  
										Car_go(LookCM + HeadToWheelCM);
										
										
										FindStartFlag=0;
										FindTimeCount=0;
									}
										
								}
							break;
								
							case 6:
								if(Stop_Flag == 1)
								{
									Do_count++;
									if(TargetRoom == 'C' || TargetRoom == 'E' || TargetRoom == 'G')
									{
										spin_Turn(left_90);
									}
									else if(TargetRoom == 'D' || TargetRoom == 'F' || TargetRoom == 'H')
									{
										spin_Turn(right_90);
									}
								}
							break;
								
							case 7:
								if(Spin_succeed_flag == 1)
								{
									Do_count++;
									Car_go(40-5+15-HeadToWheelCM +3);
								}
							break;
								
							case 8:
								if(Stop_Flag == 1)
								{
									
									RedSignal_on;
									 //点亮红色指示灯
								}
							break;	
								
						}
					}
			 }

			 //送药完毕返回 
			 else if(Load_flag == 2)       // 全部房间都可返回了，就是远端的三叉路口那可能有时候会出现问题。
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
								
									
									Car_go(40- 5 + 15-HeadToWheelCM +3);   //40-5+15-13+3 //15是车头到驱动轮的距离  //此时 Stop_Flag ==0  完成后自动置1        //近端病房不预留openmv识别的位置
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
									 //Car_go(60);   //60-5+15-13 +3  //13是驱动轮到车头的距离   //Stop_Flag再次置1
										Car_go(60);   //60-5+15-13 +3  //13是驱动轮到车头的距离   //Stop_Flag再次置1
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
								
									
									Car_go(40-5+15 -HeadToWheelCM +3);    //此时 Stop_Flag ==0  完成后自动置1        //近端病房不预留openmv识别的位置
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
									// Car_go(60);   //60-5+15-13 +3  //13是驱动轮到车头的距离   //Stop_Flag再次置1
										Car_go(60);   //60-5+15-13 +3  //13是驱动轮到车头的距离   //Stop_Flag再次置1
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
					
					else if(TargetRoom == 'C')   //中端病房   这两个与近端病房的差别只是在最后直走的时候走多了几十cm而已
					{
						switch(Do2_count)
						{
							case 0:
								Do2_count++;
								RedSignal_off; //关闭红色指示灯
								spin_Turn(back_180);
							break;
							
							case 1:
								if(Spin_succeed_flag == 1)
								{
									Do2_count++;								 
									Car_go(40-5+15-HeadToWheelCM+3);    //13是车头到驱动轮的距离  //此时 Stop_Flag ==0  完成后自动置1        //近端病房不预留openmv识别的位置
								}
							break;
					 
							case 2:
								 if(Stop_Flag ==1)
								 {
									 Do2_count++;
									 spin_Turn(right_90);   //此时Spin_succeed_flag== 0 ,完成后自动置1
								 }
							break;
							 
							case 3:
								 if(Spin_succeed_flag == 1)
								 {
									 Do2_count++;
									 Car_go(150-5 +3);      //Stop_Flag再次置1
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
					
					else if(TargetRoom == 'D')
					{
						switch(Do2_count)
						{
							case 0:
								Do2_count++;
								RedSignal_off;//关闭红色指示灯
								spin_Turn(back_180);
							break;
							
							case 1:
								if(Spin_succeed_flag == 1)
								{
									Do2_count++;								 
									Car_go(40-5+15-HeadToWheelCM+3);    //13是驱动轮到车头的距离  //此时 Stop_Flag ==0  完成后自动置1      
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
									 Car_go(150-5 +3);     //13是驱动轮到车头的距离   //Stop_Flag再次置1   
									 
									 //小车跑到这个部分的时候巡线可能会寻不了。。。。，不知道是场地的反光还是什么原因所致
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
					
					else if(TargetRoom == 'E')   //远端病房   四个只是在两个路口的转向不同而已
					{
						switch(Do2_count)
						{
							case 0:
								Do2_count++;
								RedSignal_off;//关闭红色指示灯
								spin_Turn(back_180);
							break;
							
							case 1:
								if(Spin_succeed_flag == 1)
								{
									Do2_count++;								 
									Car_go(40-5+15-HeadToWheelCM+3);    //13是驱动轮到车头的距离  //此时 Stop_Flag ==0  完成后自动置1      
								}
							break;
								
							case 2:
								 if(Stop_Flag ==1)
								 {
									 Do2_count++;
									 spin_Turn(right_90);   //此时Spin_succeed_flag== 0 ,完成后自动置1
								 }
							break;
								 
							case 3:
								if(Spin_succeed_flag == 1)
								{
									Do2_count++;
									Car_go(60+30);
								}
							break;
								
							case 4:
								if(Stop_Flag == 1)
								{
									Do2_count++;
									spin_Turn(right_90);
								}
							break;
								
							case 5:
								if(Spin_succeed_flag == 1)
								{
									Do2_count++;
									Car_go(240-5+3);
								}
							break;
								
							case 6:
								if(Stop_Flag == 1)
								{
									GreenSignal_on; //点亮绿色指示灯
								}
							break;
							
						}
					}
					
					else if(TargetRoom == 'F')
					{
						switch(Do2_count)
						{
							case 0:
								Do2_count++;
								RedSignal_off;//关闭红色指示灯
								spin_Turn(back_180);
							break;
							
							case 1:
								if(Spin_succeed_flag == 1)
								{
									Do2_count++;								 
									Car_go(40-5+15-13+3);    //13是驱动轮到车头的距离  //此时 Stop_Flag ==0  完成后自动置1      
								}
							break;
								
							case 2:
								 if(Stop_Flag ==1)
								 {
									 Do2_count++;
									 spin_Turn(left_90);   //此时Spin_succeed_flag== 0 ,完成后自动置1
								 }
							break;
								 
							case 3:
								if(Spin_succeed_flag == 1)
								{
									Do2_count++;
									Car_go(60+30);
								}
							break;
								
							case 4:
								if(Stop_Flag == 1)
								{
									Do2_count++;
									spin_Turn(right_90);
								}
							break;
								
							case 5:
								if(Spin_succeed_flag == 1)
								{
									Do2_count++;
									Car_go(240-5+3);
								}
							break;
								
							case 6:
								if(Stop_Flag == 1)
								{
									GreenSignal_on; //点亮绿色指示灯
								}
							break;
							
						}
					}
					
					else if(TargetRoom == 'G')
					{
						switch(Do2_count)
						{
							case 0:
								Do2_count++;
								RedSignal_off;//关闭红色指示灯
								spin_Turn(back_180);
							break;
							
							case 1:
								if(Spin_succeed_flag == 1)
								{
									Do2_count++;								 
									Car_go(40-5+15-HeadToWheelCM+3);    //13是驱动轮到车头的距离  //此时 Stop_Flag ==0  完成后自动置1      
								}
							break;
								
							case 2:
								 if(Stop_Flag ==1)
								 {
									 Do2_count++;
									 spin_Turn(right_90);   //此时Spin_succeed_flag== 0 ,完成后自动置1
								 }
							break;
								 
							case 3:
								if(Spin_succeed_flag == 1)
								{
									Do2_count++;
									Car_go(60+30);
								}
							break;
								
							case 4:
								if(Stop_Flag == 1)
								{
									Do2_count++;
									spin_Turn(left_90);
								}
							break;
								
							case 5:
								if(Spin_succeed_flag == 1)
								{
									Do2_count++;
									Car_go(240-5+3);
								}
							break;
								
							case 6:
								if(Stop_Flag == 1)
								{
									 GreenSignal_on;//点亮绿色指示灯
								}
							break;
							
						}
					}
					
					else if(TargetRoom == 'H')
					{
						switch(Do2_count)
						{
							case 0:
								Do2_count++;
								RedSignal_off;  //关闭红色指示灯
								spin_Turn(back_180);
							break;
							
							case 1:
								if(Spin_succeed_flag == 1)
								{
									Do2_count++;								 
									Car_go(40-5+15-HeadToWheelCM+3);    //13是驱动轮到车头的距离  //此时 Stop_Flag ==0  完成后自动置1      
								}
							break;
								
							case 2:
								 if(Stop_Flag ==1)
								 {
									 Do2_count++;
									 spin_Turn(left_90);   //此时Spin_succeed_flag== 0 ,完成后自动置1
								 }
							break;
								 
							case 3:
								if(Spin_succeed_flag == 1)
								{
									Do2_count++;
									Car_go(60+30);
								}
							break;
								
							case 4:
								if(Stop_Flag == 1)
								{
									Do2_count++;
									spin_Turn(left_90);
								}
							break;
								
							case 5:
								if(Spin_succeed_flag == 1)
								{
									Do2_count++;
									Car_go(240-5+3);
								}
							break;
								
							case 6:
								if(Stop_Flag == 1)
								{
									GreenSignal_on; //点亮绿色指示灯
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
