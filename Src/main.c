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
#include "command.h"
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
uint8_t rxBuffer[100];
char msg[100];
char time_text[20];
uint32_t time_in_sec=0;
uint8_t currentBackground = 6;
/* USER CODE END PV */

uint16_t year=2019;
uint8_t month=12;
uint8_t day=14;

uint32_t time_when_start_setting;
uint8_t mode=0; // 0: sim clock disp, 1: digital clock disp, 2: setting
uint8_t sub_mode=0; // submode setting

int setting_values[6]; // values to be set in setting mode

uint8_t month_days[] = {0, 31, 30, 28, 30, 31, 30, 31, 31, 30, 31, 30, 31}; // days in month

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
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
    update_month_day_arr_by_year(year);
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
//        sprintf(m, "hello, y=%d", y++);
      POINT_COLOR = BLACK;
      LCD_ShowString(0, 0, 200, 16, 16, (uint8_t*) "        ");
      LCD_ShowString(0, 0, 200, 16, 16, (uint8_t*) time_text);
//      LCD_ShowString(220,300, 200,16,16, (uint8_t*) "H");
//        setting_display();

        if(mode==0 || mode==1){
             time_display_for_debug();
        } else {
            setting_display();
        }

      HAL_Delay(500);
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

// date time util
uint8_t year_is_leap(uint16_t year){
    if(year%4!=0) return 0;
    if(year%100==0 && year%400!=0) return 0;
    return 1;
}

void update_month_day_arr_by_year(uint16_t year){
    if(year_is_leap(year)==1){
        // leap_year
        month_days[2] = 29;
    } else {
        month_days[2] = 28;
    }
}

void move_day_forward(){
    update_month_day_arr_by_year(year);
    day++;
    if(day > month_days[month]){
        day = 1;
        month++;
    }
    if(month>12){
        month = 1;
        year++;
    }
}

uint8_t day_is_valid(uint16_t year, uint8_t month, uint8_t day){
    update_month_day_arr_by_year(year);

    if( (1<=day && day<=month_days[month])  ){
        return 1;
    } else {
        return 0;
    }

}

// settings related

// freeze values for entering setting mode
void freeze_values_for_setting(){
    // prepare frozen time
    uint8_t hour, minute, second;
    hour = time_in_sec / (60 * 60);
    minute = time_in_sec / 60  - hour*60;
    second = time_in_sec % 60;

    // put things into setting values
    setting_values[0] = year;
    setting_values[1] = month;
    setting_values[2] = day;
    setting_values[3] = hour;
    setting_values[4] = minute;
    setting_values[5] = second;
}

void save_set_value_back(){
    year = setting_values[0];
    month = setting_values[1];
    day = setting_values[2];
    time_in_sec = (setting_values[3] * 60 * 60 + setting_values[4] * 60 + setting_values[5]);
}



void setting_highlight_disp(int i){
    if(sub_mode==i){
        POINT_COLOR = RED;
    } else{
        POINT_COLOR = BLACK;
    }
}

// display the setting page
// TODO: highlight the value being set.
void setting_display(){
    LCD_Fill(0, 30, 240, 290, WHITE);

    char tmpStr[30];
    // print hints
    LCD_ShowString(60, 50, 200, 16, 16, (uint8_t*)"Date/Time Set");
    LCD_ShowString(20, 250, 300, 16, 16, (uint8_t*)"[Next] [+] [-]");


    // display date
    sprintf(tmpStr, "%04d", setting_values[0]);
    setting_highlight_disp(0);
    LCD_ShowString(60, 70, 100, 16, 16, (uint8_t*) tmpStr);
    sprintf(tmpStr, "%02d", setting_values[1]);
    setting_highlight_disp(1);
    LCD_ShowString(100, 70, 100, 16, 16, (uint8_t*) tmpStr);
    sprintf(tmpStr, "%02d", setting_values[2]);
    setting_highlight_disp(2);
    LCD_ShowString(120, 70, 100, 16, 16, (uint8_t*) tmpStr);



    // display time
    sprintf(tmpStr, "%02d", setting_values[3]);
    setting_highlight_disp(3);
    LCD_ShowString(60, 90, 100, 16, 16, (uint8_t*) tmpStr);
    sprintf(tmpStr, "%02d", setting_values[4]);
    setting_highlight_disp(4);
    LCD_ShowString(80, 90, 100, 16, 16, (uint8_t*) tmpStr);
    sprintf(tmpStr, "%02d", setting_values[5]);
    setting_highlight_disp(5);
    LCD_ShowString(100, 90, 100, 16, 16, (uint8_t*) tmpStr);

    // debug show current sub mode
    sprintf(tmpStr, "sub=%d", sub_mode);
    LCD_ShowString(0, 300, 200, 16, 16, (uint8_t*) tmpStr);

}

