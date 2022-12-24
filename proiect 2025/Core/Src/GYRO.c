/*
 * GYRO.c
 *
 *  Created on: Dec 8, 2022
 *      Author: Andrei
 */
#include <stdio.h>
#include <stdint.h>
#include "main.h"
#include <stdbool.h>

extern SPI_HandleTypeDef hspi1;
#define GYRO_SPI_ReadOp (1 << 7)
#define GYRO_SPI_MB     (1 << 6)
#define GYRO_DATAX0     0x28
#define GYRO_CTRL_REG1       0x20
#define GYRO_POWERMODENORMAL 0xF
#define GYRO345_SCALE_FACTOR 0.00875

bool GYRO_WriteData(uint8_t regAddress, uint8_t registerValue) {
	uint8_t TxBuffer[2];
	uint8_t RxBuffer[2];

	TxBuffer[0] = regAddress;
	TxBuffer[1] = registerValue;

	HAL_GPIO_WritePin(SPI_CS_GYRO_GPIO_Port, SPI_CS_GYRO_Pin, GPIO_PIN_RESET);
	if (HAL_SPI_TransmitReceive(&hspi1, TxBuffer, RxBuffer, 2, HAL_MAX_DELAY) != HAL_OK)
	{
		HAL_GPIO_WritePin(SPI_CS_GYRO_GPIO_Port, SPI_CS_GYRO_Pin, GPIO_PIN_SET);

		return false;
	}
	HAL_GPIO_WritePin(SPI_CS_GYRO_GPIO_Port, SPI_CS_GYRO_Pin, GPIO_PIN_SET);

	return true;
}

bool GYRO_ReadData(uint8_t regAddress, uint8_t *registerValue) {
	uint8_t TxBuffer[2];
	uint8_t RxBuffer[2];

	TxBuffer[0] = GYRO_SPI_ReadOp | regAddress;
	TxBuffer[1] = 0;
	HAL_GPIO_WritePin(SPI_CS_GYRO_GPIO_Port, SPI_CS_GYRO_Pin, GPIO_PIN_RESET);
	if (HAL_SPI_TransmitReceive(&hspi1, TxBuffer, RxBuffer, 2, 200) != HAL_OK) {
		HAL_GPIO_WritePin(SPI_CS_GYRO_GPIO_Port, SPI_CS_GYRO_Pin, GPIO_PIN_SET);
		return false;

	}
	HAL_GPIO_WritePin(SPI_CS_GYRO_GPIO_Port, SPI_CS_GYRO_Pin, GPIO_PIN_SET);
	*registerValue = RxBuffer[1];

	return true;
}

void GYRO_ReadValuesXYZ(int16_t *x, int16_t *y, int16_t *z)
{
	uint8_t TxBuffer[7] = { 0 };
	uint8_t RxBuffer[7];

	TxBuffer[0] = GYRO_DATAX0 | GYRO_SPI_MB | GYRO_SPI_ReadOp;
	HAL_GPIO_WritePin(SPI_CS_GYRO_GPIO_Port, SPI_CS_GYRO_Pin, GPIO_PIN_RESET);
	HAL_SPI_TransmitReceive(&hspi1, TxBuffer, RxBuffer, 7, HAL_MAX_DELAY);

	*x = (int16_t)(RxBuffer[2] << 8) | RxBuffer[1];
	*y = (int16_t)(RxBuffer[4] << 8) | RxBuffer[3];
	*z = (int16_t)(RxBuffer[6] << 8) | RxBuffer[5];
	HAL_GPIO_WritePin(SPI_CS_GYRO_GPIO_Port, SPI_CS_GYRO_Pin, GPIO_PIN_SET);
}

