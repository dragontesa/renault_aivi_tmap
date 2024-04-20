QT += qml quick dbus widgets sql concurrent xml gui-private
CONFIG += c++11

PRECOMPILED_HEADER = pch.h
CONFIG += precompile_header

ignore_mnssdk {
    DEFINES += USE_DUMMY_MNSSDK
    DEFINES += USE_DUMMY_NDDSSDK
}
    DEFINES += GEN3ARM

equals(TEMPLATE, lib) {
    DEFINES += \
        BUILD_EXPORT
} else {
    SOURCES += main.cpp
    DEFINES += BUILD_APP
}

SOURCES += \
    $$PWD/mns/serverpolicyfetch.cpp \
    tmapapplication.cpp \
    tmap_hostnames.cpp \
    gps/gpsservice.cpp \
    gps/gpsutil.cpp \
    gps/filegpsprovider.cpp \
    gps/positioncache.cpp \
    simulator/rgsimulator.cpp \
    util/logger.cpp \
    util/messagequeue.cpp \
    util/textdecoder.cpp \
    announcement/rgaudiohelper.cpp \
    route/rgdatapool.cpp \
    build.cpp \
    mns/searchmanager.cpp \
    mns/searchmanagertest.cpp \
    mns/serversearch.cpp \
    mns/simplesearch.cpp \
    tmapcontroller.cpp \
    tmapproperty.cpp \
    update/updateworker.cpp \
    gps/satellitedatapool.cpp \
    gps/nmeadata.cpp \
    update/packagemanager.cpp \
    update/packageinfo.cpp \
    update/packageutil.cpp \
    util/sessionmanager.cpp \
    gps/locationdatapool.cpp \
    util/sunrisesunsetcalculator.cpp \
    util/daynightdetector.cpp \
    debugkeyfilter.cpp \
    gps/drdata.cpp \
    $$PWD/platforms/linux-gen3/zone1helper.cpp \
    $$PWD/platforms/linux-gen3/zone3helper.cpp \
    gps/positionreceive.cpp \
    gps/logfile.cpp \
    $$PWD/util/datetimeutil.cpp \
    $$PWD/gps/locationdata.cpp \
    $$PWD/gps/satellitedata.cpp \
    $$PWD/util/cipherservice.cpp \
    $$PWD/util/qaesencryption.cpp \
    $$PWD/common/loggingcategories.cpp \
    $$PWD/platforms/linux-gen3/hardkeyfilter.cpp \
    $$PWD/tmapcontroller_cluster.cpp \
    $$PWD/tmapcontroller_gadget.cpp \
    util/crashlogger.cpp \
    $$PWD/cswfilter.cpp \
    util/opensslthreadlock.cpp

RESOURCES += qml.qrc

INCLUDEPATH += \
    gps \
    simulator \
    route \
    util \
    announcement \
    test \
    platforms \
    common \
    update

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

