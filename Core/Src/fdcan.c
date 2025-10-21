/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    fdcan.c
 * @brief   This file provides code for the configuration
 *          of the FDCAN instances.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "fdcan.h"

/* USER CODE BEGIN 0 */
extern uint8_t rx_buffer[8];
extern int8_t READ_FLAG;
extern uint16_t can_id;

/* USER CODE END 0 */

FDCAN_HandleTypeDef hfdcan1;

/* FDCAN1 init function */
void MX_FDCAN1_Init(void)
{

  /* USER CODE BEGIN FDCAN1_Init 0 */

  /* USER CODE END FDCAN1_Init 0 */

  /* USER CODE BEGIN FDCAN1_Init 1 */

  /* USER CODE END FDCAN1_Init 1 */
  hfdcan1.Instance = FDCAN1;
  hfdcan1.Init.ClockDivider = FDCAN_CLOCK_DIV1;
  hfdcan1.Init.FrameFormat = FDCAN_FRAME_CLASSIC;
  hfdcan1.Init.Mode = FDCAN_MODE_NORMAL;
  hfdcan1.Init.AutoRetransmission = DISABLE;
  hfdcan1.Init.TransmitPause = DISABLE;
  hfdcan1.Init.ProtocolException = DISABLE;
  hfdcan1.Init.NominalPrescaler = 7;
  hfdcan1.Init.NominalSyncJumpWidth = 2;
  hfdcan1.Init.NominalTimeSeg1 = 18;
  hfdcan1.Init.NominalTimeSeg2 = 5;
  hfdcan1.Init.DataPrescaler = 7;
  hfdcan1.Init.DataSyncJumpWidth = 5;
  hfdcan1.Init.DataTimeSeg1 = 18;
  hfdcan1.Init.DataTimeSeg2 = 5;
  hfdcan1.Init.StdFiltersNbr = 1;
  hfdcan1.Init.ExtFiltersNbr = 0;
  hfdcan1.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
  if (HAL_FDCAN_Init(&hfdcan1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN FDCAN1_Init 2 */

  /* USER CODE END FDCAN1_Init 2 */

}

void HAL_FDCAN_MspInit(FDCAN_HandleTypeDef* fdcanHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
  if(fdcanHandle->Instance==FDCAN1)
  {
  /* USER CODE BEGIN FDCAN1_MspInit 0 */

  /* USER CODE END FDCAN1_MspInit 0 */

  /** Initializes the peripherals clocks
  */
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_FDCAN;
    PeriphClkInit.FdcanClockSelection = RCC_FDCANCLKSOURCE_PLL;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
    {
      Error_Handler();
    }

    /* FDCAN1 clock enable */
    __HAL_RCC_FDCAN_CLK_ENABLE();

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**FDCAN1 GPIO Configuration
    PB8-BOOT0     ------> FDCAN1_RX
    PB9     ------> FDCAN1_TX
    */
    GPIO_InitStruct.Pin = CAN_RX_Pin|CAN_TX_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF9_FDCAN1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* FDCAN1 interrupt Init */
    HAL_NVIC_SetPriority(FDCAN1_IT0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(FDCAN1_IT0_IRQn);
  /* USER CODE BEGIN FDCAN1_MspInit 1 */

  /* USER CODE END FDCAN1_MspInit 1 */
  }
}

void HAL_FDCAN_MspDeInit(FDCAN_HandleTypeDef* fdcanHandle)
{

  if(fdcanHandle->Instance==FDCAN1)
  {
  /* USER CODE BEGIN FDCAN1_MspDeInit 0 */

  /* USER CODE END FDCAN1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_FDCAN_CLK_DISABLE();

    /**FDCAN1 GPIO Configuration
    PB8-BOOT0     ------> FDCAN1_RX
    PB9     ------> FDCAN1_TX
    */
    HAL_GPIO_DeInit(GPIOB, CAN_RX_Pin|CAN_TX_Pin);

    /* FDCAN1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(FDCAN1_IT0_IRQn);
  /* USER CODE BEGIN FDCAN1_MspDeInit 1 */

  /* USER CODE END FDCAN1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
void Can_Config(void) {
	FDCAN_FilterTypeDef FDCAN_FilterType;
	FDCAN_FilterType.IdType = FDCAN_STANDARD_ID;
	FDCAN_FilterType.FilterIndex = 0;
	FDCAN_FilterType.FilterType = FDCAN_FILTER_MASK;
	FDCAN_FilterType.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
	FDCAN_FilterType.FilterID1 = 0x111;
	FDCAN_FilterType.FilterID2 = 0x000;
	if (HAL_FDCAN_ConfigFilter(&SERVO_CAN, &FDCAN_FilterType) != HAL_OK) {
		Error_Handler();
	}
	HAL_FDCAN_ConfigGlobalFilter(&SERVO_CAN, FDCAN_REJECT, FDCAN_REJECT, DISABLE, DISABLE);
	if (HAL_FDCAN_Start(&SERVO_CAN) != HAL_OK) {
		Error_Handler();
	}
	if (HAL_FDCAN_ActivateNotification(&SERVO_CAN, FDCAN_IT_RX_FIFO0_NEW_MESSAGE,0) != HAL_OK) {
		Error_Handler();
	}
}

uint8_t Can_Send_Msg(uint32_t id, uint8_t len, uint8_t *data) {
	FDCAN_TxHeaderTypeDef FDCAN_TxHeader;
	HAL_StatusTypeDef HAL_RetVal;
	FDCAN_TxHeader.Identifier = id;
	FDCAN_TxHeader.IdType = FDCAN_STANDARD_ID; // FDCAN_STANDARD_ID or FDCAN_EXTENDED_ID
	FDCAN_TxHeader.TxFrameType = FDCAN_DATA_FRAME; // FDCAN_DATA_FRAME or FDCAN_REMOTE_FRAME
	FDCAN_TxHeader.DataLength = FDCAN_DLC_BYTES_8; // 0 1 2 3 4 5 6 7 8 12 16 20 24 32 48 64
	FDCAN_TxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE; // FDCAN_ESI_ACTIVE or FDCAN_ESI_PASSIVE
	FDCAN_TxHeader.BitRateSwitch = FDCAN_BRS_OFF; // FDCAN_BRS_OFF or FDCAN_BRS_ON
	FDCAN_TxHeader.FDFormat = FDCAN_CLASSIC_CAN; // FDCAN_CLASSIC_CAN or FDCAN_FD_CAN
	FDCAN_TxHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
	//CAN_TxHeader.MessageMarker=0;
	HAL_RetVal = HAL_FDCAN_AddMessageToTxFifoQ(&SERVO_CAN, &FDCAN_TxHeader, data);
	if (HAL_RetVal != HAL_OK)
		return 1;
	return 0;
}
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs) {
	FDCAN_RxHeaderTypeDef hFDCAN1_RxHeader;
	if (HAL_FDCAN_GetRxMessage(&SERVO_CAN, FDCAN_RX_FIFO0, &hFDCAN1_RxHeader, rx_buffer) == HAL_OK) {
		can_id = hFDCAN1_RxHeader.Identifier;
		READ_FLAG = 1;
	}
	HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);
}

/* USER CODE END 1 */
