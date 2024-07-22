/*视频教程链接 https://www.bilibili.com/video/BV1UL411V7XK?p=2&share_source=copy_web   昵称：海喂喂喂*/
#include "control.h"
#include "usart.h"

#include "tim.h"
#include "bsp_pid.h"
#include "encoder.h"
#include "Fire_protocol.h"
#include "bsp_delay.h"
#include "bsp_led.h"
#include "adc.h"
#include "openmv.h"
#include "bsp_GraySensor.h"

unsigned char location_control_count = 0;  //执行频率不需要那么高的用这个事件计数，用在中断中

float  MotorPWM =0.0, Motor2PWM =0.0;


float speed_Outval, location_Outval;
float speed2_Outval, location2_Outval;
float Line_Outval;
int Turn_val;

static motor_dir_t direction  = MOTOR_FWD;     // 记录电机方向
static uint16_t    dutyfactor = 0;             // 记录电机占空比
static motor_dir_t direction2  = MOTOR_FWD;     // 记录电机2方向
static uint16_t    dutyfactor2 = 0;             // 记录电机2占空比
u8 is_motor_en = 0, is_motor2_en = 0;            // 电机使能

//u8 LineNumToggleFlag;

u8 Spin_start_flag , Spin_succeed_flag , Stop_Flag;
u8 Line_flag = 0, Turn_flag = 0 ;
u8 stop_count , spin_count;


u16 WaitTimeCount;
u8 WaitFlag , BackFlag;


float g_fTargetJourney = 0;  //存放小车左右轮所走路程和 ， 单位cm，需要在下一阶段任务中设置

