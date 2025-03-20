/*
 * UI.c
 *
 *  Created on: Feb 28, 2024
 *      Author: Cross
 */

#include "UI.h"
#include "TimerCallbacks.h"

volatile HOME Home;
volatile DriverS Drivers;
volatile float tension_sensor[3];
//volatile float SystemOccupancy;
volatile int system_cycle_counter, error_code;
volatile int system_timeout_flag;
int system_frequency;
int key_push_flag[4];
int key_push_flag_last[4];
int key_reflash_flag[4];
int screen_sequence;
int parameters_reflash_counter, target_reflash_counter, status_reflash_counter;
//int TimerLast;
uint8_t c_Red, c_Green, c_Blue;
volatile int program_group_flag[3];
volatile int parameters_reflash_flag, targets_reflash_flag, status_reflash_flag;
volatile int driver_monitoring_flag;
volatile int program_mode_code;
PID_Controller pid_tension_1, pid_tension_2, pid_tension_3;

int numN(double num) {
	int offset = 0;
	if (num < 0) {
		offset = 1;
	} else {
		offset = 0;
	}
	return offset;
}

int numlen(double num) {
	int len = 1;
	while ((int) (num /= 10)) {
		len++;
	}
	return len + numN(num);
}

void UI_Startup(void) {
	float X, Y, R;
	Paint_ClearWindows(0, 0, 239, 134, BLACK);
	for (int i = 0; i < 100; i += 2) {
		X = 120;
		Y = 135 - (10000 - ((100 - i) * (100 - i))) * 67 / 10000;
		R = 5;
		Paint_DrawCircle(X, Y, R, GBLUE, 1, 0);
		HAL_Delay(5);
		Paint_DrawCircle(X, Y, R, BLACK, 1, 0);
	}
	HAL_Delay(200);
	for (int i = 0; i < 12; i += 2) {
		X = 119;
		Y = 67;
		R = i * i + 5;
		Paint_DrawCircle(X, Y, R, RGB888ToRGB565(rand() % 255, rand() % 255, rand() % 255), 1, 0);
		HAL_Delay(5);
		Paint_DrawCircle(X, Y, R, BLACK, 1, 0);
	}
	for (int i = 0; i < 20; i++) {
		X = rand() % 239;
		Y = rand() % 134;
		R = rand() % 50;
		Paint_DrawCircle(X, Y, R, RGB888ToRGB565(rand() % 255, rand() % 255, rand() % 255), 1, 0);
		HAL_Delay(5);
	}
	Paint_ClearWindows(39, 44, 200, 90, BLACK);
	Paint_DrawString_EN(43, 49, "Snake Arm", &Font24, BLACK, GBLUE);
	Paint_DrawString_EN(64, 73, "Ver.202502", &Font16, BLACK, GBLUE);
	HAL_Delay(500);
	Paint_ClearWindows(0, 0, 239, 134, BLACK);
}

void Parameters_Reflash(void) {
	parameters_reflash_counter++;
	switch (parameters_reflash_counter) {
	case 1:
		Paint_DrawFloatNum(154, 7, Home.status[0].num1, 6 - numlen(Home.status[0].num1), &Font16, BLACK, Home.status[0].Color);
		break;
	case 2:
		Paint_DrawFloatNum(154, 26, Home.status[1].num1, 6 - numlen(Home.status[1].num1), &Font16, BLACK, Home.status[1].Color);
		break;
	case 3:
		Paint_DrawFloatNum(154, 45, Home.status[2].num1, 6 - numlen(Home.status[2].num1), &Font16, BLACK, Home.status[2].Color);
		break;
	case 4:
		Paint_DrawFloatNum(52, 63, Home.params[0].num1, 5 - numlen(Home.params[0].num1), &Font16, BLACK, Home.status[0].Color);
		break;
	case 5:
		Paint_DrawFloatNum(52, 81, Home.params[1].num1, 5 - numlen(Home.params[1].num1), &Font16, BLACK, Home.params[1].Color);
		break;
	case 6:
		Paint_DrawFloatNum(52, 99, Home.params[2].num1, 5 - numlen(Home.params[2].num1), &Font16, BLACK, Home.params[2].Color);
		break;
	case 7:
		Paint_DrawFloatNum(52, 117, Home.params[3].num1, 5 - numlen(Home.params[3].num1), &Font16, BLACK, Home.params[3].Color);
		break;
	case 8:
		Paint_DrawFloatNum(165, 63, Home.params[0].num2, 5 - numlen(Home.params[0].num2), &Font16, BLACK, Home.status[0].Color);
		break;
	case 9:
		Paint_DrawFloatNum(165, 81, Home.params[1].num2, 5 - numlen(Home.params[1].num2), &Font16, BLACK, Home.params[1].Color);
		break;
	case 10:
		Paint_DrawFloatNum(165, 99, Home.params[2].num2, 5 - numlen(Home.params[2].num2), &Font16, BLACK, Home.params[2].Color);
		break;
	case 11:
		Paint_DrawFloatNum(165, 117, Home.params[3].num2, 5 - numlen(Home.params[3].num2), &Font16, BLACK, Home.params[3].Color);
		break;
	default:
		parameters_reflash_counter = 0;
	}
}

