#ifndef __CLOCK_H
#define __CLOCK_H
#include "stm32f1xx_hal.h"
#include "rtc.h"
#include <time.h>


#define RTC_INIT_FLAG 0x5A5A

struct tm* Clock_RTC_GetTime(void);
HAL_StatusTypeDef Clock_RTC_SetTime(struct tm *time);
void Clock_RTC_Init(void);

#endif