float Motor_journey_cm, Motor2_journey_cm;


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)    //好像处理sysTick，其他定时器产生中断要调用回调函数时，都要进到这个函数里进行判断后在执行相应操作
{
	if(htim == (&htim7))  
	{

		GetMotorPulse();  
						
		//有了 巡线环之后，小车直线距离可能需要补偿，根据寻直线的效果来定。  3.14即可补偿？巡线效果不好（不直）的话可以继续减小这个pai的值
		Motor_journey_cm	  = (  g_lMotorPulseSigma /(56*11*4) ) * (6.5*3.14);
		Motor2_journey_cm	= (  g_lMotor2PulseSigma /(56*11*4) ) * (6.5*3.14);
		
    Light_GoStraight_control();    //一直获取灰度传感器状态，但只有在执行car_go的时候，才进行补偿		
		
		/*****上位机pid调试速度话用,实际运行时注释掉*****/
		
//		if(is_motor_en == 1)
//		{
//			MotorPWM = speed_pid_control();
//		}			
//		 if(is_motor2_en == 1) 
//		 {
//			 Motor2PWM  = speed2_pid_control();
//		 }
//		 MotorOutput(MotorPWM,Motor2PWM);
    
		/******/
		 
		 /******上位机调试位置速度串级PID时用，实际使用时注释掉******/
//		 
//		 if(is_motor_en == 1 && is_motor2_en == 1)
//		 {
//			 Location_Speed_control();
//			 MotorPWM = speed_Outval;  
//			 Motor2PWM = speed2_Outval;
//			 MotorOutput(MotorPWM,Motor2PWM);
//		 }
		 /************/
		
			if(Line_flag == 1) 
			{	
				
				 //来个判断，直行到达位置后停止PID控制，防止识别图片时或者等待装卸药时电机耗能，直走巡线结束的阶段才用这个， 
				 //其实这个要是用好一点的驱动模块的话，短时间内运行也不需要.....不用怕电机驱动过热...一直PID控制也没啥
				
				
				 //	调试位置速度串级PID时注释掉这个让电机失能的函数
				 if( (Motor_journey_cm <= (g_fTargetJourney + 25)) && (Motor_journey_cm >= (g_fTargetJourney - 25)) )  //这里偏差这么多真的好吗？并不是真的偏差
				 {
						stop_count++;   //stop_count不能超过256
						if(stop_count >=  100)  //100 * 20 = 1.6s  最少也要至少在目标位置停留1s  //可以时间判断放长点，以便刹车停稳
						{
							Line_flag = 0;
							Stop_Flag = 1; //这个标志位可以用来判断是否执行下一阶段任务
							stop_count = 0;
							
							//greenLED_on;    //实际运行时不需要这个
							
							set_motor_disable();
							set_motor2_disable();
							
						}
				 }
				 else
				 {
						Stop_Flag = 0;  
						stop_count = 0;  
						
						//greenLED_off;   //实际运行时不需要这个
			 	 }           
		
		
				if (is_motor_en == 1 || is_motor2_en == 1)     // 电机在使能状态下才进行控制处理
				{
						Location_Speed_control(); //位置环速度环串级PID的输出是速度环输出的PWM值
					 
						if(Line_Num == 0)  //每次回到线上需要补偿的时候，都将两个电机的累计脉冲数取平均值，这个也会有在转向后帮助回到线上的效果
						{
							long Pulse;
							
							Pulse = (g_lMotorPulseSigma + g_lMotor2PulseSigma) / 2;
							
							g_lMotorPulseSigma = Pulse;  //可能有时候这里加上个补偿会更好
							g_lMotor2PulseSigma = Pulse;
						}
								
								
						 //这个是灰度传感器的巡线补偿
						 Line_Outval =  Line_Num;     //lineNum得在PWM的重装载值一半左右才会有明显的效果
						 MotorPWM = speed_Outval  + Line_Outval ;  
						 Motor2PWM = speed2_Outval - Line_Outval ;
						
							
						 MotorOutput( MotorPWM, Motor2PWM);   
				 }
			 }			 
				 
			if( Spin_start_flag == 1)  
			{
				
					if (is_motor_en == 1 || is_motor2_en == 1)     // 电机在使能状态下才进行控制处理
					{	
						Location_Speed_control(); //位置环速度环串级PID的输出是速度环输出的PWM值

						
						MotorPWM = speed_Outval;
						Motor2PWM = speed2_Outval;
						
						spin_count++;
						if( spin_count >= 100)      //20ms进入一次   100*20 = 2s，以能过够完成倒转时间为下限，应该已经倒转完毕了
						{
							 Spin_start_flag = 0;
							 spin_count = 0;
							
							//转向有点问题，转完之后还一直转,暂时用下面两句解决
							MotorPWM =0;   
							Motor2PWM = 0;
												 
							Spin_succeed_flag = 1;  
							
							//Line_Num = 0.0;   //转向时要避免记住上一次的补偿值吗？会影响到转弯后的巡线！！！
							//在转向前有线的话，记住上一次的巡线补偿反而可以达到自回线的效果
							//可以转向时转少一些！！至少让转向那一侧的灰度传感器打到线上，就可以达到自回线上的效果！！！					
						}
					
						MotorOutput( MotorPWM, Motor2PWM);
					}					
			}
			
					
	}
	
	else if(htim == (&htim6))  //1s进入一次TIM6的中断   
	{
		 greenLED_Toggle;
//	   BlueSignal_Toggle;
	}
	
}




/****走完后自动将 Stop_flag置1****/

/*这里加将位置环的输入直接编程距离（cm）的 函数*/   //所有普通直行都用这个
void Car_go(int32_t location_cm)   //直走函数     //连续两次的直行好像会让巡线补偿失效
{
	float Car_location;
	
	Line_flag = 1;   
	Stop_Flag = 0;
	
	Spin_start_flag = 0;
	Spin_succeed_flag = 0;  
	
//	if(location_cm < 0)
//	{
//		LineNumToggleFlag = 1;
//	}
//	else LineNumToggleFlag = 0;
	
	
	Motor_journey_cm = 0;
	Motor2_journey_cm = 0;
	
	g_fTargetJourney = location_cm;   //防止长时间PID控制用
	
	//如果巡线效果不好就将3.142加大
	Car_location   = ( location_cm / (6.5 * 3.14) ) *   (56*4*11) ;    //Car_location 将  location_cm 转换为对应的脉冲数   g_fTargetJourney = 要转多少圈 * 1圈的脉冲数 
	//小车位置直接用一个电机的脉冲数累积就好，转向时不计数，开始一个位置前直接清零。   
	//打滑导致一个轮比另一个轮转位置不一样咋办,用“巡线环“弥补就好，转向就用“转向环”
	 
	
	  set_pid_target(&pid_location, Car_location);   
	  set_pid_target(&pid_location2, Car_location);
	
	  set_motor_enable();   //使能电机控制PWM输出    
    set_motor2_enable();   //使能电机2控制PWM输出
	 
	  g_lMotorPulseSigma = 0;    //之前没清除所以用不了
	  g_lMotor2PulseSigma = 0; 
	
		
}




