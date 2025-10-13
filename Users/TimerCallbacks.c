/*
 * TimerCallbacks.c
 *
 *  Created on: Mar 20, 2025
 *      Author: ME_Gu
 */

#include "TimerCallbacks.h"
#include "UI.h"
#include "error.h"

extern volatile HOME Home;
extern volatile DriverS Drivers;
extern volatile float tension_sensor[3];
extern volatile float PWR_sensor[2];
extern volatile int system_cycle_counter;
extern volatile int system_timeout_flag;
extern int system_frequency;
extern int screen_sequence;
extern volatile int UI_Init_Flag;
extern uint8_t c_Red, c_Green, c_Blue;
extern volatile int program_group_flag[3];
extern volatile int parameters_reflash_flag, targets_reflash_flag, status_reflash_flag;
extern volatile int driver_monitoring_flag;
extern volatile int program_mode_code;
extern PID_Controller pid_tension_1, pid_tension_2, pid_tension_3;

int waring_flag = 0;
HOME original_Home;

void HandleDriverMonitoring() {
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
	vofa_send_data(12, (float) system_frequency);
	vofa_send_data(13, (float) program_mode_code);
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
	if (targets_reflash_flag) {
		//Targets_Reflash();
		targets_reflash_flag = 0;
	}
	if (status_reflash_flag) {
		Status_Reflash();
		status_reflash_flag = 0;
	}
}

void HandleControlThread() {
	static float last_offboard_data[3] = { 0 };
	ADC_Read();
	PWR_sensor[0] = (float) ADC_PWR_Value[0] / 4096 * 33;	//读取电源信息
	PWR_sensor[1] = (float) ADC_PWR_Value[1] / 4096 * 33 * 1.12;

//		tension_sensor[0] = (float) ADC_value_2[0] / 4096 * 3.3;	//读取传感器信息
//		tension_sensor[1] = (float) ADC_value_2[1] / 4096 * 3.3;
//		tension_sensor[2] = (float) ADC_value_2[2] / 4096 * 3.3;
	tension_sensor[0] = (float) ADC_SEN_Value_Kalman[0] * 3.3 / 4096;	//读取传感器信息
	tension_sensor[1] = (float) ADC_SEN_Value_Kalman[1] * 3.3 / 4096;
	tension_sensor[2] = (float) ADC_SEN_Value_Kalman[2] * 3.3 / 4096;

	//	if (tension_sensor[0] < 0.02 || tension_sensor[1] < 0.02 || tension_sensor[2] < 0.02) {
	//		HandleWarning(WARNING_TENSION_OUT);
	//	} else {
	//		HandleWarning(ERROR_NONE);
	//	}

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
	case 201:	//外部信号控制循环
		for (int i = 0; i < 3; i++) {
			if (offboard_data[i] != last_offboard_data[i]) { // 检查数据是否变化
				last_offboard_data[i] = offboard_data[i]; // 更新记录的值
				switch (offboard_command[i]) {
				case 20:
					if (offboard_data[i] > 1.2 || offboard_data[i] < -1.2) { //入口限制
						estop(0);
						program_mode_code = 999;
						HandleError(1 << 9);
					}
					set_torque(i + 1, offboard_data[i], 1, 0);
					break;
				case 16:
					set_angle(i + 1, offboard_data[i], 10, 10, 1);
					break;
				case 22:
					set_speed(i + 1, offboard_data[i], 1000, 1);
					break;
				case 50:
					switch (i) {
					case 0:
						Servo_SetAngle(SERVO_CH1, Float_To_ServoAngle(offboard_data[i]));
						break;
					case 1:
						Servo_SetAngle(SERVO_CH2, Float_To_ServoAngle(offboard_data[i]));
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
//		for (int i = 0; i < 3; i++) {
//			set_torque(i + 1, 0.15, 1, 0);					//预紧
//		}
		break;
	case 203:					//自定义控制
//		for (int i = 0; i < 3; i++) {
//			if (tension_sensor[i] < 0.1) {
//				set_torque(i + 1, 0.15, 1, 0);					//预紧
//			} else {
//				set_speed(i + 1, PID_Compute(&pid_tension_1, offboard_data[i], tension_sensor[i], 0.001f), 1000, 1);					//预紧
//			}
//		}
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

	case (uint32_t) TIM6: // 占用率错误监测基准时钟1000ms
		HAL_TIM_Base_Start_IT(&htim6);
		CheckAndHandleErrors();
		system_frequency = system_cycle_counter;
		system_cycle_counter = 0;
		system_timeout_flag++;
		break;

	default:
		// 未知定时器实例
		NVIC_SystemReset();
		break;
	}
}