HEADERS += \
    $$PWD/mns/serverpolicyfetch.h \
    tmapapplication.h \
    gps/gpsservice.h \
    gps/gpsutil.h \
    gps/filegpsprovider.h \
    gps/positionprovider.h \
    gps/positioncache.h \
    simulator/rgsimulator.h \
    environment.h \
    util/logger.h \
    util/objectpool.h \
    util/messagequeue.h \
    util/textdecoder.h \
    announcement/rgaudiohelper.h \
    tmapconfig.h \
    common/hostnames.h \
    common/rgdataptr.h \
    route/rgdatapool.h \
    build.h \
    common/searchrequest.h \
    mns/searchmanager.h \
    mns/searchmanagertest.h \
    mns/serversearch.h \
    mns/simplesearch.h \
    common/rprequest.h \
    common/navigationcontroller.h \
    tmapcontroller.h \
    platformcontroller.h \
    platform_connectivity.h \
    tmapproperty.h \
    common/navigationproperty.h \
    util/fpscalculator.h \
    common/uilayout.h \
    common/navigationdata.h \
    common/updatemanagerinterface.h \
    update/updateworker.h \
    util/stringutil.h \
    common/satellitedataptr.h \
    gps/satellitedatapool.h \
    common/nmeatype.h \
    gps/nmeadata.h \
    update/packagemanager.h \
    update/packageinfo.h \
    update/packageutil.h \
    util/sessionmanager.h \
    common/sessionmanagerinterface.h \
    common/gpsmanagerinterface.h \
    common/simulatorinterface.h \
    common/locationdataptr.h \
    gps/locationdatapool.h \
    util/sunrisesunsetcalculator.h \
    util/daynightdetector.h \
    debugkeyfilter.h \
    common/searchmanagerinterface.h \
    common/rpmanagerinterface.h \
    gps/drdata.h \
    util/keymapping.h \
    $$PWD/platforms/linux-gen3/zone1helper.h \
    $$PWD/platforms/linux-gen3/zone3helper.h \
    gps/positionreceive.h \
    gps/logfile.h \
    util/keymapping.h \
    test/qctestingfeatures.h \
    $$PWD/util/datetimeutil.h \
    $$PWD/common/cipherserviceinterface.h \
    $$PWD/util/cipherservice.h \
    $$PWD/util/qaesencryption.h \
    $$PWD/common/loggingcategories.h \
    $$PWD/pch.h \
    $$PWD/platforms/linux-gen3/hardkeyfilter.h \
    $$PWD/common/skyconstants.h \
    util/crashlogger.h \
    $$PWD/common/uiconstants.h \
    $$PWD/common/mapviewconstants.h \
    $$PWD/cswfilter.h \
    util/opensslthreadlock.h


GEN3_SOURCES += \
    platforms/linux-gen3/interface/guicontrol/guicontrolinterface.cpp \
    platforms/linux-gen3/guicontrolpropertyobserver.cpp \
    platforms/linux-gen3/interface/server/navigation/navigationserviceadaptor.cpp \
    platforms/linux-gen3/interface/navigationext/navigationextserviceinterface.cpp \
    platforms/linux-gen3/interface/server/navigation/navigationservicetype.cpp \
    platforms/linux-gen3/interface/guicontrol/guicontroltype.cpp \
    platforms/linux-gen3/interface/navigationext/navigationexttype.cpp \
    platforms/linux-gen3/interface/server/navigation/navigationserver.cpp \
    platforms/linux-gen3/interface/tdbusabstractinterface.cpp \
    platforms/linux-gen3/interface/deadreckoning/drpositionserviceinterface.cpp \
    platforms/linux-gen3/interface/deadreckoning/drpositiontype.cpp \
    platforms/linux-gen3/interface/carconfiguration/carconfigurationinterface.cpp \
    platforms/linux-gen3/interface/privateDataEncrypt/privateDatainterface.cpp \
    platforms/linux-gen3/platformcontroller_gen3.cpp \
    platforms/linux-gen3/appinterface.cpp \
    platforms/linux-gen3/applicationthread.cpp \
    platforms/linux-gen3/layermanager.cpp \
    platforms/linux-gen3/navigationextpropertyobserver.cpp \
    platforms/linux-gen3/interface/HMIZone13/hmiZoneDataInterface.cpp \
    platforms/linux-gen3/interface/HMIZone13/hmiZonetype.cpp \
    platforms/linux-gen3/hmizonedatapropertyobserver.cpp \
    platforms/linux-gen3/interface/extAgent/extagenttype.cpp \
    platforms/linux-gen3/interface/extAgent/navigationExtAgent.cpp \
    platforms/linux-gen3/interface/extAgent/navigationExtAgentAdaptor.cpp \
    platforms/linux-gen3/interface/privateDataEncrypt/privateDataEncryptiontype.cpp \
    platforms/linux-gen3/navigationprofilepropertyobserver.cpp \
    platforms/linux-gen3/interface/server/navigationSDS/navigationsdsservicetype.cpp \
    platforms/linux-gen3/interface/server/navigationSDS/navigationsdsserviceadaptor.cpp \
    platforms/linux-gen3/interface/server/navigationSDS/navigationsdsservice.cpp \
    platforms/linux-gen3/interface/server/navigationDataUpdate/navigationdataupdateservicetype.cpp \
    platforms/linux-gen3/interface/server/navigationDataUpdate/navigationdataupdateserviceadaptor.cpp \
    platforms/linux-gen3/interface/server/navigationDataUpdate/navigationdataupdateservice.cpp \
    platforms/linux-gen3/interface/lifecyclemanager/lifecyclemanagertype.cpp \
    platforms/linux-gen3/interface/lifecyclemanager/lifecyclemanagerinterface.cpp \
    platforms/linux-gen3/interface/securedatacom/securedatacommunicationinterface.cpp \
    platforms/linux-gen3/NavigationExtCentralLock.cpp


