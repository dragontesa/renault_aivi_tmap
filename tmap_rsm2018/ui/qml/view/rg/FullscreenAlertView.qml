import QtQuick 2.1
import Constants 1.0

import "../../component"
import "../../view"
import FullscreenAlertViewEvent 1.0
import PreciseTimer 1.0

PopupView {
    id: root

    property color alertColor: 'red'
    property int blinkInterval: 1750
    property bool appRunning: true
    property bool frameVisible: false

    width: parent ? parent.width : undefined
    height: parent ? parent.height : undefined
    hasFocus: false

    onMediated: {
        sendEvent(FullscreenAlertViewEvent.Mediated)
    }

    onVisibleChanged: {
        timer.running = visible
    }

    Component.onCompleted: {
    }

    onAppRunningChanged: {
        if ( appRunning ) {
            timer.running = true
        }
        else {
            timer.running = false
        }
    }

    Rectangle {
        id: frame
        anchors.fill: parent
        color: alertColor
        opacity: 0.5
        visible: frameVisible
    }

    PreciseTimer {
        id: timer
        interval: blinkInterval
        running: true
        property bool turnOn: false
        triggeredOnStart: true

        onTriggered: {
            turnOn = !turnOn
            if ( turnOn ) {
                frame.color = alertColor
                frameVisible = true
            }
            else {
                frame.color = 'transparent'
                frameVisible = false
            }
        }
    }
}
