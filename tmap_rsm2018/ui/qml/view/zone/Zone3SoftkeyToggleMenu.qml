import QtQuick 2.0
import "../../GlobalConstants.js" as GlobalConstants
import "../../component"
import '../../view/util'

TFocusItem {
    id: root
    property var layout
    property var menus: []
    property int currentIndex: 0
    property bool isIndexMode: false
    property bool enableMenu0: (menus !== null && menus !== undefined && menus.length > 0 && (menus[0].indexOf(':disabled') <0))
    property bool enableMenu1: (menus !== null && menus !== undefined && menus.length > 1 && (menus[1].indexOf(':disabled') <0))

    signal clicked(int index, string menuName)

    width: layout.toolMenuWidth; height: layout.toolMenuHeight
    anchors.centerIn: parent

    function getMenuText(menu, index) {
        var ret = ''
        if (menu !== undefined && menu !== null && menu.length > index) {
            var menuText = menu[index]
            var n = menuText.indexOf(":disabled")
            if (n >= 0) {
                ret = menuText.substring(0, n)
            } else {
                ret = menuText
            }
        }
        return ret
    }

    function selectCurrentMenu() {
        root.clicked(currentIndex, menus[currentIndex])
        if (currentIndex === 0) {
            currentIndex = 1
        } else {
            currentIndex = 0
        }
    }

    Image {
        id: background
        anchors.centerIn: parent
        source: {
            if (0 == currentIndex)
                return Resource.imageUrl("toggle_left.png")
            else
                return Resource.imageUrl("toggle_right.png")
        }
    }
    Text2 {
        anchors { fill: parent; leftMargin: 5; rightMargin: 5+151 }
        verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter
        text: getMenuText(menus, 0)
        enabled: enableMenu0
        opacity: enabled ? 1.0 : 0.3
        font {
            family: GlobalConstants.kFontFamilySKP
            pixelSize: Resource.pixelToDp(35)
        }
        offsetY: 2
        color: currentIndex == 0 ? "#222226" : "#ffffff"
        MouseArea {
            anchors.fill: parent
            onClicked: {
                if (enabled) {
                    currentIndex = 0
                    root.clicked(currentIndex, menus[0])
                }
            }
        }
    }
    Text2 {
        anchors{ fill: parent; leftMargin: 5+151; rightMargin: 5 }
        verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter
        text: getMenuText(menus, 1)
        enabled: enableMenu1
        color: currentIndex == 1 ? "#222226" : "#ffffff"
        opacity: enabled ? 1.0 : 0.3
        font {
            family: GlobalConstants.kFontFamilySKP
            pixelSize: Resource.pixelToDp(35)
        }
        offsetY: 2
        MouseArea {
            anchors.fill: parent
            onClicked: {
                if (enabled) {
                    currentIndex = 1
                    root.clicked(currentIndex, menus[1])
                }
            }
        }
    }
}
