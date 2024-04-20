import QtQuick 2.0
import HardKeyViewEvent 1.0
import "../../component"
import "../../view"
import "../../GlobalConstants.js" as GlobalConstants

View {
    focusName: "HardKey"
    width: parent? parent.width : undefined
    height: GlobalConstants.kScreenHardKeyHeight
    anchors.bottom: parent ? parent.bottom : undefined

    property alias cswDebugInfoText: cswDebugInfo.text
    property int bufferSize: 0
    visible: true

    onMediated: { sendEvent(HardKeyViewEvent.Mediated) }

    Rectangle {
        id: rect
        anchors.fill: parent
        color: "black"
        TPushButton {
            id: recentTileButton
            focusName: "RecentTileButton"
            hasFocus: false
            label: "∷"
            width: layout.buttonWidth
            height: layout.buttonHeight
            radius: layout.buttonRadius
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            onButtonClicked: {
                sendEvent( HardKeyViewEvent.RecentTileButtonClicked )
            }
        }
        Text {
            id: cswDebugInfo
            anchors.right: recentTileButton.right
            anchors.left: parent.left
            height: 16
            anchors.verticalCenter: parent.verticalCenter
            color: 'white'
        }

        property int buttonWidth: parent.height / 2 - 8
        property int buttonHeight: parent.height / 2 - 8
        Row {
            anchors { left: recentTileButton.right; leftMargin: 16; right: parent.right
                verticalCenter: parent.verticalCenter
            }
            height: rect.buttonHeight
            spacing: 2
            Rectangle {
                width: rect.buttonWidth; height: rect.buttonHeight
                color: 'yellow'
                Text { text: 'L'; anchors.centerIn: parent }
                MouseArea { anchors.fill: parent;
                    onClicked: sendEvent(HardKeyViewEvent.CswLeftClicked)
                }
            }
            Rectangle {
                width: rect.buttonWidth; height: rect.buttonHeight
                color: 'yellow'
                Text { text: 'R'; anchors.centerIn: parent }
                MouseArea { anchors.fill: parent;
                    onClicked: sendEvent(HardKeyViewEvent.CswRightClicked)
                }
            }
            Rectangle {
                width: rect.buttonWidth; height: rect.buttonHeight
                color: 'yellow'
                Text { text: 'U'; anchors.centerIn: parent }
                MouseArea { anchors.fill: parent;
                    onClicked: sendEvent(HardKeyViewEvent.CswUpClicked)
                }
            }
            Rectangle {
                width: rect.buttonWidth; height: rect.buttonHeight
                color: 'yellow'
                Text { text: 'D'; anchors.centerIn: parent }
                MouseArea { anchors.fill: parent;
                    onClicked: sendEvent(HardKeyViewEvent.CswDownClicked)
                }
            }
            Rectangle {
                width: rect.buttonWidth; height: rect.buttonHeight
                color: 'yellow'
                Text { text: 'Ccw'; anchors.centerIn: parent }
                MouseArea { anchors.fill: parent;
                    onClicked: sendEvent(HardKeyViewEvent.CswSpinCcwClicked)
                }
            }
            Rectangle {
                width: rect.buttonWidth; height: rect.buttonHeight
                color: 'yellow'
                Text { text: 'Cw'; anchors.centerIn: parent }
                MouseArea { anchors.fill: parent;
                    onClicked: sendEvent(HardKeyViewEvent.CswSpinCwClicked)
                }
            }
            Rectangle {
                width: rect.buttonWidth; height: rect.buttonHeight
                color: 'orange'
                Text { text: 'OK'; anchors.centerIn: parent }
                MouseArea { anchors.fill: parent;
                    onClicked: sendEvent(HardKeyViewEvent.CswOkClicked)
                }
            }
            Rectangle {
                width: rect.buttonWidth; height: rect.buttonHeight
                color: 'orange'
                Text { text: '<-'; anchors.centerIn: parent }
                MouseArea { anchors.fill: parent;
                    onClicked: sendEvent(HardKeyViewEvent.CswBackClicked)
                }
            }
            Rectangle {
                width: rect.buttonWidth; height: rect.buttonHeight
                color: 'orange'
                Text { text: '+'; anchors.centerIn: parent }
                MouseArea { anchors.fill: parent;
                    onClicked: sendEvent(HardKeyViewEvent.CswOptionClicked)
                }
            }
        }
    }
    Text {
        color: 'yellow'
        text: 'buffer:' + bufferSize
    }
}
