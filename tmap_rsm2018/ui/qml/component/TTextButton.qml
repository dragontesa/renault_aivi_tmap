import QtQuick 2.0
import "../singleton"
import '../view/util'

Item {
    id: root

    property alias text: text
    property alias label: text.text
    property alias fontPixelSize: text.font.pixelSize
    property alias radius: bg.radius
    property bool focused: false
    property bool parentFocused: false
    property bool parentPressed: false
    property alias mouseAreaEnabled: mouseArea.enabled

    signal clicked()

    state: 'normal'
    states: [
        State {
            name: 'normal'
            PropertyChanges {
                target: bg
                color: focused ? '#ffffff' : (parentFocused ? '#000000' : 'transparent')
            }
            PropertyChanges {
                target: root
                scale: 1.0
            }
        },
        State {
            name: 'pressed'
            PropertyChanges {
                target: bg
                color: {
                    if (focused) {
                        return '#ffffff'
                    } else {
                        if (parentPressed) {
                            return 'transparent'
                        } else {
                            return parentFocused ? '#000000' : MultiSenseCoordinator.kAmbientColor
                        }
                    }
                }
            }
            PropertyChanges {
                target: root
                scale: 0.85
            }
        }
    ]
    Rectangle {
        id: bg
        anchors.fill: parent
        color: 'transparent'
    }
    Rectangle {
        id: outline
        anchors.fill: parent
        color: 'transparent'
        border.width: 2
        border.color: focused ? '#ffffff' : MultiSenseCoordinator.kAmbientColor
        radius: bg.radius
    }

    Text2 {
        id: text
        anchors.fill: parent
        color: (focused) ? '#000000' : '#ffffff'
        verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter
        offsetY: 3
    }
    MouseArea {
        id: mouseArea

        anchors.fill: parent
        onPressedChanged: {
            if (pressed) {
                root.state = 'pressed'; focused = false; parentFocused = false
            } else {
                root.state = 'normal'
            }
        }

        onClicked: root.clicked()
    }
}
