import QtQuick 2.0
import "../../component"
import "../../singleton"
import "../../GlobalConstants.js" as GlobalConstants
import "../../view/util/CommonUtils.js" as CommonUtils
import Zone3ViewEvent 1.0

Item {
    id: root

    property var layout
    property real ratio: layout.ratio
    property real iconWidth: layout.iconWidth * ratio
    property alias enableBackButton: backButton.enabled
    property alias enableOptionButton: optionButton.enabled
    property alias enableNaviButton: naviButton.enabled
    property string subMenu
    property var optionMenu: []
    property var optionMenu2: []
    property int rgDispType: 0
    property int rgTimeType: 0
    property var owner: null
    property string navi_menu_img: lang == 'ko' ? 'zone/zone3_btn_menu.png' : 'zone/zone3_btn_menu_f_eng.png'
    property string navi_menu_f_img: lang == 'ko' ? 'zone/zone3_btn_menu_f.png' : 'zone/zone3_btn_menu_eng.png'

    width: GlobalConstants.kScreenWidth
    height: GlobalConstants.kScreenZone3Height

    signal menuClicked(string menuName)
    signal menu2Clicked(int index, string menuName)
    signal backClicked()
    signal optionClicked()
    signal naviClicked()
    signal rgRemainDistanceClicked()

    function setSoftkeyMenus(menus, options) {        
        var noOptionIndex = -1
        if (menus !== null && menus.length > 0) {
            noOptionIndex = menus.indexOf("noOption")
            if (0 <= noOptionIndex)
                menus.splice(noOptionIndex, 1)
        }
        softkeyMenu.menus = menus
        softkeyMenu.isTab = false
        setSubmenu("menu")
        backButton.visible = true
        setBackgroundOpacity(1.0)
        optionMenu = options
        optionMenu2 = []

        optionButton.enabled = (noOptionIndex < 0) || (0 < options.length)
    }
    function setSoftkeyMenus2(menus, options, defaultMenu) {
        softkeyMenu.menus2 = menus
        softkeyMenu.isTab = false
        softkeyMenu.defaultMenu = defaultMenu
        setSubmenu("menu")
        backButton.visible = true
        setBackgroundOpacity(1.0)
        optionMenu = []
        optionMenu2 = options

        optionButton.enabled = options && (0 < options.length)
    }
    function setSoftkeyMenusWithTab(menus, select)
    {
        setSoftkeyMenus(menus, [])
        softkeyMenu.isTab = true
        softkeyMenu.selectedMenu = select ? select : menus[0]
    }
    function setSoftkeyMenusWithTab2(menus, select)
    {
        var noOptionIndex = -1
        if (menus !== null && menus.length > 0) {
            noOptionIndex = menus.indexOf("noOption")
            if (0 <= noOptionIndex)
                menus.splice(noOptionIndex, 1)
        }

        var menus2 = []
        for (var i = 0; i < menus.length; i++) {
            menus2.push({id:i, text:menus[i]})
        }

        setSoftkeyMenus2(menus2, [], '')
        softkeyMenu.isTab = true
        softkeyMenu.selectedMenu = select ? select : menus[0]
    }

    function editSoftkeyMenu(index, menu) {
        softkeyMenu.menus[index] = menu
        var menus = softkeyMenu.menus
        softkeyMenu.menus = undefined
        softkeyMenu.menus = menus
    }
    function editSoftkeyMenu2(index, menu) {
        softkeyMenu.menus2[index] = menu
        var menus = softkeyMenu.menus2
        softkeyMenu.menus2 = undefined
        softkeyMenu.menus2 = menus
    }
    function softkeyMenuUnload() {
        softkeyMenu.unload()
        optionMenu = []
        optionMenu2 = []
        owner.sendEvent(Zone3ViewEvent.RemoveSoftkeySection)
    }

    function setNaviMenu() {
        backButton.visible = false
        softkeyMenu.visible = false
        softkeyMenu.unload()
        optionMenu = []
        optionMenu2 = []
        toggleMenu.visible = false
        setSubmenu("guide")
        setBackgroundOpacity(0.85)

        rgDispType = 0
    }

    function stopNavi() {
        if (subMenu === "guide") {
            setSubmenu("drive")
        }
    }
    function changeRgDispType() {
        switch (rgDispType) {
        case 0: rgDispType = 1
            break
        case 1: rgDispType = 2
            break
        case 2: rgDispType = 0
            break
        }
    }
    function changeRgTimeType() {
        rgTimeType = rgTimeType ? 0 : 1
        if (rgTimeType === 0) {
            naviRouteGuide.setArrivalItemVisible(true)
        } else {
            naviRouteGuide.setRemainItemVisible(true)
        }
    }

    function setToggleMenu(menus, select) {
        toggleMenu.menus = undefined
        toggleMenu.menus = menus
        toggleMenu.currentIndex = select
        toggleMenu.isIndexMode = false
        setSubmenu("toggle")
        backButton.visible = true
    }
    function setToggleMenu2(menus, select) {
        setToggleMenu(menus, select)
        toggleMenu.isIndexMode = true
    }

    function setSubmenu(menuName) {
        softkeyMenu.visible = menuName === "menu"
        toggleMenu.visible = menuName === "toggle"
        naviNormalDrive.visible = menuName === "drive"
        naviRouteGuide.visible = menuName === "guide"

        root.subMenu = menuName
    }
    function setNaviNormalDrive(address) {
        if (subMenu == "drive") {
            naviNormalDrive.address = address
            naviRouteGuide.visible = false
            naviNormalDrive.visible = true
            backButton.visible = false
            setBackgroundOpacity(0.85)

            naviRouteGuide.setRemainItemVisible(false)
            naviRouteGuide.setArrivalItemVisible(false)
            rgDispType = 0
        }
    }
    function setSimulationMenu() {
        backButton.visible = true
        softkeyMenu.visible = false
        softkeyMenu.unload()
        optionMenu = []
        optionMenu2 = []
        toggleMenu.visible = false
        setSubmenu("guide")
        setBackgroundOpacity(0.85)
        naviRouteGuide.setRemainItemVisible(false)
        naviRouteGuide.setArrivalItemVisible(false)

        rgDispType = 1
    }

    function setNaviRouteGuide(poi, yah, road, remainDistance, remainTime) {
        naviRouteGuide.remainDistance = remainDistance
        if (rgTimeType < 0) {
            rgTimeType = 0
            naviRouteGuide.setArrivalItemVisible(true)
        }

        if (rgTimeType === 0) {
            var t = gpsTimeUpdated
                    ? CommonUtils.getArrivalTimeUsingTime(gpsHour, gpsMin, gpsSec, remainTime)
                    : CommonUtils.getArrivalTimeUsingDate(new Date(), remainTime)
            naviRouteGuide.arrivalTime = t

            naviRouteGuide.setArrivalItemVisible(true)
        } else {
            naviRouteGuide.remainTime = remainTime
            naviRouteGuide.setRemainItemVisible(true)
        }

        switch (rgDispType) {
        case 0: naviRouteGuide.setRouteGuideByCurrentPosition(yah)
            break
        case 1: naviRouteGuide.setRouteGuideByCurrentRoad(road)
            break
        case 2: naviRouteGuide.setRouteGuideByPoi(poi)
            break;
        }
    }

    function updateSoftkeyMenu(index, menu) {
        softkeyMenu.updateMenu(index, menu)
    }

    function setBackgroundOpacity(opacity) {
        centerBg.opacity = opacity
    }

    Rectangle {
        id: centerBg
        anchors.left: parent.left
        anchors.right: parent.right
        height: parent.height
        color: GlobalConstants.kNormalColor
    }
    IconButton {
        id: backButton

        width: iconWidth
        height: parent.height
        visible: false

        iconSource: {
            if (enabled) {
                switch (state) {
                case 'normal':
                case 'pressed':
                    return Resource.imageUrl("zone/zone3_btn_back.png")
                case 'focused':
                    return Resource.imageUrl('zone/zone3_btn_back_f.png')
                }
            } else {
                return Resource.imageUrl('zone/zone3_btn_back_d.png')
            }
        }
        onClicked: {
            root.backClicked()
        }
    }
    IconButton {
        id: naviButton
        visible: !backButton.visible
        anchors.fill: backButton
        iconSource: {
            switch (state) {
            case 'normal':
            case 'pressed':
                return Resource.imageUrl(navi_menu_img) // "zone/zone3_btn_menu.png")
            case 'focused':
                return Resource.imageUrl(navi_menu_f_img) // 'zone/zone3_btn_menu_f.png')
            }
        }
        onClicked: {
            root.naviClicked()
            forceActiveFocus() // fixed, the recent grid view actived is not closed sometimes when navimenu is clicked, it needs to have TMouseArea [TMCRSM-230]
        }
        Rectangle {
            anchors.fill: parent
            visible: !naviButton.enabled
            color: "black"
            opacity: 0.7
        }
    }
    IconButton {
        id: optionButton

        anchors.right: parent.right
        width: iconWidth
        height: parent.height

        iconSource: {
            if (enabled) {
                switch (state) {
                case 'normal':
                case 'pressed':
                    return Resource.imageUrl("zone/zone3_btn_option.png")
                case 'focused':
                    return Resource.imageUrl('zone/zone3_btn_option_f.png')
                }
            } else {
                return Resource.imageUrl('zone/zone3_btn_option_d.png')
            }
        }
        onClicked: {
            root.optionClicked()
        }        
    }

    Zone3SoftkeyMenu {
        id: softkeyMenu
        layout: root.layout

        anchors.left: backButton.right
        anchors.right: optionButton.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom

        onMenuClicked: root.menuClicked(menuName)
        onMenuCleared: owner.sendEvent(Zone3ViewEvent.MenuCleared)
        onMenuCreated: owner.sendEvent(Zone3ViewEvent.MenuCreated, [])
        onMenu2Clicked: root.menu2Clicked(index, menuName)
    }
    Zone3NaviNormalDrive {
        id: naviNormalDrive
        layout: root.layout

        visible: false
        anchors.left: backButton.right
        anchors.right: optionButton.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom
    }
    Zone3NaviRouteGuide {
        id: naviRouteGuide
        layout: root.layout

        visible: false
        anchors.left: backButton.right
        anchors.right: optionButton.left
        anchors.top: parent.top
        anchors.bottom: parent.bottom

        onTimeClicked: changeRgTimeType()
        onAreaClicked: {
            navi.reset()
            changeRgDispType()
        }
        onDistanceClicked: {
            root.rgRemainDistanceClicked()
        }
    }
    Rectangle {
        id: toggleMenuBg
        color: '#ffffff'
        anchors.left: naviButton.right
        anchors.right: optionButton.left
        height: parent.height
        visible: toggleFocused && toggleMenu.visible
    }

    property bool toggleFocused: false
    Zone3SoftkeyToggleMenu {
        id: toggleMenu
        layout: root.layout

        visible: false

        onClicked: {
            if (isIndexMode) {
                root.menu2Clicked(index, menuName)
            } else {
                root.menuClicked(menuName)
            }
        }        
    }

    Zone3Divider {
        layout: root.layout
        lineHeight: layout.backButtonLineHeight*ratio
        anchors.left: backButton.right
        anchors.verticalCenter: centerBg.verticalCenter
    }
    Zone3Divider {
        layout: root.layout
        lineHeight: layout.optionButtonLineHeight*ratio
        anchors.right: optionButton.left
        anchors.verticalCenter: centerBg.verticalCenter
    }
}
