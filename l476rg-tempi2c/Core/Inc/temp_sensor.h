/**
 * @brief Reads temperature value from sensor
 *
 * @param outputValue
 * @return status, 0 for success
 */

#ifndef INC_TEMP_SENSOR_H_
#define INC_TEMP_SENSOR_H_

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




