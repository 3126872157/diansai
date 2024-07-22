/*视频教程链接 https://www.bilibili.com/video/BV1UL411V7XK?p=2&share_source=copy_web   昵称：海喂喂喂*/
#include "bsp_GraySensor.h"
#include "main.h"
#include "adc.h"
#include "control.h"

//用来巡线的 '灰度传感器' 和 用来检测是否放上药品的 '红外传感器' 的代码都放在这个.c文件里了

u8 L2_Val, L1_Val, M_Val, R1_Val, R2_Val;
int Line_Num;


u16 ADC_Value;
float ADC_Volt;
u8 Load_flag =0;  // 0表示还没转载药品，表示转载完毕，-1表示药品送

u8 LoadCount,NotLoadCount;

/*******************灰度传感器巡线*************/
//只要把速度环控制好，补偿值即可确定。  不同速度值需要不同的补偿值。  测试好一个最好的。  不同的转速要对应不同的补偿系数或数值
void Light_GoStraight_control(void)   //灰度巡线直行, 需要有个判断需要直行多长距离的函数    //补偿要到中间的传感器测到回到线上为止
{
	  Get_Light_TTL();  
	
	//如果要更加精准的控制，只要有了巡线环，都需要对位置进行补偿。  可否直接将 g_lMotorPulseSigma  和g_lMotor2PulseSigma 进行数值加减？？
  
	//如果寻直线用的传感器可以读取模拟量的话，三个传感器都可以巡直线寻挺稳的


		if(L1_Val == 0 &&  M_Val == 1 && R1_Val == 0 )  Line_Num =  0;
	  if(L1_Val == 0 &&  M_Val == 1 && R1_Val == 1) Line_Num =  400;
	  if(L1_Val == 0 &&  M_Val == 0 && R1_Val == 1 )  Line_Num =  500;
	  if(L1_Val == 1 &&  M_Val == 0 && R1_Val == 0) Line_Num = -500;   
    if(L1_Val == 1 &&  M_Val == 1 && R1_Val == 0) Line_Num = -400;	
	
//	 if(LineNumToggleFlag == 1)
//	 {
//		 Line_Num = - Line_Num;
//	 }
	
}


//把灰度传感器当作只输出高低电平。 高电平是识别到红线了。
void Get_Light_TTL(void)  
{
	if(HAL_GPIO_ReadPin(L2_GPIO_Port, L2_Pin) == GPIO_PIN_SET){L2_Val = 1;} else {L2_Val = 0;}
	if(HAL_GPIO_ReadPin(L1_GPIO_Port, L1_Pin) == GPIO_PIN_SET){L1_Val = 1;} else {L1_Val = 0;}
	if(HAL_GPIO_ReadPin(M_GPIO_Port, M_Pin) == GPIO_PIN_SET)  {M_Val = 1;} else {M_Val = 0;}
	if(HAL_GPIO_ReadPin(R1_GPIO_Port, R1_Pin) == GPIO_PIN_SET){R1_Val = 1;} else {R1_Val = 0;}
	if(HAL_GPIO_ReadPin(R2_GPIO_Port, R2_Pin) == GPIO_PIN_SET){R2_Val = 1;} else {R2_Val = 0;}
}



/********检测是否转载药品*************/
void LoadOrNot()
{
			HAL_ADC_Start(&hadc1);      
			if(HAL_ADC_PollForConversion(&hadc1, 10) == HAL_OK)
			{
				ADC_Value = HAL_ADC_GetValue(&hadc1);
				ADC_Volt =  (float)ADC_Value*3.3/4096.0;
			}
			
			if(ADC_Volt <= 1.6)
			{
				NotLoadCount = 0;
				
				LoadCount++;
				if(LoadCount > 3) //3次程序执行到这之后再次判断
				{
					Load_flag = 1;
				}
			}
			else if(ADC_Volt >  1.6)
			{
				LoadCount = 0;
				
				if(Load_flag == 1)    //必须先装载过药品的情况下才能判断是否拿走药品
				{
					NotLoadCount++;
					if(NotLoadCount > 3 )  //3次程序执行到这之后再次判断
					{
						Load_flag = 2;
					}
			  }
			}
}
