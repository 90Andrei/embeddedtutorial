/*
 * BMP.h
 *
 *  Created on: Jan 4, 2023
 *      Author: Andrei
 */

#ifndef INC_BMP_H_
#define INC_BMP_H_

#include <stdbool.h>
#include <stdint.h>

bool BMP_Init(void);
int32_t BMP_GetPres(void);
void BMP_Start_IT_Temp(void);
void BMP_Start_IT_Pres(void);
int32_t BMP_GetTemp(void);

#endif /* INC_BMP_H_ */
