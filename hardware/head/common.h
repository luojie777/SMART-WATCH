#ifndef __COMMON_H
#define __COMMON_H
#include "stm32f1xx_hal.h"
#include "cmsis_os.h"

// 延时函数
void Delay_us(uint32_t us);
void Delay_Us(uint32_t us);
void Delay_ms(uint32_t ms);

// 错误处理
void Error_Handler(void);

#endif
