/*
 * PID.h
 *
 *  Created on: Mar 20, 2025
 *      Author: ME_Gu
 */

#ifndef PID_H_
#define PID_H_

#include <stdint.h>

// PID structure
typedef struct {
	float Kp;  // Proportional coefficient
	float Ki;  // Integral coefficient
	float Kd;  // Derivative coefficient
	float prev_error;  // Previous error
	float integral;  // Integral term
} PID_Controller;

// Function prototypes
void PID_Init(PID_Controller *pid, float Kp, float Ki, float Kd);
float PID_Compute(PID_Controller *pid, float setpoint, float tension, float dt);
void PID_Module_Init(void);
extern PID_Controller pid_tension[3];

#endif /* PID_H_ */

