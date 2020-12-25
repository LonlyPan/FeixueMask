################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../GUI/lvgl/examples/porting/lv_port_disp_template.c \
../GUI/lvgl/examples/porting/lv_port_fs_template.c \
../GUI/lvgl/examples/porting/lv_port_indev_template.c 

OBJS += \
./GUI/lvgl/examples/porting/lv_port_disp_template.o \
./GUI/lvgl/examples/porting/lv_port_fs_template.o \
./GUI/lvgl/examples/porting/lv_port_indev_template.o 

C_DEPS += \
./GUI/lvgl/examples/porting/lv_port_disp_template.d \
./GUI/lvgl/examples/porting/lv_port_fs_template.d \
./GUI/lvgl/examples/porting/lv_port_indev_template.d 


# Each subdirectory must supply rules for building sources it contributes
GUI/lvgl/examples/porting/lv_port_disp_template.o: ../GUI/lvgl/examples/porting/lv_port_disp_template.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F401xC -DDEBUG -c -I../Core/Inc -I"C:/Users/23714/Desktop/FeixueMask/01_Firmware/01_source/01_new_source/feixue-V1.0.0/User" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O3 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"GUI/lvgl/examples/porting/lv_port_disp_template.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
GUI/lvgl/examples/porting/lv_port_fs_template.o: ../GUI/lvgl/examples/porting/lv_port_fs_template.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F401xC -DDEBUG -c -I../Core/Inc -I"C:/Users/23714/Desktop/FeixueMask/01_Firmware/01_source/01_new_source/feixue-V1.0.0/User" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O3 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"GUI/lvgl/examples/porting/lv_port_fs_template.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
GUI/lvgl/examples/porting/lv_port_indev_template.o: ../GUI/lvgl/examples/porting/lv_port_indev_template.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DUSE_HAL_DRIVER -DSTM32F401xC -DDEBUG -c -I../Core/Inc -I"C:/Users/23714/Desktop/FeixueMask/01_Firmware/01_source/01_new_source/feixue-V1.0.0/User" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O3 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"GUI/lvgl/examples/porting/lv_port_indev_template.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

