/****************************************************************************
 *	@笔者	：	Q
 *	@日期	：	2023年2月8日
 *	@所属	：	杭州友辉科技
 *	@功能	：	存放舵机相关的函数
 ****************************************************************************/
#ifndef _Y_SERVO_H_
#define _Y_SERVO_H_
#include "main.h"

#define DJ_NUM 8 /* 舵机数量，为8是因为定时器中断计算pwm周期需要 */

typedef struct
{
    // uint8_t valid; // 有效 TODO
    uint16_t aim;  // 执行目标
    uint16_t time; // 执行时间
    float cur;     // 当前值
    float inc;     // 增量
} servo_t;

extern servo_t duoji_doing[DJ_NUM];

/*******LED相关函数声明*******/
void servo_init(void);                            /* 舵机引脚初始化 */
void duoji_doing_set(uint8_t index, int aim, int time); /* 设置舵机参数 */
void servo_inc_offset(uint8_t index);                  /* 设置舵机每次增加的偏移量 */
#endif