GEN3_HEADERS += \
    platforms/linux-gen3/interface/tdbusabstractinterface.h \
    platforms/linux-gen3/guicontrolpropertyobserver.h \
    platforms/linux-gen3/interface/server/navigation/navigationserviceadaptor.h \
    platforms/linux-gen3/interface/server/navigation/navigationservicetype.h \
    platforms/linux-gen3/interface/server/navigation/navigationserver.h \
    platforms/linux-gen3/interface/guicontrol/guicontrolinterface.h \
    platforms/linux-gen3/interface/guicontrol/guicontroltype.h \
    platforms/linux-gen3/interface/navigationext/navigationextserviceinterface.h \
    platforms/linux-gen3/interface/navigationext/navigationexttype.h \
    platforms/linux-gen3/interface/deadreckoning/drpositionserviceinterface.h \
    platforms/linux-gen3/interface/deadreckoning/drpositiontype.h \
    platforms/linux-gen3/interface/deadreckoning/DrPosInterface.h \
    platforms/linux-gen3/interface/carconfiguration/carconfigurationinterface.h \
    platforms/linux-gen3/interface/privateDataEncrypt/privateDatainterface.h \
    platforms/linux-gen3/platformcontroller_gen3.h \
    platforms/linux-gen3/appinterface.h \
    platforms/linux-gen3/applicationthread.h \
    platforms/linux-gen3/ExternalThirdPartyNavigationApp_interface.h \
    platforms/linux-gen3/layermanager.h \
    platforms/linux-gen3/navigationextpropertyobserver.h \
    platforms/linux-gen3/interface/HMIZone13/hmiZoneDataInterface.h \
    platforms/linux-gen3/interface/HMIZone13/hmiZonetype.h \
    platforms/linux-gen3/hmizonedatapropertyobserver.h \
    platforms/linux-gen3/interface/extAgent/extagenttype.h \
    platforms/linux-gen3/interface/extAgent/navigationExtAgent.h \
    platforms/linux-gen3/interface/extAgent/navigationExtAgentAdaptor.h \
    platforms/linux-gen3/interface/privateDataEncrypt/privateDataEncryptiontype.h \
    platforms/linux-gen3/navigationprofilepropertyobserver.h \
    platforms/linux-gen3/interface/server/navigationSDS/navigationsdsservicetype.h \
    platforms/linux-gen3/interface/server/navigationSDS/navigationsdsserviceadaptor.h \
    platforms/linux-gen3/interface/server/navigationSDS/navigationsdsservice.h \
    platforms/linux-gen3/interface/server/navigationDataUpdate/navigationdataupdateservicetype.h \
    platforms/linux-gen3/interface/server/navigationDataUpdate/navigationdataupdateserviceadaptor.h \
    platforms/linux-gen3/interface/server/navigationDataUpdate/navigationdataupdateservice.h \
    platforms/linux-gen3/interface/lifecyclemanager/lifecyclemanagertype.h \
    platforms/linux-gen3/interface/lifecyclemanager/lifecyclemanagerinterface.h \
    platforms/linux-gen3/interface/securedatacom/securedatacommunicationinterface.h \
    platforms/linux-gen3/NavigationExtCentralLock.h

    equals(TEMPLATE, lib) {
            GEN3_SOURCES += platforms/linux-gen3/connmanager.cpp
            GEN3_HEADERS += platforms/linux-gen3/connmanager.h
    } else {
            GEN3_SOURCES += platforms/linux-gen3/connmanager_lxc.cpp
            GEN3_HEADERS += platforms/linux-gen3/connmanager_lxc.h
            GEN3_SOURCES += platforms/linux-gen3/connmanager_csm.cpp
            GEN3_HEADERS += platforms/linux-gen3/connmanager_csm.h
    }

