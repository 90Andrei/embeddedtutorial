/*
 * temp_sensor.c
 *
 *  Created on: Nov 28, 2022
 *      Author: Andrei
 */

/**
  ******************************************************************************
  * @file           : temp_sensor.c
  * @brief          : Driver for temperature sensor measurement using i2c
  * @author         : Sebastian Popa, sebastian.popa@raptor-technologies.com
  * @date           : Sep 21, 2021
  ******************************************************************************
  */

/* Includes */
#include <errno.h>

#include "stm32l4xx_hal.h"
#include "i2c.h"

#include "temp_sensor.h"


/* Macros */
#define ARRAY_LENGTH(array) (sizeof((array))/sizeof((array)[0]))
#define ESUCCESS    0

/* Defines */
#define TEMP_SENSOR_I2C_HANDLER            hi2c1       /**< i2c HAL handler */
#define TEMP_SENSOR_I2C_ADDRESS            (0x48 << 1) /**< 7bit address of device, shifted 1bit as required by HAL driver */

/* LM75B 7.4.1 POINTER REGISTER
 * The Pointer register contains an 8-bit data byte, of which the two LSB bits represent the
 * pointer value of the other four registers, and the other 6 MSB bits are equal to 0.
 * The Pointer register is not accessible to the user, but is used to select the data register
 * for write/read operation by including the pointer data byte in the bus command.*/
#define TEMPERATURE_REGISTER_POINTER       0b00 /**< Temperature Register (Temp)*/
#define CONFIGURATION_REGISTER_POINTER     0b01 /**< Configuration Register (Conf)*/
#define HYSTERESIS_REGISTER_POINTER        0b10 /**< Hysteresis register (Thyst), NOT USED*/
#define OVERTEMP_SHUTDOWN_REGISTER_POINTER 0b11 /**< Overtemperature shutdown register (Tos), NOT USED*/

#define TEMP_DATA_SIZE                     sizeof(uint16_t) /**< Output from Temperature Sensor is 2 bytes*/
#define TEMP_REGISTER_POINTER_SIZE         sizeof(uint8_t) /**< Temperature Register Pointer is 1 byte*/

/* The Configuration register (Conf) is a write/read register and contains an 8-bit
 * non-complement data byte that is used to configure the device for different operation
 * conditions. Last 2 bits are used to place the device in normal operation or shutdown.
 * The other bits have the default value of 0 and will not be changed
 */
#define CONFIG_REGISTER_NORMAL_OPERATION   0b00 /**< Device operation mode: Normal*/
#define CONFIG_REGISTER_SHUTDOWN           0b01 /**< Device operation mode: Shutdown*/

#define TEMP_SENSOR_I2C_TIMEOUT_MS         1000 /**< Timeout for i2c communication in polling mode*/

/* Static Variable Declarations */

/* Static function prototypes */
static int DRIVERUTILS_ConvertHALStatus(HAL_StatusTypeDef HALStatus)
{
    int status = ESUCCESS;
    switch (HALStatus)
    {
        case HAL_OK:
            status = ESUCCESS;
            break;
        case HAL_ERROR:
            status = -EIO;
            break;
        case HAL_BUSY:
            status = -EBUSY;
            break;
        case HAL_TIMEOUT:
            status = -ETIMEDOUT;
            break;
    }

    return status;
}

/**
 * @brief Sets the Temperature Sensor to Normal Operation by writing the config register
 *
 * @return status, 0 for success
 */
static int _setDeviceToNormalOperation(void)
{
    // Wake-up device
    // Set Config Register to Normal Operation
    uint8_t configData[] = { CONFIGURATION_REGISTER_POINTER, CONFIG_REGISTER_NORMAL_OPERATION };

    HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(&TEMP_SENSOR_I2C_HANDLER,
            TEMP_SENSOR_I2C_ADDRESS, configData, ARRAY_LENGTH(configData), TEMP_SENSOR_I2C_TIMEOUT_MS);
    return DRIVERUTILS_ConvertHALStatus(status);
}

/**
 * @brief Selects the Temperature Register for reading by writing the POINTER REGISTER
 *
 * @return status, 0 for success
 */
static int _selectTemperatureRegister(void)
{
    // Select the Temperature Register
    uint8_t tempRegisterData = TEMPERATURE_REGISTER_POINTER;

    HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(&TEMP_SENSOR_I2C_HANDLER,
            TEMP_SENSOR_I2C_ADDRESS, &tempRegisterData, TEMP_REGISTER_POINTER_SIZE, TEMP_SENSOR_I2C_TIMEOUT_MS);
    return DRIVERUTILS_ConvertHALStatus(status);
}

