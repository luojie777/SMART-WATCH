#include "common.h"
#include "tim.h"

void Delay_us(uint32_t us)
{
    __HAL_TIM_SET_COUNTER(&htim2, 0);  // 计数器清 0
    while(__HAL_TIM_GET_COUNTER(&htim2) < us); // 等待到达 us
}

void Delay_Us(uint32_t us)
{
    __HAL_TIM_SET_COUNTER(&htim3, 0);  // 计数器清 0
    while(__HAL_TIM_GET_COUNTER(&htim3) < us); // 等待到达 us
}



