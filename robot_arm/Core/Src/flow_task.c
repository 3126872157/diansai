//�����ļ�

#include "y_global.h"
#include "flow_task.h"
#include "y_servo.h"
#include "tim.h"

extern uint8_t revbuf[1];

position chess_center[9];	//���̸�����������

uint8_t black_count = 0;	//�õ��ڼ��ź���
uint8_t white_count = 0;

uint8_t task_flag;
uint8_t last_revbuf = 0x99;
uint8_t chess_center_update = 0;

float test_x = 0;
float test_y = 160;
float test_z = 150;

position blackStack[5];		//��ź��������
position whiteStack[5];		//��Ű��������

void robot_arm_move(float x, float y, float z, int time)
{
	kinematics_move(x, y, z, time);
	osDelay(time + 300);
}

void flow_task_delay()
{

}

void magnet_on()	//��������������
{
	__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1, 18000);	//ͨ��pwm���ã�arrΪ20000
}

void magnet_off()	//�����أ�������
{
	__HAL_TIM_SET_COMPARE(&htim8, TIM_CHANNEL_1, 0);
}

void home()
{
	robot_arm_move(HOME_X, HOME_Y, HOME_Z, 1000);
}

void get_chess(position from)
{
	robot_arm_move(from.x, from.y, HOME_Z, 1000);	//�ƶ�
	robot_arm_move(from.x, from.y, TOUCH_Z, 1000);	//�½�
	magnet_on();
	osDelay(1500);
	robot_arm_move(from.x, from.y, HOME_Z, 1000);	//����
}

void drop_chess(position to)
{
	robot_arm_move(to.x, to.y, HOME_Z, 1000);	//�ƶ�
	robot_arm_move(to.x, to.y, TOUCH_Z, 1000);	//�½�
	magnet_off();
	osDelay(1500);
	robot_arm_move(to.x, to.y, HOME_Z, 1000);	//����
}


void put_chess(position from, position to)
{
	home();				//�ص���ʼλ��
	get_chess(from);	//������
	drop_chess(to);		//������
	home();				//�ص���ʼλ��
}

void flow_Task(void const * argument)
{
	while(1)
	{
//		servo_init();
//		robot_arm_move(-81, 55, 125, 1000);
//		magnet_on();
//		osDelay(2500);
//		robot_arm_move(-75, 155, 125, 1000);
//		magnet_off();
//		osDelay(2500);
//		magnet_on();
//		osDelay(2500);
		switch(revbuf[0])
		{
			case 0x10:	//����1
			{
					put_chess(blackStack[black_count], chess_center[5-1]);
					black_count ++;	//����һ������
					break;
			}
			case 0x11:	//����2
			{
					//black_count = 0;	//����1�ĺ���Ż�ȥ��
					uint8_t i = 0;
					while(i < 4)
					{
							if(revbuf[0] == 0x01 || 
								 revbuf[0] == 0x02 || 
								 revbuf[0] == 0x03 || 
								 revbuf[0] == 0x04 || 
								 revbuf[0] == 0x05 || 
								 revbuf[0] == 0x06 || 
								 revbuf[0] == 0x07 || 
								 revbuf[0] == 0x08 || 
								 revbuf[0] == 0x09)
							{
									if(i <= 1)
									{
											put_chess(blackStack[black_count], chess_center[revbuf[0] - 1]);
											black_count ++;
									}
									else
									{
											put_chess(whiteStack[white_count], chess_center[revbuf[0] - 1]);
											white_count ++;
									}
									i ++;
							}
							osDelay(1);
					}
					break;
			}
			case 0x12:
			{
					//��openMV����һ��chess_center,������
					while(chess_center_update != 1)	//�ȴ�openMV��������
					{
							osDelay(1);
					}
					//black_count = 0;
					//white	_count = 0;
					uint8_t i = 0;
					while(i < 4)
					{
							if(revbuf[0] == 0x01 || 
								 revbuf[0] == 0x02 || 
								 revbuf[0] == 0x03 || 
								 revbuf[0] == 0x04 || 
								 revbuf[0] == 0x05 || 
								 revbuf[0] == 0x06 || 
								 revbuf[0] == 0x07 || 
								 revbuf[0] == 0x08 || 
								 revbuf[0] == 0x09)
							{
									if(i <= 1)
									{
											put_chess(blackStack[black_count], chess_center[revbuf[0] - 1]);
											black_count ++;
									}
									else
									{
											put_chess(whiteStack[white_count], chess_center[revbuf[0] - 1]);
											white_count ++;
									}
									i ++;
									osDelay(1);
							}
					}
					break;
			}
			
		}
		osDelay(1);
	}
}