/******转向完成时自动将  Spin_start_flag == 0 、 Spin_succeed_flag == 1  ********/

/*转角有左转90，右转90，和转180三种情况。*/


void spin_Turn(spin_dir_t zhuanxiang)   //传入小车的轮距(mm) 175mm  //其实转向环并不需要很精准，转弯后直接用直走时的巡线函数回正车身就好
{
	
	  Line_flag = 0;  //不进行巡线的补偿了
	  Stop_Flag = 0;   //执行转弯时，将直走完成的标志位清零. 即如果上一次是直行，这次是转弯，则不用在业务代码里手动置位
	  
	
    Spin_start_flag = 1;   
	  Spin_succeed_flag = 0; 
	
		float spin90_val;
		float Car_Turn_val;
		
		Motor_journey_cm = 0;
		Motor2_journey_cm = 0;
		
		spin90_val = 0.25*3.1416*LUN_JU;
		
	/****转弯后不循线可以调转向系数**********/
		if(zhuanxiang == left_90)  //openmv识别到需要往左边病房走
		{
			Car_Turn_val = ( spin90_val / (6.5 * 3.142) ) *   (56*4*11) ; 
			Car_Turn_val = 0.95* Car_Turn_val;   //90*0.94 = 84.6   //惯性影响，导致转弯比理论设定的多。直接设90度接下来的巡线就寻不回来了
			
		}
		else if(zhuanxiang == right_90)  //openmv识别到需要往右边病房走
		{
			Car_Turn_val = -(spin90_val / (6.5 * 3.142) ) *   (56*4*11); 
			Car_Turn_val = 0.98 * Car_Turn_val;  //90*0.96 = 86.4    //惯性影响，导致转弯比理论设定的多。 接下来的巡线可能就寻不回来了
		}
		else if(zhuanxiang == back_180)
		{
			Car_Turn_val = -(spin90_val / (6.5 * 3.142) ) *   (56*4*11);
			Car_Turn_val = 0.98 * Car_Turn_val;   //同理
			Car_Turn_val = 2*Car_Turn_val;      //0.96*180 = 175.5
		}
		
			set_pid_target(&pid_location, -Car_Turn_val);   
			set_pid_target(&pid_location2, Car_Turn_val);
		
			g_lMotorPulseSigma = 0;    //之前没清除所以用不了，哈哈哈哈哈
			g_lMotor2PulseSigma = 0; 
		
			set_motor_enable();   //使能电机控制PWM输出    
		  set_motor2_enable();   //使能电机2控制PWM输出

}


/************巡线环减速补偿函数**************/ 

//尝试改成用最外边两个灰度传感器只用来辅助识别十字路口，光中间那个检测到有线的时候表示已经回到线上。  

//往那边偏，速度环目标值补偿就加到哪边  


/*********************各PID**********************/
		/****速度环位置环串级PID控制*****/   


void Location_Speed_control(void)          //原地转向可以直接 调用这个
{
			//mpu_dmp_get_data放在这会影响到编码器脉冲的获取。
			 if (is_motor_en == 1 || is_motor2_en == 1)     // 电机在使能状态下才进行控制处理
			 {
					 location_control_count++;
				  if(location_control_count >= 2)
					{
						location_control_count = 0; 
						
						location_Outval = location_pid_control();
						location2_Outval = location2_pid_control();
					}
					
					set_pid_target(&pid_speed, location_Outval); //每次都必须有位置环的值     //调试速度环PID时取消这两句
					set_pid_target(&pid_speed2, location2_Outval); //每次都必须有位置环的值
				 
           speed_Outval = speed_pid_control();    //要是电机转向不符合预期，就在这两句里取反数值
					 speed2_Outval = speed2_pid_control();  
				}

}