void GYRO_ReadXYZv2(int16_t *x, int16_t *y, int16_t *z)
{
	uint8_t TxBuffer[2]={0x28,00};
	uint8_t RxBuffer[2];
	uint8_t x0,x1,y0,y1,z0,z1;


	HAL_GPIO_WritePin(SPI_CS_GYRO_GPIO_Port, SPI_CS_GYRO_Pin, GPIO_PIN_RESET);
	HAL_SPI_TransmitReceive(&hspi1,TxBuffer, RxBuffer, 2, HAL_MAX_DELAY);
	x0 = RxBuffer[1];
	TxBuffer[0]=0x29;
	HAL_GPIO_WritePin(SPI_CS_GYRO_GPIO_Port, SPI_CS_GYRO_Pin, GPIO_PIN_RESET);

	HAL_GPIO_WritePin(SPI_CS_GYRO_GPIO_Port, SPI_CS_GYRO_Pin, GPIO_PIN_RESET);
    HAL_SPI_TransmitReceive(&hspi1,TxBuffer, RxBuffer, 2, HAL_MAX_DELAY);
	x1 = RxBuffer[1];
	TxBuffer[0]=0x2A;
	HAL_GPIO_WritePin(SPI_CS_GYRO_GPIO_Port, SPI_CS_GYRO_Pin, GPIO_PIN_RESET);

	HAL_GPIO_WritePin(SPI_CS_GYRO_GPIO_Port, SPI_CS_GYRO_Pin, GPIO_PIN_RESET);
	HAL_SPI_TransmitReceive(&hspi1,TxBuffer, RxBuffer, 2, HAL_MAX_DELAY);
	y0 = RxBuffer[1];
	TxBuffer[0]=0x2B;
	HAL_GPIO_WritePin(SPI_CS_GYRO_GPIO_Port, SPI_CS_GYRO_Pin, GPIO_PIN_RESET);

	HAL_GPIO_WritePin(SPI_CS_GYRO_GPIO_Port, SPI_CS_GYRO_Pin, GPIO_PIN_RESET);
	HAL_SPI_TransmitReceive(&hspi1,TxBuffer, RxBuffer, 2, HAL_MAX_DELAY);
	y1 = RxBuffer[1];
	TxBuffer[0]=0x2C;
	HAL_GPIO_WritePin(SPI_CS_GYRO_GPIO_Port, SPI_CS_GYRO_Pin, GPIO_PIN_RESET);

	HAL_GPIO_WritePin(SPI_CS_GYRO_GPIO_Port, SPI_CS_GYRO_Pin, GPIO_PIN_RESET);
	HAL_SPI_TransmitReceive(&hspi1,TxBuffer, RxBuffer, 2, HAL_MAX_DELAY);
	z0 = RxBuffer[1];
	TxBuffer[0]=0x2D;
    HAL_GPIO_WritePin(SPI_CS_GYRO_GPIO_Port, SPI_CS_GYRO_Pin, GPIO_PIN_RESET);

	HAL_GPIO_WritePin(SPI_CS_GYRO_GPIO_Port, SPI_CS_GYRO_Pin, GPIO_PIN_RESET);
    HAL_SPI_TransmitReceive(&hspi1,TxBuffer, RxBuffer, 2, HAL_MAX_DELAY);
	z1 = RxBuffer[1];
	HAL_GPIO_WritePin(SPI_CS_GYRO_GPIO_Port, SPI_CS_GYRO_Pin, GPIO_PIN_RESET);

	*x = (int16_t)(x1 << 8) | x0;
	*y = (int16_t)(y1 << 8) | y0;
	*z = (int16_t)(z1 << 8) | z0;

}
void GYRO_PowerMode()
{
	GYRO_WriteData(GYRO_CTRL_REG1,0);
	GYRO_WriteData(GYRO_CTRL_REG1,GYRO_POWERMODENORMAL);
	GYRO_WriteData(0x23, 0x80);
}

void GYRO_XYZConv(float *xg , float *yg, float *zg)
{   int16_t x,y,z;


	GYRO_ReadValuesXYZ(&x, &y, &z);
	*xg = x * GYRO345_SCALE_FACTOR;
	*yg = y * GYRO345_SCALE_FACTOR;
	*zg = z * GYRO345_SCALE_FACTOR;
}