void Targets_Reflash(void) {
	Paint_DrawFloatNum(165, 63, Home.params[0].num2, 5 - numlen(Home.params[0].num2), &Font16, BLACK, Home.status[0].Color);
	Paint_DrawFloatNum(165, 81, Home.params[1].num2, 5 - numlen(Home.params[1].num2), &Font16, BLACK, Home.params[1].Color);
	Paint_DrawFloatNum(165, 99, Home.params[2].num2, 5 - numlen(Home.params[2].num2), &Font16, BLACK, Home.params[2].Color);
	Paint_DrawFloatNum(165, 117, Home.params[3].num2, 5 - numlen(Home.params[3].num2), &Font16, BLACK, Home.params[3].Color);
}

void Status_Reflash(void) {
	Paint_ClearWindows(0, 12, 120, 36, BLACK);
	Paint_DrawString_EN((120 - strlen((char*) Home.flag.Label) * 17) / 2 + 2, 12, (char*) Home.flag.Label, &Font24, BLACK, Home.flag.Color);
	Paint_ClearWindows(0, 40, 120, 56, BLACK);
	Paint_DrawString_EN((120 - strlen((char*) Home.mode.Label) * 11) / 2 + 2, 40, (char*) Home.mode.Label, &Font16, BLACK, Home.mode.Color);
}

void Status_Set(char *flag_label, uint16_t flag_color, char *mode_label, uint16_t mode_color) {
	if (flag_label != 0) {
		Home.flag.Label = flag_label;
		Home.flag.Color = flag_color;
	}
	if (mode_label != 0) {
		Home.mode.Label = mode_label;
		Home.mode.Color = mode_color;
	}
	status_reflash_flag = 1;
}

void KEY_Scan(void) {
	key_push_flag_last[0] = key_push_flag[0];
	key_push_flag_last[1] = key_push_flag[1];
	key_push_flag_last[2] = key_push_flag[2];
	key_push_flag_last[3] = key_push_flag[3];
	key_push_flag[0] = !HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin);
	key_push_flag[1] = !HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin);
	key_push_flag[2] = !HAL_GPIO_ReadPin(KEYIN1_GPIO_Port, KEYIN1_Pin);
	key_push_flag[3] = !HAL_GPIO_ReadPin(KEYIN2_GPIO_Port, KEYIN2_Pin);
	key_reflash_flag[0] = key_push_flag[0] ^ key_push_flag_last[0];
	key_reflash_flag[1] = key_push_flag[1] ^ key_push_flag_last[1];
	key_reflash_flag[2] = key_push_flag[2] ^ key_push_flag_last[2];
	key_reflash_flag[3] = key_push_flag[3] ^ key_push_flag_last[3];

	/*--------KEY1 PUSH--------*/ //自复位按键
	if (key_reflash_flag[0]) {
		program_group_flag[0] = key_push_flag[0];
		/*
		 if (KEY_Flag[0]) {
		 Home.flag.Label = "[TEST]";
		 Home.flag.Color = BLUE;
		 Home.mode.Label = "ZPSetting";
		 Home.mode.Color = WHITE;
		 Program_Flag[0] = 1;
		 //Program_Flag[1] = 0;
		 //Program_Flag[2] = 0;
		 } else {
		 Home.flag.Label = "[READY]";
		 Home.flag.Color = GREEN;
		 Home.mode.Label = "RdToWork";
		 Home.mode.Color = WHITE;
		 Program_Flag[0] = 0;
		 }
		 */
		goto Reflash;
	}
	/*--------KEY2 CLICK--------*/ //自锁按键
	if (key_reflash_flag[1]) {
		/*
		 if (Program_Flag[1]) {
		 Home.flag.Label = "[WORK]";
		 Home.flag.Color = YELLOW;
		 Home.mode.Label = "Normal";
		 Home.mode.Color = WHITE;
		 } else {
		 Home.flag.Label = "[READY]";
		 Home.flag.Color = GREEN;
		 Home.mode.Label = "RdToWork";
		 Home.mode.Color = WHITE;
		 }*/
		if (key_push_flag[1]) {
			program_group_flag[0] = 0;
			program_group_flag[2] = 0;
			program_group_flag[1] = !program_group_flag[1];
		}
		goto Reflash;
	}
	/*--------KEYIN1 ON--------*/
	if (key_reflash_flag[2]) {
		/*
		 if (KEY_Flag[2]) {
		 Home.flag.Label = "[KEYIN1]";
		 Home.flag.Color = BLUE;
		 Home.mode.Label = "Testing";
		 Home.mode.Color = WHITE;
		 Program_Flag[0] = 0;
		 Program_Flag[1] = 0;
		 Program_Flag[2] = 1;
		 } else {
		 Home.flag.Label = "[READY]";
		 Home.flag.Color = GREEN;
		 Home.mode.Label = "RdToWork";
		 Home.mode.Color = WHITE;
		 Program_Flag[2] = 0;
		 }*/
		goto Reflash;
	}
	Reflash: if (key_reflash_flag[0] || key_reflash_flag[1] || key_reflash_flag[2] || key_reflash_flag[3]) {
		status_reflash_flag = 1;
	}
	targets_reflash_flag = 1;
}

