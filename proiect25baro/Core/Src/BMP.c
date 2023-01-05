/*
 * BMP.c
 *
 *  Created on: Jan 4, 2023
 *      Author: Andrei
 */

#include <stdint.h>
#include "i2c.h"
#include <stdbool.h>
#include "main.h"
#include <stdio.h>
#include "BMP.h"

//read and write defines

#define BMP_ADDR_WRITE 0xEE
#define BMP_ADDR_READ  0xEF

//read calibration data registers

#define AC1MSB 0xAA
#define AC1LSB 0xAB
#define AC2MSB 0xAC
#define AC2LSB 0xAD
#define AC3MSB 0xAE
#define AC3LSB 0xAF
#define AC4MSB 0xB0
#define AC4LSB 0xB1
#define AC5MSB 0xB2
#define AC5LSB 0xB3
#define AC6MSB 0xB4
#define AC6LSB 0xB5
#define B1MSB  0xB6
#define B1LSB  0xB7
#define B2MSB  0xB8
#define B2LSB  0xB9
#define MBMSB  0xBA
#define MBLSB  0xBB
#define MCMSB  0xBC
#define MCLSB  0xBD
#define MDMSB  0xBE
#define MDLSB  0xBF

typedef struct barometer
{
	int16_t  AC1;
	int16_t  AC2;
	int16_t  AC3;
	uint16_t    AC4;
    uint16_t    AC5;
	uint16_t    AC6;
	int16_t  B1;
	int16_t  B2;
	int16_t  MB;
	int16_t  MC;
	int16_t  MD;
}baro;
baro b1;


bool BMP_DevId()
{
	if(HAL_I2C_IsDeviceReady(&hi2c1, BMP_ADDR_WRITE, 5, HAL_MAX_DELAY) == HAL_OK)
	{
		return true;
	}

	return false;
}

bool BMP_ReadCalibration()
{

	uint8_t calibValues[22];
	if(HAL_I2C_Mem_Read(&hi2c1, BMP_ADDR_READ, AC1MSB, I2C_MEMADD_SIZE_8BIT, calibValues, 22, HAL_MAX_DELAY) != HAL_OK)
	{
		return false;
	}
	b1.AC1 = ((int16_t)calibValues[0] << 8) | calibValues[1];
	b1.AC2 = ((int16_t)calibValues[2] << 8) | calibValues[3];
	b1.AC3 = ((int16_t)calibValues[4] << 8) | calibValues[5];
	b1.AC4 = ((uint16_t)calibValues[6] << 8) | calibValues[7];
	b1.AC5 = ((uint16_t)calibValues[8] << 8) | calibValues[9];
	b1.AC6 = ((uint16_t)calibValues[10] << 8) | calibValues[11];
	b1.B1 = ((int16_t)calibValues[12] << 8) | calibValues[13];
	b1.B2 = ((int16_t)calibValues[14] << 8) | calibValues[15];
	b1.MB = ((int16_t)calibValues[16] << 8) | calibValues[17];
	b1.MC = ((int16_t)calibValues[18] << 8) | calibValues[19];
	b1.MD = ((int16_t)calibValues[20] << 8) | calibValues[21];

	return true;
}
