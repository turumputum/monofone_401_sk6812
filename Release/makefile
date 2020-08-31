################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Startup/subdir.mk
-include Src/subdir.mk
-include Middlewares/Third_Party/FatFs/src/option/subdir.mk
-include Middlewares/Third_Party/FatFs/src/subdir.mk
-include Middlewares/ST/STM32_USB_Device_Library/Core/Src/subdir.mk
-include Middlewares/ST/STM32_USB_Device_Library/Class/HID/Src/subdir.mk
-include Middlewares/ST/STM32_USB_Device_Library/Class/AUDIO/Src/subdir.mk
-include Drivers/STM32F4xx_HAL_Driver/Src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
monofon_USB_sk6812.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
monofon_USB_sk6812.list \

OBJCOPY_HEX += \
monofon_USB_sk6812.hex \

OBJCOPY_BIN += \
monofon_USB_sk6812.bin \


# All Target
all: monofon_USB_sk6812.elf secondary-outputs

# Tool invocations
monofon_USB_sk6812.elf: $(OBJS) $(USER_OBJS) C:\Users\turum\GoogleDrive\Monofon\monofone_401_sk6812\STM32F401RETX_FLASH.ld
	arm-none-eabi-gcc -o "monofon_USB_sk6812.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"C:\Users\turum\GoogleDrive\Monofon\monofone_401_sk6812\STM32F401RETX_FLASH.ld" --specs=nosys.specs -Wl,-Map="monofon_USB_sk6812.map" -Wl,--gc-sections -static -u _printf_float --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

monofon_USB_sk6812.list: $(EXECUTABLES)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "monofon_USB_sk6812.list"
	@echo 'Finished building: $@'
	@echo ' '

monofon_USB_sk6812.hex: $(EXECUTABLES)
	arm-none-eabi-objcopy  -O ihex $(EXECUTABLES) "monofon_USB_sk6812.hex"
	@echo 'Finished building: $@'
	@echo ' '

monofon_USB_sk6812.bin: $(EXECUTABLES)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "monofon_USB_sk6812.bin"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) *
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_HEX) $(OBJCOPY_BIN)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
