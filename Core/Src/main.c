/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "GPIO_Handmade.h"
#include "RCC_Handmade.h"
#include "ADC_Handmade.h"
#include "DMA_Handmade.h"
#include "Timer_Handmade.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint32_t map(int32_t x, int32_t in_min, int32_t in_max, int32_t out_min, int32_t out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
uint8_t pwmValue;
/* USER CODE END 0 */
uint16_t adc_buffer[16];

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

  /* Initialize all configured peripherals */
	Enable_Clock_RCC_APB2(IOPA_EN_CLK);
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */
	RCC_config_t rccconfig ={
		.source_clock = HSE_CLOCK,
		.frequency = FREQ_72MHZ
	};
	init_RCC(&rccconfig);
//	
	
	
	GPIO_config_t gpioled ={
		.port = Port_C,
		.pin = PIN_13,
		.mode = OUTPUT_MODE_2_MHZ,
		.cnf_mode = CNF_MODE_00
	};
	InitGPIO(&gpioled);
	
	
	ADC_EnableClock(ADC_CLOCK_DIV_6);
	
	ADC_Config_t adcConfig = {
		.RegularSequence = {ADC_CHANNEL_8, ADC_CHANNEL_9},  // Ch?n 2 kênh (PA0 và PA1)
		.num_of_conversions = 2,
    .conversion_mode = CONTINUOUS_CONVERSION,
    .sample_time = SAMPLE_239_5_CYCLES,  // Ch?n th?i gian l?y m?u thích h?p
		.dma_mode = DMA_ENABLE,
		.scan_mode = SCAN_ENABLE
  };
	
  ADC_Init(&adcConfig);
	


	DMA_Config_t dmaADC = {
    .Channel = DMA_CHANNEL_1, // DMA1 Channel 1
    .Priority = DMA_HIGH_PRIORITY,
    .PeripheralIncrement = DISABLE_DMA_PINC,
    .MemoryIncrement = ENABLE_DMA_MINC,
    .PeripheralSize = DMA_PSIZE_16BIT,
    .MemorySize = DMA_MSIZE_16BIT,
    .CircularMode = DMA_CIRC_ENABLE, // L?p vô h?n
    .Direction = DMA_DIR_PERIPH_TO_MEM,
    .TransferErrorInt = DMA_TEIE_DISABLE,
    .HalfTransferInt = DMA_HTIE_DISABLE,
    .TransferCompleteInt = DMA_TCIE_DISABLE,
    .PeripheralAddress = (uint32_t)&ADC1_HANDMADE->ADC_DR, // Ð?c t? ADC
    .MemoryAddress = (uint32_t)adc_buffer,    // Luu vào m?ng
    .NumData = 2 // Ð?c 16 m?u ADC tru?c khi d?ng (ho?c reset n?u Circular Mode)
	};
	DMA_Init(&dmaADC);
	DMA_Start(DMA_CHANNEL_1);
	ADC_DMA_Start();
  /* USER CODE BEGIN WHILE */
	
	
	 GPIO_config_t GPIO_PWM = {
        .port = Port_A,
        .pin = PIN_0,
        .mode = OUTPUT_MODE_50_MHZ,
        .cnf_mode = CNF_MODE_10 // Alternate Function Push-Pull
    };
	InitGPIO(&GPIO_PWM);
		
	TIM_Config_t pwmconfig = {
		.Timer = TIMER_2,
		.ValueCount = 0,
		.ValueAutoReload = 1000,
		.DutyCycle = 100,
		.DesiredFrequency = 1000000UL,
		.autoReloadPreload = Custom_ARPE_BUFFERED,
		.counterEnable = Custom_CEN_ON
	};
	initTimer(&pwmconfig);
	StartPWMNormal(TIMER_2,CHANNEL_TIMER_1);
	
  while (1)
  {
		pwmValue = (uint8_t)map(adc_buffer[0],0,4096,0,100);
    /* USER CODE END WHILE */
		//ADC_ReadMultiple(&adcConfig);
//		for (uint8_t duty = 0; duty <= 100; duty++) {
//        //TIM2_HANDMADE->TIM_CCR1 = calculateDutyCycle(1000, duty);
//			SetDutyCycle(TIMER_2,CHANNEL_TIMER_1,duty);
//        HAL_Delay(50); // Gi?m delay d? thay d?i mu?t hon
//    }
//		for (uint8_t duty = 100; duty >0 ; duty--) {
//        SetDutyCycle(TIMER_2,CHANNEL_TIMER_1,duty);
//        HAL_Delay(50); // Gi?m delay d? thay d?i mu?t hon
//    }
		//HAL_Delay(1000);
		SetDutyCycle(TIMER_2,CHANNEL_TIMER_1,pwmValue);
     HAL_Delay(50); // Gi?m delay d? thay d?i mu?t hon
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}


