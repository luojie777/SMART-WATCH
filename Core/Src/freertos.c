/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "clock.h"
#include <stdio.h>
#include <string.h>
#include "usart.h"
#include "tim.h"
#include "time.h"
#include "oled.h"
#include "DS18B20.h"
#include "game.h"
#include "BMP280.h"
#include "AHT20.h"
#include "queue.h"
#include "MPU6050.h"
#include "adc.h"
#include "flash_storage.h"
#include <math.h>
#include <stdlib.h>
#include "notes.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define UART_QUEUE_LEN     10    // 串口队列长度

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
// 游戏相关宏定义（优化补充：帧率、滤波、节流阈值）
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

#define R25_VALUE    1000.0f    // R25=1KΩ
#define R26_VALUE    3600.0f    // R26=3.6KΩ
#define ADC_RESOLUTION  4095.0f // 12位ADC分辨率（0~4095）
#define VREF_VOLTAGE   3.3f     // ADC参考电压（通常为3.3V）

#define USART2_RX_BUF_SIZE  80
uint8_t usart2_rx_single_buf[1];  // 单字节接收缓冲区（中断用）
uint8_t usart2_rx_str_buf[USART2_RX_BUF_SIZE] = {0};  // 字符串累积缓冲区
uint8_t usart2_rx_str_len = 0;    // 缓冲区当前长度
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
// 全局变量
volatile Show_Picture_t Current_Show_Picture = home_time;
volatile Show_Picture_t Picture_before = home_time;
volatile Work_State_t g_work_state = NORMAL;
volatile Key_State_t g_home_key_state = KEY_NONE;
volatile Key_State_t g_left_key_state = KEY_NONE;
volatile Key_State_t g_right_key_state = KEY_NONE;
volatile Modify_Time_choose_t Modify_Time_choose = year;
volatile Modify_Temp_choose_t Modify_Temp_choose = mintemp;
struct tm *time_temp = NULL;
uint8_t oled_screen_on = 1;



uint32_t adc_raw;
struct tm *now;

V_Obj_t V_Array[MAX_V_COUNT] = {0};    // V的结构体数组（最多10个）

Env_Thresh_Config_t g_global_env_config;
QueueHandle_t xUartQueue = NULL;   // 串口指令队列


int V_MOVE_INTERVAL = 500;     // V的下移间隔（1秒，FreeRTOS ticks）
int V_SPAWN_INTERVAL = 500;     // V的生成间隔（1秒，FreeRTOS ticks）


xTaskHandle xTask3_Handle = NULL;
xTaskHandle xTask2_Handle = NULL;
float AHT20_Temperature = 0;
float AHT20_Humidity = 0;


float BMP280_P0 = 101325.0;
float BMP280_Temperature = 0;
float BMP280_Pressure = 0;
uint8_t temp_flag = 0;
uint8_t pre_flag = 0;
float altitude = 0;
float ds18b20_Temperature = 0;

//掉电不丢失数据
float  H_temp;
float  L_temp;
float  L_preesure;
uint32_t  Screen_off_time = 30000;


float AccX = 0;
float AccY = 0;
float AccZ = 0;
float GyroX = 0;
float GyroY = 0;
float GyroZ = 0;


float percentage=0;

volatile uint8_t music_flag = 0;

#define ACC_CHANGE_THRESHOLD 0.2f

static float    last_AccX = 0.0f, last_AccY = 0.0f, last_AccZ = 0.0f; // 上一次加速度数据
static uint32_t axy_check_counter = 1;

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
		.name = "defaultTask",
		.stack_size = 256 * 4,
		.priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for HomeButtonListe */
