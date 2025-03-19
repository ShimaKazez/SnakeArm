/*
 * Servo.c
 *
 *  Created on: Mar 19, 2025
 *      Author: ME_Gu
 */

#include "Servo.h"
#include "tim.h"  // 确保包含CubeMX生成的定时器头文件

// 初始化所有舵机通道
void Servo_Init(void) {
	HAL_TIM_PWM_Start(SERVO_TIM_HANDLE, SERVO_CH1); // 启动通道1
	HAL_TIM_PWM_Start(SERVO_TIM_HANDLE, SERVO_CH2); // 启动通道2
}

// 设置舵机角度（0~180°）
void Servo_SetAngle(Servo_Channel ch, uint8_t angle) {
	if (angle > 180)
		angle = 180;
	// 角度到脉冲宽度映射
	uint16_t pulse = SERVO_MIN_PULSE + (angle * (SERVO_MAX_PULSE - SERVO_MIN_PULSE)) / 180;
	__HAL_TIM_SET_COMPARE(SERVO_TIM_HANDLE, ch, pulse);
}

// 直接设置微秒级脉冲（0.5ms~2.5ms）
void Servo_WriteMicroseconds(Servo_Channel ch, uint16_t us) {
	if (us < 500)
		us = 500;
	else if (us > 2500)
		us = 2500;
	__HAL_TIM_SET_COMPARE(SERVO_TIM_HANDLE, ch, us);
}

uint8_t Float_To_ServoAngle(float input) {
    // 限制范围
    input = (input < 0.0f) ? 0.0f : (input > 180.0f) ? 180.0f : input;
    // 四舍五入
    return (uint8_t)(input + 0.5f);
}
