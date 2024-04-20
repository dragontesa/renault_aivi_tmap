################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../../interface/NddsInterface/NDDS_Interface.cpp 

OBJS += \
./OUT/NDDS_Interface.o 

CPP_DEPS += \
./OUT/NDDS_Interface.d 


# Each subdirectory must supply rules for building sources it contributes
OUT/%.o: ../../interface/NddsInterface/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D_LINUX -D_GLIBCXX_USE_CXX11_ABI=0 -D_DEBUG -I"../../include" -I"../../interface/include" -I"../../interface/NddsInterface" -O2 -g -ggdb -Wall -fPIC -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


