#!/bin/sh
## standalone Qt 5.6 설치시 필요, 자신의 경로에 맞추면 된다
export QT_QPA_PLATFORM_PLUGIN_PATH=/home/thirdpartydev/tmap_rsm2018/Qt5.6.1_dt_es/plugins
export QML2_IMPORT_PATH=/home/thirdpartydev/tmap_rsm2018/Qt5.6.1_dt_es/qml
# tmap prefix (tmap_rsm2018 소스 디렉토리의 상위 경로)
export RSM2018_PREFIX=/home/thirdpartydev
# LD_LIBRARY_PATH  - tmap_rsm2018 실행시 필요한 라이브러리(libmnssdk.so, libvsmsdk.so, libndds.so, libtmap_rsm_ui.so)를 tmap_rsm2018프로그램과 같은 위치로 모두 복사하고 LD path를 같은 위치로 선언해준다
export RSM2018_BUILD=/home/thirdpartydev/build-5.6-tmap_rsm2018-Desktop-Debug
export LD_LIBRARY_PATH=./:$RSM2018_PREFIX/tmap_rsm2018/mnssdk/lib/linux/debug

# run tmap_rsm2018 by valgrind
/usr/local/valgrind/bin/valgrind --memcheck:leak-check=full --vgdb=yes --demangle=yes --log-file=vg_%p_rsm2018 ./thirdpartynavigation_out.out

