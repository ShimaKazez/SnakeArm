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
#include "dma.h"
#include "fdcan.h"
#include "i2c.h"
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
#include "GUI_Paint.h"
#include "fonts.h"
#include "image.h"
#include "LCD_1in14.h"
#include "Vofa+.h"
#include "UI.h"
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
long GetMicros(void) {
	//获取当前毫秒
	uint32_t m = HAL_GetTick();
	//获取嘀嗒定时器重装载值
	const uint32_t tms = SysTick->LOAD + 1;
	//获取当前滴答定时器计数值
	__IO uint32_t u = tms - SysTick->VAL;
	//返还对应的值
	return (long) (m * 1000 + (u * 1000) / tms);
}
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
//FLASH_OBProgramInitTypeDef OptionsByteStruct;
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

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
  MX_FDCAN1_Init();
  MX_SPI1_Init();
  MX_SPI3_Init();
  MX_TIM1_Init();
  MX_USART3_UART_Init();
  MX_USB_Device_Init();
  MX_TIM17_Init();
  MX_TIM16_Init();
  MX_I2C2_Init();
  MX_TIM3_Init();
  MX_TIM15_Init();
  /* USER CODE BEGIN 2 */
	Can_Config(); //Can配置信息
	//HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);
	vofa_start();

	HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);
	HAL_ADCEx_Calibration_Start(&hadc2, ADC_SINGLE_ENDED);
	HAL_ADC_Start(&hadc1);
	HAL_ADC_Start(&hadc2);

	UI_Init();

	int TC_INIT_Flag = 0;
	int Main_Program_Flag = 0;
	int P0_Long_Press_Counter = 0;
	struct angle_speed_torque angle_speed_torque_1 = { 0, 0, 0 };
	struct angle_speed_torque angle_speed_torque_2 = { 0, 0, 0 };
	struct angle_speed_torque angle_speed_torque_3 = { 0, 0, 0 };

	//int SystemCircleTimes = 0;
	//uint8_t SystemClock = 0;
	//long SystemTimer = 0;

	HAL_TIM_Base_Start_IT(&htim17);
	HAL_TIM_Base_Start_IT(&htim16);
	HAL_TIM_Base_Start_IT(&htim15);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1) {
		/*//系统占用率计算
		 SystemCircleTimes++;
		 if ((float) (HAL_GetTick() - SystemClock) > 1000) {
		 SystemOccupancy = 1 - ((float) (SystemTimer / 1000) / (float) (HAL_GetTick() - SystemClock));
		 SystemClock = HAL_GetTick();
		 SystemCircleTimesRecord = SystemCircleTimes;
		 SystemCircleTimes = 0;

		 }
		 long SystemTimerLast = GetMicros();*/

		TensionSensor[0] = (float) ADC_Value2[0] / 4096 * 3.3;	//读取传感器信息
		TensionSensor[1] = (float) ADC_Value2[1] / 4096 * 3.3;
		TensionSensor[2] = (float) ADC_Value2[2] / 4096 * 3.3;
		Home.status[0].num2 = (float) ADC_Value1[0] / 4096 * 26.4;
		Home.status[1].num2 = (float) ADC_Value1[1] / 4096 * 5;
		//Home.status[2].num2 = SystemOccupancy * 100;
		Home.status[0].num1 = TensionSensor[0];
		Home.status[1].num1 = TensionSensor[1];
		Home.status[2].num1 = TensionSensor[2];
		Parameters_Reflash_Flag = 1;

		KEY_Scan();

		if (Motor_Monitor_FLAG) {
			if (!Program_Flag[1]) {	//主程序未执行下的操作，通常用于测试
				if (Program_Flag[0]) {
					Home.flag.Label = "[TEST]";
					Home.flag.Color = CYAN;
					switch (P0_Long_Press_Counter) {
					case 0:
						Home.mode.Label = "TestPrg1";
						Home.mode.Color = LIGHTBLUE;
						Status_Reflash_Flag = 1;
						break;
					case 200:
						Home.mode.Label = "TestPrg2";
						Home.mode.Color = LIGHTBLUE;
						Status_Reflash_Flag = 1;
						break;
					case 400:
						Home.mode.Label = "TestPrg3";
						Home.mode.Color = LIGHTBLUE;
						Status_Reflash_Flag = 1;
						break;
					case 600:
						Home.mode.Label = "Exit";
						Home.mode.Color = GREEN;
						Status_Reflash_Flag = 1;
					case 800:
						P0_Long_Press_Counter = 0;
						break;
					default:
					}
					P0_Long_Press_Counter++;
				} else {	//解算工作状态
					if (P0_Long_Press_Counter != 0) {	//测试子程序，仅执行一次
						if (P0_Long_Press_Counter > 0 && P0_Long_Press_Counter < 200) {
							//测试程序1
						} else if (P0_Long_Press_Counter > 200 && P0_Long_Press_Counter < 400) {
							//测试程序2
						} else if (P0_Long_Press_Counter > 400 && P0_Long_Press_Counter < 600) {
							//测试程序3
						} else {

						}
						Home.flag.Label = "[READY]";
						Home.flag.Color = GREEN;
						Home.mode.Label = "RdToWork";
						Home.mode.Color = WHITE;
						P0_Long_Press_Counter = 0;	//清空长按计数器
					}
				}
			}
			if (Program_Flag[1]) {
				if (!TC_INIT_Flag) {
					/////**************设置零点位置*************////////
					set_zero_position(0); //给关节设置零点
					/////**************开启角度、转速、力矩实时反馈*************////////
					enable_angle_speed_torque_state(0);
					set_state_feedback_rate_ms(0, 20);
					HAL_Delay(200);
					TC_INIT_Flag = 1;
					for (int i = 0; i < 3; i++) {
						Mset_Pattern[i] = 20;					//初始化为力矩模式 设置力矩为零
						Mset_Data[i] = 0;
					}
				}
				if (Program_Flag[0]) {
					if (P0_Long_Press_Counter == 0) {
						//短按软归零
						set_zero_position_temp(0);
						Home.mode.Label = "ZeroSetted";
						Home.mode.Color = GREEN;
						Status_Reflash_Flag = 1;
						for (int i = 0; i < 3; i++) {
							Mset_Pattern[i] = 16;					//初始化为位置模式 设置位置为零
							Mset_Data[i] = 0;
						}
						P0_Long_Press_Counter++;
					} else if (P0_Long_Press_Counter > 0 && P0_Long_Press_Counter < 100) {
						P0_Long_Press_Counter++;
					} else {
						//长按子程序
						//set_zero_position_temp(0);
						Home.mode.Label = "LongPressed";
						Home.mode.Color = GREEN;
						Status_Reflash_Flag = 1;
					}
				} else {
					P0_Long_Press_Counter = 0;					//清空长按计数器
				}

				angle_speed_torque_1 = angle_speed_torque_state(1);
				angle_speed_torque_2 = angle_speed_torque_state(2);
				angle_speed_torque_3 = angle_speed_torque_state(3);
				float Angle_Data[] = { angle_speed_torque_1.angle, angle_speed_torque_2.angle, angle_speed_torque_3.angle };
				float Speed_Data[] = { angle_speed_torque_1.speed, angle_speed_torque_2.speed, angle_speed_torque_3.speed };
				float Torque_Data[] = { angle_speed_torque_1.torque, angle_speed_torque_2.torque, angle_speed_torque_3.torque };
				for (int i = 0; i < 3; i++) {
					Home.params[i].num2 = Mset_Data[i];
					switch (Mset_Pattern[i]) {
					case 20:
						if (Mset_Data[i] > 1.2) {
							estop(0);
							Home.flag.Label = "ERROR";
							Home.flag.Color = RED;
							Home.mode.Label = "OverTorque";					//力矩软限制
							Home.mode.Color = YELLOW;
							Status_Reflash_Flag = 1;
							Program_Flag[1] = 0;
							HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, RESET);
						}
						set_torque(i + 1, Mset_Data[i], 1, 0);
						Home.params[i].num1 = Torque_Data[i];
						Paint_DrawString_EN(125, (63 + i * 18), "T->", &Font16, BLACK, GBLUE);
						HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, SET);
						break;
					case 16:
						set_angle(i + 1, Mset_Data[i], 10, 10, 1);
						Home.params[i].num1 = Angle_Data[i];
						Paint_DrawString_EN(125, (63 + i * 18), "P->", &Font16, BLACK, GBLUE);
						HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, SET);
						break;
					case 22:
						set_speed(i + 1, Mset_Data[i], 1000, 1);
						Home.params[i].num1 = Speed_Data[i];
						Paint_DrawString_EN(125, (63 + i * 18), "V->", &Font16, BLACK, GBLUE);
						HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, SET);
						break;
					default:
						estop(0);
						Home.flag.Label = "ERROR";
						Home.flag.Color = RED;
						Home.mode.Label = "SignalLost";					//信号格式限制
						Home.mode.Color = YELLOW;
						Status_Reflash_Flag = 1;
						Program_Flag[1] = 0;
						HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, RESET);
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

			} else {
				HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, RESET);
				if (TC_INIT_Flag) {
					estop(0);
				}
			}
			/*
			 if (Program_Flag[0]) {					//LED控制
			 HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, SET);
			 } else {
			 HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, RESET);
			 }*/

			Motor_Monitor_FLAG = 0;

			HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, Main_Program_Flag);	//主循环工作标识
			Main_Program_Flag = !Main_Program_Flag;
		}

		//SystemTimer += (GetMicros() - SystemTimerLast);

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	}
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;
  RCC_OscInitStruct.PLL.PLLN = 28;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV8;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
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
