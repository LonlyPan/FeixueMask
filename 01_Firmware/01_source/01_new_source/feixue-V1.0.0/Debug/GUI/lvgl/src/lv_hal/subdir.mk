################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../GUI/lvgl/src/lv_hal/lv_hal_disp.c \
../GUI/lvgl/src/lv_hal/lv_hal_indev.c \
../GUI/lvgl/src/lv_hal/lv_hal_tick.c 

OBJS += \
./GUI/lvgl/src/lv_hal/lv_hal_disp.o \
./GUI/lvgl/src/lv_hal/lv_hal_indev.o \
./GUI/lvgl/src/lv_hal/lv_hal_tick.o 

C_DEPS += \
./GUI/lvgl/src/lv_hal/lv_hal_disp.d \
./GUI/lvgl/src/lv_hal/lv_hal_indev.d \
./GUI/lvgl/src/lv_hal/lv_hal_tick.d 


# Each subdirectory must supply rules for building sources it contributes
GUI/lvgl/src/lv_hal/lv_hal_disp.o: ../GUI/lvgl/src/lv_hal/lv_hal_disp.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F401xC -DDEBUG -c -I../Core/Inc -I"C:/Users/23714/Desktop/FeixueMask/01_Firmware/01_source/01_new_source/feixue-V1.0.0/User" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O3 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"GUI/lvgl/src/lv_hal/lv_hal_disp.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
GUI/lvgl/src/lv_hal/lv_hal_indev.o: ../GUI/lvgl/src/lv_hal/lv_hal_indev.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F401xC -DDEBUG -c -I../Core/Inc -I"C:/Users/23714/Desktop/FeixueMask/01_Firmware/01_source/01_new_source/feixue-V1.0.0/User" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O3 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"GUI/lvgl/src/lv_hal/lv_hal_indev.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
GUI/lvgl/src/lv_hal/lv_hal_tick.o: ../GUI/lvgl/src/lv_hal/lv_hal_tick.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F401xC -DDEBUG -c -I../Core/Inc -I"C:/Users/23714/Desktop/FeixueMask/01_Firmware/01_source/01_new_source/feixue-V1.0.0/User" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O3 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"GUI/lvgl/src/lv_hal/lv_hal_tick.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

