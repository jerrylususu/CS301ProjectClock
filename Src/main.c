/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lcd.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define PI acos(-1)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t rxBuffer[100];
char msg[100];
char time_text[20];
uint32_t time_in_sec=0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void show_analogue(u_int32_t accumulative_second);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  LCD_Init();
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
    HAL_TIM_Base_Start_IT(&htim3);
    HAL_UART_Receive_IT(&huart1, (uint8_t *)rxBuffer, 1);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
    uint8_t y = 0;
    char m[20];
    sprintf(time_text, " ");
    while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

    show_analogue(69655);
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

  /** Initializes the CPU, AHB and APB busses clocks 
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
  /** Initializes the CPU, AHB and APB busses clocks 
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
}

/* USER CODE BEGIN 4 */

// utils

void switch_face(){

}

void show_calendar(u_int16_t year, u_int8_t month, u_int8_t day){

}

void show_analogue(u_int32_t accumulative_second){
    // get hour, minute, second
    u_int32_t hour = accumulative_second / 3600;
    u_int32_t minute = (accumulative_second % 3600) / 60;
    u_int32_t second = accumulative_second - 3600 * hour - 60 * minute;
    if (hour > 12) hour -= 12;
    u_int32_t x0 = 120;
    u_int32_t y0 = 120;
    double a_hour, a_min, a_sec;
    u_int32_t x_hour, y_hour, x_min, y_min, x_sec, y_sec;
    // get radian
    a_sec = second * 2 * PI / 60;
    a_min = minute * 2 * PI / 60 ;
    a_hour= hour * 2 * PI / 12 + a_min / 12;
    // get end locations of 3 pointers
    x_sec = 120 + (int)(85 * sin(a_sec));
    y_sec = 120 - (int)(85 * cos(a_sec));
    x_min = 120 + (int)(65 * sin(a_min));
    y_min = 120 - (int)(65 * cos(a_min));
    x_hour= 120 + (int)(45 * sin(a_hour));
    y_hour= 120 - (int)(45 * cos(a_hour));
    LCD_DrawPoint(x0, y0);
    LCD_Draw_Circle(x0, y0, 1);
    LCD_Draw_Circle(x0, y0, 2);
    LCD_Draw_Circle(x0, y0, 100);
    LCD_Draw_Circle(x0, y0, 101);
    POINT_COLOR = RED;
    LCD_DrawLine(x0, y0, x_sec, y_sec);
    POINT_COLOR = BLACK;
    LCD_DrawLine(x0, y0, x_min, y_min);
    LCD_DrawLine(x0, y0, x_hour, y_hour);
    for (int i = 0; i < 12; i++) {
        a_hour= i * 2 * PI / 12;
        LCD_DrawLine(120 + (int) (95 * sin(a_hour)), 120 - (int) (95 * cos(a_hour)), 120 + (int) (100 * sin(a_hour)),
                     120 - (int) (100 * cos(a_hour)));
    }
}

void show_digit(u_int32_t accumulative_second){
    u_int32_t hour = accumulative_second / 3600;
    u_int32_t minute = (accumulative_second % 3600) / 60;
    u_int32_t second = accumulative_second - 3600 * hour - 60 * minute;

}

void send_message_invoke(){
    HAL_UART_Transmit(&huart1, (uint8_t *) msg, strlen(msg), HAL_MAX_DELAY);
}

// system interrupt handlers & callbacks
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance==USART1)
    {
        static unsigned char uRx_Data[1024] = {0};
        static unsigned char uLength = 0;
        if(rxBuffer[0] == '\n')
        {
            HAL_UART_Transmit(&huart1, uRx_Data, uLength, 0xffff);
            uLength = 0;
        }
        else
        {
            uRx_Data[uLength] = rxBuffer[0];
            uLength++;
        }
    }
    HAL_UART_Receive_IT(&huart1, (uint8_t *)rxBuffer, 1);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    HAL_Delay(100);
    switch (GPIO_Pin) {
        case KEY0_Pin:
            if (HAL_GPIO_ReadPin(KEY0_GPIO_Port, KEY0_Pin) == GPIO_PIN_RESET) {

            }
            break;
        case KEY1_Pin:
            if (HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin) == GPIO_PIN_RESET) {

            }
            break;
        case KEY_WK_Pin:
            if (HAL_GPIO_ReadPin(KEY_WK_GPIO_Port, KEY_WK_Pin) == GPIO_PIN_SET) {

            }
            break;
        default:
            break;
    }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
    if(htim->Instance==TIM3){
        sprintf(time_text, "sec=%d", time_in_sec);
        time_in_sec++;

        sprintf(msg, "sec=%d", time_in_sec);
        send_message_invoke();
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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
