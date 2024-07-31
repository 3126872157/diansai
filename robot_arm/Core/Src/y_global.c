#include "y_global.h"
#include "y_kinematics.h"
#include "y_servo.h"

// int型 取绝对值函数
int abs_int(int int1)
{
    if (int1 > 0)
        return int1;
    return (-int1);
}

float abs_float(float value)
{
    if (value > 0)
    {
        return value;
    }
    return (-value);
}

// 两个int变量交换
void int_exchange(int *int1, int *int2)
{
    int int_temp;
    int_temp = *int1;
    *int1 = *int2;
    *int2 = int_temp;
}


int kinematics_move(float x, float y, float z, int time)
{
    int i, j, min = 0, flag = 0;

//    if (y < 0)
//        return 0;

//    // 寻找最佳角度
//    flag = 0;
//    for (i = 0; i >= -135; i--)
//    {
//        if (0 == kinematics_analysis(x, y, z, i, &kinematics))
//        {
//            if (i < min)
//                min = i;
//            flag = 1;
//        }
//    }

//    // 用3号舵机与水平最大的夹角作为最佳值
//    if (flag)
//    {
        kinematics_analysisa(x, y, z);
        for (j = 0; j < 4; j++)
        {
            set_servo(j, kinematics.servo_pwm[j], time);
        }
        return 1;
//    }

//    return 0;
}


void set_servo(int index, int pwm, int time)
{
    duoji_doing[index].aim = pwm;
    duoji_doing[index].time = time;
    duoji_doing[index].inc = (duoji_doing[index].aim - duoji_doing[index].cur) / (duoji_doing[index].time / 20.000);
}

