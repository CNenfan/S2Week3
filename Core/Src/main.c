/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "i2c.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "string.h"

#include "math.h"
#include "stdarg.h"
#include "OLED.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define PI 3.1415926f
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint8_t Receive_Data[1] = {0}; //接收数据数组
float data_test[1] = {0}; //接收数据数组
float t = 0.0f;
uint8_t receivedByte;


  void Vofa_FireWater(const char *format, ...)
  {
      uint8_t txBuffer[100];
      uint32_t n;
      va_list args;
      va_start(args, format);
      n = vsnprintf((char *)txBuffer, 100, format, args);


      HAL_UART_Transmit_IT(&huart1, (uint8_t *)txBuffer, n);
      //......................................

      va_end(args);
  }


  void Vofa_JustFloat(float *_data, uint8_t _num)
  {
      uint8_t tempData[100];
      uint8_t temp_end[4] = {0, 0, 0x80, 0x7F};
      float temp_copy[_num];

      memcpy(&temp_copy, _data, sizeof(float) * _num);

      memcpy(tempData, (uint8_t *)&temp_copy, sizeof(temp_copy));
      memcpy(&tempData[_num * 4], &temp_end[0], 4);


      HAL_UART_Transmit_IT(&huart1, tempData, (_num + 1) * 4);
      //......................................
  }

  char doItOnce = '0';

  uint32_t ADC_Value[2];


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

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_ADC1_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
  

  HAL_UART_Receive_IT(&huart1, Receive_Data, 1); //接收数据

  OLED_init();
  OLED_display_on();
  OLED_LOGO();
  HAL_Delay(1000);

  HAL_ADC_Start(&hadc1);
  HAL_ADCEx_Calibration_Start(&hadc1);



  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

    /* JustFloat */
    if(Receive_Data[0] == '1')
    {
      data_test[0] = 2*sin(t*PI);
      t+= 0.02f;
      if(doItOnce == '0')
      {
        OLED_operate_gram(PEN_CLEAR);
        doItOnce = '1';
        OLED_show_string(0, 0, "Sine");
        OLED_refresh_gram();
      }
    }
    else if(Receive_Data[0] == '2')
    {
      data_test[0] = 1*t - 0.5f;
      t+= 0.02f;
      if(doItOnce == '0')
      {
        OLED_operate_gram(PEN_CLEAR);
        doItOnce = '1';
        OLED_show_string(0, 0, "Sawtooth");
        OLED_refresh_gram();
      }
      if(t >= 1)
      {
        t = 0;
      }
    }
    else if(Receive_Data[0] == '3')
    {
      data_test[0] = 0.5*(t < 0.75 ? 1 : -1);
      t+= 0.01f;
      if(doItOnce == '0')
      {
        OLED_operate_gram(PEN_CLEAR);
        doItOnce = '1';
        OLED_show_string(0, 0, "Square");
        OLED_refresh_gram();
      }
      if(t >= 1.5)
      {
        t = 0;
      }
    }
    else if(Receive_Data[0] == '4')
    {
      for(int i=0;i<1;i++)  // 放弃y轴值，因为懒得改data_test的长度了
      {
        HAL_ADC_Start(&hadc1);
        HAL_ADC_PollForConversion(&hadc1,10);
        ADC_Value[i] = HAL_ADC_GetValue(&hadc1);
        data_test[i] = ADC_Value[i] * 3.3 / 4095;
      }
      if(doItOnce == '0')
      {
        OLED_operate_gram(PEN_CLEAR);
        doItOnce = '1';
        OLED_show_string(0, 0, "ADC");
        OLED_refresh_gram();
      }
    }
    
    Vofa_JustFloat(data_test, 1);
    HAL_Delay(10);



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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if(huart->Instance == USART1) 
  {
    if(Receive_Data[0] != '1' && Receive_Data[0] != '2' && Receive_Data[0] != '3' && Receive_Data[0] != '4')
    {
      Receive_Data[0] = receivedByte;
    }
    else
    {
      receivedByte = Receive_Data[0];
    }
    HAL_UART_Receive_IT(&huart1, Receive_Data, 1); //接收数据
    doItOnce = '0';
  }
}



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
  while (1)
  {
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
