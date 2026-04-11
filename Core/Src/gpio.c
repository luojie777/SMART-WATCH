/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    gpio.c
 * @brief   This file provides code for the configuration
 *          of all used GPIO pins.
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
#include "gpio.h"

/* USER CODE BEGIN 0 */
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "tim.h"

// 按键状态变量
extern volatile Key_State_t g_left_key_state;
extern volatile Key_State_t g_right_key_state;

// 按键防抖相关变量
uint32_t left_button_press_time = 0;
uint32_t right_button_press_time = 0;
uint8_t left_button_state = 0;
uint8_t right_button_state = 0;

// 按键长按时间阈值（单位：ms）
#define LONG_PRESS_THRESHOLD 1000

/**
 * @brief  EXTI line detection callback.
 * @param  GPIO_Pin: Specifies the port pin connected to corresponding EXTI line.
 * @retval None
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	if (GPIO_Pin == left_button_Pin)
	{
		// 左按键中断处理 - 只在按键按下时触发
		if (HAL_GPIO_ReadPin(left_button_GPIO_Port, left_button_Pin) == GPIO_PIN_RESET)
		{
			g_left_key_state = KEY_SHORT_PRESS;
		}
		else
		{
			// 按键释放，停止PA8的PWM输出
			HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
		}

	}
	else if (GPIO_Pin == right_button_Pin)
	{
		// 右按键中断处理 - 只在按键按下时触发
		if (HAL_GPIO_ReadPin(right_button_GPIO_Port, right_button_Pin) == GPIO_PIN_RESET)
		{
			g_right_key_state = KEY_SHORT_PRESS;
		}
		else
		{
			// 按键释放，停止PA8的PWM输出
			HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
		}
	}
}
/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(DS18B20_GPIO_Port, DS18B20_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, chip_led_Pin|MPU6050_SCL_Pin|MPU6050_SDA_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : DS18B20_Pin */
  GPIO_InitStruct.Pin = DS18B20_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(DS18B20_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : left_button_Pin */
  GPIO_InitStruct.Pin = left_button_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(left_button_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : home_button_Pin */
  GPIO_InitStruct.Pin = home_button_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(home_button_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : BOOT1_Pin TF_flag_Pin */
  GPIO_InitStruct.Pin = BOOT1_Pin|TF_flag_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : chip_led_Pin */
  GPIO_InitStruct.Pin = chip_led_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(chip_led_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : MPU6050_SCL_Pin MPU6050_SDA_Pin */
  GPIO_InitStruct.Pin = MPU6050_SCL_Pin|MPU6050_SDA_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : right_button_Pin */
  GPIO_InitStruct.Pin = right_button_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(right_button_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI1_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

}

/* USER CODE BEGIN 2 */

/* USER CODE END 2 */
