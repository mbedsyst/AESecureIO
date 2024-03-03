################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/AES.c \
../Src/Banner.c \
../Src/DMA.c \
../Src/Delay.c \
../Src/Operations.c \
../Src/SDIO.c \
../Src/UART.c \
../Src/main.c \
../Src/syscalls.c \
../Src/sysmem.c 

OBJS += \
./Src/AES.o \
./Src/Banner.o \
./Src/DMA.o \
./Src/Delay.o \
./Src/Operations.o \
./Src/SDIO.o \
./Src/UART.o \
./Src/main.o \
./Src/syscalls.o \
./Src/sysmem.o 

C_DEPS += \
./Src/AES.d \
./Src/Banner.d \
./Src/DMA.d \
./Src/Delay.d \
./Src/Operations.d \
./Src/SDIO.d \
./Src/UART.d \
./Src/main.d \
./Src/syscalls.d \
./Src/sysmem.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o Src/%.su Src/%.cyclo: ../Src/%.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DSTM32F401xE -c -I../Inc -I"C:/Users/zalma/OneDrive/Documents/BuildYourOwnRTOS/GCC_STM32CubeIDE/SDIO_AES_Device/Chip_Headers/CMSIS/Device/ST/STM32F4xx/Include" -I"C:/Users/zalma/OneDrive/Documents/BuildYourOwnRTOS/GCC_STM32CubeIDE/SDIO_AES_Device/Chip_Headers/CMSIS/Include" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Src

clean-Src:
	-$(RM) ./Src/AES.cyclo ./Src/AES.d ./Src/AES.o ./Src/AES.su ./Src/Banner.cyclo ./Src/Banner.d ./Src/Banner.o ./Src/Banner.su ./Src/DMA.cyclo ./Src/DMA.d ./Src/DMA.o ./Src/DMA.su ./Src/Delay.cyclo ./Src/Delay.d ./Src/Delay.o ./Src/Delay.su ./Src/Operations.cyclo ./Src/Operations.d ./Src/Operations.o ./Src/Operations.su ./Src/SDIO.cyclo ./Src/SDIO.d ./Src/SDIO.o ./Src/SDIO.su ./Src/UART.cyclo ./Src/UART.d ./Src/UART.o ./Src/UART.su ./Src/main.cyclo ./Src/main.d ./Src/main.o ./Src/main.su ./Src/syscalls.cyclo ./Src/syscalls.d ./Src/syscalls.o ./Src/syscalls.su ./Src/sysmem.cyclo ./Src/sysmem.d ./Src/sysmem.o ./Src/sysmem.su

.PHONY: clean-Src

