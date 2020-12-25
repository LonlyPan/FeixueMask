################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include User/subdir.mk
-include GUI/lvgl_driver/subdir.mk
-include GUI/lvgl/tests/lv_test_widgets/subdir.mk
-include GUI/lvgl/tests/lv_test_fonts/subdir.mk
-include GUI/lvgl/tests/lv_test_core/subdir.mk
-include GUI/lvgl/tests/subdir.mk
-include GUI/lvgl/src/lv_widgets/subdir.mk
-include GUI/lvgl/src/lv_themes/subdir.mk
-include GUI/lvgl/src/lv_misc/subdir.mk
-include GUI/lvgl/src/lv_hal/subdir.mk
-include GUI/lvgl/src/lv_gpu/subdir.mk
-include GUI/lvgl/src/lv_font/subdir.mk
-include GUI/lvgl/src/lv_draw/subdir.mk
-include GUI/lvgl/src/lv_core/subdir.mk
-include GUI/lvgl/examples/porting/subdir.mk
-include Drivers/STM32F4xx_HAL_Driver/Src/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

BUILD_ARTIFACT_NAME := feixue-V1.0.0
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX := 
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME).$(BUILD_ARTIFACT_EXTENSION)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
feixue-V1.0.0.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
feixue-V1.0.0.list \

OBJCOPY_BIN += \
feixue-V1.0.0.bin \


# All Target
all: main-build

# Main-build Target
main-build: feixue-V1.0.0.elf secondary-outputs

# Tool invocations
feixue-V1.0.0.elf: $(OBJS) $(USER_OBJS) C:\Users\23714\Desktop\FeixueMask\01_Firmware\01_source\01_new_source\feixue-V1.0.0\STM32F401CCUX_FLASH.ld
	arm-none-eabi-gcc -o "feixue-V1.0.0.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"C:\Users\23714\Desktop\FeixueMask\01_Firmware\01_source\01_new_source\feixue-V1.0.0\STM32F401CCUX_FLASH.ld" --specs=nosys.specs -Wl,-Map="feixue-V1.0.0.map" -Wl,--gc-sections -static --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

feixue-V1.0.0.list: $(EXECUTABLES)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "feixue-V1.0.0.list"
	@echo 'Finished building: $@'
	@echo ' '

feixue-V1.0.0.bin: $(EXECUTABLES)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "feixue-V1.0.0.bin"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) *
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_BIN)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents fail-specified-linker-script-missing warn-no-linker-script-specified
.SECONDARY:

-include ../makefile.targets
