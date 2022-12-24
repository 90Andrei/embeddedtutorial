/*
 * ADXL345.h Accelerometer I2C Driver
 *
 *  Created on: Dec 2, 2022
 *      Author: Andrei
 */

#ifndef INC_ADXL345_H_
#define INC_ADXL345_H_

#include "stm32l4xx_hal.h"
#include "i2c.h"


//defines

#define ADXL_I2C_ADDR (0x1D << 1) //we shift left by 1 because i2c addr firs bit is read or write bit

#define ADXL_DEVICE_ID E5   // device id reset value

//registers
#define ADXL_REG_DEVID_AD 0x00

//struct

typedef struct
{
	I2C_HandleTypeDef * i2cHandle;

	//acceleration data(x,y,z) in m/s^2
	float acc_mps2[3];
}ADXL;

//init

uint8_t ADXL_Init( ADXL *dev, I2C_HandleTypeDef *i2cHandle);

//data read

HAL_StatusTypeDef ADXL_ReadAccelerations(ADXL *dev);

//low level functions

HAL_StatusTypeDef ADXL_ReadRegister( ADXL *dev, uint8_t reg, uint8_t *data );
HAL_StatusTypeDef ADXL_ReadRegisters(ADXL *dev, uint8_t reg, uint8_t *data ,uint8_t lengh);

HAL_StatusTypeDef ADXL_WriteRegister(ADXL *dev, uint8_t reg, uint8_t *data);





#endif /* INC_ADXL345_H_ */
