import QtQuick 2.0
import "../../component"
import "../../view"
import "../../singleton"
import "../../GlobalConstants.js" as GlobalConstants
import OptionMenuViewEvent 1.0
import Csw 1.0

PopupView {
    id: root
    property var menus
    property string currentViewName
    property string selectedMenu
    property int selectedIndex: -1
    property int hardKeyMargin: 0
    property bool indexMode: false

    anchors { fill: parent; bottomMargin: GlobalConstants.kScreenHardKeyHeight }

    signal clicked(string menuName)
    signal indexClicked(int index)

    function goBack() {
        selectedIndex = -1
        selectedMenu = ''
        exitAnimation.start()
    }

    modal: true

    onMediated: {
        sendEvent(OptionMenuViewEvent.Mediated)
    }
    onActivated: {
        sendEvent(OptionMenuViewEvent.Activated)
        sendEvent(OptionMenuViewEvent.RegistCSW, [csw])
    }
    onDeactivated: {
        sendEvent(OptionMenuViewEvent.Deactivated)
    }

    function getHeight(menus) {
        if (menus === null) {
            console.log('invalid menus')
            return 0
        }
        var menuCount = menus.length

        switch (menuCount) {
        case 1:
            return 86
        case 2:
            return 170
        case 3:
            return 254
        default:
            return 84 * menuCount + 1
        }
    }

    function closeOptionMenu(menuName, menuIndex) {
        if (indexMode) {
            root.indexClicked(menuIndex)
        } else {
            root.clicked(menuName)
        }

        root.close()

        if (indexMode) {
            sendEvent(OptionMenuViewEvent.SelectMenuIndex, {menu:menuName, index:menuIndex})
        } else {
            sendEvent(OptionMenuViewEvent.SelectMenu, menuName)
        }
    }

    function releaseOptionMenu(menuName, menuIndex) {
        if (indexMode) {
            sendEvent(OptionMenuViewEvent.ReleaseMenuIndex, {menu:menuName, index:menuIndex, name:currentViewName})
        } else {
            sendEvent(OptionMenuViewEvent.ReleaseMenu, {menu:menuName, name:currentViewName})
        }
    }

    Csw {
        id: csw
        section: Csw.Section_Dialog
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            selectedMenu = ''
            exitAnimation.start()
        }
    }

    Item {
        id: itemRoot
        width: 356; height: getHeight(menus)
        anchors { right: parent.right; rightMargin: 67; bottom: parent.bottom; bottomMargin: 36+root.hardKeyMargin }
        transformOrigin: Item.BottomRight

        Rectangle {
            id: menuBorder
            anchors.fill: parent
            color: 'transparent'; border.color: MultiSenseCoordinator.kAmbientColor
        }

        NumberAnimation {
            id: enterAnimation
            target: itemRoot
            property: "scale"
            duration: 250
            easing.type: Easing.InOutQuad
            from: 0.0; to: 1.0
            running: true
        }

        SequentialAnimation {
            id: exitAnimation

            ScriptAction { script: { releaseOptionMenu(selectedMenu, selectedIndex) } }
            NumberAnimation {
                target: itemRoot
                property: "scale"
                duration: 250; easing.type: Easing.InOutQuad
                from: 1.0; to: 0.0
            }
            ScriptAction { script: { closeOptionMenu(selectedMenu, selectedIndex) } }
        }

        Rectangle {
            anchors { fill: listView; leftMargin: 1; rightMargin: 1 }
            color: '#000000'; border.color: 'transparent'; opacity: 0.77
        }
        ListView {
            id: listView
            anchors{ fill: parent; topMargin: 2; bottomMargin: 2 }
            model: menus
            interactive: false
            delegate: Item {
                property bool isLast: (index !== 0 && index === menus.length-1)
                width: parent.width
                height: {
                    if (isLast) {
                        if (index === 0) {
                            return 86
                        } else {
                            return 84
                        }
                    } else {
                        return 83
                    }
                }
                Rectangle {
                    anchors{ fill: parent; leftMargin: 1; rightMargin: 1; topMargin: -1 }
                    color: MultiSenseCoordinator.kAmbientColor
                    opacity: 0.3
                    visible: menuMouseArea.pressed
                }

                Text {
                    anchors { left: parent.left; leftMargin: 30; right: parent.right; rightMargin: 30; verticalCenter: parent.verticalCenter }
                    height: 30
                    font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(32) }
                    verticalAlignment: Text.AlignVCenter
                    color: '#ffffff'
                    text: indexMode ? modelData.text : modelData
                }
                Rectangle {
                    visible: !isLast && listView.count > 1
                    anchors { left: parent.left; leftMargin: 10; right: parent.right; rightMargin: 10; bottom: parent.bottom }
                    height: 1
                    color: '#ffffff'; opacity: 0.5
                }
                MouseArea {
                    id: menuMouseArea
                    anchors.fill: parent
                    onClicked: {
                        if (indexMode) {
                            selectedIndex = modelData.id
                            selectedMenu = modelData.text
                        } else {
                            selectedMenu = modelData
                        }

                        exitAnimation.start()
                    }
                }
            }
        }
    }
    Rectangle {
        id: buttonImage
        anchors { right: parent.right; rightMargin: 12; bottom: parent.bottom; bottomMargin: root.hardKeyMargin }
        scale: buttonMouseArea.pressed ? 0.85 : 1.0
        width: 111; height: 69; radius: 34
        color: buttonMouseArea.pressed ? MultiSenseCoordinator.kAmbientColor
                                       : '#222226'
        Image {
            source: Resource.imageUrl('popup/popup_option_btn_close.png')
            anchors.centerIn: buttonImage
        }
        MouseArea {
            id: buttonMouseArea
            anchors.fill: parent
            onClicked: {
                goBack()
            }
        }
    }
}
