import QtQuick 2.0
import QtGraphicalEffects 1.0
import ViewRegistry 1.0
import CommonConstants 1.0
import ZoneConstants 1.0
import SearchConstants 1.0
import RecentDestinationPageViewEvent 1.0
import SearchTabViewEvent 1.0
import SearchResultDetailViewEvent 1.0
import SearchResultViewEvent 1.0
import Csw 1.0

import "../../singleton"
import "../../component"
import "../../view"
import "../../view/util"
import "../../view/search"
import "../../GlobalConstants.js" as GlobalConstants
import "../../view/util/CommonUtils.js" as CommonUtils

PopupView {
    id: searchRoot
    width: parent ? parent.width : undefined
    height: {
        if (parent) {
            var h = parent.height //  remove + GlobalConstants.kScreenZone3Height , fix https://tde.sktelecom.com/pms/browse/TMCRSM-2272
            if (isViaSettingMode || isRegFavoriteMode) {
                h += 84 /* tabBar.height */
            }
            return h
        } else {
            return undefined
        }
    }

    property string caller: CommonConstants.objectName(ViewRegistry.SearchTabView)
    property int  index: 0
    property int currentCnt : 0
    property int requestMax : 20
    property var result: null
    property var editView: null
    property string destAddress
    property string owner: CommonConstants.objectName(ViewRegistry.SearchTabView) // for the favorite registination scenario 2017.7.7
    property bool isRegFavoriteMode: false // for the favorite registination scenario 2017.7.7
    property bool isHome: false
    property bool isOffice: false
    property bool isViaSettingMode: false
    property bool isCompleted: false
    property bool focusCsw: false

    readonly property int kMaxRecentDestination: 250
    property color highlightColor: MultiSenseCoordinator.kAmbientColor
    property real highlightOpacity: 1.0


    // language
    property string currentLang: "ko"

    //Timer
    property var timer : null

    //ID
    property var listView: null
    property var message: null
    property Item currentPressedContentItem: null

    property int rowHeight: GlobalConstants.kListTextItemHeight;

    // alias
    readonly property string kLabelTitle: qsTr("최근 목적지") + Tr.d
    readonly property string kLabelNoHistory: qsTr("최근 목적지가 없습니다") + Tr.d
    readonly property string kLabelDetails: qsTr("상세") + Tr.d 
    readonly property string kLabelConfirm: qsTr("확인") + Tr.d

    readonly property string kLabelRecentDestEdit: qsTr("최근 목적지 편집") + Tr.d
    
    readonly property var kSoftkeys: {
        "Back": ZoneConstants.Back, // 뒤로
        "Option": ZoneConstants.Option, // 옵션
        "Navi": ZoneConstants.Navi, // 네비
        "Menu": ZoneConstants.Menu, // 메뉴
        "Confirm": kLabelConfirm, // 확인
        "RecentDestinationEdit": kLabelRecentDestEdit, // 편집하기
    }

    readonly property var kHeader: {
        "Id":0, // 아이디
        "Name":1, // 명칭
        "Address":2, // 주소
        "Type":3, // 타입
        "MapX":4, // 좌표X
        "MapY":5, // 좌표Y
        "Hit":6, // 빈도
        "PoiID": 7, // pOI ID
        "ArrivalTime":8, // 소요시간
        "NavSeq":9,
        "RpFlag":10,
        "OpiPoleCate": 11// 주유소,충전소 브랜드 표시
    }


    function showSearchResultDetailView(subject,address, mapX, mapY, poiId, navSeq, rpFlag, poleCate){
        var parent = searchRoot

        var result = SearchConstants.resultInit()
        result[SearchConstants.ResultPOIID               ] = poiId
        result[SearchConstants.ResultNavseq              ] = navSeq
        result[SearchConstants.ResultRpFlag              ] = rpFlag
        result[SearchConstants.ResultName                ] = subject
        result[SearchConstants.ResultStdName             ] = subject
        result[SearchConstants.ResultAddress             ] = address
        result[SearchConstants.ResultX                   ] = mapX
        result[SearchConstants.ResultY                   ] = mapY
        result[SearchConstants.ResultOPIPoleCate         ] = poleCate
        result[SearchConstants.ResultNavseq              ] = navSeq

        var data = ({})
        if (isViaSettingMode) {
            data[CommonConstants.ObjectName ] = CommonConstants.objectName(ViewRegistry.RecentDestinationPageView)
            sendEvent(RecentDestinationPageViewEvent.HideView)
        } else {
            data[CommonConstants.ObjectName ] = CommonConstants.objectName(ViewRegistry.SearchTabView)
            sendEvent(RecentDestinationPageViewEvent.CloseView, data)
        }

        data = ({})
        data[CommonConstants.Url] = CommonConstants.url(ViewRegistry.SearchResultDetailView)
        data[CommonConstants.Caller] = caller
        if (isRegFavoriteMode || isViaSettingMode)
            data[CommonConstants.Owner  ] = CommonConstants.objectName(ViewRegistry.RecentDestinationView)
        else
            data[CommonConstants.Owner  ] = CommonConstants.objectName(ViewRegistry.RecentDestinationPageView)
        data[SearchConstants.Result     ] = result
        data[SearchConstants.Favorite   ] = isRegFavoriteMode
        data[SearchConstants.SearchType ] = CommonConstants.Name
        data[CommonConstants.Visible    ] = SearchConstants.AlwaysShow
        data[SearchConstants.IsHome     ] = isHome
        data[SearchConstants.IsOffice   ] = isOffice

        //data[CommonConstants.Z          ] = CommonConstants.MAX_INT_VALUE
        data[CommonConstants.ReFocus    ] = true
        data[CommonConstants.ObjectName ] = CommonConstants.objectName(ViewRegistry.SearchResultDetailView)
        sendEvent(RecentDestinationPageViewEvent.UpdateView, data)

        sendEvent(RecentDestinationPageViewEvent.CreateView, data )
        visible = false;
    }

    function showRecentDestinationEditView(){
        var data = ({})
        data[CommonConstants.Url] = CommonConstants.url(ViewRegistry.RecentDestinationEditView)
        data[CommonConstants.Caller] = caller
        data[SearchConstants.SearchPageObj] = searchRoot
        data[SearchConstants.Result] = result
        data[SearchConstants.Request] = true
        sendEvent(RecentDestinationPageViewEvent.CreateView, data)
    }

    function resetRecentDestList() {
        result = null
        listModel.clear()
        listView.model = null
        currentCnt = 0
        if (editView !== null && editView !== undefined) {
            editView.resetRecentDestList()

        }
    }

    function activatedPage() {

        // 17.11.8 hskim
        if (isViaSettingMode || isRegFavoriteMode) {
            sendEvent(RecentDestinationPageViewEvent.SelectZone1Title, kLabelTitle)
        } else {
            sendEvent(RecentDestinationPageViewEvent.SelectZone1TextInput)
        }

        hideKeyboard()

        // donot show the EDIT softkey in favorite regist mode
        if (isRegFavoriteMode || isViaSettingMode )
        {   
            // sendEvent(RecentDestinationPageViewEvent.SelectSoftkeyIndexMenuMode, {});
            if ( listModel.count > 0 ) //  fixed 'Edit Menu' appears on Popup, TMCRSM-2748 , TMCRSM-1619 from UID v1.66 p121
               sendEvent(RecentDestinationPageViewEvent.SelectSoftkeyIndexMenuMode, {"optionMenu":optionMenu.menus[optionMenu.orderType]})
            else
               sendEvent(RecentDestinationPageViewEvent.SelectSoftkeyIndexMenuMode, {"menus":['noOption']})
        }  // fixed,TMCRSM-2348
        else {  // fix there are no items then dimm softkey [TMCRSM-534] 18.3.16
            if ( listModel.count > 0 )
               sendEvent(RecentDestinationPageViewEvent.SelectSoftkeyIndexMenuMode, {"menus":[kLabelRecentDestEdit],"optionMenu":optionMenu.menus[optionMenu.orderType]})
            else
               sendEvent(RecentDestinationPageViewEvent.SelectSoftkeyIndexMenuMode, {"menus":[kLabelRecentDestEdit+":disabled", 'noOption']})
        }

        sendEvent(RecentDestinationPageViewEvent.ResetCsw)
        var args = [ csw ]
        sendEvent(RecentDestinationPageViewEvent.RegistCsw, args)
    }

    function pushRecentDestList(newList, cnt)
    {
        if (currentCnt == 0)
            resetRecentDestList()

        if (result == null) result = {}

        for (var p in newList)
        {
            var dest = newList[p];
            result[currentCnt] = dest
            listModel.append (
                        {
                            seq      : dest[kHeader.Id],
                            name     : dest[kHeader.Name],
                            address  : dest[kHeader.Address],
                            destType : dest[kHeader.Type],
                            mapX     : dest[kHeader.MapX],
                            mapY     : dest[kHeader.MapY],
                            poiID    : parseInt(dest[kHeader.PoiID]),
                            arrivalTime: parseInt(dest[kHeader.ArrivalTime]),
                            navSeq   : dest[kHeader.NavSeq],
                            rpFlag   : dest[kHeader.RpFlag],
                            poleCate : dest[kHeader.OpiPoleCate]

                        });
            currentCnt++;
        }

        if (editView !== null && editView !== undefined) {
            editView.pushRecentDestList(newList);
        }

        listView.model = listModel
        listView.currentIndex = -1
        listView.visible = listModel.count > 0;
        if (message != null)
            message.visible = (listModel.count == 0)
    }

    function requestViewData(sortType)
    {
        if (sortType !== 0 && sortType !== 1 && sortType !== 2)
            sortType = 0
        var data = ({})
        data[CommonConstants.Max] = requestMax
        data[CommonConstants.Offset] = currentCnt
        data[CommonConstants.Type] = sortType // sort type 0: regist desc 1: hit desc 2: hybrid
        data[CommonConstants.Owner] =  owner // identify owner view such the bookmark regist view in order to prevent duplicated noti response 2017.9.13
        sendEvent( RecentDestinationPageViewEvent.RequestRecentDestination, data )
    }

    /** the list data is sync both list and edit view. this called from edit view */
    function updateListFromEditView()
    {
        listModel.clear();
        listView.model = null
        currentCnt = 0;
        result = null
        requestViewData()
        listView.model = listModel
        listView.currentIndex = -1
    }

    function retranslateUi() {
         if (isViaSettingMode)
            sendEvent(RecentDestinationPageViewEvent.SelectZone1Title, kLabelTitle)

        if (!isRegFavoriteMode) {  // fix there are no items then dimm softkey [TMCRSM-534] 18.3.16
            if ( listModel.count > 0 )
                sendEvent(RecentDestinationPageViewEvent.SelectSoftkeyIndexMenuMode, {"menus":[kLabelRecentDestEdit],"optionMenu":optionMenu.menus[optionMenu.orderType]})
            else
                sendEvent(RecentDestinationPageViewEvent.SelectSoftkeyIndexMenuMode, {"menus":[kLabelRecentDestEdit+":disabled"],"optionMenu":""})
        }
    }

    Component.onCompleted: {
        isCompleted = true
    }

    onMediated: {
        loader.sourceComponent = main
        sendEvent(RecentDestinationPageViewEvent.Mediated)
    }

    onActivated: {
        if (currentPressedContentItem !== null) {
            currentPressedContentItem.hasReleased = false;
        }

        sendEvent(RecentDestinationPageViewEvent.Activated)
        activatedPage()
        if (focusCsw) {
            sendEvent(RecentDestinationPageViewEvent.FocusCsw, csw.objectName)
            focusCsw = false
        }
    }

    onDeactivated: {
        optionMenu.visible = false // 2017.12.8 fixed TMCRSM-140 "통합검색(최근목적지 Tap)에서 하단영역 옵션버튼 선택시, 최신순/빈도순 옵셥값 노출될때, 최근검색or즐겨찾기 Tap한상태에서 다시 최근목적지 돌아왔을때 옵션 값 사라지지 않고 노출되는 현상."
        sendEvent(RecentDestinationPageViewEvent.Deactivated)
        sendEvent(RecentDestinationPageViewEvent.ShowZone3)
    }

    Component.onDestruction: {
        timer = CommonUtils.destroy(timer)
    }

    // Favorite Registination 2017.7.7
    onOwnerChanged: {
        isRegFavoriteMode = (owner.indexOf(CommonConstants.objectName(ViewRegistry.BookmarkPageView)) >= 0)
        isHome = (0 <= owner.indexOf('/home'))
        isOffice = (0 <= owner.indexOf('/office'))
    }
    onCallerChanged: {
        isViaSettingMode = (caller === CommonConstants.objectName(ViewRegistry.RouteSettingView))
    }

    function hideKeyboard(){
        var data = ({})
        data[CommonConstants.Text] = CommonConstants.Empty
        sendEvent(RecentDestinationPageViewEvent.HideKeyboard, data)
    }

    function closeView() {
        isCompleted = false
        sendEvent(RecentDestinationPageViewEvent.BackwardClicked, caller)
        hideKeyboard()

        // 17.11.7 hskim commented the code below not to recreate search tab view
        //sendEvent(RecentDestinationPageViewEvent.SelectZone1TextInput)
        sendEvent(RecentDestinationPageViewEvent.ActivatedForNavi)
        var data = ({})
        data[CommonConstants.ObjectName] = CommonConstants.objectName(ViewRegistry.SearchTabView)
        sendEvent(RecentDestinationPageViewEvent.CloseView, data)
    }

    function __doSoftKeyFunction(button, menuName, menuIndex) {
      if (button == kSoftkeys.Back) {
          closeView()

          // TMCRSM-481. 네비메뉴로부터 최근목적지로 왔다면 뒤로가기시 네비메뉴로 복귀
          // 18.03.04 ymhong
          if (caller == CommonConstants.objectName(ViewRegistry.NaviMenuView)) {
              var url = CommonConstants.url(ViewRegistry.NaviMenuView)
              sendEvent(RecentDestinationPageViewEvent.CreateView, {'url':url})
          }
      } else if (button == kSoftkeys.Navi) {
      } else if (button == kSoftkeys.Menu) {
          switch (menuName) {
            case kLabelRecentDestEdit:
                showRecentDestinationEditView();
                break;
            case optionMenu.menus[0]://  최신순 (default)
            case optionMenu.menus[1]: // 빈도순
                 optionMenu.toggleMenu()
                 break;
            default:
                optionMenu.visible = false
              break
          }
      }
    }

    //main
    Loader {
         id: loader
         width: parent.width
         height: parent.height
         onLoaded: {
             requestViewData()
         }
    }

    Component {
        id: main
            Rectangle{
                width: parent.width
                height: parent.height
                color: GlobalConstants.kListNormalColor
                ListView {
                    id: listView
                    width: parent.width
                    height: parent.height// - GlobalConstants.kScreenZone3Height
                    Component.onCompleted: searchRoot.listView = this
                    cacheBuffer: rowHeight * 2
                    visible: (listModel.count) ? true : false
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
                            color: {
                                if (currentItem !== null) {
                                    if (currentItem.state != "focused")
                                        isFocused = false

                                    if (currentItem.state == "normal" )
                                        return "transparent"
                                    else if (currentItem.state == "pressed")
                                        return MultiSenseCoordinator.kAmbientColor
                                    else if (currentItem.state == "focused")
                                        return GlobalConstants.kListFocusColor
                                    else
                                        return "transparent"
                                }
                                else
                                    return "transparent"
                            }

                            Component.onCompleted: listView.highlightBar = this
                        }
                    }
                    highlightFollowsCurrentItem: false

                    onMovementStarted: {
                        sendEvent(RecentDestinationPageViewEvent.HideZone3)
                        sendEvent(RecentDestinationPageViewEvent.ListScrolled)
                    }
                    onMovementEnded:  {
                        sendEvent(RecentDestinationPageViewEvent.ShowZone3)
                        sendEvent(RecentDestinationPageViewEvent.ListScrolled)
                    }

                    Image {
                          id: imgMask
                          width: parent.width
                          height: 30
                          anchors.bottom: parent.bottom
                          anchors.bottomMargin: GlobalConstants.kScreenZone3Height
//                          y: parent.y + (parent.height - height)
                          visible: scrollBar.heightRatio < 1.0 && !listView.dragging
                          sourceSize.width: width
                          sourceSize.height: height
                          source: Resource.imageUrl("zone/zone2_img_mask.png")
                    }

                    TScrollBar {
                        id: scrollBar
                        width: 2
                        flickable: parent
                        color: MultiSenseCoordinator.kAmbientColor
                        grooveColor: GlobalConstants.kScrollBarBackColor
                        rightMargin: GlobalConstants.kScrollBarRightMargin
                    }

                    onAtYEndChanged: {
                        // fix TMCRSM-3522, don't check flicking state
                        if (contentHeight > 0
                                && atYEnd
                                && contentY > 0
                                && contentY+height >= contentHeight) {
                            // next list up
                            // cancelFlick()
                            requestViewData(!optionMenu.orderType)
                        }
                    }
                    footer: Component {
                        Item { // last item get shadow under zone3, fixed TMCRSM-1407
                            width: listView.width; height: GlobalConstants.kListViewFooterHeight
                            }
                    }
                }

                Rectangle {
                    id: message
                    Component.onCompleted: searchRoot.message = this
                    width: parent.width
                    height: parent.height - GlobalConstants.kScreenZone3Height
                    anchors.top: parent.top
                    anchors.topMargin: 0
                    visible: false
                    color: GlobalConstants.kBasicBackgoundColor
                    Text {
                        width: parent.width
                        height: GlobalConstants.kGeneralMessageFontSize+2
                        anchors.verticalCenter: parent.verticalCenter
                        horizontalAlignment: Qt.AlignHCenter
                        verticalAlignment: Qt.AlignVCenter
                        font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(GlobalConstants.kGeneralMessageFontSize) }
                        color: GlobalConstants.kListTextColor
                        text: kLabelNoHistory
                    }
                }
            }
    }

    ListModel { id: listModel   }

    Component {
        id: listDelegate
        Column{
            id: listItem
            width: parent.width
            height: rowHeight
            spacing: 0
            property alias detailButton: detailBtn
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
                    PropertyChanges {
                        target: detailBtn
                        state: "normal"
                        hasFocus: false
                    }
                },
                // this state should release csw focused
                State {
                    name: 'pressed'
                    PropertyChanges {
                        target: listView
                        currentIndex: index
                    }
                    PropertyChanges {
                        target: label
                        width: label.parent.width
                        elide: Text.ElideRight
                        color: "white"
                    }
                    PropertyChanges {
                        target: detailBtn
                        box.colorN: "transparent"
                        hasFocus: false
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
                    PropertyChanges {
                        target: detailBtn
                        state: "normal"  // 2019.5.23 fixed TMCRSM-3201
                        hasFocus: false
                    }
                },
                State {
                    name: 'focusedButton'
                    PropertyChanges {
                        target: detailBtn
                        overlayColor: "#ffffff"
                        txt.color: "#000000"
                        box.colorN: GlobalConstants.kListFocusColor
                        box.colorP: GlobalConstants.kListFocusColor
                        box.border.color: GlobalConstants.kListFocusColor
                        hasFocus: true
                    }
                    PropertyChanges {
                        target: label
                        width: label.parent.width
                        elide: Text.ElideRight
                        color: GlobalConstants.kListTextColor
                    }
                }

            ]

            property var itemClicked: function () {
                csw.forceLostFocus()
                listView.currentIndex = model.index
                highlightColor = Qt.binding(function(){ return MultiSenseCoordinator.kAmbientColor})
                var sleep =  GlobalConstants.kPressedColorExposureTime
                timer = CommonUtils.destroy(timer)
                timer = CommonUtils.delay(searchRoot, sleep, function() {
                    try {
                        //highlightColor = CommonConstants.Transparent

                       // 선택된 아이템을 다시 클릭하면 경로 안내 시작.
                       var data = ({})
                       data[CommonConstants.X] = model.mapX
                       data[CommonConstants.Y] = model.mapY
                       data[CommonConstants.Name] = model.name
                       data[CommonConstants.Address] = model.address
                       data[SearchConstants.PoiID] = model.poiID
                       data[SearchConstants.NavSeq] = model.navSeq
                       data[SearchConstants.RpFlag] = model.rpFlag
                       data[CommonConstants.Owner] = owner
                       data[CommonConstants.Caller] = caller

                       if (isRegFavoriteMode) {
                           data[SearchConstants.Favorite] = true
                           data[SearchConstants.Dest] = model.name
                           sendEvent(RecentDestinationPageViewEvent.ItemClicked, data)
                       } else if (isViaSettingMode) {
                           data[SearchConstants.ViaSetting ] = true
                           data[SearchConstants.Dest] = model.name
                           sendEvent(RecentDestinationPageViewEvent.ItemClicked, data)
                       } else {
                           sendEvent(RecentDestinationPageViewEvent.QueryInDriveMode, data)
                       }
                    } catch(e) {
                    }
                })
            }

            property var goToDetail: function () {
                var navSeq = model.navSeq
                var rpFlag = model.rpFlag
                var poleCate = model.poleCate
                showSearchResultDetailView(name,address,mapX,mapY,poiID,navSeq,rpFlag, poleCate)
            }

            Item {
                id: mouseAreaItem
                width: parent.width
                height: parent.height - splitBar.height
                property alias mouseArea: mouseArea
                TMouseArea {
                    id: mouseArea
                    anchors.fill: parent
                    property bool hasReleased: false
                    PressBarMouseSync2 {
                        targetItem: listItem
                    }
                    onPressed: {
                        hasReleased = false;
                        sendEvent(RecentDestinationPageViewEvent.ItemPressed)
                    }
                    onReleased: { hasReleased = true; }
                    onClicked: {
//                        detailBtn.mouseArea.enabled = false // that could cause a problem while current content item lives
                        itemClicked()
                        searchRoot.currentPressedContentItem = this // fixed TMCRSM-3131
                    }
               }
                Row{
                    x: GlobalConstants.kListTextLeftMargin;
                    width: parent.width - (GlobalConstants.kListTextLeftMargin + GlobalConstants.kListTextRightMargin)
                    height: parent.height
                    Item {
                        id: marqueeItem
                        width: parent.width  - (detailBtn.width + marqueeRightSpace.width)
                        height: parent.height
                        clip:true
                        Text {
                            id: label
                            width: parent.width
                            anchors.verticalCenter: parent.verticalCenter
                            font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(GlobalConstants.kOneLineListTextFontSize) }
                            color: GlobalConstants.kListTextColor
                            elide: Text.ElideRight
                            text: (model.name != "") ? model.name : model.address
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

                    Item { id: marqueeRightSpace; width: 30; height:1 } // marquee takes margin 30 from detail button fixed, TMCRSM-2348

                    TPushButton {
                        id: detailBtn
                        width: 88; height: 58
                        anchors.verticalCenter: parent.verticalCenter
                        mutisense: true

//                        overlayColor: (listView.currentIndex == index && ) ?  CommonConstants.Transparent : MultiSenseCoordinator.kAmbientColor
                        // visible에서 !isRegFavoriteMode를 주석 처리하면 최근 목적지 세에 이동 및
                        // 이전 뷰 이동등의 처리가 가능.
                        // 일단 사양에 없기 때문에 이전 처럼 처리.
                        // 17.12.04 ymhong
                        // visible: !isRegFavoriteMode// && !isViaSettingMode // 2018.3.28 fix TMCRSM-577, detail button must always appears
                        Component.onCompleted: {
                            box.border.width    = 2
                            box.radius          = height/2
                            box.colorN          = GlobalConstants.kBasicBackgoundColor
                            box.colorP          =  MultiSenseCoordinator.kAmbientColor
                            box.border.color    = MultiSenseCoordinator.kAmbientColor
                        }
                        Text2 {
                            anchors.fill: parent
                            font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(30)}
                            text: kLabelDetails
                            verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter
                            offsetY: 3
                            color: detailBtn.txt.color
                            scale: detailBtn.txt.scale
                        }

                        onButtonPressed: { csw.forceLostFocus() }
                        onButtonClicked: {
                            if (!mouseArea.pressed && !mouseArea.hasReleased)  // fixed TMCRSM-3131
                                goToDetail()
                        }
                    }
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

    RecentDestinationPageViewOption {
        id: optionMenu
        anchors { bottom: loader.bottom; bottomMargin: GlobalConstants.kScreenZone3Height; right: parent.right }
        onByHitClicked: {
           resetRecentDestList()
            requestViewData(1)
        }
        onByOrderClicked: {
           resetRecentDestList()
            requestViewData(0)
        }
    }

    Csw {
        id: csw
        objectName: 'RecentDestinationCsw'
        section: Csw.Section_List
        sequence: 0
        enable: 0 < listModel.count
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
                listView.positionViewAtIndex(listView.currentIndex+1, ListView.Contain)
                listView.highlightBar.isFocused = true
                listView.currentItem.state = "focused"
            }
        }
        onFocused: {
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
            if (listView.highlightBar !== null)
              listView.highlightBar.isFocused = false
            listView.currentItem.state = "normal"
            listView.currentIndex = -1
        }
        onLeftPressed: {
            if (listView.currentItem !== undefined) {
                if (listView.currentItem.detailButton.hasFocus === true) {
                    listView.currentItem.state = "focused"
                    listView.highlightBar.isFocused = true
                    listView.currentItem.detailButton.hasFocus = false
                }
                else {
                    listView.currentItem.state = "normal"
                    listView.highlightBar.isFocused = false
                    sendEvent( RecentDestinationPageViewEvent.ChangeSearchTabViewPage,
                              {isCswControl:true, page:0})
                }
            }
        }

        onRightPressed: {
            if (listView.currentItem !== undefined) {
                if (listView.highlightBar.isFocused === true &&
                        listView.currentItem.detailButton.hasFocus === false ) {
                    listView.currentItem.state = "focusedButton"
                    listView.highlightBar.isFocused = false
                    listView.currentItem.detailButton.hasFocus = true
                }
                else {
                    listView.currentItem.state = "normal"
                    listView.highlightBar.isFocused = false
                    listView.currentItem.detailButton.hasFocus = false
                    sendEvent( RecentDestinationPageViewEvent.ChangeSearchTabViewPage,
                              {isCswControl:true, page:2})
                }
            }
        }

        onSpinCcwPressed: {
            goPrev()
        }
        onSpinCwPressed: {
            goNext()
        }
        onSelected: {
            if (listView.currentItem !== undefined) {
                if (listView.currentItem.detailButton.hasFocus === true)
                    listView.currentItem.goToDetail()
                else
                    listView.currentItem.itemClicked()
            }
        }
    }
}
