import QtQuick 2.0
import RgSignalState 1.0

Item {
    id: root
    width: childrenRect.width
    height: childrenRect.height
    property int blinkInterval: 1000 / 2
    property int signalState: RgSignalState.NONE

    onSignalStateChanged: {
        if ( signalState == RgSignalState.OFF || signalState == RgSignalState.BAD ) {
            icon.visible = true
            timer.running = true
        }
        else {
            icon.visible = false
            timer.running = false
        }
    }

    Image {
        id: icon
        source: Resource.imageUrl( "map", "map_icon_gps.png")
        visible: false
    }

    Timer {
        id: timer
        interval: blinkInterval
        running: false
        repeat: true
        onTriggered: {
            icon.visible = !icon.visible
        }
    }
}
