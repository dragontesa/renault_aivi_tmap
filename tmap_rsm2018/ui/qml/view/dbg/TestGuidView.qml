import QtQuick 2.0

import "../../view"

PopupView {
    id: root

    anchors.fill: parent

    property int unitSize: 20
    property bool showGuideLine: false
    property bool horizontal: false
    property int value0: 0
    property int value1: 0
    property int step: 0

    Column {
        id: grid

        Repeater {
            id: col

            model: root.height / unitSize

            Row {
                id: row

                property int rowIndex: index

                Repeater {
                    model: root.width / unitSize

                    Rectangle {
                        width: unitSize
                        height: unitSize

                        color: (row.rowIndex + index) % 2 === 0 ? "white" : "transparency"
                        opacity: 0.1

                        Component.onCompleted: console.log(row.rowIndex + "," + index)
                    }
                }
            }
        }
    }

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        onDoubleClicked: {
            if (mouse.buttons & Qt.RightButton) {
                grid.visible = !grid.visible
            } else {
                horizontal = !horizontal
                step = 0
                line0.x = line0.y = line1.x = line1.y = 0
            }
        }
        onPressed: {
            if (mouse.button & Qt.RightButton)
                return

            switch (step) {
            case 0: line0.visible = true; step = 1
                break
            case 1: line1.visible = true; step = 2;
                break
            case 2: line0.visible = false; line1.visible = false; step = 3
                break
            case 3: step = 0
                line0.x = line0.y = line1.x = line1.y = 0
                break
            }
        }
        onMouseXChanged: {
            if (!horizontal) {
                switch (step) {
                case 1: line0.x = mouseX
                    break
                case 2: line1.x = mouseX
                    break
                }
            }
        }
        onMouseYChanged: {
            if (horizontal) {
                switch (step) {
                case 1: line0.y = mouseY
                    break
                case 2: line1.y = mouseY
                    break
                }
            }
        }
    }
    Rectangle {
        id: line0
        width: root.horizontal ? parent.width : 1
        height: root.horizontal ? 1 : parent.height
        color: "yellow"
        visible: false
    }
    Rectangle {
        id: line1
        width: root.horizontal ? parent.width : 1
        height: root.horizontal ? 1 : parent.height
        color: "orange"
        visible: false
    }
    Text {
        id: distanceText
        text: {
            switch (step) {
            case 0:
                return horizontal ? "horizontal" : "vertical"
            case 1:
                return horizontal ? line0.y : line0.x
            case 2:
            case 3:
                return horizontal ? (line1.y - line0.y) : (line1.x - line0.x)
            }
        }
        color: "white"
    }
}
