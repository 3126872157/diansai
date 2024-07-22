/*视频教程链接 https://www.bilibili.com/video/BV1UL411V7XK?p=2&share_source=copy_web   昵称：海喂喂喂*/
#include "bsp_key.h"
#include "menu.h"



int iButtonCount;
int iButtonFlag;
char g_nButton;     //声明外部变量，方便其他地方引用。当使用定时器消抖的时候用这个,这个记录了有效按下了什么键


u8 KEY=0;

void KEY_GPIO_Init(void)    //自己团队制作的PCB板上的另外两个按键用外部中断的方式弄吧
{
	GPIO_InitTypeDef KEY_Init;
	
	__HAL_RCC_GPIOC_CLK_ENABLE();
	
	KEY_Init.Mode = GPIO_MODE_INPUT;
	KEY_Init.Speed = GPIO_SPEED_FREQ_HIGH;
	KEY_Init.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 ;  //PE1暂时不进行初始化
	KEY_Init.Pull =  GPIO_PULLUP;
	HAL_GPIO_Init(GPIOC, &KEY_Init);
	
}

void ButtonScan(void)   //利用定时器给按键消抖，并且也放在定时器的更新中断函数里
{                       //判定按键按下是否有效
	switch(KEY_Scan())   
	{
		case(1):  //KEY1
			iButtonCount++;        //sysTick那5ms进行一次扫描，则这里5ms加一次，乘上2就是10ms
			if(iButtonCount >= 10)   //如果相应更新中断的定时器的更新周期为3ms，那么这里就是得保持至少30ms的按键按下的电平
			{
				if(iButtonFlag == 0)       //判断有没有重按，1为有，0为没有
				{
					iButtonCount = 0;
					iButtonFlag = 1;
					g_nButton = 1;
				}
				else iButtonCount = 0; //如果重按按键，则重新计数
			}
			else g_nButton = 0;  //如果没有稳定按下30ms，则表示没有按下按键
		break;
		
		case(2):  //KEY2
			iButtonCount++;
			if(iButtonCount >= 10)   
			{
				if(iButtonFlag == 0)      
				{
					iButtonCount = 0;
					iButtonFlag = 1;
					
					g_nButton = 2;
				}
				else iButtonCount = 0; 
			}
			else g_nButton = 0;
		break;
		
		case(3):  //K3
			iButtonCount++;
			if(iButtonCount >= 10)   
			{
				if(iButtonFlag == 0)     
				{
					iButtonCount = 0;
					iButtonFlag = 1;
					
					g_nButton = 3;
				}
				else iButtonCount = 0; 
			}
			else g_nButton = 0;  
		break;
	
		default:                   
			iButtonCount = 0;
			iButtonFlag = 0;
		   g_nButton = 0;
		break;
	}
	
}

uint8_t KEY_Scan(void)   //检测有无按键按下 
{
	if(KEY1==0)       return 1;      //如果是用if  else if  则越前面的优先级越高
  else if(KEY2==0)  return 2;
	else if(KEY3==0)  return 3;
	
	return 0;   //无按键按下
}


/**
  * 函数功能: 读取按键K1的状态
  * 输入参数：无
  * 返 回 值: KEY_DOWN：按键被按下；
  *           KEY_UP  ：按键没被按下
  * 说    明：无。
  */
KEYState_TypeDef K1_StateRead(void)
{
  /* 读取此时按键值并判断是否是被按下状态，如果是被按下状态进入函数内 */
  if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)==K1_DOWN_LEVEL)
  {
    /* 延时一小段时间，消除抖动 */
    HAL_Delay(10);
    /* 延时时间后再来判断按键状态，如果还是按下状态说明按键确实被按下 */
    if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)==K1_DOWN_LEVEL)
    {
      /* 等待按键弹开才退出按键扫描函数 */
      while(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)==K1_DOWN_LEVEL);      
       /* 按键扫描完毕，确定按键被按下，返回按键被按下状态 */
      return KEY0_DOWN;
    }
  }
  /* 按键没被按下，返回没被按下状态 */
  return KEY0_UP;
}

