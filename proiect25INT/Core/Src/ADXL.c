/*
 * ADXL.c
 *
 *  Created on: Dec 6, 2022
 *      Author: Andrei
 */
#include <stdio.h>
#include <stdint.h>
#include <ADXL.h>
#include "main.h"
#include <stdbool.h>
#include <errno.h>

extern SPI_HandleTypeDef hspi1;
#define ADXL_SPI_ReadOp (1 << 7)
#define ADXL_SPI_MB     (1 <<6)
#define ADXL_INT_ENABLE_REG 0x2E
#define ADXL_INT_MAP_REG    0x2F
#define ADXL_DEVICEID       0x00

bool ADXL_WriteData(uint8_t regAddress, uint8_t registerValue) {
	uint8_t TxBuffer[2];
	uint8_t RxBuffer[2];

	TxBuffer[0] = regAddress;
	TxBuffer[1] = registerValue;

	HAL_GPIO_WritePin(SPI_CS_ACC_GPIO_Port, SPI_CS_ACC_Pin, GPIO_PIN_RESET);
	if (HAL_SPI_TransmitReceive(&hspi1, TxBuffer, RxBuffer, 2, 200) != HAL_OK)
	{
		HAL_GPIO_WritePin(SPI_CS_ACC_GPIO_Port, SPI_CS_ACC_Pin, GPIO_PIN_SET);

		return false;
	}
	HAL_GPIO_WritePin(SPI_CS_ACC_GPIO_Port, SPI_CS_ACC_Pin, GPIO_PIN_SET);

	return true;
}

bool ADXL_ReadData(uint8_t regAddress, uint8_t *registerValue) {
	uint8_t TxBuffer[2];
	uint8_t RxBuffer[2];

	TxBuffer[0] = ADXL_SPI_ReadOp | regAddress;
	TxBuffer[1] = 0;
	HAL_GPIO_WritePin(SPI_CS_ACC_GPIO_Port, SPI_CS_ACC_Pin, GPIO_PIN_RESET);
	if (HAL_SPI_TransmitReceive(&hspi1, TxBuffer, RxBuffer, 2, 200) != HAL_OK) {
		HAL_GPIO_WritePin(SPI_CS_ACC_GPIO_Port, SPI_CS_ACC_Pin, GPIO_PIN_SET);
		return false;

	}
	HAL_GPIO_WritePin(SPI_CS_ACC_GPIO_Port, SPI_CS_ACC_Pin, GPIO_PIN_SET);
	*registerValue = RxBuffer[1];

	return true;
}

bool ADXL_ReadMultipleBytes(uint8_t firstRegAddress, uint8_t *regValues,
		uint8_t lenght) {
	uint8_t TxBuffer[lenght];
	uint8_t RxBuffer[lenght];
	uint8_t i;
	TxBuffer[0] = ADXL_SPI_MB | ADXL_SPI_ReadOp | firstRegAddress;

	for (i = 1; i < lenght + 1; i++)
	{
		TxBuffer[i] = 0;
	}

	HAL_GPIO_WritePin(SPI_CS_ACC_GPIO_Port, SPI_CS_ACC_Pin, GPIO_PIN_RESET);
	if (HAL_SPI_TransmitReceive(&hspi1, TxBuffer, RxBuffer, lenght,
			HAL_MAX_DELAY) != HAL_OK)
	{
		HAL_GPIO_WritePin(SPI_CS_ACC_GPIO_Port, SPI_CS_ACC_Pin, GPIO_PIN_SET);

		return false;
	}
	HAL_GPIO_WritePin(SPI_CS_ACC_GPIO_Port, SPI_CS_ACC_Pin, GPIO_PIN_SET);

	for (i = 0; i < lenght; i++) {
		regValues[i] = RxBuffer[i];
	}

	return true;

}

void ADXL_ReadValuesXYZ(int16_t *x, int16_t *y, int16_t *z) {
	uint8_t TxBuffer[] = { 0, 0, 0, 0, 0, 0, 0 };
	uint8_t RxBuffer[7];

	TxBuffer[0] = ADXL_DATAX0 | ADXL_SPI_MB | ADXL_SPI_ReadOp;
	HAL_GPIO_WritePin(SPI_CS_ACC_GPIO_Port, SPI_CS_ACC_Pin, GPIO_PIN_RESET);
	HAL_SPI_TransmitReceive(&hspi1, TxBuffer, RxBuffer, 7, 200);


	*x = ((int16_t) RxBuffer[2] << 8) + RxBuffer[1];
	*y = ((int16_t) RxBuffer[4] << 8) + RxBuffer[3];
	*z = ((int16_t) RxBuffer[6] << 8) + RxBuffer[5];
	HAL_GPIO_WritePin(SPI_CS_ACC_GPIO_Port, SPI_CS_ACC_Pin, GPIO_PIN_SET);
}

void ADXL_DMA_ReadValuesXYZ(int16_t *x, int16_t *y, int16_t *z)
{
	uint8_t TxBuffer[] = { 0, 0, 0, 0, 0, 0, 0 };
	uint8_t RxBuffer[7];

	TxBuffer[0] = ADXL_DATAX0 | ADXL_SPI_MB | ADXL_SPI_ReadOp;
	HAL_GPIO_WritePin(SPI_CS_ACC_GPIO_Port, SPI_CS_ACC_Pin, GPIO_PIN_RESET);
	HAL_SPI_TransmitReceive_DMA(&hspi1, TxBuffer, RxBuffer, 7);


	*x = ((int16_t) RxBuffer[2] << 8) + RxBuffer[1];
	*y = ((int16_t) RxBuffer[4] << 8) + RxBuffer[3];
	*z = ((int16_t) RxBuffer[6] << 8) + RxBuffer[5];

	HAL_GPIO_WritePin(SPI_CS_ACC_GPIO_Port, SPI_CS_ACC_Pin, GPIO_PIN_SET);
}

void ADXL_Stop_Measurebit(void)
{
	ADXL_WriteData(ADXL_POWER_CTL, 0x4);

}

void ADXL_Set_Measurebit(void)
{
	ADXL_WriteData(ADXL_POWER_CTL, 0x0);
	ADXL_WriteData(ADXL_POWER_CTL, 0x8);
}

void ADXL_XYZValuesG(int16_t *xg , int16_t *yg, int16_t *zg)
{   int16_t x,y,z;

//	ADXL_Set_Measurebit();
	ADXL_ReadValuesXYZ(&x, &y, &z);
	*xg = x * ADXL345_SCALE_FACTOR;
	*yg = y * ADXL345_SCALE_FACTOR;
	*zg = z * ADXL345_SCALE_FACTOR;
}

void ADXL_INIT_IT()
{
	ADXL_WriteData(ADXL_INT_MAP_REG , 0x80);  //setam bit d7 ca sa activam intreruperea data ready pt int2
	ADXL_WriteData(ADXL_INT_ENABLE_REG, 0x0);
	ADXL_WriteData(ADXL_INT_ENABLE_REG, 0x80);
}

int ADXL_Init(void)
{
	uint8_t devId;
    int16_t x,y,z;

    // Enter low power mode
    ADXL_Stop_Measurebit();

    // Enable Interupts
    ADXL_INIT_IT();

	// Check SPI Communication
    ADXL_ReadData(ADXL_DEVICEID, &devId);
    if (devId != 0xE5)
    {
    	Error_Handler();
    }

	// Start Measure
    ADXL_Set_Measurebit();

	// Make 1 dummy read to x, y, z, return -EIO if HAL Read fails
  //  ADXL_ReadValuesXYZ(&x, &y, &z);

    return 0;
}
