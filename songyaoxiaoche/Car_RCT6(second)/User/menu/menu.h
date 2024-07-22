#ifndef __MENU_H
#define __MENU_H


#include "bsp_sys.h"

extern u8 Menu_Item;



void OLED_Display(u8 num);

void OLED_MPU_Accelerometer(void);
void OLED_MPU_Gyroscope(void);
void OLED_MPU_EnlerAngle(void);

void MENU_Item_KEY(void);

void OLED_Wing_Auto_PID(void);
void OLED_Wing_Draw_Line(void);
void OLED_Wing_Round(void);
void OLED_Wing_ALL(void);


#endif

