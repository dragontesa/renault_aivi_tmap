import QtQuick 2.0
import QtGraphicalEffects 1.0
import ViewRegistry 1.0
import CommonConstants 1.0
import ZoneConstants 1.0
import SearchConstants 1.0
import RecentSearchPageViewEvent 1.0
import SearchTabViewEvent 1.0
import Csw 1.0

import "../../singleton"
import "../../component"
import "../../view"
import "../../view/util"
import "../../view/search"
import "../../GlobalConstants.js" as GlobalConstants
import "../../component/internal/Keyboard.js" as CustomKeyboard
import "../../view/util/CommonUtils.js" as CommonUtils


PopupView {
    id: searchRoot
    width: parent ? parent.width : undefined
    height: parent ? parent.height : undefined

    property var caller: null
    property int  index: 0
    property int currentCnt : 0
    property int requestMax : 20
    property int  keyboardHeight: 419
    property var result: null
    property var editView: null
    property bool isCompleted: false
    property bool focusCsw: false
    property bool isShowTextSearchView: false
    property bool isShowSearchResultView: false
    property bool reFocus           : false
    property bool syncLock: false

    //Timer
    property var timer : null

    //ID
    property var listView: null
    property var message: null
    property var keyboard: null

    readonly property string kNoDataMessage: qsTr("최근 검색어가 없습니다.") + Tr.d
    readonly property string kEditMessage: qsTr("최근 검색 편집하기") + Tr.d
    readonly property var kHeader: {
              "Id": 0, // 아이디
              "Keyword": 1 // 검색어
    }

    property color highlightColor: MultiSenseCoordinator.kAmbientColor
    property real highlightOpacity: 1.0

    // TMCRSM-2359.
    // RecentSearchEditView에서 이 뷰의 visible을 false로 처리할 때
    // onVisibleChanged에서 키보드 감추는 코드를 호출하지 못하도록 하기 위한 용도
    // 이유: hideKeyboard가 중복 호출 됨.

    property bool dontHideKeyboard: false

    readonly property int searchMaxLen: 20

    // 17.10.23 hskim
    function showView( args )   {
        sendEvent(RecentSearchPageViewEvent.CreateView, args)
    }

    function closeView(index) {
        console.log( "RecentSearchPageView.qml closeView(" + index + ")" )

        isCompleted = false
        sendEvent(RecentSearchPageViewEvent.BackwardClicked, caller)
        hideKeyboard()

        // 17.11.7 hskim commented the code below not to recreate search tab view
        //sendEvent(RecentSearchPageViewEvent.SelectZone1TextInput)
        sendEvent(RecentSearchPageViewEvent.ActivatedForNavi)
        var data = ({})
        data[CommonConstants.ObjectName] = CommonConstants.objectName(ViewRegistry.SearchTabView)
        sendEvent(RecentSearchPageViewEvent.CloseView, data)
    }

    function goBack() {
        closeView(searchRoot.index)
    }

    function __changedSyncLock(lock) {
        setSyncLock(lock)
    }

    function isSyncLock() {
        return syncLock
    }

    function setSyncLock(lock) {
        syncLock = lock
    }

    function __processKeyboard(key, text) {
        if (isSyncLock())
            return
        switch(key) {
        case CustomKeyboard.CodeCancel:
            goBack()
            break
        case CustomKeyboard.CodeSymbol:
        case CustomKeyboard.CodeKorean:
        case CustomKeyboard.CodeEnglish:
        case CustomKeyboard.CodeShift:
        case CustomKeyboard.CodeClear:
        case CustomKeyboard.CodeSetting:
        case CustomKeyboard.CodeSearch:
        case CustomKeyboard.CodeSpace:
        case CustomKeyboard.CodeDelete:
            break
        case CustomKeyboard.CodeConfirm:
            showSearchResultView(SearchConstants.normalized(text))
            break
        default:
            showTextSearchView(text)
            break
        }
    }

    function showRecentSearchEditView(){        
        var data = ({})
        data[CommonConstants.Url] = CommonConstants.url(ViewRegistry.RecentSearchEditView)
        data[CommonConstants.Caller] = caller
        data[SearchConstants.SearchPageObj] = searchRoot
        data[SearchConstants.Result] = result
        data[SearchConstants.Request] = true
        showView(data)

        var args = ({})
        args[CommonConstants.Owner         ] = CommonConstants.objectName(ViewRegistry.RecentSearchPageView)
        args[CommonConstants.DontUseEditBox] = true
        sendEvent(RecentSearchPageViewEvent.HideKeyboard, args)
    }

    function showTextSearchView(text){
        if (text.length <= 0)
            return
        if (isSyncLock()) {
            return
        }
        setSyncLock(true)
        var data = ({})
        data[CommonConstants.Url        ] = CommonConstants.url(ViewRegistry.TextSearchView)
        data[CommonConstants.Caller     ] = CommonConstants.objectName(ViewRegistry.SearchTabView)
        data[CommonConstants.Owner      ] = CommonConstants.objectName(ViewRegistry.RecentSearchPageView)
        data[SearchConstants.Query      ] = text
        data[CommonConstants.Visible    ] = SearchConstants.AlwaysShow

        //data[CommonConstants.Z          ] = CommonConstants.MAX_INT_VALUE
        data[CommonConstants.ReFocus    ] = true
        data[CommonConstants.ObjectName ] = CommonConstants.objectName(ViewRegistry.TextSearchView)
        sendEvent(RecentSearchPageViewEvent.UpdateView, data)

        showView(data)
    }

    function getLimitText(text, maxLen) {
        if (text.length > maxLen) {
            text = CommonUtils.substring(text, 0, maxLen)
        }
        return text
    }

    //2017.11.10 cjlee - 키보드가 사라질때 Zone1입력박스에 표시    
    property string searchText : CommonConstants.Empty
    property var timerCloseCheck : null
    function showSearchResultView(text){
        text = getLimitText(text, searchMaxLen)
        text = CommonUtils.trim(text)
        if (text.length <= 0) {
            return
        }

        if (isSyncLock()) {
            return
        }
        setSyncLock(true)

        hideKeyboard()
        searchText = text

        var data = ({})
        data[CommonConstants.Url] = CommonConstants.url(ViewRegistry.SearchResultView)
        data[CommonConstants.Caller] = CommonConstants.objectName(ViewRegistry.SearchTabView)
        data[CommonConstants.Owner] = CommonConstants.objectName(ViewRegistry.RecentSearchPageView)
        data[SearchConstants.Query] = text
        data[SearchConstants.SearchType] = CommonConstants.Name
        data[SearchConstants.Visible   ] = SearchConstants.DialogNoResult

        //data[CommonConstants.Z         ] = CommonConstants.MAX_INT_VALUE
        data[CommonConstants.ReFocus   ] = true
        data[CommonConstants.ObjectName] = CommonConstants.objectName(ViewRegistry.SearchResultView)
        sendEvent(RecentSearchPageViewEvent.UpdateView, data)

        showView(data)
    }

    function requestViewData()
    {
        var data = ({})
        data[CommonConstants.Max] = requestMax
        data[CommonConstants.Offset] = currentCnt
        sendEvent( RecentSearchPageViewEvent.RequestRecentSearch, data )
    }


    function pushRecentSearchList(newList, cnt)
    {
        if (result == null) result = {}

        // remove last edit button item
        if (listModel.count > 0)
            listModel.remove(listModel.count-1);

        for (var p in newList)
        {
            var dest = newList[p];
            result[currentCnt] = dest
            if ( listModel.count > currentCnt )
            {
                listModel.set(
                            currentCnt,
                            {
                                seq      : dest[kHeader.Id],
                                text  : dest[kHeader.Keyword]
                            });
            }
            else
            {
              listModel.append (
                  {
                      seq      : dest[kHeader.Id],
                      text  : dest[kHeader.Keyword]
                  });
            }

            currentCnt++;
        }

        /** edit view takes constantly all items  because view's scrollbars have to move to end
        if (editView !== null && editView !== undefined)
            editView.pushRecentDestList(newList);
        */
        if (listModel.count > 0)
            listModel.append({seq:0, text: kEditMessage})

        listView.model = listModel
        listView.currentIndex = -1
        message.visible = (listModel.count == 0)

        //console.log("push recent search: cnt:"+cnt+",model cnt:"+ listModel.count+"(current:"+currentCnt+")");
    }

    /** the list data is sync both list and edit view. this called from edit view */
    function updateListFromEditView()
    {
        listModel.clear();
        listView.model = null
        currentCnt = 0
        result = null
        requestViewData()
        listView.model = listModel
        listView.currentIndex = -1
    }

    function showKeyboard(keyboard){
        if (isShowTextSearchView) return
        if (!isCompleted) return
        var data = ({})
        // 17.10.26 hskim changed owner from SearchTabView as its own's object name
        data[CommonConstants.Owner] = CommonConstants.objectName(ViewRegistry.RecentSearchPageView)
        data[CommonConstants.X] = keyboard.x
        data[CommonConstants.Y] = keyboard.y
        data[CommonConstants.Z] = keyboard.z + 1
        //data[CommonConstants.Width] = keyboard.width
        data[CommonConstants.Width] = GlobalConstants.kScreenWidth
        data[CommonConstants.Height] = keyboard.height
        data[CommonConstants.Text] = ""
        data[CommonConstants.Pressed] = CustomKeyboard.hangleKeybd
        sendEvent(RecentSearchPageViewEvent.ShowKeyboard, data)
    }

    function visibleKeyboard(visible) {
        if (isShowTextSearchView) return
        if (visible) {
            keyboard.height = keyboardHeight
            showKeyboard(keyboard)
        } else {
            keyboard.height  = 0
            hideKeyboard()
        }
    }

    function hideKeyboard(){
        if (isShowTextSearchView) return
        var data = ({})
        data[CommonConstants.Owner] = CommonConstants.objectName(ViewRegistry.RecentSearchPageView)
        data[CommonConstants.Text] = searchText
        sendEvent(RecentSearchPageViewEvent.HideKeyboard, data)
    }

    function retranslateUi() {
        if (listModel.count > 0) {
            listModel.remove(listModel.count-1);
            listModel.append({seq:0, text: kEditMessage})
        }
    }

    Component.onCompleted: {
        isCompleted = true
    }

    Component.onDestruction: {
        timer = CommonUtils.destroy(timer)
    }

    onVisibleChanged: {
        sendEvent(RecentSearchPageViewEvent.RequestViewList)
        if ( ! dontHideKeyboard ) {
            visibleKeyboard(visible)
        } else {
            dontHideKeyboard = false
        }
    }

    function __onResultViewList(result) {
        isShowTextSearchView = false
        isShowSearchResultView = false
        for(var i= 0; i<result.length; i++) {
            if (CommonUtils.isEqual(result[i][CommonConstants.ResultViewId], ViewRegistry.TextSearchView)) {
                isShowTextSearchView = result[i][CommonConstants.ResultViewVisible]
            } else if (CommonUtils.isEqual(result[i][CommonConstants.ResultViewId], ViewRegistry.SearchResultView)) {
                isShowSearchResultView = result[i][CommonConstants.ResultViewVisible]
            }
        }
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
        loader.sourceComponent = main
    }

    onActivated: {
        setSyncLock(false)
        sendEvent(RecentSearchPageViewEvent.RequestViewList)
        if (isShowTextSearchView || isShowSearchResultView) {
            viewDeactivated()
            return
        }
        if (!visible) {
            return
        }
        sendEvent(RecentSearchPageViewEvent.Activated)
        sendEvent(RecentSearchPageViewEvent.ResetCsw)

        var args = [ csw ]
        sendEvent(RecentSearchPageViewEvent.RegistCsw, args)

        sendEvent(RecentSearchPageViewEvent.SelectZone1TextInput)
        if (!keyboardDown) {
            visibleKeyboard(true)
        }
        if (focusCsw) {
            sendEvent(RecentSearchPageViewEvent.FocusCsw, csw.objectName)
            focusCsw = false
        }
    }

    onDeactivated: {
        if (keyboard.height) {
        } else {
            sendEvent(RecentSearchPageViewEvent.Deactivated)
        }
    }

    function __doSoftKeyFunction(button, menuName, menuIndex) {
      if (button === "back") {
          goBack()
      }
    }

    property bool keyboardDown: false
    function __pullDownKeyboard() {
        if (isShowTextSearchView)
            return
        keyboardDown = true
        visibleKeyboard(false)
    }

    function __pullUpKeyboard() {
        if (isShowTextSearchView)
            return
        keyboardDown = false
        visibleKeyboard(true)
    }

    //main
    Loader {
         id: loader
         width: parent.width
         height: parent.height + GlobalConstants.kScreenZone3Height
         onLoaded: {
             requestViewData()
         }
    }

    Component {
        id: main
        Rectangle{
            width: parent.width
            height: parent.height
            color: (listModel.count) ? GlobalConstants.kListNormalColor : "gray"
            Column {
                width: parent.width
                height: parent.height
                ListView {
                    id: listView
                    Component.onCompleted: searchRoot.listView = this
                    width: parent.width
                    height: parent.height - keyboard.height
                    property int rowHeight: GlobalConstants.kListTextItemHeight
                    cacheBuffer: rowHeight * 2
                    clip: true
                    model: listModel
                    delegate: listDelegate
                    currentIndex: -1
                    property PressBar highlightBar: null
                    highlight: Component {
                        PressBar {
                            currentItem: listView.currentItem
                            leftMargin: GlobalConstants.kListBackgroundLeftMargin
                            rightMargin: GlobalConstants.kListBackgroundRightMargin
                            Component.onCompleted: listView.highlightBar = this
                        }
                    }
                    highlightFollowsCurrentItem: false

                    footer: Item {
                        width: listView.width; height: (0<keyboard.height) ? 0 : GlobalConstants.kListViewFooterHeight
                    }

                    Image {
                          width: parent.width
                          height: 30
                          y: parent.y + (parent.height - height)
                          visible: scrollBar.heightRatio < 1.0
                          sourceSize.width: width
                          sourceSize.height: height
                          source: Resource.imageUrl("zone/zone2_img_mask.png")
                    }

                    TScrollBar {
                        id: scrollBar
                        flickable: parent
                        width: 2
                        color: MultiSenseCoordinator.kAmbientColor
                        grooveColor: GlobalConstants.kScrollBarBackColor
                        rightMargin: GlobalConstants.kScrollBarRightMargin
                    }

                    onAtYEndChanged: {
                        // fix TMCRSM-3522,TMCRSM-4394, don't check flicking state
                        if ( contentHeight> 0 
                            && atYEnd 
                            && contentY+height >= contentHeight) {
                            // next list up
                            requestViewData()
                        }
                    }
                    onMovementStarted: {
                        sendEvent(RecentSearchPageViewEvent.PulldownKeyboard)
                    }
                }
                Item {
                    id: keyboard
                    Component.onCompleted: searchRoot.keyboard = this
                    width: parent.width
                    height: 0
                }
            }
            Rectangle {
                id: message
                Component.onCompleted: searchRoot.message = this
                width: parent.width
                height: parent.height
                visible: false
                color: GlobalConstants.kBasicBackgoundColor
                Text {
                    width: parent.width
                    height: parent.height - keyboard.height
                    anchors.top: parent.top
                    anchors.topMargin: 0
                    horizontalAlignment: Qt.AlignHCenter
                    verticalAlignment: Qt.AlignVCenter
                    font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(GlobalConstants.kGeneralMessageFontSize) }
                    color: GlobalConstants.kListTextColor
                    text: kNoDataMessage
                }
            }
        }
    }

    ListModel { id: listModel   }

    Component {
        id: listDelegate
            Column{
                id: column
                width: listView.width
                height: listView.rowHeight
                
                states: [
                    State {
                        name: 'normal'
                        PropertyChanges {
                            target: label
                            width: label.parent.width
                            elide: Text.ElideRight
                            color: GlobalConstants.kListTextColor
                        }
                        PropertyChanges {
                            target: marqueeHilight
                            marquee: false
                        }
                    },
                    State {
                        name: 'pressed'
                        PropertyChanges {
                            target: listView
                            currentIndex: index
                        }
                    },
                    State {
                        name: 'focused'
                        PropertyChanges {
                            target: label
                            width: label.contentWidth
                            elide: Text.ElideNone
                            color: "#000000"
                        }
                        PropertyChanges {
                            target: marqueeHilight
                            marquee: true
                        }
                    }
                ]

                property var itemClicked: function() {
                    csw.forceLostFocus()
                    listView.currentIndex = model.index
                    highlightColor = Qt.binding(function(){ return MultiSenseCoordinator.kAmbientColor})
                    var sleep =  GlobalConstants.kPressedColorExposureTime
                    timer = CommonUtils.destroy(timer)
                    timer = CommonUtils.delay(searchRoot, sleep, function() {
                        try {
                            //highlightColor = CommonConstants.Transparent
                            if (model.index == listModel.count - 1)
                                showRecentSearchEditView()
                            else{
                                showSearchResultView(model.text)
                            }
                        } catch(e) {
                        }
                    })
                }

                Item {
                    id: mouseArea
                    width: parent.width
                    height: parent.height - splitBar.height
                    TMouseArea {
                        width: parent.width
                        height: parent.height

                        PressBarMouseSync2 {
                            targetItem: column
                        }
                        onClicked: {
                            itemClicked()
                        }
                    }
                    Item {
                        id: marqueeItem
                        x: GlobalConstants.kListTextLeftMargin;
                        width: parent.width - (GlobalConstants.kListTextLeftMargin + GlobalConstants.kListLineRightMargin)
                        height: parent.height
                        clip:true
                        Text {
                            id: label
                            width: parent.width
                            horizontalAlignment: (model.index == listModel.count - 1) ? Qt.AlignHCenter : Qt.AlignLeft
                            anchors.verticalCenter: parent.verticalCenter
                            font { family: GlobalConstants.kFontFamily;/* pointSize: GlobalConstants.kListTextFontSize * 0.5*/ }
                            font.pixelSize: (model.index != listModel.count - 1 ) ?  Resource.pixelToDp(GlobalConstants.kOneLineListTextFontSize) :  Resource.pixelToDp(30)
                            color: GlobalConstants.kListTextColor
                            elide: Text.ElideRight
                            text: model.text
                        }
                        TMarquee {
                            id:marqueeHilight
                            parentObj: parent
                            textObj: label
                            marquee: (listView.currentIndex == model.index) && parent.width<textObj.width
                            onMarqueeFinished: {
                                marquee = false
                                marquee =  (listView.currentIndex == model.index) && parent.width<textObj.width
                            }
                            onTextMoved: gradientRect.visible = true
                            onTextStopped: gradientRect.visible = false
                        }
                    }

                    LinearGradient {
                        id: gradientRect
                        anchors.fill: marqueeItem
                        gradient: Gradient {
                            GradientStop { position: 0; color: Qt.rgba(0,0,0,1) }
                            GradientStop { position: 16/gradientRect.width; color: Qt.rgba(0,0,0,0) }
                            GradientStop { position: (gradientRect.width-16)/gradientRect.width; color: Qt.rgba(0,0,0,0) }
                            GradientStop { position: 1; color: Qt.rgba(0,0,0,1) }
                        }
                        start: Qt.point(0, 0)
                        end: Qt.point(gradientRect.width, 0)
                        visible: false
                    }
                }

                Rectangle {
                   id: splitBar
                   x: GlobalConstants.kListLineLeftMargin
                   width: parent.width - (GlobalConstants.kListLineLeftMargin  + GlobalConstants.kListLineRightMargin)
                   height: 1
                   color: GlobalConstants.kListLineColor
                }
            }
    }


    Csw {
        id: csw
        objectName: 'RecentSearchCsw'
        section: Csw.Section_List
        sequence: 0
        enable: searchRoot.visible

        function goPrev() {
            if (0 <listView.currentIndex) {
                listView.currentItem.state = "normal"
                --listView.currentIndex
                listView.positionViewAtIndex(listView.currentIndex, ListView.Contain)
                listView.highlightBar.isFocused = true  // fix the item's state doesnt' get effect when a delgate out of viewport is instanced
                listView.currentItem.state = "focused"
            }
        }
        function goNext() {
            if (listView.currentIndex < listModel.count -1) {
                listView.currentItem.state = "normal"
                ++listView.currentIndex
                listView.positionViewAtIndex(listView.currentIndex, ListView.Contain)
                listView.highlightBar.isFocused = true
                listView.currentItem.state = "focused"
            }
        }

        onFocused: {
            searchRoot.focus = true
            if (0 == listModel.count) {
                forceNextSection()
            } else {
                if (listView.currentIndex < 0)
                    listView.currentIndex = 0
                if (listView.highlightBar !== null)
                   listView.highlightBar.isFocused = true
                listView.currentItem.state = "focused"
            }
        }
        onLostFocusd: {
            searchRoot.focus = false
            if (listView.highlightBar !== null)
              listView.highlightBar.isFocused = false
            listView.currentItem.state = "normal"
            listView.currentIndex = -1
       }
        onLeftPressed: {
        }

        onRightPressed: {
            listView.currentItem.state = "normal"
            listView.highlightBar.isFocused = false
            sendEvent( RecentSearchPageViewEvent.ChangeSearchTabViewPage,
                {isCswControl:true, page:1})
        }

        onSpinCcwPressed: {
            goPrev()
        }
        onSpinCwPressed: {
            goNext()
        }
        onSelected: {
            if (listView.currentItem !== undefined) {
               listView.currentItem.itemClicked()
            }
        }
    }
}

