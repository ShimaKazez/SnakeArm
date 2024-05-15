/*
 * PID.h
 *
 *  Created on: Mar 5, 2024
 *      Author: Cross
 */

#ifndef PID_H_
#define PID_H_

#include "math.h"
#include "arm_math.h"

typedef struct _pid {
	float SetSpeed;            //定义设定值
	float ActualSpeed;        //定义实际值
	float err;                //定义偏差值
	float err_last;            //定义上一个偏差值
	float Kp, Ki, Kd;            //定义比例、积分、微分系数
	float Setpower;          //定义电压值（控制执行器的变量）
	float integral;            //定义积分值
} PID;

extern volatile PID pid;

void PID_init(void);
float PID_realize(float, float);

#endif /* PID_H_ */
