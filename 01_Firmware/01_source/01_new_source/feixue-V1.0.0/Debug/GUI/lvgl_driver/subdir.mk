################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../GUI/lvgl_driver/lv_port_disp.c 

OBJS += \
./GUI/lvgl_driver/lv_port_disp.o 

C_DEPS += \
./GUI/lvgl_driver/lv_port_disp.d 


# Each subdirectory must supply rules for building sources it contributes
GUI/lvgl_driver/lv_port_disp.o: ../GUI/lvgl_driver/lv_port_disp.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F401xC -DDEBUG -c -I../Core/Inc -I"C:/Users/23714/Desktop/FeixueMask/01_Firmware/01_source/01_new_source/feixue-V1.0.0/User" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/23714/Desktop/FeixueMask/01_Firmware/01_source/01_new_source/feixue-V1.0.0/GUI/lvgl" -I"C:/Users/23714/Desktop/FeixueMask/01_Firmware/01_source/01_new_source/feixue-V1.0.0/GUI" -I"C:/Users/23714/Desktop/FeixueMask/01_Firmware/01_source/01_new_source/feixue-V1.0.0/GUI/lvgl_driver" -I"C:/Users/23714/Desktop/FeixueMask/01_Firmware/01_source/01_new_source/feixue-V1.0.0/GUI/lvgl/src" -O3 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"GUI/lvgl_driver/lv_port_disp.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

