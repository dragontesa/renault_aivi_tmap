import QtQuick 2.0
import "../../GlobalConstants.js" as GlobalConstants
import "../../component"
import "../../singleton"

TFocusItem {
    id: root

    property alias text: label.text
    property alias font: label.font
    property alias color: label.color

    signal clicked()

    function setCswFocus() {
        label.state = "focused"
    }
    function setCswLostFocus() {
        label.state = "normal"
    }

    Rectangle {
        id: labelBackground

        anchors.fill: parent
    }

    Text {
        id: label

        anchors.fill: parent
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter

        TMouseArea {
            id: mouseArea

            anchors.fill: parent

            onClicked: root.clicked()
            onPressed: label.state = "pressed"
            onReleased: label.state = "normal"
        }

        state: "normal"
        onStateChanged: console.log("state = " + state)
        states: [
            State {
                name: "pressed"
                PropertyChanges {
                    target: labelBackground
                    color: MultiSenseCoordinator.kAmbientColor// GlobalConstants.kPressedColor
                    opacity: 0.3
                }
            },
            State {
                name: "normal"
                //when: !mouseArea.pressed
                PropertyChanges {
                    target: labelBackground
                    color: "transparent"
                }
            },
            State {
                name: "focused"
                PropertyChanges {
                    target: labelBackground
                    color: GlobalConstants.kFocusedColor
                }
            }
        ]
    }
}
