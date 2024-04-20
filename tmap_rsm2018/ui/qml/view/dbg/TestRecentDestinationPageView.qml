import QtQuick 2.0
import ViewRegistry 1.0
import CommonConstants 1.0
import ZoneConstants 1.0
import SearchConstants 1.0
import RecentDestinationPageViewEvent 1.0
import SearchTabViewEvent 1.0
import SearchResultDetailViewEvent 1.0
import SearchResultViewEvent 1.0
import Csw 1.0

import "../../component"
import "../../view"
import "../../view/search"
import "../../GlobalConstants.js" as GlobalConstants
import "../../view/util/CommonUtils.js" as CommonUtils

import TestRecentDestinationPageViewEvent 1.0

PopupView {
    id: searchRoot
    width: parent ? parent.width : undefined
    height: parent ? parent.height : undefined
    objectName: 'RecentDestinationPageView'

    property string caller: CommonConstants.objectName(ViewRegistry.SearchTabView)
    property int  index: 0
    property int currentCnt : 0
    property int requestMax : 20
    property var result: null
    property var editView: null
    property string destAddress
    property string owner // for the favorite registination scenario 2017.7.7
    property bool isRegFavoriteMode: false // for the favorite registination scenario 2017.7.7
    property bool isViaSettingMode: false
    property bool isCompleted: false

    readonly property int kMaxRecentDestination: 250

    property var textEnum: (function(){
        textEnum = CommonUtils.createEnum(textEnum)
        CommonUtils.appendEnum(textEnum, "Detail",     [CommonConstants.Name], [qsTr("상세")])
        CommonUtils.appendEnum(textEnum, "NoData", [CommonConstants.Name], [qsTr("최근 목적지가 없습니다")])
    }())
    property var textArr: (function() {
        textArr = []
        CommonUtils.addArr(textEnum, CommonConstants.Name, textArr)
    }())

    property var softKeyEnum : (function() {
        softKeyEnum = CommonUtils.createEnum(softKeyEnum)
        CommonUtils.appendEnum(softKeyEnum,"Back"                 ,[CommonConstants.Name], [ZoneConstants.Back] )
        CommonUtils.appendEnum(softKeyEnum,"Option"               ,[CommonConstants.Name], [ZoneConstants.Option])
        CommonUtils.appendEnum(softKeyEnum,"Navi"                 ,[CommonConstants.Name], [ZoneConstants.Navi])
        CommonUtils.appendEnum(softKeyEnum,"Menu"                 ,[CommonConstants.Name], [ZoneConstants.Menu])
        CommonUtils.appendEnum(softKeyEnum,"Confirm"              ,[CommonConstants.Name], [ZoneConstants.Confirm])
        CommonUtils.appendEnum(softKeyEnum,"RecentDestinationEdit",[CommonConstants.Name], [ZoneConstants.RecentDestinationEdit] )
    }())
    property var softKeyMenu: (function() {
        softKeyMenu = []
        CommonUtils.addArr(softKeyEnum, CommonConstants.Name, softKeyMenu)
    }())

    property var resEnum: (function() {
        resEnum = CommonUtils.createEnum(resEnum)
        CommonUtils.appendEnum(resEnum,"Id"                 ,[CommonConstants.Name], [qsTr("아이디")] )
        CommonUtils.appendEnum(resEnum,"Name"               ,[CommonConstants.Name], [qsTr("명칭")] )
        CommonUtils.appendEnum(resEnum,"Address"            ,[CommonConstants.Name], [qsTr("주소")] )
        CommonUtils.appendEnum(resEnum,"Type"               ,[CommonConstants.Name], [qsTr("타입")] )
        CommonUtils.appendEnum(resEnum,"MapX"               ,[CommonConstants.Name], [qsTr("좌표X")] )
        CommonUtils.appendEnum(resEnum,"MapY"               ,[CommonConstants.Name], [qsTr("좌표Y")] )
        CommonUtils.appendEnum(resEnum,"Hit"                ,[CommonConstants.Name], [qsTr("빈도")] )
        CommonUtils.appendEnum(resEnum,"ArrivalTime"        ,[CommonConstants.Name], [qsTr("소요시간")] )
    }())
    property var resArr : (function() {
        resArr = []
        CommonUtils.addArr(resEnum,  CommonConstants.Name, resArr)
    }())

    function showRecentSearchDetailView( args )
    {
        sendEvent(TestRecentDestinationPageViewEvent.CreateView, args )


        visible = false;
    }

    function showSearchResultDetailView(subject,address, mapX, mapY){
        var parent = searchRoot

        var result = []
        result[SearchResultViewEvent.ResultObject]              = null
        result[SearchResultViewEvent.ResultOPIPoleCate]         = -1
        result[SearchResultViewEvent.ResultX]                   = mapX
        result[SearchResultViewEvent.ResultY]                   = mapY
        result[SearchResultViewEvent.ResultStdName]             = subject
        result[SearchResultViewEvent.ResultAddress]             = address
        result[SearchResultViewEvent.ResultDistance]            = 0
        result[SearchResultViewEvent.ResultOPIPrice]            = -1
        result[SearchResultViewEvent.ResultTelNum]              = ""
        result[SearchResultViewEvent.ResultWebSite]             = SearchConstants.NoneData
        result[SearchResultViewEvent.ResultHolliday]            = SearchConstants.NoneData
        result[SearchResultViewEvent.ResultWorkTime]            = SearchConstants.NoneData
        result[SearchResultViewEvent.ResultIndustry]            = SearchConstants.NoneData
        result[SearchResultViewEvent.ResultParking]             = SearchConstants.NoneData
        result[SearchResultViewEvent.ResultIntroduction]        = SearchConstants.NoneData
        var data = ({})
        data[CommonConstants.Url] = CommonConstants.url(ViewRegistry.SearchResultDetailView)
        data[CommonConstants.Caller] = caller
        data[CommonConstants.Owner] = CommonConstants.objectName(ViewRegistry.RecentDestinationPageView)
        data[SearchConstants.Result] = result
        console.log(" search destination------> detailview : "+mapX+", "+mapY);
        showRecentSearchDetailView(data)

        //17.11.1 hskim dont need to close
       //close()
        visible = false;
    }

    function showView( args )
    {
        sendEvent(TestRecentDestinationPageViewEvent.CreateView, args)
    }

    function showRecentDestinationEditView(){
        var data = ({})
        data[CommonConstants.Url] = CommonConstants.url(ViewRegistry.RecentDestinationEditView)
        data[CommonConstants.Caller] = caller
        data[SearchConstants.SearchPageObj] = searchRoot
        data[SearchConstants.Result] = result
        data[SearchConstants.Request] = true
        showView(data)
    }

    function resetRecentDestList() {
        result = null
        listModel.clear()
        listView.model = null
        currentCnt = 0
    }

    function activatedPage() {
        console.log( "#1" )

        // 17.11.8 hskim
        sendEvent(TestRecentDestinationPageViewEvent.SelectZone1TextInput)
        hideKeyboard()

        // donot show the EDIT softkey in favorite regist mode
        if (!isRegFavoriteMode) {
            sendEvent(TestRecentDestinationPageViewEvent.SelectSoftkey, [softKeyMenu[softKeyEnum.RecentDestinationEdit]])
        }
        else {
            sendEvent(TestRecentDestinationPageViewEvent.SelectSoftkey, [])
        }

        sendEvent(TestRecentDestinationPageViewEvent.ResetCsw)
        var args = [ csw ]
        sendEvent(TestRecentDestinationPageViewEvent.RegistCsw, args)
    }

    function pushRecentDestList(newList, cnt)
    {
        if (result == null)
            result = newList
        else
            result += newList // no gurantee

        if (editView != null)
            editView.pushRecentDestList(newList)

        for (var p in newList)
        {
            var dest = newList[p];
            listModel.append (
                        {
                            seq      : dest[resEnum.Id],
                            name     : dest[resEnum.Name],
                            address  : dest[resEnum.Address],
                            destType : dest[resEnum.Type],
                            mapX     : dest[resEnum.MapX],
                            mapY     : dest[resEnum.MapY],
                            arrivalTime: parseInt(dest[resEnum.ArrivalTime]),
                        });
            currentCnt++;
            //console.log("-->recent dest list [ "+dest[resEnum.Address] + " ]");
        }

        listView.model = listModel
        message.visible = ! listModel.count

        //console.log("push recent dest: cnt:"+cnt+",model cnt:"+ listModel.count+"(current:"+currentCnt+")");

    }

    function requestViewData()
    {
        var data = ({})
        data[CommonConstants.Max] = requestMax
        data[CommonConstants.Offset] = currentCnt
        data[CommonConstants.Type] = 0 // sort type 0: regist desc 1: hit desc 3: hybrid
        data[CommonConstants.Owner] =  CommonConstants.objectName(ViewRegistry.SearchTabView) // identify owner view such the bookmark regist view in order to prevent duplicated noti response 2017.9.13
        sendEvent( TestRecentDestinationPageViewEvent.RequestRecentDestination, data )
    }

    /** the list data is sync both list and edit view. this called from edit view */
    function updateList()
    {
        listModel.clear();
        currentCnt = 0;
        result = null
        requestViewData()
    }

    // 17.10.23 hskim not used
    // 경로 요약정보 요청
//    function requestRouteSimpleInfo(reqIdx,mapX,mapY)
//    {
//        reqPendingIndex = reqIdx;
//        reqPendingMapX = mapX;
//        reqPendingMapY = mapY;
//        var data = ({})
//        data[SearchConstants.Id] = reqIdx
//        data[CommonConstants.X] = mapX
//        data[CommonConstants.Y] = mapY
//        sendEvent( RecentTileGridViewEvent.RequestRouteSimpleInfo, data );
//    }

    // 17.10.23 hskim not used
    // 목적지 선택
//    function requestRoute(seq, mapX,mapY)
//    {
//        var data = ({})
//        data[CommonConstants.X] = mapX
//        data[CommonConstants.Y] = mapY
//        sendEvent( RecentTileGridViewEvent.RequestRoute, data);

//        root.close();
//    }

    Component.onCompleted: {
        isCompleted = true
    }

    onMediated: {
        requestViewData()
    }

    onActivated: {
        sendEvent(TestRecentDestinationPageViewEvent.Activated)

        activatedPage()

        if (isRegFavoriteMode || isViaSettingMode) {
            var data = {}
            // 17.10.24 hskim
            sendEvent(TestRecentDestinationPageViewEvent.SelectSoftkey, data)
        }
    }

    onDeactivated: {
        sendEvent(TestRecentDestinationPageViewEvent.Deactivated)
    }

    // Favorite Registination 2017.7.7
    onOwnerChanged: {
        isRegFavoriteMode = (owner.indexOf(CommonConstants.objectName(ViewRegistry.BookmarkPageView)) >= 0)
    }
    onCallerChanged: {
        isViaSettingMode = (caller === CommonConstants.objectName(ViewRegistry.RouteSettingView))
    }

    function hideKeyboard(){
        var data = ({})
        data[CommonConstants.Text] = ""
        sendEvent(TestRecentDestinationPageViewEvent.HideKeyboard, data)
    }

    function closeView() {
        console.log( "#1" )
        isCompleted = false
        sendEvent(TestRecentDestinationPageViewEvent.BackwardClicked, caller)
        hideKeyboard()

        // 17.11.7 hskim commented the code below not to recreate search tab view
        //sendEvent(TestRecentDestinationPageViewEvent.SelectZone1TextInput)
        sendEvent(TestRecentDestinationPageViewEvent.ActivatedForNavi)
        var data = ({})
        data[CommonConstants.ObjectName] = CommonConstants.objectName(ViewRegistry.SearchTabView)
        sendEvent(TestRecentDestinationPageViewEvent.CloseView, data)
    }

    function __doSoftKeyFunction(button, menuName) {
      if (button == softKeyMenu[softKeyEnum.Back]) {
          closeView()
      } else if (button == softKeyMenu[softKeyEnum.Option]) {
          optionMenu.toggleVisible()
      } else if (button == softKeyMenu[softKeyEnum.Navi]) {          
      } else if (button == softKeyMenu[softKeyEnum.Menu]) {
          switch (menuName) {
            case softKeyMenu[softKeyEnum.RecentDestinationEdit]:
                showRecentDestinationEditView();
                optionMenu.visible = false
              break
          }
      }
    }


    ListModel { id: listModel   }

    Component {
        id: listDelegate
        Column{
            width: listView.width
            height: listView.rowHeight
            property var itemClicked: function () {
                listView.currentIndex = model.index
                 // 선택된 아이템을 다시 클릭하면 경로 안내 시작.
                    var data = ({})
                    data[CommonConstants.X] = model.mapX
                    data[CommonConstants.Y] = model.mapY
                    data[CommonConstants.Name] = model.name
                    data[CommonConstants.Address] = model.address
                    data[CommonConstants.Owner] = owner


                    if (isRegFavoriteMode) {
                        data[SearchConstants.Favorite] = true
                        data[SearchConstants.Dest] = model.name
                        sendEvent(TestRecentDestinationPageViewEvent.ItemClicked, data)
                    } else if (isViaSettingMode) {
                        data[SearchConstants.ViaSetting ] = true
                        data[SearchConstants.Dest] = model.name
                        sendEvent(TestRecentDestinationPageViewEvent.ItemClicked, data)
                    } else {
                        sendEvent(TestRecentDestinationPageViewEvent.QueryInDriveMode, data)
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
                Row{
                    x: GlobalConstants.kListLineLeftMargin
                    width: parent.width - (x + GlobalConstants.kListLineRightMargin)
                    height: parent.height
                    Item {
                        width: (parent.width)  - (btn.width + GlobalConstants.kListLineRightMargin)
                        height: parent.height
                        Text {
                            id: label
                            width: parent.width
                            height: parent.height
                            verticalAlignment: Qt.AlignVCenter
                            font { family: GlobalConstants.kFontFamily; pixelSize: GlobalConstants.kListTextFontSize * 1.25 }
                            color: GlobalConstants.kListTextColor
                            text: (model.name != "") ? model.name : model.address
                        }
                    }
                    TPushButton {
                        id: btn
                        label: textArr[textEnum.Detail]
                        width: 80
                        height: 30
                        anchors.verticalCenter: parent.verticalCenter
                        visible: !isRegFavoriteMode && !isViaSettingMode
                        Component.onCompleted: {
                            box.colorN = "transparent"
                            box.colorP = "transparent"
                            img.sourceN = Resource.imageUrl("btn_bg_n.png")
                            img.sourceP = Resource.imageUrl("btn_bg_lp.png")
                            img.sourceSize.width = width
                            img.sourceSize.height = height
                        }

                        onButtonClicked: {
                            showSearchResultDetailView(name,address,mapX,mapY)
//                            if (address != "" && mapX >= 0 && mapY >= 0)
//                               requestRoute(index,mapX,mapY);
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
        height: parent.height
        Rectangle{
            width: parent.width
            height: parent.height
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
    }

    RecentDestinationPageViewOption {
        id: optionMenu
        anchors { bottom: parent.bottom; right: parent.right }
        onByHitClicked: {
            var data = { max: requestMax, offset: 0, type: 1, owner:CommonConstants.objectName(ViewRegistry.SearchTabView)}
            resetRecentDestList()
            sendEvent(TestRecentDestinationPageViewEvent.RequestRecentDestination, data)
        }
        onByOrderClicked: {
            var data = { max: requestMax, offset: 0, type: 0, owner:CommonConstants.objectName(ViewRegistry.SearchTabView) }
            resetRecentDestList()
            sendEvent(TestRecentDestinationPageViewEvent.RequestRecentDestination, data)
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
        onLeftPressed: {
            sendEvent( TestRecentDestinationPageViewEvent.ChangeSearchTabViewPage,
                {isCswControl:true, page:0})
        }

        onRightPressed: {
            sendEvent( TestRecentDestinationPageViewEvent.ChangeSearchTabViewPage,
                {isCswControl:true, page:2})
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

