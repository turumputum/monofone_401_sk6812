/*
 * tlv493.c
 *
 *  Created on: 18 мар. 2020 г.
 *      Author: Dmitry
 */
#include "main.h"
#include "tlv493.h"
#include "math.h"

//uint16_t addr= 0x0BC;
uint16_t addr= 0x00;
extern I2C_HandleTypeDef hi2c1;
static int inited=0;
uint8_t i2cData[7];

int tlv_init()
{

	uint8_t regs[2]={0x00,0x02}; // no int, lp=0,fast=1; master controlled mode


	for (addr=2; addr<255; addr++)
	{
		if (HAL_I2C_Master_Transmit(&hi2c1, addr, regs, 2, 5) ==HAL_OK)
		{
			break;
		}
		HAL_Delay(1);
	}

	//uint8_t regs[2]={0x00,0x02}; // no int, lp=0,fast=1; master controlled mode
	//HAL_I2C_Master_Transmit(&hi2c1, addr, regs, 2, 5)

	if (HAL_I2C_IsDeviceReady(&hi2c1, addr, 1, 20) ==HAL_OK || addr<255){
		inited=1;
		return 1;
	} else {
		return -1;
	}

}
int tlv_reset()
{
	HAL_I2C_Master_Transmit(&hi2c1, addr, 0, 1, 20);
	return 1;
}
int getMagVal()
{
	/*
	if (!inited)
		{
			tlv_init();
			HAL_Delay(30);
		}
	*/
	int16_t bz_value;
	int16_t by_value;
	int16_t bx_value;
	int magnitude;
	//uint8_t tlvData[8];
	//uint8_t res=	HAL_I2C_Master_Receive(&hi2c1, addr, tlvData, 7, 10);
	for(int y = 0;y<6;y++){
		if(HAL_I2C_IsDeviceReady(&hi2c1, addr, 1, 20) == HAL_BUSY ){
			HAL_Delay(30);
		}
	}
	if(HAL_I2C_Mem_Read(&hi2c1, addr, 0x01, 7, i2cData,7,10)== HAL_OK){
		/*
		if (res!=HAL_OK)
			{
				tlv_init();
				HAL_Delay(30);
			}
			*/
		uint8_t bz_high = i2cData[2];
		uint8_t bz_low = i2cData[5];
		bz_value = (bz_high << 8) | ((bz_low & 0x0F)<<4);
		bz_value>>=4;

		uint8_t by_high = i2cData[1];
		uint8_t by_low = i2cData[4];
		by_value = (by_high << 8) | ((by_low & 0x0F)<<4);
		by_value>>=4;

		uint8_t bx_high = i2cData[0];
		uint8_t bx_low = i2cData[4];
		bx_value = (bx_high << 8) | bx_low;
		bx_value>>=4;
		//if (bz_value>4000){bz_value=0;}

 		magnitude=abs(bz_value)+abs(by_value)+abs(bx_value);

 		if(magnitude>50){magnitude=50;}
		return(magnitude);
	} else { return -1;}

}