void Homepage_Init(void) {
	Home.flag.Label = "[START]";
	Home.flag.Color = GREEN;

	Home.status[0].Label = "A:";
	Home.status[0].Color = WHITE;
	Home.status[0].num1 = 0;

	Home.status[1].Label = "B:";
	Home.status[1].Color = WHITE;
	Home.status[1].num1 = 0;

	Home.status[2].Label = "C:";
	Home.status[2].Color = WHITE;
	Home.status[2].num1 = 0;

	Home.mode.Label = "Initiating";
	Home.mode.Color = WHITE;

	Home.params[0].Label = "MT1:";
	Home.params[0].Color = WHITE;
	Home.params[0].num1 = 0;

	Home.params[1].Label = "MT2:";
	Home.params[1].Color = WHITE;
	Home.params[1].num1 = 0;

	Home.params[2].Label = "MT3:";
	Home.params[2].Color = WHITE;
	Home.params[2].num1 = 0;

	Home.params[3].Label = "SYS:";
	Home.params[3].Color = WHITE;
	Home.params[3].num1 = 0;

	Paint_DrawString_EN(125, 5, (char*) Home.status[0].Label, &Font20, BLACK, GBLUE);
	Paint_DrawString_EN(125, 24, (char*) Home.status[1].Label, &Font20, BLACK, GBLUE);
	Paint_DrawString_EN(125, 43, (char*) Home.status[2].Label, &Font20, BLACK, GBLUE);

	Paint_DrawString_EN(5, 63, (char*) Home.params[0].Label, &Font16, BLACK, GBLUE);
	Paint_DrawString_EN(5, 81, (char*) Home.params[1].Label, &Font16, BLACK, GBLUE);
	Paint_DrawString_EN(5, 99, (char*) Home.params[2].Label, &Font16, BLACK, GBLUE);
	Paint_DrawString_EN(5, 117, (char*) Home.params[3].Label, &Font16, BLACK, GBLUE);
	Paint_DrawString_EN(125, 63, "-->", &Font16, BLACK, GBLUE);
	Paint_DrawString_EN(125, 81, "-->", &Font16, BLACK, GBLUE);
	Paint_DrawString_EN(125, 99, "-->", &Font16, BLACK, GBLUE);
	Paint_DrawString_EN(125, 117, "---", &Font16, BLACK, GBLUE);

	Status_Reflash();
	Parameters_Reflash();
	Targets_Reflash();
}

void UI_Init(void) {
	DEV_Module_Init(); //IO
	LCD_1IN14_SetBackLight(SET); //IO
	DEV_Delay_ms(100);
	LCD_1IN14_Init(HORIZONTAL); //Data
	DEV_Delay_ms(100);
	Paint_NewImage(LCD_1IN14.WIDTH, LCD_1IN14.HEIGHT, ROTATE_0, WHITE);
	Paint_SetClearFuntion(LCD_1IN14_Clear);
	Paint_SetDisplayFuntion(LCD_1IN14_DrawPaint);
	UI_Startup();
	Homepage_Init();
	Home.flag.Label = "[READY]";
	Home.flag.Color = GREEN;
	Home.mode.Label = "Idling";
	Home.mode.Color = WHITE;
	Status_Reflash();

}

