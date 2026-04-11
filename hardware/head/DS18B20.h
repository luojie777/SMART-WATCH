#ifndef __DS18B20_H
#define __DS18B20_H
#include "stm32f1xx_hal.h"

#define DS18B20_GPIO_Port GPIOA
#define DS18B20_Pin GPIO_PIN_0

uint8_t DS18B20_Init(void);
void DS18B20_WriteByte(uint8_t dat);
uint8_t DS18B20_ReadByte(void);
float DS18B20_ReadTemperature(void);

#endif