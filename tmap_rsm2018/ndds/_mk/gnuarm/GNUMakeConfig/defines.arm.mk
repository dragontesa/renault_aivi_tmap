PREFIX = arm-none-linux-gnueabi-
CC = $(PREFIX)gcc
CXX = $(PREFIX)g++

AR = $(PREFIX)ar
ARFLAGS = -crfs
LD = $(PREFIX)g++
RANLIB = $(PREFIX)ranlib
STRIP = $(PREFIX)strip

#본인 PC의 sysroots로 변경해야함
BOSCH_ROOTFS_DIR = /home/thirdpartydev/sysroots_for_sk/F28/mx6q

#디버그 버전 빌드시 -g추가, -O2를 -O0로 변경하고 makefile의 CPPFLAGS에 -D_DEBUG추가
#릴리즈 버전 빌드시 -g삭제, -O0를 -O2로 변경하고 makefile의 CPPFLAGS에 -D_DEBUG삭제
BOSCH_GEN3_CPPFLAGS = -fcheck-new -fexceptions -fPIC -g -mfloat-abi=softfp -mfpu=vfpv3-d16 -march=armv7-a -mtune=cortex-a9 \
--sysroot==$(BOSCH_ROOTFS_DIR) -Wno-psabi -O0  

INCLUDE = 

BOSCH_GEN3_LDFLAGS = -Wl,--gc-sections -Wl,--hash-style=gnu --sysroot=$(BOSCH_ROOTFS_DIR) \
-L$(BOSCH_ROOTFS_DIR)/usr/lib -Wl,--exclude-libs,ALL

CPPFLAGS = $(INCLUDE) $(BOSCH_GEN3_CPPFLAGS) 
CFLAGS = $(CPPFLAGS)
CFLAGS += -fvisibility=hidden
CXXFLAGS = $(CPPFLAGS) 
CXXFLAGS += -fvisibility=hidden -fvisibility-inlines-hidden

LDFLAGS = $(BOSCH_GEN3_LDFLAGS) -rdynamic -Wl,-R,'$$ORIGIN',--enable-new-dtags
