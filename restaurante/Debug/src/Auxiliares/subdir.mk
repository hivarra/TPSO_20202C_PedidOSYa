################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/Auxiliares/Cocinero.c \
../src/Auxiliares/Conexion.c \
../src/Auxiliares/Configuracion.c \
../src/Auxiliares/Mensajes.c \
../src/Auxiliares/Planificador.c \
../src/Auxiliares/Plato.c \
../src/Auxiliares/Utils.c 

OBJS += \
./src/Auxiliares/Cocinero.o \
./src/Auxiliares/Conexion.o \
./src/Auxiliares/Configuracion.o \
./src/Auxiliares/Mensajes.o \
./src/Auxiliares/Planificador.o \
./src/Auxiliares/Plato.o \
./src/Auxiliares/Utils.o 

C_DEPS += \
./src/Auxiliares/Cocinero.d \
./src/Auxiliares/Conexion.d \
./src/Auxiliares/Configuracion.d \
./src/Auxiliares/Mensajes.d \
./src/Auxiliares/Planificador.d \
./src/Auxiliares/Plato.d \
./src/Auxiliares/Utils.d 


# Each subdirectory must supply rules for building sources it contributes
src/Auxiliares/%.o: ../src/Auxiliares/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/Repositorios/tp-2020-2c-Thread-Away/shared" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


