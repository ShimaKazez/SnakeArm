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
volatile float PWR_sensor[2];
//volatile float SystemOccupancy;
volatile int system_cycle_counter, error_code;
volatile int system_timeout_flag;
int system_frequency;
int key_push_flag[4];
int key_push_flag_last[4];
int key_reflash_flag[4];
int screen_sequence;
int parameters_reflash_counter, target_reflash_counter, status_reflash_counter;
volatile int UI_Init_Flag;
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
	Paint_DrawString_EN(64, 73, "Ver.202510", &Font16, BLACK, GBLUE);
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
		goto Reflash;
	}
	/*--------KEY2 CLICK--------*/ //自锁按键
	if (key_reflash_flag[1]) {
		if (key_push_flag[1]) {
			program_group_flag[0] = 0;
			program_group_flag[2] = 0;
			program_group_flag[1] = !program_group_flag[1];
		}
		goto Reflash;
	}
	/*--------KEYIN1 ON--------*/
	if (key_reflash_flag[2]) {
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

	Home.params[3].Label = "PWR:";
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
	Paint_DrawString_EN(125, 117, "CUR", &Font16, BLACK, GBLUE);

	Status_Reflash();
	Parameters_Reflash();
	Targets_Reflash();
}

void UI_Init(void) {
	UI_Init_Flag = 1;
	DEV_Module_Init(); //IO
	LCD_1IN14_SetBackLight(SET); //IO
//	DEV_Delay_ms(100);
	LCD_1IN14_Init(HORIZONTAL); //Data
//	DEV_Delay_ms(100);
//	LCD_1IN14_Clear(BLACK); //Data
	Paint_NewImage(LCD_1IN14.WIDTH, LCD_1IN14.HEIGHT, ROTATE_0, WHITE);
	Paint_SetClearFuntion(LCD_1IN14_Clear);
	Paint_SetDisplayFuntion(LCD_1IN14_DrawPaint);
//	Paint_Clear(BLACK);
	UI_Startup();
	Homepage_Init();
	Home.flag.Label = "[READY]";
	Home.flag.Color = GREEN;
	Home.mode.Label = "Idling";
	Home.mode.Color = WHITE;
	Status_Reflash();
	UI_Init_Flag = 0;
}

void PID_Module_Init(void) {
	PID_Init(&pid_tension_1, 0.5f, 0.01f, 0.1f);
	PID_Init(&pid_tension_2, 0.5f, 0.01f, 0.1f);
	PID_Init(&pid_tension_3, 0.5f, 0.01f, 0.1f);
}

