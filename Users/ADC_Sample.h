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

void ADC_Read(void);

extern volatile uint16_t ADC_value_1[2];
extern volatile uint16_t ADC_value_2[4];
extern volatile uint16_t ADC_value_2_Kalman[4];
extern Kalman kfp_1, kfp_2, kfp_3, kfp_4;

#endif /* ADC_SAMPLE_H_ */
