#include "flash_storage.h"
#include "common.h"

HAL_StatusTypeDef Flash_Storage_Erase_Sector7(void){
    FLASH_EraseInitTypeDef flash_erase_init;
    uint32_t erase_error_addr = 0;
    HAL_StatusTypeDef status = HAL_FLASH_Unlock();
    
    if (status != HAL_OK) return status;

    flash_erase_init.TypeErase = TYPEERASE_PAGES;
    flash_erase_init.Banks = FLASH_BANK_1;
    flash_erase_init.PageAddress = FLASH_SECTOR7_BASE_ADDR;
    flash_erase_init.NbPages = FLASH_SECTOR7_PAGE_COUNT;

    status = HAL_FLASHEx_Erase(&flash_erase_init, &erase_error_addr);
    if (status != HAL_OK) { HAL_FLASH_Lock(); return status; }
    
    HAL_FLASH_Lock();
    return HAL_OK;
}

HAL_StatusTypeDef Flash_Storage_Write_Thresh(Env_Thresh_Config_t *pNewConfig){
    if (pNewConfig == NULL) return HAL_ERROR;

    HAL_StatusTypeDef status = Flash_Storage_Erase_Sector7();
    if (status != HAL_OK) return status;

    status = HAL_FLASH_Unlock();
    if (status != HAL_OK) return status;

    uint16_t *p_src_data = (uint16_t *)pNewConfig;
    uint32_t write_addr = FLASH_SECTOR7_BASE_ADDR;
    for (uint32_t i = 0; i < (ENV_CONFIG_SIZE / 2); i++){
        status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, write_addr, *p_src_data++);
        if (status != HAL_OK) { HAL_FLASH_Lock(); return status; }
        write_addr += 2;
    }

    HAL_FLASH_Lock();
    return HAL_OK;
}

HAL_StatusTypeDef Flash_Storage_Read_Latest_Thresh(Env_Thresh_Config_t *pLatestConfig){
    if (pLatestConfig == NULL) return HAL_ERROR;

    uint8_t is_all_ff = 1;
    uint8_t *p_flash = (uint8_t *)FLASH_SECTOR7_BASE_ADDR;
    for (uint32_t i = 0; i < ENV_CONFIG_SIZE; i++){
        if (*p_flash++ != 0xFF) { is_all_ff = 0; break; }
    }

    if (is_all_ff){
        pLatestConfig->temp_high_thresh = 30.0f;
        pLatestConfig->temp_low_thresh = 5.0f;
        pLatestConfig->press_low_thresh = 80000.0f;
        pLatestConfig->Screen_off_time = 30000;
        Flash_Storage_Write_Thresh(pLatestConfig);
    }
    else{
        uint8_t *p_src = (uint8_t *)FLASH_SECTOR7_BASE_ADDR;
        uint8_t *p_dst = (uint8_t *)pLatestConfig;
        for (uint32_t i = 0; i < ENV_CONFIG_SIZE; i++) *p_dst++ = *p_src++;
    }

    return HAL_OK;
}
