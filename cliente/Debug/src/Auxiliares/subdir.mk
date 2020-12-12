################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Auxiliares/Conexion.c \
../src/Auxiliares/Configuracion.c \
../src/Auxiliares/Consola.c \
../src/Auxiliares/SocketEscucha.c 

OBJS += \
./src/Auxiliares/Conexion.o \
./src/Auxiliares/Configuracion.o \
./src/Auxiliares/Consola.o \
./src/Auxiliares/SocketEscucha.o 

C_DEPS += \
./src/Auxiliares/Conexion.d \
./src/Auxiliares/Configuracion.d \
./src/Auxiliares/Consola.d \
./src/Auxiliares/SocketEscucha.d 


# Each subdirectory must supply rules for building sources it contributes
src/Auxiliares/%.o: ../src/Auxiliares/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/Repositorios/tp-2020-2c-Thread-Away/shared" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


