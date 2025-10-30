/*
 * TimerCallbacks.c
 *
 *  Created on: Mar 20, 2025
 *      Author: ME_Gu
 */

#include "Filters.h"
#include "TimerCallbacks.h"
#include "UI.h"
#include "error.h"

float tension_sensor_raw[3];

int waring_flag = 0;
HOME original_Home;

void HandleDriverMonitoring() {
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
//	vofa_send_data(12, (float) system_frequency);
//	vofa_send_data(13, (float) program_mode_code);
//	vofa_send_data(14, tension_sensor_raw[0]);
//	vofa_send_data(15, tension_sensor_raw[1]);
//	vofa_send_data(16, tension_sensor_raw[2]);

	vofa_sendframetail();
	driver_monitoring_flag = 1;
}

void HandleScreenReflash() {
	//系统运行状态提示进度条
	if (screen_sequence > 90)
		screen_sequence = 0;
	if (screen_sequence >= 5)
		Paint_ClearWindows((screen_sequence - 10) * 3, 133, (screen_sequence - 9) * 3, 134, BLACK);
	if (screen_sequence <= 80) {
		if (screen_sequence >= 0 && screen_sequence <= 27) {
			c_Red = screen_sequence * 9 + 10;
			c_Green = 0;
			c_Blue = (27 - screen_sequence) * 9 + 10;
		}
		if (screen_sequence > 27 && screen_sequence <= 54) {
			c_Red = (54 - screen_sequence) * 9 + 10;
			c_Green = (screen_sequence - 28) * 9 + 10;
			c_Blue = 0;
		}
		if (screen_sequence > 54 && screen_sequence <= 80) {
			c_Red = 0;
			c_Green = (80 - screen_sequence) * 9 + 10;
			c_Blue = (screen_sequence - 55) * 9 + 10;
		}
		Paint_ClearWindows(screen_sequence * 3, 133, (screen_sequence + 1) * 3, 134, RGB888ToRGB565(c_Red, c_Green, c_Blue));
	}
	screen_sequence++;

//	Home.params[3].num1 = (float) system_frequency / 1000;
//	Home.params[3].num2 = (float) program_mode_code;
	Home.params[3].num1 = PWR_sensor[1];
	Home.params[3].num2 = PWR_sensor[0];

	Home.status[0].num1 = tension_sensor[0] / 3.3 * 300;
	Home.status[1].num1 = tension_sensor[1] / 3.3 * 300;
	Home.status[2].num1 = tension_sensor[2] / 3.3 * 300;

	if (parameters_reflash_flag) {
		Parameters_Reflash();
		parameters_reflash_flag = 0;
	}
	if (Targets_Reflash_Flag) {
		//Targets_Reflash();
		Targets_Reflash_Flag = 0;
	}
	if (Status_Reflash_Flag) {
		Status_Reflash();
		Status_Reflash_Flag = 0;
	}
}