float location_pid_control(void)  
{
	float cont_val = 0.0; 
	int32_t actual_location;
	
	
	  actual_location =  g_lMotorPulseSigma;   //1圈 = 2464个脉冲 = 56*11*4  //这里位置用圈数代替。

    cont_val = location_pid_realize(&pid_location, actual_location);   
	  
	  //还没加串级pID之前，位置环的cont_val对应PWM。 改成串级PID后，位置换的cont_val对应目标速度
	    
     	/* 目标速度上限处理 */
      if (cont_val > TARGET_SPEED_MAX)
      {
        cont_val = TARGET_SPEED_MAX;
      }
      else if (cont_val < -TARGET_SPEED_MAX)
      {
        cont_val = -TARGET_SPEED_MAX;
      }
	
	#if defined(PID_ASSISTANT_EN)
    set_computer_value(SEND_FACT_CMD, CURVES_CH1, &actual_location, 1);                // 给通道 1 发送实际值
    //BlueSignal_Toggle;  //可以进来
  #endif
//	
	return cont_val;
}


float speed_pid_control(void)  
{
   
    float cont_val = 0.0;                       // 当前控制值
    int32_t actual_speed;
	
	
	  actual_speed = ((float)g_nMotorPulse*1000.0*60.0)/(ENCODER_TOTAL_RESOLUTION*REDUCTION_RATIO*SPEED_PID_PERIOD);

	
    cont_val = speed_pid_realize(&pid_speed, actual_speed);    // 进行 PID 计算
  
//  #if defined(PID_ASSISTANT_EN)
//    set_computer_value(SEND_FACT_CMD, CURVES_CH1, &actual_speed, 1);                // 给通道 1 发送实际值
//  #endif
	
	return cont_val;
}


float location2_pid_control(void)  
{
	float cont_val = 0.0; 
	int32_t actual_location;
	
	  actual_location =  g_lMotor2PulseSigma;   //1圈 = 2464个脉冲 = 56*11*4  //这里位置用圈数代替。

    cont_val = location_pid_realize(&pid_location2, actual_location);   
	  
	  // 改成串级PID后，位置换的cont_val对应目标速度
	
	//目标速度限幅
	    	/* 目标速度上限处理 */
      if (cont_val > TARGET_SPEED_MAX)
      {
        cont_val = TARGET_SPEED_MAX;
      }
      else if (cont_val < -TARGET_SPEED_MAX)
      {
        cont_val = -TARGET_SPEED_MAX;
      }
	
	
//	  #if defined(PID_ASSISTANT_EN)
//    set_computer_value(SEND_FACT_CMD, CURVES_CH1, &actual_location, 1);                // 给通道 1 发送实际值
//		//set_computer_value(SEND_TARGET_CMD, CURVES_CH1,&TargetSpeed, 1);                // 给通道 1 发送目标值？这个是目标控制值，整定参数时不用发送，否则无法在上位机上设置
//  #endif
	
	return cont_val;
}


float speed2_pid_control(void)  
{
   
    float cont_val = 0.0;                       // 当前控制值
    int32_t actual_speed;    
	
	
	  actual_speed = ((float)g_nMotor2Pulse*1000.0*60.0)/(ENCODER_TOTAL_RESOLUTION*REDUCTION_RATIO*SPEED_PID_PERIOD);

	
    cont_val = speed_pid_realize(&pid_speed2, actual_speed);    // 进行 PID 计算
    
//  #if defined(PID_ASSISTANT_EN)
//    set_computer_value(SEND_FACT_CMD, CURVES_CH1, &actual_speed, 1);                // 给通道 1 发送实际值
//  #else
//    printf("实际值：%d. 目标值：%.0f\n", actual_speed, get_pid_target());      // 打印实际值和目标值
//  #endif
//	
	return cont_val;
}



/*****************电机的控制函数***************/

