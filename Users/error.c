/*
 * error.c
 *
 *  Created on: May 12, 2025
 *      Author: ME_Gu
 */

#include "error.h"

volatile ErrorCodeUnion ErrorCode_Sys; // 16 位错误代码
char ErrorCodeStr[7]; // 用于显示错误代码的字符串
StatusMonitor Tension_Monitors[3];
StatusMonitor Angle_Monitors[3];
StatusMonitor Speed_Monitors[3];
StatusMonitor Torque_Monitors[3];
StatusMonitor Voltage_Monitor, Current_Monitor, Timeout_Monitor;

void HandleError(uint16_t error) {
	uint16_t Error_Bitmask = error & 0xFF00;
	ErrorCode_Sys.all = (ErrorCode_Sys.all & 0x00FF) | Error_Bitmask; // 设置错误位

	if (Error_Bitmask != 0x0000) { // 如果存在错误
		Buttom_Flag[0] = 0;
		Buttom_Flag[1] = 0;

		// 设置错误状态，直接显示16进制错误代码
		snprintf(ErrorCodeStr, sizeof(ErrorCodeStr), "0x%04X", ErrorCode_Sys.all);
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
		vofa_send_data(0, Angle_Data[0]);
		vofa_send_data(1, Speed_Data[0]);
		vofa_send_data(2, Torque_Data[0]);
		vofa_send_data(3, Angle_Data[1]);
		vofa_send_data(4, Speed_Data[1]);
		vofa_send_data(5, Torque_Data[1]);
		vofa_send_data(6, Angle_Data[2]);
		vofa_send_data(7, Speed_Data[2]);
		vofa_send_data(8, Torque_Data[2]);
		vofa_send_data(9, tension_sensor[0]);
		vofa_send_data(10, tension_sensor[1]);
		vofa_send_data(11, tension_sensor[2]);
		vofa_send_data(9, tension_sensor[0]);
		vofa_send_data(10, tension_sensor[1]);
		vofa_send_data(11, tension_sensor[2]);
		vofa_send_data(12, 0);
		vofa_send_data(13, (float) ErrorCode_Sys.all);
		vofa_sendframetail();

		// 阻塞线程
		while (1) {
		}
	}
}

void ClearError(uint16_t error) {
	ErrorCode_Sys.all &= ~error; // 清除特定错误位
}

void HandleWarning(uint16_t warning) {
	static uint16_t Warning_Bitmask = 0x0000;
	static uint16_t Warning_Bitmask_Last = 0x0000;

	Warning_Bitmask = warning & 0x00FF;
	ErrorCode_Sys.all = (ErrorCode_Sys.all & 0xFF00) | Warning_Bitmask; // 设置警告位

	if (Buttom_Pushed_Flag[0] == 0 && Buttom_Pushed_Flag[1] == 0 && Home.mode.Color != YELLOW && Warning_Bitmask != 0) {
		snprintf(ErrorCodeStr, sizeof(ErrorCodeStr), "0x%04X", ErrorCode_Sys.all);
		Status_Set(0, 0, ErrorCodeStr, YELLOW);
	}

	if (Warning_Bitmask != Warning_Bitmask_Last) {
		if (Warning_Bitmask != 0x0000) { // 如果警告状态刷新且不为零，设置警告状态，直接显示16进制警告代码
			snprintf(ErrorCodeStr, sizeof(ErrorCodeStr), "0x%04X", ErrorCode_Sys.all);
			Status_Set(0, 0, ErrorCodeStr, YELLOW);
			HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, SET);
		} else { // 所有警告清除
			for (int i = 0; i < 4; i++) {
				if (L1_menu_items[i].program_mode_code == program_mode_code) {
					Status_Set(L1_menu_items[i].flag_label, L1_menu_items[i].flag_color, "Normal", GREEN);
					break;
				} else if (L2_menu_items[i].program_mode_code == program_mode_code) {
					Status_Set(L2_menu_items[i].flag_label, L2_menu_items[i].flag_color, "Normal", GREEN);
					break;
				}
			}
			if (program_mode_code == 000) {
				Status_Set("[READY]", GREEN, "Normal", GREEN);
			} else if (program_mode_code == 200) {
				Status_Set(0, 0, "ZeroNone", WHITE);
			}
			HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, RESET);
		}
	}
	Warning_Bitmask_Last = Warning_Bitmask;
}

/**
 * @brief 更新变量监控状态
 * @param monitor 监控器结构体指针
 */
