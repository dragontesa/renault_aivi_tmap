import QtQuick 2.0
import ViewRegistry 1.0
import CommonConstants 1.0
import ZoneConstants 1.0
import SearchConstants 1.0
import RecentSearchPageViewEvent 1.0
import SearchTabViewEvent 1.0
import Csw 1.0

import "../../component"
import "../../view"
import "../../view/search"
import "../../GlobalConstants.js" as GlobalConstants
import "../../component/internal/Keyboard.js" as CustomKeyboard
import "../../view/util/CommonUtils.js" as CommonUtils

import TestRecentSearchPageViewEvent 1.0

PopupView {
    id: searchRoot
    width: parent ? parent.width : undefined
    height: parent ? parent.height : undefined
    objectName: 'RecentSearchPageView'

    property var caller: null
    property int  index: 0
    property int currentCnt : 0
    property int requestMax : 20
    property int  keyboardHeight: 150
    property alias searchKeyboard: keyboard
    property var result: null
    property var editView: null
    property bool isCompleted: false

    property var textEnum: (function() {
        textEnum = CommonUtils.createEnum(textEnum)
        CommonUtils.appendEnum(textEnum, "NoData",       [CommonConstants.Name], [qsTr("검색 기록이 없습니다")])
        CommonUtils.appendEnum(textEnum, "RecentEdit",   [CommonConstants.Name], [qsTr("최근검색어 편집하기")])

    }())
    property var textArr : (function() {
        textArr = []
        CommonUtils.addArr(textEnum,  CommonConstants.Name, textArr)
    }())

    property var resEnum: (function() {
        resEnum = CommonUtils.createEnum(resEnum)
        CommonUtils.appendEnum(resEnum,"Id"                 ,[CommonConstants.Name], [qsTr("아이디")] )
        CommonUtils.appendEnum(resEnum,"Keyword"            ,[CommonConstants.Name], [qsTr("검색어")] )
    }())
    property var resArr : (function() {
        resArr = []
        CommonUtils.addArr(resEnum,  CommonConstants.Name, resArr)
    }())

    // 17.10.23 hskim
    function showView( args )
    {
        sendEvent(TestRecentSearchPageViewEvent.CreateView, args)
    }

    function closeView(index) {
        console.log( "#1" )

        isCompleted = false
        sendEvent(TestRecentSearchPageViewEvent.BackwardClicked, caller)
        hideKeyboard()

        // 17.11.7 hskim commented the code below not to recreate search tab view
        //sendEvent(TestRecentSearchPageViewEvent.SelectZone1TextInput)
        sendEvent(TestRecentSearchPageViewEvent.ActivatedForNavi)
        var data = ({})
        data[CommonConstants.ObjectName] = CommonConstants.objectName(ViewRegistry.SearchTabView)
        sendEvent(TestRecentSearchPageViewEvent.CloseView, data)
    }

    function __processKeyboard(key, text) {
        switch(key) {
        case CustomKeyboard.CodeCancel:
            closeView(searchRoot.index)
            break
        case CustomKeyboard.CodeSymbol:     break
        case CustomKeyboard.CodeKorean:     break
        case CustomKeyboard.CodeEnglish:    break
        case CustomKeyboard.CodeShift:      break
        case CustomKeyboard.CodeClear:      break
        case CustomKeyboard.CodeSetting:    break
        case CustomKeyboard.CodeSearch:     break
        case CustomKeyboard.CodeConfirm:
            showTextSearchView(text)
            break
        case CustomKeyboard.CodeSpace:
        case CustomKeyboard.CodeDelete:
            break;
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
        visibleKeyboard(false)
    }

    function showTextSearchView(text){
        var data = ({})
        data[CommonConstants.Url] = CommonConstants.url(ViewRegistry.TextSearchView)
        data[CommonConstants.Caller] = CommonConstants.objectName(ViewRegistry.SearchTabView)
        data[SearchConstants.Query] = text
        data[SearchConstants.Request] = true
        showView(data)
    }

    function showSearchResultView(text){
        var data = ({})
        data[CommonConstants.Url] = CommonConstants.url(ViewRegistry.SearchResultView)
        data[CommonConstants.Caller] = CommonConstants.objectName(ViewRegistry.SearchTabView)
        data[CommonConstants.Owner] = CommonConstants.objectName(ViewRegistry.RecentSearchPageView)
        data[SearchConstants.Query] = text
        showView(data)

        data = ({})
        data[CommonConstants.ObjectName] = CommonConstants.objectName(ViewRegistry.SearchTabView)
        sendEvent(TestRecentSearchPageViewEvent.CloseView, data)
    }

    function requestViewData()
    {
        var data = ({})
        data[CommonConstants.Max] = requestMax
        data[CommonConstants.Offset] = currentCnt
        sendEvent( TestRecentSearchPageViewEvent.RequestRecentSearch, data )
    }

    /*!
      \qmlmethod void createModel()
       none push version
      \brief depricated!
      */
    function createModel(){
        listModel.clear()
        var len = (result) ? result.length : 0
        for (var i = 0; i< len; i++) {
            listModel.append( {
                text: result[i]
            })
        }
        message.visible = listModel.count ? false : (request ? true : false)
        request = false
        if (listModel.count > 0)
            listModel.append({text: textArr[textEnum.RecentEdit]})
    }

    function pushRecentSearchList(newList, cnt)
    {
        if (result == null)
            result = newList
        else
            result += newList

        if (editView != null)
            editView.pushRecentSearchList(newList)

        for (var p in newList)
        {
            var dest = newList[p];
            if ( listModel.count > currentCnt )
            {
                listModel.set(
                            currentCnt,
                            {
                                seq      : dest[resEnum.Id],
                                text  : dest[resEnum.Keyword]
                            });
            }
            else
            {
              listModel.append (
                  {
                      seq      : dest[resEnum.Id],
                      text  : dest[resEnum.Keyword]
                  });
            }
            //console.log("-->recent search list [ "+dest[resEnum.Keyword] + " ]");
            currentCnt++;
        }

        if (listModel.count > 0)
            listModel.append({seq:0, text: textArr[textEnum.RecentEdit]})

        message.visible = !listModel.count
        //console.log("push recent search: cnt:"+cnt+",model cnt:"+ listModel.count+"(current:"+currentCnt+")");

    }

    /** the list data is sync both list and edit view. this called from edit view */
    function updateList()
    {
        listModel.clear();
        currentCnt = 0
        result = null
        requestViewData()
    }

    function showKeyboard(keyboard){
        if (!isCompleted) return
        var data = ({})
        // 17.10.26 hskim changed owner from SearchTabView as its own's object name
        data[CommonConstants.Owner] = CommonConstants.objectName(ViewRegistry.RecentSearchPageView)
        data[CommonConstants.X] = keyboard.x
        data[CommonConstants.Y] = keyboard.y
        data[CommonConstants.Z] = keyboard.z + 1
        data[CommonConstants.Width] = keyboard.width
        data[CommonConstants.Height] = keyboard.height
        data[CommonConstants.Text] = ""
        data[CommonConstants.SearchBtnAutoEnable] = true
        sendEvent(TestRecentSearchPageViewEvent.ShowKeyboard, data)
    }

    function visibleKeyboard(visible) {
        if (visible) {
            keyboard.height = keyboardHeight
            showKeyboard(keyboard)
        } else {
            keyboard.height  = 0
            hideKeyboard()
        }
    }

    function hideKeyboard(){
        var data = ({})
        data[CommonConstants.Text] = ""
        sendEvent(TestRecentSearchPageViewEvent.HideKeyboard, data)
    }

    function activatedPage() {
        console.log( "#1" )

        sendEvent(TestRecentSearchPageViewEvent.ResetCsw)
        var args = [ csw ]
        sendEvent(TestRecentSearchPageViewEvent.RegistCsw, args)

        sendEvent(TestRecentSearchPageViewEvent.SelectZone1TextInput)
        visibleKeyboard(true)
    }

    Component.onCompleted: {
        isCompleted = true
    }

    Component.onDestruction: {
    }

    onVisibleChanged: {
        visibleKeyboard(visible)
    }

    onMediated: {
        requestViewData()
    }

    onActivated: {
        sendEvent(TestRecentSearchPageViewEvent.Activated)
        activatedPage()
    }    

    onDeactivated: {
        sendEvent(TestRecentSearchPageViewEvent.Deactivated)
    }

    function __doSoftKeyFunction(button, menuName) {
      if (button == softKeyMenu[softKeyEnum.Back]) {
      } else if (button == softKeyMenu[softKeyEnum.Option]) {
      } else if (button == softKeyMenu[softKeyEnum.Navi]) {
      } else if (button == softKeyMenu[softKeyEnum.Menu]) {
      }
    }

    ListModel { id: listModel   }

    Component {
        id: listDelegate
            Column{
                width: listView.width
                height: listView.rowHeight
                property var itemClicked: function() {
                    listView.currentIndex = model.index
                    if (model.index == listModel.count - 1)
                        showRecentSearchEditView()
                    else{
                        showSearchResultView(model.text)
                    }
                }

                Item {
                    width: parent.width
                    height: parent.height - splitBar.height
                    TMouseArea {
                        width: parent.width
                        height: parent.height
                        onClicked: {
                            itemClicked()
                        }
                    }
                    Text {
                        id: label
                        x: GlobalConstants.kListLineLeftMargin
                        width: parent.width - (x + GlobalConstants.kListLineRightMargin)
                        height: parent.height
                        horizontalAlignment: (model.index == listModel.count - 1) ? Qt.AlignHCenter : Qt.AlignLeft
                        verticalAlignment: Qt.AlignVCenter
                        font { family: GlobalConstants.kFontFamily;/* pixelSize: GlobalConstants.kListTextFontSize * 0.5*/ }
                        font.pixelSize: (model.index != listModel.count - 1 ) ?  GlobalConstants.kListTextFontSize * 1.25 :  GlobalConstants.kListTextFontSize
                        color: GlobalConstants.kListTextColor
                        text: model.text
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

    Component {
          id: highlightBar
          Rectangle {
              x: GlobalConstants.kListBackgroundLeftMargin
              y: listView.currentItem.y;
              width: listView.currentItem.width  - (GlobalConstants.kListBackgroundLeftMargin + GlobalConstants.kListBackgroundRightMargin)
              height: listView.currentItem.height
              color: GlobalConstants.kListPressColor
          }
    }

    Column {
        width: parent.width
        height: parent.height + GlobalConstants.kScreenZone3Height
        Rectangle{
            width: parent.width
            height: parent.height - keyboard.height
            color: (listModel.count) ? GlobalConstants.kListNormalColor : "gray"
            ListView {
                id: listView
                width: parent.width
                height: parent.height
                property int rowHeight: 64
                cacheBuffer: rowHeight * 2
                visible: (listModel.count) ? true : false
                clip: true
                model: listModel
                delegate: listDelegate
                currentIndex: -1
                highlight: highlightBar
                highlightFollowsCurrentItem: false

                // masking
                onMovementStarted:  imgMask.visible = true
                onMovementEnded:  imgMask.visible = false

                Image {
                      id: imgMask
                      width: parent.width
                      height: 30
                      y: parent.y + (parent.height - height)
                      visible: false
                      sourceSize.width: width
                      sourceSize.height: height
                      source: Resource.imageUrl("zone/zone2_img_mask.png")
                }

                TScrollBar {
                    flickable: parent
                    width: 2
                    color: GlobalConstants.kScrollBarFrontColor
                    grooveColor: GlobalConstants.kScrollBarBackColor
                    rightMargin: GlobalConstants.kScrollBarRightMargin
                    onScroll:    imgMask.visible = true
                    onMouseRelease:  imgMask.visible = false
                }
            }

            Rectangle {
                id: message
                width: parent.width
                height: parent.height
                visible: false
                color: GlobalConstants.kBasicBackgoundColor
                Text {
                    anchors.fill: parent
                    horizontalAlignment: Qt.AlignHCenter
                    verticalAlignment: Qt.AlignVCenter
                    font { family: GlobalConstants.kFontFamily; pixelSize: GlobalConstants.kListTextFontSize * 1.25 }
                    color: GlobalConstants.kListTextColor
                    text: textArr[textEnum.NoData]
                }
            }
        }
        Item {
            id: keyboard
            width: parent.width
            height: 0
        }
    }

    Csw {
        id: csw
        section: Csw.Section_List
        sequence: 0
        enable: searchRoot.visible

        function goPrev() {
            if (0 <listView.currentIndex) {
                listView.currentItem.state = "normal"
                --listView.currentIndex
                listView.currentItem.state = "focused"
            }
        }
        function goNext() {
            if (listView.currentIndex < listModel.count -1) {
                listView.currentItem.state = "normal"
                ++listView.currentIndex
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
                listView.currentItem.state = "focused"
            }
        }
        onLostFocusd: {
            searchRoot.focus = false
            if (listView.currentItem !== undefined)
                listView.currentItem.state = "normal"
        }
        onRightPressed: {
            sendEvent( TestRecentSearchPageViewEvent.ChangeSearchTabViewPage,
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

