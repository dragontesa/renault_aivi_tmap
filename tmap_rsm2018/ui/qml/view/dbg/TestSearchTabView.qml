import QtQuick 2.1
import CommonConstants 1.0
import ZoneConstants 1.0
import SearchConstants 1.0
import TestSearchTabViewEvent 1.0
import LayoutConstants 1.0
import ViewRegistry 1.0
import Csw 1.0

import "../../component"
import "../../view"
import "../../view/search"
import "../../GlobalConstants.js" as GlobalConstants
import "../../component/internal/Keyboard.js" as CustomKeyboard
import "../../view/util/CommonUtils.js" as CommonUtils

PopupView {
    id: searchRoot
    width: parent ? parent.width : undefined
    height: parent ? parent.height : undefined

    objectName: "SearchTabView"

    layout: QtObject {
        property int tabBarHeight : 48
        function updateLayout( lay ) {
            if ( lay === LayoutConstants.L800x480 ) {
            }
            else if ( lay === LayoutConstants.L820x1024 ) {
            }
        }
    }

    property int page: -1
    property bool isCompleted: false
    property bool beenActivated: false
    property bool isCswControl: false

    property var textEnum: (function() {
        textEnum = CommonUtils.createEnum(textEnum)
        CommonUtils.appendEnum(textEnum, "DeleteAll", [CommonConstants.Name], [qsTr("전체삭제")])
        CommonUtils.appendEnum(textEnum, "WantToDeleteAll", [CommonConstants.Name], [qsTr("즐겨찾기 리스트를\n모두 삭제하시겠습니까?")])
        CommonUtils.appendEnum(textEnum, "ChangeName", [CommonConstants.Name], [qsTr("명칭변경")])
    }())
    property var textArr : (function() {
        textArr = []
        CommonUtils.addArr(textEnum,  CommonConstants.Name, textArr)
    }())
    property var tabBtnEnum : (function() {
        tabBtnEnum = CommonUtils.createEnum(tabBtnEnum)
        CommonUtils.appendEnum(tabBtnEnum,"RecentSearch"                 ,[CommonConstants.Name], [qsTr("최근 검색")] )
        CommonUtils.appendEnum(tabBtnEnum,"RecentDestination"            ,[CommonConstants.Name], [qsTr("최근 목적지")] )
        CommonUtils.appendEnum(tabBtnEnum,"Favorite"                     ,[CommonConstants.Name], [qsTr("즐겨찾기")] )
    }())
    property var tabBtnMenu : (function() {
        tabBtnMenu = []
        CommonUtils.addArr(tabBtnEnum, CommonConstants.Name, tabBtnMenu)
    }())

    function __focusActivatedZone1TextInput() {
        //console.log( "#1" )
        page = tabBtnEnum.RecentSearch
        isCswControl = false
    }

    Component.onCompleted: {
        console.log( "#1 page: " + page )

        isCompleted = true
    }

    onMediated: {
        console.log( "#1 page: " + page )

        // 17.11.8 hskim only in the case that the page argument isn't specified, it performs force initialization.
        if ( page == -1 ) {
            page = tabBtnEnum.RecentSearch
        }

        sendEvent( TestSearchTabViewEvent.Mediated )
        searchRoot.sendEvent( TestSearchTabViewEvent.CreateView, {viewId: ViewRegistry.RecentSearchPageView, parent: tabPage1} )
        searchRoot.sendEvent( TestSearchTabViewEvent.CreateView, {viewId: ViewRegistry.RecentDestinationPageView, parent: tabPage2} )
        searchRoot.sendEvent( TestSearchTabViewEvent.CreateView, {viewId: ViewRegistry.BookmarkPageView, parent: tabPage3} )
    }

    onActivated: {
        console.log( "#1 page: " + page )

        beenActivated = true

        // 17.11.8 hskim added to emit an activation signal even for changes to the same tab page
        var oldPage = page
        page = -1
        page = oldPage

        sendEvent(TestSearchTabViewEvent.ResetCsw)
        var args = [ csw ]
        sendEvent(TestSearchTabViewEvent.RegistCsw, args)
    }

    onDeactivated: {
        console.log( "#1 page: " + page )

        sendEvent(TestSearchTabViewEvent.ResetCsw)
    }

    onClosed: {
        searchRoot.sendEvent( TestSearchTabViewEvent.CloseView, {viewId: ViewRegistry.RecentSearchPageView} )
        searchRoot.sendEvent( TestSearchTabViewEvent.CloseView, {viewId: ViewRegistry.RecentDestinationPageView} )
        searchRoot.sendEvent( TestSearchTabViewEvent.CloseView, {viewId: ViewRegistry.BookmarkPageView} )
    }

    onPageChanged: {        
        if (!beenActivated) {
            return
        }
        if (page == -1) {
            return
        }
        if (page < tabBtnEnum.RecentSearch || page > tabBtnEnum.Favorite) {
            console.log( "Detected an attempt to change to invalide tab page page:" + page )
            return
        }

        tabBar.setActiveTab(page)               // 탭버튼 UI property 설정
        tabView.setCurrentPage( page )            // 각 페이지의 visible 설정

        searchRoot.sendEvent( TestSearchTabViewEvent.TabPageActivated, {page: page} )
    }

    Column {
        width: parent.width
        height: parent.height
        TabBar {
            id: tabBar
            width: parent.width
            height: layout.tabBarHeight
            headers: [tabBtnMenu[tabBtnEnum.RecentSearch],
                      tabBtnMenu[tabBtnEnum.RecentDestination],
                      tabBtnMenu[tabBtnEnum.Favorite]]
            tabWidth: width / tabBar.headers.length
            tabHeight: height
            tabColor:  "#222226"
            tabActiveColor: "#0091ff"
            tabTextColor: "white"
            tabTextActiveColor: "white"
            tabActiveStyle: tabBar.tabActiveStyleEnum.Border
            tabBorderName: tabBar.tabBorderNameEnum.Bottom
            tabBorderWidth: 1

            onClicked: {
                if (!beenActivated) {
                    console.log( "#1 page: " + page )
                    return
                }
                searchRoot.page = index
            }
        }
        TabView {
            id: tabView
            width: parent.width
            height: parent.height - tabBar.height
            Item {
                id: tabPage1
                anchors.fill: parent
                objectName: 'tabPage1'
                visible: false
            }
            Item {
                id: tabPage2
                anchors.fill: parent
                visible: false
            }
            Item {
                id: tabPage3
                anchors.fill: parent
                visible: false
            }
        }

    } // Column

    Csw {
        id: csw
        section: Csw.Section_Tab
        sequence: 0
        enable: searchRoot.visible
        onFocused: {
            console.log("#1")
            searchRoot.focus = true
            if (!isCswControl) {
                searchRoot.page = 0
            }
            else {
                searchRoot.page = page
            }
        }
        onLostFocusd: {
            console.log("#1")
            searchRoot.focus = false
            isCswControl = false
        }

        onSelected: {
            searchRoot.focus = false
        }
    }
}

