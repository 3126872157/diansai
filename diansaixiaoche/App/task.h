#ifndef _TASK_H_
#define _TASK_H_

#include "main.h"

enum chassis_behaviour	//µ×ÅÌÐÐÎª
{
	chassis_speed,
	chassis_distance,
	chassis_stop
};

enum car_mode
{
	go_distance,
	turn_left,
	turn_right,
	turn_around,
	follow_line = 4,
	avoid_obstacle,
	test1,
	stop = 7
};

typedef struct _Car
{
	uint8_t mode;
	uint8_t chassis;
	
}Car;

void car_chassis_set(Car *car_chassis_set);
void task_set_control(Car *car_set_control);

#endif
