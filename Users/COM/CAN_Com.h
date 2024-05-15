/*
 * CAN_Com.h
 *
 *  Created on: Mar 5, 2024
 *      Author: Cross
 */

#ifndef COM_CAN_COM_H_
#define COM_CAN_COM_H_

#include "fdcan.h"

typedef struct {
	uint32_t StdId;
	uint32_t ExtId;
	uint32_t IDE;
	uint32_t RTR;
	uint32_t DLC;
	FunctionalState TransmitGlobalTime;
} CAN_TxHeaderTypeDef;

typedef struct {
	volatile uint16_t Angle;
	volatile uint16_t Speed;
	volatile uint16_t Current;
	volatile uint8_t Temp;
	volatile uint8_t Null;
} C620_StatusTypeDef;
extern volatile C620_StatusTypeDef C620_Status;

typedef struct {
	volatile uint16_t Current1;
	volatile uint16_t Current2;
	volatile uint16_t Current3;
	volatile uint16_t Current4;
} C620_ControlTypeDef;
extern volatile C620_ControlTypeDef C620_Control;

void FDCAN_init(void);
void FDCAN_Transmit(void);
void FDCAN_Receive(void);

#endif /* COM_CAN_COM_H_ */
