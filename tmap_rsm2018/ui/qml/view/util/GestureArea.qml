import QtQuick 2.0
import '../../GlobalConstants.js' as G

Item {
    id: root

    signal issued()

    MouseArea {
        id: mouseArea

        property real fromX: -1
        property bool isGestureMode: false

        anchors.fill: parent
        enabled: root.enabled
        propagateComposedEvents: true

        onPressed: {
            if (mouseY < G.kScreenZone3Y + G.kScreenZone3Height/2) {
                mouse.accepted = false
            } else {
                isGestureMode = true
                fromX = mouseX
            }
        }
        onReleased: {
            isGestureMode = false
        }

        onMouseYChanged: {
            if ( isGestureMode && mouseY < height/2) {
                var slope = Math.abs((mouseX-fromX) / mouseY);
                if ( slope < 0.5) {
                    root.issued()

                    isGestureMode = false
                }
            }
        }
    }
}
