/*
 * UI.c
 *
 *  Created on: Feb 28, 2024
 *      Author: Cross
 */

#include "UI.h"
#include "Vofa+.h"

volatile HOME Home;
volatile DriverS Drivers;
volatile float TensionSensor[3];
volatile float SystemOccupancy;
volatile int SystemCircleTimesRecord;
int KEY_Flag[4];
int KEY_FlagOld[4];
int KEY_Reflash[4];
int Screen_Seq;
int ErrorFlag;
int ParamsReflashSteps, TargetReflashSteps, StatusReflashSteps;
int TimerLast;
uint8_t cRed, cGreen, cBlue;
volatile int Program_Flag[3];
volatile int Parameters_Reflash_Flag, Targets_Reflash_Flag, Status_Reflash_Flag;

int numlen(double num) {
	int len = 1;
	while ((int) (num /= 10)) {
		len++;
	}
	return len;
}

void UI_Startup(void) {
	float X, Y, R;
	Paint_ClearWindows(0, 0, 239, 134, BLACK);
	for (int i = 0; i < 100; i++) {
		X = 120;
		Y = 135 - (10000 - ((100 - i) * (100 - i))) * 67 / 10000;
		R = 5;
		Paint_DrawCircle(X, Y, R, GBLUE, 1, 0);
		HAL_Delay(5);
		Paint_DrawCircle(X, Y, R, BLACK, 1, 0);
	}
	HAL_Delay(200);
	for (int i = 0; i < 12; i++) {
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
		HAL_Delay(20);
	}
	Paint_ClearWindows(39, 44, 200, 90, BLACK);
	Paint_DrawString_EN(43, 49, "Snake Arm", &Font24, BLACK, GBLUE);
	Paint_DrawString_EN(64, 73, "Ver.202411", &Font16, BLACK, GBLUE);
	HAL_Delay(500);
	Paint_ClearWindows(0, 0, 239, 134, BLACK);
}

void Parameters_Reflash(void) {
	ParamsReflashSteps++;
	switch (ParamsReflashSteps) {
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
		ParamsReflashSteps = 0;
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

void KEY_Scan(void) {
	KEY_FlagOld[0] = KEY_Flag[0];
	KEY_FlagOld[1] = KEY_Flag[1];
	KEY_FlagOld[2] = KEY_Flag[2];
	KEY_FlagOld[3] = KEY_Flag[3];
	KEY_Flag[0] = !HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin);
	KEY_Flag[1] = !HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin);
	KEY_Flag[2] = !HAL_GPIO_ReadPin(KEYIN1_GPIO_Port, KEYIN1_Pin);
	KEY_Flag[3] = !HAL_GPIO_ReadPin(KEYIN2_GPIO_Port, KEYIN2_Pin);
	KEY_Reflash[0] = KEY_Flag[0] ^ KEY_FlagOld[0];
	KEY_Reflash[1] = KEY_Flag[1] ^ KEY_FlagOld[1];
	KEY_Reflash[2] = KEY_Flag[2] ^ KEY_FlagOld[2];
	KEY_Reflash[3] = KEY_Flag[3] ^ KEY_FlagOld[3];

	/*--------KEY1 PUSH--------*/
	if (KEY_Reflash[0]) {
		if (KEY_Flag[0]) {
			Home.flag.Label = "[TEST]";
			Home.flag.Color = BLUE;
			Home.mode.Label = "Idling";
			Home.mode.Color = WHITE;
			Program_Flag[0] = 1;
			Program_Flag[1] = 0;
			Program_Flag[2] = 0;
		} else {
			Home.flag.Label = "[READY]";
			Home.flag.Color = GREEN;
			Home.mode.Label = "RdToWork";
			Home.mode.Color = WHITE;
			Program_Flag[0] = 0;
		}
		goto Reflash;
	}
	/*--------KEY2 CLICK--------*/
	if (KEY_Reflash[1]) {
		if (Program_Flag[1]) {
			Home.flag.Label = "[WORK]";
			Home.flag.Color = YELLOW;
			Home.mode.Label = "Single";
			Home.mode.Color = WHITE;
		} else {
			Home.flag.Label = "[READY]";
			Home.flag.Color = GREEN;
			Home.mode.Label = "RdToWork";
			Home.mode.Color = WHITE;
		}
		if (KEY_Flag[1]) {
			Program_Flag[0] = 0;
			Program_Flag[2] = 0;
			Program_Flag[1] = !Program_Flag[1];
		}
		goto Reflash;
	}
	/*--------KEYIN1 ON--------*/
	if (KEY_Reflash[2]) {
		if (KEY_Flag[2]) {
			Home.flag.Label = "[TEST]";
			Home.flag.Color = BLUE;
			Home.mode.Label = "Idling";
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
		}
		goto Reflash;
	}
	Reflash: if (KEY_Reflash[0] || KEY_Reflash[1] || KEY_Reflash[2] || KEY_Reflash[3]) {
		Status_Reflash_Flag = 1;
	}
	Targets_Reflash_Flag = 1;
}

