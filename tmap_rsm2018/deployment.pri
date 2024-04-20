# Default rules for deployment.
linux-gen3-g++ {
    equals(TEMPLATE, lib) {
       target.path = /opt/bosch/navigation_kor/lib
       !isEmpty(RSM2018_TARGET_PREFIX): target.path = $${RSM2018_TARGET_PREFIX}/lib
    } else {
       target.path = /opt/bosch/extnav/bin
       !isEmpty(RSM2018_TARGET_PREFIX): target.path = $${RSM2018_TARGET_PREFIX}/bin
    }
    export(target.path)
    INSTALLS += target
} else:jlr2019 {
    target.path = /var/nav/bin
    export(target.path)
    INSTALLS += target
} else:unix {
    isEmpty(target.path) {
        qnx {
            target.path = /tmp/$${TARGET}/bin
        } else {
            target.path = /opt/$${TARGET}/bin
        }
        export(target.path)
    }
    INSTALLS += target
}

linux-gen3-g++ {
    mnssdk.path = /opt/bosch/navigation_kor/lib
    !isEmpty(RSM2018_TARGET_PREFIX): mnssdk.path = $${RSM2018_TARGET_PREFIX}/lib

    CONFIG(debug,debug|release){
       mnssdk.files = $$files(mnssdk/lib/linux-gen3/debug/libmnssdk.*)
    }
    else{
        mnssdk.files = $$files(mnssdk/lib/linux-gen3/release/libmnssdk.*)
    }

    export(mnssdk)

    # connman csmanager 3rd libs
#    connman.path = /opt/bosch/navigation_kor/lib
#    !isEmpty(RSM2018_TARGET_PREFIX): connman.path = $${RSM2018_TARGET_PREFIX}/lib
#    connman.files = $$files(platforms/linux-gen3/connman/lib/*)

#    export(connman)

    INSTALLS += mnssdk
} else:jlr2019 {
    mnssdk.path = /var/nav/lib
    CONFIG(debug,debug|release){
        mnssdk.files = $$files(mnssdk/lib/qnx-aarch64le/debug/libmnssdk.*)
    }
    else{
        mnssdk.files = $$files(mnssdk/lib/qnx-aarch64le/release/libmnssdk.*)
    }

    export(mnssdk)
    INSTALLS += mnssdk
} else:unix {
    mnssdk.path = $${target.path}
    CONFIG(debug,debug|release){
        mnssdk.files = $$files(mnssdk/lib/linux/debug/libmnssdk.*)
    }else{
        mnssdk.files = $$files(mnssdk/lib/linux/release/libmnssdk.*)
    }

    export(mnssdk)
    INSTALLS += mnssdk
}
export(INSTALLS)
