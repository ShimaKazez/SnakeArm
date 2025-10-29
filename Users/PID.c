/*
 * PID.c
 *
 *  Created on: Mar 20, 2025
 *      Author: ME_Gu
 */

#include "PID.h"

PID_Controller pid_tension[3];

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

void PID_Module_Init(void) {
	for (int i = 0; i < 3; i++) {
		PID_Init(&pid_tension[i], 0.5f, 0.0f, 0.0f);
	}
}
