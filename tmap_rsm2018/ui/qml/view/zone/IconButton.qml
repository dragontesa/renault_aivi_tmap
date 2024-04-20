import QtQuick 2.0
import "../../component"
import "../../singleton"
import "../../GlobalConstants.js" as GlobalConstants

TFocusItem {
    id: root

    property alias iconSource: icon.source
    property alias iconOpacity: icon.opacity
    property alias backgroundOpacity: iconBackground.opacity
    property color pressedColor: MultiSenseCoordinator.kAmbientColor
    property color focusedColor: GlobalConstants.kFocusedColor
    property bool focused: false

    signal clicked()

    state: {
        if (mouseArea.pressed) {
            return 'pressed'
        } else {
            return focused ? 'focused' : 'normal'
        }
    }

    states: [
        State {
            name: 'pressed'
            PropertyChanges {
                target: iconBackground
                color: MultiSenseCoordinator.kAmbientColor
                opacity: 0.3
            }
        },
        State {
            name: 'normal'
            PropertyChanges {
                target: iconBackground
                color: 'transparent'
            }
        },
        State {
            name: 'focused'
            PropertyChanges {
                target: iconBackground
                color: focusedColor
            }
        }
    ]

    Rectangle { id: iconBackground; anchors.fill: parent }
    MouseArea {
        id: mouseArea

        anchors.fill: iconBackground

        onClicked: root.clicked()
    }

    Image {
        id: icon
        anchors.fill: parent
        fillMode: Image.PreserveAspectFit
        scale: root.state === 'pressed' ? 0.85 : 1.0
    }
}
