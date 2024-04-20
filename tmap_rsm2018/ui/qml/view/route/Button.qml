import QtQuick 2.0
import "../../singleton"
import "../../GlobalConstants.js" as GConstants
import '../../view/util'

Item {
    id: root

    property alias text: label.text
    signal clicked
    signal pressed
    signal released

    scale: state === 'pressed' ? 0.85 : 1.0

    state: 'normal'
    states: [
        State {
            name: 'normal'
            PropertyChanges {
                target: label
                color: "#ffffff"
            }
            PropertyChanges {
                target: button
                color: 'transparent'
                border.color: MultiSenseCoordinator.kAmbientColor
            }
        },
        State {
            name: "pressed"
            PropertyChanges {
                target: label
                color: "#ffffff"
            }
            PropertyChanges {
                target: button
                color: MultiSenseCoordinator.kAmbientColor
                border.color: 'transparent'
            }
        },
        State {
            name: 'lpressed'
            PropertyChanges {
                target: label
                color: "#ffffff"
            }
            PropertyChanges {
                target: button
                color: 'transparent'
                border.color: '#ffffff'
            }
        },
        State {
            name: "focused"
            PropertyChanges {
                target: label
                color: "#000000"
            }
            PropertyChanges {
                target: button
                color: '#ffffff'
                border.color: 'transparent'
            }
        },
        State {
            name: "otherFocused"
            PropertyChanges {
                target: label
                color: "#000000"
            }
            PropertyChanges {
                target: button
                color: '#ffffff'
                border.color: '#000000'
            }
        }
    ]

    Rectangle {
        id: button
        width: 88; height: 58; radius: 29; border.width: 2
        opacity: (!root.enabled) ? 0.3 : 1.0
    }
    Text2 {
        id: label
        width: 88; height: 28
        anchors.centerIn: button
        font { family: GConstants.kFontFamily; pixelSize: Resource.pixelToDp(30) }//35 }//pixelSize: 35 }
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        color: "#ffffff"
        opacity: (!root.enabled) ? 0.3 : 1.0
        offsetY: 3
    }

    MouseArea {
        anchors.fill: button

        onPressed: root.pressed()
        onReleased: root.released()
        onClicked: root.clicked()
    }
}