/**
 * @brief Reads Temperature Data from the Temperature Register.
 *
 * @param outputData
 * @param outputSize
 * @return status, 0 for success
 */
static int _readTempData(uint8_t *outputData, uint8_t outputSize)
{
    assert_param(outputSize == TEMP_DATA_SIZE);

    _selectTemperatureRegister();

    HAL_StatusTypeDef status = HAL_I2C_Master_Receive(&TEMP_SENSOR_I2C_HANDLER,
            TEMP_SENSOR_I2C_ADDRESS, outputData, outputSize, TEMP_SENSOR_I2C_TIMEOUT_MS);
    return DRIVERUTILS_ConvertHALStatus(status);
}

/**
 * @brief  The Temperature register (Temp) holds the digital result of temperature measurement or
 *         monitor at the end of each analog-to-digital conversion. This register is read-only and
 *         contains two 8-bit data bytes consisting of one Most Significant Byte (MSByte) and one
 *         Least Significant Byte (LSByte).
 *         However, only 11 bits of those two bytes are used to store the Temp data in two’s complement
 *         format with the resolution of 0.125 deg C.
 *
 *         -# If the Temp data MSByte bit D10 = 0, then the temperature is positive and
 *            Temp value(deg C) = +(Temp data) * 0.125 degC.
 *         -# If the Temp data MSByte bit D10 = 1, then the temperature is negative and
 *            Temp value(deg C) = -(two’s complement of Temp data) * 0.125 degC.
 *
 * @param rawData 2 byte array containing raw data from temperature sensor
 * @param rawDatasize expected data size is 2, if not, an error is raised
 * @return converted temperature in degrees Celsius
 */
static int16_t _convertRawTempValue(const uint8_t *rawData, uint8_t rawDatasize)
{
    int16_t tempValue;
    assert_param(rawDatasize == TEMP_DATA_SIZE);

    uint8_t MSByte = rawData[0];
    uint8_t LSByte = rawData[1];

    tempValue = (MSByte << 8u) + LSByte;

    // Ignore last 5 bits of LSB
    tempValue = tempValue >> 5u;

    if (tempValue > 0x3FFu) // temperature value is negative
    {
        // Ignore signed bit
        tempValue &= ~(0x400u);

        // Two's complement
        tempValue = (~(tempValue) + 1) * (-1);
        tempValue = (int16_t)(tempValue * 10 / 8); // tempValue * 10 * 125 / 1000 (0.125)
    }
    else // temperature value is positive
    {
        tempValue = (int16_t)(tempValue * 10 / 8); // tempValue * 10 * 125 / 1000 (0.125)
    }

    return tempValue;
}

/* Function Declarations */
int TEMPSENSOR_Init(void)
{
    int err;

    // Set device to Normal Operation
    err = _setDeviceToNormalOperation();
    if (err != ESUCCESS)
    {
      //  log_msg("TEMPSENSOR_Init FAILED WITH CODE: %d", err);
        return err;
    }

    /* @ref LM75B datasheet 7.4.1 page 8
     * After power-up, the first temperature reading will be incorrect and must be ignored.
     * Subsequent reads of the temperature register will be correct.
     */
    uint8_t tempData[TEMP_DATA_SIZE];
    err = _readTempData(tempData, ARRAY_LENGTH(tempData));
    if (err != ESUCCESS)
    {
      //  log_msg("TEMPSENSOR_Init FAILED WITH CODE: %d", err);
        return err;
    }

    return err;
}

int TEMPSENSOR_ReadTempData(int16_t *outputValue)
{
    int err;

    uint8_t rawTempData[TEMP_DATA_SIZE];
    err = _readTempData(rawTempData, ARRAY_LENGTH(rawTempData));

    if (err != ESUCCESS)
    {
      //  log_msg("TEMPSENSOR_ReadTempData FAILED WITH CODE: %d", err);
        return err;
    }

    *outputValue = _convertRawTempValue(rawTempData, ARRAY_LENGTH(rawTempData));
//    log_msg("Temperature: %d", *outputValue);

    return err;
}

int TEMPSENSOR_ShutDown(void)
{
    // Set Config Register to ShutDown
    uint8_t configData[] = { CONFIGURATION_REGISTER_POINTER, CONFIG_REGISTER_SHUTDOWN };

    HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(&TEMP_SENSOR_I2C_HANDLER,
            TEMP_SENSOR_I2C_ADDRESS, configData, ARRAY_LENGTH(configData), TEMP_SENSOR_I2C_TIMEOUT_MS);

    int err = DRIVERUTILS_ConvertHALStatus(status);

    if (err != ESUCCESS)
    {
      //  log_msg("TEMPSENSOR_ShutDown FAILED WITH CODE: %d", err);
    }

    return err;
}

/* End of File */

