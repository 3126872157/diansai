#include "stdio.h"
#include "usart.h"
#include "cmsis_os.h"

uint8_t val1 = 0,val2 = 0,val3 = 0,val4 = 0,val5 = 0,val6 = 0;
uint8_t revbuf[1];

//使用串口函数printf
#ifdef __GNUC__

  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)

#else

  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)

#endif /* __GNUC__ */
	
//串口1中断，与串口屏进行通信
int fputc(int ch,FILE *f)
{
	HAL_UART_Transmit(&huart1,(uint8_t *)&ch,1,0xFFFF);
	return ch;
}


void HMI_send_number(char* name, int num)
{
    // printf("t0.txt=\"%d\"\xff\xff\xff", num);
    printf("%s=%d\xff\xff\xff", name, num);
}

void HMI_send_string(char* name, char* showdata)
{
    // printf("t0.txt=\"%d\"\xff\xff\xff", num);
    printf("%s=\"%s\"\xff\xff\xff", name, showdata);
}

void HMI_Task(void const * argument)
{
		while(1)
		{
				HMI_send_number("n0.val", val1);
			  HMI_send_number("n1.val", val2);
			  HMI_send_number("n2.val", val3);
			  HMI_send_number("n3.val", val4);
			  HMI_send_number("n4.val", val5);
			  HMI_send_number("n5.val", val6);
			  osDelay(1);
		}	
}
