/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#include "stm32f4xx_hal.h"

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

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define OLED_RS_Pin GPIO_PIN_1
#define OLED_RS_GPIO_Port GPIOA
#define USART_TX_Pin GPIO_PIN_2
#define USART_TX_GPIO_Port GPIOA
#define USART_RX_Pin GPIO_PIN_3
#define USART_RX_GPIO_Port GPIOA
#define OLED_DC_Pin GPIO_PIN_4
#define OLED_DC_GPIO_Port GPIOA
#define BUZZER_Pin GPIO_PIN_5
#define BUZZER_GPIO_Port GPIOA
#define OLED_CS_Pin GPIO_PIN_2
#define OLED_CS_GPIO_Port GPIOB
#define OLED_SCK_Pin GPIO_PIN_10
#define OLED_SCK_GPIO_Port GPIOB
#define TEMP_SCK_Pin GPIO_PIN_12
#define TEMP_SCK_GPIO_Port GPIOB
#define TEMP_ADD_Pin GPIO_PIN_13
#define TEMP_ADD_GPIO_Port GPIOB
#define TEMP_ADU_Pin GPIO_PIN_14
#define TEMP_ADU_GPIO_Port GPIOB
#define OLED_MOSI_Pin GPIO_PIN_15
#define OLED_MOSI_GPIO_Port GPIOB
#define Heater_Bottom_Pin GPIO_PIN_7
#define Heater_Bottom_GPIO_Port GPIOC
#define Heater_Top_Pin GPIO_PIN_8
#define Heater_Top_GPIO_Port GPIOC
#define DCFAN_Pin GPIO_PIN_9
#define DCFAN_GPIO_Port GPIOC
#define Motor2_Pin GPIO_PIN_8
#define Motor2_GPIO_Port GPIOA
#define Motor1_Pin GPIO_PIN_9
#define Motor1_GPIO_Port GPIOA
#define BT_RX_Pin GPIO_PIN_10
#define BT_RX_GPIO_Port GPIOA
#define LAMP_Pin GPIO_PIN_11
#define LAMP_GPIO_Port GPIOA
#define BT_TX_Pin GPIO_PIN_15
#define BT_TX_GPIO_Port GPIOA
#define TEMP_SO_Pin GPIO_PIN_11
#define TEMP_SO_GPIO_Port GPIOC
#define SW1_Pin GPIO_PIN_3
#define SW1_GPIO_Port GPIOB
#define SW2_Pin GPIO_PIN_4
#define SW2_GPIO_Port GPIOB
#define SW3_Pin GPIO_PIN_5
#define SW3_GPIO_Port GPIOB
#define LEDR_Pin GPIO_PIN_6
#define LEDR_GPIO_Port GPIOB
#define LEDB_Pin GPIO_PIN_7
#define LEDB_GPIO_Port GPIOB
#define SW4_Pin GPIO_PIN_8
#define SW4_GPIO_Port GPIOB
#define SW5_Pin GPIO_PIN_9
#define SW5_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
