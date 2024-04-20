import QtQuick 2.0
import "../../GlobalConstants.js" as GlobalConstants
import "../../component"
import "../../singleton"
import '../../view/util'

TFocusItem {
    id: root

    property alias buttonText: label.text
    property bool enabled: true
    property bool selected: false
    property int sequence: -1
    property int index: -1
    height: 83

    signal clicked()
    signal clicked2(int index)

    Rectangle {
        id: buttonBackground
        anchors.fill: parent
    }
    Text2 {
        id: label
        width: parent.width - 48; height: 28
        anchors.centerIn: parent
        horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter
        font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(29) }
        color: root.enabled ? "#ffffff" : "#444444"
        scale: root.state === 'pressed' ? 0.85 : 1.0
        offsetY: 2
    }
    TMouseArea {
        id: mouseArea
        anchors.fill: parent
        onPressed: root.state = 'pressed'
        onReleased: root.state = 'normal'
        onClicked: {
            if (root.enabled) {
                if (index < 0) {
                    root.clicked()
                } else {
                    root.clicked2(index)
                }
            }
        }
    }
    Rectangle {
        id: tab
        width: parent.width
        height: 5
        anchors { bottom: buttonBackground.bottom }
        color: root.state === 'focused' ? '#ffffff' : MultiSenseCoordinator.kAmbientColor
        visible: selected
    }

    state: 'normal'
    states: [
        State {
            name: 'pressed'
            PropertyChanges {
                target: buttonBackground
                color: root.enabled ? MultiSenseCoordinator.kAmbientColor : GlobalConstants.kNormalColor
                opacity: root.enabled ? 0.3 : 1.0
            }
            PropertyChanges {
                target: label
                color: root.enabled ? "#ffffff" : "#444444"
            }
        },
        State {
            name: 'normal'
            PropertyChanges {
                target: buttonBackground
                color: selected ? MultiSenseCoordinator.kAmbientColor : GlobalConstants.kNormalColor
                opacity: selected ? 0.3 : 1.0
            }
            PropertyChanges {
                target: label
                color: root.enabled ? "#ffffff" : "#444444"
            }
        },
        State {
            name: 'focused'
            PropertyChanges {
                target: buttonBackground
                color: "#ffffff"
            }
            PropertyChanges {
                target: label
                color: "#000000"
            }
        }
    ]
}
