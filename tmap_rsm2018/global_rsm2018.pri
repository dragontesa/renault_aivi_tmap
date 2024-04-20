CONFIG += rsm2018
DEFINES += PRODUCT_RSM2018

RSM2018_TARGET_PREFIX = $$(RSM2018_TARGET_PREFIX)

# https://gcc.gnu.org/onlinedocs/libstdc++/manual/using_dual_abi.html
*-g++ {
    QMAKE_CXXFLAGS += -D_GLIBCXX_USE_CXX11_ABI=0
}

CONFIG(release, debug|release): DEFINES += NDEBUG
