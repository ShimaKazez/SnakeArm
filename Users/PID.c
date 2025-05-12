/*
 * PID.c
 *
 *  Created on: Mar 20, 2025
 *      Author: ME_Gu
 */

#include "PID.h"

// Initialize PID controller
void PID_Init(PID_Controller *pid, float Kp, float Ki, float Kd) {
	pid->Kp = Kp;
	pid->Ki = Ki;
	pid->Kd = Kd;
	pid->prev_error = 0.0f;
	pid->integral = 0.0f;
}

// Compute PID output
float PID_Compute(PID_Controller *pid, float setpoint, float tension, float dt) {
	float error = setpoint - tension;
	pid->integral += error * dt;
	float derivative = (error - pid->prev_error) / dt;
	float output = pid->Kp * error + pid->Ki * pid->integral + pid->Kd * derivative;
	pid->prev_error = error;
	return output;
}

