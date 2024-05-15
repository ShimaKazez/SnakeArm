/*
 * PID.c
 *
 *  Created on: Mar 5, 2024
 *      Author: Cross
 */
#include "PID.h"

volatile PID pid;

float PID_realize(float Sspeed, float Aspeed) {
	pid.Kp = 0.2;
	pid.Ki = 0.02;
	pid.Kd = 0.0;
	pid.SetSpeed = Sspeed;
	pid.ActualSpeed = Aspeed;
	pid.err = pid.SetSpeed - pid.ActualSpeed;
	pid.integral += pid.err;
	pid.Setpower = pid.Kp * pid.err + pid.Ki * pid.integral + pid.Kd * (pid.err - pid.err_last);
	pid.err_last = pid.err;
	if (pid.Setpower > 3000)
		pid.Setpower = 3000;
	if (pid.Setpower < 0)
		pid.Setpower = 0;
	return pid.Setpower;
}
