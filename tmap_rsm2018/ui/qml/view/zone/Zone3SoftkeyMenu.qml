import QtQuick 2.0
import "../../component"
import "../../GlobalConstants.js" as GlobalConstants

TFocusItem {
    id: root

    property int menusWidth: width
    property int menusLength: 0
    property int menusX: 0
    property var menus: []
    property var menus2: []
    property var layout
    property bool isTab: false
    property string selectedMenu
    property string defaultMenu

    signal menuClicked(string menuName)
    signal menu2Clicked(int index, string menuName)
    signal menuCleared
    signal menuCreated(var csw)

    onMenusChanged: {
        menus2 = null
        clearMenu()

        if (menus === undefined)
            return

        if (menus && menus.length > 0) {
            menusLength = menus.length;
            menusWidth = root.width

            for (var i=0; i<menus.length; i++) {
                var item = {title:menus[i].text }
                console.log('menu = ' + item.title)
                listModel.append({title:menus[i], menuIndex:-1})

            }
            loader.sourceComponent = compMenus
            loader.item.arrangeItems()
        } else {
            loader.sourceComponent = undefined
        }
    }
    onMenus2Changed: {
        clearMenu()

        if (menus2 === undefined)
            return

        if (menus2 && menus2.length > 0) {
            menusLength = menus2.length;
            menusWidth = root.width

            for (var i=0; i<menus2.length; i++) {
                var item = {title:menus2[i].text, menuIndex:menus2[i].id}
                listModel.append(item)
            }
            loader.sourceComponent = compMenus
            loader.item.arrangeItems()
        } else {
            loader.sourceComponent = undefined
        }
    }

    function clearMenu() {
        root.menuCleared()
        selectedMenu = ""
        listModel.clear()
    }

    function unload() {
        root.menuCleared()

        if (loader.sourceComponent !== undefined)
            loader.sourceComponent = undefined

        if (loader.source !== "")
            loader.source = ""
    }

    function updateMenu(index, menu) {
        listModel.get(index).title = menu;
    }

    function getTitle(title) {
        var ret
        var n = title.indexOf(":disabled")
        if (n >= 0) {
            ret = title.substring(0, n)
        } else {
            ret = title
        }
        return ret
    }

    function isQml(str) {
        var n = str.indexOf(".qml")
        return n >= 0
    }

    ListModel { id: listModel }

    Component {
        id: compMenuButton

        Zone3SoftkeyMenuButton {
            id: menuButton

            property bool isDividerVisible: false

            width: (menusWidth / menusLength); height: root.height
            selected: isTab && (selectedMenu === buttonText)

            Zone3Divider {
                lineHeight: root.layout.menuButtonLineHeight*root.layout.ratio
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.right
                visible: currentViewName === "AddressSearchView" ? false : isDividerVisible;
            }
            onClicked: {
                root.menuClicked(buttonText)
                selectedMenu = buttonText
            }
            onClicked2: {
                root.menu2Clicked(index, buttonText)
                selectedMenu = buttonText
            }
        }
    }

    Component {
        id: compLoader
        Loader {
            property bool isQmlComponent: false

            function setPosition() {
                item.x = menusX
                if (isQmlComponent) {
                    menusX += item.width
                } else {
                    menusX += (menusWidth / menusLength)
                }
            }

            Component.onCompleted: {
                isQmlComponent = isQml(title)

                if (isQmlComponent) {
                    source = title
                } else {
                    sourceComponent = compMenuButton
                }
            }
            onLoaded: {
                item.height = root.height

                if (menus2 && 0 < menus2.length) {
                    menusLength = menus2.length
                    menusWidth = root.width

                    root.menuCreated(null)

                    if (!isQmlComponent) {
                        item.buttonText = getTitle(title)
                        item.enabled = title.indexOf(":disabled") < 0
                        item.index = menuIndex
                        item.isDividerVisible = (menus2 !== undefined) && index != (menus2.length-1)
                    } else {
                        if (menus2.length === 2) {
                            item.width = (GlobalConstants.kScreenHeight === 480) ? 330 : 270
                        }

                        root.menusWidth -= item.width
                        menusLength -= 1

                        item.clicked.connect(function () {
                            root.menu2Clicked(menuIndex, title)
                            selectedMenu = title
                        })
                    }
                } else {
                    root.menuCreated(null)

                    menusLength = menus.length

                    menusWidth = root.width

                    if (!isQmlComponent) {
                        item.buttonText = getTitle(title)
                        item.enabled = title.indexOf(":disabled") < 0
                        item.isDividerVisible = (menus !== undefined) && index != (menus.length-1)
                    } else {
                        if (menus.length === 2) {
                            item.width = (GlobalConstants.kScreenHeight === 480) ? 330 : 270
                        }

                        root.menusWidth -= item.width
                        menusLength -= 1

                        item.clicked.connect(function () {
                            root.menuClicked(title)
                            selectedMenu = title
                        })
                    }
                }
            }
        }
    }

    Component {
        id: compMenus

        Item {
            id: repeaterRoot
            anchors.fill: parent

            Repeater {
                id: repeater

                model: listModel
                delegate: compLoader
            }

            function arrangeItems() {
                root.menusX  = 0
                for (var i=0; i<children.length; i++) {
                    var child = children[i]

                    if (child !== repeater) {
                        child.setPosition()
                    }
                }
            }
        }
    }

    MultiPointTouchArea {
        anchors.fill: loader
    }

    Loader {
        id: loader
        anchors.fill: parent
    }
}
