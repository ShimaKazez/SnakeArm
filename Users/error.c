/*
 * error.c
 *
 *  Created on: May 12, 2025
 *      Author: ME_Gu
 */

#include "error.h"
#include "TimerCallbacks.h"

extern volatile uint16_t ADC_PWR_Value[2];
extern volatile uint16_t ADC_SEN_Value_Kalman[4];

volatile ErrorCodeUnion ErrorCode; // 16 位错误代码
char ErrorCodeStr[7]; // 用于显示错误代码的字符串

void HandleError(uint16_t error) {
	ErrorCode.all |= error; // 设置错误位
	if (ErrorCode.all != 0) { // 如果存在错误
		program_group_flag[0] = 0;
		program_group_flag[1] = 0;

		// 设置错误状态，直接显示16进制错误代码
		snprintf(ErrorCodeStr, sizeof(ErrorCodeStr), "0x%04X", ErrorCode.all);
		Status_Set("ERROR", RED, ErrorCodeStr, YELLOW);

		Parameters_Reflash();
		Status_Reflash();

		// 停止所有定时器
		HAL_TIM_Base_Stop_IT(&htim7);
		HAL_TIM_Base_Stop_IT(&htim6);
		HAL_TIM_Base_Stop_IT(&htim16);
		HAL_TIM_Base_Stop_IT(&htim17);

		// 停止系统
		estop(0);

		// 发送错误数据
		vofa_send_data(0, Drivers.driver1.angle);
		vofa_send_data(1, Drivers.driver1.speed);
		vofa_send_data(2, Drivers.driver1.torque);
		vofa_send_data(3, Drivers.driver2.angle);
		vofa_send_data(4, Drivers.driver2.speed);
		vofa_send_data(5, Drivers.driver2.torque);
		vofa_send_data(6, Drivers.driver3.angle);
		vofa_send_data(7, Drivers.driver3.speed);
		vofa_send_data(8, Drivers.driver3.torque);
		vofa_send_data(9, tension_sensor[0]);
		vofa_send_data(10, tension_sensor[1]);
		vofa_send_data(11, tension_sensor[2]);
		vofa_send_data(12, 0);
		vofa_send_data(13, (float) ErrorCode.all);
		vofa_sendframetail();

		// 阻塞线程
		while (1) {
		}
	}
}

void ClearError(uint16_t error) {
	ErrorCode.all &= ~error; // 清除特定错误位
}

static int warning_flag = 0;

void HandleWarning(uint16_t warning) {
	ErrorCode.all |= warning; // 设置警告位
	if (ErrorCode.all != 0) {
		// 设置警告状态，直接显示16进制警告代码
		snprintf(ErrorCodeStr, sizeof(ErrorCodeStr), "0x%04X", ErrorCode.all);
		Status_Set("WARNING", YELLOW, ErrorCodeStr, YELLOW);
		warning_flag = 1;
	}
	Parameters_Reflash();
	Status_Reflash();
}

void ClearWarning(uint16_t warning) {
	ErrorCode.all &= ~warning; // 清除特定警告位
	if (ErrorCode.all == 0 && warning_flag == 1) { // 所有警告清除且之前有警告
		switch (program_mode_code) {	//控制循环模式识别
				case 000:	//空闲
					Status_Set("[READY]", GREEN, "Idling", WHITE);
					break;
				case 101:	//测试程序1
					Status_Set("[TEST]", CYAN, "TestPrg1", LIGHTBLUE);
					break;
				case 102:	//测试程序2
					Status_Set("[TEST]", CYAN, "TestPrg2", LIGHTBLUE);
					break;
				case 103:	//测试程序3
					Status_Set("[TEST]", CYAN, "TestPrg3", LIGHTBLUE);
					break;
				case 200:	//外部信号驱动
					Status_Set("ONBOARD", GREEN, "ZeroNone", YELLOW);
					break;
				case 201:	//外部信号控制循环
					Status_Set("ONBOARD", GREEN, "ZeroSetted", YELLOW);
					break;
				case 202:					//自定义控制循环
					Status_Set("OFFBOARD", YELLOW, "Tighten", LIGHTBLUE);
					break;
				case 203:					//自定义控制
					Status_Set("OFFBOARD", YELLOW, "Enforce", LIGHTBLUE);
					break;
				default:
				}
		Parameters_Reflash();
		Status_Reflash();
		warning_flag = 0;
	}
}

void CheckAndHandleErrors(void) {
	// 电压和电流监测
	float PWR_Voltage = (float) ADC_PWR_Value[1] / 4096 * 33 * 1.12;
	float PWR_Current = (float) ADC_PWR_Value[0] / 4096 * 33;

	if (PWR_Voltage < MIN_VOLTAGE || PWR_Voltage > MAX_VOLTAGE) {
		HandleWarning(1 << 2); // 电压警告（位2）
	} else {
		ClearWarning(1 << 2); // 清除电压警告
	}

	if (PWR_Current > MAX_CURRENT) {
		HandleWarning(1 << 3); // 电流警告（位3）
	} else {
		ClearWarning(1 << 3); // 清除电流警告
	}

	// 张力监测
	float tension1 = (float) ADC_SEN_Value_Kalman[0] * 300 / 4096;// 假设传感器输出范围为0-3.3V，对应张力0-300N，比例系数为300
	float tension2 = (float) ADC_SEN_Value_Kalman[1] * 300 / 4096;
	float tension3 = (float) ADC_SEN_Value_Kalman[2] * 300 / 4096;

	if (tension1 < MIN_TENSION || tension1 > MAX_TENSION || tension2 < MIN_TENSION || tension2 > MAX_TENSION
			|| tension3 < MIN_TENSION || tension3 > MAX_TENSION) {
		HandleWarning(1 << 0); // 张力警告（位0）
	} else {
		ClearWarning(1 << 0); // 清除张力警告
	}

	// 超时监测
	if (system_timeout_flag == 1) {
		HandleWarning(1 << 1); // 超时警告（位1）
	} else if (system_timeout_flag > 2 && program_mode_code != 888) {
		HandleError(1 << 8); // 超时错误（位8）
	} else if (system_timeout_flag > 8 && program_mode_code == 888) {
		HandleError(1 << 11); // 归零超时错误（位11）
	} else {
		ClearWarning(1 << 1); // 清除超时警告
	}

}

