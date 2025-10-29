/*
 * ADC_Sample.h
 *
 *  Created on: Feb 29, 2024
 *      Author: Cross
 */

#ifndef ADC_SAMPLE_H_
#define ADC_SAMPLE_H_

#include <Filters.h>
#include "main.h"
#include "adc.h"

#define ADC_PWR_HANDLE  &hadc1
#define ADC_SEN_HANDLE  &hadc2

void ADC_Read(void);

extern uint16_t ADC_PWR_Value[2];
extern uint16_t ADC_SEN_Value[4];
extern uint16_t ADC_SEN_Value_Kalman[4];
extern Kalman KFP_SEN_1, KFP_SEN_2, KFP_SEN_3, KFP_SEN_4;

#endif /* ADC_SAMPLE_H_ */
