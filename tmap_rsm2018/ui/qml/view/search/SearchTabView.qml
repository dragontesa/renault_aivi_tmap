import QtQuick 2.1
import CommonConstants 1.0
import ZoneConstants 1.0
import SearchConstants 1.0
import SearchTabViewEvent 1.0
import LayoutConstants 1.0
import ViewRegistry 1.0
import Csw 1.0

import "../../component"
import "../../view"
import "../../view/search"
import "../../GlobalConstants.js" as GlobalConstants
import "../../component/internal/Keyboard.js" as CustomKeyboard
import "../../view/util/CommonUtils.js" as CommonUtils
import "../../singleton"

PopupView {
    id: searchRoot

    background.visible: false
    fader.fadingOpacity: 0
    mouseArea.enabled : false

    width: parent ? parent.width : undefined
    height: parent ? parent.height : undefined

    objectName: "SearchTabView"

    layout: QtObject {
        property int tabBarHeight : 84
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
    property bool tabBarVisible: false
    property int cswTabIndex: 0
    property bool tabCswEnable: true
    property bool tabEnable: true
    property bool isShowTextSearchView: false
    property bool reFocus: false

    //ID
    property var tabBar: null
    property var tabView: null
    property var tabPage1: null
    property var tabPage2: null
    property var tabPage3: null
    property var tabPage4: null

    property var caller
    property string query: ""

    readonly property var kPageIndex: {
        'RecentSearch':SearchConstants.RecentSearchPageIndex,
        'RecentDestination':SearchConstants.RecentDestPageIndex,
        'Favorite':SearchConstants.FavoritePageIndex,
        'NoResult':SearchConstants.NoResultPageIndex,
    }

    function __focusActivatedZone1TextInput() {
        console.log( "#1" )
        page = kPageIndex.RecentSearch
        isCswControl = false
    }

    function __hideContent() {
        tabBar.visible = false
        tabView.visible = false
    }

    function changePageByCsw(newPage) {
        page = newPage
        tabView.setCurrentPageCswFocus(newPage)
    }

    // [주의] 언어가 동적으로 바뀌지 않는 문제 발생하여 임시 처리함, 추후 원인 파악 필요
    property var __lang: null  // SearchTabViewEvent.RequestLang 요청시 설정된 언어를 얻어옴
    readonly property var headersTextKo: [qsTr("최근 검색")+Tr.d     , qsTr("최근 목적지")+Tr.d       , qsTr("즐겨찾기")+Tr.d ]
    // property var headersTextEn: ["Search history", "Recent destination", "Favorites"]
    function __retranslateUi(lang) {//언어 전환이 발생할 때 들어옴
        searchRoot.__lang = lang
    }

    Component.onCompleted: {
        isCompleted = true
    }

    function __onResultViewList(result) {
        for(var i= 0; i<result.length; i++) {
            if (CommonUtils.isEqual(result[i][CommonConstants.ResultViewId], ViewRegistry.TextSearchView)) {
                isShowTextSearchView = result[i][CommonConstants.ResultViewVisible]
                return
            }
        }
        isShowTextSearchView = false
    }

    onReFocusChanged: {
        if (!reFocus)
            return
        reFocus = false
        if (searchRoot.visible) {
            if (!active) {
              //viewActivated()
            }
        }
    }


    onMediated: {
        //console.log( "#1 page: " + page )
        sendEvent(SearchTabViewEvent.RequestLang)

        loader.sourceComponent = main

        // 17.11.8 hskim only in the case that the page argument isn't specified, it performs force initialization.
        if ( page == -1 ) {
            page = kPageIndex.RecentSearch
        }

        sendEvent( SearchTabViewEvent.Mediated )
        searchRoot.sendEvent( SearchTabViewEvent.CreateView, {viewId: ViewRegistry.RecentSearchPageView, parent: tabPage1} )
        searchRoot.sendEvent( SearchTabViewEvent.CreateView, {viewId: ViewRegistry.RecentDestinationPageView, parent: tabPage2} )
        searchRoot.sendEvent( SearchTabViewEvent.CreateView, {viewId: ViewRegistry.BookmarkPageView, parent: tabPage3} )
        searchRoot.sendEvent( SearchTabViewEvent.CreateView, {viewId: ViewRegistry.NoResultPageView, parent: tabPage4, query: query} )
    }

    onActivated: {
        //console.log( "#1 page: " + page )
        sendEvent(SearchTabViewEvent.RequestViewList)
        if (isShowTextSearchView) {
            viewDeactivated()
            return
        }

        tabBar.visible = true
        tabView.visible = true
        beenActivated = true
        tabCswEnable = true

        // 17.11.8 hskim added to emit an activation signal even for changes to the same tab page
        var oldPage = page
        page = -1
        page = oldPage

        sendEvent(SearchTabViewEvent.ResetCsw)
        var args = [ csw ]
        sendEvent(SearchTabViewEvent.RegistCsw, args)
    }

    onDeactivated: {
        //console.log( "#1 page: " + page )

        sendEvent(SearchTabViewEvent.ResetCsw)
    }

    onClosed: {
        searchRoot.sendEvent( SearchTabViewEvent.CloseView, {viewId: ViewRegistry.RecentSearchPageView} )
        searchRoot.sendEvent( SearchTabViewEvent.CloseView, {viewId: ViewRegistry.RecentDestinationPageView} )
        searchRoot.sendEvent( SearchTabViewEvent.CloseView, {viewId: ViewRegistry.BookmarkPageView} )
        searchRoot.sendEvent( SearchTabViewEvent.CloseView, {viewId: ViewRegistry.NoResultPageView} )
    }

    onPageChanged: {
        if (!beenActivated) {
            return
        }
        if (page == -1) {
            return
        }
        if (page < kPageIndex.RecentSearch || page > kPageIndex.NoResult) {
            console.log( "Detected an attempt to change to invalide tab page page:" + page )
            return
        }

        tabBar.setActiveTab(page)               // 탭버튼 UI property 설정
        tabView.setCurrentPage( page )            // 각 페이지의 visible 설정

        searchRoot.sendEvent( SearchTabViewEvent.ActiveTabPageChanged, {page: page} )
    }

    onCswTabIndexChanged: {
        if (cswTabIndex == page) {
            tabBar.tabActiveBorderColor = GlobalConstants.kCswFocusColor
        }
        else {
            tabBar.tabActiveBorderColor = MultiSenseCoordinator.kAmbientColor
        }
    }

    //main
    Loader {
         id: loader
         width: parent.width
         height: parent.height
    }

    Component {
        id: main
        Column {
            width: parent.width
            height: parent.height
            TabBar {
                id: tabBar
                Component.onCompleted: searchRoot.tabBar = this
                width: parent.width
                height: layout.tabBarHeight
                headers: searchRoot.headersTextKo // (searchRoot.__lang === CommonConstants.Ko) ? searchRoot.headersTextKo : searchRoot.headersTextEn
                tabWidth: width / tabBar.headers.length
                tabHeight: height

                tabColor: "#222226"
                tabActiveColor: MultiSenseCoordinator.kAmbientColor
                tabActiveColorOpacity: 0.3
                tabActiveBorderColor: MultiSenseCoordinator.kAmbientColor

                tabTextColor: "white"
                tabTextActiveColor: "white"
                tabActiveStyle: tabBar.tabActiveStyleEnum.Border
                tabBorderName: tabBar.tabBorderNameEnum.Bottom
                tabBorderWidth: 5
                visible: searchRoot.tabBarVisible

                isCswControl: searchRoot.isCswControl
                cswTabIndex: searchRoot.cswTabIndex

                enabled:  {
                    if (!tabEnable)
                        return false
                    if (searchRoot.page < kPageIndex.RecentSearch || searchRoot.page > kPageIndex.NoResult)
                        return true
                    if (tabView.children[ searchRoot.page].children.length <= 0)
                        return true
                    return true
                }

                onClicked: {
                    if (!beenActivated) {
                        //console.log( "#1 page: " + page )
                        return
                    }
                    if (tabEnable) {
                        searchRoot.page = index
                        sendEvent(SearchTabViewEvent.TabTouched, index)
                    }
                }
            }
            TabView {
                id: tabView
                Component.onCompleted: searchRoot.tabView = this
                width: parent.width
                height: parent.height - tabBar.height
                Item {
                    id: tabPage1
                    Component.onCompleted: searchRoot.tabPage1 = this
                    anchors.fill: parent
                    objectName: 'tabPage1'
                    visible: false
                }
                Item {
                    id: tabPage2
                    Component.onCompleted: searchRoot.tabPage2 = this
                    width: parent.width
                    height: parent.height + GlobalConstants.kScreenZone3Height
                    visible: false
                }
                Item {
                    id: tabPage3
                    Component.onCompleted: searchRoot.tabPage3 = this
                    width: parent.width
                    height: parent.height + GlobalConstants.kScreenZone3Height
                    visible: false
                }
                Item {
                    id: tabPage4
                    Component.onCompleted: searchRoot.tabPage4 = this
                    anchors.fill: parent
                    visible: false
                }
            }

        } // Column
    }

    Csw {
        id: csw
        objectName: 'tabCsw'
        section: Csw.Section_Tab
        sequence: 0
        enable: tabCswEnable
        deletable: false
        onFocused: {
            isCswControl = true
            cswTabIndex = page
        }
        onLostFocusd: {
            isCswControl = false
        }
        onSpinCwPressed: {
            if (cswTabIndex < 2)
                cswTabIndex++
        }
        onSpinCcwPressed: {
            if (0 < cswTabIndex)
                cswTabIndex--
        }
        onSelected: {
            searchRoot.page = cswTabIndex
        }
    }
}