GEN3_INCLUDEPATH += \
    platforms/linux-gen3 \
    platforms/linux-gen3/announcement/ \
    platforms/linux-gen3/interface \
    platforms/linux-gen3/interface/guicontrol \
    platforms/linux-gen3/interface/navigationext \
    platforms/linux-gen3/interface/server/navigation \
    platforms/linux-gen3/interface/deadreckoning \
    platforms/linux-gen3/interface/HMIZone13 \
    platforms/linux-gen3/interface/extAgent \
    platforms/linux-gen3/interface/carconfiguration \
    platforms/linux-gen3/interface/privateDataEncrypt \


rsm2018 {
    linux-gen3-g++ {
        DEFINES += BUILD_TARGET
        DEFINES += PLATFORM_GEN3
        #DEFINES += BUILD_TEST
        DEFINES += USE_DBUS
        DEFINES += USE_WAYLAND
        #DEFINES += RSM_TOUCH_INTEGRATION
        DEFINES += USE_SYNCBLOCK
        DEFINES += USE_MIC
        DEFINES += USE_ALSA
        DEFINES += USE_FBO_FOR_GADGET

        LIBS += -L$$PWD/platforms/linux-gen3/connman/lib -lcsmanagergdbus_so  # csmanagergdbus_so, csmanagergdbus_stripped_so, for connman csmanager
        LIBS += -lgio-2.0

        LIBS += -lilmCommon -lilmClient -lilmControl
        LIBS += -ldlt
        LIBS += -lrt
        LIBS += -lwayland-client

        # ssl stack required from connectivity with ecm
        LIBS += -lssl -lcrypto

        # suppress the default RPATH if you wish
        QMAKE_LFLAGS_RPATH=
        # add your own with quoting gyrations to make sure $ORIGIN gets to the command line unexpanded
        QMAKE_LFLAGS += \
            "-Wl,-rpath,\'/opt/bosch/navigation_kor/lib'"

        SOURCES += \
            platforms/linux-gen3/gps/drpositionprovider_gen3.cpp \
            platforms/linux-gen3/gps/socket_client.cpp \
            platforms/linux-gen3/gps/gpsprovider_gen3.cpp \
            platforms/linux-gen3/announcement/SharedMemory.cpp \
            platforms/linux-gen3/announcement/rgaudioplayer.cpp \
            platforms/linux-gen3/announcement/rgalarmplayer.cpp \
            platforms/linux-gen3/announcement/rgaudiodelegate_gen3.cpp \
            platforms/linux-gen3/environment_gen3.cpp \
            platforms/linux-gen3/logger_gen3.cpp \
            platforms/linux-gen3/platform_connectivity_gen3.cpp \
            $$GEN3_SOURCES

        HEADERS += \
            platforms/linux-gen3/gps/drpositionprovider_gen3.h \
            platforms/linux-gen3/gps/socket_client.h \
            platforms/linux-gen3/gps/gpsprovider_gen3.h \
            platforms/linux-gen3/announcement/ThirdPartyShareMemoryName.h \
            platforms/linux-gen3/announcement/SharedMemory.h \
            platforms/linux-gen3/announcement/rgaudioplayer.h \
            platforms/linux-gen3/announcement/rgalarmplayer.h \
            platforms/linux-gen3/announcement/rgaudioplayerinterface.h \
            platforms/linux-gen3/announcement/rgaudiodelegate_gen3.h \
            platforms/linux-gen3/platform_connectivity_gen3.h \
            $$GEN3_HEADERS

             # standalone glib for connman csmanager 
            INCLUDEPATH += platforms/linux-gen3/interface/mic/include/glib-2.0
            INCLUDEPATH += platforms/linux-gen3/interface/mic/include/glib-2.0/include
            INCLUDEPATH += platforms/linux-gen3/interface/mic/include/glib-2.0/glib

        INCLUDEPATH += \
            $$GEN3_INCLUDEPATH

        # TEST CODE
        contains(DEFINES, BUILD_TEST) {
            SOURCES += \
                test/linux-gen3/dbustest.cpp \
                test/linux-gen3/drpositiontest.cpp

            HEADERS += \
                test/linux-gen3/dbustest.h \
                test/linux-gen3/drpositiontest.h

            INCLUDEPATH += \
                test/linux-gen3
        }
    } else {
        DEFINES += PLATFORM_GEN3
        DEFINES += MAPVIEWQUICK_MOUSE_EVENT
        DEFINES += TMAP_CONNMAN_PROXY_API  # must be defined on desktop, csmanager must be used only for linux-gen3

        SOURCES += \
            platforms/dummy/platform_connectivity_dummy.cpp \
            platforms/dummy/gpsprovider_dummy.cpp \
            platforms/dummy/rgaudiodelegate_dummy.cpp \
            platforms/dummy/environment_dummy.cpp \
#            platforms/dummy/logger_dummy.cpp \
            platforms/dummy/logger_desktop.cpp

        SOURCES += \
            $$GEN3_SOURCES

        HEADERS += \
            platforms/dummy/logger_desktop.h \
            platforms/dummy/platform_connectivity_dummy.h \
            $$GEN3_HEADERS
        INCLUDEPATH += \
            $$GEN3_INCLUDEPATH
    }


} else: jlr2019 {
    DEFINES += TMAP_UI_LAYOUT=1920720

    qnx-aarch64le-qcc {
        DEFINES += BUILD_TARGET
        DEFINES += PLATFORM_QNX
        #DEFINES += USE_INTERNAL_CP949   # TODO: LGPL!

        SOURCES += \
            platforms/qnx/environment_qnx.cpp \
            platforms/qnx/gpsprovider_qnx.cpp \
            platforms/qnx/logger_qnx.cpp \
            platforms/qnx/rgaudiodelegate_qnx.cpp \

        SOURCES += \
            platforms/qnx/platformcontroller_qnx.cpp
        HEADERS += \
            platforms/qnx/platformcontroller_qnx.h
        INCLUDEPATH += \
            platforms/qnx
    } else {
        DEFINES += MAPVIEWQUICK_MOUSE_EVENT
        #DEFINES += USE_INTERNAL_CP949   # TODO: LGPL!

        SOURCES += \
            platforms/dummy/gpsprovider_dummy.cpp \
            platforms/dummy/rgaudiodelegate_dummy.cpp \
            platforms/dummy/environment_dummy.cpp \
            platforms/dummy/logger_dummy.cpp

        SOURCES += \
            platforms/qnx/platformcontroller_qnx.cpp
        HEADERS += \
            platforms/qnx/platformcontroller_qnx.h
        INCLUDEPATH += \
            platforms/qnx
    }
} else {
    DEFINES += MAPVIEWQUICK_MOUSE_EVENT

    SOURCES += \
        platforms/dummy/gpsprovider_dummy.cpp \
        platforms/dummy/rgaudiodelegate_dummy.cpp \
        platforms/dummy/environment_dummy.cpp \
        platforms/dummy/logger_dummy.cpp

    SOURCES += \
        platforms/dummy/platformcontroller_dummy.cpp
    HEADERS += \
        platforms/dummy/platformcontroller_dummy.h
    INCLUDEPATH += \
        platforms/dummy
}

