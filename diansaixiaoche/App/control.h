#ifndef _CONTROL_H_
#define _CONTROL_H_

#define MOTOR_SPEED_RERATIO 20u    							//������ٱ�
#define PULSE_PRE_ROUND 13 									//����
#define RADIUS_OF_TYRE 4.80f 								//��̥�뾶����λ����
#define LINE_SPEED_C RADIUS_OF_TYRE * 2 * 3.142				//һȦ�ߵ�·��
#define GAP_FREQ 100										//gap��ʱ��Ƶ��
#define ENCODER_MODE 4										//����������ģʽ������Ƶ��

void control_init(void);

#endif
