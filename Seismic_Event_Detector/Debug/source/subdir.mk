################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/main.c \
../source/semihost_hardfault.c 

S_SRCS += \
../source/leds.s 

C_DEPS += \
./source/main.d \
./source/semihost_hardfault.d 

OBJS += \
./source/leds.o \
./source/main.o \
./source/semihost_hardfault.o 


# Each subdirectory must supply rules for building sources it contributes
source/%.o: ../source/%.s source/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU Assembler'
	arm-none-eabi-gcc -c -x assembler-with-cpp -D__REDLIB__ -I"/Users/saminashraf/Documents/MCUXpressoIDE_25.6.136/workspace_project/Seismic_Event_Detector/board" -I"/Users/saminashraf/Documents/MCUXpressoIDE_25.6.136/workspace_project/Seismic_Event_Detector/source" -g3 -gdwarf-4 -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -specs=redlib.specs -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

source/%.o: ../source/%.c source/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -D__REDLIB__ -DCPU_MK64FN1M0VLL12 -DCPU_MK64FN1M0VLL12_cm4 -DSDK_OS_BAREMETAL -DSDK_DEBUGCONSOLE=1 -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -DSERIAL_PORT_TYPE_UART=1 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -I"/Users/saminashraf/Documents/MCUXpressoIDE_25.6.136/workspace_project/Seismic_Event_Detector/board" -I"/Users/saminashraf/Documents/MCUXpressoIDE_25.6.136/workspace_project/Seismic_Event_Detector/source" -I"/Users/saminashraf/Documents/MCUXpressoIDE_25.6.136/workspace_project/Seismic_Event_Detector/utilities" -I"/Users/saminashraf/Documents/MCUXpressoIDE_25.6.136/workspace_project/Seismic_Event_Detector/drivers" -I"/Users/saminashraf/Documents/MCUXpressoIDE_25.6.136/workspace_project/Seismic_Event_Detector/device" -I"/Users/saminashraf/Documents/MCUXpressoIDE_25.6.136/workspace_project/Seismic_Event_Detector/component/serial_manager" -I"/Users/saminashraf/Documents/MCUXpressoIDE_25.6.136/workspace_project/Seismic_Event_Detector/component/lists" -I"/Users/saminashraf/Documents/MCUXpressoIDE_25.6.136/workspace_project/Seismic_Event_Detector/CMSIS" -I"/Users/saminashraf/Documents/MCUXpressoIDE_25.6.136/workspace_project/Seismic_Event_Detector/component/uart" -O0 -fno-common -g3 -gdwarf-4 -Wall -c -ffunction-sections -fdata-sections -fno-builtin -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-source

clean-source:
	-$(RM) ./source/leds.o ./source/main.d ./source/main.o ./source/semihost_hardfault.d ./source/semihost_hardfault.o

.PHONY: clean-source

