/*
 * error.c
 *
 *  Created on: May 12, 2025
 *      Author: ME_Gu
 */

#include "error.h"
#include "TimerCallbacks.h"
#include "ADC_Sample.h"

volatile ErrorCodeUnion ErrorCode_Sys, ErrorCode_Last; // 16 位错误代码
char ErrorCodeStr[7]; // 用于显示错误代码的字符串
StatusMonitor Tension_Monitors[3];
StatusMonitor Angle_Monitors[3];
StatusMonitor Speed_Monitors[3];
StatusMonitor Torque_Monitors[3];
MonitorGroup_t tension_monitors_group = {
		.monitors = (StatusMonitor*[] ) { &Tension_Monitors[0], &Tension_Monitors[1], &Tension_Monitors[2] },
		.monitor_count = 3,
		.shared_error_mask = (1 << 12),
		.shared_warn_mask = (1 << 4) };
MonitorGroup_t angle_monitors_group = {
		.monitors = (StatusMonitor*[] ) { &Angle_Monitors[0], &Angle_Monitors[1], &Angle_Monitors[2] },
		.monitor_count = 3, .shared_error_mask = (1 << 13),
		.shared_warn_mask =
		(1 << 5) };
MonitorGroup_t speed_monitors_group = {
		.monitors = (StatusMonitor*[] ) { &Speed_Monitors[0], &Speed_Monitors[1], &Speed_Monitors[2] },
		.monitor_count = 3, .shared_error_mask = (1 << 14),
		.shared_warn_mask =
		(1 << 6) };
MonitorGroup_t torque_monitors_group = {
		.monitors = (StatusMonitor*[] ) { &Torque_Monitors[0], &Torque_Monitors[1], &Torque_Monitors[2] },
		.monitor_count = 3,
		.shared_error_mask = (1 << 15),
		.shared_warn_mask = (1 << 7) };

