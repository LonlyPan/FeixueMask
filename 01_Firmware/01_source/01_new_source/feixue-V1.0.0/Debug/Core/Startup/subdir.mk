################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../Core/Startup/startup_stm32f401ccux.s 

OBJS += \
./Core/Startup/startup_stm32f401ccux.o 

S_DEPS += \
./Core/Startup/startup_stm32f401ccux.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Startup/startup_stm32f401ccux.o: ../Core/Startup/startup_stm32f401ccux.s
	arm-none-eabi-gcc -mcpu=cortex-m4 -g3 -c -I"C:/Users/23714/Desktop/FeixueMask/01_Firmware/01_source/01_new_source/feixue-V1.0.0/User" -x assembler-with-cpp -MMD -MP -MF"Core/Startup/startup_stm32f401ccux.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@" "$<"

