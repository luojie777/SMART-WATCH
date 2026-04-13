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
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "cmsis_os.h"
#include <time.h>
#include "flash_storage.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
// 显示模式枚举
typedef enum {
	home_time,
	temperature,
	pressure,
	game,
	music
} Show_Picture_t;

// 工作状态枚举
typedef enum {
	NORMAL,
	MODIFY_TIME,
	MODIFY_TEMP,
	PLAY_GAME
} Work_State_t;

// 按键状态枚举
typedef enum {
	KEY_NONE,
	KEY_SHORT_PRESS,
	KEY_LONG_PRESS
} Key_State_t;

// 时间修改选项枚举
typedef enum {
	year,
	mon,
	day,
	hour,
	min,
	sec
} Modify_Time_choose_t;

// 温度修改选项枚举
typedef enum {
	mintemp,
	maxtemp
} Modify_Temp_choose_t;

// 串口命令枚举
typedef enum {
	CMD_NONE,
	CMD_SHOW_TIME,
	CMD_CALIBRATION_TIME,
	CMD_MODIFY_ENV,
	CMD_SHOW_TEMP,
	CMD_SHOW_PRESS,
	CMD_SHOW_GAME
} UART_Cmd_t;

// 定义V的结构体（记录每个V的状态）
typedef struct
{
	uint8_t x_pos;      // 当前X坐标
	uint8_t page;       // 当前显示页
	uint8_t is_valid;   // 是否有效（1=存在，0=消失/无效）
} V_Obj_t;
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
// 游戏相关宏定义
#define UART_QUEUE_LEN     10    // 串口队列长度
#define GAME_CHAR_A         "A"        // 玩家控制字符
#define GAME_CHAR_V         "V"        // 下落字符
#define GAME_DISPLAY_PAGE  7           // 固定显示页
#define ACC_X_MIN          -1.0f       // X轴加速度下限
#define ACC_X_MAX          1.0f        // X轴加速度上限
#define SCREEN_X_MIN       0           // 屏幕X坐标下限
#define SCREEN_X_MAX       122         // 屏幕X坐标上限
#define CHAR_WIDTH          6          // 6*8点阵字符宽度（用于碰撞判断）
#define GAME_DISPLAY_PAGE_A 7           // 玩家字符A的显示页（页1，行8~15）
#define GAME_FRAME_DELAY   30          // 合理帧率延时（30ms，约33次/秒，流畅不卡顿）
#define POS_CHANGE_THRESH  3           // OLED操作节流阈值（坐标变化超过1才更新）
#define FILTER_ALPHA       0.7f        // 滑动滤波系数（0~1，越接近1越平滑，响应越慢；推荐0.7）
#define MAX_V_COUNT         10         // V的最大数量
#define GAME_DISPLAY_PAGE_V_INIT 0      // V的初始显示页（页0，最上方，行0~7）

// ADC相关宏定义
#define R25_VALUE    1000.0f    // R25=1KΩ
#define R26_VALUE    3600.0f    // R26=3.6KΩ
#define ADC_RESOLUTION  4095.0f // 12位ADC分辨率（0~4095）
#define VREF_VOLTAGE   3.3f     // ADC参考电压（通常为3.3V）

// 串口接收缓冲区大小
#define USART2_RX_BUF_SIZE  80

// 加速度变化阈值
#define ACC_CHANGE_THRESHOLD 0.3f   // 加速度变化阈值（超过此值判定为有动作，需实际测试微调）

// 检查间隔
#define CHECK_INTERVAL_MS 5000   // 加速度检查周期（300ms，不要太短，避免占用过多CPU）
/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
// 全局变量声明
extern volatile Show_Picture_t Current_Show_Picture;
extern volatile Show_Picture_t Picture_before;
extern volatile Work_State_t g_work_state;
extern volatile Key_State_t g_home_key_state;
extern volatile Key_State_t g_left_key_state;
extern volatile Key_State_t g_right_key_state;
extern volatile Modify_Time_choose_t Modify_Time_choose;
extern volatile Modify_Temp_choose_t Modify_Temp_choose;
extern struct tm *time_temp;
extern uint8_t oled_screen_on;
extern Env_Thresh_Config_t g_global_env_config;
extern QueueHandle_t xUartQueue;
extern V_Obj_t V_Array[MAX_V_COUNT];
extern int V_MOVE_INTERVAL;
extern int V_SPAWN_INTERVAL;
extern xTaskHandle xTask2_Handle;
extern float AHT20_Temperature;
extern float AHT20_Humidity;
extern float BMP280_P0;
extern float BMP280_Temperature;
extern float BMP280_Pressure;
extern uint8_t temp_flag;
extern uint8_t pre_flag;
extern float altitude;
extern float ds18b20_Temperature;
extern float H_temp;
extern float L_temp;
extern float L_preesure;
extern uint32_t Screen_off_time;
extern float AccX;
extern float AccY;
extern float AccZ;
extern float GyroX;
extern float GyroY;
extern float GyroZ;
extern float percentage;
extern uint32_t adc_raw;
extern struct tm *now;
extern uint8_t usart2_rx_single_buf[1];
extern uint8_t usart2_rx_str_buf[USART2_RX_BUF_SIZE];
extern uint8_t usart2_rx_str_len;

// 函数原型
extern int Parse_Env_KeyValue(char *key, char *value);
extern UART_Cmd_t Parse_UART_Cmd(uint8_t *buf);
extern void updatea_env_value();
extern void Refresh_OLED_Time(struct tm *time_temp, char *row1, char *row2);
extern void Refresh_OLED_Temp(char *row1, char *row2);
extern int Get_Max_Day(int year_offset, int month);
extern void ADC_Read_Raw(void);
extern float ADC_Calculate_BatteryVoltage(void);
extern void gameTask(void * pvParameters);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define DS18B20_Pin GPIO_PIN_0
#define DS18B20_GPIO_Port GPIOA
#define left_button_Pin GPIO_PIN_1
#define left_button_GPIO_Port GPIOA
#define left_button_EXTI_IRQn EXTI1_IRQn
#define home_button_Pin GPIO_PIN_1
#define home_button_GPIO_Port GPIOB
#define BOOT1_Pin GPIO_PIN_2
#define BOOT1_GPIO_Port GPIOB
#define chip_led_Pin GPIO_PIN_3
#define chip_led_GPIO_Port GPIOB
#define MPU6050_SCL_Pin GPIO_PIN_4
#define MPU6050_SCL_GPIO_Port GPIOB
#define MPU6050_SDA_Pin GPIO_PIN_5
#define MPU6050_SDA_GPIO_Port GPIOB
#define right_button_Pin GPIO_PIN_8
#define right_button_GPIO_Port GPIOB
#define right_button_EXTI_IRQn EXTI9_5_IRQn
#define TF_flag_Pin GPIO_PIN_9
#define TF_flag_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