void HandleError(uint16_t error) {
	ErrorCode_Sys.all |= error; // 设置错误位
	if (ErrorCode_Sys.all != 0) { // 如果存在错误
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

static int warning_flag = 0;

void HandleWarning(uint16_t warning) {
	ErrorCode_Sys.all |= warning; // 设置警告位
	warning_flag = 1;
	if (ErrorCode_Sys.all != 0 && ErrorCode_Sys.all != ErrorCode_Last.all) { // 如果存在警告且与上次不同
	// 设置警告状态，直接显示16进制警告代码
		snprintf(ErrorCodeStr, sizeof(ErrorCodeStr), "0x%04X", ErrorCode_Sys.all);
		Status_Set("WARNING", YELLOW, ErrorCodeStr, YELLOW);
		ErrorCode_Last = ErrorCode_Sys; // 保存当前错误代码
		StStus_Reflash_Flag = 1;
	}
}

void ClearWarning(uint16_t warning) {
	ErrorCode_Sys.all &= ~warning; // 清除特定警告位
	if (ErrorCode_Sys.all == 0 && warning_flag == 1) { // 所有警告清除且之前有警告
		switch (program_mode_code) {	//控制循环模式识别
		case 000:	//空闲
			Status_Set("[READY]", GREEN, "Idling", WHITE);
			break;
		case 101:	//测试程序1
			Status_Set("TEST", CYAN, "TestPrg1", LIGHTBLUE);
			break;
		case 102:	//测试程序2
			Status_Set("TEST", CYAN, "TestPrg2", LIGHTBLUE);
			break;
		case 103:	//测试程序3
			Status_Set("TEST", CYAN, "TestPrg3", LIGHTBLUE);
			break;
		case 200:	//外部信号驱动
			Status_Set("ONBOARD", GREEN, "ZeroNone", YELLOW);
			break;
		case 201:	//外部信号控制循环
			Status_Set("ONBOARD", GREEN, "ZeroSetted", YELLOW);
			break;
		case 202:					//自定义控制循环
			Status_Set("CUSTOM", YELLOW, "Tighten", LIGHTBLUE);
			break;
		case 203:					//自定义控制
			Status_Set("CUSTOM", YELLOW, "Enforce", LIGHTBLUE);
			break;
		default:
		}
		StStus_Reflash_Flag = 1;
		warning_flag = 0;
	}
}

void CheckAndHandleErrors(void) {
	// 电压和电流监测
	float PWR_Voltage = (float) ADC_PWR_Value[1] / 4096 * 33 * 1.12;
	float PWR_Current = (float) ADC_PWR_Value[0] / 4096 * 33;

	if (PWR_Voltage < MIN_VOLTAGE || PWR_Voltage > MAX_VOLTAGE) {
		HandleWarning(1 << 0); // 电压警告（位2）
	} else {
		ClearWarning(1 << 0); // 清除电压警告
	}

	if (PWR_Current > MAX_CURRENT) {
		HandleWarning(1 << 1); // 电流警告（位3）
	} else {
		ClearWarning(1 << 1); // 清除电流警告
	}

	// 超时监测
	if (system_timeout_flag == 1) {
		HandleWarning(1 << 2); // 超时警告（位1）
	} else if (system_timeout_flag > 2 && program_mode_code != 888) {
		HandleError(1 << 10); // 超时错误（位8）
	} else if (system_timeout_flag > 8 && program_mode_code == 888) {
		HandleError(1 << 9); // 归零超时错误（位11）
	} else {
		ClearWarning(1 << 2); // 清除超时警告
	}

}

/**
 * @brief 更新变量监控状态
 * @param monitor 监控器结构体指针
 */

void Status_monitor_update(StatusMonitor* monitor,float value){
	monitor->variable=value;
}

void MonitorGroup_Update(MonitorGroup_t *group) {
    uint8_t error_triggered = 0;
    uint8_t warn_triggered = 0;

    // 检查组内所有监控器
    for (int i = 0; i < group->monitor_count; i++) {
        StatusMonitor *monitor = group->monitors[i];

        // 更新单个监控器状态（但不直接处理错误）
        if (monitor->variable < monitor->error_low_limit ||
            monitor->variable > monitor->error_high_limit) {
            monitor->error_counter++;
            monitor->warn_counter = 0;

            if (monitor->error_counter > COUNTER_LIMIT) {
                error_triggered = 1;
            }
        } else if (monitor->variable < monitor->warn_low_limit ||
                   monitor->variable > monitor->warn_high_limit) {
            monitor->warn_counter++;

            if (monitor->warn_counter > COUNTER_LIMIT) {
                warn_triggered = 1;
            }
        } else {
            monitor->error_counter = 0;
            monitor->warn_counter = 0;
        }
    }

    // 组内统一处理：任何一个监控器触发就设置整个组的错误
    if (error_triggered) {
        HandleError(group->shared_error_mask);
    } else {
        // 只有所有监控器都正常时才清除错误
        uint8_t all_normal = 1;
        for (int i = 0; i < group->monitor_count; i++) {
            if (group->monitors[i]->error_counter > 0) {
                all_normal = 0;
                break;
            }
        }
        if (all_normal) {
            ClearError(group->shared_error_mask);
        }
    }

    // 类似处理警告
    if (warn_triggered) {
        HandleWarning(group->shared_warn_mask);
    } else {
        uint8_t all_normal_warn = 1;
        for (int i = 0; i < group->monitor_count; i++) {
            if (group->monitors[i]->warn_counter > 0) {
                all_normal_warn = 0;
                break;
            }
        }
        if (all_normal_warn) {
            ClearWarning(group->shared_warn_mask);
        }
    }
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
	const float tension_limits[4] = { 0.0f, 5.0f, 100.0f, 200.0f };
	const float angle_limits[4] = { -150.0f, -90.0f, 90.0f, 150.0f };
	const float speed_limits[4] = { -60.0f, -30.0f, 30.0f, 60.0f };
	const float torque_limits[4] = { -1.2f, -0.8f, 0.8f, 1.2f };

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
}

