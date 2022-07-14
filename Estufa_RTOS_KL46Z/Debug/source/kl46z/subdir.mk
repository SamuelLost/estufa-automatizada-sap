################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/kl46z/adc.c \
../source/kl46z/delay.c \
../source/kl46z/drivers.c \
../source/kl46z/gpio.c \
../source/kl46z/lcd.c \
../source/kl46z/port.c 

OBJS += \
./source/kl46z/adc.o \
./source/kl46z/delay.o \
./source/kl46z/drivers.o \
./source/kl46z/gpio.o \
./source/kl46z/lcd.o \
./source/kl46z/port.o 

C_DEPS += \
./source/kl46z/adc.d \
./source/kl46z/delay.d \
./source/kl46z/drivers.d \
./source/kl46z/gpio.d \
./source/kl46z/lcd.d \
./source/kl46z/port.d 


# Each subdirectory must supply rules for building sources it contributes
source/kl46z/%.o: ../source/kl46z/%.c source/kl46z/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DCPU_MKL46Z256VLL4 -DCPU_MKL46Z256VLL4_cm0plus -DSDK_OS_BAREMETAL -DFSL_RTOS_BM -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -DFSL_RTOS_FREE_RTOS -DSDK_OS_FREE_RTOS -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -DSDK_DEBUGCONSOLE=0 -I"/home/pedrobotelho15/rtos/Estufa_RTOS_KL46Z/board" -I"/home/pedrobotelho15/rtos/Estufa_RTOS_KL46Z/source" -I"/home/pedrobotelho15/rtos/Estufa_RTOS_KL46Z" -I"/home/pedrobotelho15/rtos/Estufa_RTOS_KL46Z/amazon-freertos/FreeRTOS/portable" -I"/home/pedrobotelho15/rtos/Estufa_RTOS_KL46Z/freertos/portable" -I"/home/pedrobotelho15/rtos/Estufa_RTOS_KL46Z/amazon-freertos/include" -I"/home/pedrobotelho15/rtos/Estufa_RTOS_KL46Z/drivers" -I"/home/pedrobotelho15/rtos/Estufa_RTOS_KL46Z/utilities" -I"/home/pedrobotelho15/rtos/Estufa_RTOS_KL46Z/startup" -I"/home/pedrobotelho15/rtos/Estufa_RTOS_KL46Z/CMSIS" -O0 -fno-common -g3 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m0plus -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