void HandleControlThread() {
	static float Control_Data_Last[3] = { 0 };
	static uint8_t Control_Command_Last[3] = { 0 };

	ADC_Read();
	PWR_sensor[0] = (float) ADC_PWR_Value[0] / 4096 * 33;	//读取电源信息
	PWR_sensor[1] = (float) ADC_PWR_Value[1] / 4096 * 33 * 1.11;

	for (int i = 0; i < 3; i++) {
		tension_sensor_raw[i] = (float) ADC_SEN_Value[i] / 4096 * 3.3;	//读取传感器原始信息
		tension_sensor[i] = (float) ADC_SEN_Value_Kalman[i] * 3.3 / 4096;	//读取传感器信息
		Tension_Data[i] = (float) ADC_SEN_Value_Kalman[i] * 300 / 4096;	// 假设传感器输出范围为0-3.3V，对应张力0-300N，比例系数为300
	}

	if (program_mode_code != 888) {
		Sys_Timeout_Count++;
	}

	uint16_t Sys_Bitmask = 0x0000;
	for (int i = 0; i < 3; i++) {
		Sys_Bitmask |= Status_monitor(&Tension_Monitors[i], Tension_Data[i]);
		Sys_Bitmask |= Status_monitor(&Angle_Monitors[i], Angle_Data[i]);
		Sys_Bitmask |= Status_monitor(&Speed_Monitors[i], Speed_Data[i]);
		Sys_Bitmask |= Status_monitor(&Torque_Monitors[i], Torque_Data[i]);
	}
	Sys_Bitmask |= Status_monitor(&Voltage_Monitor, PWR_sensor[1]);
	Sys_Bitmask |= Status_monitor(&Current_Monitor, PWR_sensor[0]);
	Sys_Bitmask |= Status_monitor(&Timeout_Monitor, (float) Sys_Timeout_Count);
	uint16_t Error_Bitmask = Sys_Bitmask & 0xFF00;
	uint16_t Warning_Bitmask = Sys_Bitmask & 0x00FF;

	HandleError(Error_Bitmask);
	HandleWarning(Warning_Bitmask);

	static int Return_Mode = 0;
	switch (program_mode_code) {	//控制循环模式识别
	case 000:	//空闲
		parameters_reflash_flag = 1;
		break;
	case 101:	//测试程序1
		break;
	case 102:	//测试程序2
		break;
	case 103:	//测试程序3
		break;
	case 200:	//
		break;
	case 201:	//外部信号控制循环
		if (Return_Mode) {
			estop(0);
			set_zero_position_temp(0);
			for (int i = 0; i < 3; i++) {
				Control_Command[i] = 16;	//初始化为位置模式 设置位置为零
				Control_Data[i] = 0;
				set_angle(i + 1, Control_Data[i], 10, 10, 1);
			}
			Return_Mode = 0;
		}
		for (int i = 0; i < 3; i++) {
			if (Control_Data[i] != Control_Data_Last[i] || Control_Command[i] != Control_Command_Last[i]) { // 检查数据是否变化
				Control_Data_Last[i] = Control_Data[i]; // 更新记录的值
				Control_Command_Last[i] = Control_Command[i];
				switch (Control_Command[i]) {
				case 20:
					if (Control_Data[i] > 1.2 || Control_Data[i] < -1.2) { //入口限制
						estop(0);
						program_mode_code = 999;
						HandleError(1 << 9);
					}
					set_torque(i + 1, Control_Data[i], 1, 0);
					break;
				case 16:
					set_angle(i + 1, Control_Data[i], 10, 10, 1);
					break;
				case 22:
					set_speed(i + 1, Control_Data[i], 1000, 1);
					break;
				case 50:
					switch (i) {
					case 0:
						Servo_SetAngle(SERVO_CH1, Float_To_ServoAngle(Control_Data[i]));
						break;
					case 1:
						Servo_SetAngle(SERVO_CH2, Float_To_ServoAngle(Control_Data[i]));
						break;
					default:
					}
					break;
				default: //未知指令
					estop(0);
					program_mode_code = 999;
					HandleError(1 << 10);
				}
			}
		}
		break;
	case 202:					//自定义控制循环
		for (int i = 0; i < 3; i++) {
			Control_Command[i] = 20;
			Control_Data[i] = 0.15;					//预紧补偿
			set_torque(i + 1, Control_Data[i], 1, 0);
		}
		Return_Mode = 1;
		break;
	case 203:					//自定义控制
		for (int i = 0; i < 3; i++) {
			Control_Command[i] = 22;
			float actual_tension = tension_sensor[i] / 3.3f * 300.0f;					// 计算实际张力值
			float target_tension = 20.0f;					// 目标张力为20
			float dt = 0.001f;					// 时间间隔（为1ms）
			float speed = PID_Compute(&pid_tension[i], target_tension, actual_tension, dt);					// 使用PID计算速度控制值
			float speed_limit[2] = { 20.0f, -20.0f };					// 定义速度限制（mm/s）
			// 应用速度限制
			if (speed > speed_limit[0]) {
				speed = speed_limit[0];
			} else if (speed < speed_limit[1]) {
				speed = speed_limit[1];
			}
			float speed_rpm = speed * 60.0f / (3.14159f * 48.0f);
			Control_Data[i] = speed_rpm;

			set_speed(i + 1, speed_rpm, 1000, 1);			// 设置速度控制
		}
		Return_Mode = 1;
		break;
	default:
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	switch ((uint32_t) htim->Instance) {
	case (uint32_t) TIM17: // 回传基准时钟5ms
		HAL_TIM_Base_Start_IT(&htim17);
		HandleDriverMonitoring();
		break;

	case (uint32_t) TIM16: // 屏幕刷新基准时钟100ms
		HAL_TIM_Base_Start_IT(&htim16);
		if (!UI_Init_Flag) {
			HandleScreenReflash(); //UI启动时暂时终止屏幕刷新线程
		}
		break;

	case (uint32_t) TIM7: // 控制循环基准时钟1ms
		HAL_TIM_Base_Start_IT(&htim7);
		HandleControlThread();
		break;

	case (uint32_t) TIM6: // 占用率监测基准时钟1000ms
		HAL_TIM_Base_Start_IT(&htim6);
		system_frequency = system_cycle_counter;
		system_cycle_counter = 0;
		break;

	default:
		// 未知定时器实例
		NVIC_SystemReset();
		break;
	}
}
