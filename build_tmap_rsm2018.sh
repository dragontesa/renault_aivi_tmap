#!/usr/bin/env bash
WORKSPACE_DIR=/media/njas/DATA0/w0
PROJECT=tmap_rsm2018
BUILD_TYPE=debug
BUILD_OUT=build-tmap_rsm2018-Desktop-Debug
BUILD_COMMAND="build"  # clean, dist, install
DIST_OUT=bin_aivi_tmap
MYQMAKE=/opt/Qt5.6.1_dt_es/bin/qmake
export RSM2018_PREFIX=/media/njas/DATA0/w0/aivi_tmap_data

########## select job
if [ $# -gt 0 ]; then
   if [ $1 != "build" ] && [ $1 != "clean" ] && [ $1 != "dist" ] && [ $1 != "install" ]; then
      echo "type correct command, which is build, clean, dist, install"
      exit 1
   fi
   BUILD_COMMAND=$1
fi


############ build
f_build()
{
# move to working directory
cd $WORKSPACE_DIR/$BUILD_OUT

# qmake
$MYQMAKE -makefile ../$PROJECT/tmap_rsm2018.pro -spec linux-g++ CONFIG+=$BUILD_TYPE CONFIG+=qml_debug
make qmake_all

# build
make -j 2 all

if [ $? != 0 ]; then
errCode=$?
echo "build canceled by error: $errCode"
exit $?
fi
}

############ distribute
f_dist()
{
# move to working directory
cd $WORKSPACE_DIR/$BUILD_OUT
cp -fpv thirdpartynavigation_out.out ui/$BUILD_TYPE/libtmap_rsm_ui.so ndds/$BUILD_TYPE/libnddssdk.so vsmsdk/build/qtcreator_vsm_sdk/build/libvsmsdk.so ../$PROJECT/mnssdk/lib/linux/$BUILD_TYPE/libmnssdk.so ../$PROJECT/ui/res/*.rcc ../$PROJECT/ui/res/lang/*.qm ../$DIST_OUT
#cp -fpv ../$PROJECT/ui/res/lang/*.qm $RSM2018_PREFIX/tmap_rsm2018/static/res/lang
}

############ install
f_install()
{
# move to working directory
cd $WORKSPACE_DIR/$BUILD_OUT
# install
make install
}

############ clean
f_clean()
{
# move to working directory
cd $WORKSPACE_DIR/$BUILD_OUT
# clean
make clean
}


######## execute job
if [ $BUILD_COMMAND = "build" ]; then
f_build
f_dist
elif  [ $BUILD_COMMAND = "clean" ]; then
f_clean
elif  [ $BUILD_COMMAND = "dist" ]; then
f_dist
elif  [ $BUILD_COMMAND = "install" ]; then
f_install
fi

exit 0


