import QtQuick 2.0
import "../../GlobalConstants.js" as GlobalConstants

Item {
    id: root

    property alias mousePressed: mouseArea.pressed

    signal realPressed()
    signal pressed()
    signal released()
    signal clicked()

    Timer {
        id: timer
        interval: GlobalConstants.kLongPressedColorExposureTime
        onTriggered: {
            root.pressed()
        }
    }

    MouseArea {
        id: mouseArea

        anchors.fill: parent
        onPressed: {
            timer.start()
            root.realPressed()
        }
        onReleased: {
            timer.stop()
            root.released()
        }
        onClicked: {
            root.clicked()
        }
    }
}
