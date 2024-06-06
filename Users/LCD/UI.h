/*
 * UI.h
 *
 *  Created on: Feb 28, 2024
 *      Author: Cross
 */

#ifndef LCD_UI_H_
#define LCD_UI_H_
#include <COM/CAN_Com.h>
#include <stdio.h>
#include "string.h"
#include "stdint.h"
#include "main.h"
#include "GUI_Paint.h"
#include "dma.h"
#include "PID.h"
#include "tim.h"

void UI_Startup(void);
void Homepage_Init(void);
void Parameters_Reflash(void);
void Targets_Reflash(void);
void Status_Reflash(void);
void KEY_Scan(void);
void UI_Init(void);
long GetMicros(void);

extern volatile int Parameters_Reflash_Flag, Targets_Reflash_Flag, Status_Reflash_Flag;
extern volatile int Program_Flag[4];
extern volatile float TensionSensor[3];
extern volatile float SystemOccupancy;

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
