import QtQuick 2.0
import Zone3ViewEvent 1.0
import CommonConstants 1.0 //chunjae.lee 2018.04.30
import ViewRegistry 1.0 //chunjae.lee  2018.04.30
import "../../GlobalConstants.js" as GlobalConstants
import "../../view"
import "../../view/util/CommonUtils.js" as CommonUtils //chunjae.lee  2018.04.30

View {
    id: root

    y: GlobalConstants.kScreenZone3Y
    width: childrenRect.width
    height: childrenRect.height
    clip: true

    property alias currentViewName: softkey.currentViewName

    property string timeFormat: 'AP hh:mm'
    property date gpsTime
    property int gpsHour
    property int gpsMin
    property int gpsSec
    property bool gpsTimeUpdated: false
    property alias optionMenu: softkey.optionMenu
    property bool isTarget: false
    property string lang: 'ko'

    onVisibleChanged: {
        if (visible)
            y = GlobalConstants.kScreenZone3Y
    }

    state: "show"
    states: [
        State {
            name: "show"
            PropertyChanges { target: root; y: GlobalConstants.kScreenZone3Y }
        },
        State {
            name: "hide"
            PropertyChanges { target: root; y: GlobalConstants.kScreenZone3Y + height }
        }
    ]
    transitions: Transition {
        PropertyAnimation { property: "y"; easing.type: Easing.InOutQuad; duration: 200 }
    }
    onGpsTimeChanged: {
        gpsTimeUpdated = true
    }

    onMediated: {
        sendEvent(Zone3ViewEvent.Mediated)
    }

    function registSoftkey(name) {
        softkey.currentViewName = name
    }

    function isCurrent(name) {
        return name === softkey.currentViewName
    }

    function softkeyVisible(visible) {
        softkey.visible = visible
    }
    function setSoftkeyMenus(menus, options) {
        softkey.setSoftkeyMenus(menus, options)
    }
    function setSoftkeyMenusWithTab(menus, select) {
        softkey.setSoftkeyMenusWithTab(menus, select)
    }
    function setSoftkeyMenusWithTab2(menus, select) {
        softkey.setSoftkeyMenusWithTab2(menus, select)
    }
    function setSoftkeyMenus2(menus, options, defaultMenu) {
        softkey.setSoftkeyMenus2(menus, options, defaultMenu)
    }
    function editSoftkeyMenu(index, menu) {
        softkey.editSoftkeyMenu(index, menu)
    }
    function editSoftkeyMenu2(index, menu) {
        softkey.editSoftkeyMenu2(index, menu)
    }
    function setSoftkeyNavi() {
        softkey.setNaviMenu()
    }
    function setSoftkeyNormalDrive() {
        softkey.setSubmenu("drive")
        softkey.setNaviNormalDrive("")
    }
    function setSoftkeyAddress(address) {
        softkey.setNaviNormalDrive(address)
    }
    function setSoftkeySimulation() {
        softkey.setSimulationMenu()
    }
    function setSoftkeyRouteGuide(data) {
        var poi = data["poi"]
        var yah = data["yah"]
        var road = data["road"]
        var remainDistance = data["remainDistanceText"]
        var remainTime = data["remainTime"]
        softkey.setNaviRouteGuide(poi, yah, road, remainDistance, remainTime)
    }
    function setSoftkeyToggle(menus, select) {
        softkey.setToggleMenu(menus, select)
    }
    function setSoftkeyToggle2(menus, select) {
        softkey.setToggleMenu2(menus, select)
    }
    function updateSoftkeyMenu(data) {
        var index = data["index"]
        var menu = data["menu"]
        softkey.updateSoftkeyMenu(index, menu)
    }
    function stopNavi() {
        softkey.stopNavi()
    }
    function blockSoftkey(block) {
        softkeyBlocker.visible = block
    }
    function blockNaviButton(block) {
        softkey.enableNaviButton = !block
        softkey.enableOptionButton = !block
    }
    function blockMenuButton(block) {
        softkey.enableBackButton = !block
        softkey.enableOptionButton = !block
    }
    function sendSoftkeyFunction(menu) {
        sendEvent(Zone3ViewEvent.OptionMenuClicked, {name:currentViewName, menu:menu})
    }
    function sendSoftkeyFunctionIndex(menus) {
        menus['name'] = currentViewName
        sendEvent(Zone3ViewEvent.OptionMenu2Clicked, menus)
    }
    function execOption() {
        softkey.execOptionMenu()
    }

    MouseArea {
        id: disablePropagateMouseEvent
        anchors.fill: softkey
        onClicked: {
        }
    }

    Zone3Softkey {
        id: softkey
        objectName: "zone3Softkey"
        owner: parent
        layout: root.layout

        property var activeSoftkey
        property var softkeys: []
        property var currentViewName: 'MapView'

        function execOptionMenu() {
            if (optionMenu.length === 0 && optionMenu2.length === 0) {
                sendEvent(Zone3ViewEvent.OptionClicked, {name:currentViewName})
            } else {
                if (optionMenu.length > 0) {
                    sendEvent(Zone3ViewEvent.OpenOptionMenuView, {menu:optionMenu})
                } else if (optionMenu2.length > 0) {
                    sendEvent(Zone3ViewEvent.OpenOptionMenuView2, {menu:optionMenu2})
                }
            }
        }

        onBackClicked: {
            sendEvent(Zone3ViewEvent.BackClicked, {name:currentViewName})
        }
        onOptionClicked: {
            execOptionMenu()
        }
        onNaviClicked: {
            sendEvent(Zone3ViewEvent.NaviClicked, {name:currentViewName})
        }
        onMenuClicked: {
            sendEvent(Zone3ViewEvent.MenuClicked, {name:currentViewName, menu:menuName})
        }
        onMenu2Clicked: {
            sendEvent(Zone3ViewEvent.Menu2Clicked, {name:currentViewName, menu:menuName, index:index})
        }

        Component.onCompleted: {
            setSubmenu("drive");
        }
    }

    MouseArea {
        id: softkeyBlocker

        anchors.fill: softkey
        visible: false
        Rectangle {
            anchors.fill: parent
            color: "black"
            opacity: isTarget ? 0.0 : 0.7
        }
        onClicked: sendEvent(Zone3ViewEvent.FaderClicked)
    }
}