void time_display_for_debug(){

//    LCD_DrawRectangle(0, 30, 240, 290);
    LCD_Fill(0, 30, 240, 290, YELLOW);

    char tmpStr[30];
    // print hints
    LCD_ShowString(60, 50, 200, 16, 16, (uint8_t*)"NORMAL DISP!");
    if(mode==0){
        LCD_ShowString(60, 30, 200, 16, 16, (uint8_t*)"SIM");
    } else {
        LCD_ShowString(60, 30, 200, 16, 16, (uint8_t*)"DIGI");
    }
    LCD_ShowString(20, 250, 300, 16, 16, (uint8_t*)"[Tg] [Set] [Dis]");


    // display date
    sprintf(tmpStr, "%04d", year);
    LCD_ShowString(60, 70, 100, 16, 16, (uint8_t*) tmpStr);
    sprintf(tmpStr, "%02d", month);
    LCD_ShowString(100, 70, 100, 16, 16, (uint8_t*) tmpStr);
    sprintf(tmpStr, "%02d", day);
    LCD_ShowString(120, 70, 100, 16, 16, (uint8_t*) tmpStr);

    uint8_t hour, minute, second;
    hour = time_in_sec / (60 * 60);
    minute = time_in_sec / 60  - hour*60;
    second = time_in_sec % 60;

    // display time
    sprintf(tmpStr, "%02d", hour);
    LCD_ShowString(60, 90, 100, 16, 16, (uint8_t*) tmpStr);
    sprintf(tmpStr, "%02d", minute);
    LCD_ShowString(80, 90, 100, 16, 16, (uint8_t*) tmpStr);
    sprintf(tmpStr, "%02d", second);
    LCD_ShowString(100, 90, 100, 16, 16, (uint8_t*) tmpStr);

    // debug show current sub mode
    sprintf(tmpStr, "sub=%d", sub_mode);
    LCD_ShowString(0, 300, 200, 16, 16, (uint8_t*) tmpStr);

};

// utils

void send_message_invoke(){
    HAL_UART_Transmit(&huart1, (uint8_t *) msg, strlen(msg), HAL_MAX_DELAY);
}

void update_screen(){
    switch (currentBackground) {
        case 0:
            LCD_Clear(WHITE);
            BACK_COLOR = WHITE;
            break;
        case 1:
            LCD_Clear(BLACK);
            BACK_COLOR = BLACK;
            break;
        case 2:
            LCD_Clear(BLUE);
            BACK_COLOR = BLUE;
            break;
        case 3:
            LCD_Clear(RED);
            BACK_COLOR = RED;
            break;
        case 4:
            LCD_Clear(MAGENTA);
            BACK_COLOR = MAGENTA;
            break;
        case 5:
            LCD_Clear(GREEN);
            BACK_COLOR = GREEN;
            break;
        case 6:
            LCD_Clear(CYAN);
            BACK_COLOR = CYAN;
            break;
        case 7:
            LCD_Clear(YELLOW);
            BACK_COLOR = YELLOW;
            break;
        case 8:
            LCD_Clear(BRRED);
            BACK_COLOR = BRRED;
            break;
        case 9:
            LCD_Clear(GRAY);
            BACK_COLOR = GRAY;
            break;
        case 10:
            LCD_Clear(LGRAY);
            BACK_COLOR = LGRAY;
            break;
        case 11:
            LCD_Clear(BROWN);
            BACK_COLOR = BROWN;
            break;
    }
    POINT_COLOR = RED;
    LCD_ShowString(30, 40, 200, 24, 24, (uint8_t*) "Mini STM32 ^_^");
    LCD_ShowString(30, 70, 200, 16, 16, (uint8_t*) "TFTLCD TEST");
    POINT_COLOR = BLACK;
    LCD_DrawRectangle(30, 150, 210, 190);
    LCD_Fill(31, 151, 209, 189, YELLOW);
//    x++;
//    if (x == 12)
//        x = 0;
//    HAL_Delay(2000);
}

// change things


