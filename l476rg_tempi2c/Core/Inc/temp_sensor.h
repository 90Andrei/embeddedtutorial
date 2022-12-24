/**
  ******************************************************************************
  * @file           : temp_sensor.h
  * @brief          : Driver for temperature sensor measurement using i2c
  * @author         : Sebastian Popa, sebastian.popa@raptor-technologies.com
  * @date           : Sep 21, 2021
  ******************************************************************************
  */
#ifndef TEMP_SENSOR_H_
#define TEMP_SENSOR_H_
/* Includes */
#include <stdint.h>

/** \defgroup TempSensorGroup TemperatureSensor
\brief Temperature Measurement using i2c
The Temperature Sensor Module (LM75B) communicates with the MIRA Board over i2c.

The GPIO Pins used:
PB10 -> I2C_SCL (CLOCK)
PB11 -> I2C_SDA (DATA)


The i2c default address is set to 0x48 by grounding address pins A2, A1 and A0.


Use of pull-up resistors should be used on SCL and SDA lines as specified in the
LM75B Datasheet chapter 8.1

*/

/** \addtogroup TempSensorGroup
 *  @{
 */

/* Defines */

/* Enums -----------------------------------------------------------------------*/


/* Global variables */

/* Public Function Declaration */
/**
 * @brief Initializes temperature sensor
 *
 * @return status, 0 for success
 */
int TEMPSENSOR_Init(void);

/**
 * @brief Reads temperature value from sensor
 *
 * @param outputValue
 * @return status, 0 for success
 */
int TEMPSENSOR_ReadTempData(int16_t *outputValue);

/**
 * @brief Places the Temperature Sensor in Low Power Mode by writing the configuration register
 *
 * @return status, 0 for success
 */
int TEMPSENSOR_ShutDown(void);

/** @}*/

#endif /* TEMP_SENSOR_H_ */
/* End of File */

