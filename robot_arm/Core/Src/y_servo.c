/****************************************************************************
 *	@笔者	：	Q
 *	@日期	：	2023年2月8日
 *	@所属	：	杭州友辉科技
 *	@功能	：	存放舵机相关的函数
 *	@函数列表:
 *	1.	void servo_init(void) -- 舵机gpio初始化
 *	2.	void servo_pin_set(u8 index, BitAction level) -- 设置舵机引脚电平函数
 *	3.	void duoji_doing_set(u8 index, int aim, int time) -- 设置舵机控制参数函数
 ****************************************************************************/
#include "y_servo.h"
#include "y_global.h"
#include "tim.h"

servo_t duoji_doing[DJ_NUM];
extern float offset0;
extern float offset1;
extern float offset2;
extern float offset3;
/* 舵机gpio初始化 */
void servo_init(void)
{
    uint8_t i;

    for (i = 0; i < DJ_NUM; i++)
    {
        duoji_doing[i].aim = 1500;
        duoji_doing[i].cur = 1500;
        duoji_doing[i].inc = 0;
        duoji_doing[i].time = 5000;
    }
		__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_1,duoji_doing[0].cur + offset0);
		__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_2,duoji_doing[1].cur + offset1);
		__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_3,duoji_doing[2].cur + offset2);
		__HAL_TIM_SET_COMPARE(&htim1,TIM_CHANNEL_4,duoji_doing[3].cur + offset3);
}

/***********************************************
    功能介绍：	设置舵机控制参数函数
    函数参数：	index 舵机编号 aim 执行目标 time 执行时间(如果aim 执行目标==0，视为舵机停止)
    返回值：		无
 ***********************************************/
void duoji_doing_set(uint8_t index, int aim, int time)
{
    /* 限制输入值大小 */
    if (index >= DJ_NUM)
        return;

    if (aim == 0)
    {
        duoji_doing[index].inc = 0;
        duoji_doing[index].aim = duoji_doing[index].cur;
        return;
    }

    if (aim > 2490)
        aim = 2490;
    else if (aim < 510)
        aim = 510;

    if (time > 10000)
        time = 10000;

    if (duoji_doing[index].cur == aim)
    {
        aim = aim + 0.0077;
    }

    if (time < 20) /* 执行时间太短，舵机直接以最快速度运动 */
    {
        duoji_doing[index].aim = aim;
        duoji_doing[index].cur = aim;
        duoji_doing[index].inc = 0;
    }
    else
    {
        duoji_doing[index].aim = aim;
        duoji_doing[index].time = time;
        duoji_doing[index].inc = (duoji_doing[index].aim - duoji_doing[index].cur) / (duoji_doing[index].time / 20.000);
    }
}

/* 设置舵机每次增加的偏移量 */
void servo_inc_offset(uint8_t index)
{
    int aim_temp;

    if (duoji_doing[index].inc != 0)
    {

        aim_temp = duoji_doing[index].aim;

        if (aim_temp > 2490)
        {
            aim_temp = 2490;
        }
        else if (aim_temp < 500)
        {
            aim_temp = 500;
        }

        if (abs_float(aim_temp - duoji_doing[index].cur) <= abs_float(duoji_doing[index].inc + duoji_doing[index].inc))
        {
            duoji_doing[index].cur = aim_temp;
            duoji_doing[index].inc = 0;
        }
        else
        {
            duoji_doing[index].cur += duoji_doing[index].inc;
        }
    }
}
