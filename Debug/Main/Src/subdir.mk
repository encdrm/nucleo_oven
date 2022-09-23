################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Main/Src/menu.c 

OBJS += \
./Main/Src/menu.o 

C_DEPS += \
./Main/Src/menu.d 


# Each subdirectory must supply rules for building sources it contributes
Main/Src/%.o: ../Main/Src/%.c Main/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/phc72/STM32CubeIDE/Nucleo_Oven/Libraries/Inc" -I"C:/Users/phc72/STM32CubeIDE/Nucleo_Oven/Main/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Main-2f-Src

clean-Main-2f-Src:
	-$(RM) ./Main/Src/menu.d ./Main/Src/menu.o

.PHONY: clean-Main-2f-Src

