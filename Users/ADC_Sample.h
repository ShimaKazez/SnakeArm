/*
 * ADC_Sample.h
 *
 *  Created on: Feb 29, 2024
 *      Author: Cross
 */

#ifndef ADC_SAMPLE_H_
#define ADC_SAMPLE_H_

#include "main.h"
#include "adc.h"
#include "Kalman.h"

#define ADC_PWR_HANDLE  &hadc1
#define ADC_SEN_HANDLE  &hadc2

void ADC_Read(void);

extern volatile uint16_t ADC_PWR_Value[2];
extern volatile uint16_t ADC_SEN_Value[4];
extern volatile uint16_t ADC_SEN_Value_Kalman[4];
extern Kalman kfp_1, kfp_2, kfp_3, kfp_4;

#endif /* ADC_SAMPLE_H_ */