void MotorOutput(int nMotorPwm,int nMotor2Pwm)//设置电机电压和方向
{
		if (nMotorPwm >= 0)    // 判断电机方向         
		{
			set_motor_direction(MOTOR_FWD);   //正方向要对应
		}
		else
		{
			nMotorPwm = -nMotorPwm;    
			set_motor_direction(MOTOR_REV);   //正方向要对应
		}
		nMotorPwm = (nMotorPwm > PWM_MAX_PERIOD_COUNT) ? PWM_MAX_PERIOD_COUNT : nMotorPwm;    // 速度上限处理
		
		
		if (nMotor2Pwm >= 0)    // 判断电机方向         
		{
			set_motor2_direction(MOTOR_FWD);   //正方向要对应
		}
		else
		{
			nMotor2Pwm = -nMotor2Pwm;    
			set_motor2_direction(MOTOR_REV);   //正方向要对应
		}
		
		nMotor2Pwm = (nMotor2Pwm > PWM_MAX_PERIOD_COUNT) ? PWM_MAX_PERIOD_COUNT : nMotor2Pwm;    // 速度上限处理
		
		set_motor_speed(nMotorPwm);        // 设置 PWM 占空比
		set_motor2_speed(nMotor2Pwm);      // 设置 PWM 占空比
 }

 
/**
  * @brief  设置电机速度
  * @param  v: 速度（占空比）
  * @retval 无
  */
void set_motor_speed(uint16_t v)    //这种还是单极性控制。。。。。如果想要更大的力，就要改成双极性的来。
{
  dutyfactor = v;
  
  if (direction == MOTOR_FWD)
  {
    SET_FWD_COMPAER(dutyfactor);     // 设置速度
  }
  else
  {
    SET_REV_COMPAER(dutyfactor);     // 设置速度
  }
}

/**
  * @brief  设置电机方向
  * @param  无
  * @retval 无
  */
void set_motor_direction(motor_dir_t dir)    //这个要改为平衡车的前进和后退
{
  direction = dir;
  
  if (direction == MOTOR_FWD)
  {
		SET_FWD_COMPAER(dutyfactor);     // 设置速度
    SET_REV_COMPAER(0);              // 设置速度
  }
  else
  {
		SET_FWD_COMPAER(0);              // 设置速度
    SET_REV_COMPAER(dutyfactor);     // 设置速度
  }
}

/**
  * @brief  使能电机
  * @param  无
  * @retval 无
  */
void set_motor_enable(void)   //这俩个使能和禁用的函数对于双极性控制来说还有效吗？
{
	is_motor_en  = 1;
//  MOTOR_ENABLE_SD();
  MOTOR_FWD_ENABLE();
  MOTOR_REV_ENABLE();
}

/**
  * @brief  禁用电机
  * @param  无
  * @retval 无
  */
void set_motor_disable(void)
{
   is_motor_en  = 0;
//  MOTOR_DISABLE_SD();
  MOTOR_FWD_DISABLE();
  MOTOR_REV_DISABLE();
}


/*****************电机2的控制函数***************/
/**
  * @brief  设置电机2速度
  * @param  v: 速度（占空比）
  * @retval 无
  */
void set_motor2_speed(uint16_t v)
{
  dutyfactor2 = v;   
  
  if (direction2 == MOTOR_FWD)     
  {
    SET2_FWD_COMPAER(dutyfactor2);     // 设置速度
  }
  else
  {
    SET2_REV_COMPAER(dutyfactor2);     // 设置速度
  }
}

/**
  * @brief  设置电机2方向
  * @param  无
  * @retval 无
  */
void set_motor2_direction(motor_dir_t dir)  
{
  direction2 =  dir;    
  
  if (direction2 == MOTOR_FWD) 
  {
    SET2_FWD_COMPAER(dutyfactor2);     // 设置速度     
    SET2_REV_COMPAER(0);              // 设置速度
  }
  else
  {
    SET2_FWD_COMPAER(0);              // 设置速度
    SET2_REV_COMPAER(dutyfactor2);     // 设置速度
  }
}

/**
  * @brief  使能电机2
  * @param  无
  * @retval 无
  */
void set_motor2_enable(void)
{
	is_motor2_en  = 1;
//  MOTOR_ENABLE_B();
  MOTOR2_FWD_ENABLE();
  MOTOR2_REV_ENABLE();
}

/**
  * @brief  禁用电机2
  * @param  无
  * @retval 无
  */
void set_motor2_disable(void)
{
	is_motor2_en  = 0;
//  MOTOR_DISABLE_B();
  MOTOR2_FWD_DISABLE();
  MOTOR2_REV_DISABLE();
}


/************************重新移植有协议控制的上位机可查看波形并整定PID的数据处理函数***************/





