/*视频教程链接 https://www.bilibili.com/video/BV1UL411V7XK?p=2&share_source=copy_web   昵称：海喂喂喂*/
#include "menu.h"
#include <stdio.h>
#include "bsp_key.h"
#include "control.h"
#include "oled.h"
#include "encoder.h"
#include "tim.h"
#include "openmv.h"
#include "bsp_GraySensor.h"
#include "bsp_BlueTooth.h"

extern unsigned char BMP1[];   //无用的图片首页
//extern unsigned char BMP4[];  //HCF  //不要这个
extern unsigned char BMP3[];  //START

//OLED菜单有四个数据缓冲区就够了吧
u8 str_buff1[64];
u8 str_buff2[64];
u8 str_buff3[64];
u8 str_buff4[64];   
u8 str_buff5[64];
u8 str_buff6[64];

u8 Menu_Item = 132;  //这个表示一开始就跳转哪个界面


extern uint16_t ADC_Light[10];

void OLED_Display(u8 num)
{
	switch(num)      //这里光放要显示的内容
	{
		case 0:
	    OLED_DrawBMP(0, 0, 128, 8, BMP3);  
		break;
		
		
		//选中一级界面第一大选项
		case 1:
			OLED_ShowString(40, 0, "MENU", 16);
		  OLED_ShowString(20, 3, "MPU6050", 8);
		  OLED_ShowString(20, 4, "TASK", 8);
		  OLED_ShowString(20, 5, "TEST", 8);
		  OLED_ShowString(0, 3, ">>", 8);
		break;
		
		//第一大选项二级界面
		case 10:
		 OLED_ShowString(0, 0, "MPU6050 :", 16);
		 OLED_ShowString(20, 3, "Gyroscope", 8);
		 OLED_ShowString(20, 4, "Accelerometer", 8);
		 OLED_ShowString(20, 5, "Euler angle", 8);
		 OLED_ShowString(0, 3, ">>", 8);
		 break;
		
		 case 11:
		  OLED_ShowString(0, 0, "MPU6050 :", 16);
		 OLED_ShowString(20, 3, "Gyroscope", 8);
		 OLED_ShowString(20, 4, "Accelerometer", 8);
		 OLED_ShowString(20, 5, "Euler angle", 8);
		  OLED_ShowString(0, 4, ">>", 8);
		 break;
		 
		 case 12:
		  OLED_ShowString(0, 0, "MPU6050 :", 16);
		 OLED_ShowString(20, 3, "Gyroscope", 8);
		 OLED_ShowString(20, 4, "Accelerometer", 8);
		 OLED_ShowString(20, 5, "Euler angle", 8);
		  OLED_ShowString(0, 5, ">>", 8);
		 break;
		 
		 //第一大选项三级界面
			case 110:
				OLED_ShowString(0, 0, "Gyroscope ", 8);
		
//				sprintf((char *)str_buff1, "gyrox: %6d", gyrox);
//				sprintf((char *)str_buff2, "gyroy: %6d", gyroy);
//				sprintf((char *)str_buff3, "gyroz: %6d", gyroz);
//				
//				OLED_ShowString(10, 3,str_buff1, 8);
//				OLED_ShowString(10, 4,str_buff2, 8);
//				OLED_ShowString(10, 5,str_buff3, 8);		
			break;
				
			case 111 :
				OLED_ShowString(0, 0, "Accelerometer", 8);
				
				
//				sprintf((char *)str_buff1, "Accx: %6d", accx);
//				sprintf((char *)str_buff2, "Accy: %6d", accy);
//				sprintf((char *)str_buff3, "Accz: %6d", accz);
//				
//				OLED_ShowString(10, 3,str_buff1, 8);
//				OLED_ShowString(10, 4,str_buff2, 8);
//				OLED_ShowString(10, 5,str_buff3, 8);			
			break;
			
			case 112:
					OLED_ShowString(0, 0, "Euler Angle", 8);
	
//					sprintf((char *)str_buff1, "pitch:%4.2f", pitch);
//					sprintf((char *)str_buff2, "roll: %4.2f", roll);
//					sprintf((char *)str_buff3, "yaw:  %4.2f", yaw);  
//					
//					OLED_ShowString(0, 3,str_buff1, 8);
//					OLED_ShowString(0, 4,str_buff2, 8);
//					OLED_ShowString(0, 5,str_buff3, 8);
			break;
		
		
		//第二大选项
		case 2:                                  //记得与前面主菜单界面保持一直
			OLED_ShowString(40, 0, "MENU", 16);
		  OLED_ShowString(20, 3, "MPU6050", 8);
		  OLED_ShowString(20, 4, "TASK", 8);
		  OLED_ShowString(20, 5, "TEST", 8);
		  OLED_ShowString(0, 4, ">>", 8);
		  break;
		
		//第二大选项二级界面
		case 20:
		 {
			  OLED_ShowString(0, 0, "TASK", 16);
		    OLED_ShowString(20, 3, "", 8);
		    OLED_ShowString(20, 4, "", 8);
		    OLED_ShowString(20, 5, "", 8);
			  OLED_ShowString(20, 6, "", 8);
		    OLED_ShowString(0, 3, ">>", 8);
		 }
		 break;
		 
		 case 21:
		 {
			  OLED_ShowString(0, 0, "TASK", 16);
		    OLED_ShowString(20, 3, "", 8);
		    OLED_ShowString(20, 4, "", 8);
		    OLED_ShowString(20, 5, "", 8);
			  OLED_ShowString(20, 6, "", 8);
		    OLED_ShowString(0, 4, ">>", 8);
		 }
		 break;
		 
		 case 22:
		 {
			  OLED_ShowString(0, 0, "TASK", 16);
		    OLED_ShowString(20, 3, "", 8);
		    OLED_ShowString(20, 4, "", 8);
		    OLED_ShowString(20, 5, "", 8);
			  OLED_ShowString(20, 6, "", 8);
		    OLED_ShowString(0, 5, ">>", 8);
		 }
		 break;
		 
		  case 23:
		 {
			  OLED_ShowString(0, 0, "TASK", 16);
		    OLED_ShowString(20, 3, "", 8);
		    OLED_ShowString(20, 4, "", 8);
		    OLED_ShowString(20, 5, "", 8);
			  OLED_ShowString(20, 6, "", 8);
		    OLED_ShowString(0, 6, ">>", 8);
		 }
		 break;
		 
		 //第二大选项三级界面    //任务界面的显示函数用一个函数封装起来， 以后调用不同任务用得上
		 case 120:
				OLED_ShowString(0, 0, "TASK 1 :", 16);
			 OLED_ShowString(20, 4, "", 16);              
			break;
				
			case 121 :
					OLED_ShowString(0, 0, "TASK 2 :", 16);
					OLED_ShowString(20, 4, "", 8);
			break;
			
			case 122:
					OLED_ShowString(0, 0, "TASK 3 :", 16);
					OLED_ShowString(20, 4, "", 16);
			break;
			
			case 123:
			  	OLED_ShowString(0, 0, "TASK ALL :", 16);
					OLED_ShowString(20, 4, "", 16);
		  break;
		
		//第三大选项
		case 3:
			OLED_ShowString(40, 0, "MENU", 16);   //记得与前面主菜单界面保持一直
		  OLED_ShowString(20, 3, "MPU6050", 8);
		  OLED_ShowString(20, 4, "TASK", 8);
		  OLED_ShowString(20, 5, "TEST", 8);
		  OLED_ShowString(0, 5, ">>", 8);
		  break;
		
		//第三大选项二级界面
		case 30:
		 {
			  OLED_ShowString(0, 0, "TEST", 16);
		    OLED_ShowString(20, 3, "motor", 8);
		    OLED_ShowString(20, 4, "ADC", 8);
		    OLED_ShowString(20, 5, "openmv", 8);
			  OLED_ShowString(20, 6, "BlueTooth", 8);
		    OLED_ShowString(0, 3, ">>", 8);
		 }
		 break;
		 
		 case 31:
		 {
			  OLED_ShowString(0, 0, "TEST", 16);
		    OLED_ShowString(20, 3, "motor", 8);
		    OLED_ShowString(20, 4, "ADC", 8);
		    OLED_ShowString(20, 5, "openmv", 8);
			  OLED_ShowString(20, 6, "BlueTooth", 8);
		    OLED_ShowString(0, 4, ">>", 8);
		 }
		 break;
		 
		 case 32:
		 {
			  OLED_ShowString(0, 0, "TEST", 16);
		    OLED_ShowString(20, 3, "motor", 8);
		    OLED_ShowString(20, 4, "ADC", 8);
		    OLED_ShowString(20, 5, "openmv", 8);
			  OLED_ShowString(20, 6, "BlueTooth", 8);
		    OLED_ShowString(0, 5, ">>", 8);
		 }
		 break;
		 
		 case 33:
		 {
			  OLED_ShowString(0, 0, "TEST", 16);
		    OLED_ShowString(20, 3, "motor", 8);
		    OLED_ShowString(20, 4, "ADC", 8);
		    OLED_ShowString(20, 5, "openmv", 8);
			  OLED_ShowString(20, 6, "BlueTooth", 8);
		    OLED_ShowString(0, 6, ">>", 8);
		 }
		 break;
		 
		 //第三大选项三级界面
		 case 130:
			 
		 /*********测试编码器是否正常**************/

		 
   		  OLED_ShowString(0, 0, "test 1:", 16);   //在test 1 里面调试获取编码器的数值   //下面第二个表示每秒转动的圈数
		    sprintf((char *)str_buff1, "M_EncNum: %5d", g_nMotorPulse);            //这个数值取决与调用GetMotorPulse的周期  （50ms）， 下面的也要对应1000/25=40,下面要乘上40
				sprintf((char *)str_buff2, "rps:      %2.2f", g_nMotorPulse*(1000/SPEED_PID_PERIOD)*0.25/56/11);    //两相  上升沿下降沿都捕获 减速比1：56   单项编码器脉冲数11  
				sprintf((char *)str_buff3, "M2_EncNum:%5d", g_nMotor2Pulse);  
				sprintf((char *)str_buff4, "rps:      %2.2f", g_nMotor2Pulse*(1000/SPEED_PID_PERIOD)*0.25/56/11);
		 
				OLED_ShowString(0, 3,str_buff1, 8);
				OLED_ShowString(0, 4,str_buff2, 8);
				OLED_ShowString(0, 5,str_buff3, 8);
		    OLED_ShowString(0, 6,str_buff4, 8);		

			break;
		 
		 case 131:
				
// 		  OLED_ShowString(0, 0, "test 2:", 16);   //在test 1 里面调试获取编码器的数值   //下面第二个表示每秒转动的圈数
//		  sprintf((char *)str_buff1, "ADC_Volt: %2.2f", ADC_Volt );         
//    	sprintf((char *)str_buff3, "Load_flag:%d", Load_flag);  

//			OLED_ShowString(0, 3,str_buff1, 8);
//      OLED_ShowString(0, 6,str_buff3, 8);
		   
       sprintf((char *)str_buff1, "L2_Val: %d", L2_Val);   //为什么第一位总是0？
		   sprintf((char *)str_buff2, "L1_Val: %d", L1_Val);    //看成33是因为我将它用sprintf转换为了十进制的数
		   sprintf((char *)str_buff3, "M_Val: %d",  M_Val); 
		   sprintf((char *)str_buff4, "R1_Val: %d", R1_Val);
		   sprintf((char *)str_buff5, "R2_Val: %d", R2_Val);
		 
			  OLED_ShowString(0, 0, "GraySensor:", 16);
		    OLED_ShowString(0, 2,str_buff1, 8);
				OLED_ShowString(0, 3,str_buff2, 8);
				OLED_ShowString(0, 4,str_buff3, 8);
		    OLED_ShowString(0, 5,str_buff4, 8);
        OLED_ShowString(0, 6,str_buff5, 8);		 
		 
		 break;
		 
		 case 132:
				sprintf((char *)str_buff1, "TargetRoom: %c", TargetRoom);
				sprintf((char *)str_buff2, "OneTargetRoom: %c",OneTargetRoom );
				sprintf((char *)str_buff3, "LoR: %d", LoR); 
				sprintf((char *)str_buff4, "Num: %d", Num);
				sprintf((char *)str_buff5, "FindTask: %d", FindTask);
				sprintf((char *)str_buff6, "Load_flag:%d", Load_flag);
          		 
		    OLED_ShowString(0, 0, "openmv:", 16);
				OLED_ShowString(0, 2,str_buff1, 8);
				OLED_ShowString(0, 3,str_buff2, 8);
				OLED_ShowString(0, 4,str_buff3, 8);
		    OLED_ShowString(0, 5,str_buff4, 8);
        OLED_ShowString(0, 6,str_buff5, 8);	
				OLED_ShowString(0, 7,str_buff6, 8);		

		 
			break;
		 
		 case 133:
				sprintf((char *)str_buff1, "%d", BlueRxBuffer[0]);   //为什么第一位总是0？
				sprintf((char *)str_buff2, "%d", BlueRxBuffer[1]);    //看成33是因为我将它用sprintf转换为了十进制的数
				sprintf((char *)str_buff3, "%d", BlueRxBuffer[2]); 
				sprintf((char *)str_buff4, "%d", BlueRxBuffer[3]);
				sprintf((char *)str_buff5, "%d", BlueRxBuffer[4]);
		 
			  OLED_ShowString(0, 0, "BlueTooth:", 16);
		    OLED_ShowString(0, 2,str_buff1, 8);
				OLED_ShowString(0, 3,str_buff2, 8);
				OLED_ShowString(0, 4,str_buff3, 8);
		    OLED_ShowString(0, 5,str_buff4, 8);
        OLED_ShowString(0, 6,str_buff5, 8);
		   
			break;
			
		}
}


