/*
 * TimerCallbacks.h
 *
 *  Created on: Mar 20, 2025
 *      Author: ME_Gu
 */

#ifndef TIMERCALLBACKS_H_
#define TIMERCALLBACKS_H_

#include "stm32G4xx_hal.h"

extern volatile float Angle_Data[3];
extern volatile float Speed_Data[3];
extern volatile float Torque_Data[3];

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);

#endif /* TIMERCALLBACKS_H_ */

