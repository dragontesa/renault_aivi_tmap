import QtQuick 2.0
import "../../view"
import "../../GlobalConstants.js" as GlobalConstants
import "../../singleton"
import PoiTabViewEvent 1.0

PopupView {
    id: root

    modal: true

    anchors { fill: parent; bottomMargin: GlobalConstants.kScreenHardKeyHeight }

    property bool isRg: false
    property var menus
    readonly property int itemHeight: 60
    readonly property int itemPadding: 30
    readonly property int listViewSpacing: 20
    readonly property int titleHeight: 90
    readonly property int cancelButtonHeight: 60

    property int id
    property int wX
    property int wY
    property string name
    property string name2
    property string contents

    onMediated: {

    }
    onActivated: {
        sendEvent(PoiTabViewEvent.Activated)
    }

    onMenusChanged: {
        var menuCount = menus.length
        var h = menuCount * itemHeight + listViewSpacing * (menuCount - 1) + 30

        listView.height = h
    }

    Component {
        id: itemButton
        Image {
            anchors { left: parent.left; leftMargin: itemPadding; right: parent.right; rightMargin: itemPadding }
            height: itemHeight
            source: Resource.imageUrl('popup_btn_menu.png')

            Rectangle {
                anchors { fill: parent; margins: 1 }
                color: menuButtonMouseArea.pressed ? MultiSenseCoordinator.kAmbientColor : 'transparent'
                opacity: menuButtonMouseArea.pressed ? 0.3 : 0.0
            }
            Text {
                anchors.fill: parent
                verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter
                font.family: GlobalConstants.kFontFamily
                font.pixelSize: Resource.pixelToDp(25)
                color: '#ffffff'
                text: modelData
                scale: menuButtonMouseArea.pressed ? 0.85 : 1.0
            }
            MouseArea {
                id: menuButtonMouseArea
                anchors.fill: parent
                onClicked: {
                    var poiID = id * 100
                    var params = {
                        'poiID' : poiID,
                        'name' : name,
                        'x' : wX,
                        'y' : wY
                    }
                    if (0 < name2.length) {
                        params['name'] = name + ' ' + name2
                    }

                    switch (modelData) {
                    case qsTr('경유지로'):
                        sendEvent(PoiTabViewEvent.AsViaClicked, params)
                        break
                    case qsTr('목적지로'):
                        sendEvent(PoiTabViewEvent.AsDestinationClicked, params)
                        break
                    case qsTr('즐겨찾기 추가'):
                        sendEvent(PoiTabViewEvent.AsBookmarkClicked, params)
                        break
                    }
                }
            }
        }
    }

    Rectangle { anchors.fill: column; color: '#222226'; border.color: '#a1a2b3' }

    Column {
        id: column
        width: 520; height: titleHeight + listView.height + cancelButtonHeight
        anchors.centerIn: parent
        Item {
            width: parent.width; height: titleHeight
            Text {
                y: 30
                width: parent.width; height: 30
                verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter
                font.family: GlobalConstants.kFontFamily
                font.pixelSize: Resource.pixelToDp(32)
                color: '#ffffff'
                text: name2.length == 0 ? name : name + ' ' + name2
            }
        }
        ListView {
            id: listView
            width: parent.width;

            spacing: listViewSpacing
            model: menus
            delegate: itemButton
            interactive: false
        }
        Item {
            width: parent.width; height: cancelButtonHeight
            Rectangle {
                id: cancelButton
                anchors.fill: parent
                color: cancelMouseArea.pressed ? MultiSenseCoordinator.kAmbientColor : '#ffffff'
                opacity: cancelMouseArea.pressed ? 0.3 : 0.05
                MouseArea {
                    id: cancelMouseArea
                    anchors.fill: parent
                    onClicked: sendEvent(PoiTabViewEvent.CancelButtonClicked)
                }
            }
            Text {
                anchors.fill: cancelButton
                verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter
                font.family: GlobalConstants.kFontFamily
                font.pixelSize: Resource.pixelToDp(32)
                color: 'white'
                text: qsTr('취소')
                scale: cancelMouseArea.pressed ? 0.85 : 1.0
            }
            Rectangle {
                width: parent.width; height: 1
                color: '#a1a2b3'
            }
        }
    }
}
