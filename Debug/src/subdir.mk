################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/counter.c \
../src/netflow.c \
../src/snmp.c \
../src/syslog.c \
../src/test_nm.c 

OBJS += \
./src/counter.o \
./src/netflow.o \
./src/snmp.o \
./src/syslog.o \
./src/test_nm.o 

C_DEPS += \
./src/counter.d \
./src/netflow.d \
./src/snmp.d \
./src/syslog.d \
./src/test_nm.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I/home/osboxes/netmap/sys -I"/home/osboxes/workspace/netmap/inc" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


