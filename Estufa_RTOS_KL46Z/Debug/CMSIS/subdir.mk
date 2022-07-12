################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../CMSIS/system_MKL46Z4.c 

OBJS += \
./CMSIS/system_MKL46Z4.o 

C_DEPS += \
./CMSIS/system_MKL46Z4.d 


# Each subdirectory must supply rules for building sources it contributes
CMSIS/%.o: ../CMSIS/%.c CMSIS/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DCPU_MKL46Z256VLL4 -DCPU_MKL46Z256VLL4_cm0plus -DSDK_OS_BAREMETAL -DFSL_RTOS_BM -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -DFSL_RTOS_FREE_RTOS -DSDK_OS_FREE_RTOS -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -DSDK_DEBUGCONSOLE=0 -I"/home/pedrobotelho15/Área de Trabalho/Projetos com K32L2B3/Estufa_RTOS_KL46Z/board" -I"/home/pedrobotelho15/Área de Trabalho/Projetos com K32L2B3/Estufa_RTOS_KL46Z/source" -I"/home/pedrobotelho15/Área de Trabalho/Projetos com K32L2B3/Estufa_RTOS_KL46Z" -I"/home/pedrobotelho15/Área de Trabalho/Projetos com K32L2B3/Estufa_RTOS_KL46Z/amazon-freertos/FreeRTOS/portable" -I"/home/pedrobotelho15/Área de Trabalho/Projetos com K32L2B3/Estufa_RTOS_KL46Z/freertos/portable" -I"/home/pedrobotelho15/Área de Trabalho/Projetos com K32L2B3/Estufa_RTOS_KL46Z/amazon-freertos/include" -I"/home/pedrobotelho15/Área de Trabalho/Projetos com K32L2B3/Estufa_RTOS_KL46Z/drivers" -I"/home/pedrobotelho15/Área de Trabalho/Projetos com K32L2B3/Estufa_RTOS_KL46Z/utilities" -I"/home/pedrobotelho15/Área de Trabalho/Projetos com K32L2B3/Estufa_RTOS_KL46Z/startup" -I"/home/pedrobotelho15/Área de Trabalho/Projetos com K32L2B3/Estufa_RTOS_KL46Z/CMSIS" -O0 -fno-common -g3 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m0plus -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


