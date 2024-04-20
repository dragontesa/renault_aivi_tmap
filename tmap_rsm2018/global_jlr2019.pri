#CONFIG += ignore_mnssdk
CONFIG += jlr2019
DEFINES += PRODUCT_JLR2019

# https://gcc.gnu.org/onlinedocs/libstdc++/manual/using_dual_abi.html
*-g++ {
    QMAKE_CXXFLAGS += -D_GLIBCXX_USE_CXX11_ABI=0
}
