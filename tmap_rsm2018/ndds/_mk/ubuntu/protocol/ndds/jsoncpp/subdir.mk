################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../../protocol/ndds/jsoncpp/json_reader.cpp \
../../protocol/ndds/jsoncpp/json_value.cpp \
../../protocol/ndds/jsoncpp/json_writer.cpp 

OBJS += \
./OUT/json_reader.o \
./OUT/json_value.o \
./OUT/json_writer.o 

CPP_DEPS += \
./OUT/json_reader.d \
./OUT/json_value.d \
./OUT/json_writer.d 


# Each subdirectory must supply rules for building sources it contributes
OUT/%.o: ../../protocol/ndds/jsoncpp/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D_LINUX -D_GLIBCXX_USE_CXX11_ABI=0 -D_DEBUG -I"../../include" -I"../../interface/include" -I"../../interface/NddsInterface" -O2 -g -ggdb -Wall -fPIC -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


