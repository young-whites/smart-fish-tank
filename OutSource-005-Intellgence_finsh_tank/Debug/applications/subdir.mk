################################################################################
# 自动生成的文件。不要编辑！
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../applications/app_alarm.c \
../applications/app_control.c \
../applications/app_data.c \
../applications/app_display.c \
../applications/app_feed.c \
../applications/app_key.c \
../applications/app_sensor.c \
../applications/app_wifi.c \
../applications/drv_oled.c \
../applications/main.c 

OBJS += \
./applications/app_alarm.o \
./applications/app_control.o \
./applications/app_data.o \
./applications/app_display.o \
./applications/app_feed.o \
./applications/app_key.o \
./applications/app_sensor.o \
./applications/app_wifi.o \
./applications/drv_oled.o \
./applications/main.o 

C_DEPS += \
./applications/app_alarm.d \
./applications/app_control.d \
./applications/app_data.d \
./applications/app_display.d \
./applications/app_feed.d \
./applications/app_key.d \
./applications/app_sensor.d \
./applications/app_wifi.d \
./applications/drv_oled.d \
./applications/main.d 


# Each subdirectory must supply rules for building sources it contributes
applications/%.o: ../applications/%.c
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -O0 -ffunction-sections -fdata-sections -Wall  -g -gdwarf-2 -DSOC_FAMILY_STM32 -DSOC_SERIES_STM32F1 -DUSE_HAL_DRIVER -DSTM32F103xB -I"I:\Outsource_Design_Workspace\RT-Thread-Studio_Workspace\Floder-005-Intellgence_finsh_tank\OutSource-005-Intellgence_finsh_tank\drivers" -I"I:\Outsource_Design_Workspace\RT-Thread-Studio_Workspace\Floder-005-Intellgence_finsh_tank\OutSource-005-Intellgence_finsh_tank\libraries\CMSIS\Device\ST\STM32F1xx\Include" -I"I:\Outsource_Design_Workspace\RT-Thread-Studio_Workspace\Floder-005-Intellgence_finsh_tank\OutSource-005-Intellgence_finsh_tank\libraries\CMSIS\Include" -I"I:\Outsource_Design_Workspace\RT-Thread-Studio_Workspace\Floder-005-Intellgence_finsh_tank\OutSource-005-Intellgence_finsh_tank\libraries\CMSIS\RTOS\Template" -I"I:\Outsource_Design_Workspace\RT-Thread-Studio_Workspace\Floder-005-Intellgence_finsh_tank\OutSource-005-Intellgence_finsh_tank\libraries\STM32F1xx_HAL_Driver\Inc" -I"I:\Outsource_Design_Workspace\RT-Thread-Studio_Workspace\Floder-005-Intellgence_finsh_tank\OutSource-005-Intellgence_finsh_tank\libraries\STM32F1xx_HAL_Driver\Inc\Legacy" -I"I:\Outsource_Design_Workspace\RT-Thread-Studio_Workspace\Floder-005-Intellgence_finsh_tank\OutSource-005-Intellgence_finsh_tank" -I"I:\Outsource_Design_Workspace\RT-Thread-Studio_Workspace\Floder-005-Intellgence_finsh_tank\OutSource-005-Intellgence_finsh_tank\applications" -I"I:\Outsource_Design_Workspace\RT-Thread-Studio_Workspace\Floder-005-Intellgence_finsh_tank\OutSource-005-Intellgence_finsh_tank" -I"I:\Outsource_Design_Workspace\RT-Thread-Studio_Workspace\Floder-005-Intellgence_finsh_tank\OutSource-005-Intellgence_finsh_tank\cubemx\Inc" -I"I:\Outsource_Design_Workspace\RT-Thread-Studio_Workspace\Floder-005-Intellgence_finsh_tank\OutSource-005-Intellgence_finsh_tank\cubemx" -I"I:\Outsource_Design_Workspace\RT-Thread-Studio_Workspace\Floder-005-Intellgence_finsh_tank\OutSource-005-Intellgence_finsh_tank\rt-thread\components\finsh" -I"I:\Outsource_Design_Workspace\RT-Thread-Studio_Workspace\Floder-005-Intellgence_finsh_tank\OutSource-005-Intellgence_finsh_tank\rt-thread\include" -I"I:\Outsource_Design_Workspace\RT-Thread-Studio_Workspace\Floder-005-Intellgence_finsh_tank\OutSource-005-Intellgence_finsh_tank\rt-thread\libcpu\arm\common" -I"I:\Outsource_Design_Workspace\RT-Thread-Studio_Workspace\Floder-005-Intellgence_finsh_tank\OutSource-005-Intellgence_finsh_tank\rt-thread\libcpu\arm\cortex-m3" -include"I:\Outsource_Design_Workspace\RT-Thread-Studio_Workspace\Floder-005-Intellgence_finsh_tank\OutSource-005-Intellgence_finsh_tank\rtconfig_preinc.h" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"

