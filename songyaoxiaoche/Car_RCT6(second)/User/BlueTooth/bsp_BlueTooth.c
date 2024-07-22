/*视频教程链接 https://www.bilibili.com/video/BV1UL411V7XK?p=2&share_source=copy_web   昵称：海喂喂喂*/
#include "bsp_BlueTooth.h"
#include <stdio.h>
#include "usart.h"
#include "string.h"
#include "stm32f1xx.h"

u8 uart2_rxbuff;

u8 OneTargetRoom, OneLoadFlag, OneArriveFlag;   //保存一车状态的标志位
u16 BlueRxBuffer[7]={0};

//要用协议保证通信可靠的话，可能也得用到串口回调函数
//一车（ZET6）控制蓝牙主机，二车（RCT6）控制蓝牙从机
void BlueTooth_Receive_Data(uint8_t com_data)
{
		uint8_t i;
		static uint8_t BlueRxCounter=0;//计数
	  
		static uint8_t BlueRxState = 0;	
		//static uint8_t BlueRxFlag = 0;    //蓝牙成功接收标志位
	 //蓝牙成功接收标志位其实可以不要。我OneTargertNum，OneLoadFlag直接采用上一次的也没啥，反正一般不会出现卡死的情况，很快就刷新数据了
	 //更没必要在外边将OneTargertNum，OneLoadFlag使用一次之后就与BlueRxFlag一同清零

		if(BlueRxState==0&&com_data==0x52)  //0x52帧头
		{
			
			BlueRxState=1;
			BlueRxBuffer[BlueRxCounter++]=com_data;  
		}

		else if(BlueRxState==1&&com_data==0x21)  //0x21帧头
		{
			BlueRxState=2;
			BlueRxBuffer[BlueRxCounter++]=com_data;
		}
		
		else if(BlueRxState==2)
		{
			 
			BlueRxBuffer[BlueRxCounter++]=com_data;
			if(BlueRxCounter>=7||com_data == 0xf2)    
			{
				BlueRxState=3;
				
				//BlueRxFlag1=1;    //成功接收一次数据，可以用来做判断是否采用最新数据 
				
//				OneTargertRoom =  BlueRxBuffer1[BlueRxCounter-5];     //[BlueRxCounter-5]是R  也即也即82，也即十六进制的52    正确的帧头1 
//				OneLoadFlag =  BlueRxBuffer1[BlueRxCounter-4];  //[BlueRxCounter-4]  里面是十进制的33，也即十六进制的21      正确的帧头2    
				
				OneTargetRoom =  BlueRxBuffer[BlueRxCounter-3];        
				OneLoadFlag =  BlueRxBuffer[BlueRxCounter-2];          
				
				
				//如果接收正常，则RxCounter1-1放的是帧尾

			}
		}

		else if(BlueRxState==3)		//检测是否接受到结束标志
		{
				//if(BlueRxBuffer[BlueRxState-1] == 0xf2)    //这里写错啦！！！！！不是BlueRxState1-1，而是BlueRxCounter1-1！！！！全部都自动清零啦
			
			  if(BlueRxBuffer[BlueRxCounter-1] == 0xf2)    
				{
							
							//BlueRxFlag = 0;
							BlueRxState = 0;
							BlueRxCounter = 0;
						
				}
				else   //接收错误
				{
							BlueRxState  = 0;
							BlueRxCounter =0;
							for(i=0;i<7;i++)
							{
									BlueRxBuffer[i]=0x00;      //将存放数据数组清零
							}
				}
		} 

		else   //接收异常
		{
				BlueRxState = 0;
				BlueRxCounter = 0;
				for(i=0;i<7;i++)
				{
						BlueRxBuffer[i]=0x00;      //将存放数据数组清零
				}
		}
}


