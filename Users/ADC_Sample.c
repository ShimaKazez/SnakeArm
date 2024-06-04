/*
 * ADC_Sample.c
 *
 *  Created on: Feb 29, 2024
 *      Author: Cross
 */
#include "ADC_Sample.h"
#include "adc.h"

volatile uint16_t ADC_Value1[7];
volatile uint16_t ADC_Value2[8];

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {

}

void ADC_Read(void) {
	HAL_ADC_Start_DMA(&hadc1, (uint32_t*) ADC_Value1, sizeof(ADC_Value1) / sizeof(ADC_Value1[0]));
	HAL_ADC_Start_DMA(&hadc2, (uint32_t*) ADC_Value2, sizeof(ADC_Value2) / sizeof(ADC_Value2[0]));
}
