# Default rules for deployment.
linux-gen3-g++ {
    target.path = /opt/bosch/navigation_kor/lib
    !isEmpty(RSM2018_TARGET_PREFIX): target.path = $${RSM2018_TARGET_PREFIX}/lib
    export(target.path)
    INSTALLS += target
} else:jlr2019 {
    target.path = /var/nav/lib
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

export(INSTALLS)

