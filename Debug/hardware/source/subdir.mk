################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../hardware/source/BMP280.c \
../hardware/source/DS18B20.c \
../hardware/source/MPU6050.c \
../hardware/source/MYIIC.c \
../hardware/source/OLED.c \
../hardware/source/clock.c \
../hardware/source/common.c \
../hardware/source/flash_storage.c \
../hardware/source/game.c 

OBJS += \
./hardware/source/BMP280.o \
./hardware/source/DS18B20.o \
./hardware/source/MPU6050.o \
./hardware/source/MYIIC.o \
./hardware/source/OLED.o \
./hardware/source/clock.o \
./hardware/source/common.o \
./hardware/source/flash_storage.o \
./hardware/source/game.o 

C_DEPS += \
./hardware/source/BMP280.d \
./hardware/source/DS18B20.d \
./hardware/source/MPU6050.d \
./hardware/source/MYIIC.d \
./hardware/source/OLED.d \
./hardware/source/clock.d \
./hardware/source/common.d \
./hardware/source/flash_storage.d \
./hardware/source/game.d 


# Each subdirectory must supply rules for building sources it contributes
hardware/source/%.o hardware/source/%.su hardware/source/%.cyclo: ../hardware/source/%.c hardware/source/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3 -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I"D:/STM32_watch/hardware/head" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-hardware-2f-source

clean-hardware-2f-source:
	-$(RM) ./hardware/source/BMP280.cyclo ./hardware/source/BMP280.d ./hardware/source/BMP280.o ./hardware/source/BMP280.su ./hardware/source/DS18B20.cyclo ./hardware/source/DS18B20.d ./hardware/source/DS18B20.o ./hardware/source/DS18B20.su ./hardware/source/MPU6050.cyclo ./hardware/source/MPU6050.d ./hardware/source/MPU6050.o ./hardware/source/MPU6050.su ./hardware/source/MYIIC.cyclo ./hardware/source/MYIIC.d ./hardware/source/MYIIC.o ./hardware/source/MYIIC.su ./hardware/source/OLED.cyclo ./hardware/source/OLED.d ./hardware/source/OLED.o ./hardware/source/OLED.su ./hardware/source/clock.cyclo ./hardware/source/clock.d ./hardware/source/clock.o ./hardware/source/clock.su ./hardware/source/common.cyclo ./hardware/source/common.d ./hardware/source/common.o ./hardware/source/common.su ./hardware/source/flash_storage.cyclo ./hardware/source/flash_storage.d ./hardware/source/flash_storage.o ./hardware/source/flash_storage.su ./hardware/source/game.cyclo ./hardware/source/game.d ./hardware/source/game.o ./hardware/source/game.su

.PHONY: clean-hardware-2f-source

