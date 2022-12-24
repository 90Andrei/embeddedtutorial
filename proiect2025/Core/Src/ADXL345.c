/*
 * ADXL345.c Accelerometer I2C Driver
 *
 *  Created on: Dec 2, 2022
 *      Author: Andrei
 */

#include "ADXL345.h"

uint8_t ADXL_Init( ADXL *dev, I2C_HandleTypeDef *i2cHandle)
{
	dev->i2cHandle = i2cHandle;
	dev->acc_mps2[0] = 0.0f;
	dev->acc_mps2[1] = 0.0f;
	dev->acc_mps2[2] = 0.0f;

	uint8_t errNum = 0; // to return the numbers of errors i get
	HAL_StatusTypeDef status;
	uint8_t regData;
	status=ADXL_ReadRegister(dev, ADXL_REG_DEVID_AD, &regData);
	errNum += (status !=HAL_OK);

	if ( regData != ADXL_DEVICE_ID)
	{
		return 255;
	}
}

HAL_StatusTypeDef ADXL_ReadRegister( ADXL *dev, uint8_t reg, uint8_t *data )
{
	return HAL_I2C_Mem_Read(dev->i2cHandle, ADXL_I2C_ADDR, reg, I2C_MEMADD_SIZE_8BIT, data, 1, HAL_MAX_DELAY);
}

HAL_StatusTypeDef ADXL_ReadRegisters( ADXL *dev, uint8_t reg, uint8_t *data, uint8_t lenght )
{
	return HAL_I2C_Mem_Read(dev->i2cHandle, ADXL_I2C_ADDR, reg, I2C_MEMADD_SIZE_8BIT, data, lenght, HAL_MAX_DELAY);
}

HAL_StatusTypeDef ADXL_WriteRegister( ADXL *dev, uint8_t reg, uint8_t *data )
{
	return HAL_I2C_Mem_Write(dev->i2cHandle, ADXL_I2C_ADDR, reg, I2C_MEMADD_SIZE_8BIT, data, 1, HAL_MAX_DELAY);
}
