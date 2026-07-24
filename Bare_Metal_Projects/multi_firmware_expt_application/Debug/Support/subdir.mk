################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
D:/Users/soumy/GitHub/Learn_embedded_software_dev/EmbeddedProjects/Bare_Metal_Projects/multi_firmware_expt_type_1/Support/led.c \
D:/Users/soumy/GitHub/Learn_embedded_software_dev/EmbeddedProjects/Bare_Metal_Projects/multi_firmware_expt_type_1/Support/queue.c \
D:/Users/soumy/GitHub/Learn_embedded_software_dev/EmbeddedProjects/Bare_Metal_Projects/multi_firmware_expt_type_1/Support/timebase.c \
D:/Users/soumy/GitHub/Learn_embedded_software_dev/EmbeddedProjects/Bare_Metal_Projects/multi_firmware_expt_type_1/Support/uart.c 

OBJS += \
./Support/led.o \
./Support/queue.o \
./Support/timebase.o \
./Support/uart.o 

C_DEPS += \
./Support/led.d \
./Support/queue.d \
./Support/timebase.d \
./Support/uart.d 


# Each subdirectory must supply rules for building sources it contributes
Support/led.o: D:/Users/soumy/GitHub/Learn_embedded_software_dev/EmbeddedProjects/Bare_Metal_Projects/multi_firmware_expt_type_1/Support/led.c Support/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DNUCLEO_F401RE -DSTM32 -DSTM32F401RETx -DSTM32F4 -DSTM32F407xx -c -I../Inc -I"D:/Users/soumy/GitHub/Learn_embedded_software_dev/EmbeddedProjects/Bare_Metal_Projects/multi_firmware_expt_type_1/Support" -I"D:/Users/soumy/GitHub/Learn_embedded_software_dev/EmbeddedProjects/Bare_Metal_Projects/multi_firmware_expt_application/chip_headers/CMSIS/Core/Include" -I"D:/Users/soumy/GitHub/Learn_embedded_software_dev/EmbeddedProjects/Bare_Metal_Projects/multi_firmware_expt_application/chip_headers/CMSIS/Device/ST/STM32F4xx/Include" -O0 -ffunction-sections -fdata-sections -Wall -Wextra -pedantic -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Support/queue.o: D:/Users/soumy/GitHub/Learn_embedded_software_dev/EmbeddedProjects/Bare_Metal_Projects/multi_firmware_expt_type_1/Support/queue.c Support/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DNUCLEO_F401RE -DSTM32 -DSTM32F401RETx -DSTM32F4 -DSTM32F407xx -c -I../Inc -I"D:/Users/soumy/GitHub/Learn_embedded_software_dev/EmbeddedProjects/Bare_Metal_Projects/multi_firmware_expt_type_1/Support" -I"D:/Users/soumy/GitHub/Learn_embedded_software_dev/EmbeddedProjects/Bare_Metal_Projects/multi_firmware_expt_application/chip_headers/CMSIS/Core/Include" -I"D:/Users/soumy/GitHub/Learn_embedded_software_dev/EmbeddedProjects/Bare_Metal_Projects/multi_firmware_expt_application/chip_headers/CMSIS/Device/ST/STM32F4xx/Include" -O0 -ffunction-sections -fdata-sections -Wall -Wextra -pedantic -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Support/timebase.o: D:/Users/soumy/GitHub/Learn_embedded_software_dev/EmbeddedProjects/Bare_Metal_Projects/multi_firmware_expt_type_1/Support/timebase.c Support/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DNUCLEO_F401RE -DSTM32 -DSTM32F401RETx -DSTM32F4 -DSTM32F407xx -c -I../Inc -I"D:/Users/soumy/GitHub/Learn_embedded_software_dev/EmbeddedProjects/Bare_Metal_Projects/multi_firmware_expt_type_1/Support" -I"D:/Users/soumy/GitHub/Learn_embedded_software_dev/EmbeddedProjects/Bare_Metal_Projects/multi_firmware_expt_application/chip_headers/CMSIS/Core/Include" -I"D:/Users/soumy/GitHub/Learn_embedded_software_dev/EmbeddedProjects/Bare_Metal_Projects/multi_firmware_expt_application/chip_headers/CMSIS/Device/ST/STM32F4xx/Include" -O0 -ffunction-sections -fdata-sections -Wall -Wextra -pedantic -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Support/uart.o: D:/Users/soumy/GitHub/Learn_embedded_software_dev/EmbeddedProjects/Bare_Metal_Projects/multi_firmware_expt_type_1/Support/uart.c Support/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DNUCLEO_F401RE -DSTM32 -DSTM32F401RETx -DSTM32F4 -DSTM32F407xx -c -I../Inc -I"D:/Users/soumy/GitHub/Learn_embedded_software_dev/EmbeddedProjects/Bare_Metal_Projects/multi_firmware_expt_type_1/Support" -I"D:/Users/soumy/GitHub/Learn_embedded_software_dev/EmbeddedProjects/Bare_Metal_Projects/multi_firmware_expt_application/chip_headers/CMSIS/Core/Include" -I"D:/Users/soumy/GitHub/Learn_embedded_software_dev/EmbeddedProjects/Bare_Metal_Projects/multi_firmware_expt_application/chip_headers/CMSIS/Device/ST/STM32F4xx/Include" -O0 -ffunction-sections -fdata-sections -Wall -Wextra -pedantic -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Support

clean-Support:
	-$(RM) ./Support/led.cyclo ./Support/led.d ./Support/led.o ./Support/led.su ./Support/queue.cyclo ./Support/queue.d ./Support/queue.o ./Support/queue.su ./Support/timebase.cyclo ./Support/timebase.d ./Support/timebase.o ./Support/timebase.su ./Support/uart.cyclo ./Support/uart.d ./Support/uart.o ./Support/uart.su

.PHONY: clean-Support

