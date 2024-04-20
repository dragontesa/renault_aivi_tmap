import QtQuick 2.0
import AddressSearchViewEvent 1.0
import CommonConstants 1.0
import SettingConstants 1.0
import SearchConstants 1.0
import ViewRegistry 1.0

import "../../singleton"
import "../../component"
import "../../view"
import "../../view/util"
import "../../view/search"
import "../../GlobalConstants.js" as GlobalConstants
import "../../component/internal/Keyboard.js" as CustomKeyboard
import "../util/DebugUtils.js" as Debug
import "../../view/util/CommonUtils.js" as CommonUtils
import "../../singleton"
import QtGraphicalEffects 1.0

import Csw 1.0

PopupView {
    id: root

    width: parent ? parent.width : undefined
    height: parent ? parent.height : undefined

    layout: AddressSearchViewLayout {}

    property bool reFocus     : false
    property bool isMediated  : false
    property bool isShowTopView: false


    property bool isBuildTarget: false

    property variant listModel
    property variant roadListModel
    property variant bldListModel // not used
    property variant lotListModel // not used

    readonly property int roadSearch: 0
    readonly property int lotSearch: 1
    property int searchType
    // chunjae.lee - s 2018.03.16  - define된 enum값으로 변경
    readonly property int lotSearchPref: SettingConstants.AddressDisplayMethodLandNumber
    readonly property int roadSearchPref: SettingConstants.AddressDisplayMethodRoadName
    // chunjae.lee - e 2018.03.16  - define된 enum값으로 변경
    property int searchTypePref

    property var caller: null
    property var owner: null
    // 지번, 도로명 입력 전 선택된 address search vo
    property var selectedModel
    property string zone1Title: qsTr("주소 검색") + Tr.d

    property var softKeyOptions: [qsTr("도로명") + Tr.d,qsTr("지번") + Tr.d, qsTr("noOption")]
    property var roadListShortcutMap

    property string lcdName
    property string mcdName
    property string scdName

    property string roadScdTitle: qsTr("도로명") + Tr.d
    property string lotScdTitle: qsTr("읍/면/동") + Tr.d
    property string lcdTitle: qsTr("광역시/도") + Tr.d
    property string mcdTitle: qsTr("시/군/구") + Tr.d
    property string buildTitle: qsTr("도로명") + Tr.d
    property string lotTitle: qsTr("지번") + Tr.d
    property string lang;

	property string keyboardText
    onKeyboardTextChanged: {
        if (lotInputWrap.visible) {
            lotInputWrap.lotInputField.text = keyboardText
        }
        if (bldInputWrap.visible) {
            bldInputWrap.bldInputField.text = keyboardText
        }
    }

    function updateKeyboardDisplay() {
        if (visible) {
            if (!keyboard)
                return
            if ( bldInputWrap.visible || lotInputWrap.visible ) {
                console.log('bldInputWrap.visible = ' + bldInputWrap.visible)
                console.log('lotInputWrap.visible = ' + lotInputWrap.visible)
                showKeyboard(keyboard)
            }
        } else {
            sendEvent(AddressSearchViewEvent.HideKeyboard)
        }
    }

    // chunjae.lee  s - 2018.04.11 UID v1.27 - 주소검색결과화면에서 Back 버튼 Tap 시, 진입 이전 화면(번지수/건물번호 입력화면)으로 이동
    onVisibleChanged: {
        updateKeyboardDisplay()
    }
    // chunjae.lee  e - 2018.04.11 UID v1.27 - 주소검색결과화면에서 Back 버튼 Tap 시, 진입 이전 화면(번지수/건물번호 입력화면)으로 이동

    onReFocusChanged: {
        if (!reFocus)
            return
        reFocus = false
        if (isMediated) {
            //viewActivated()
        }
    }

    onMediated: {
        sendEvent(AddressSearchViewEvent.Mediated )
        isMediated = true
    }

    onActivated: {
        sendEvent(AddressSearchViewEvent.RequestViewList)
//        if (isShowTopView) {
//            return
//        }
        sendEvent(AddressSearchViewEvent.SelectZone1Title, zone1Title)
        sendEvent(AddressSearchViewEvent.Activated )

        updateKeyboardDisplay()

        sendEvent(AddressSearchViewEvent.ResetCsw)
        sendEvent(AddressSearchViewEvent.RegistCsw, [csw])
    }

    onSearchTypePrefChanged: {
        // addr: 0, road: 1
        if ( searchTypePref === roadSearchPref ) {
            console.log( "도로명 searchTypePref: " + searchTypePref )
            searchType = roadSearch
        }
        else {
            console.log( "지번 searchTypePref: " + searchTypePref )
            searchType = lotSearch
        }
    }

    function __onResultViewList(result) {
        for(var i= 0; i<result.length; i++) {
            if (CommonUtils.isIndexOf(result[i][CommonConstants.ResultViewName],
                                      [CommonConstants.objectName(ViewRegistry.SearchResultView),
                                       CommonConstants.objectName(ViewRegistry.SearchResultDetailView)])) {
                if (result[i][CommonConstants.ResultViewVisible]) {
                    isShowTopView = true
                    return
                }
            }
        }
        isShowTopView = false
    }

    function reset( index ) {
        bar.currentIndex = index
        root.listModel = []
        root.roadListModel = []
        root.bldListModel = []
        root.lotListModel = []
        siGunGuListView.visible = true
        roadListView.visible = false
    }

    onSearchTypeChanged: {
        if ( searchType == roadSearch ) {
            console.log( "도로명 searchType: " + searchType )
            bar.setName( 2, roadScdTitle )
        }
        else if ( searchType == lotSearch ) {
            console.log( "지번 searchType: " + searchType )
            bar.setName( 2, lotScdTitle )
        }

        reset( 0 )

        sendEvent(AddressSearchViewEvent.AddressSearchRequested, {searchType:searchType, type:'lcd', from:'bar'} )
    }    

    function doInitialRequest() {
        sendEvent(AddressSearchViewEvent.AddressSearchRequested, {searchType:searchType, type:'lcd'} )
    }

    function updateZoneAndCategoryBar() {
        if ( active ) {
            sendEvent(AddressSearchViewEvent.RequestViewList)
//            if (isShowTopView) {
//                return
//            }
            sendEvent(AddressSearchViewEvent.SelectZone1Title, zone1Title)
            setupToggleMenu()
            var barName = bar.name( 0 )
            if ( barName == '광역시/도' || barName == 'Gwangyeoksi/Do' ) {
                bar.setName( 0, lcdTitle )
            }
            barName = bar.name( 1 )
            if ( barName == '시/군/구' || barName == 'Si/Gun/Gu' ) {
                bar.setName( 1, mcdTitle )
            }
            barName = bar.name( 2 )
            if ( barName == '읍/면/동' || barName == 'Dong/Eup/Myeon' ) {
                bar.setName( 2, lotScdTitle )
            }
            if ( barName == '도로명' || barName == 'Road name' ) {
                bar.setName( 2, buildTitle )
            }
        }
    }

    // 17.6.29 hskim dynamic localization
    function retransalteUi(lang) {
        //console.debug(lang)
        if (lang) {
            root.lang = lang
        }
        updateZoneAndCategoryBar();
    }

    // chunjae.lee 2018.04.09 - 로컬검색시에 '아파트'가 체크된 경우나 input box에 입력이 있는 경우 '검색'버튼 활성화
    property bool online: false
    function __changedOnlineStatus(online) {
        root.online = online
        sendEvent(AddressSearchViewEvent.UpdateKeyboard, {"searchBtnAlwaysEnable": (online || lotInputWrap.aptSelected) ? true : false })
     }

    function __processKeyboard(key, text) {
        switch(key) {
        case CustomKeyboard.CodeDelete:     break
        case CustomKeyboard.CodeCancel:
            sendEvent(AddressSearchViewEvent.HideKeyboard)
            if ( bldInputWrap.visible ) {
                bldInputWrap.visible = false
            }
            else if ( lotInputWrap.visible ) {
                lotInputWrap.visible = false
            }
            else {
                root.close()
            }
            break
        case CustomKeyboard.CodeSymbol:     break
        case CustomKeyboard.CodeKorean:     break
        case CustomKeyboard.CodeEnglish:    break
        case CustomKeyboard.CodeShift:      break
        case CustomKeyboard.CodeClear:      break
        case CustomKeyboard.CodeSetting:    break
        case CustomKeyboard.CodeSearch:     break
        case CustomKeyboard.CodeConfirm:
            showSearchResultView(text )
            break
        case CustomKeyboard.CodeSpace:      break
        default:
            break
        }
    }

    property bool keyboardDown: false
    function __pullDownKeyboard() {
        keyboardDown = true
        sendEvent(AddressSearchViewEvent.HideKeyboard)
    }

    function __pullUpKeyboard() {
        keyboardDown = false
        if ( bldInputWrap.visible || lotInputWrap.visible ) {
            showKeyboard(keyboard)
        }
    }

    property var keyboard: null
    function showKeyboard(keyboard){
        root.keyboard = keyboard
        sendEvent(AddressSearchViewEvent.ShowKeyboard, {
              "owner": "AddressSearchView",
              "x":keyboard.x,
              "y":keyboard.y,
              "z":keyboard.z+1,
              "width":keyboard.width,
              "height":keyboard.height,
              //v 1.32 - 번지수나 건물번호 입력되어 있지 않을 경우, 키보드의 검색 버튼을 Dim (Disable함)
              //"searchBtnAlwaysEnable": online ? true : false ,
              "pressed": CustomKeyboard.symbolKeybd,
              "dontUseEditBox":true,
              "numberKeyboard":true
              })
    }

    property var timerCloseCheck : null
    function showSearchResultView(text){
        var keyword = bar.value( 2 )['name']

        console.log(
            '[AddressSearchView]' +
            'text:' + text + ' ' +
            'keyword:' + keyword + ' ' +
            'selected code:' + selectedModel.code + ' ' +
            'name:' + selectedModel.name + ' ' +
            'jibunStIdx:' + selectedModel.jibunStIdx + ' ' +
            'jibunCnt:' + selectedModel.jibunCnt + ' ' +
            'aptStIdx:' + selectedModel.aptStIdx + ' ' +
            'aptCnt:' + selectedModel.aptCnt + ' ' +
            'newAddrStIdx:' + selectedModel.newAddrStIdx + ' ' +
            'newAddrCnt:' + selectedModel.newAddrCnt + ' ' +
            'jibunRicode:' + selectedModel.jibunRicode
            )

        var params = {
            name: selectedModel.name,
            jibunStIdx: selectedModel.jibunStIdx,
            jibunCnt: selectedModel.jibunCnt,
            aptStIdx: selectedModel.aptStIdx,
            aptCnt: selectedModel.aptCnt ,
            newAddrStIdx:  selectedModel.newAddrStIdx,
            newAddrCnt: selectedModel.newAddrCnt,
            jibunRicode: selectedModel.jibunRicode,
            // 18.5.11 hskim https://tde.sktelecom.com/pms/browse/TMCRSM-620
            lcdName: lcdName,
            mcdName: mcdName,
            scdName: scdName,
        }

        var data = ({})
        data[CommonConstants.Url        ] = CommonConstants.url(ViewRegistry.SearchResultView)
        data[CommonConstants.Caller     ] = caller
        data[CommonConstants.Owner      ] = objectName
        data[CommonConstants.Text       ] = text
        data[CommonConstants.Keyword    ] = keyword
        data[SearchConstants.SearchType ] = CommonConstants.Address
        data[SearchConstants.IsRoad     ] = (searchType == roadSearch)
        data[SearchConstants.IsSan      ] = lotInputWrap.sanCheckButton.selected
        data[SearchConstants.IsApt      ] = lotInputWrap.aptCheckButton.selected
        data[SearchConstants.AddrCode   ] = selectedModel.code
        if (caller === CommonConstants.objectName(ViewRegistry.BookmarkPageView) || caller === CommonConstants.objectName(ViewRegistry.NaviMenuView)) {
            data[SearchConstants.Favorite] = owner && owner.indexOf(CommonConstants.objectName(ViewRegistry.BookmarkPageView)) >= 0
            data[SearchConstants.IsHome] = owner && owner.indexOf('/home') >= 0
            data[SearchConstants.IsOffice] = owner && owner.indexOf('/office') >= 0
        }
        data[SearchConstants.Params     ] = params
        data[CommonConstants.Visible    ] = SearchConstants.DialogNoResult

        //data[CommonConstants.Z          ] = CommonConstants.MAX_INT_VALUE
        data[CommonConstants.ReFocus    ] = true
        data[CommonConstants.ObjectName ] = CommonConstants.objectName(ViewRegistry.SearchResultView)
        sendEvent(AddressSearchViewEvent.UpdateView, data )

        sendEvent(AddressSearchViewEvent.CreateView, data )
    }

    function updateData( respData, reqType, reqFrom, reqCode, reqName, shortcutMap ) {
        //QmlAppHelper.debug( 'reqType: ' + reqType + " " + " reqFrom: " + reqFrom + " reqName: " + reqName )

        roadListShortcut.visible = false
        if ( reqType === 'lcd' ) {
            listModel = respData
            if ( reqFrom === "bar" ) {
                bar.setName(0, lcdTitle )
                bar.setName(1, mcdTitle )
            }
        }
        else if ( reqType === 'mcd' ) {
            lcdName = reqName
            listModel = respData
            if ( reqFrom === "bar" ) {
                bar.setName(1, mcdTitle )
            }
            else if ( reqFrom === "list" ) {
                bar.setName(0, reqName )
                bar.setValue(0, {type:reqType,code:reqCode,name:reqName} )
                bar.currentIndex = 1
            }
        }
        else if ( reqType === 'road' ) {
            mcdName = reqName
            roadListShortcut.visible = true
            roadListModel = respData
            if ( reqFrom === "list" ) {
                bar.setName(1, reqName )
                bar.setValue(1, {type:reqType,code:reqCode,name:reqName} )
                bar.currentIndex = 2
            }

            roadListShortcutMap = shortcutMap;
        }
        else if ( reqType === 'dong' ) {
            mcdName = reqName
            roadListShortcut.visible = false
            listModel = respData
            if ( reqFrom === "list" ) {
                bar.setName(1, reqName )
                bar.setValue(1, {type:reqType,code:reqCode,name:reqName} )
                bar.currentIndex = 2
            }
        }
        else if ( reqType === 'bld' ) {
            bldListModel = respData
        }
        else if ( reqType === 'lot' ) {
            lotListModel = respData
        }
    }

    function setupToggleMenu() {
        if (searchType == roadSearch || searchType == lotSearch) {
            //[SOFTKEYINDEX]
            //var select = searchType == roadSearch ? kSoftButtonRoadName : kSoftButtonLotNo
            var select = searchType == roadSearch ? softKeyOptions[0] : softKeyOptions[1]
            var args = {'menu':softKeyOptions, 'select':select}
            sendEvent(AddressSearchViewEvent.ChangeSoftKey, args)
        } else {
            console.log("illigal address search type error !!!")
        }
    }

    property var stateMap: []

    function backToLastState() {
        if ( bar.currentIndex > 0 )  {
            bar.currentIndex--

            //console.debug( 'bar.currentIndex: ' + bar.currentIndex)

            listModel = stateMap[bar.currentIndex][0]
            roadListModel = stateMap[bar.currentIndex][1]
            bar.setState( stateMap[bar.currentIndex][2] )
            if ( bar.currentIndex < 2 ) { // si, gun
            	// chunjae.lee s 2018.04.12 - init함수 호출 추가
                bldInputWrap.init(bldInputWrap.visible = false)
                lotInputWrap.init(lotInputWrap.visible = false)
                // chunjae.lee e 2018.04.12 - init함수 호출 추가
                if ( searchType == roadSearch ) {
                    siGunGuListView.visible = true
                    roadListView.visible = false
                }
            }
        }

        retransalteUi()
    }

    function saveCurrentState() {
        var newStateMap = stateMap
        newStateMap[bar.currentIndex] = [listModel,roadListModel,bar.getState()]
        stateMap = newStateMap
    }

    function onSoftkeyBackClicked() {
        QmlAppHelper.debug("bar.currentIndex:" + bar.currentIndex)
        if ( bar.currentIndex == 0 )  {

           // chunjae.lee s 2018.04.11
           if (caller === CommonConstants.objectName(ViewRegistry.BookmarkPageView)) {
             sendEvent(AddressSearchViewEvent.ReturnToFavorite)
           } else if (caller === CommonConstants.objectName(ViewRegistry.RouteSettingView)) {
             sendEvent(AddressSearchViewEvent.ReturnToViaSetting)
           } else if (caller === CommonConstants.objectName(ViewRegistry.NaviMenuView)) {
             sendEvent(AddressSearchViewEvent.ReturnToNaviMenu)
           }
           // chunjae.lee e 2018.04.11

           root.close()
        }
        else {
            bldInputWrap.visible = false
            lotInputWrap.visible = false
            roadListShortcut.visible = false
            backToLastState()
        }
    }

    function onSoftkeyMenuClicked(menuName) {
        switch (menuName) {
        case buildTitle:
            searchType = roadSearch
            console.log('=>도로명')
            break
        case lotTitle:
            searchType = lotSearch
            console.log('=>지번')
            break
        }
    }

    Csw {
        id: csw
        property var focusedObj
        section: Csw.Section_AddressSearchSiDo
        sequence: 0
        //enable: !(roadListView.visible || bldInputWrap.visible || lotInputWrap.visible)
        property bool focused

        function changeFocusedObj(obj) {
            var oldFocusedObj = focusedObj
            if (oldFocusedObj) {
                oldFocusedObj.cswFocused = false
            }

            focusedObj = obj
            focusedObj.cswFocused = focused
            console.log('[CSW][AddressSearchView]changeFocusedObj ' + obj.objectName + ' focused: ' + focused)
        }

        onFocused: {
            focused = true
            if (bldInputWrap.visible) {
                changeFocusedObj(bldInputWrap)
            }
            else if (roadListView.visible) {
                changeFocusedObj(roadListView)
            }
            else if (lotInputWrap.visible) {
                changeFocusedObj(lotInputWrap)
            }
            else {
                changeFocusedObj(siGunGuListView)
            }
        }

        onLostFocusd: {
            console.log('[CSW][AddressSearchView]onLostFocusd')
            focused = false
            focusedObj.cswFocused = false
        }

        onSpinCcwPressed: {
            console.log('[CSW][AddressSearchView]ccw')
            focusedObj.onCswCcw()
        }
        onSpinCwPressed: {
            console.log('[CSW][AddressSearchView]cw')
            focusedObj.onCswCw()
        }
        onLeftPressed: {
            focusedObj.onCswLeft()
        }
        onRightPressed: {
            focusedObj.onCswRight()
        }
        onSelected: {
            focusedObj.onCswSelect( focusedObj.cswFocusedIndex )
        }
    }

    Rectangle {
        width: parent.width
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        color: GlobalConstants.kBasicBackgoundColor
        //color: 'blue'

        AddressSearchViewCategoryBar {
            id: bar
            layout: root.layout
            width: parent.width
            height: layout.barHeight

            onCategoryClicked: {
                var val = value( currentIndex )                
                if ( currentIndex == 0 ) {
                    reset(0)
                    sendEvent(AddressSearchViewEvent.AddressSearchRequested, {searchType:searchType, type:'lcd', code:val['code'], name:val['name'], from:'bar'} )
                }
                else if ( currentIndex == 1 ) {
                    reset(1)
                    var val = value( 0 )
                    sendEvent(AddressSearchViewEvent.AddressSearchRequested, {searchType:searchType, type:'mcd', code:val['code'], name:val['name'], from:'bar'} )
                }
            }
        }

        Item {
            id: listWrap
            anchors.top: bar.bottom
            anchors.bottom: parent.bottom
            width: parent.width

            AddressSearchViewSiGunGuList {
                id: siGunGuListView
                objectName: "siGunGuListView"
                owner: root
                csw: csw
                layout: root.layout
                model: listModel
                x: layout.listLeftMargin
                y: layout.listTopMargin
                width: layout.listWidth
                height: layout.listHeight
            }

            AddressSearchViewRoadList {
                id: roadListView
                owner: root
                csw: csw
                layout: root.layout
                shortcut: roadListShortcut
                siGunGuListView: siGunGuListView
                width: parent.width
                anchors.fill: parent
                visible: false
            }

            AddressSearchViewRoadListShortcut {
                id: roadListShortcut
                layout: root.layout
                owner: roadListView
                csw: csw
                width: layout.shortcutWidth
                anchors.top: parent.top
                anchors.topMargin: layout.shortcutTopMargin
                anchors.right: parent.right
                anchors.rightMargin: layout.shortcutRightMargin
                visible: false
                onShortcutClicked: {
                    if ( roadListShortcutMap[shortcut] !== undefined ) {
                        roadListView.positionViewAtIndex( roadListShortcutMap[shortcut], ListView.Beginning )
                    }
                }
            }
        }
    }

    AddressSearchViewBldInput {
        id: bldInputWrap
        visible: false
        width: parent.width
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        color: GlobalConstants.kBasicBackgoundColor
        layout: root.layout
        owner: root
        bldListModel: root.bldListModel

        onVisibleChanged: {
            if ( visible ) {
                bldInputWrap.showKeyboard()
            }
        }
    }

    AddressSearchViewLotInput {
        id: lotInputWrap
        visible: false
        width: parent.width
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        color: GlobalConstants.kBasicBackgoundColor
        layout: root.layout
        owner: root
        lotListModel: root.lotListModel
        lang: root.lang

        // chunjae.lee 2018.04.09 - 로컬검색시에 '아파트'가 체크된 경우나 input box에 입력이 있는 경우 '검색'버튼 활성화
        property bool aptSelected : aptCheckButton.selected
        onAptSelectedChanged: {
             sendEvent(AddressSearchViewEvent.UpdateKeyboard, {"searchBtnAlwaysEnable": (online || aptSelected) ? true : false })
        }

        onVisibleChanged: {
            if ( visible ) {
                lotInputWrap.showKeyboard()
            }
        }
    }
}
