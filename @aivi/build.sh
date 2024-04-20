#!/bin/sh
PRO=./tmap_rsm2018/tmap_rsm2018_so.pro
CC=/home/xjas/opt/mgc/embedded/codebench/bin/arm-none-linux-gnueabi-gcc
CXX=/home/xjas/opt/mgc/embedded/codebench/bin/arm-none-linux-gnueabi-g++
QMAKE=/opt/qt-5.6.1-embedded-sysroot/bin/qmake
$QMAKE /media/njas/DATA0/w0/tmap_rsm2018/tmap_rsm2018.pro -spec linux-arm-gnueabi-g++ CONFIG+=debug CONFIG+=qml_debug && /usr/bin/make qmake_all
