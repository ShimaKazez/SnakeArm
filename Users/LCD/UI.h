/*
 * UI.h
 *
 *  Created on: Feb 28, 2024
 *      Author: Cross
 */

#ifndef LCD_UI_H_
#define LCD_UI_H_
#include <CAN_Com.h>
#include <Filters.h>
#include <stdio.h>
#include "string.h"
#include "stdint.h"
#include "main.h"
#include "GUI_Paint.h"
#include "dma.h"
#include "tim.h"
#include "ADC_Sample.h"
#include "DrEmpower_can.h"
#include "Vofa+.h"
#include "Servo.h"
#include "PID.h"

void UI_Startup(void);
void Homepage_Init(void);
void Parameters_Reflash(void);
void Targets_Reflash(void);
void Status_Reflash(void);
void Status_Set(char*, uint16_t, char*, uint16_t);
void KEY_Scan(void);
void UI_Init(void);
void PID_Module_Init(void);
long GetMicros(void);

extern volatile int parameters_reflash_flag, Targets_Reflash_Flag, Status_Reflash_Flag;
extern volatile int Buttom_Flag[3];
extern volatile int Buttom_Pushed_Flag[4];
extern volatile float tension_sensor[3];
extern volatile float PWR_sensor[2];
extern volatile float SystemOccupancy;
extern volatile int system_cycle_counter;
extern volatile int driver_monitoring_flag;
extern volatile int program_mode_code;
extern volatile int Sys_Timeout_Count;
extern volatile int UI_Init_Flag;
extern volatile int system_frequency;
extern volatile int screen_sequence;
extern volatile uint8_t c_Red, c_Green, c_Blue;

extern volatile float Tension_Data[3];
extern volatile float Angle_Data[3];
extern volatile float Speed_Data[3];
extern volatile float Torque_Data[3];

struct ELEMENT {
	volatile char *Label;
	volatile uint16_t Color;
	volatile float num1;
	volatile float num2;
	volatile float num3;
};

typedef struct {
	volatile struct ELEMENT flag;
	volatile struct ELEMENT status[3];
	volatile struct ELEMENT mode;
	volatile struct ELEMENT params[4];
} HOME;
extern volatile HOME Home;

// 定义菜单项结构体
typedef struct {
	char *flag_label;
	uint16_t flag_color;
	char *mode_label;
	uint16_t mode_color;
	int program_mode_code;
} MenuItem;
extern MenuItem L1_menu_items[4];
extern MenuItem L2_menu_items[4];
void HandleMenuSwitch(MenuItem[], int, int*);

#endif /* LCD_UI_H_ */
