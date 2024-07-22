#ifndef _CONTROL_H_
#define _CONTROL_H_

#define MOTOR_SPEED_RERATIO 20u    							//电机减速比
#define PULSE_PRE_ROUND 500 								//一圈多少个脉冲
#define RADIUS_OF_TYRE 34 									//轮胎半径，单位毫米
#define LINE_SPEED_C RADIUS_OF_TYRE * 2 * 3.14
#define GAP_FREQ 100										//gap定时器频率
#define ENCODER_MODE 2										//编码器计数模式，即倍频数

#endif
