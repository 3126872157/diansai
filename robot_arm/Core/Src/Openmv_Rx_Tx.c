#include "Openmv_Rx_Tx.h"
#include "usart.h"
#include <stdio.h>

//Openmv发给stm的数据帧格式(mode == 1)：
//数据帧内容解释：帧头1，帧头2，收到的Openmv工作模式，返回每个格子棋子的状态，帧尾1，帧尾2。
//{ 0x43, 0x4B, mode, state1, state2, ... , state9, 0x59, 0x46 }//一帧完整的数据帧总共有14个数据。

//Openmv发给stm的数据帧格式(mode == 2)：
//数据帧内容解释：帧头1，帧头2，收到的Openmv工作模式，返回每个格子的坐标(x, y)，帧尾1，帧尾2。
//{ 0x43, 0x4B, mode, x1, y1, x2, y2, ... , x9, y9, 0x59, 0x46 }//一帧完整的数据帧总共有23个数据。

//stm发给Openmv的数据帧格式：{ 0x43, 0x4B, mode, 0x59, 0x46 }
//数据帧内容解释：帧头1，帧头2，指定Openmv的工作模式，帧尾1，帧尾2。
//帧头，帧尾用ASCII码翻译为：   C     K           Y     F

//改变以下变量以修改openmv串口接收或发送模式。
uint8_t Rx_Openmv_mode = 2;
uint8_t Tx_Openmv_mode = 0;

uint8_t Buffer_Head[2] = { 0x43, 0x4B };
uint8_t Buffer_End[2]  = { 0x59, 0x46 };

uint8_t USART2_Rxbuffer = 0;
uint8_t USART2_Txbuffer[5] = { 0x43, 0x4B, 0x99, 0x59, 0x46 };

int8_t Chess_State[9];//格子中棋子的状态(无棋子/黑棋子/白棋子)。
uint8_t Chess_Location[2][9] = {0};//格子的坐标(x, y)。

uint8_t count_mode1 = 0;
uint8_t count_mode2 = 0;
uint8_t arr_index = 0;
uint8_t Rxflag = 0;

extern uint8_t revbuf[1];
extern uint8_t val1,val2,val3,val4,val5,val6;


void Receive_Data_from_Openmv(uint8_t Rx_Openmv_mode, uint8_t USART2_Rxbuffer)
{
	if (Rx_Openmv_mode == 1)
	{
		switch(count_mode1)
		{
			case 0:
			{
				if (USART2_Rxbuffer == Buffer_Head[0])	count_mode1++;
				break;
			}
			case 1:
			{
				if (USART2_Rxbuffer == Buffer_Head[1])	count_mode1++;
				break;
			}	
			case 2:
			{
				if (USART2_Rxbuffer == Rx_Openmv_mode)	count_mode1++;
				break;
			}		
			case 3:
			{
				Chess_State[arr_index++] = USART2_Rxbuffer;
				if (arr_index >= 9 && USART2_Rxbuffer == Buffer_End[0])	count_mode1++;
				break;
			}		
			case 4:
			{
				if (USART2_Rxbuffer == Buffer_End[1])		{ count_mode1++; arr_index = 0; Rxflag = 1; }
				break;
			}			
			case 5:
				break;
			default :
			{
				for (int k = 0; k < 18; k++)	Chess_State[k] = 0x99;		
			}
		}
	}

	if (Rx_Openmv_mode == 2)
	{
		switch(count_mode2)
		{
			case 0:
			{
				if (USART2_Rxbuffer == Buffer_Head[0])	count_mode2++;
				break;
			}
			case 1:
			{
				if (USART2_Rxbuffer == Buffer_Head[1])	count_mode2++;
				break;
			}	
			case 2:
			{
				if (USART2_Rxbuffer == Rx_Openmv_mode)	count_mode2++;
				break;
			}		
			case 3:
			{
				if (arr_index % 2 == 0) Chess_Location[0][arr_index++] = USART2_Rxbuffer;
				else 										Chess_Location[1][arr_index++] = USART2_Rxbuffer;
				
				if (arr_index >= 18 && USART2_Rxbuffer == Buffer_End[0])	count_mode2++;
				break;
			}		
			case 4:
			{
				if (USART2_Rxbuffer == Buffer_End[1])	{ count_mode2++; arr_index = 0; Rxflag = 1; }
				break;
			}			
			case 5:
				break;
			default :
			{
				for (int k = 0; k < 18; k++)	Chess_State[k] = 0x99;		
			}
		}
	}
}


//接收中断回调函数
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance== USART6)
	{
		if (Rx_Openmv_mode == 1)
		{
			Receive_Data_from_Openmv(Rx_Openmv_mode, USART2_Rxbuffer);
			//HAL_UART_Transmit_IT(&huart1,&USART2_Rxbuffer,1);
			HAL_UART_Receive_IT(&huart6,&USART2_Rxbuffer,1);
		}
		else if (Rx_Openmv_mode == 2)
		{
			Receive_Data_from_Openmv(Rx_Openmv_mode, USART2_Rxbuffer);
			//HAL_UART_Receive_IT(&huart6,&USART2_Rxbuffer,1);
			HAL_UART_Transmit_IT(&huart1,&USART2_Rxbuffer,1);
		}
	}
	if(huart ->Instance == USART1)
	{
		//HAL_UART_Transmit_IT(&huart1, revbuf, sizeof(revbuf));
		switch(revbuf[0])
		{
			case 0x31: val1++;break;
			case 0x32: val1--;break;
			case 0x33: val2++;break;
			case 0x34: val2--;break;
			case 0x35: val3++;break;
			case 0x36: val3--;break;
			case 0x37: val4++;break;
			case 0x38: val4--;break;
			case 0x39: val5++;break;
			case 0x40: val5--;break;
			case 0x41: val6++;break;
			case 0x42: val6--;break;
			
		}
		HAL_UART_Receive_IT(&huart1, revbuf, sizeof(revbuf));
	}
}


//向openmv发送工作模式
void Send_Data_To_Openmv(uint8_t Tx_Openmv_mode)
{
	USART2_Txbuffer[2] = Tx_Openmv_mode;
	int i = 0;
	for (i = 0; i < 5; i++)
	{
		HAL_UART_Transmit(&huart6, &USART2_Txbuffer[i], sizeof(USART2_Txbuffer[i]), 500);
	}
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance== USART6)
	{
		Send_Data_To_Openmv(Tx_Openmv_mode);
	}
}