# VSM HELPER
SOURCES += \
    vsm/map.cpp \
    vsm/mapviewquick.cpp \
    vsm/mapviewquickfborenderer.cpp \
    vsm/maptapdetector.cpp \
    vsm/mapgesturedetector.cpp \
    vsm/addressmanager.cpp \
    vsm/mapviewclient.cpp \
    vsm/worldpoint.cpp \
    vsm/mapswitcher.cpp \
    vsm/taplistenerimpl.cpp \
    vsm/gesturelistenerimpl.cpp \
    vsm/euckrcodec.cpp \
    vsm/signatureutil.cpp \
    vsm/sslcontext.cpp


HEADERS += \
    vsm/map.h \
    vsm/mapviewquick.h \
    vsm/mapviewquickfborenderer.h \
    vsm/maptapdetector.h \
    vsm/mapgesturedetector.h \
    vsm/mapviewconfiguration.h \
    vsm/addressmanager.h \
    vsm/mapviewclient.h \
    vsm/worldpoint.h \
    vsm/mapswitcher.h \
    vsm/gesturelistenerimpl.h \
    vsm/euckrcodec.h \
    vsm/velocitytracker.h \
    vsm/signatureutil.h \
    vsm/sslcontext.h

INCLUDEPATH += \
    vsm

