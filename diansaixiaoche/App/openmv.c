#include "openmv.h"
uint8_t rx_buffer[1];
uint8_t openmv_receive[2];

int cnt;

void openmv_init(void){
	HAL_UART_Receive_IT(&huart1,(uint8_t*)rx_buffer, 1);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if(huart->Instance==USART1){
    switch(cnt){
      case 0:
        if(rx_buffer[0] == 0x00){
          cnt++;
          break;
        }
        else{
          cnt=0;
          break;
        }
      case 1:
        if(rx_buffer[0] == 0x00){
          cnt++;
          break;
        }
        else{
          cnt=0;
          break;
        }      
      case 2:
        if(rx_buffer[0] == 0x79){
          cnt++;
          break;
        }
        else{
          cnt=0;
          break;
        }      
      case 3:
        if(rx_buffer[0] == 0x80){
          cnt++;
          break;
        }
        else{
          cnt=0;
          break;
        }
      case 4:
        openmv_receive[DELTA_X] = rx_buffer[0];
        cnt++;
        break;
      case 5:
        openmv_receive[DELTA_Y] = rx_buffer[0];
        cnt=0;
        break;
      default:
        cnt=0;
        break;
    }

    HAL_UART_Receive_IT(&huart1,(uint8_t*)rx_buffer, 1);
 }
}
