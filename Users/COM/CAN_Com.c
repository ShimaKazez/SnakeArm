/*
 * CAN_Com.c
 *
 *  Created on: Mar 5, 2024
 *      Author: Cross
 */

#include "CAN_Com.h"
#include "Vofa+.h"
#include "UI.h"

volatile C620_StatusTypeDef C620_Status;
volatile C620_ControlTypeDef C620_Control;

unsigned short swapShort16(unsigned short shortValue) {
	return ((shortValue & 0x00FF) << 8) | ((shortValue & 0xFF00) >> 8);
}

void FDCAN_init(void) {
	FDCAN_FilterTypeDef sFilterConfig;
	sFilterConfig.IdType = FDCAN_STANDARD_ID;
	sFilterConfig.FilterIndex = 0;
	sFilterConfig.FilterType = FDCAN_FILTER_MASK;
	sFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
	sFilterConfig.FilterID1 = 0x203;
	sFilterConfig.FilterID2 = 0x7ff;
	HAL_FDCAN_ConfigFilter(&hfdcan1, &sFilterConfig);
	HAL_FDCAN_ConfigGlobalFilter(&hfdcan1, FDCAN_REJECT, FDCAN_REJECT, DISABLE, DISABLE);
	HAL_FDCAN_Start(&hfdcan1);
}

void FDCAN_Transmit(void) {
	union {
		uint8_t TxData[8];
		struct {
			uint16_t C1, C2, C3, C4;
		};
	} Data;
	Data.C1 = swapShort16(C620_Control.Current1);
	Data.C2 = swapShort16(C620_Control.Current2);
	Data.C3 = swapShort16(C620_Control.Current3);
	Data.C4 = swapShort16(C620_Control.Current4);
	FDCAN_TxHeaderTypeDef TxHeader = { 0 };
	//CDC_Transmit_FS(Data.TxData,sizeof(Data.TxData));
	TxHeader.Identifier = 0x200;
	TxHeader.IdType = FDCAN_STANDARD_ID; // FDCAN_STANDARD_ID or FDCAN_EXTENDED_ID
	TxHeader.TxFrameType = FDCAN_DATA_FRAME; // FDCAN_DATA_FRAME or FDCAN_REMOTE_FRAME
	TxHeader.DataLength = FDCAN_DLC_BYTES_8; // 0 1 2 3 4 5 6 7 8 12 16 20 24 32 48 64
	TxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE; // FDCAN_ESI_ACTIVE or FDCAN_ESI_PASSIVE
	TxHeader.BitRateSwitch = FDCAN_BRS_OFF; // FDCAN_BRS_OFF or FDCAN_BRS_ON
	TxHeader.FDFormat = FDCAN_CLASSIC_CAN; // FDCAN_CLASSIC_CAN or FDCAN_FD_CAN
	//TxHeader.TxEventFifoControl=FDCAN_NO_TX_EVENTS;
	//TxHeader.MessageMarker=0;
	//while (HAL_FDCAN_GetTxFifoFreeLevel(&hfdcan1) == 0); // �ȴ��з����������
	HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeader, Data.TxData); // ��������֡
}

void FDCAN_Receive(void) {
	if (HAL_FDCAN_GetRxFifoFillLevel(&hfdcan1, FDCAN_RX_FIFO0) != 0) {
		FDCAN_RxHeaderTypeDef RxHeader;
		union {
			uint8_t RxData[8];
			struct {
				uint16_t C1, C2, C3, C4;
			};
		} Data;
		if (HAL_FDCAN_GetRxMessage(&hfdcan1, FDCAN_RX_FIFO0, &RxHeader, Data.RxData) == HAL_OK) {
			if (Buttom_Flag[1]) {
				vofa_send_data(0, swapShort16(Data.C1));
				vofa_send_data(1, swapShort16(Data.C2));
				vofa_send_data(2, swapShort16(Data.C3));
				vofa_send_data(3, swapShort16(Data.C4));
				vofa_sendframetail();
				Home.params[0].num2 = swapShort16(Data.C1);
				Home.params[1].num2 = swapShort16(Data.C2);
				Home.params[2].num2 = swapShort16(Data.C3);
				Home.params[3].num2 = swapShort16(Data.C4);
				Targets_Reflash();
			}
			if (Buttom_Flag[0] || Buttom_Flag[3]) {
				C620_Status.Angle = Data.RxData[0] << 8 | Data.RxData[1];
				C620_Status.Speed = Data.RxData[2] << 8 | Data.RxData[3];
				C620_Status.Current = Data.RxData[4] << 8 | Data.RxData[5];
				C620_Status.Temp = Data.RxData[6];
			}
		}
	}
}
