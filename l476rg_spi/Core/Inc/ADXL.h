/*
 * ADXL.h
 *
 *  Created on: Dec 6, 2022
 *      Author: Andrei
 */

#ifndef INC_ADXL_H_
#define INC_ADXL_H_


#include <stdbool.h>

#define ADXL_DATAX0 0x32
#define ADXL_POWER_CTL 0x2D

void ADXL_ReadValuesXYZ(int16_t *x, int16_t *y, int16_t *z);
void ADXL_Set_Measurebit();
bool ADXL_ReadMultipleBytes(uint8_t firstRegAddress, uint8_t * regValues, uint8_t lenght);
void ADXL_XYZValuesG(float *xg , float *yg, float *zg);

#endif /* INC_ADXL_H_ */
