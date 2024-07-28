#include "task.h"
#include "encoder.h"
#include "pid.h"
#include "motor.h"
#include "control.h"
#include "openmv.h"

float target_distance;
float target_speed1 = 0;
float target_speed2 = 0;
float motor1_in;
float motor2_in;

Car car;

extern Motor motor1;
extern Motor motor2;
extern float distance_out1;
extern float distance_out2;
extern float motor1_out;
extern float motor2_out;
extern float follow_line_turn_out;
extern float follow_line_target;
extern float max_speed_error;	
extern float slow_start_k;	
extern pid_type_def speed_pid;
extern pid_type_def distance_pid;
extern pid_type_def follow_line_pid;

//测试用
float distance_set;
float speed_set;
float turn90_set = 21.5;

void task_init(void)
{
	car.chassis = chassis_stop;
	car.mode = stop;
}

void car_chassis_set(Car *car_chassis_set)
{
	switch (car_chassis_set->mode)
	{
		case 0:
		case 1:
		case 2:
		case 3:
			car_chassis_set->chassis = chassis_distance;
			break;
		case 4:
		case 5:
		case 6:
			car_chassis_set->chassis = chassis_speed;
			break;
		case 7:
			car_chassis_set->chassis = chassis_stop;
		default:
			break;
	}
}

void task_set_control(Car *car_set_control)
{
	//根据车的模式，设置控制的量
	if(car_set_control->mode == go_distance)
	{
		distance_out1 += distance_set;
		distance_out2 += distance_set;
	}
	
	
	//根据底盘行为选择是速度环，还是串级环，还是停止
	if(car_set_control->chassis == chassis_stop)
	{
		motor1_out = 0;
		motor2_out = 0;
	}
	else if(car_set_control->chassis == chassis_speed)
	{
		//速度环
		target_speed1 = speed_set;
		target_speed2 = speed_set;
		
		motor1_out = PID_calc(&speed_pid, motor1.speed, target_speed1);
		motor2_out = PID_calc(&speed_pid, motor2.speed, target_speed2);
	}
	else if(car_set_control->chassis == chassis_distance)
	{
		
		if(car_set_control->mode == turn_left)
		{
			distance_out1 -= turn90_set; 
			distance_out2 += turn90_set;
		}
		else if(car_set_control->mode == turn_right)
		{
			distance_out1 -= -turn90_set;
			distance_out2 += -turn90_set;
		}
		else if(car_set_control->mode == turn_around)
		{
			distance_out1 -= turn90_set * 2; 
			distance_out2 += turn90_set * 2;
			
		}
		
		//串级速度位置pid环
		distance_out1 = PID_calc(&distance_pid, motor1.distance, distance_out1);
		distance_out2 = PID_calc(&distance_pid, motor2.distance, distance_out2);
		
		if(car_set_control->mode ==  go_distance)
		{
			//缓起
			if(distance_out1 - motor1.speed > max_speed_error)
			{
				distance_out1 = motor1.speed + (distance_out1 - motor1.speed) * slow_start_k;
			}
			if(distance_out2 - motor2.speed > max_speed_error)
			{
				distance_out2 = motor2.speed + (distance_out2 - motor2.speed) * slow_start_k;
			}
		}
		
		motor1_out = PID_calc(&speed_pid, motor1.speed, distance_out1);
		motor2_out = PID_calc(&speed_pid, motor2.speed, distance_out2);
	}
	
	//根据车的模式加上差速转向的量
	if(car_set_control->mode == follow_line)
	{
		//巡线环pid环
		follow_line_turn_out = PID_calc(&follow_line_pid, openmv_receive[DELTA_X], follow_line_target);
		motor1_out = motor1_out - follow_line_turn_out;
		motor2_out = motor1_out + follow_line_turn_out;
	}
	
}