uint16_t Status_monitor(StatusMonitor *monitor, float value) {
	monitor->variable = value;
	uint16_t bitmask = 0x0000;

// 检查值域范围
	if (value < monitor->error_low_limit || value > monitor->error_high_limit) {
		monitor->error_counter++;
		monitor->warn_counter = 0; // 错误时清除警告计数
	} else if (value < monitor->warn_low_limit || value > monitor->warn_high_limit) {
		monitor->warn_counter++;
	} else {
		// 值在正常范围内，清除计数器
		monitor->error_counter = 0;
		monitor->warn_counter = 0;
	}

// 处理错误和警告触发
	if (monitor->error_counter > COUNTER_LIMIT) {
		bitmask |= monitor->error_bitmask;
	} else if (monitor->warn_counter > COUNTER_LIMIT) {
		bitmask |= monitor->warn_bitmask;
	} else {
		bitmask = 0x0000;
	}

	return bitmask;
}

void Stauts_monitor_init(StatusMonitor *monitor, float variable, float err_low, float err_high, float warn_low, float warn_high, uint32_t err_mask, uint32_t warn_mask) {
	monitor->variable = variable;
	monitor->error_low_limit = err_low;
	monitor->error_high_limit = err_high;
	monitor->warn_low_limit = warn_low;
	monitor->warn_high_limit = warn_high;
	monitor->error_bitmask = err_mask;
	monitor->warn_bitmask = warn_mask;
	monitor->error_counter = 0;
	monitor->warn_counter = 0;
}

void Status_monitor_init_all() {
// 定义限制值数组：{错误下限, 警告下限, 警告上限, 错误上限}
	const float tension_limits[4] = { -20.0f, 3.0f, 150.0f, 180.0f };
	const float angle_limits[4] = { -150.0f, -90.0f, 90.0f, 150.0f };
	const float speed_limits[4] = { -10.0f, -5.0f, 5.0f, 10.0f };
	const float torque_limits[4] = { -1.2f, -0.8f, 0.8f, 1.2f };
	const float voltage_limits[4] = { 0.0f, 22.0f, 25.2f, 28.0f };
	const float current_limits[4] = { 0.0f, 0.0f, 6.0f, 8.0f };
	const float timeout_limits[4] = { -2.0f, -1.0f, 300.0f, 1000.0f };

// 初始化各个监控器
	Stauts_monitor_init(&Tension_Monitors[0], 0, tension_limits[0], tension_limits[3], tension_limits[1], tension_limits[2], 1 << 12, 1 << 4);
	Stauts_monitor_init(&Tension_Monitors[1], 0, tension_limits[0], tension_limits[3], tension_limits[1], tension_limits[2], 1 << 12, 1 << 4);
	Stauts_monitor_init(&Tension_Monitors[2], 0, tension_limits[0], tension_limits[3], tension_limits[1], tension_limits[2], 1 << 12, 1 << 4);

	Stauts_monitor_init(&Angle_Monitors[0], 0, angle_limits[0], angle_limits[3], angle_limits[1], angle_limits[2], 1 << 13, 1 << 5);
	Stauts_monitor_init(&Angle_Monitors[1], 0, angle_limits[0], angle_limits[3], angle_limits[1], angle_limits[2], 1 << 13, 1 << 5);
	Stauts_monitor_init(&Angle_Monitors[2], 0, angle_limits[0], angle_limits[3], angle_limits[1], angle_limits[2], 1 << 13, 1 << 5);

	Stauts_monitor_init(&Speed_Monitors[0], 0, speed_limits[0], speed_limits[3], speed_limits[1], speed_limits[2], 1 << 14, 1 << 6);
	Stauts_monitor_init(&Speed_Monitors[1], 0, speed_limits[0], speed_limits[3], speed_limits[1], speed_limits[2], 1 << 14, 1 << 6);
	Stauts_monitor_init(&Speed_Monitors[2], 0, speed_limits[0], speed_limits[3], speed_limits[1], speed_limits[2], 1 << 14, 1 << 6);

	Stauts_monitor_init(&Torque_Monitors[0], 0, torque_limits[0], torque_limits[3], torque_limits[1], torque_limits[2], 1 << 15, 1 << 7);
	Stauts_monitor_init(&Torque_Monitors[1], 0, torque_limits[0], torque_limits[3], torque_limits[1], torque_limits[2], 1 << 15, 1 << 7);
	Stauts_monitor_init(&Torque_Monitors[2], 0, torque_limits[0], torque_limits[3], torque_limits[1], torque_limits[2], 1 << 15, 1 << 7);

	Stauts_monitor_init(&Voltage_Monitor, 0, voltage_limits[0], voltage_limits[3], voltage_limits[1], voltage_limits[2], 1 << 0, 1 << 0);
	Stauts_monitor_init(&Current_Monitor, 0, current_limits[0], current_limits[3], current_limits[1], current_limits[2], 1 << 1, 1 << 1);
	Stauts_monitor_init(&Timeout_Monitor, 0, timeout_limits[0], timeout_limits[3], timeout_limits[1], timeout_limits[2], 1 << 9, 1 << 2);
}

