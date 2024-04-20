#!/bin/sh
## only for Qt5.6
export QT_QPA_PLATFORM_PLUGIN_PATH=/opt/Qt5.6.1_dt_es/plugins
export QML2_IMPORT_PATH=/opt/Qt5.6.1_dt_es/qml
# tmap prefix
export RSM2018_PREFIX=/media/njas/DATA0/w0/aivi_tmap_data
# LD_LIBRARY_PATH 
export RSM2018_BUILD=/media/njas/DATA0/w0/build-tmap_rsm2018-Desktop-Debug
#export TMAP_RUN_ROOT=/media/njas/DATA0/w0/bin_aivi_tmap
export TMAP_RUN_ROOT=$RSM2018_BUILD
#export LD_LIBRARY_PATH=./:$RSM2018_PREFIX/tmap_rsm2018/mnssdk/lib/linux/debug
export LD_LIBRARY_PATH=$RSM2018_BUILD/ndds/debug:$RSM2018_BUILD/ui/debug:$RSM2018_BUILD/vsmsdk/build/qtcreator_vsm_sdk/build:$RSM2018_BUILD/../tmap_rsm2018/mnssdk/lib/linux/debug

export TMAP_LOG_FILTER_DISABLE_STDOUT=0
export TMAP_LOG_FILTER_ENABLE=1
export TMAP_LOG_FILTER_FILEPATH=$RSM2018_PREFIX/tmap_rsm2018/aivi_logging.dlf

#$RSM2018_PREFIX/tmap_rsm2018/vsmsdk/lib/linux/debug:

#$RSM2018_BUILD/thirdpartynavigation_out.out
tag=$(date +'%Y%m%dT%H%M%S')
mkdir -p /home/njas/tmp/vg/$tag
outdir=/home/njas/tmp/vg/$tag
/usr/local/valgrind/bin/valgrind --memcheck:leak-check=full --vgdb=yes --demangle=yes --log-file=$outdir/%p $TMAP_RUN_ROOT/thirdpartynavigation_out.out

#--child-silent-after-fork=yes --xml-socket=127.0.0.1:42951 --log-socket=127.0.0.1:41415 --xml=yes --smc-check=stack --tool=memcheck --gen-suppressions=all --track-origins=yes --leak-check=summary --num-callers=25

#--vgdb=yes --vgdb-error=0 
