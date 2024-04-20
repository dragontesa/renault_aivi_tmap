import QtQuick 2.0
import Csw 1.0
import "../component"
import "../singleton"
import "../GlobalConstants.js" as GlobalConstants

Item {
    id: root

    property alias title: titleText.text
    property alias buttonText: buttonText.text
    property int rowCount
    property int cellWidth
    property int cellHeight
    property var menus
    property int menuCount
    readonly property int contentHeight: 198

    property int currentIndex: 0
    property bool cswEnable: false
    property bool buttonCswEnable: false

    property alias csw: csw
    property alias buttonCsw: buttonCsw

    signal clicked(int index)
    signal cancelClicked()

    onMenusChanged: {
        var length = menus.length
        rowCount = Math.floor((length+1) / 2)
        cellWidth = width / rowCount
        cellHeight = contentHeight / 2
        gridView.model = menus

        menuCount = 0
        for (var i=0; i<menus.length; i++) {
            var menu = menus[i]
            if (menu.length > 0 && menu !== ' ') {
                ++menuCount
            }
        }
    }

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
                currentIndex = menuCount - 1
        }
        onSpinCwPressed: {
            if (currentIndex < menuCount - 1)
                currentIndex += 1
            else
                currentIndex = 0
        }
        onSelected: {
            root.clicked(currentIndex)
        }
    }

    width: 640; height: 350
    Rectangle {
        anchors.fill: parent
        color: '#222226'; border.color: '#a1a2b3'

        Column {
            anchors.fill: parent

            Text {
                id: titleText
                width: parent.width; height: 90
                font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(32) }
                //text: title
                color: '#ffffff'
                verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter

                Rectangle {
                    anchors { left: parent.left; right: parent.right; bottom: parent.bottom }
                    height: 1
                    color: '#a1a2b3'; opacity: 0.3
                }
            }
            GridView {
                id: gridView

                width: parent.width; height: 198
                cellWidth: root.cellWidth; cellHeight: root.cellHeight
                interactive: false

                delegate: Item {
                    width: root.cellWidth; height: root.cellHeight
                    //BG
                    Rectangle {
                        id: bg
                        anchors.fill: parent
                        color: {
                            if (modelData != '') {
                                return gridArea.pressed
                                        ? MultiSenseCoordinator.kAmbientColor
                                        : (cswEnable && currentIndex == index) ? '#ffffff' : '#000000'
                            } else {
                                return '#000000'
                            }
                        }
                        opacity: {
                            if (modelData != '') {
                                if (gridArea.pressed) {
                                    return 0.3
                                } else  {
                                    return (cswEnable && currentIndex == index) ? 1.0 : 0.0
                                }
                            } else {
                                return 0.3
                            }
                        }
                    }
                    Text {
                        anchors { left: parent.left; leftMargin: 10
                            right: parent.right; rightMargin: 10
                            verticalCenter: parent.verticalCenter
                        }
                        height: 30
                        color: (cswEnable && currentIndex == index) ? '#000000' : '#ffffff'
                        font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(32) }
                        text: modelData
                        scale: gridArea.pressed ? 0.85 : 1.0
                        verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter
                    }
                    MouseArea {
                        id: gridArea
                        anchors.fill: parent
                        enabled: modelData != ''
                        onPressed: bg.opacity = 0.3
                        onReleased: bg.opacity = 0.0
                        onClicked: root.clicked(index)
                    }//MouseArea

                    Rectangle {
                        anchors.right: parent.right
                        width: 1; height: cellHeight; color: '#a1a2b3'; opacity: 0.3
                    }
                    Rectangle {
                        anchors.bottom: parent.bottom
                        width: cellWidth; height: 1; color: '#a1a2b3'; opacity: 0.3
                    }
                }//delegate: Item
            }//GridView
            Item {
                width: parent.width; height: 62
                Rectangle {
                    id: cancelBg
                    anchors.fill: parent
                    color: buttonCswEnable ? '#ffffff' : MultiSenseCoordinator.kAmbientColor
                    opacity: {
                        if (buttonCswEnable) {
                            return 1.0
                        } else {
                            cancelArea.pressed ? 0.3 : 0.0
                        }
                    }
                }
                Text {
                    id: buttonText
                    anchors.fill: parent
                    font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(32) }
                    color: buttonCswEnable ? '#000000' : '#ffffff'
                    scale: cancelArea.pressed ? 0.85 : 1.0
                    verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter

                    MouseArea { id: cancelArea; anchors.fill: parent; onClicked: root.cancelClicked() }
                }
                Csw {
                    id: buttonCsw
                    section: Csw.Section_DialogBottom
                    sequence: 0
                    onFocused: {
                        buttonCswEnable = true
                    }
                    onLostFocusd: {
                        buttonCswEnable = false
                    }
                    onSelected: {
                        root.cancelClicked()
                    }
                }
            }
        }//Row
    }//BorderImage
}
