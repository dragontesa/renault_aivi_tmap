################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../../protocol/ndds/ndds_base64.cpp \
../../protocol/ndds/ndds_hancode_table.cpp \
../../protocol/ndds/ndds_hancode.cpp \
../../protocol/ndds/ndds_memory.cpp \
../../protocol/ndds/ndds_memorypool.cpp \
../../protocol/ndds/ndds_unicode_table.cpp \
../../protocol/ndds/ndds_unicode.cpp \
../../protocol/ndds/NddsLibBase.cpp \
../../protocol/ndds/NddsLibCommonData.cpp \
../../protocol/ndds/NddsLibHttpHeader.cpp \
../../protocol/ndds/NddsLibInterface.cpp \
../../protocol/ndds/NddsLibModule.cpp \
../../protocol/ndds/NddsLibTmap.cpp 

OBJS += \
./OUT/ndds_base64.o \
./OUT/ndds_hancode_table.o \
./OUT/ndds_hancode.o \
./OUT/ndds_memory.o \
./OUT/ndds_memorypool.o \
./OUT/ndds_unicode_table.o \
./OUT/ndds_unicode.o \
./OUT/NddsLibBase.o \
./OUT/NddsLibCommonData.o \
./OUT/NddsLibHttpHeader.o \
./OUT/NddsLibInterface.o \
./OUT/NddsLibModule.o \
./OUT/NddsLibTmap.o 

CPP_DEPS += \
./OUT/ndds_base64.d \
./OUT/ndds_hancode_table.d \
./OUT/ndds_hancode.d \
./OUT/ndds_memory.d \
./OUT/ndds_memorypool.d \
./OUT/ndds_unicode_table.d \
./OUT/ndds_unicode.d \
./OUT/NddsLibBase.d \
./OUT/NddsLibCommonData.d \
./OUT/NddsLibHttpHeader.d \
./OUT/NddsLibInterface.d \
./OUT/NddsLibModule.d \
./OUT/NddsLibTmap.d 


# Each subdirectory must supply rules for building sources it contributes
OUT/%.o: ../../protocol/ndds/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -D_LINUX -D_GLIBCXX_USE_CXX11_ABI=0 -D_DEBUG -I"../../include" -I"../../interface/include" -I"../../interface/NddsInterface" -O2 -g -ggdb -Wall -fPIC -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


