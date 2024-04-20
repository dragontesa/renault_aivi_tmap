################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../../interface/NddsModule/NddsModule.cpp \
../../interface/NddsModule/interface.cpp 

OBJS += \
./OUT/NddsModule.o \
./OUT/interface.o 

CPP_DEPS += \
./OUT/NddsModule.d \
./OUT/interface.d 

# Each subdirectory must supply rules for building sources it contributes
OUT/%.o: ../../interface/NddsModule/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D_LINUX -D_GLIBCXX_USE_CXX11_ABI=0 -D_DEBUG -I"../../include" -I"../../interface/include" -I"../../interface/NddsInterface" -O2 -g -ggdb -Wall -fPIC -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


