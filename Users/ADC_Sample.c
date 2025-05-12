/*
 * ADC_Sample.c
 *
 *  Created on: Feb 29, 2024
 *      Author: Cross
 */
#include "ADC_Sample.h"

#define time_smoothing_window_width 5

volatile uint16_t ADC_value_1[2];
volatile uint16_t ADC_value_2[4];
volatile uint16_t ADC_value_2_Kalman[4];
uint16_t time_smoothing_window[4][time_smoothing_window_width];
uint16_t time_smoothing_counter[4];
int time_smoothing_flag;
Kalman kfp_1, kfp_2, kfp_3, kfp_4;

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {

}

uint16_t ADC_Read_Single(ADC_HandleTypeDef *hadc, uint32_t Channel) {
	ADC_ChannelConfTypeDef sConfig = { 0 };
	sConfig.Channel = Channel; /* ͨ�� */
	sConfig.Rank = ADC_REGULAR_RANK_1;
	sConfig.SamplingTime = ADC_SAMPLETIME_24CYCLES_5; /* ����ʱ�� */
	if (HAL_ADC_ConfigChannel(hadc, &sConfig) != HAL_OK) {
		Error_Handler();
	}
	HAL_ADC_Start(hadc);
	HAL_ADC_PollForConversion(hadc, 10);
	return (uint16_t) HAL_ADC_GetValue(hadc);
}

uint16_t SafeADCRead(ADC_HandleTypeDef *hadc, uint32_t channel) {
    uint16_t value = ADC_Read_Single(hadc, channel);
    if (value > 4095) value = 4095; // 限制最大值
    return value;
}

void ADC_Read(void) {
	//HAL_ADC_Start(&hadc1);
	//HAL_ADC_PollForConversion(&hadc1, 50);
	//ADC_Value1[1] = ADC_Read_Single(&hadc1, ADC_CHANNEL_12);
	//ADC_Value1[0] = ADC_Read_Single(&hadc1, ADC_CHANNEL_15);

	ADC_value_2[0] = SafeADCRead(&hadc2, ADC_CHANNEL_3);
	ADC_value_2[1] = SafeADCRead(&hadc2, ADC_CHANNEL_4);
	ADC_value_2[2] = SafeADCRead(&hadc2, ADC_CHANNEL_11);
	ADC_value_2[3] = SafeADCRead(&hadc2, ADC_CHANNEL_12);

	ADC_value_2_Kalman[0] = KalmanFilter(&kfp_1, ADC_value_2[0]);
	ADC_value_2_Kalman[1] = KalmanFilter(&kfp_2, ADC_value_2[1]);
	ADC_value_2_Kalman[2] = KalmanFilter(&kfp_3, ADC_value_2[2]);
	ADC_value_2_Kalman[3] = KalmanFilter(&kfp_4, ADC_value_2[3]);

	/*
	 for (int i = 0; i < 4; i++) {
	 TimeSmoothingSum[i] -= TimeSmoothing[i][TimeSmoothingFlag];
	 switch (i) {
	 case 0:
	 TimeSmoothing[i][TimeSmoothingFlag] = ADC_Read_Single(&hadc2, ADC_CHANNEL_3);
	 break;
	 case 1:
	 TimeSmoothing[i][TimeSmoothingFlag] = ADC_Read_Single(&hadc2, ADC_CHANNEL_4);
	 break;
	 case 2:
	 TimeSmoothing[i][TimeSmoothingFlag] = ADC_Read_Single(&hadc2, ADC_CHANNEL_11);
	 break;
	 case 3:
	 TimeSmoothing[i][TimeSmoothingFlag] = ADC_Read_Single(&hadc2, ADC_CHANNEL_12);
	 break;
	 }
	 TimeSmoothingSum[i] += TimeSmoothing[i][TimeSmoothingFlag];
	 ADC_Value2[i] = TimeSmoothingSum[i] / TimeWindowWidth;
	 }

	 TimeSmoothingFlag++;
	 if (TimeSmoothingFlag >= TimeWindowWidth) {
	 TimeSmoothingFlag = 0;
	 }
	 */
	//if(HAL_ADC_PollForConversion(&hadc2, 10)){
	//HAL_ADC_Start_DMA(&hadc2, (uint32_t*) ADC_Value2, sizeof(ADC_Value2) / sizeof(ADC_Value2[0]));}
}


