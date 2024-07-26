#include "GRAYSCALE_SENSOR.h"

uint8_t i2c_cmd;
uint8_t data=0;
uint8_t i=0;
int ret_transmit=0,ret_receive=0;
uint8_t Data[8];
void grayscale_init()
{
	/* ��������ģʽ, �������֮��, ÿ�ζ�ȡһ��8bit������, ÿ��λ��ʾ1-8̽ͷ��״̬ */
	i2c_cmd = 0xDD;
	ret_transmit = HAL_I2C_Master_Transmit(&hi2c1, GW_GRAY_ADDR << 1, &i2c_cmd, 1, HAL_MAX_DELAY);
	if (ret_transmit != HAL_OK) {
		
		Error_Handler();
	}
}
void grayscale_GetData()
{
	//����Ķ�дλ��ʵ����ȥ���֣�hal��ĺ����Զ�����
	ret_receive = HAL_I2C_Master_Receive(&hi2c1,((GW_GRAY_ADDR << 1)|0x01) , &data, 1,HAL_MAX_DELAY);
	if (ret_receive != HAL_OK) {
		Error_Handler();
	}
	for(i=0;i<8;i++)
	{
		Data[i]=(data >> i) & 0x01;
	}
	HAL_Delay(1);
}