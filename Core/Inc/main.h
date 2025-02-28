/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.h
 * @brief          : Header for main.c file.
 *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32g4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define TimerPeriod17 5
#define TimerPeriod16 100
#define TimerPeriod6 1000
#define TimerPeriod7 1
#define KEY1_Pin GPIO_PIN_13
#define KEY1_GPIO_Port GPIOC
#define KEY2_Pin GPIO_PIN_14
#define KEY2_GPIO_Port GPIOC
#define KEY3_Pin GPIO_PIN_15
#define KEY3_GPIO_Port GPIOC
#define O_IN_Pin GPIO_PIN_0
#define O_IN_GPIO_Port GPIOF
#define O_OUT_Pin GPIO_PIN_1
#define O_OUT_GPIO_Port GPIOF
#define PWM1_Pin GPIO_PIN_0
#define PWM1_GPIO_Port GPIOC
#define PWM2_Pin GPIO_PIN_1
#define PWM2_GPIO_Port GPIOC
#define PWM3_Pin GPIO_PIN_2
#define PWM3_GPIO_Port GPIOC
#define PWM4_Pin GPIO_PIN_3
#define PWM4_GPIO_Port GPIOC
#define ADC1_Pin GPIO_PIN_6
#define ADC1_GPIO_Port GPIOA
#define ADC2_Pin GPIO_PIN_7
#define ADC2_GPIO_Port GPIOA
#define ISCL_Pin GPIO_PIN_4
#define ISCL_GPIO_Port GPIOC
#define ADC3_Pin GPIO_PIN_5
#define ADC3_GPIO_Port GPIOC
#define U_BAT_Pin GPIO_PIN_0
#define U_BAT_GPIO_Port GPIOB
#define I_BUS_Pin GPIO_PIN_1
#define I_BUS_GPIO_Port GPIOB
#define ADC4_Pin GPIO_PIN_2
#define ADC4_GPIO_Port GPIOB
#define TX_RX_Pin GPIO_PIN_10
#define TX_RX_GPIO_Port GPIOB
#define RX_TX_Pin GPIO_PIN_11
#define RX_TX_GPIO_Port GPIOB
#define LED2_Pin GPIO_PIN_12
#define LED2_GPIO_Port GPIOB
#define LED1_Pin GPIO_PIN_13
#define LED1_GPIO_Port GPIOB
#define PWM6_Pin GPIO_PIN_8
#define PWM6_GPIO_Port GPIOC
#define PWM5_Pin GPIO_PIN_9
#define PWM5_GPIO_Port GPIOC
#define ISDA_Pin GPIO_PIN_8
#define ISDA_GPIO_Port GPIOA
#define KEYIN2_Pin GPIO_PIN_9
#define KEYIN2_GPIO_Port GPIOA
#define KEYIN1_Pin GPIO_PIN_10
#define KEYIN1_GPIO_Port GPIOA
#define BDM_Pin GPIO_PIN_11
#define BDM_GPIO_Port GPIOA
#define BDP_Pin GPIO_PIN_12
#define BDP_GPIO_Port GPIOA
#define DIO_Pin GPIO_PIN_13
#define DIO_GPIO_Port GPIOA
#define CLK_Pin GPIO_PIN_14
#define CLK_GPIO_Port GPIOA
#define LCD_RES_Pin GPIO_PIN_15
#define LCD_RES_GPIO_Port GPIOA
#define LCD_SCK_Pin GPIO_PIN_10
#define LCD_SCK_GPIO_Port GPIOC
#define LCD_DC_Pin GPIO_PIN_11
#define LCD_DC_GPIO_Port GPIOC
#define LCD_SDA_Pin GPIO_PIN_12
#define LCD_SDA_GPIO_Port GPIOC
#define CSN_Pin GPIO_PIN_2
#define CSN_GPIO_Port GPIOD
#define SCK_Pin GPIO_PIN_3
#define SCK_GPIO_Port GPIOB
#define MISO_Pin GPIO_PIN_4
#define MISO_GPIO_Port GPIOB
#define MOSI_Pin GPIO_PIN_5
#define MOSI_GPIO_Port GPIOB
#define LCD_CS_Pin GPIO_PIN_6
#define LCD_CS_GPIO_Port GPIOB
#define LCD_BLK_Pin GPIO_PIN_7
#define LCD_BLK_GPIO_Port GPIOB
#define CAN_RX_Pin GPIO_PIN_8
#define CAN_RX_GPIO_Port GPIOB
#define CAN_TX_Pin GPIO_PIN_9
#define CAN_TX_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