# VSM SDK
VSMSDKPath = $$PWD/vsmsdk
INCLUDEPATH += $$VSMSDKPath/build/linux_vsm_sdk/Include
LIBS += -Lvsmsdk/build/qtcreator_vsm_sdk/build -lvsmsdk

# the libvsmsdk needs the tts library
linux-gen3-g++ {
    QMAKE_LFLAGS += "-Wl,-rpath-link,\'$$VSMSDKPath/build/qtcreator_vsm_sdk/tts/linux-gen3/lib'"
} else: qnx-aarch64le-qcc {
    QMAKE_LFLAGS += "-Wl,-rpath-link,\'$$VSMSDKPath/build/qtcreator_vsm_sdk/tts/qnx-aarch64le/lib'"
}

# MNS HELPER
SOURCES += \
    mns/rpnsearch.cpp \
    mns/rpnsearchserver.cpp \
    mns/tvasparser.cpp

HEADERS += \
    mns/rpnsearch.h \
    mns/rpnsearchserver.h \
    mns/tvasparser.h

INCLUDEPATH += \
    mns

# MNS SDK
MNSSDKPath = $$PWD/mnssdk
HEADERS += $$MNSSDKPath/include/*.h
INCLUDEPATH += $$MNSSDKPath/include

#DEFINES += BUILD_TARGET_MNS

contains(DEFINES, USE_DUMMY_MNSSDK) {
    SOURCES += mns_dummy.cpp
} else {
    linux-gen3-g++ {
        CONFIG(debug,debug|release) {
            LIBS += -L$$MNSSDKPath/lib/linux-gen3/debug -lmnssdk
        } else{
            LIBS += -L$$MNSSDKPath/lib/linux-gen3/release -lmnssdk
        }
    } else: win32-g++ {
        LIBS += -L$$MNSSDKPath/lib/win32/release -lmnssdk
    } else: qnx-aarch64le-qcc {
        CONFIG(debug, debug|release) {
            LIBS += -L$$MNSSDKPath/lib/qnx-aarch64le/debug -lmnssdk
        } else {
            LIBS += -L$$MNSSDKPath/lib/qnx-aarch64le/release -lmnssdk
        }
    } else: unix {
        LIBS += -L$$MNSSDKPath/lib/linux/debug -lmnssdk
    }
}

# NDDS
INCLUDEPATH += ndds/interface/NddsInterface

contains(DEFINES, USE_DUMMY_NDDSSDK) {
    SOURCES += ndds_dummy.cpp
} else {
    CONFIG(debug,debug|release) {
        LIBS += -Lndds/debug -lnddssdk
    } else{
        LIBS += -Lndds/release -lnddssdk
    }
}

CONFIG( debug, debug|release ) {
    #DEFINES += QT_NO_DEBUG_OUTPUT
}
else {
    DEFINES += QT_NO_DEBUG
    DEFINES += QT_NO_DEBUG_OUTPUT
}

# TMAP RSM UI
TMAP_RSM_UI_PATH = ui
DEFINES += TMAP_RSM_UI_DLL_LIBRARY
INCLUDEPATH += $$TMAP_RSM_UI_PATH
CONFIG(debug,debug|release){
	LIBS += -L$$TMAP_RSM_UI_PATH/debug -ltmap_rsm_ui
} else{
	LIBS += -L$$TMAP_RSM_UI_PATH/release -ltmap_rsm_ui        
}

# 3rd party
win32-g++ {
    # openssl
    INCLUDEPATH += $$PWD/3rdparty/openssl-1.0.2l-mingw32/include
    LIBS += -L$$PWD/3rdparty/openssl-1.0.2l-mingw32/bin -leay32 -lssleay32

    # libcurl: just for library search path
    LIBS += -L$$PWD/3rdparty/curl-7.55.1-mingw32/lib
} else {
    # openssl
    LIBS += -lcrypto -lssl

    # libcurl
    LIBS += -lcurl
}

#SyncBlock
contains(DEFINES, USE_SYNCBLOCK) {
    LIBS += -L$$PWD/platforms/linux-gen3/interface/syncBlock/lib/ -lhmibase_gadget_syncblock2_dlt_so
    LIBS += -L$$PWD/platforms/linux-gen3/interface/syncBlock/lib/ -lhmibase_gadget_videobuffer_dlt_so
    LIBS += -L$$PWD/platforms/linux-gen3/interface/syncBlock/lib/ -lhmibase_util_dlt_so
    LIBS += -L$$PWD/platforms/linux-gen3/interface/syncBlock/lib/ -lutility
    LIBS += -L$$PWD/platforms/linux-gen3/interface/syncBlock/lib/ -lapx

    INCLUDEPATH += $$PWD/platforms/linux-gen3/interface/syncBlock/include

    HEADERS += platforms/linux-gen3/interface/syncBlock/SimpleProducer.h \
               platforms/linux-gen3/interface/syncBlock/producer.h \
               platforms/linux-gen3/interface/syncBlock/consumer.h

    SOURCES +=  platforms/linux-gen3/interface/syncBlock/SimpleProducer.cpp \
                platforms/linux-gen3/interface/syncBlock/producer.cpp \
                platforms/linux-gen3/interface/syncBlock/consumer.cpp
}

contains(DEFINES, USE_MIC) {
#    LIBS += -L$$PWD/platforms/linux-gen3/interface/mic/lib/ -lgstreamer-0.10
#    LIBS += -L$$PWD/platforms/linux-gen3/interface/mic/lib/ -lglib-2.0
#    LIBS += -L$$PWD/platforms/linux-gen3/interface/mic/lib/ -lxml2
#    LIBS += -L$$PWD/platforms/linux-gen3/interface/mic/lib/ -lgstvideo-0.10
#    LIBS += -L$$PWD/platforms/linux-gen3/interface/mic/lib/ -lgobject-2.0
#    LIBS += -L$$PWD/platforms/linux-gen3/interface/mic/lib/ -lgstapp-0.10

#    INCLUDEPATH += $$PWD/platforms/linux-gen3/interface/mic/include/gstreamer-0.10
#    INCLUDEPATH += $$PWD/platforms/linux-gen3/interface/mic/include/glib-2.0
#    INCLUDEPATH += $$PWD/platforms/linux-gen3/interface/mic/include/glib-2.0/glib
#    INCLUDEPATH += $$PWD/platforms/linux-gen3/interface/mic/include/glib-2.0/gobject
#    INCLUDEPATH += $$PWD/platforms/linux-gen3/interface/mic/include/glib-2.0/include
#    INCLUDEPATH += $$PWD/platforms/linux-gen3/interface/mic/include/libxml2

    LIBS += -L$$[QT_SYSROOT]/usr/lib/ -lgstreamer-0.10 -lglib-2.0 -lxml2 -lgstvideo-0.10 -lgobject-2.0 -lgstapp-0.10

    INCLUDEPATH += $$[QT_SYSROOT]/usr/include/gstreamer-0.10
    INCLUDEPATH += $$[QT_SYSROOT]/usr/include/glib-2.0
    INCLUDEPATH += $$[QT_SYSROOT]/usr/include/glib-2.0/glib
    INCLUDEPATH += $$[QT_SYSROOT]/usr/include/glib-2.0/gobject
    INCLUDEPATH += $$[QT_SYSROOT]/usr/include/glib-2.0/include
    INCLUDEPATH += $$[QT_SYSROOT]/usr/include/libxml2

    HEADERS += platforms/linux-gen3/interface/mic/videostream.h
    SOURCES += platforms/linux-gen3/interface/mic/videostream.cpp

# make links with so names dependent on their libs
mic_libs = $$files($$_PRO_FILE_PWD_/platforms/linux-gen3/interface/mic/lib/*.so.*.*)
 for (so, mic_libs) {
     fn = $$replace(so,(?:.+\/),"")
     linkeed_so = $$replace(fn, .so.*, .so)
     linkeed_so_0 = $$replace(fn, .so.*, .so.0)
    system("ln -sf $$fn $$_PRO_FILE_PWD_/platforms/linux-gen3/interface/mic/lib/$$linkeed_so")
    system("ln -sf $$fn $$_PRO_FILE_PWD_/platforms/linux-gen3/interface/mic/lib/$$linkeed_so_0")
 }
}

#ALSA
contains(DEFINES, USE_ALSA) {
SOURCES += platforms/linux-gen3/interface/ALSA/ALSAPlayer.cpp \
    platforms/linux-gen3/interface/ALSA/AudioManagerInterface.cpp \
    platforms/linux-gen3/interface/ALSA/ThirdPartyPlayNavigationVoice.cpp \
    platforms/linux-gen3/interface/ALSA/ThirdPartyVoiceIF.cpp \
    platforms/linux-gen3/interface/ALSA/WaveFilePlayer.cpp \
    platforms/linux-gen3/interface/ALSA/WaveFileReader.cpp

HEADERS += platforms/linux-gen3/interface/ALSA/ALSAPlayer.h \
    platforms/linux-gen3/interface/ALSA/AudioManagerInterface.h \
    platforms/linux-gen3/interface/ALSA/EventWaiter.h \
    platforms/linux-gen3/interface/ALSA/Singleton.h \
    platforms/linux-gen3/interface/ALSA/ThirdPartyPlayNavigationVoice.h \
    platforms/linux-gen3/interface/ALSA/ThirdPartyVoiceIF.h \
    platforms/linux-gen3/interface/ALSA/ThreadSync.h \
    platforms/linux-gen3/interface/ALSA/use_boost_header_only.hpp \
    platforms/linux-gen3/interface/ALSA/WaveFilePlayer.h \
    platforms/linux-gen3/interface/ALSA/WaveFileReader.h

# make links with so names dependent on their libs
#alsa_libs=$$files($$_PRO_FILE_PWD_/platforms/linux-gen3/interface/ALSA/lib/*.so.*.*)
# for (so, alsa_libs) {
#     fn = $$replace(so,(?:.+\/),"")
#     linkeed_so = $$replace(fn, .so.*, .so)
#     linkeed_so_0 = $$replace(fn, .so.*, .so.0)
#    system("ln -sf $$fn $$_PRO_FILE_PWD_/platforms/linux-gen3/interface/ALSA/lib/$$linkeed_so")
#    system("ln -sf $$fn $$_PRO_FILE_PWD_/platforms/linux-gen3/interface/ALSA/lib/$$linkeed_so_0")
# }

#LIBS += -L$$PWD/platforms/linux-gen3/interface/ALSA/lib/ -lasound
LIBS += -lasound
INCLUDEPATH += $$$$PWD/platforms/linux-gen3/interface/ALSA/include

#LIBS += -L$$PWD/platforms/linux-gen3/interface/ALSA/lib/ -lboost_system
LIBS += -lboost_system
#LIBS += -L$$PWD/platforms/linux-gen3/interface/ALSA/lib/ -lboost_date_time
LIBS += -lboost_date_time
INCLUDEPATH += $$PWD/platforms/linux-gen3/interface/ALSA/
}

