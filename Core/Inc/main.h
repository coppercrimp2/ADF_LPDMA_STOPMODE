/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32u5xx_hal.h"

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
#define OLED_13V_EN_Pin GPIO_PIN_2
#define OLED_13V_EN_GPIO_Port GPIOA
#define OLED_RST_Pin GPIO_PIN_12
#define OLED_RST_GPIO_Port GPIOB
#define OLED_CS_Pin GPIO_PIN_13
#define OLED_CS_GPIO_Port GPIOB
#define PWR_STATUS_0_Pin GPIO_PIN_6
#define PWR_STATUS_0_GPIO_Port GPIOC
#define PWR_STATUS_1_Pin GPIO_PIN_7
#define PWR_STATUS_1_GPIO_Port GPIOC
#define USB_VBUS_Pin GPIO_PIN_9
#define USB_VBUS_GPIO_Port GPIOA
#define USB_VBUS_EXTI_IRQn EXTI9_IRQn
#define PWR_3p3V_EN_Pin GPIO_PIN_10
#define PWR_3p3V_EN_GPIO_Port GPIOA
#define SDCard_En_Pin GPIO_PIN_7
#define SDCard_En_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
#define REC_BUFF_SIZE        (2048)//(16382)//(2048U)//(1024U)
#define REC_BUFF_SIZE_DIV2  (REC_BUFF_SIZE / 2)
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
