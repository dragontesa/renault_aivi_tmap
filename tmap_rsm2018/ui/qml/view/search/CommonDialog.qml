import QtQuick 2.0
import Csw 1.0
import "../../GlobalConstants.js" as GlobalConstants
import "../../singleton"

Item {
    id: root

    property string title
    property var menus: ['확인']
    property alias content: contentWrap.children
    property int currentIndex: 0
    property bool cswEnable: false
    property alias csw: csw

    signal accepted()
    signal rejected()
    signal menuClicked(string menu)

    Csw {
        id: csw
        section: Csw.Section_Dialog
        sequence: 0
        onFocused: {
            cswEnable = true
            currentIndex = 0
        }
        onLostFocusd: {
            cswEnable = false
        }

        onSpinCcwPressed: {
            if (0 < currentIndex)
                currentIndex -= 1
            else
                currentIndex = menus.length - 1
        }
        onSpinCwPressed: {
            if (currentIndex < menus.length - 1)
                currentIndex += 1
            else
                currentIndex = 0
        }
        onSelected: {
            var menu = menus[currentIndex]
            menuClicked(menu)
        }
    }

    Rectangle {
        anchors.fill: parent
        color: '#222226'; border.color: '#a1a2b3'
        Text {
            id: titleText
            anchors {
                top: parent.top; topMargin: 30;
                left: parent.left; leftMargin: 30;
                right: parent.right; rightMargin: 30 }
            height: 30
            font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(32) }
            text: title
            color: '#ffffff'
            verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter
        }

        Item {
            id: contentWrap
            width: parent.width;
            anchors { top: parent.top; topMargin: 90; bottom: parent.bottom; bottomMargin: 60 }
        }

        ListView {
            id: buttons
            width: parent.width; height: 60;
            anchors { bottom: parent.bottom; bottomMargin: 1 }
            model: menus
            orientation: ListView.Horizontal
            interactive: false

            delegate: Item {
                id: menuItem
                width : root.width / menus.length; height: 60
                state: 'normal'
                states: [
                    State {
                        name: 'normal'
                        PropertyChanges {
                            target: bg
                            color: '#ffffff'
                            opacity: (cswEnable && currentIndex == index) ? 1.0 : 0.05
                        }
                    },
                    State {
                        name: 'pressed'
                        PropertyChanges {
                            target: bg
                            color: MultiSenseCoordinator.kAmbientColor
                            opacity: 0.3
                        }
                    }
                ]
                Rectangle {
                    id: bg
                    anchors.fill: parent
                }
                Text {
                    anchors{ centerIn: parent }
                    width: contentWidth; height: contentHeight;
                    font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(32) }
                    color: (cswEnable && currentIndex === index) ? '#000000' : '#ffffff'
                    text: modelData
                    scale: menuItem.state === 'pressed' ? 0.85 : 1.0
                }
                Rectangle { width: 1; height: parent.height; anchors.right: parent.right;
                    color: '#a1a2b3'; visible: index != menus.length-1 }
                MouseArea {
                    anchors.fill: parent
                    onPressed: menuItem.state = 'pressed'
                    onReleased: menuItem.state = 'normal'
                    onClicked: {
                        if (modelData == qsTr('확인') || modelData == 'OK') {
                            root.accepted()
                        } else if (modelData == qsTr('취소') || modelData == 'Cancel') {
                            root.rejected()
                        } else {
                            root.menuClicked(modelData)
                        }
                    }
                }
            }// delegate
            Rectangle { width: parent.width; height: 1; anchors.top: parent.top; color: '#a1a2b3' }
        }

    }
}
