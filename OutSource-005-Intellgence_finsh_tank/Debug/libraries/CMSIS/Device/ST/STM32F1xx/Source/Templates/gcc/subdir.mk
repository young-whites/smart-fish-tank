################################################################################
# 自动生成的文件。不要编辑！
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_UPPER_SRCS += \
../libraries/CMSIS/Device/ST/STM32F1xx/Source/Templates/gcc/startup_stm32f103xb.S 

OBJS += \
./libraries/CMSIS/Device/ST/STM32F1xx/Source/Templates/gcc/startup_stm32f103xb.o 

S_UPPER_DEPS += \
./libraries/CMSIS/Device/ST/STM32F1xx/Source/Templates/gcc/startup_stm32f103xb.d 


# Each subdirectory must supply rules for building sources it contributes
libraries/CMSIS/Device/ST/STM32F1xx/Source/Templates/gcc/%.o: ../libraries/CMSIS/Device/ST/STM32F1xx/Source/Templates/gcc/%.S
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -O0 -ffunction-sections -fdata-sections -Wall  -g -gdwarf-2 -x assembler-with-cpp -I"I:\Outsource_Design_Workspace\RT-Thread-Studio_Workspace\Floder-005-Intellgence_finsh_tank\OutSource-005-Intellgence_finsh_tank" -Xassembler -mimplicit-it=thumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"

