#ifndef _OPENMV_H_
#define _OPENMV_H_


#include <stdio.h>

#include "stm32f1xx_hal.h"
#include "usart.h"
#include "struct_typedef.h"
#include "main.h"

enum {
    DELTA_X,
    DELTA_Y
};

extern uint8_t openmv_receive[2];

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void openmv_init(void);
#endif