void Homepage_Init(void) {
	Home.flag.Label = "[READY]";
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
	Paint_DrawString_EN(125, 117, "-->", &Font16, BLACK, GBLUE);

	Status_Reflash();
	Parameters_Reflash();
	Targets_Reflash();
}

void UI_Init(void) {
	DEV_Module_Init();
	LCD_1IN14_SetBackLight(SET);
	LCD_1IN14_Init(HORIZONTAL);
	Paint_NewImage(LCD_1IN14.WIDTH, LCD_1IN14.HEIGHT, ROTATE_180, WHITE);
	Paint_SetClearFuntion(LCD_1IN14_Clear);
	Paint_SetDisplayFuntion(LCD_1IN14_DrawPaint);
	UI_Startup();
	Homepage_Init();
	Home.flag.Label = "[READY]";
	Home.flag.Color = GREEN;
	Home.mode.Label = "RdToWork";
	Home.mode.Color = WHITE;
	Status_Reflash();
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim == &htim17) {
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
		vofa_send_data(9, TensionSensor[0]);
		vofa_send_data(10, TensionSensor[1]);
		vofa_send_data(11, TensionSensor[2]);
		vofa_send_data(12, Home.status[0].num2);//Voltage
		vofa_send_data(13, Home.status[1].num2);//Current
		vofa_sendframetail();

	}
	if (htim == &htim16) {
		HAL_TIM_Base_Start_IT(&htim16);
		if (Screen_Seq > 90)
			Screen_Seq = 0;
		if (Screen_Seq >= 5)
			Paint_ClearWindows((Screen_Seq - 10) * 3, 133, (Screen_Seq - 9) * 3, 134, BLACK);
		if (Screen_Seq <= 80) {
			if (Screen_Seq >= 0 && Screen_Seq <= 27) {
				cRed = Screen_Seq * 9 + 10;
				cGreen = 0;
				cBlue = (27 - Screen_Seq) * 9 + 10;
			}
			if (Screen_Seq > 27 && Screen_Seq <= 54) {
				cRed = (54 - Screen_Seq) * 9 + 10;
				cGreen = (Screen_Seq - 28) * 9 + 10;
				cBlue = 0;
			}
			if (Screen_Seq > 54 && Screen_Seq <= 80) {
				cRed = 0;
				cGreen = (80 - Screen_Seq) * 9 + 10;
				cBlue = (Screen_Seq - 55) * 9 + 10;
			}
			Paint_ClearWindows(Screen_Seq * 3, 133, (Screen_Seq + 1) * 3, 134, RGB888ToRGB565(cRed, cGreen, cBlue));
		}
		Screen_Seq++;

		if (Parameters_Reflash_Flag) {
			Parameters_Reflash();
			Parameters_Reflash_Flag = 0;
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
}

