/*
 * HMC.h
 *
 *  Created on: Dec 29, 2022
 *      Author: Andrei
 */

#ifndef INC_HMC_H_
#define INC_HMC_H_

#include <stdbool.h>
#include <stdint.h>

bool HMC_Devid ();
bool HMC_DEVIDv2();
void HMC_IT_GetValuesXYZ(int16_t *x, int16_t *y, int16_t *z);
bool HMC_ReadValues();
bool Set_SingleMeasureMode();

#endif /* INC_HMC_H_ */