void set_year(uint8_t change){
    if(change==1){
        setting_values[0]++;
    } else {
        setting_values[0]--;
    }

    if(setting_values[0]<2012){
        setting_values[0] = 2012;
    } else if (setting_values[0] > 2021){
        setting_values[0] = 2021;
    }
}

void set_month(uint8_t change){
    if(change==1){
        setting_values[1]++;
    } else {
        setting_values[1]--;
    }

    setting_values[1] = setting_values[1] % 12;
    if(setting_values[1]==0){
        setting_values[1] = 12;
    }

    if( setting_values[1]==2 && setting_values[2] > 28 ){
        // if feb, put bay inside
        setting_values[2] = 28;
    }

    update_month_day_arr_by_year(setting_values[0]);

    // prevent day exceed
    if (setting_values[2] > month_days[setting_values[1]]){
        setting_values[2] = month_days[setting_values[1]];
    }
}



void set_day(uint8_t change){
    int before_day = setting_values[2];

    if(change==1){
        setting_values[2]++;
    } else {
        setting_values[2]--;
    }

    // day validity check
    if(day_is_valid(setting_values[0], setting_values[1], setting_values[2])!=1){
        setting_values[2] = before_day;
    }
}

void set_hour(uint8_t change){
    if(change==1){
        setting_values[3]++;
    } else {
        setting_values[3]--;
    }

    setting_values[3] = (setting_values[3]+24) % 24;
}

void set_minute(uint8_t change){
    if(change==1){
        setting_values[4]++;
    } else {
        setting_values[4]--;
    }

    setting_values[4] = (setting_values[4]+60) % 60;
}

void set_second(uint8_t change){
    if(change==1){
        setting_values[5]++;
    } else {
        setting_values[5]--;
    }

    setting_values[5] = (setting_values[5]+60) % 60;
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
            parse_command(uRx_Data);
            memset(uRx_Data, 0 , sizeof(uRx_Data));
            //HAL_UART_Transmit(&huart1, uRx_Data, uLength, 0xffff);
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
                switch (mode){
                    case 0:
                    case 1: // disable alarm/countdown notification
                        break;
                    case 2: // decrease current value
                        //
                        switch (sub_mode){
                            case 0: set_year(2); break;
                            case 1: set_month(2); break;
                            case 2: set_day(2); break;
                            case 3: set_hour(2); break;
                            case 4: set_minute(2); break;
                            case 5: set_second(2); break;
                        }
                        break;
                }

            }
            break;
        case KEY1_Pin:
            if (HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin) == GPIO_PIN_RESET) {
                update_screen();

                switch (mode){
                    case 0:
                    case 1: // enter setting mode in both sim/digi clock
                        mode = 2;
                        freeze_values_for_setting();
                        break;
                    case 2: // increase current value
                        //
                        switch (sub_mode){
                            case 0: set_year(1); break;
                            case 1: set_month(1); break;
                            case 2: set_day(1); break;
                            case 3: set_hour(1); break;
                            case 4: set_minute(1); break;
                            case 5: set_second(1); break;
                        }
                        break;
                }

            }
            break;
        case KEY_WK_Pin:
            if (HAL_GPIO_ReadPin(KEY_WK_GPIO_Port, KEY_WK_Pin) == GPIO_PIN_SET) {

                update_screen();

                switch (mode){
                    case 0: // sim clock
                        mode = 1;
                        break;
                    case 1: // digi clock
                        mode = 0;
                        break;
                    case 2: // setting
                        if(sub_mode<5){ // need to set next
                            sub_mode++; // move to next
                        } else { // finished setting
                            save_set_value_back(); // save values back
                            sub_mode = 0;
                            mode = 0; // back to sim
                        }
                        break;
                }

            }
            break;
        default:
            break;
    }
    if(currentBackground<0){
        currentBackground += 12;
    } else if(currentBackground>=12){
        currentBackground -= 12;
    }
    sprintf(msg, "BG: %d\r\n", currentBackground);
    send_message_invoke();
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
    if(htim->Instance==TIM3){
//        sprintf(time_text, "sec=%lu, mode=%d, sub=%d", time_in_sec, mode, sub_mode);
        sprintf(time_text, "sec=%lu", time_in_sec);
        time_in_sec++;

        // reset time for new day
        if(time_in_sec == (24*60*60)){
            move_day_forward();
            time_in_sec = 0;
        }

        sprintf(msg, "sec=%lu, mode=%d, sub=%d", time_in_sec, mode, sub_mode);
//        sprintf(msg, "sec=%lu", time_in_sec);
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
