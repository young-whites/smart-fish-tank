21:32:26 **** Build of configuration Debug for project OutSource-005-Intellgence_finsh_tank ****
make -j28 all 
arm-none-eabi-gcc "../rt-thread/src/clock.c"
arm-none-eabi-gcc "../rt-thread/src/components.c"
arm-none-eabi-gcc "../rt-thread/src/cpu.c"
arm-none-eabi-gcc "../rt-thread/src/idle.c"
arm-none-eabi-gcc "../rt-thread/src/ipc.c"
arm-none-eabi-gcc "../rt-thread/src/irq.c"
arm-none-eabi-gcc "../rt-thread/src/kservice.c"
arm-none-eabi-gcc "../rt-thread/src/object.c"
arm-none-eabi-gcc "../rt-thread/src/scheduler.c"
arm-none-eabi-gcc "../rt-thread/src/thread.c"
arm-none-eabi-gcc "../rt-thread/src/timer.c"
arm-none-eabi-gcc "../rt-thread/libcpu/arm/cortex-m3/context_gcc.S"
arm-none-eabi-gcc "../rt-thread/libcpu/arm/cortex-m3/cpuport.c"
arm-none-eabi-gcc "../rt-thread/libcpu/arm/common/backtrace.c"
arm-none-eabi-gcc "../rt-thread/libcpu/arm/common/div0.c"
arm-none-eabi-gcc "../rt-thread/libcpu/arm/common/showmem.c"
arm-none-eabi-gcc "../rt-thread/components/finsh/cmd.c"
arm-none-eabi-gcc "../rt-thread/components/finsh/msh.c"
arm-none-eabi-gcc "../rt-thread/components/finsh/shell.c"
arm-none-eabi-gcc "../libraries/STM32F1xx_HAL_Driver/Src/Legacy/stm32f1xx_hal_can.c"
arm-none-eabi-gcc "../libraries/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal.c"
arm-none-eabi-gcc "../libraries/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_adc.c"
arm-none-eabi-gcc "../libraries/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_adc_ex.c"
arm-none-eabi-gcc "../libraries/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_can.c"
arm-none-eabi-gcc "../libraries/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_cec.c"
arm-none-eabi-gcc "../libraries/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_cortex.c"
arm-none-eabi-gcc "../libraries/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_crc.c"
arm-none-eabi-gcc "../libraries/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_dac.c"
arm-none-eabi-gcc "../libraries/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_dac_ex.c"
arm-none-eabi-gcc "../libraries/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_dma.c"
arm-none-eabi-gcc "../libraries/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_eth.c"
arm-none-eabi-gcc "../libraries/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_exti.c"
arm-none-eabi-gcc "../libraries/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash.c"
arm-none-eabi-gcc "../libraries/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_flash_ex.c"
arm-none-eabi-gcc "../libraries/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio.c"
arm-none-eabi-gcc "../libraries/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio_ex.c"
arm-none-eabi-gcc "../libraries/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_hcd.c"
arm-none-eabi-gcc "../libraries/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_i2c.c"
arm-none-eabi-gcc "../libraries/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_i2s.c"
arm-none-eabi-gcc "../libraries/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_irda.c"
arm-none-eabi-gcc "../libraries/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_iwdg.c"
arm-none-eabi-gcc "../libraries/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_mmc.c"
arm-none-eabi-gcc "../libraries/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_nand.c"
arm-none-eabi-gcc "../libraries/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_nor.c"
arm-none-eabi-gcc "../libraries/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_pccard.c"
arm-none-eabi-gcc "../libraries/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_pcd.c"
arm-none-eabi-gcc "../libraries/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_pcd_ex.c"
arm-none-eabi-gcc "../libraries/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_pwr.c"
arm-none-eabi-gcc "../libraries/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc.c"
arm-none-eabi-gcc "../libraries/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rcc_ex.c"
arm-none-eabi-gcc "../libraries/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rtc.c"
arm-none-eabi-gcc "../libraries/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_rtc_ex.c"
arm-none-eabi-gcc "../libraries/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_sd.c"
arm-none-eabi-gcc "../libraries/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_smartcard.c"
arm-none-eabi-gcc "../libraries/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_spi.c"
arm-none-eabi-gcc "../libraries/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_sram.c"
arm-none-eabi-gcc "../libraries/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_tim.c"
arm-none-eabi-gcc "../libraries/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_tim_ex.c"
arm-none-eabi-gcc "../libraries/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_uart.c"
arm-none-eabi-gcc "../libraries/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_usart.c"
arm-none-eabi-gcc "../libraries/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_wwdg.c"
arm-none-eabi-gcc "../libraries/STM32F1xx_HAL_Driver/Src/stm32f1xx_ll_usb.c"
arm-none-eabi-gcc "../libraries/CMSIS/Device/ST/STM32F1xx/Source/Templates/gcc/startup_stm32f103xb.S"
arm-none-eabi-gcc "../libraries/CMSIS/Device/ST/STM32F1xx/Source/Templates/system_stm32f1xx.c"
arm-none-eabi-gcc "../drivers/board.c"
arm-none-eabi-gcc "../drivers/drv_clk.c"
arm-none-eabi-gcc "../drivers/drv_common.c"
arm-none-eabi-gcc "../drivers/drv_usart.c"
arm-none-eabi-gcc "../applications/app_alarm.c"
arm-none-eabi-gcc "../applications/app_control.c"
arm-none-eabi-gcc "../applications/app_data.c"
arm-none-eabi-gcc "../applications/app_display.c"
arm-none-eabi-gcc "../applications/app_feed.c"
arm-none-eabi-gcc "../applications/app_key.c"
arm-none-eabi-gcc "../applications/app_sensor.c"
arm-none-eabi-gcc "../applications/app_wifi.c"
arm-none-eabi-gcc "../applications/drv_oled.c"
arm-none-eabi-gcc "../applications/main.c"
linking...
d:/rt-threadstudio/repo/extract/toolchain_support_packages/arm/gnu_tools_for_arm_embedded_processors/10.2.1/bin/../lib/gcc/arm-none-eabi/10.2.1/../../../../arm-none-eabi/bin/ld.exe: rtthread.elf section `.text' will not fit in region `ROM'
d:/rt-threadstudio/repo/extract/toolchain_support_packages/arm/gnu_tools_for_arm_embedded_processors/10.2.1/bin/../lib/gcc/arm-none-eabi/10.2.1/../../../../arm-none-eabi/bin/ld.exe: region `ROM' overflowed by 4128 bytes
d:/rt-threadstudio/repo/extract/toolchain_support_packages/arm/gnu_tools_for_arm_embedded_processors/10.2.1/bin/../lib/gcc/arm-none-eabi/10.2.1/../../../../arm-none-eabi/bin/ld.exe: ./applications/app_alarm.o: in function `app_alarm_init':
I:\Outsource_Design_Workspace\RT-Thread-Studio_Workspace\Floder-005-Intellgence_finsh_tank\OutSource-005-Intellgence_finsh_tank\Debug/../applications/app_alarm.c:69: undefined reference to `rt_thread_create'
d:/rt-threadstudio/repo/extract/toolchain_support_packages/arm/gnu_tools_for_arm_embedded_processors/10.2.1/bin/../lib/gcc/arm-none-eabi/10.2.1/../../../../arm-none-eabi/bin/ld.exe: ./applications/app_control.o: in function `app_control_init':
I:\Outsource_Design_Workspace\RT-Thread-Studio_Workspace\Floder-005-Intellgence_finsh_tank\OutSource-005-Intellgence_finsh_tank\Debug/../applications/app_control.c:92: undefined reference to `rt_thread_create'
d:/rt-threadstudio/repo/extract/toolchain_support_packages/arm/gnu_tools_for_arm_embedded_processors/10.2.1/bin/../lib/gcc/arm-none-eabi/10.2.1/../../../../arm-none-eabi/bin/ld.exe: ./applications/app_display.o: in function `app_display_init':
I:\Outsource_Design_Workspace\RT-Thread-Studio_Workspace\Floder-005-Intellgence_finsh_tank\OutSource-005-Intellgence_finsh_tank\Debug/../applications/app_display.c:51: undefined reference to `rt_thread_create'
d:/rt-threadstudio/repo/extract/toolchain_support_packages/arm/gnu_tools_for_arm_embedded_processors/10.2.1/bin/../lib/gcc/arm-none-eabi/10.2.1/../../../../arm-none-eabi/bin/ld.exe: ./applications/app_feed.o: in function `servo_init':
I:\Outsource_Design_Workspace\RT-Thread-Studio_Workspace\Floder-005-Intellgence_finsh_tank\OutSource-005-Intellgence_finsh_tank\Debug/../applications/app_feed.c:25: undefined reference to `HAL_TIM_PWM_Init'
d:/rt-threadstudio/repo/extract/toolchain_support_packages/arm/gnu_tools_for_arm_embedded_processors/10.2.1/bin/../lib/gcc/arm-none-eabi/10.2.1/../../../../arm-none-eabi/bin/ld.exe: I:\Outsource_Design_Workspace\RT-Thread-Studio_Workspace\Floder-005-Intellgence_finsh_tank\OutSource-005-Intellgence_finsh_tank\Debug/../applications/app_feed.c:32: undefined reference to `HAL_TIM_PWM_ConfigChannel'
d:/rt-threadstudio/repo/extract/toolchain_support_packages/arm/gnu_tools_for_arm_embedded_processors/10.2.1/bin/../lib/gcc/arm-none-eabi/10.2.1/../../../../arm-none-eabi/bin/ld.exe: I:\Outsource_Design_Workspace\RT-Thread-Studio_Workspace\Floder-005-Intellgence_finsh_tank\OutSource-005-Intellgence_finsh_tank\Debug/../applications/app_feed.c:33: undefined reference to `HAL_TIM_PWM_Stop'
d:/rt-threadstudio/repo/extract/toolchain_support_packages/arm/gnu_tools_for_arm_embedded_processors/10.2.1/bin/../lib/gcc/arm-none-eabi/10.2.1/../../../../arm-none-eabi/bin/ld.exe: ./applications/app_feed.o: in function `servo_open':
I:\Outsource_Design_Workspace\RT-Thread-Studio_Workspace\Floder-005-Intellgence_finsh_tank\OutSource-005-Intellgence_finsh_tank\Debug/../applications/app_feed.c:39: undefined reference to `HAL_TIM_PWM_Start'
d:/rt-threadstudio/repo/extract/toolchain_support_packages/arm/gnu_tools_for_arm_embedded_processors/10.2.1/bin/../lib/gcc/arm-none-eabi/10.2.1/../../../../arm-none-eabi/bin/ld.exe: ./applications/app_feed.o: in function `servo_close':
I:\Outsource_Design_Workspace\RT-Thread-Studio_Workspace\Floder-005-Intellgence_finsh_tank\OutSource-005-Intellgence_finsh_tank\Debug/../applications/app_feed.c:44: undefined reference to `HAL_TIM_PWM_Stop'
d:/rt-threadstudio/repo/extract/toolchain_support_packages/arm/gnu_tools_for_arm_embedded_processors/10.2.1/bin/../lib/gcc/arm-none-eabi/10.2.1/../../../../arm-none-eabi/bin/ld.exe: ./applications/app_feed.o: in function `app_feed_init':
I:\Outsource_Design_Workspace\RT-Thread-Studio_Workspace\Floder-005-Intellgence_finsh_tank\OutSource-005-Intellgence_finsh_tank\Debug/../applications/app_feed.c:90: undefined reference to `rt_thread_create'
d:/rt-threadstudio/repo/extract/toolchain_support_packages/arm/gnu_tools_for_arm_embedded_processors/10.2.1/bin/../lib/gcc/arm-none-eabi/10.2.1/../../../../arm-none-eabi/bin/ld.exe: ./applications/app_key.o: in function `app_key_init':
I:\Outsource_Design_Workspace\RT-Thread-Studio_Workspace\Floder-005-Intellgence_finsh_tank\OutSource-005-Intellgence_finsh_tank\Debug/../applications/app_key.c:62: undefined reference to `rt_thread_create'
d:/rt-threadstudio/repo/extract/toolchain_support_packages/arm/gnu_tools_for_arm_embedded_processors/10.2.1/bin/../lib/gcc/arm-none-eabi/10.2.1/../../../../arm-none-eabi/bin/ld.exe: ./applications/app_sensor.o: in function `adc_init':
I:\Outsource_Design_Workspace\RT-Thread-Studio_Workspace\Floder-005-Intellgence_finsh_tank\OutSource-005-Intellgence_finsh_tank\Debug/../applications/app_sensor.c:123: undefined reference to `HAL_ADC_Init'
d:/rt-threadstudio/repo/extract/toolchain_support_packages/arm/gnu_tools_for_arm_embedded_processors/10.2.1/bin/../lib/gcc/arm-none-eabi/10.2.1/../../../../arm-none-eabi/bin/ld.exe: ./applications/app_sensor.o: in function `adc_read_channel':
I:\Outsource_Design_Workspace\RT-Thread-Studio_Workspace\Floder-005-Intellgence_finsh_tank\OutSource-005-Intellgence_finsh_tank\Debug/../applications/app_sensor.c:132: undefined reference to `HAL_ADC_ConfigChannel'
d:/rt-threadstudio/repo/extract/toolchain_support_packages/arm/gnu_tools_for_arm_embedded_processors/10.2.1/bin/../lib/gcc/arm-none-eabi/10.2.1/../../../../arm-none-eabi/bin/ld.exe: I:\Outsource_Design_Workspace\RT-Thread-Studio_Workspace\Floder-005-Intellgence_finsh_tank\OutSource-005-Intellgence_finsh_tank\Debug/../applications/app_sensor.c:133: undefined reference to `HAL_ADC_Start'
d:/rt-threadstudio/repo/extract/toolchain_support_packages/arm/gnu_tools_for_arm_embedded_processors/10.2.1/bin/../lib/gcc/arm-none-eabi/10.2.1/../../../../arm-none-eabi/bin/ld.exe: I:\Outsource_Design_Workspace\RT-Thread-Studio_Workspace\Floder-005-Intellgence_finsh_tank\OutSource-005-Intellgence_finsh_tank\Debug/../applications/app_sensor.c:134: undefined reference to `HAL_ADC_PollForConversion'
d:/rt-threadstudio/repo/extract/toolchain_support_packages/arm/gnu_tools_for_arm_embedded_processors/10.2.1/bin/../lib/gcc/arm-none-eabi/10.2.1/../../../../arm-none-eabi/bin/ld.exe: I:\Outsource_Design_Workspace\RT-Thread-Studio_Workspace\Floder-005-Intellgence_finsh_tank\OutSource-005-Intellgence_finsh_tank\Debug/../applications/app_sensor.c:135: undefined reference to `HAL_ADC_GetValue'
d:/rt-threadstudio/repo/extract/toolchain_support_packages/arm/gnu_tools_for_arm_embedded_processors/10.2.1/bin/../lib/gcc/arm-none-eabi/10.2.1/../../../../arm-none-eabi/bin/ld.exe: ./applications/app_sensor.o: in function `app_sensor_init':
I:\Outsource_Design_Workspace\RT-Thread-Studio_Workspace\Floder-005-Intellgence_finsh_tank\OutSource-005-Intellgence_finsh_tank\Debug/../applications/app_sensor.c:166: undefined reference to `rt_thread_create'
d:/rt-threadstudio/repo/extract/toolchain_support_packages/arm/gnu_tools_for_arm_embedded_processors/10.2.1/bin/../lib/gcc/arm-none-eabi/10.2.1/../../../../arm-none-eabi/bin/ld.exe: ./applications/app_wifi.o: in function `app_wifi_init':
I:\Outsource_Design_Workspace\RT-Thread-Studio_Workspace\Floder-005-Intellgence_finsh_tank\OutSource-005-Intellgence_finsh_tank\Debug/../applications/app_wifi.c:119: undefined reference to `rt_thread_create'
d:/rt-threadstudio/repo/extract/toolchain_support_packages/arm/gnu_tools_for_arm_embedded_processors/10.2.1/bin/../lib/gcc/arm-none-eabi/10.2.1/../../../../arm-none-eabi/bin/ld.exe: d:/rt-threadstudio/repo/extract/toolchain_support_packages/arm/gnu_tools_for_arm_embedded_processors/10.2.1/bin/../lib/gcc/arm-none-eabi/10.2.1/../../../../arm-none-eabi/lib/thumb/v7-m/nofp\libg.a(lib_a-sbrkr.o): in function `_sbrk_r':
sbrkr.c:(.text._sbrk_r+0xc): undefined reference to `_sbrk'
d:/rt-threadstudio/repo/extract/toolchain_support_packages/arm/gnu_tools_for_arm_embedded_processors/10.2.1/bin/../lib/gcc/arm-none-eabi/10.2.1/../../../../arm-none-eabi/bin/ld.exe: d:/rt-threadstudio/repo/extract/toolchain_support_packages/arm/gnu_tools_for_arm_embedded_processors/10.2.1/bin/../lib/gcc/arm-none-eabi/10.2.1/../../../../arm-none-eabi/lib/thumb/v7-m/nofp\libg.a(lib_a-abort.o): in function `abort':
abort.c:(.text.abort+0xa): undefined reference to `_exit'
d:/rt-threadstudio/repo/extract/toolchain_support_packages/arm/gnu_tools_for_arm_embedded_processors/10.2.1/bin/../lib/gcc/arm-none-eabi/10.2.1/../../../../arm-none-eabi/bin/ld.exe: d:/rt-threadstudio/repo/extract/toolchain_support_packages/arm/gnu_tools_for_arm_embedded_processors/10.2.1/bin/../lib/gcc/arm-none-eabi/10.2.1/../../../../arm-none-eabi/lib/thumb/v7-m/nofp\libg.a(lib_a-signalr.o): in function `_kill_r':
signalr.c:(.text._kill_r+0xe): undefined reference to `_kill'
d:/rt-threadstudio/repo/extract/toolchain_support_packages/arm/gnu_tools_for_arm_embedded_processors/10.2.1/bin/../lib/gcc/arm-none-eabi/10.2.1/../../../../arm-none-eabi/bin/ld.exe: d:/rt-threadstudio/repo/extract/toolchain_support_packages/arm/gnu_tools_for_arm_embedded_processors/10.2.1/bin/../lib/gcc/arm-none-eabi/10.2.1/../../../../arm-none-eabi/lib/thumb/v7-m/nofp\libg.a(lib_a-signalr.o): in function `_getpid_r':
signalr.c:(.text._getpid_r+0x0): undefined reference to `_getpid'
d:/rt-threadstudio/repo/extract/toolchain_support_packages/arm/gnu_tools_for_arm_embedded_processors/10.2.1/bin/../lib/gcc/arm-none-eabi/10.2.1/../../../../arm-none-eabi/bin/ld.exe: d:/rt-threadstudio/repo/extract/toolchain_support_packages/arm/gnu_tools_for_arm_embedded_processors/10.2.1/bin/../lib/gcc/arm-none-eabi/10.2.1/../../../../arm-none-eabi/lib/thumb/v7-m/nofp\libg.a(lib_a-writer.o): in function `_write_r':
writer.c:(.text._write_r+0x14): undefined reference to `_write'
d:/rt-threadstudio/repo/extract/toolchain_support_packages/arm/gnu_tools_for_arm_embedded_processors/10.2.1/bin/../lib/gcc/arm-none-eabi/10.2.1/../../../../arm-none-eabi/bin/ld.exe: d:/rt-threadstudio/repo/extract/toolchain_support_packages/arm/gnu_tools_for_arm_embedded_processors/10.2.1/bin/../lib/gcc/arm-none-eabi/10.2.1/../../../../arm-none-eabi/lib/thumb/v7-m/nofp\libg.a(lib_a-closer.o): in function `_close_r':
closer.c:(.text._close_r+0xc): undefined reference to `_close'
d:/rt-threadstudio/repo/extract/toolchain_support_packages/arm/gnu_tools_for_arm_embedded_processors/10.2.1/bin/../lib/gcc/arm-none-eabi/10.2.1/../../../../arm-none-eabi/bin/ld.exe: d:/rt-threadstudio/repo/extract/toolchain_support_packages/arm/gnu_tools_for_arm_embedded_processors/10.2.1/bin/../lib/gcc/arm-none-eabi/10.2.1/../../../../arm-none-eabi/lib/thumb/v7-m/nofp\libg.a(lib_a-fstatr.o): in function `_fstat_r':
fstatr.c:(.text._fstat_r+0xe): undefined reference to `_fstat'
d:/rt-threadstudio/repo/extract/toolchain_support_packages/arm/gnu_tools_for_arm_embedded_processors/10.2.1/bin/../lib/gcc/arm-none-eabi/10.2.1/../../../../arm-none-eabi/bin/ld.exe: d:/rt-threadstudio/repo/extract/toolchain_support_packages/arm/gnu_tools_for_arm_embedded_processors/10.2.1/bin/../lib/gcc/arm-none-eabi/10.2.1/../../../../arm-none-eabi/lib/thumb/v7-m/nofp\libg.a(lib_a-isattyr.o): in function `_isatty_r':
isattyr.c:(.text._isatty_r+0xc): undefined reference to `_isatty'
d:/rt-threadstudio/repo/extract/toolchain_support_packages/arm/gnu_tools_for_arm_embedded_processors/10.2.1/bin/../lib/gcc/arm-none-eabi/10.2.1/../../../../arm-none-eabi/bin/ld.exe: d:/rt-threadstudio/repo/extract/toolchain_support_packages/arm/gnu_tools_for_arm_embedded_processors/10.2.1/bin/../lib/gcc/arm-none-eabi/10.2.1/../../../../arm-none-eabi/lib/thumb/v7-m/nofp\libg.a(lib_a-lseekr.o): in function `_lseek_r':
lseekr.c:(.text._lseek_r+0x14): undefined reference to `_lseek'
d:/rt-threadstudio/repo/extract/toolchain_support_packages/arm/gnu_tools_for_arm_embedded_processors/10.2.1/bin/../lib/gcc/arm-none-eabi/10.2.1/../../../../arm-none-eabi/bin/ld.exe: d:/rt-threadstudio/repo/extract/toolchain_support_packages/arm/gnu_tools_for_arm_embedded_processors/10.2.1/bin/../lib/gcc/arm-none-eabi/10.2.1/../../../../arm-none-eabi/lib/thumb/v7-m/nofp\libg.a(lib_a-readr.o): in function `_read_r':
readr.c:(.text._read_r+0x14): undefined reference to `_read'
collect2.exe: error: ld returned 1 exit status
make: *** [makefile:54: rtthread.elf] Error 1
Console-Error: 请启用libc组件
"make -j28 all" terminated with exit code 2. Build might be incomplete.

21:32:28 Build Failed. 43 errors, 0 warnings. (took 1s.916ms)

