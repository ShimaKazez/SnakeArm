/*
 * Servo.h
 *
 *  Created on: Mar 19, 2025
 *      Author: ME_Gu
 */

#ifndef __SERVO_H
#define __SERVO_H

#include "stm32g4xx_hal.h"

// 舵机参数宏定义
#define SERVO_PWM_FREQ    50      // 频率50Hz（周期20ms）
#define SERVO_NEUTRAL_PULSE 1500  // 1.5ms
#define SERVO_MIN_PULSE   500     // 0.5ms对应CCR值（根据定时器配置计算）
#define SERVO_MAX_PULSE   2500    // 2.5ms对应CCR值
#define SERVO_TIM_HANDLE  &htim1  // 使用的定时器句柄

// 舵机通道枚举定义
typedef enum {
	SERVO_CH1 = TIM_CHANNEL_1,  // 通道1
	SERVO_CH2 = TIM_CHANNEL_2,  // 通道2
	SERVO_CH3 = TIM_CHANNEL_3,  // 通道1
	SERVO_CH4 = TIM_CHANNEL_4   // 通道2
} Servo_Channel;

// 函数声明
void Servo_Init(void);                     // 舵机初始化
void Servo_SetAngle(Servo_Channel ch, uint8_t angle);        // 设置舵机角度（0~180°）
void Servo_WriteMicroseconds(Servo_Channel ch, uint16_t us); // 直接设置脉冲宽度（微秒）
uint8_t Float_To_ServoAngle(float input);

#endif /* SERVO_H_ */
