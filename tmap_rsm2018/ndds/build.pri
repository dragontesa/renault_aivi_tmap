#QT       += core network
CONFIG -= qt
TEMPLATE = lib
TARGET = nddssdk

QMAKE_CXXFLAGS += -Wno-narrowing -fexec-charset=CP949

DEFINES += TMAP_RSM_NDDS_SO_LIBRARY

CONFIG(debug, debug|release) {
    DESTDIR = debug
} else {
    DESTDIR = release
}

CONFIG += plugin

rsm2018 {
    linux-gen3-g++ {
        DEFINES += _LINUX NDDS_EXPORTS __MM2018__
        LIBS += -ldlt
        QMAKE_CXXFLAGS += -Wno-narrowing
    } else: win32-g++ {
        DEFINES += _WIN32
    } else: unix {
        DEFINES += _LINUX
    }
} else:jlr2019 {
    qnx-aarch64le-qcc {
        DEFINES += _LINUX NDDS_EXPORTS __QNXNTO__ __JLR2019__
    #    LIBS += -ldlt
        QMAKE_CXXFLAGS += -Wno-narrowing
    } else {
        DEFINES += _LINUX __JLR2019__ NDDS_EXPORTS
    }
}

CONFIG(debug, debug|release){
    DEFINES += _DEBUG
} else {
    DEFINES +=
}
CONFIG += c++11

INCLUDEPATH = include interface/include interface/NddsInterface

rsm2018 {
    win32-g++ {
        INCLUDEPATH += include/SDL2
        INCLUDEPATH += $$PWD/../3rdparty/openssl-1.0.2l-mingw32/include
        LIBS += -L$$PWD/../3rdparty/openssl-1.0.2l-mingw32/bin -leay32 -lssleay32
        LIBS += -L$$PWD/include/SDL2/lib -lSDL2 -lws2_32
    } else {
        LIBS = -ldl -lpthread -lssl -lcrypto
    }
} else:jlr2019 {
    LIBS = -lssl -lcrypto
    qnx-aarch64le-qcc {
        LIBS += -liconv
    }
}

SOURCES += interface/NddsInterface/NDDS_Interface.cpp  \
           interface/NddsModule/interface.cpp  \
           interface/NddsModule/NddsModule.cpp  \
           protocol/ndds/jsoncpp/json_reader.cpp  \
           protocol/ndds/jsoncpp/json_value.cpp  \
           protocol/ndds/jsoncpp/json_writer.cpp  \
           protocol/ndds/NddsLibBase.cpp \
           protocol/ndds/NddsLibCommonData.cpp \
           protocol/ndds/NddsLibHttpHeader.cpp \
           protocol/ndds/NddsLibInterface.cpp \
           protocol/ndds/NddsLibModule.cpp \
           protocol/ndds/NddsLibTmap.cpp \
           protocol/ndds/ndds_base64.cpp \
           protocol/ndds/ndds_hancode.cpp \
           protocol/ndds/ndds_hancode_table.cpp \
           protocol/ndds/ndds_memory.cpp \
           protocol/ndds/ndds_memorypool.cpp \
           protocol/ndds/ndds_openssl.cpp \
           protocol/ndds/ndds_unicode.cpp \
           protocol/ndds/ndds_unicode_table.cpp

HEADERS += include/classcfg.h \
           include/common_def.h \
           include/debug.h \
           include/debug_linux.h \
           include/define_inc.h \
           include/disable_warning_inc.h \
           include/global.h \
           include/ndds_base_def.h \
           include/ndds_inc.h \
           include/ndds_tmap_def.h \
           include/singleton.h \
           include/version.h \
           interface/include/nddsmodule_inc.h \
           interface/NddsInterface/NDDS_Define.h \
           interface/NddsInterface/NDDS_Interface.h \
           interface/NddsModule/interface.h \
           interface/NddsModule/interface_def.h \
           interface/NddsModule/NddsModule.h \
           protocol/ndds/jsoncpp/json_batchallocator.h \
           protocol/ndds/jsoncpp/json_tool.h \
           protocol/ndds/NddsLibBase.h \
           protocol/ndds/NddsLibCommonData.h \
           protocol/ndds/NddsLibConst.h \
           protocol/ndds/NddsLibHttpHeader.h \
           protocol/ndds/NddsLibInterface.h \
           protocol/ndds/NddsLibModule.h \
           protocol/ndds/NddsLibTmap.h \
           protocol/ndds/ndds_base64.h \
           protocol/ndds/ndds_hancode.h \
           protocol/ndds/ndds_hancode_table.h \
           protocol/ndds/ndds_memory.h \
           protocol/ndds/ndds_memorypool.h \
           protocol/ndds/ndds_openssl.h \
           protocol/ndds/ndds_unicode.h \
           protocol/ndds/ndds_unicode_table.h
