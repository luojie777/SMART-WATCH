#ifndef __FLASH_STORAGE_H
#define __FLASH_STORAGE_H
#include "stm32f1xx_hal.h"

// 系统选用Sector7扇区作为专用存储区域，起始地址为0x08018000，大小16KB
// 程序代码结束地址约为0x08013d20（79KB），此地址避免冲突
#define FLASH_SECTOR7_BASE_ADDR 0x08018000    // Flash存储起始地址（96KB处）
#define FLASH_SECTOR7_PAGE_COUNT 16           // 存储区域大小（16页，16KB）
#define ENV_CONFIG_SIZE 19                    // 配置结构体大小（19字节）
#define RTC_INIT_FLAG 0x5A5A                 // RTC初始化标志

// 阈值配置结构体
typedef struct{
    float temp_high_thresh;
    float temp_low_thresh;
    float press_low_thresh;
    uint32_t Screen_off_time;
    uint8_t alarm_hour;
    uint8_t alarm_min;
    uint8_t alarm_second;
}Env_Thresh_Config_t;

HAL_StatusTypeDef Flash_Storage_Erase_Sector7(void);
HAL_StatusTypeDef Flash_Storage_Write_Thresh(Env_Thresh_Config_t *pNewConfig);
HAL_StatusTypeDef Flash_Storage_Read_Latest_Thresh(Env_Thresh_Config_t *pLatestConfig);

#endif