osThreadId_t HomeButtonListeHandle;
const osThreadAttr_t HomeButtonListe_attributes = {
		.name = "HomeButtonListe",
		.stack_size = 16 * 4,
		.priority = (osPriority_t) osPriorityNormal3,
};
/* Definitions for HomeButtonHandl */
osThreadId_t HomeButtonHandlHandle;
const osThreadAttr_t HomeButtonHandl_attributes = {
		.name = "HomeButtonHandl",
		.stack_size = 256 * 4,
		.priority = (osPriority_t) osPriorityNormal2,
};
/* Definitions for lrbuttonListen */
osThreadId_t lrbuttonListenHandle;
const osThreadAttr_t lrbuttonListen_attributes = {
		.name = "lrbuttonListen",
		.stack_size = 256 * 4,
		.priority = (osPriority_t) osPriorityNormal1,
};
/* Definitions for UartTestTask */
osThreadId_t UartTestTaskHandle;
const osThreadAttr_t UartTestTask_attributes = {
		.name = "UartTestTask",
		.stack_size = 256 * 4,
		.priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for OledshowTask */
osThreadId_t OledshowTaskHandle;
const osThreadAttr_t OledshowTask_attributes = {
		.name = "OledshowTask",
		.stack_size = 256 * 4,
		.priority = (osPriority_t) osPriorityNormal3,
};
/* Definitions for MPU6050Task */
osThreadId_t MPU6050TaskHandle;
const osThreadAttr_t MPU6050Task_attributes = {
		.name = "MPU6050Task",
		.stack_size = 128 * 4,
		.priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for UARTQueue */
osMessageQueueId_t UARTQueueHandle;
const osMessageQueueAttr_t UARTQueue_attributes = {
		.name = "UARTQueue"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */



/**
 * @brief  解析环境参数键值对，更新全局配置
 * @param  key: 键名（M_Htemp/M_Ltemp/M_Lpre/M_OffTime）
 * @param  value: 数值字符串
 * @retval 0: 成功，1: 键名未知，2: 数值非法
 */
int Parse_Env_KeyValue(char *key, char *value)
{
	//	char dbg_msg[20];
	//	sprintf(dbg_msg, "AAA[%s], BBB:%s\n", key, value);
	//	HAL_UART_Transmit(&huart2, (uint8_t*)dbg_msg, strlen(dbg_msg), 100);
	float f_value;
	uint32_t u32_value;
	// 1. 最高温 M_Htemp
	if(strcmp(key, "M_Htemp") == 0)
	{
		//		sprintf(dbg_msg, "ENV_DBG: value=[%s], len=%d\r\n", value, strlen(value));
		//		HAL_UART_Transmit(&huart2, (uint8_t*)dbg_msg, strlen(dbg_msg), 100);
		//if(sscanf(value, "%f", &f_value) != 1) {return 2;}
		sscanf(value, "%f", &f_value);
		g_global_env_config.temp_high_thresh = f_value;
		//		sprintf(dbg_msg, "f_value:%.2f ENV_DBG: %.2f AAA", f_value,g_global_env_config.temp_high_thresh);
		//		HAL_UART_Transmit(&huart2, (uint8_t*)dbg_msg, strlen(dbg_msg), 100);
		return 0;
	}
	// 2. 最低温 M_Ltemp
	else if(strcmp(key, "M_Ltemp") == 0)
	{
		//HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
		sscanf(value, "%f", &f_value);
		g_global_env_config.temp_low_thresh = f_value;
		return 0;
	}
	// 3. 最低气压 M_Lpre
	else if(strcmp(key, "M_Lpre") == 0)
	{
		sscanf(value, "%f", &f_value);
		g_global_env_config.press_low_thresh = f_value;
		return 0;
	}
	// 4. 熄屏时间 M_OffTime
	else if(strcmp(key, "M_OffTime") == 0)
	{
		sscanf(value, "%ld", &u32_value);
		g_global_env_config.Screen_off_time = u32_value;
		return 0;
	}
	// 未知键名
	else
	{
		HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
		return 1;
	}
}

UART_Cmd_t Parse_UART_Cmd(uint8_t *buf)
{
	if(buf == NULL || buf[0] == '\0')
	{
		return CMD_NONE;
	}

	// 转换为C字符串方便处理
	char *cmd_str = (char *)buf;

	// 1. 时间校准命令：check_time+YYYY-MM-DD HH:MM:SS
	if(strstr(cmd_str, "check_time+") != NULL)
	{
		//HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
		return CMD_CALIBRATION_TIME;
	}
	// 新增：环境参数修改命令：modify_env+$M_Htemp:xx$...
	if(strstr(cmd_str, "modify_env+") != NULL)
	{
		return CMD_MODIFY_ENV;
	}
	// 2. 原有显示命令（改成字符串判断，兼容旧逻辑）
	else if(strcmp(cmd_str, "show_time\r\n") == 0)
	{
		return CMD_SHOW_TIME;
	}
	else if(strcmp(cmd_str, "show_temp\r\n") == 0)
	{
		return CMD_SHOW_TEMP;
	}
	else if(strcmp(cmd_str, "show_press\r\n") == 0)
	{
		return CMD_SHOW_PRESS;
	}
	else if(strcmp(cmd_str, "show_game\r\n") == 0)
	{
		return CMD_SHOW_GAME;
	}
	// 3. 未知命令
	else
	{
		return CMD_NONE;
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART2) // 确保是USART2的中断
	{
		BaseType_t xHigherPriorityTaskWoken = pdFALSE;

		// 1. 将收到的单字节添加到字符串缓冲区
		if(usart2_rx_str_len < USART2_RX_BUF_SIZE - 1) // 预留最后1字节存结束符
		{
			usart2_rx_str_buf[usart2_rx_str_len++] = usart2_rx_single_buf[0];

			// 2. 判断是否收到结束符（\n），表示命令字符串接收完成
			if(usart2_rx_single_buf[0] == '\n')
			{
				// 添加字符串结束符，方便C语言字符串处理
				usart2_rx_str_buf[usart2_rx_str_len] = '\0';

				// 3. 将完整命令字符串发送到FreeRTOS队列
				if(xUartQueue != NULL)
				{
					xQueueSendFromISR(xUartQueue, usart2_rx_str_buf, &xHigherPriorityTaskWoken);
				}

				// 4. 清空缓冲区，准备下一次接收
				usart2_rx_str_len = 0;
				memset(usart2_rx_str_buf, 0, USART2_RX_BUF_SIZE);
			}
		}
		else
		{
			// 缓冲区满，强制清空
			usart2_rx_str_len = 0;
			memset(usart2_rx_str_buf, 0, USART2_RX_BUF_SIZE);
		}

		// 5. 重新开启串口中断（必须手动重启单次中断）
		HAL_UART_Receive_IT(&huart2, usart2_rx_single_buf, 1);
	}
}

void updatea_env_value()
{
	H_temp = g_global_env_config.temp_high_thresh;
	L_temp = g_global_env_config.temp_low_thresh;
	L_preesure = g_global_env_config.press_low_thresh;
	Screen_off_time = g_global_env_config.Screen_off_time;
}

// 提取OLED刷新公共函数，减少代码冗余
void Refresh_OLED_Time(struct tm *time_temp, char *row1, char *row2) {
	//OLED_CLS();
	sprintf(row1,"%02d-%02d-%02d",time_temp->tm_year + 1900,time_temp->tm_mon + 1,time_temp->tm_mday);
	sprintf(row2,"%02d:%02d:%02d",time_temp->tm_hour,time_temp->tm_min,time_temp->tm_sec);
	OLED_ShowStr(24, 1, (unsigned char*)row1, 2);
	OLED_ShowStr(32, 5, (unsigned char*)row2, 2);
}

void Refresh_OLED_Temp(char *row1, char *row2) {
	//OLED_CLS();
	sprintf(row1,"MIN_TEMP:%.2f",L_temp);
	sprintf(row2,"MAX_TEMP:%.2f",H_temp);
	OLED_ShowStr(10, 1, (unsigned char*)row1, 2);
	OLED_ShowStr(10, 5, (unsigned char*)row2, 2);
}


// 先声明一个辅助函数：获取指定月份的最大天数（基础版，适配平年，进阶版可增加闰年判断）
int Get_Max_Day(int year_offset, int month) {
	// month是tm_mon（0~11），对应1~12月
	switch(month) {
	case 1: // 2月（平年28天，闰年29天，此处先做平年处理，后续可扩展闰年判断）
		return 28;
	case 3: case 5: case 8: case 10: // 4、6、9、11月（30天）
		return 30;
	default: // 1、3、5、7、8、10、12月（31天）
		return 31;
	}
}


/**
 * @brief  读取ADC2_IN8的原始采样值
 * @retval ADC原始值（0~4095）
 */
void ADC_Read_Raw(void)
{
	if (HAL_ADC_Start(&hadc2) != HAL_OK)
	{
		Error_Handler();
	}

	// 等待转换完成
	if (HAL_ADC_PollForConversion(&hadc2, 10) == HAL_OK)
	{
		// 读取ADC原始值
		adc_raw = HAL_ADC_GetValue(&hadc2);
	}

	HAL_ADC_Stop(&hadc2);
}

/**
 * @brief  计算电池电压（基于分压电路反推）
 * @retval 电池电压值（单位：V，如锂电池满电约4.2V）
 */
float ADC_Calculate_BatteryVoltage(void)
{
	ADC_Read_Raw();
	// 1. 计算ADC采样到的分压后电压
	float sampled_voltage = (adc_raw * VREF_VOLTAGE) / ADC_RESOLUTION;
	// 2. 根据分压比反推电池电压：Vbat = Vsampled * (R25+R26)/R26
	float battery_voltage = sampled_voltage * (R25_VALUE + R26_VALUE) / R26_VALUE;

	return battery_voltage;
}

void gameTask(void * pvParameters);
void musicTask(void * pvParameters);
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void StartHomeButtonListen(void *argument);
void StartHomeButtonHandler(void *argument);
void StartlrbuttonListen(void *argument);
void StartUartTestTask(void *argument);
void StartOledshowTask(void *argument);
void StartMPU6050Task(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
 * @brief  FreeRTOS initialization
 * @param  None
 * @retval None
 */
void MX_FREERTOS_Init(void) {
	/* USER CODE BEGIN Init */

	xUartQueue = xQueueCreate(UART_QUEUE_LEN, USART2_RX_BUF_SIZE);

	HAL_UART_Receive_IT(&huart2, usart2_rx_single_buf, 1);
	/* USER CODE END Init */

	/* USER CODE BEGIN RTOS_MUTEX */
	/* add mutexes, ... */
	/* USER CODE END RTOS_MUTEX */

	/* USER CODE BEGIN RTOS_SEMAPHORES */
	/* add semaphores, ... */
	/* USER CODE END RTOS_SEMAPHORES */

	/* USER CODE BEGIN RTOS_TIMERS */
	/* start timers, add new ones, ... */
	/* USER CODE END RTOS_TIMERS */

	/* Create the queue(s) */
	/* creation of UARTQueue */
	UARTQueueHandle = osMessageQueueNew (16, sizeof(uint16_t), &UARTQueue_attributes);

	/* USER CODE BEGIN RTOS_QUEUES */
	/* add queues, ... */
	/* USER CODE END RTOS_QUEUES */

	/* Create the thread(s) */
	/* creation of defaultTask */
	defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

	/* creation of HomeButtonListe */
	HomeButtonListeHandle = osThreadNew(StartHomeButtonListen, NULL, &HomeButtonListe_attributes);

	/* creation of HomeButtonHandl */
	HomeButtonHandlHandle = osThreadNew(StartHomeButtonHandler, NULL, &HomeButtonHandl_attributes);

	/* creation of lrbuttonListen */
	lrbuttonListenHandle = osThreadNew(StartlrbuttonListen, NULL, &lrbuttonListen_attributes);

	/* creation of UartTestTask */
	UartTestTaskHandle = osThreadNew(StartUartTestTask, NULL, &UartTestTask_attributes);

	/* creation of OledshowTask */
	OledshowTaskHandle = osThreadNew(StartOledshowTask, NULL, &OledshowTask_attributes);

	/* creation of MPU6050Task */
	MPU6050TaskHandle = osThreadNew(StartMPU6050Task, NULL, &MPU6050Task_attributes);

	/* USER CODE BEGIN RTOS_THREADS */
	/* add threads, ... */
	/* USER CODE END RTOS_THREADS */

	/* USER CODE BEGIN RTOS_EVENTS */
	/* add events, ... */
	/* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
 * @brief  Function implementing the defaultTask thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
	/* USER CODE BEGIN StartDefaultTask */
	/* Infinite loop */
	//HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	char message[50] = "";
	if(Flash_Storage_Read_Latest_Thresh(&g_global_env_config)==HAL_OK)
	{
		updatea_env_value();
	}
	//	g_global_env_config.temp_high_thresh = 32.0f;
	//	g_global_env_config.temp_low_thresh = 6.0f;
	//	g_global_env_config.press_low_thresh = 90000.0f;
	//	g_global_env_config.Screen_off_time = 30000;
	//	Flash_Storage_Write_Thresh(&g_global_env_config);
	UART_Cmd_t uart_cmd = CMD_NONE;
	BaseType_t xStatus;
	char *env_param, *token, *key, *value;
	char env_buf[USART2_RX_BUF_SIZE] = {0}; // 环境参数缓冲区
	int parse_result;
	for(;;)
	{
		xStatus = xQueueReceive(xUartQueue, usart2_rx_str_buf, 0);
		if(xStatus == pdPASS)
		{
			uart_cmd = Parse_UART_Cmd(usart2_rx_str_buf);

			// 处理串口指令
			switch(uart_cmd)
			{
			case CMD_SHOW_TIME:
				Current_Show_Picture = home_time;
				break;

				// ========== 新增：时间校准命令处理 ==========
			case CMD_CALIBRATION_TIME:
			{
				// 提取时间参数：从"check_time+"后面截取时间字符串
				char *time_param = strstr((char *)usart2_rx_str_buf, "check_time+") + strlen("check_time+");
				if(time_param != NULL)
				{
					// 去除末尾的换行符，避免解析错误
					time_param[strcspn(time_param, "\r\n")] = '\0';

					// 解析时间字符串为struct tm格式
					struct tm calib_time = {0};
					sscanf(time_param, "%d-%d-%d %d:%d:%d",
							&calib_time.tm_year, &calib_time.tm_mon, &calib_time.tm_mday,
							&calib_time.tm_hour, &calib_time.tm_min, &calib_time.tm_sec);

					// 转换为RTC需要的格式：tm_year是从1900年开始的偏移，tm_mon是0-11
					calib_time.tm_year -= 1900;
					calib_time.tm_mon -= 1;
					calib_time.tm_sec += 1;

					// 设置RTC时间
					Clock_RTC_SetTime(&calib_time);

					// 发送校准成功响应
					sprintf(message, "CALIB_OK: %s\r\n", time_param);
					HAL_UART_Transmit(&huart2, (uint8_t*)message, strlen(message), 100);
				}
			}
			break;
			// ========== 新增：环境参数修改命令处理 ==========
			case CMD_MODIFY_ENV:
			{
				// 1. 提取环境参数（跳过modify_env+前缀）
				env_param = strstr((char *)usart2_rx_str_buf, "modify_env+") + strlen("modify_env+");
				if(env_param == NULL)
				{
					sprintf(message, "ENV_ERR: Param NULL\r\n");
					HAL_UART_Transmit(&huart2, (uint8_t*)message, strlen(message), 100);
					break;
				}

				// 2. 清洗参数：去除\r\n，复制到临时缓冲区
				env_param[strcspn(env_param, "\r\n")] = '\0';
				strncpy(env_buf, env_param, sizeof(env_buf)-1);
				env_buf[sizeof(env_buf)-1] = '\0';
				HAL_UART_Transmit(&huart2, (uint8_t*)"SDSD  ", strlen("SDSD  "), 100);
				HAL_UART_Transmit(&huart2, (uint8_t*)env_buf, strlen(env_buf), 100);
				// 3. 解析键值对（以$分隔，格式：$key:value$key:value$）
				token = strtok(env_buf, "$");
				int all_ok = 1; // 所有参数是否解析成功
				taskENTER_CRITICAL();
				while(token != NULL && *token != '\0')
				{
					// ========== 关键修改：用strchr手动分割:，替代嵌套strtok ==========
					char *colon_pos = strchr(token, ':');
					if(colon_pos == NULL)
					{
						all_ok = 0;
						break;
					}

					// 分割key
					char key_buf[10] = {0};
					int key_len = colon_pos - token;
					strncpy(key_buf, token, key_len);
					key_buf[key_len] = '\0';

					// 分割value
					char value_buf[10] = {0};
					strncpy(value_buf, colon_pos + 1, strlen(colon_pos + 1));
					value_buf[strlen(colon_pos + 1)] = '\0';

					// 调试打印（可选）
					//					char dbg_msg[64] = {0};
					//					sprintf(dbg_msg, "ENV_DBG: key=%s, value=%s\r\n", key_buf, value_buf);
					//					HAL_UART_Transmit(&huart2, (uint8_t*)dbg_msg, strlen(dbg_msg), 100);

					// 校验并解析
					if(strlen(key_buf) > 0 && strlen(value_buf) > 0)
					{
						parse_result = Parse_Env_KeyValue(key_buf, value_buf);
						if(parse_result != 0)
						{
							all_ok = 0;
							break;
						}
					}
					else
					{
						all_ok = 0;
						break;
					}

					// 继续分割下一个token（上下文未被破坏）
					token = strtok(NULL, "$");
				}
				taskEXIT_CRITICAL();

				// 4. 所有参数解析成功，写入Flash并返回成功响应
				if(all_ok)
				{
					if(Flash_Storage_Write_Thresh(&g_global_env_config) == HAL_OK)
					{
						// 更新全局变量（和你的updatea_env_value逻辑对齐）
						updatea_env_value();
						sprintf(message, "ENV_OK: Save Success\r\n");
					}
					else
					{
						sprintf(message, "ENV_ERR: Flash Write Fail\r\n");
					}
				}
				else
				{
					sprintf(message, "ENV_ERR: Invalid Param\r\n");
				}

				// 5. 发送响应
				HAL_UART_Transmit(&huart2, (uint8_t*)message, strlen(message), 100);
			}
			break;
			case CMD_SHOW_TEMP:
				Current_Show_Picture = temperature;
				break;
			case CMD_SHOW_PRESS:
				Current_Show_Picture = pressure;
				break;
			case CMD_SHOW_GAME:
				Current_Show_Picture = game;
				break;
			default:
				break;
			}
		}

		osDelay(100);  // 降低任务CPU占用率
	}
	/* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartHomeButtonListen */
/**
 * @brief Function implementing the homeButtonListe thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartHomeButtonListen */
void StartHomeButtonListen(void *argument)
{
	/* USER CODE BEGIN StartHomeButtonListen */
	/* Infinite loop */
	while(1)
	{
		//		if(oled_screen_on == 0)
		//		{
		//			OLED_ON();
		//			oled_screen_on = 1;
		//		}
		if (HAL_GPIO_ReadPin(home_button_GPIO_Port, home_button_Pin) == GPIO_PIN_RESET)
		{
			// 按键按下：清零累计时长，启动定时器10ms中断
			//			g_home_key_press_ms = 0;
			//			HAL_TIM_Base_Start_IT(&htim3);
			osDelay(600);
			if (HAL_GPIO_ReadPin(home_button_GPIO_Port, home_button_Pin) == GPIO_PIN_RESET)
			{
				//HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);
				g_home_key_state = KEY_LONG_PRESS;
			}
			else if (HAL_GPIO_ReadPin(home_button_GPIO_Port, home_button_Pin) == GPIO_PIN_SET)
			{
				//HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);
				g_home_key_state = KEY_SHORT_PRESS;
			}
			//HAL_TIM_Base_Start_IT(&htim3);
			//g_home_key_press_ms = 0;
		}
		else
		{
			// 按键松开：重置按键状态
			g_home_key_state = KEY_NONE;
		}
		osDelay(30);
	}
	/* USER CODE END StartHomeButtonListen */
}

/* USER CODE BEGIN Header_StartHomeButtonHandler */
/**
 * @brief Function implementing the homeButtonHandl thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartHomeButtonHandler */
void StartHomeButtonHandler(void *argument)
{
	/* USER CODE BEGIN StartHomeButtonHandler */
	/* Infinite loop */
	struct tm *now;
	char row1[30] = "";
	char row2[30] = "";
	for(;;)
	{

		if(g_home_key_state == KEY_NONE && g_work_state == NORMAL)
		{

		}
		else if(g_home_key_state == KEY_SHORT_PRESS && g_work_state == NORMAL && Current_Show_Picture != home_time)
		{
			//返回主界面
			Current_Show_Picture = home_time;
			g_home_key_state = KEY_NONE;
		}
		else if(g_home_key_state == KEY_LONG_PRESS && g_work_state == NORMAL&&Current_Show_Picture == game)
		{

			if (xTask2_Handle == NULL)
			{
				g_work_state = PLAY_GAME;
				g_home_key_state = KEY_NONE;
				vTaskSuspend(OledshowTaskHandle);
				//				vTaskSuspend(UartTestTaskHandle);
				//				vTaskSuspend(lrbuttonListenHandle);
				vTaskSuspend(MPU6050TaskHandle);
				OLED_CLS();
				xTaskCreate(gameTask,"gameTask",32,NULL,(osPriority_t) osPriorityNormal3,&xTask2_Handle );
			}
		}
		else if(g_home_key_state == KEY_LONG_PRESS && g_work_state == NORMAL&&Current_Show_Picture == music)
		{

			if (xTask3_Handle == NULL)
			{
				xTaskCreate(musicTask,"musicTask",256,NULL,(osPriority_t) osPriorityNormal3,&xTask3_Handle );
				music_flag = 1;
			}
			if(music_flag == 0)
			{
				vTaskResume(xTask3_Handle);
				music_flag = 1;
			}
			if(music_flag == 1)
			{
				vTaskSuspend(xTask3_Handle);
				music_flag = 0;
			}
		}
		else if(g_home_key_state == KEY_LONG_PRESS && g_work_state == PLAY_GAME)
		{
			g_work_state = NORMAL;
			g_home_key_state = KEY_NONE; // 重置按键状态，避免重复触发
			OLED_CLS();

			// 恢复所有挂起的任务（包括homeButtonListeHandle）
			vTaskResume(OledshowTaskHandle);
			vTaskResume(UartTestTaskHandle);//			    //vTaskResume(homeButtonListeHandle); // 恢复之前注释的按键监听任务
			//			    vTaskResume(lrbuttonListenHandle);
			vTaskResume(MPU6050TaskHandle);

		}
		else if(g_home_key_state == KEY_LONG_PRESS && g_work_state == NORMAL&&Current_Show_Picture == home_time)
		{
			//修改时间
			g_work_state = MODIFY_TIME;
			Modify_Time_choose = year;
			now = Clock_RTC_GetTime();
			time_temp = now;
			vTaskSuspend(OledshowTaskHandle);
			vTaskSuspend(UartTestTaskHandle);
			sprintf(row1,"%02d-%02d-%02d",now->tm_year + 1900,now->tm_mon + 1,now->tm_mday);
			sprintf(row2,"%02d:%02d:%02d",now->tm_hour,now->tm_min,now->tm_sec);
			OLED_CLS();
			OLED_ShowStr(24, 1, (unsigned char*)row1, 2);
			OLED_ShowStr(32, 5, (unsigned char*)row2, 2);
			g_home_key_state = KEY_NONE;
		}
		else if(g_home_key_state == KEY_LONG_PRESS && g_work_state == MODIFY_TIME)
		{
			//退出修改时间
			g_work_state = NORMAL;
			g_home_key_state = KEY_NONE;
			now = time_temp;
			Clock_RTC_SetTime(now);
			OLED_CLS();
			vTaskResume(OledshowTaskHandle);
			vTaskResume(UartTestTaskHandle);
		}
		else if(g_home_key_state == KEY_LONG_PRESS && g_work_state == NORMAL&&Current_Show_Picture == temperature)
		{
			//修改温度
			g_work_state = MODIFY_TEMP;
			Modify_Temp_choose = mintemp;
			OLED_CLS();
			vTaskSuspend(OledshowTaskHandle);
			vTaskSuspend(UartTestTaskHandle);
			sprintf(row1,"MIN_TEMP:%.2f",L_temp);
			sprintf(row2,"MAX_TEMP:%.2f",H_temp);
			OLED_ShowStr(10, 1, (unsigned char*)row1, 2);
			OLED_ShowStr(10, 5, (unsigned char*)row2, 2);
			g_home_key_state = KEY_NONE;
			//HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
		}
		else if(g_home_key_state == KEY_LONG_PRESS && g_work_state == MODIFY_TEMP)
		{
			//退出修改温度
			g_work_state = NORMAL;
			g_home_key_state = KEY_NONE;
			g_global_env_config.temp_high_thresh = H_temp;
			g_global_env_config.temp_low_thresh = L_temp;
			Flash_Storage_Write_Thresh(&g_global_env_config);
			OLED_CLS();
			vTaskResume(OledshowTaskHandle);
			vTaskResume(UartTestTaskHandle);
		}
		else if(g_home_key_state == KEY_SHORT_PRESS && g_work_state == MODIFY_TIME)
		{
			//选择要修改的时间
			//taskENTER_CRITICAL();
			Modify_Time_choose = (Modify_Time_choose + 1) % 6;
			//taskEXIT_CRITICAL();
			g_home_key_state = KEY_NONE;
			//其他两个按键监听g_work_state修改
		}
		else if(g_home_key_state == KEY_SHORT_PRESS && g_work_state == MODIFY_TEMP)
		{
			//选择要修改的时间
			//taskENTER_CRITICAL();
			Modify_Temp_choose = (Modify_Temp_choose + 1) % 2;
			//taskEXIT_CRITICAL();
			g_home_key_state = KEY_NONE;
			//其他两个按键监听g_work_state修改
		}
		else if(g_home_key_state == KEY_NONE && g_work_state == MODIFY_TIME)
		{
			//无操作
		}

		osDelay(200);
		//延时不能太短，不然会判断两次长按
	}
	/* USER CODE END StartHomeButtonHandler */
}

/* USER CODE BEGIN Header_StartlrbuttonListen */
/**
 * @brief Function implementing the lrbuttonListen thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartlrbuttonListen */
void StartlrbuttonListen(void *argument)
{
	/* USER CODE BEGIN StartlrbuttonListen */
	/* Infinite loop */
	char row1[30] = "";
	char row2[30] = "";
	while(1)
	{
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_3);
		if(g_work_state == NORMAL)
		{
			if(g_right_key_state == KEY_SHORT_PRESS)
			{
				Current_Show_Picture = (Current_Show_Picture + 1) % 5;
				g_right_key_state = KEY_NONE;
			}
			else if(g_left_key_state == KEY_SHORT_PRESS)
			{
				if(Current_Show_Picture > 0)
					Current_Show_Picture = (Current_Show_Picture - 1) % 5;
				g_left_key_state = KEY_NONE;
			}
		}
		else if(g_work_state == MODIFY_TIME)
		{
			if(g_left_key_state == KEY_SHORT_PRESS)
			{
				switch(Modify_Time_choose)
				{
				case year:
					// 年份下限：建议不低于0（对应1900年，可自定义调整）
					if(time_temp->tm_year > 0) {
						time_temp->tm_year = time_temp->tm_year - 1;
					}
					break;
				case mon:
					// 月份下限：0（对应1月）
					if(time_temp->tm_mon > 0) {
						time_temp->tm_mon = time_temp->tm_mon - 1;
						// 月份递减后，需同步修正日期（避免原日期超过新月份的最大天数，比如31日减到4月（30天））
						int max_day = Get_Max_Day(time_temp->tm_year, time_temp->tm_mon);
						if(time_temp->tm_mday > max_day) {
							time_temp->tm_mday = max_day;
						}
					}
					break;
				case day:
					// 日期下限：1，上限：当前月份的最大天数
					if(time_temp->tm_mday > 1) {
						time_temp->tm_mday = time_temp->tm_mday - 1;
					}
					break;
				case hour:
					// 小时下限：0
					if(time_temp->tm_hour > 0) {
						time_temp->tm_hour = time_temp->tm_hour - 1;
					}
					break;
				case min:
					// 分钟下限：0
					if(time_temp->tm_min > 0) {
						time_temp->tm_min = time_temp->tm_min - 1;
					}
					break;
				case sec:
					// 秒数下限：0
					if(time_temp->tm_sec > 0) {
						time_temp->tm_sec = time_temp->tm_sec - 1;
					}
					break;
				}
				// 调用公共刷新函数
				Refresh_OLED_Time(time_temp, row1, row2);
				g_left_key_state = KEY_NONE;
			}
			else if(g_right_key_state == KEY_SHORT_PRESS)
			{
				switch(Modify_Time_choose)
				{
				case year:
					// 年份上限：可自定义（比如2100年，对应tm_year=200）
					if(time_temp->tm_year < 200) {
						time_temp->tm_year = time_temp->tm_year + 1;
					}
					break;
				case mon:
					// 月份上限：11（对应12月）
					if(time_temp->tm_mon < 11) {
						time_temp->tm_mon = time_temp->tm_mon + 1;
						// 月份递增后，同步修正日期（避免原日期超过新月份的最大天数）
						int max_day = Get_Max_Day(time_temp->tm_year, time_temp->tm_mon);
						if(time_temp->tm_mday > max_day) {
							time_temp->tm_mday = max_day;
						}
					}
					break;
				case day:
					// 日期上限：当前月份的最大天数
					int max_day = Get_Max_Day(time_temp->tm_year, time_temp->tm_mon);
					if(time_temp->tm_mday < max_day) {
						time_temp->tm_mday = time_temp->tm_mday + 1;
					}
					break;
				case hour:
					// 小时上限：23
					if(time_temp->tm_hour < 23) {
						time_temp->tm_hour = time_temp->tm_hour + 1;
					}
					break;
				case min:
					// 分钟上限：59
					if(time_temp->tm_min < 59) {
						time_temp->tm_min = time_temp->tm_min + 1;
					}
					break;
				case sec:
					// 秒数上限：59
					if(time_temp->tm_sec < 59) {
						time_temp->tm_sec = time_temp->tm_sec + 1;
					}
					break;
				}
				// 调用公共刷新函数
				Refresh_OLED_Time(time_temp, row1, row2);
				g_right_key_state = KEY_NONE;
			}


		}
		else if(g_work_state == MODIFY_TEMP)
		{
			if(g_left_key_state == KEY_SHORT_PRESS)
			{
				switch(Modify_Temp_choose)
				{
				case mintemp:
					if(L_temp > -40) {
						L_temp--;
						// 确保低温不能比高温高
						if(L_temp >= H_temp) {
							L_temp = H_temp - 1;
						}
					}
					break;
				case maxtemp:
					if(H_temp > -40) {
						H_temp--;
						// 确保高温不能比低温低
						if(H_temp <= L_temp) {
							H_temp = L_temp + 1;
						}
					}
					break;
				}
				// 调用公共刷新函数
				Refresh_OLED_Temp(row1, row2);
				g_left_key_state = KEY_NONE;
			}
			else if(g_right_key_state == KEY_SHORT_PRESS)
			{
				switch(Modify_Temp_choose)
				{
				case mintemp:
					if(L_temp < 85) {
						L_temp++;
						// 确保低温不能比高温高
						if(L_temp >= H_temp) {
							L_temp = H_temp - 1;
						}
					}
					break;
				case maxtemp:
					if(H_temp < 85) {
						H_temp++;
						// 确保高温不能比低温低
						if(H_temp <= L_temp) {
							H_temp = L_temp + 1;
						}
					}
					break;
				}
				// 调用公共刷新函数
				Refresh_OLED_Temp(row1, row2);
				g_right_key_state = KEY_NONE;
			}
		}
		osDelay(50);
	}
	/* USER CODE END StartlrbuttonListen */
}

/* USER CODE BEGIN Header_StartUartTestTask */
/**
 * @brief Function implementing the uartTestTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartUartTestTask */
void StartUartTestTask(void *argument)
{
	/* USER CODE BEGIN StartUartTestTask */
	/* Infinite loop */
	char message[150] = "";
	//	char er[6] = "";
	//	char ee[6] = "";
	//	char ea[6] = "";
	HAL_TIM_Base_Start(&htim1);
	float battery_volt=0.0f;
	for(;;)
	{
		//		if(g_home_key_state == KEY_SHORT_PRESS)
		//		{
		//			strcpy(er, "SHORT");
		//		}
		//		else if(g_home_key_state == KEY_LONG_PRESS)
		//		{
		//			strcpy(er, "LONG");
		//		}
		//		else if(g_home_key_state == KEY_NONE)
		//		{
		//			strcpy(er, "NO");
		//		}
		//		if(g_left_key_state == KEY_SHORT_PRESS)
		//		{
		//			strcpy(ee, "SHORT");
		//		}
		//		else if(g_left_key_state == KEY_LONG_PRESS)
		//		{
		//			strcpy(ee, "LONG");
		//		}
		//		else if(g_left_key_state == KEY_NONE)
		//		{
		//			strcpy(ee, "NO");
		//		}
		//		if(g_right_key_state == KEY_SHORT_PRESS)
		//		{
		//			strcpy(ea, "SHORT");
		//		}
		//		else if(g_right_key_state == KEY_LONG_PRESS)
		//		{
		//			strcpy(ea, "LONG");
		//		}
		//		else if(g_right_key_state == KEY_NONE)
		//		{
		//			strcpy(ea, "NO");
		//		}
		//		now = Clock_RTC_GetTime();
		//		sprintf(message,"%d-%d-%d %02d:%02d:%02d "
		//				"modify_num:%d home_ms:%ld home_key:%s "
		//				"l_key:%s r_key:%s "
		//				"bmp280_T:%.2f bmp280_P:%.2f"
		//				"ATH20_T:%.2f AHT20_H:%.2f"
		//				"DS18B20_T:%.2f",
		//				now->tm_year + 1900,now->tm_mon + 1,now->tm_mday,
		//				now->tm_hour,now->tm_min,now->tm_sec,Modify_Time_choose,g_home_key_press_ms,
		//				er,ee,ea,
		//				BMP280_Temperature,BMP280_Pressure,
		//				AHT20_Temperature,AHT20_Humidity,ds18b20_Temperature);
		//		HAL_UART_Transmit(&huart1, (uint8_t*)message, strlen(message), 100);


		//taskENTER_CRITICAL();

		battery_volt = ADC_Calculate_BatteryVoltage();
		if(battery_volt<3.3f)
			percentage = 0.0;
		else
			percentage = ((battery_volt - 3.3f) / (4.2f - 3.3f)) * 100.0f ;

		if(percentage>100)
			percentage=100.0;
		now = Clock_RTC_GetTime();
		snprintf(message,sizeof(message),"TIME(:)%d-%d-%d %02d:%02d:%02d(-)",
				now->tm_year + 1900,now->tm_mon + 1,now->tm_mday,
				now->tm_hour,now->tm_min,now->tm_sec);
		taskENTER_CRITICAL();
		HAL_UART_Transmit(&huart2, (uint8_t*)message, strlen(message), 100);
		taskEXIT_CRITICAL();
		memset(message, 0, sizeof(message));

		BMP280_Temperature = BMP280_ReadTemp();
		ds18b20_Temperature = DS18B20_ReadTemperature();
		BMP280_Pressure = BMP280_ReadPress();
		if(BMP280_Temperature>H_temp)
		{
			temp_flag = 1;
		}
		else if(BMP280_Temperature<L_temp)
		{
			temp_flag = 2;
		}
		else
		{
			temp_flag = 0;
		}
		if(BMP280_Pressure<L_preesure)
		{
			pre_flag = 1;
		}
		else
		{
			pre_flag = 0;
		}
		altitude = 44330.0 * (1.0 - pow( (BMP280_Pressure / BMP280_P0), 1.0 / 5.255 ));
		snprintf(message,sizeof(message),"TEMP1(:)%.2f(-)TEMP2(:)%.2f(-)Pressure(:)%.2f(-)HIGH(:)%.2f(-)",
				ds18b20_Temperature,BMP280_Temperature,BMP280_Pressure,altitude);
		taskENTER_CRITICAL();
		HAL_UART_Transmit(&huart2, (uint8_t*)message, strlen(message), 100);
		//taskEXIT_CRITICAL();
		memset(message, 0, sizeof(message));

		//HAL_Delay(10);
		snprintf(message,sizeof(message),"H_temp(:)%.2f(-)L_temp(:)%.2f(-)L_PRE(:)%.0f(-)OFF(:)%ld(-)",H_temp,L_temp,L_preesure,Screen_off_time);
		//taskENTER_CRITICAL();
		HAL_UART_Transmit(&huart2, (uint8_t*)message, strlen(message), 100);
		taskEXIT_CRITICAL();
		//taskEXIT_CRITICAL();
		osDelay(1000);
	}

	/* USER CODE END StartUartTestTask */
}

/* USER CODE BEGIN Header_StartOledshowTask */
/**
 * @brief Function implementing the oledshowTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartOledshowTask */
void StartOledshowTask(void *argument)
{
	/* USER CODE BEGIN StartOledshowTask */
	/* Infinite loop*/
	OLED_CLS();
	char row1[30] = "";
	char row2[30] = "";
	char row3[30] = "";
	for(;;)
	{
		switch(Current_Show_Picture)
		{
		case home_time:
			if(Current_Show_Picture != Picture_before)
				OLED_CLS();
			now = Clock_RTC_GetTime();
			//OLED_ShowStr(36, 0, (unsigned char*)"BAT", 2);
			sprintf(row1,"%02d-%02d-%02d",now->tm_year + 1900,now->tm_mon + 1,now->tm_mday);
			sprintf(row2,"%02d:%02d:%02d",now->tm_hour,now->tm_min,now->tm_sec);
			sprintf(row3,"BAT:%3.0f",percentage);
			OLED_ShowStr(50, 0, (unsigned char*)row3, 1);
			OLED_ShowStr(24, 1, (unsigned char*)row1, 2);
			OLED_ShowStr(32, 3, (unsigned char*)row2, 2);
			if(temp_flag == 1)
			{
				OLED_ShowStr(0, 6, (unsigned char*)"T_HIGH ", 1);
			}
			else if(temp_flag == 2)
			{
				OLED_ShowStr(0, 6, (unsigned char*)"T_LOW  ", 1);
			}
			else
			{
				OLED_ShowStr(0, 6, (unsigned char*)"T_heshi", 1);
			}
			if(pre_flag == 1)
			{
				OLED_ShowStr(50, 6, (unsigned char*)"P_LOW", 1);
			}
			else
			{
				OLED_ShowStr(50, 6, (unsigned char*)"     ", 1);
			}
			Picture_before = Current_Show_Picture;
			break;
		case temperature:
			if(Current_Show_Picture != Picture_before)
				OLED_CLS();
			//taskENTER_CRITICAL();
			BMP280_Temperature = BMP280_ReadTemp();
			//AHT20_Read(&AHT20_Temperature, &AHT20_Humidity);
			//AHT2需要hal_delay，会破坏调度，导致UI卡顿
			ds18b20_Temperature = DS18B20_ReadTemperature();
			//taskEXIT_CRITICAL();
			sprintf(row1,"BM280_T:%.2fC",BMP280_Temperature);
			//sprintf(row2,"AHT20_T:%.2fC",AHT20_Temperature);
			sprintf(row2,"18B20_T:%.2fC",ds18b20_Temperature);
			sprintf(row3,"BAT:%3.0f",percentage);
			OLED_ShowStr(50, 0, (unsigned char*)row3, 1);
			OLED_ShowCN_STR(48,1,0,2);
			OLED_ShowStr(0, 3, (unsigned char*)row1, 2);
			OLED_ShowStr(0, 5, (unsigned char*)row2, 2);
			sprintf(row2,"min:%.2f max:%.2f",L_temp,H_temp);
			//OLED_ShowStr(0, 6, (unsigned char*)row2, 2);
			OLED_ShowStr(0, 7, (unsigned char*)row2, 1);
			Picture_before = Current_Show_Picture;
			break;
		case pressure:
			if(Current_Show_Picture != Picture_before)
				OLED_CLS();
			BMP280_Pressure = BMP280_ReadPress();
			altitude = 44330.0 * (1.0 - pow( (BMP280_Pressure / BMP280_P0), 1.0 / 5.255 ));
			sprintf(row1,"BMP280_P:%.0fPa",BMP280_Pressure);
			sprintf(row2,"current_H:%.0fm",altitude);
			sprintf(row3,"BAT:%3.0f",percentage);
			OLED_ShowStr(50, 0, (unsigned char*)row3, 1);
			OLED_ShowCN_STR(48,2,2,2);
			OLED_ShowStr(0, 4, (unsigned char*)row1, 2);
			OLED_ShowStr(0, 6, (unsigned char*)row2, 2);
			Picture_before = Current_Show_Picture;
			break;
		case music:
			if(Current_Show_Picture != Picture_before)
				OLED_CLS();
			OLED_ShowCN_STR(48,2,8,2);
			Picture_before = Current_Show_Picture;
			break;
		case game:
			if(Current_Show_Picture != Picture_before)
			{
				OLED_CLS();
				//MPU6050_Init();
			}

			OLED_ShowCN_STR(48,3,6,2);
			//						MPU6050_GetData(&AccX, &AccY, &AccZ,&GyroX, &GyroY, &GyroZ);
			//						sprintf(row1,"%.2f %.2f %.2f",AccX,AccY,AccZ);
			//						sprintf(row2,"%.2f %.2f %.2f",GyroX,GyroY,GyroZ);
			//						sprintf(row3,"BAT:%.2f",percentage);
			//						OLED_ShowStr(36, 0, (unsigned char*)row3, 2);
			//						OLED_ShowStr(0, 4, (unsigned char*)row1, 2);
			//						OLED_ShowStr(0, 6, (unsigned char*)row2, 2);
			Picture_before = Current_Show_Picture;
			break;
		}
		osDelay(200);
		//HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_3);
	}
	/* USER CODE END StartOledshowTask */
}

/* USER CODE BEGIN Header_StartMPU6050Task */
/**
 * @brief Function implementing the mpu6050Task thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartMPU6050Task */
void StartMPU6050Task(void *argument)
{
	/* USER CODE BEGIN StartMPU6050Task */
	/* Infinite loop */
	float acc_x_change, acc_y_change, acc_z_change, total_acc_change;
	//MPU6050_Init();

	MPU6050_GetData(&AccX, &AccY, &AccZ, &GyroX, &GyroY, &GyroZ);
	last_AccX = AccX;
	last_AccY = AccY;
	last_AccZ = AccZ;
	// 新增：记录最后一次有效运动（加速度超阈值）的时间戳（FreeRTOS节拍数）
	TickType_t xLast_Valid_Motion_Ticks = xTaskGetTickCount();

	// 固定短延时（保证检测实时性，推荐100ms，可根据需求调整）
	for(;;)
	{
		// ========== 步骤1：获取当前3轴加速度数据 ==========
		MPU6050_GetData(&AccX, &AccY, &AccZ, &GyroX, &GyroY, &GyroZ);

		// ========== 步骤2：计算3轴加速度的动态变化量 ==========
		// 计算当前与上一次的差值（绝对值，避免正负抵消）
		acc_x_change = fabsf(AccX - last_AccX);
		acc_y_change = fabsf(AccY - last_AccY);
		acc_z_change = fabsf(AccZ - last_AccZ);

		// 计算总变化量（3轴变化量求和，提高检测灵敏度）
		total_acc_change = acc_x_change + acc_y_change + acc_z_change;


		// ========== 步骤3：核心逻辑：判断运动状态，处理亮屏/息屏（修改后的核心） ==========
		if(oled_screen_on == 1)  // 亮屏状态：判断是否30秒无有效运动
		{
			// 子判断1：检测到有效运动（加速度超阈值），重置时间戳
			if(total_acc_change > ACC_CHANGE_THRESHOLD)
			{
				xLast_Valid_Motion_Ticks = xTaskGetTickCount();  // 重置最后一次有效运动时间戳
			}
			// 子判断2：计算累计无有效运动时长，达到30秒则息屏
			else
			{
				// 计算当前无有效运动时长（FreeRTOS溢出安全写法，无符号类型差值自动处理）
				TickType_t xNo_Motion_Ticks = xTaskGetTickCount() - xLast_Valid_Motion_Ticks;
				if(xNo_Motion_Ticks >= Screen_off_time)
				{
					OLED_OFF();  // 息屏
					oled_screen_on = 0;  // 更新屏幕状态
				}
			}
		}
		else if(oled_screen_on == 0)  // 息屏状态：实时检测是否有有效运动，唤醒屏幕
		{
			if(total_acc_change > ACC_CHANGE_THRESHOLD)
			{
				OLED_ON();  // 唤醒屏幕
				oled_screen_on = 1;  // 更新屏幕状态
				xLast_Valid_Motion_Ticks = xTaskGetTickCount();  // 重置时间戳，重新开始30秒计时
			}
		}

		// ========== 步骤4：更新数据和计数器（你的原有逻辑，保持不变） ==========
		axy_check_counter++;
		last_AccX = AccX;
		last_AccY = AccY;
		last_AccZ = AccZ;

		// ========== 步骤5：任务周期性延时（固定短延时，释放CPU，保证实时检测） ==========
		// HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_3);
		osDelay(100);
	}
	/* USER CODE END StartMPU6050Task */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

void delay_ms(uint32_t ms)
{
	uint32_t i, j;
	for(i = 0; i < ms; i++)
	{
		for(j = 0; j < 1000; j++); // 内循环：约1ms（根据MCU主频微调）
	}
}

void gameTask(void * pvParameters)
{
	// 玩家字符A相关变量
	uint8_t u8Last_X_Pos_A = 0;
	float fFiltered_AccX = 0.0f; // 滤波后的AccX，减少抖动
	// 时序计数器（替代xTaskGetTickCount()，累加毫秒数）
	uint32_t u32Spawn_Cnt = 0;   // 距离上一次生成V的时间计数器
	uint32_t u32Move_Cnt = 0;    // 距离上一次下移V的时间计数器
	// 游戏状态标志
	uint8_t u8Game_Over = 0;
	// 伪随机数种子（用于V的X坐标生成，避免重复）
	uint32_t u32Rand_Seed = 0;
	uint32_t fen = 0;
	char fens[10];

	// ========== 初始化 ==========
	OLED_CLS(); // 清屏（确保游戏界面干净）
	// 初始化玩家字符A
	OLED_ShowStr(SCREEN_X_MIN, GAME_DISPLAY_PAGE_A, (unsigned char*)GAME_CHAR_A, 1);
	u8Last_X_Pos_A = SCREEN_X_MIN;
	// 初始化V数组（全部置为无效）
	for(uint8_t i=0; i<MAX_V_COUNT; i++)
	{
		V_Array[i].is_valid = 0;
	}
	// 初始化伪随机数种子
	u32Rand_Seed = 12345;

	// ========== 按照你的要求：进入临界区，包裹整个主循环（永久锁定） ==========
	//taskENTER_CRITICAL();
	//MPU6050_Init();

	// ========== 游戏主循环（不依赖xTaskGetTickCount()，用软件延时+计数器实现时序） ==========
	for(;;)
	{
		// ========== 游戏结束判断：若结束则显示结束信息 ==========
		if(u8Game_Over == 1)
		{
			OLED_CLS();
			OLED_ShowStr(40, 3, (unsigned char*)"GAME OVER", 1);
			sprintf(fens,"score:%ld ",fen);
			OLED_ShowStr(45, 5, (unsigned char*)fens, 1);
			osDelay(1000); // 使用FreeRTOS延时，兼容调度
			vTaskSuspend(NULL);
		}

		// ========== 步骤1：处理玩家字符A的移动（直接读取MPU6050，不依赖其他任务） ==========
		// 直接在任务内读取MPU6050（其他任务已卡死，无法复用全局AccX）
		float AccX, AccY, AccZ, GyroX, GyroY, GyroZ;
		MPU6050_GetData(&AccX, &AccY, &AccZ, &GyroX, &GyroY, &GyroZ);

		// 滑动滤波：消除微小抖动
		fFiltered_AccX = FILTER_ALPHA * (-AccX) + (1 - FILTER_ALPHA) * fFiltered_AccX;

		// 加速度限幅
		float fAccX_Curr = fFiltered_AccX;
		if(fAccX_Curr < ACC_X_MIN) fAccX_Curr = ACC_X_MIN;
		if(fAccX_Curr > ACC_X_MAX) fAccX_Curr = ACC_X_MAX;

		// 映射到屏幕X坐标
		uint8_t u8Curr_X_Pos_A = (uint8_t)(((fAccX_Curr - ACC_X_MIN) / (ACC_X_MAX - ACC_X_MIN)) * (SCREEN_X_MAX - SCREEN_X_MIN) + SCREEN_X_MIN);

		// 坐标节流：只有变化超过阈值才更新OLED，减少I2C开销
		if(abs(u8Curr_X_Pos_A - u8Last_X_Pos_A) >= POS_CHANGE_THRESH)
		{
			OLED_ShowStr(u8Last_X_Pos_A, GAME_DISPLAY_PAGE_A, (unsigned char*)" ", 1);
			OLED_ShowStr(u8Curr_X_Pos_A, GAME_DISPLAY_PAGE_A, (unsigned char*)GAME_CHAR_A, 1);
			u8Last_X_Pos_A = u8Curr_X_Pos_A;
		}

		// ========== 步骤2：软件延时+计数器累加（实现1秒时序） ==========
		delay_ms(20); // 每次循环延时20ms
		u32Spawn_Cnt += 20; // 累加生成V的计数器
		u32Move_Cnt += 20;  // 累加下移V的计数器

		// ========== 步骤3：计数器判断：每秒生成1个V（最多10个） ==========
		if(u32Spawn_Cnt >= V_SPAWN_INTERVAL)
		{
			// 更新伪随机数种子
			u32Rand_Seed += 7;
			// 寻找数组中第一个无效的V
			for(uint8_t i=0; i<MAX_V_COUNT; i++)
			{
				if(V_Array[i].is_valid == 0)
				{
					// 生成伪随机X坐标（基于种子，避免重复）
					V_Array[i].x_pos = (uint8_t)(rand() % (SCREEN_X_MAX + 1));
					V_Array[i].page = GAME_DISPLAY_PAGE_V_INIT;
					V_Array[i].is_valid = 1;

					// 绘制新生成的V
					OLED_ShowStr(V_Array[i].x_pos, V_Array[i].page, (unsigned char*)GAME_CHAR_V, 1);
					break;
				}
			}

			// 清零生成V的计数器，准备下一次计时
			u32Spawn_Cnt = 0;
			if(V_SPAWN_INTERVAL>21)
				V_SPAWN_INTERVAL--;
		}

		// ====s====== 步骤4：计数器判断：每秒移动所有有效V向下一页 ==========
		if(u32Move_Cnt >= V_MOVE_INTERVAL)
		{
			for(uint8_t i=0; i<MAX_V_COUNT; i++)
			{
				if(V_Array[i].is_valid == 1)
				{
					// 清除当前V的位置（防残影）
					OLED_ShowStr(V_Array[i].x_pos, V_Array[i].page, (unsigned char*)" ", 1);

					// 更新V的页号（向下移动一页）
					V_Array[i].page++;

					// 判断V是否超出屏幕（页号>7，OLED最大页为7）
					if(V_Array[i].page > 7)
					{
						V_Array[i].is_valid = 0;
						fen++;
						continue;
					}

					// 绘制移动后的V
					OLED_ShowStr(V_Array[i].x_pos, V_Array[i].page, (unsigned char*)GAME_CHAR_V, 1);
				}
			}

			// 清零下移V的计数器，准备下一次计时
			u32Move_Cnt = 0;
			if(V_MOVE_INTERVAL>=21)
				V_MOVE_INTERVAL--;
		}

		// ========== 步骤5：碰撞判断（V与A碰撞则游戏结束） ==========
		for(uint8_t i=0; i<MAX_V_COUNT; i++)
		{
			if(V_Array[i].is_valid == 1)
			{
				// 碰撞条件：同页 + X坐标重叠
				if((V_Array[i].page == GAME_DISPLAY_PAGE_A) && (abs(V_Array[i].x_pos - u8Last_X_Pos_A) < CHAR_WIDTH))
				{
					u8Game_Over = 1;//taskEXIT_CRITICAL();
					break;
				}
			}
		}
	}
}

void musicTask(void * pvParameters)
{
	/* USER CODE BEGIN musicTask */
	/* ========== 性能调节 ========== */
#define TARGET_VOLUME 0.15f  // 15% 占空比，保证高频不失真
#define NOTE_GAP_MS   15     // 音符间隙

	/* ========== 全音域频率表 (严格对齐) ========== */
	const uint32_t freq_tab[] = {
			0,                                                  // 0: 休止
			131, 147, 165, 175, 196, 220, 247,                  // 11~17: 低音
			0, 0, 0,                                            // 8~10: 占位
			262, 294, 330, 349, 392, 440, 494,                  // 1~7: 中音
			0, 0, 0,                                            // 18~20: 占位
			523, 587, 659, 698, 784, 880, 988,                  // 21~27: 高音
			0, 0, 0,                                            // 28~30: 占位
			1046, 1175, 1318, 1397, 1568, 1760, 1976            // 31~37: 超高音
	};

	/* 拍长单位 (ms) */
	const uint32_t beat_ms[] = {0, 125, 250, 375, 500};

	/* ========== 驱动函数 ========== */
	void Buzzer_Set(uint32_t freq)
	{
		if(freq < 50)
		{
			HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
			return;
		}
		uint16_t arr = (uint16_t)(1000000 / freq) - 1;

		HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
		__HAL_TIM_SET_AUTORELOAD(&htim1, arr);
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, (uint16_t)(arr * TARGET_VOLUME));
		HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	}

	void Play_Music(void)
	{
		for(uint32_t i = 0; ; i++)
		{
			unsigned char note = music_config[i][0];
			unsigned char beat = music_config[i][1];
			if(note == 0 && beat == 0) break;

			//索引范围检查，防止访问数组越界导致哑音
			if(note < (sizeof(freq_tab)/sizeof(freq_tab[0])))
			{
				Buzzer_Set(freq_tab[note]);
				osDelay(beat_ms[beat]);
			}
			Buzzer_Set(0);
			osDelay(NOTE_GAP_MS);
		}
	}

	/* 启动PWM输出 */
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);

	/* Infinite loop */
	for(;;)
	{
		Play_Music();
		osDelay(2000);
	}
	/* USER CODE END musicTask */
}
/* USER CODE END Application */

