/*
 * ADC_Sample.h
 *
 *  Created on: Feb 29, 2024
 *      Author: Cross
 */

#ifndef ADC_SAMPLE_H_
#define ADC_SAMPLE_H_

#include "main.h"

void ADC_Read(void);

extern volatile uint16_t ADC_Value1[6];
extern volatile uint16_t ADC_Value2[6];

#endif /* ADC_SAMPLE_H_ */