void PID_Module_Init(void) {
	PID_Init(&pid_tension_1, 0.5f, 0.01f, 0.1f);
	PID_Init(&pid_tension_2, 0.5f, 0.01f, 0.1f);
	PID_Init(&pid_tension_3, 0.5f, 0.01f, 0.1f);
}

/*
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim == &htim17) { //回传基准时钟5ms
		HAL_TIM_Base_Start_IT(&htim17);

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
		//vofa_send_data(12, Home.status[0].num2); //Voltage
		//vofa_send_data(13, Home.status[1].num2); //Current
		vofa_sendframetail();

		driver_monitoring_flag = 1;
	}
	if (htim == &htim16) { //屏幕刷新基准时钟100ms
		HAL_TIM_Base_Start_IT(&htim16);

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

		Home.params[3].num1 = (float) system_frequency / 1000;
		Home.params[3].num2 = (float) program_mode_code;

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
	if (htim == &htim7) { //控制循环基准时钟1ms
		HAL_TIM_Base_Start_IT(&htim7);

		ADC_Read();
		tension_sensor[0] = (float) ADC_value_2[0] / 4096 * 3.3;	//读取传感器信息
		tension_sensor[1] = (float) ADC_value_2[1] / 4096 * 3.3;
		tension_sensor[2] = (float) ADC_value_2[2] / 4096 * 3.3;

		switch (program_mode_code) {	//控制循环模式识别
		case 000:	//空闲
			break;
		case 101:	//测试程序1
			break;
		case 102:	//测试程序2
			break;
		case 103:	//测试程序3
			break;
		case 201:	//外部信号控制循环
			for (int i = 0; i < 3; i++) {
				switch (offboard_command[i]) {
				case 20:
					if (offboard_data[i] > 1.2) {
						estop(0);
						program_mode_code = 999;
						error_code = 901;
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
				default:
					estop(0);
					program_mode_code = 999;
					error_code = 902;
				}
			}
			break;
		case 202:					//自定义控制循环
			for (int i = 0; i < 3; i++) {
				set_torque(i + 1, 0.15, 1, 0);					//预紧
			}
			break;
		case 203:					//自定义控制
			for (int i = 0; i < 3; i++) {
				set_speed(i + 1, PID_Compute(&pid_tension_1, offboard_data[i], tension_sensor[i], 0.001f), 1000, 1);					//预紧
			}
			break;
		case 999:
			program_group_flag[0] = 0;
			program_group_flag[1] = 0;
			switch (error_code) {
			case 900:
				Home.flag.Label = "ERROR";
				Home.flag.Color = RED;
				Home.mode.Label = "Timeout";					//系统超时
				Home.mode.Color = YELLOW;
				break;
			case 901:
				Home.flag.Label = "ERROR";
				Home.flag.Color = RED;
				Home.mode.Label = "OverTorque";					//力矩软限制
				Home.mode.Color = YELLOW;
				break;
			case 902:
				Home.flag.Label = "ERROR";
				Home.flag.Color = RED;
				Home.mode.Label = "SignalLost";					//信号格式限制
				Home.mode.Color = YELLOW;
				break;
			case 903:
				Home.flag.Label = "ERROR";
				Home.flag.Color = RED;
				Home.mode.Label = "ZeroOut";					//归零超时
				Home.mode.Color = YELLOW;
				break;
			default:
				Home.flag.Label = "ERROR";
				Home.flag.Color = RED;
				Home.mode.Label = "Unknown";					//未知错误
				Home.mode.Color = RED;
				break;
			}
			Home.params[3].num2 = (float) error_code;
			Parameters_Reflash();
			Status_Reflash();
			while (1)
				estop(0);
			//线程阻塞
			;
			break;
		default:
		}
	}
	if (htim == &htim6) { //占用率基准时钟1000ms
		HAL_TIM_Base_Start_IT(&htim6);

		if (system_timeout_flag >= 1 && program_mode_code != 888) {
			program_mode_code = 999;
			error_code = 900;
		}
		if (system_timeout_flag >= 5 && program_mode_code == 888) {
			program_mode_code = 999;
			error_code = 903;
		}
		system_frequency = system_cycle_counter;
		system_cycle_counter = 0;
		system_timeout_flag++;
	}
}
*/

