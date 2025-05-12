/*
 * UI.h
 *
 *  Created on: Feb 28, 2024
 *      Author: Cross
 */

#ifndef LCD_UI_H_
#define LCD_UI_H_
#include <CAN_Com.h>
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
#include "Kalman.h"

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

extern volatile int parameters_reflash_flag, targets_reflash_flag, status_reflash_flag;
extern volatile int program_group_flag[3];
extern volatile float tension_sensor[3];
extern volatile float SystemOccupancy;
extern volatile int system_cycle_counter;
extern volatile int driver_monitoring_flag;
extern volatile int program_mode_code;
extern volatile int system_timeout_flag;

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

#endif /* LCD_UI_H_ */
