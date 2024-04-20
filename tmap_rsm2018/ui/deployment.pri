android-no-sdk {
    target.path = /data/user/qt
    export(target.path)
    INSTALLS += target
} else:android {
    x86 {
        target.path = /libs/x86
    } else: armeabi-v7a {
        target.path = /libs/armeabi-v7a
    } else {
        target.path = /libs/armeabi
    }
    export(target.path)
    INSTALLS += target
} else:linux-gen3-g++ {
    target.path = /opt/bosch/navigation_kor/lib
    !isEmpty(RSM2018_TARGET_PREFIX): target.path = $${RSM2018_TARGET_PREFIX}/lib
    export(target.path)
    INSTALLS += target

    res.path = /opt/bosch/navigation_kor
    !isEmpty(RSM2018_TARGET_PREFIX): res.path = $${RSM2018_TARGET_PREFIX}
    res.files = $${RSM2018_RESDIR}
    INSTALLS += res
} else:jlr2019 {
    target.path = /var/nav/lib
    export(target.path)
    INSTALLS += target

    res.path = /var/nav
    res.files = $${RSM2018_RESDIR}
    INSTALLS += res
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

#    res.path = /home/thirdpartydev/tmap_rsm2018/res
#    CONFIG(debug,debug|release){
#        res.files = $$files($$PWD/res/*)
#    }
#    else{
#        res.files = $$files($$PWD/res/*)
#    }

#    export(res)
#    INSTALLS += res
}

export(INSTALLS)
