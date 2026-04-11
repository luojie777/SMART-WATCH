#include "DS18B20.h"
#include "common.h"

uint8_t DS18B20_Init(){
    uint8_t result;
    HAL_GPIO_WritePin(DS18B20_GPIO_Port, DS18B20_Pin, GPIO_PIN_RESET);
    Delay_us(480);
    HAL_GPIO_WritePin(DS18B20_GPIO_Port, DS18B20_Pin, GPIO_PIN_SET);
    Delay_us(60);
    result = HAL_GPIO_ReadPin(DS18B20_GPIO_Port, DS18B20_Pin) == GPIO_PIN_RESET ? 0 : 1;
    Delay_us(420);
    return result;
}

void DS18B20_WriteByte(uint8_t dat){
    uint8_t i;
    for(i=0; i<8; i++){
        HAL_GPIO_WritePin(DS18B20_GPIO_Port, DS18B20_Pin, GPIO_PIN_RESET);
        Delay_us(1);
        if(dat&0x01) HAL_GPIO_WritePin(DS18B20_GPIO_Port, DS18B20_Pin, GPIO_PIN_SET);
        else HAL_GPIO_WritePin(DS18B20_GPIO_Port, DS18B20_Pin, GPIO_PIN_RESET);
        Delay_us(60);
        HAL_GPIO_WritePin(DS18B20_GPIO_Port, DS18B20_Pin, GPIO_PIN_SET);
        dat >>= 1;
    }
}

uint8_t DS18B20_ReadByte(void){
    uint8_t i, dat=0;
    for(i=0; i<8; i++){
        HAL_GPIO_WritePin(DS18B20_GPIO_Port, DS18B20_Pin, GPIO_PIN_RESET);
        Delay_us(1);
        HAL_GPIO_WritePin(DS18B20_GPIO_Port, DS18B20_Pin, GPIO_PIN_SET);
        Delay_us(1);
        if(HAL_GPIO_ReadPin(DS18B20_GPIO_Port, DS18B20_Pin)) dat |= (1<<i);
        Delay_us(60);
    }
    return dat;
}

float DS18B20_ReadTemperature(void){
    taskENTER_CRITICAL();
    uint8_t temp_L, temp_H;
    int16_t temp;
    float temperature;

    DS18B20_Init();
    DS18B20_WriteByte(0xCC);
    DS18B20_WriteByte(0x44);
    while(DS18B20_ReadByte()!=0xFF){}

    DS18B20_Init();
    DS18B20_WriteByte(0xCC);
    DS18B20_WriteByte(0xBE);

    temp_L = DS18B20_ReadByte();
    temp_H = DS18B20_ReadByte();
    temp = (temp_H << 8) | temp_L;
    temperature = (float)temp / 16.0f;

    taskEXIT_CRITICAL();
    return temperature;
}