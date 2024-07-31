#ifndef __HMI_H_
#define __HMI_H_

void HMI_send_number(char* name, int num);
void HMI_send_string(char* name, char* showdata);
void HMI_Task(void const * argument);

#endif
