################################################################################
# 自动生成的文件。不要编辑！
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../rt-thread/libcpu/arm/common/backtrace.c \
../rt-thread/libcpu/arm/common/div0.c \
../rt-thread/libcpu/arm/common/showmem.c 

OBJS += \
./rt-thread/libcpu/arm/common/backtrace.o \
./rt-thread/libcpu/arm/common/div0.o \
./rt-thread/libcpu/arm/common/showmem.o 

C_DEPS += \
./rt-thread/libcpu/arm/common/backtrace.d \
./rt-thread/libcpu/arm/common/div0.d \
./rt-thread/libcpu/arm/common/showmem.d 


# Each subdirectory must supply rules for building sources it contributes
rt-thread/libcpu/arm/common/%.o: ../rt-thread/libcpu/arm/common/%.c
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -O0 -ffunction-sections -fdata-sections -Wall  -g -gdwarf-2 -DSOC_FAMILY_STM32 -DSOC_SERIES_STM32F1 -DUSE_HAL_DRIVER -DSTM32F103xB -I"I:\Outsource_Design_Workspace\RT-Thread-Studio_Workspace\Floder-005-Intellgence_finsh_tank\OutSource-005-Intellgence_finsh_tank\drivers" -I"I:\Outsource_Design_Workspace\RT-Thread-Studio_Workspace\Floder-005-Intellgence_finsh_tank\OutSource-005-Intellgence_finsh_tank\libraries\CMSIS\Device\ST\STM32F1xx\Include" -I"I:\Outsource_Design_Workspace\RT-Thread-Studio_Workspace\Floder-005-Intellgence_finsh_tank\OutSource-005-Intellgence_finsh_tank\libraries\CMSIS\Include" -I"I:\Outsource_Design_Workspace\RT-Thread-Studio_Workspace\Floder-005-Intellgence_finsh_tank\OutSource-005-Intellgence_finsh_tank\libraries\CMSIS\RTOS\Template" -I"I:\Outsource_Design_Workspace\RT-Thread-Studio_Workspace\Floder-005-Intellgence_finsh_tank\OutSource-005-Intellgence_finsh_tank\libraries\STM32F1xx_HAL_Driver\Inc" -I"I:\Outsource_Design_Workspace\RT-Thread-Studio_Workspace\Floder-005-Intellgence_finsh_tank\OutSource-005-Intellgence_finsh_tank\libraries\STM32F1xx_HAL_Driver\Inc\Legacy" -I"I:\Outsource_Design_Workspace\RT-Thread-Studio_Workspace\Floder-005-Intellgence_finsh_tank\OutSource-005-Intellgence_finsh_tank" -I"I:\Outsource_Design_Workspace\RT-Thread-Studio_Workspace\Floder-005-Intellgence_finsh_tank\OutSource-005-Intellgence_finsh_tank\applications" -I"I:\Outsource_Design_Workspace\RT-Thread-Studio_Workspace\Floder-005-Intellgence_finsh_tank\OutSource-005-Intellgence_finsh_tank" -I"I:\Outsource_Design_Workspace\RT-Thread-Studio_Workspace\Floder-005-Intellgence_finsh_tank\OutSource-005-Intellgence_finsh_tank\cubemx\Inc" -I"I:\Outsource_Design_Workspace\RT-Thread-Studio_Workspace\Floder-005-Intellgence_finsh_tank\OutSource-005-Intellgence_finsh_tank\cubemx" -I"I:\Outsource_Design_Workspace\RT-Thread-Studio_Workspace\Floder-005-Intellgence_finsh_tank\OutSource-005-Intellgence_finsh_tank\rt-thread\components\finsh" -I"I:\Outsource_Design_Workspace\RT-Thread-Studio_Workspace\Floder-005-Intellgence_finsh_tank\OutSource-005-Intellgence_finsh_tank\rt-thread\include" -I"I:\Outsource_Design_Workspace\RT-Thread-Studio_Workspace\Floder-005-Intellgence_finsh_tank\OutSource-005-Intellgence_finsh_tank\rt-thread\libcpu\arm\common" -I"I:\Outsource_Design_Workspace\RT-Thread-Studio_Workspace\Floder-005-Intellgence_finsh_tank\OutSource-005-Intellgence_finsh_tank\rt-thread\libcpu\arm\cortex-m3" -include"I:\Outsource_Design_Workspace\RT-Thread-Studio_Workspace\Floder-005-Intellgence_finsh_tank\OutSource-005-Intellgence_finsh_tank\rtconfig_preinc.h" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"

