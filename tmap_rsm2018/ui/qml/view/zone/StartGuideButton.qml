import QtQuick 2.0
import "../../GlobalConstants.js" as GlobalConstants
import "../../component"
import "../../singleton"
import Csw 1.0
import '../../view/util'

Item {
    id: root

    property int remainTime: 10
    property var csw: csw

    signal clicked()

    width: 240
    height: parent.height


    Rectangle {
        id: bg

        anchors.fill: parent
        color: MultiSenseCoordinator.kAmbientColor
        opacity: root.state == 'pressed' ? 0.3 : 1.0
        Text2 {
            id: label
            width: 106; height: 34; anchors.centerIn: parent
            verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter
            scale: root.state === 'pressed' ? 0.85 : 1.0
            text: qsTr("안내시작")
            color: "white"
            font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(35) }//pixelSize: 24 }
            offsetY: 2
        }
        TRingCountDown {
            id: ringCountDown

            width: GlobalConstants.kScreenHeight === 480 ? 24 : 28 //32 : 48
            height: width

            anchors { left: label.right; leftMargin: 4; verticalCenter: label.verticalCenter }

            onTimeout: {
                root.clicked()
            }
        }
    }
    Component.onCompleted: {
        ringCountDown.start()
    }
    state: "normal"
    states: [
        State {
            name: "normal"
            PropertyChanges {
                target: bg
                color: MultiSenseCoordinator.kAmbientColor
            }
            PropertyChanges {
                target: label
                color: "#ffffff"
            }
            PropertyChanges {
                target: ringCountDown
                color: "#ffffff"
            }
        },
        State {
            name: "pressed"
            PropertyChanges {
                target: bg
                color: MultiSenseCoordinator.kAmbientColor
            }
            PropertyChanges {
                target: label
                color: "#ffffff"
            }
            PropertyChanges {
                target: ringCountDown
                color: "#ffffff"
            }
        },
        State {
            name: "focused"
            PropertyChanges {
                target: bg
                color: "#ffffff"
            }
            PropertyChanges {
                target: label
                color: "#000000"
            }
            PropertyChanges {
                target: ringCountDown
                color: "#000000"
            }
        }
    ]

    MouseArea {
        anchors.fill: parent
        onPressed: root.state = "pressed"
        onReleased: root.state = "normal"
        onClicked: {
            root.clicked()
            csw.forceLostFocus()
        }
    }
    Csw {
        id: csw
        section: Csw.Section_Softkey
        onFocused: {
            root.state = "focused"
        }
        onLostFocusd: {
            root.state = "normal"
        }
    }
}
