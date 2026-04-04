/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define PIN_LED2_Pin GPIO_PIN_0
#define PIN_LED2_GPIO_Port GPIOF
#define PIN_DIR_RIGHT_Pin GPIO_PIN_1
#define PIN_DIR_RIGHT_GPIO_Port GPIOF
#define PIN_ENC_LA_Pin GPIO_PIN_0
#define PIN_ENC_LA_GPIO_Port GPIOA
#define PIN_ENC_LB_Pin GPIO_PIN_1
#define PIN_ENC_LB_GPIO_Port GPIOA
#define PIN_PWM_RIGHT_Pin GPIO_PIN_2
#define PIN_PWM_RIGHT_GPIO_Port GPIOA
#define PIN_DIR_LEFT_Pin GPIO_PIN_3
#define PIN_DIR_LEFT_GPIO_Port GPIOA
#define PIN_ENC_RA_Pin GPIO_PIN_4
#define PIN_ENC_RA_GPIO_Port GPIOA
#define PIN_CHARGING_Pin GPIO_PIN_5
#define PIN_CHARGING_GPIO_Port GPIOA
#define PIN_ENC_RB_Pin GPIO_PIN_6
#define PIN_ENC_RB_GPIO_Port GPIOA
#define PIN_HC05_EN_Pin GPIO_PIN_7
#define PIN_HC05_EN_GPIO_Port GPIOA
#define PIN_DET_LEFT_Pin GPIO_PIN_0
#define PIN_DET_LEFT_GPIO_Port GPIOB
#define PIN_HC05_STATE_Pin GPIO_PIN_8
#define PIN_HC05_STATE_GPIO_Port GPIOA
#define PIN_HC05_RX_Pin GPIO_PIN_9
#define PIN_HC05_RX_GPIO_Port GPIOA
#define PIN_HC05_TX_Pin GPIO_PIN_10
#define PIN_HC05_TX_GPIO_Port GPIOA
#define PIN_USB_DM_Pin GPIO_PIN_11
#define PIN_USB_DM_GPIO_Port GPIOA
#define PIN_USB_DP_Pin GPIO_PIN_12
#define PIN_USB_DP_GPIO_Port GPIOA
#define PIN_SWDIO_Pin GPIO_PIN_13
#define PIN_SWDIO_GPIO_Port GPIOA
#define PIN_SWCLK_Pin GPIO_PIN_14
#define PIN_SWCLK_GPIO_Port GPIOA
#define PIN_I2C_SCL_Pin GPIO_PIN_15
#define PIN_I2C_SCL_GPIO_Port GPIOA
#define PIN_DIR_RIGHTB3_Pin GPIO_PIN_3
#define PIN_DIR_RIGHTB3_GPIO_Port GPIOB
#define PIN_LED0_Pin GPIO_PIN_4
#define PIN_LED0_GPIO_Port GPIOB
#define PIN_I2C_ALERT_Pin GPIO_PIN_5
#define PIN_I2C_ALERT_GPIO_Port GPIOB
#define PIN_LED1_Pin GPIO_PIN_6
#define PIN_LED1_GPIO_Port GPIOB
#define PIN_I2C_SDA_Pin GPIO_PIN_7
#define PIN_I2C_SDA_GPIO_Port GPIOB
#define PIN_PWM_RIGHTB8_Pin GPIO_PIN_8
#define PIN_PWM_RIGHTB8_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
