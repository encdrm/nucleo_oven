################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Libraries/Src/Graph.c \
../Libraries/Src/OLED.c \
../Libraries/Src/Switch.c \
../Libraries/Src/control.c \
../Libraries/Src/heater.c \
../Libraries/Src/max6675.c 

OBJS += \
./Libraries/Src/Graph.o \
./Libraries/Src/OLED.o \
./Libraries/Src/Switch.o \
./Libraries/Src/control.o \
./Libraries/Src/heater.o \
./Libraries/Src/max6675.o 

C_DEPS += \
./Libraries/Src/Graph.d \
./Libraries/Src/OLED.d \
./Libraries/Src/Switch.d \
./Libraries/Src/control.d \
./Libraries/Src/heater.d \
./Libraries/Src/max6675.d 


# Each subdirectory must supply rules for building sources it contributes
Libraries/Src/%.o Libraries/Src/%.su: ../Libraries/Src/%.c Libraries/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/user01/OneDrive - UOS/Work/Zetin/Workspace_CubeIDE/Nucleo_Oven/Libraries/Inc" -I"C:/Users/user01/OneDrive - UOS/Work/Zetin/Workspace_CubeIDE/Nucleo_Oven/Main/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Libraries-2f-Src

clean-Libraries-2f-Src:
	-$(RM) ./Libraries/Src/Graph.d ./Libraries/Src/Graph.o ./Libraries/Src/Graph.su ./Libraries/Src/OLED.d ./Libraries/Src/OLED.o ./Libraries/Src/OLED.su ./Libraries/Src/Switch.d ./Libraries/Src/Switch.o ./Libraries/Src/Switch.su ./Libraries/Src/control.d ./Libraries/Src/control.o ./Libraries/Src/control.su ./Libraries/Src/heater.d ./Libraries/Src/heater.o ./Libraries/Src/heater.su ./Libraries/Src/max6675.d ./Libraries/Src/max6675.o ./Libraries/Src/max6675.su

.PHONY: clean-Libraries-2f-Src

