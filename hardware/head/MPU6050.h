#ifndef MPU6050_H
#define MPU6050_H

#include "stm32f1xx_hal.h"
#include "i2c.h"

// MPU6050的I2C从机地址
#define MPU6050_ADDRESS 0xD0

// 采样率分频寄存器
#define MPU6050_SMPLRT_DIV      0x19

// 配置寄存器
#define MPU6050_CONFIG          0x1A

// 陀螺仪配置寄存器
#define MPU6050_GYRO_CONFIG     0x1B

// 加速度计配置寄存器
#define MPU6050_ACCEL_CONFIG    0x1C

// 加速度计X轴输出高字节
#define MPU6050_ACCEL_XOUT_H    0x3B
// 加速度计X轴输出低字节
#define MPU6050_ACCEL_XOUT_L    0x3C
// 加速度计Y轴输出高字节
#define MPU6050_ACCEL_YOUT_H    0x3D
// 加速度计Y轴输出低字节
#define MPU6050_ACCEL_YOUT_L    0x3E
// 加速度计Z轴输出高字节
#define MPU6050_ACCEL_ZOUT_H    0x3F
// 加速度计Z轴输出低字节
#define MPU6050_ACCEL_ZOUT_L    0x40
// 温度输出高字节
#define MPU6050_TEMP_OUT_H      0x41
// 温度输出低字节
#define MPU6050_TEMP_OUT_L      0x42
// 陀螺仪X轴输出高字节
#define MPU6050_GYRO_XOUT_H     0x43
// 陀螺仪X轴输出低字节
#define MPU6050_GYRO_XOUT_L     0x44
// 陀螺仪Y轴输出高字节
#define MPU6050_GYRO_YOUT_H     0x45
// 陀螺仪Y轴输出低字节
#define MPU6050_GYRO_YOUT_L     0x46
// 陀螺仪Z轴输出高字节
#define MPU6050_GYRO_ZOUT_H     0x47
// 陀螺仪Z轴输出低字节
#define MPU6050_GYRO_ZOUT_L     0x48

// 电源管理1寄存器
#define MPU6050_PWR_MGMT_1      0x6B
// 电源管理2寄存器
#define MPU6050_PWR_MGMT_2      0x6C
// WHO_AM_I寄存器
#define MPU6050_WHO_AM_I        0x75

// 函数声明
uint8_t MPU6050_ReadReg(uint8_t RegAddress);
void MPU6050_WriteReg(uint8_t RegAddress, uint8_t Data);
void MPU6050_Init(void);
uint8_t MPU6050_GetID(void);
void MPU6050_GetData(float *AccX, float *AccY, float *AccZ, float *GyroX, float *GyroY, float *GyroZ);

#endif
