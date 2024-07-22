#ifndef __BSP_BLUETOOTH_H
#define __BSP_BLUETOOTH_H

#include "bsp_sys.h"



#define RXBUFFERSIZE 256

extern u8 uart2_rxbuff;

extern u16 BlueRxBuffer[7];

extern u8 OneTargetRoom, OneLoadFlag, OneArriveFlag;

void BlueTooth_Receive_Data(u8 com_data);


#endif
