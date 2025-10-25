/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   This file provides code for the configuration
  *          of all used GPIO pins.
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
#include "gpio.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Pinout Configuration
*/
void MX_GPIO_Init(void)
{

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();

  /* Configure PG13 and PG14 as Digital output */
  GPIOG->MODER &= ~(0x3 << (13 * 2)); // Clear mode bits for PG13
  GPIOG->MODER |= (0x1 << (13 * 2));  // Set mode to output (01)
  GPIOG->OTYPER &= ~(0x1 << 13);      // Set output type to push-pull (0)
  GPIOG->OSPEEDR |= (0x3 << (13 * 2)); // Set speed to high (11)
  GPIOG->PUPDR &= ~(0x3 << (13 * 2)); // No pull-up, pull-down (00)

  GPIOG->MODER &= ~(0x3 << (14 * 2)); // Clear mode bits for PG14
  GPIOG->MODER |= (0x1 << (14 * 2));  // Set mode to output (01)
  GPIOG->OTYPER &= ~(0x1 << 14);      // Set output type to push-pull (0)
  GPIOG->OSPEEDR |= (0x3 << (14 * 2)); // Set speed to high (11)
  GPIOG->PUPDR &= ~(0x3 << (14 * 2)); // No pull-up, pull-down (00)

}

/* USER CODE BEGIN 2 */

/* USER CODE END 2 */
