################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../DIO_program.c \
../MCP2515_Program.c \
../SPI_Program.c \
../UART_Program.c \
../main.c 

OBJS += \
./DIO_program.o \
./MCP2515_Program.o \
./SPI_Program.o \
./UART_Program.o \
./main.o 

C_DEPS += \
./DIO_program.d \
./MCP2515_Program.d \
./SPI_Program.d \
./UART_Program.d \
./main.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -Wall -g2 -gstabs -O0 -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega32 -DF_CPU=8000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


