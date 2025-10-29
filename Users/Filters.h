/*
 * Filters.h
 *
 *  Created on: Mar 26, 2025
 *      Author: ME_Gu
 */

#ifndef Filters_H
#define Filters_H

#include "main.h"

typedef struct {
	float Last_P; //上次估算协方差 不可以为0 ! ! ! ! !
	float Now_P; //当前估算协方差
	float out; //卡尔曼滤波器输出
	float Kg; //卡尔曼增益
	float Q; //过程噪声协方差
	float R; //观测噪声协方差
} Kalman;

void Kalman_Init_All(void);
float KalmanFilter(Kalman *kfp, float input);
extern Kalman KFP_SEN_1, KFP_SEN_2, KFP_SEN_3, KFP_SEN_4;

#endif /* FILTERS_H_ */
