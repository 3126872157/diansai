#ifndef __CONTROL_H
#define __CONTROL_H

#include "math.h"
#include <stdio.h>
#include <stdlib.h>
#include "stm32f1xx.h"
#include "usart.h"
#include "tim.h"
#include "main.h"
#include  "bsp_sys.h"



/* 累计 TIM_Period个后产生一个更新或者中断*/		
/* 当定时器从0计数到PWM_PERIOD_COUNT，即为PWM_PERIOD_COUNT+1次，为一个定时周期 */
#define PWM_PERIOD_COUNT     (1000)     //可以尝试把这个变大，这样PID控制可以更顺滑
#define PWM2_PERIOD_COUNT     (1000)


/* 最大比较值 */
#define PWM_MAX_PERIOD_COUNT              (PWM_PERIOD_COUNT - 30)    //如果PWM弄成了满的，一些驱动板就会出现问题（硬件上的原因）
#define PWM2_MAX_PERIOD_COUNT              (PWM2_PERIOD_COUNT - 30)


/****************左边电机引脚初始化**************/
/* 设置速度（占空比） */
#define SET_FWD_COMPAER(ChannelPulse)     __HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_3,ChannelPulse)    // 设置比较寄存器的值   //AIN1    
#define SET_REV_COMPAER(ChannelPulse)     __HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_4,ChannelPulse)    // 设置比较寄存器的值   //AIN2   

/* 使能输出 */
#define MOTOR_FWD_ENABLE()      HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_3);  
#define MOTOR_REV_ENABLE()      HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_4);  

/* 禁用输出 */
#define MOTOR_FWD_DISABLE()     HAL_TIM_PWM_Stop(&htim4,TIM_CHANNEL_3);
#define MOTOR_REV_DISABLE()     HAL_TIM_PWM_Stop(&htim4,TIM_CHANNEL_4);



/****************右边电机引脚初始化**************/      //当两个轮子转向不一致的时候，要同时调这里的宏定义和encode.c编码器捕获脉冲的正负
/* 设置速度（占空比）2 */     
#define SET2_FWD_COMPAER(ChannelPulse)       __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_4,ChannelPulse)    // 设置比较寄存器的值   //BIN1
#define SET2_REV_COMPAER(ChannelPulse)       __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_3,ChannelPulse)    // 设置比较寄存器的值   //BIN2



/* 使能输出2 */
#define MOTOR2_FWD_ENABLE()                  HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_4);   
#define MOTOR2_REV_ENABLE()                  HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_3);    


/* 禁用输出2 */
#define MOTOR2_FWD_DISABLE()                 HAL_TIM_PWM_Stop(&htim2,TIM_CHANNEL_4);
#define MOTOR2_REV_DISABLE()                 HAL_TIM_PWM_Stop(&htim2,TIM_CHANNEL_3);     
    


/* 编码器物理分辨率 */
#define ENCODER_RESOLUTION                     11

/* 经过倍频之后的总分辨率 */
#define ENCODER_TOTAL_RESOLUTION             (ENCODER_RESOLUTION * 4)  /* 4倍频后的总分辨率 */

/* 减速电机减速比 */
#define REDUCTION_RATIO  56

/*在sysTick里调用PID计算的周期，以毫秒为单位*/
#define SPEED_PID_PERIOD  20    //这个要看定时器7的中断周期
#define TARGET_SPEED_MAX  110  //// 60rpm可以3s走完60cm

#define  LookCM 32  //openmv那里都是在车头距离红线中线31cm处去识别数字的

#define HideCM 30

#define LUN_JU 17.5   //单位cm
#define HeadToWheelCM 13
	

typedef enum
{
  left_90,
	right_90,
	back_180
}spin_dir_t;



/* 电机方向控制枚举 */
typedef enum
{
  MOTOR_FWD = 0,
  MOTOR_REV,
}motor_dir_t;

extern unsigned char g_ucMainEventCount;
extern unsigned char g_ucMainEventCountLong;
//extern unsigned char TASK_NUM;
extern uint8_t is_motor_en;

extern int32_t  actual_speed;

extern float g_fJourney;   //存放小车左右轮所走路程和 ， 单位cm, 需要及时清零
extern float g_fTargetJourney;  //存放小车左右轮所走路程和 ， 单位cm，需要在下一阶段任务中设置

extern u8 Line_flag , Turn_flag;
extern  u8  Spin_succeed_flag , Stop_Flag;

extern u8  WaitFlag;
extern u16 WaitTimeCount;

//extern u8 LineNumToggleFlag;


void set_motor_speed(uint16_t v);
void set_motor_speed(uint16_t v);
void set_motor_direction(motor_dir_t dir);
void set_motor_enable(void);
void set_motor_disable(void);
void set_motor2_speed(uint16_t v);
void set_motor2_direction(motor_dir_t dir);
void set_motor2_enable(void);
void set_motor2_disable(void);
void show_help(void);
void deal_serial_data(void);

float speed_pid_control(void);
float location_pid_control(void);
float speed2_pid_control(void);
float location2_pid_control(void);
void Location_Speed_control(void);



void Car_go(int32_t location_cm);

void spin_Turn(spin_dir_t zhuanxiang);
void slow_GoForward(void);

void MotorOutput(int nMotorPwm,int nMotor2Pwm);




#endif 

