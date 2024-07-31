#ifndef _Y_GLOBAL_H_
#define _Y_GLOBAL_H_

#include "main.h"

#define DJ_NUM 8

extern uint8_t duoji_index1;

int abs_int(int int1);
void int_exchange(int *int1, int *int2);
float abs_float(float value);

int kinematics_move(float x, float y, float z, int time);
void set_servo(int index, int pwm, int time);

void car_set(int car_left1, int car_right1, int car_left2, int car_right2);
void car_set1(int car_left1, int car_right1, int car_left2, int car_right2);
#endif
