#ifndef _FLOW_TASK_H_
#define _FLOW_TASK_H_

#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

#define HOME_X 0
#define HOME_Y 70
#define HOME_Z 160

#define TOUCH_Z 123	//磁铁吸棋子的高度

typedef struct{
	float x;
	float y;
}position;

void flow_Task(void const * argument);

#endif
