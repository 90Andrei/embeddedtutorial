/*
 * GYRO.h
 *
 *  Created on: Dec 8, 2022
 *      Author: Andrei
 */

#ifndef INC_GYRO_H_
#define INC_GYRO_H_

void GYRO_PowerMode();
void GYRO_ReadValuesXYZ(int16_t *x, int16_t *y, int16_t *z);
void GYRO_ReadXYZv2(int16_t *x, int16_t *y, int16_t *z);
void GYRO_XYZConv(float *xg , float *yg, float *zg);
#endif /* INC_GYRO_H_ */