void MENU_Item_KEY(void)              //按键按下才给调参数，所以代码逻辑要放这里面
{
			switch(g_nButton)
			{
				
				//选择键，调参键 。 加数，当到了最后一个选项后返回第一个选项，或者在最后一级界面去调参
				case KEY1_PRES:     
			   	OLED_Clear();
				  Menu_Item++;
				
				     //下面是控制Menu_Item的switch
			    	switch(Menu_Item) 
					{
						case 4: 
							Menu_Item  = 1; 
						break;
						
						//第一大选一级界面
						case 13:
							Menu_Item = 10;
						break;
						
						   // 第一大选项二级界面
					  case 111:
						case 112:
						case 113:   //最大到112，不可切换
						 Menu_Item--;
						break;	
						
						
							//第二大选一级界面
						case 24:
							Menu_Item = 20;
						break;
						
						// 第二大选项二级界面
						case 121:
						case 122:
						case 123: 
						case 124:							//最大到124 ，不可切换
						 Menu_Item--;
						break;
						
						//第三大选一级界面
						case 34:
							Menu_Item = 30;
						break;
						
						// 第三大选项二级界面
						case 131:
						case 132:
						case 133:
						case 134:    //最大到133，不可切换
						 Menu_Item--;
						break;
					}	
 					
				  break;
				
				case KEY2_PRES:     //确定键
						OLED_Clear();
					switch(Menu_Item)
					{
						case 1: 
						case 2:
					  case 3:
							Menu_Item *= 10;   //这里以乘10操作来实现确认的效果
						break;
						
						//下面几个虽然可以都写到一起 ，但这样好像更明了，容易改
						case 10:      //第一大选项
						case 11:	
						case 12:
							Menu_Item  += 100;   //这里以进百位操作来实现确认的效果
						break;
						
						case 20:      //第二大选项
						case 21:	
						case 22:
						case 23:
							Menu_Item  += 100;   //这里以进百位操作来实现确认的效果
						break;
						
						case 30:      //第三大选项
						case 31:	
						case 32:
						case 33:
							Menu_Item  += 100;   //这里以进百位操作来实现确认的效果
						break;
					}					
				break;
				
					
					//返回主菜单或者状态清零
				case KEY3_PRES :              //home键 
					 Menu_Item = 0;    
//				  PWM_update(0,0,0,0);      //所有状态恢复默认
				
				 break;
			}
}
