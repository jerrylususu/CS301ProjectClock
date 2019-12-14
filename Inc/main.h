/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
typedef struct {
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
} my_time;
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

// date time util
uint8_t year_is_leap(uint16_t year);
void update_month_day_arr_by_year(uint16_t year);
void move_day_forward();
uint8_t day_is_valid(uint16_t year, uint8_t month, uint8_t day);

// settings related
void freeze_values_for_setting();
void save_set_value_back();
void setting_highlight_disp(int i);

// display
void setting_display();
void time_display_for_debug();

// utils
void send_message_invoke();

// test only - deprecated
void update_screen();

// really set
void set_year(uint8_t change);
void set_month(uint8_t change);
void set_day(uint8_t change);
void set_hour(uint8_t change);
void set_minute(uint8_t change);
void set_second(uint8_t change);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define KEY_WK_Pin GPIO_PIN_0
#define KEY_WK_GPIO_Port GPIOA
#define KEY_WK_EXTI_IRQn EXTI0_IRQn
#define KEY0_Pin GPIO_PIN_5
#define KEY0_GPIO_Port GPIOC
#define KEY0_EXTI_IRQn EXTI9_5_IRQn
#define LED0_Pin GPIO_PIN_8
#define LED0_GPIO_Port GPIOA
#define KEY1_Pin GPIO_PIN_15
#define KEY1_GPIO_Port GPIOA
#define KEY1_EXTI_IRQn EXTI15_10_IRQn
#define LED1_Pin GPIO_PIN_2
#define LED1_GPIO_Port GPIOD
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
