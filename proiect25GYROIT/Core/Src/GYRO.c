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
#define GYRO_DEVID       0xF
#define GYRO_SPI_ReadOp (1 << 7)
#define GYRO_SPI_MB     (1 << 6)
#define GYRO_DATAX0     0x28
#define GYRO_CTRL_REG1       0x20
#define GYRO_POWERMODENORMAL 0xF
#define GYRO345_SCALE_FACTOR 9   //0,009 inmultim cu 1000 sa avem integer
#define GYRO_CTRLREG3   0x22

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


void GYRO_PowerMode()
{   uint8_t devId;
	//reset GYRO
	HAL_GPIO_TogglePin(ADXL_RESET_GPIO_Port, ADXL_RESET_Pin);
	HAL_Delay(100);
    HAL_GPIO_TogglePin(ADXL_RESET_GPIO_Port, ADXL_RESET_Pin);

   //stop GYRO power mode
    GYRO_WriteData(GYRO_CTRL_REG1,0);

   //Check SPI communication
    GYRO_ReadData(GYRO_DEVID, &devId);
    if(devId != 0xD3)
    {
    	Error_Handler();
    }
   //enable pin2 DRDY interrupt
    GYRO_WriteData(GYRO_CTRLREG3, 0x8);   //setam bit D3 I2_DRDY pentru a activa intreruprea Data Ready;

   //start GYRO power mode-normal
	GYRO_WriteData(GYRO_CTRL_REG1,GYRO_POWERMODENORMAL);

}

void GYRO_XYZConv(int16_t *xg, int16_t *yg, int16_t *zg)
{



	*xg = (*xg) * GYRO345_SCALE_FACTOR;
	*yg = (*yg) * GYRO345_SCALE_FACTOR;
	*zg = (*zg) * GYRO345_SCALE_FACTOR;
}
