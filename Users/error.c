/*
 * error.c
 *
 *  Created on: May 12, 2025
 *      Author: ME_Gu
 */

#include "error.h"
#include "TimerCallbacks.h"

void HandleError(ErrorCode code) {
    program_group_flag[0] = 0;
    program_group_flag[1] = 0;

    // 设置错误状态
    switch (code) {
    case ERROR_TIMEOUT:
        Status_Set("ERROR", RED, "Timeout", YELLOW);
        break;
    case ERROR_TORQUE_OUT:
        Status_Set("ERROR", RED, "TorqueOut", YELLOW);
        break;
    case ERROR_SIGNAL_LOST:
        Status_Set("ERROR", RED, "SignalLost", YELLOW);
        break;
    case ERROR_ZERO_OUT:
        Status_Set("ERROR", RED, "ZeroOut", YELLOW);
        break;
    default:
        Status_Set("ERROR", RED, "Unknown", RED);
        break;
    }

    Home.params[3].num2 = (float)code;
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
    vofa_send_data(13, (float)code);
    vofa_sendframetail();

    // 阻塞线程
    while (1) {
    }
}


void HandleWarning(ErrorCode code) {
	static HOME original_Home;
	static int warning_active = 0;

	if (code == WARNING_TENSION_OUT && !warning_active) {
		original_Home = Home; // 保存当前状态
		Status_Set(0, 0, "TensionOut", YELLOW);
		warning_active = 1;

	} else if (code == WARNING_TIME_OUT && !warning_active) {
		original_Home = Home; // 保存当前状态
		Status_Set(0, 0, "TimeOut", YELLOW);
		Home.params[3].Color = YELLOW; // 高亮提醒
		warning_active = 1;

	} else if (code == ERROR_NONE && warning_active) {
		Home = original_Home; // 恢复原始状态
		Home.params[3].Color = WHITE; // 恢复为原始颜色
		status_reflash_flag = 1;
		warning_active = 0;
	}
}

void CheckAndHandleErrors(void) {
	if (system_timeout_flag == 1) {
		HandleWarning(WARNING_TIME_OUT); // 汇报超时警告
	} else if (system_timeout_flag > 2 && program_mode_code != 888) {
		HandleError(ERROR_TIMEOUT); // 汇报错误并阻塞
	} else if (system_timeout_flag > 8 && program_mode_code == 888) {
		HandleError(ERROR_ZERO_OUT); // 汇报归零超时错误并阻塞
	} else {
		HandleWarning(ERROR_NONE); // 清除警告
	}

}

