#!/bin/sh
## only for Qt5.6
export QT_QPA_PLATFORM_PLUGIN_PATH=/opt/Qt5.6.1_dt_es/plugins
export QML2_IMPORT_PATH=/opt/Qt5.6.1_dt_es/qml
# tmap prefix
export RSM2018_PREFIX=/media/njas/DATA0/w0
# LD_LIBRARY_PATH 
export RSM2018_BUILD=/media/njas/DATA0/w0/build-5.6-tmap_rsm2018-Desktop-Debug
export LD_LIBRARY_PATH=./:$RSM2018_PREFIX/tmap_rsm2018/mnssdk/lib/linux/debug

#$RSM2018_PREFIX/tmap_rsm2018/vsmsdk/lib/linux/debug:

#$RSM2018_BUILD/thirdpartynavigation_out.out
gdb ./thirdpartynavigation_out.out
