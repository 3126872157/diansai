#ifndef GRAYSCALE_SENSOR_H
#define GRAYSCALE_SENSOR_H

#include "main.h"
#include "i2c.h"

#define GW_GRAY_ADDR 0x4C
void grayscale_init();
void grayscale_GetData();

#endif