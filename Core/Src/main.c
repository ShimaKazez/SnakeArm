/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
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
#include "main.h"
#include "adc.h"
#include "dac.h"
#include "dma.h"
#include "fdcan.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "usbd_cdc_if.h"
#include "math.h"
#include "arm_math.h"
#include "GUI_Paint.h"
#include "fonts.h"
#include "image.h"
#include "LCD_1in14.h"
#include "Vofa+.h"
#include "UI.h"
#include "ADC_Sample.h"
#include "NTC.h"
#include "FOC.h"
#include "CAN_Com.h"
#include "DrEmpower_can.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
extern volatile uint8_t Mset_Pattern[3];
extern volatile float Mset_Data[3];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
//FLASH_OBProgramInitTypeDef OptionsByteStruct;
/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {

	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */
	//nSWBOOT0 Setting
	/*HAL_FLASH_Unlock();
	 HAL_FLASH_OB_Unlock();
	 HAL_FLASHEx_OBGetConfig(&OptionsByteStruct);
	 OptionsByteStruct.OptionType = OPTIONBYTE_USER;
	 OptionsByteStruct.USERType = OB_USER_nBOOT0;
	 OptionsByteStruct.USERConfig = OB_nBOOT0_SET;
	 HAL_FLASHEx_OBProgram(&OptionsByteStruct);
	 OptionsByteStruct.OptionType = OPTIONBYTE_USER;
	 OptionsByteStruct.USERType = OB_USER_nSWBOOT0;
	 OptionsByteStruct.USERConfig = OB_BOOT0_FROM_OB;
	 HAL_FLASHEx_OBProgram(&OptionsByteStruct);
	 HAL_FLASH_OB_Launch();
	 HAL_FLASH_OB_Lock();
	 HAL_FLASH_Lock();*/

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_DMA_Init();
	MX_ADC1_Init();
	MX_ADC2_Init();
	MX_DAC1_Init();
	MX_TIM2_Init();
	MX_FDCAN1_Init();
	MX_SPI1_Init();
	MX_SPI3_Init();
	MX_TIM1_Init();
	MX_TIM3_Init();
	MX_USART3_UART_Init();
	MX_USB_Device_Init();
	MX_TIM17_Init();
	/* USER CODE BEGIN 2 */
	Can_Config(); //Can配置信息+
	vofa_start();

	HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);
	HAL_ADCEx_Calibration_Start(&hadc2, ADC_SINGLE_ENDED);
	HAL_ADC_Start(&hadc1);
	HAL_ADC_Start(&hadc2);

	UI_Init();

	int TC_INIT_Flag = 0;
	//int TC_INIT_Flag_1 = 0;
	struct angle_speed_torque angle_speed_torque_1 = { 0, 0, 0 };
	struct angle_speed_torque angle_speed_torque_2 = { 0, 0, 0 };
	struct angle_speed_torque angle_speed_torque_3 = { 0, 0, 0 };

	HAL_TIM_Base_Start_IT(&htim17);

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {
		ADC_Read();
		//TensionSensor[0] = (float) ADC_Value2[1];
		//TensionSensor[1] = (float) ADC_Value2[2];
		//TensionSensor[2] = (float) ADC_Value1[7];
		//Home.status[0].num1 = 3.3 * 16 * (float) ADC_Value2[6] / 4096;
		//Home.status[1].num1 = (float) NTC_Cov(ADC_Value1[6]);
		//Home.status[2].num1 = (float) NTC_Cov(ADC_Value2[7]);
		//Parameters_Reflash_Flag = 1;
		/* for DJI C620 with M3508
		 FDCAN_Receive();
		 Home.params[0].num1 = (float) C620_Status.Angle * 360 / 8191;
		 if ((float) C620_Status.Speed <= 32768)
		 Home.params[1].num1 = (float) C620_Status.Speed;
		 else
		 Home.params[1].num1 = 65535 - (float) C620_Status.Speed;

		 if ((float) C620_Status.Current <= 32768)
		 Home.params[2].num1 = (float) C620_Status.Current;
		 else
		 Home.params[2].num1 = 65535 - (float) C620_Status.Current;

		 Home.params[3].num1 = (float) C620_Status.Temp;
		 */
		KEY_Scan();

		if (Program_Flag[0]) {
			//Home.params[1].num2 = 600;
			//C620_Control.Current3 = PID_realize(Home.params[1].num2, Home.params[1].num1);
			//Home.params[2].num2 = (float) C620_Control.Current3;
			//Targets_Reflash_Flag = 1;
			//FDCAN_Transmit();
			Parameters_Reflash_Flag = 1;
			HAL_GPIO_WritePin(GPIOC, LED1_Pin, RESET);
			HAL_GPIO_WritePin(GPIOC, LED2_Pin, RESET);
		} else {
			//pid.integral = 0;
		}
		if (Program_Flag[1]) {
			//FDCAN_Receive();
			HAL_GPIO_WritePin(GPIOC, LED1_Pin, RESET);
			HAL_GPIO_WritePin(GPIOC, LED2_Pin, SET);
		} else {
			//estop(1);
		}
		if (Program_Flag[2]) {
			if (!TC_INIT_Flag) {
				/////**************设置零点位置*************////////
				set_zero_position(0); //给 1 号关节设置零点
				/////**************开启角度、转速、力矩实时反馈*************////////
				enable_angle_speed_torque_state(1);
				set_state_feedback_rate_ms(1, 2);
				HAL_Delay(200);
				enable_angle_speed_torque_state(2);
				set_state_feedback_rate_ms(2, 2);
				HAL_Delay(200);
				enable_angle_speed_torque_state(3);
				set_state_feedback_rate_ms(3, 2);
				HAL_Delay(200);
				TC_INIT_Flag = 1;
			}
			angle_speed_torque_1 = angle_speed_torque_state(1);
			angle_speed_torque_2 = angle_speed_torque_state(2);
			angle_speed_torque_3 = angle_speed_torque_state(3);
			Home.params[0].num1 = angle_speed_torque_1.angle;
			Home.params[1].num1 = angle_speed_torque_1.speed;
			Home.params[2].num1 = angle_speed_torque_1.torque;
			for (int i = 0; i < 3; i++) {
				Home.params[i].num2 = Mset_Data[i];
				switch (Mset_Pattern[i]) {
				case 20:
					set_torque(i + 1, Mset_Data[i], 1, 0);
					break;
				case 16:
					set_angle(i + 1, Mset_Data[i], 0, 0, 1);
					break;
				case 22:
					set_speed(i + 1, Mset_Data[i], 1000, 1);
					break;
				default:
					estop(0);
				}
			}
			Drivers.driver1.angle = angle_speed_torque_1.angle;
			Drivers.driver1.speed = angle_speed_torque_1.speed;
			Drivers.driver1.torque = angle_speed_torque_1.torque;
			Drivers.driver2.angle = angle_speed_torque_2.angle;
			Drivers.driver2.speed = angle_speed_torque_2.speed;
			Drivers.driver2.torque = angle_speed_torque_2.torque;
			Drivers.driver3.angle = angle_speed_torque_3.angle;
			Drivers.driver3.speed = angle_speed_torque_3.speed;
			Drivers.driver3.torque = angle_speed_torque_3.torque;
			Parameters_Reflash_Flag = 1;

			HAL_GPIO_WritePin(GPIOC, LED1_Pin, SET);
			HAL_GPIO_WritePin(GPIOC, LED2_Pin, RESET);
		} else {
			estop(0);
		}
		if (Program_Flag[3]) {
			//C620_Control.Current3 = UART_RxBuffer[1] << 8 | UART_RxBuffer[0];
			//Home.params[2].num2 = UART_Rx.Rx.RxData1;
			//FDCAN_Transmit();
			//Targets_Reflash_Flag = 1;
			HAL_GPIO_WritePin(GPIOC, LED1_Pin, SET);
			HAL_GPIO_WritePin(GPIOC, LED2_Pin, SET);
		} else {
			//Home.params[2].num2 = 0;
			//C620_Control.Current3 = 0;
		}
		if (Program_Flag[0] || Program_Flag[1] || Program_Flag[2] || Program_Flag[3]) {
			HAL_GPIO_WritePin(GPIOC, LED3_Pin, SET);
		} else {
			HAL_GPIO_WritePin(GPIOC, LED3_Pin, RESET);
		}

		/*
		 vofa_send_data(0, Home.status[0].num1);
		 vofa_send_data(1, Home.status[1].num1);
		 vofa_send_data(2, Home.status[2].num1);
		 vofa_send_data(3, Home.params[0].num1);
		 vofa_send_data(3, Home.params[1].num1);
		 vofa_send_data(3, Home.params[2].num1);
		 vofa_send_data(3, Home.params[3].num1);
		 vofa_sendframetail();
		 */

		//FOC_Control();
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

	/** Configure the main internal regulator output voltage
	 */
	HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST);

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48 | RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;
	RCC_OscInitStruct.PLL.PLLN = 42;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
	RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK) {
		Error_Handler();
	}
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
	}
	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
