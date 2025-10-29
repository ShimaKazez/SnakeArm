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

extern volatile int parameters_reflash_flag, Targets_Reflash_Flag, StStus_Reflash_Flag;
extern volatile int Buttom_Flag[3];
extern volatile float tension_sensor[3];
extern volatile float PWR_sensor[2];
extern volatile float SystemOccupancy;
extern volatile int system_cycle_counter;
extern volatile int driver_monitoring_flag;
extern volatile int program_mode_code;
extern volatile int system_timeout_flag;
extern volatile int UI_Init_Flag;
extern volatile int system_frequency;
extern volatile int screen_sequence;
extern volatile uint8_t c_Red, c_Green, c_Blue;

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

struct DriverStatus {
	float angle;
	float speed;
	float torque;
};

typedef struct {
	volatile struct DriverStatus driver1;
	volatile struct DriverStatus driver2;
	volatile struct DriverStatus driver3;
} DriverS;
extern volatile DriverS Drivers;

// 定义菜单项结构体
typedef struct {
	char *flag_label;
	uint16_t flag_color;
	char *mode_label;
	uint16_t mode_color;
	int program_mode_code;
} MenuItem;
void HandleMenuSwitch(MenuItem[], int, int*);

#endif /* LCD_UI_H_ */
