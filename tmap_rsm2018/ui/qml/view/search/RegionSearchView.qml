import QtQuick 2.0
import ViewRegistry 1.0
import CommonConstants 1.0
import ZoneConstants 1.0
import SearchConstants 1.0
import RegionSearchViewEvent 1.0
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

    //언어 관련
    property var __lang             : null


    property var caller             : null
    property var query              : null

    //검색 요청 갯수
    property int reqMaxCount        : SearchConstants.RequestCount

    property string searchType      : CommonConstants.Empty

    //.RequestAddressType Event요청할 때 결과 값
    property string addressType     : SearchConstants.None

    property var listModel          : list1DepthModel
    property var listView           : list1DepthView
    property int currDepth          : -1

    property bool focused           : false
    property color selectColor      : MultiSenseCoordinator.kAmbientColor
    property color highlightColor   : CommonConstants.Transparent

    property bool messageVisible    : false

    property string firstDepthRegion: CommonConstants.Empty

    //서버 검색시 사용
    property string lcdName         : CommonConstants.Empty
    property string mcdName         : CommonConstants.Empty
    property string scdName         : CommonConstants.Empty


    //Timer
    property var timer              : null


    //ID
    property var list1DepthView     : null
    property var list2DepthView     : null
    property var list3DepthView     : null
    property var message            : null

    //Event
    property var textEnum           : ({})
    property var softKeyEnum        : ({})
    property var depthEnum          : ({})
    property var reqEnum            : ({})
    property var resEnum            : ({})

    property var textArr            : ([])
    property var softKeyMenuArr     : ([])

    function allocateTextEnum(){
        if (CommonUtils.getObjectLen(textEnum) <= 0) {
            textEnum = CommonUtils.createEnum(textEnum)
        }
        var i = 0
        CommonUtils.insertEnum(textEnum , "Zone1Title" , i++, [CommonConstants.Name] , [qsTr("지역 설정")                    ])
        CommonUtils.insertEnum(textEnum , "NoData"     , i++, [CommonConstants.Name] , [qsTr("검색 데이타가 없습니다.")      ])
        CommonUtils.insertEnum(textEnum , "All"        , i++, [CommonConstants.Name] , [qsTr("전체")                         ])
        CommonUtils.insertEnum(textEnum , "LessThan"   , i++, [CommonConstants.Name] , ["<"                                  ])
        CommonUtils.insertEnum(textEnum , "GrateThan"  , i++, [CommonConstants.Name] , [">"                                  ])
        CommonUtils.insertEnum(textEnum , "Space"      , i++, [CommonConstants.Name] , [String.fromCharCode(0x20)            ])
        textSync()
    }
    function textSync() {
        CommonUtils.fetchArr(textEnum , CommonConstants.Name , textArr)
    }

    function allocateSoftKeyEnum() {
        if (CommonUtils.getObjectLen(softKeyEnum) <= 0) {
            softKeyEnum = CommonUtils.createEnum(softKeyEnum)
        }
        var i = 0
        CommonUtils.insertEnum(softKeyEnum , "Back"     , i++, [CommonConstants.Name] , [ZoneConstants.Back   ])
        CommonUtils.insertEnum(softKeyEnum , "Option"   , i++, [CommonConstants.Name] , [ZoneConstants.Option ])
        CommonUtils.insertEnum(softKeyEnum , "Navi"     , i++, [CommonConstants.Name] , [ZoneConstants.Navi   ])
        CommonUtils.insertEnum(softKeyEnum , "Menu"     , i++, [CommonConstants.Name] , [ZoneConstants.Menu   ])

        CommonUtils.fetchArr(softKeyEnum , CommonConstants.Name , softKeyMenuArr)
    }

    function allocateDepthEnum(){
        if (CommonUtils.getObjectLen(depthEnum) <= 0) {
            depthEnum = CommonUtils.createEnum(depthEnum)
        }
        var i = 0
        CommonUtils.insertEnum(depthEnum , "None"   ,i++)
        CommonUtils.insertEnum(depthEnum , "First"  ,i++)
        CommonUtils.insertEnum(depthEnum , "Second" ,i++)
        CommonUtils.insertEnum(depthEnum , "Third"  ,i++)
    }

    function allocateReqEnum(){
        if (CommonUtils.getObjectLen(reqEnum) <= 0) {
            reqEnum = CommonUtils.createEnum(reqEnum)
        }
        var i = 0
        CommonUtils.insertEnum(reqEnum , "StartPageIndex", i++)
    }

    function allocateResEnum(){
        if (CommonUtils.getObjectLen(resEnum) <= 0) {
            resEnum = CommonUtils.createEnum(resEnum)
        }
        var i = 0
        CommonUtils.insertEnum(resEnum , "Name" ,i++)
        CommonUtils.insertEnum(resEnum , "Code" ,i++)
        CommonUtils.insertEnum(resEnum , "Count",i++)
    }

    function initEnum() {
        allocateTextEnum()
        allocateSoftKeyEnum()
        allocateDepthEnum()
        allocateReqEnum()
        allocateResEnum()
        if (active) {
            sendEvent(RegionSearchViewEvent.SelectZone1Title, textArr[textEnum.Zone1Title])
        }
    }

    function init(){
       initEnum()

       loader.sourceComponent = undefined
       loader.sourceComponent = main

       if (active) {
        sendEvent(RegionSearchViewEvent.SelectZone1Title, textArr[textEnum.Zone1Title])
       }
    }

    function requestSearch(code, requestPageIndex, maxCount, depth) {
        searchRoot.query = code
        searchRoot.currDepth = depth
        var params = ({})
        params[SearchConstants.StartIndex] = (requestPageIndex) ? (requestPageIndex * listView.maxCount) : 0
        params[SearchConstants.Count     ] = maxCount
        params[SearchConstants.Depth     ] = depth
        var data = ({})
        data[SearchConstants.Query       ] = code
        data[SearchConstants.AddressType ] = addressType
        data[SearchConstants.Params      ] = params
        sendEvent(RegionSearchViewEvent.RequestSearch, data)
    }

    function closeView() {
        var data = ({})
        //검색이 안될 때  메시지 박스를 표시하기 위하여
        data[CommonConstants.ObjectName] = CommonConstants.objectName(ViewRegistry.SearchResultView)
        data[CommonConstants.Visible   ] = true
        sendEvent(RegionSearchViewEvent.UpdateView, data)

        data = ({})
        data[CommonConstants.ObjectName] = CommonConstants.objectName(ViewRegistry.RegionSearchView)
        sendEvent(RegionSearchViewEvent.CloseView, data)
    }

    function  updateSearchResultView(code, text) {
        var params = ({})
        params[SearchConstants.LcdName  ] = lcdName
        params[SearchConstants.McdName  ] = mcdName
        params[SearchConstants.ScdName  ] = scdName

        var data = ({})
        data[CommonConstants.ObjectName ] = CommonConstants.objectName(ViewRegistry.SearchResultView)
        data[SearchConstants.Params     ] = params
        data[SearchConstants.AddrCode   ] = code        
        sendEvent(RegionSearchViewEvent.UpdateView, data)
    }

    function createModel(_result, startIndex, listModel) {
        var len = (_result) ? _result.length : 0
        var index =  startIndex
        for (var i = 0; i< len; i++) {
            var result = _result[i]
            var text = result[resEnum.Name]
            var code = result[resEnum.Code]
            var all = false
            if (CommonUtils.indexOf(text, textArr[textEnum.LessThan]) >= 0 &&
                CommonUtils.indexOf(text, textArr[textEnum.GrateThan]) >= 0) {
                all = true
            }
            listModel.set (index + i, {text: text, code: code, all: all})
        }
        messageVisible = listModel.count ? false : true
    }

    onMediated: {
        sendEvent(RegionSearchViewEvent.RequestLang)
        init()
        list1DepthView.visible = false
        list2DepthView.visible = false
        list3DepthView.visible = false
        searchRoot.query = 0

        //2018.05.14 - 설정된 주소방식이 아닌 구주소로 검색
        addressType = SearchConstants.OldAddrType

        requestSearch(searchRoot.query, reqEnum.StartPageIndex, list1DepthView.maxCount, depthEnum.First)


        //검색이 안될 때  메시지 박스를 표시하기 위하여
        var data = ({})
        data[CommonConstants.ObjectName] = CommonConstants.objectName(ViewRegistry.SearchResultView)
        data[CommonConstants.Visible   ] = false
        sendEvent(RegionSearchViewEvent.UpdateView, data)

    }

    onActivated: {
        sendEvent(RegionSearchViewEvent.ResetCsw)
        sendEvent(RegionSearchViewEvent.RegistCsw, [ csw ])
        sendEvent(RegionSearchViewEvent.SelectZone1Title, textArr[textEnum.Zone1Title])
        sendEvent(RegionSearchViewEvent.Activated, [CommonConstants.NoOption])
    }

    Component.onDestruction: {
        timer = CommonUtils.destroy(timer)
    }

    function showListView(depth) {
        switch (depth) {
           case depthEnum.First:
               listModel = list1DepthModel
               listView = list1DepthView
               break
           case depthEnum.Second:
               listModel = list2DepthModel
               listView = list2DepthView
               break
           case depthEnum.Third:
               listModel = list3DepthModel
               listView = list3DepthView
               break
        }
        list1DepthView.visible = CommonUtils.isEqual(depth , depthEnum.First)  ? true : false
        list2DepthView.visible = CommonUtils.isEqual(depth , depthEnum.Second) ? true : false
        list3DepthView.visible = CommonUtils.isEqual(depth , depthEnum.Third)  ? true : false
    }

    function setTitle(depth, name) {
        CommonUtils.insertEnum(textEnum, undefined,  depth,   [CommonConstants.Name],   [name])
        textSync()
    }

    function __onResult(search, result, totalCount, startIndex, depth) {
        startIndex = Math.max(0, startIndex)
        if (startIndex <= 0) {
            showListView(depth)
            listModel.clear()
            listView.contentHeight      = 0
            listView.contentY           = 0
            listView.currentIndex       = -1
            listView.totalPage          = 0
            listView.requestPageIndex   = 0
            listView.totalPage          = totalCount / listView.maxCount
            listView.totalPage         += (totalCount % listView.maxCount) ? 1 : 0
        }
        createModel(result, startIndex, listModel)
    }

    function processByModel(model) {
        listView.currentIndex = model.index
        var sleep =  GlobalConstants.kPressedColorExposureTime
        timer = CommonUtils.destroy(timer)
        timer = CommonUtils.delay(searchRoot, sleep, function() {
            try {
                if (searchRoot.focused) {
                    highlightColor = GlobalConstants.kListFocusColor
                } else {
                    highlightColor = CommonConstants.Transparent
                    //listView.currentIndex = -1
                }

                if (CommonUtils.isEqual(currDepth , depthEnum.First)) {
                    firstDepthRegion = model.text
                }

                var modelText = model.text
                if (!CommonUtils.isEqual(currDepth , depthEnum.First)) {
                    if (model.all) {
                        if (CommonUtils.isEqual(currDepth , depthEnum.Second)) {
                            mcdName   = CommonConstants.Empty
                        }
                        modelText = CommonConstants.Empty
                    } else {
                        modelText = model.text
                    }
                }

                if (CommonUtils.isEqual(currDepth , depthEnum.Third) || model.all) {
                    scdName = modelText
                    updateSearchResultView(model.code, modelText)
                } else {
                    if (CommonUtils.isEqual(currDepth , depthEnum.First)) {
                        lcdName = model.text
                    } else if (CommonUtils.isEqual(currDepth , depthEnum.Second)) {
                        mcdName = model.text
                    }
                    searchRoot.query = model.code
                    requestSearch(searchRoot.query, reqEnum.StartPageIndex, listView.maxCount, ++currDepth)
                }
            } catch(e) {
            }
        })

    }

    //언어가 변경되었을 때
    function __retranslateUi(lang) {
        initEnum()
        searchRoot.__lang = lang
    }

    function __doSoftKeyFunction(button, menuName, menuIndex) {
        switch(button) {
          case softKeyMenuArr[softKeyEnum.Back]:
              if (currDepth > depthEnum.First) {
                  showListView(--currDepth)
              } else {
                closeView()
              }
              break
          case softKeyMenuArr[softKeyEnum.Option]:
          case softKeyMenuArr[softKeyEnum.Navi]:
          case softKeyMenuArr[softKeyEnum.Menu]:
          default:
              break
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
            onParentChanged: {
                if (CommonUtils.isNull(parent))
                    return
                width = parent.width
                height = parent.height
            }
            Rectangle{
                width: parent.width
                height: parent.height
                color: GlobalConstants.kBasicBackgoundColor
                ListView {
                    Component.onCompleted: searchRoot.list1DepthView = this
                    width: parent.width
                    height: parent.height
                    property int rowHeight: GlobalConstants.kListTextItemHeight
                    property int maxCount: searchRoot.reqMaxCount
                    property int totalPage: 0
                    property int requestPageIndex: 0                    

                    clip: true
                    model: list1DepthModel
                    delegate: listDelegate
                    currentIndex: -1

                    property var pressedItem: null
                    PressBar {
                        currentItem: list1DepthView.pressedItem
                        leftMargin: GlobalConstants.kListBackgroundLeftMargin
                        rightMargin: GlobalConstants.kListBackgroundRightMargin
                    }
                    highlight: HighlightBar {
                        currentItem: list1DepthView.currentItem
                        leftMargin: GlobalConstants.kListBackgroundLeftMargin
                        rightMargin: GlobalConstants.kListBackgroundRightMargin
                        color: highlightColor
                    }
                    highlightFollowsCurrentItem: false

                    Image {
                          id: imgMask1
                          width: parent.width
                          height: 30
                          y: parent.y + (parent.height - height)
                          visible: scrollbar1Depth.heightRatio < 1.0 && !list1DepthView.dragging
                          sourceSize.width: width
                          sourceSize.height: height
                          source: Resource.imageUrl("zone/zone2_img_mask.png")
                    }

                    TScrollBar {
                        id: scrollbar1Depth
                        flickable: parent
                        width: 2
                        dynamicPage: list1DepthView.requestPageIndex
                        color: MultiSenseCoordinator.kAmbientColor
                        grooveColor: GlobalConstants.kScrollBarBackColor
                        rightMargin: GlobalConstants.kScrollBarRightMargin
                    }

                    onAtYEndChanged: {
                        if (!scrollbar1Depth.visible) {
                            return
                        }
                        if (!atYEnd) {
                             return
                        }
                        cancelFlick()
                        if (CommonUtils.isNull(list1DepthView)                ||
                            CommonUtils.isNull(list1DepthView.totalPage)      ||
                            CommonUtils.isNull(list1DepthView.requestPageIndex) ) {
                            return
                        }

                        if ( (list1DepthView.totalPage - 1) <= list1DepthView.requestPageIndex) {
                            return
                        }
                        requestSearch(searchRoot.query, ++list1DepthView.requestPageIndex, list1DepthView.maxCount, depthEnum.First)
                    }
                }

                ListView  {
                    Component.onCompleted: searchRoot.list2DepthView = this
                    width: list1DepthView.width
                    height: list1DepthView.height
                    property int rowHeight: GlobalConstants.kListTextItemHeight
                    property int maxCount: searchRoot.reqMaxCount
                    property int totalPage: 0
                    property int requestPageIndex: 0

                    clip: true
                    model: list2DepthModel
                    delegate: listDelegate
                    currentIndex: -1

                    property var pressedItem: null
                    PressBar {
                        currentItem: list2DepthView.pressedItem
                        leftMargin: GlobalConstants.kListBackgroundLeftMargin
                        rightMargin: GlobalConstants.kListBackgroundRightMargin
                    }
                    highlight: HighlightBar {
                        currentItem: list2DepthView.currentItem
                        leftMargin: GlobalConstants.kListBackgroundLeftMargin
                        rightMargin: GlobalConstants.kListBackgroundRightMargin
                        color: highlightColor
                    }
                    highlightFollowsCurrentItem: false

                    Image {
                          id: imgMask2
                          width: parent.width
                          height: 30
                          y: parent.y + (parent.height - height)
                          visible: scrollbar2Depth.heightRatio < 1.0 && !list2DepthView.dragging
                          sourceSize.width: width
                          sourceSize.height: height
                          source: Resource.imageUrl("zone/zone2_img_mask.png")
                    }

                    TScrollBar {
                        id: scrollbar2Depth
                        flickable: parent
                        width: 2
                        dynamicPage: list2DepthView.requestPageIndex
                        color: MultiSenseCoordinator.kAmbientColor
                        grooveColor: GlobalConstants.kScrollBarBackColor
                        rightMargin: GlobalConstants.kScrollBarRightMargin
                    }

                    onAtYEndChanged: {
                        if (!scrollbar2Depth.visible) {
                            return
                        }
                        if (!atYEnd) {
                             return
                        }
                        cancelFlick()
                        if (CommonUtils.isNull(list2DepthView)                ||
                            CommonUtils.isNull(list2DepthView.totalPage)      ||
                            CommonUtils.isNull(list2DepthView.requestPageIndex) ) {
                            return
                        }

                        if ( (list2DepthView.totalPage - 1) <= list2DepthView.requestPageIndex) {
                            return
                        }
                        requestSearch(searchRoot.query, ++list2DepthView.requestPageIndex, list2DepthView.maxCount, depthEnum.Second)
                    }
                }
                ListView  {
                    Component.onCompleted: searchRoot.list3DepthView = this
                    width: list1DepthView.width
                    height: list1DepthView.height
                    property int rowHeight: searchRoot.layout.rowHeight
                    property int maxCount: searchRoot.reqMaxCount
                    property int totalPage: 0
                    property int requestPageIndex: 0


                    clip: true
                    model: list3DepthModel
                    delegate: listDelegate
                    currentIndex: -1

                    property var pressedItem: null
                    PressBar {
                        currentItem: list3DepthView.pressedItem
                        leftMargin: GlobalConstants.kListBackgroundLeftMargin
                        rightMargin: GlobalConstants.kListBackgroundRightMargin
                    }
                    highlight: HighlightBar {
                        currentItem: list3DepthView.currentItem
                        leftMargin: GlobalConstants.kListBackgroundLeftMargin
                        rightMargin: GlobalConstants.kListBackgroundRightMargin
                        color: highlightColor
                    }
                    highlightFollowsCurrentItem: false

                    Image {
                          id: imgMask3
                          width: parent.width
                          height: 30
                          y: parent.y + (parent.height - height)
                          visible: scrollbar3Depth.heightRatio < 1.0 && !list3DepthView.dragging
                          sourceSize.width: width
                          sourceSize.height: height
                          source: Resource.imageUrl("zone/zone2_img_mask.png")
                    }

                    TScrollBar {
                        id: scrollbar3Depth
                        flickable: parent
                        width: 2
                        dynamicPage: list3DepthView.requestPageIndex
                        color: MultiSenseCoordinator.kAmbientColor
                        grooveColor: GlobalConstants.kScrollBarBackColor
                        rightMargin: GlobalConstants.kScrollBarRightMargin
                    }

                    onAtYEndChanged: {
                        if (!scrollbar3Depth.visible) {
                            return
                        }
                        if (!atYEnd) {
                             return
                        }
                        cancelFlick()
                        if (CommonUtils.isNull(list3DepthView)                ||
                            CommonUtils.isNull(list3DepthView.totalPage)      ||
                            CommonUtils.isNull(list3DepthView.requestPageIndex) ) {
                            return
                        }

                        if ( (list3DepthView.totalPage - 1) <= list3DepthView.requestPageIndex) {
                            return
                        }
                        requestSearch(searchRoot.query, ++list3DepthView.requestPageIndex, list3DepthView.maxCount, depthEnum.Third)
                    }
                }
                Text {
                    Component.onCompleted: searchRoot.message = this
                    width: parent.width
                    height: parent.height
                    visible: messageVisible
                    horizontalAlignment: Qt.AlignHCenter
                    verticalAlignment: Qt.AlignVCenter
                    font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(GlobalConstants.kGeneralMessageFontSize) }
                    color: GlobalConstants.kListTextColor
                    text: textArr[textEnum.NoData]
                }
            }
        }
    }

    ListModel { id: list1DepthModel   }
    ListModel { id: list2DepthModel   }
    ListModel { id: list3DepthModel   }

    Component {
        id: listDelegate
        Column{
            width: listView.width
            height: listView.rowHeight            

            property bool isSelected: CommonUtils.isEqual(listView.currentIndex , model.index) && CommonUtils.isEqual(highlightColor , selectColor)
            property bool isFocused: CommonUtils.isEqual(listView.currentIndex , model.index) && searchRoot.focused


            //Normal
            property color mainTxtColor      : GlobalConstants.kListTextColor
            property color subTxtColor       : GlobalConstants.kListSubTextColor


            function normalState() {
                mainTxtColor      = GlobalConstants.kListTextColor
                subTxtColor       = GlobalConstants.kListSubTextColor
                rightColumn.state = CommonConstants.Normal
            }

            function focusState() {
                mainTxtColor      = GlobalConstants.kBasicBackgoundColor
                subTxtColor       = GlobalConstants.kListSubTextColor
                rightColumn.state = CommonConstants.Pressed
            }

            function selectState() {
                mainTxtColor      = GlobalConstants.kListTextColor
                subTxtColor       = GlobalConstants.kListTextColor
                rightColumn.state = CommonConstants.Normal
            }

            onIsSelectedChanged: {
                if (isFocused) {
                    focusState()
                } else if (isSelected) {
                    selectState()
                } else {
                    normalState()
                }
            }

            onIsFocusedChanged: {
                if (isFocused) {
                    focusState()
                } else if (isSelected) {
                    selectState()
                } else {
                    normalState()
                }
            }

            function clickItem(){
              processByModel(model)
            }

            function getText() {
                var text = model.text
                if (model.all) {
                    text = CommonUtils.replace(text,textArr[textEnum.LessThan], CommonConstants.Empty)
                    text = CommonUtils.replace(text,textArr[textEnum.GrateThan], CommonConstants.Empty)
                    if (CommonUtils.isEqual(searchRoot.__lang, CommonConstants.En)) {
                        text = textArr[textEnum.All] + textArr[textEnum.Space] +  text
                    } else {
                        text += textArr[textEnum.Space] + textArr[textEnum.All]
                    }

                    if (CommonUtils.isEqual(currDepth , depthEnum.Third)) {
                        text =  firstDepthRegion + textArr[textEnum.Space] + textArr[textEnum.GrateThan] + textArr[textEnum.Space] +  text
                    }
                }
                return text
            }


            Item {
                width: parent.width
                height: parent.height - splitBar.height
                MouseArea {
                    width: parent.width
                    height: parent.height

                    PressBarMouseSync {
                        cswObj: csw
                        currentItem: listView.pressedItem
                        onCurrentItemChanged: {
                            if (CommonUtils.isEqual(currentItem, listView.pressedItem)) {
                                return
                            }
                            listView.pressedItem = currentItem
                        }
                    }

                    onClicked: {
                        highlightColor = Qt.binding(function(){ return MultiSenseCoordinator.kAmbientColor})
                        clickItem()
                    }
                }
                Row {
                    width: parent.width
                    height: parent.height
                    Item {width:GlobalConstants.kListTextLeftMargin;height:parent.height}
                    Text {
                        width: parent.width
                        height: parent.height
                        Component.onCompleted: CommonUtils.applyRemainWidth(this,parent)
                        horizontalAlignment: Qt.AlignLeft
                        verticalAlignment: Qt.AlignVCenter
                        font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(GlobalConstants.kOneLineListTextFontSize) }
                        color: mainTxtColor
                        text: getText()
                    }
                    TImageButton {
                          id: rightColumn
                          width: 63
                          height: 63
                          visible: CommonUtils.isEqual(currDepth , depthEnum.Third) ? false : (model.all ? false : true)
                          bg.sourceSize.width: width
                          bg.sourceSize.height: height
                          anchors.verticalCenter : parent.verticalCenter
                          bgSources: function (state) {
                              switch (state) {
                              case CommonConstants.Normal:  return isFocused ? "list_ic_more_f.png": "list_ic_more.png"
                              case CommonConstants.Pressed: return isFocused ? "list_ic_more_f.png": "list_ic_more.png"
                              }
                          }
                          onClicked: processByModel(model)
                    }
                    Item {width:GlobalConstants.kListTextRightMargin;height:parent.height}
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

        section: Csw.Section_List
        sequence: 0
        enable: searchRoot.visible


        onFocused: {
            searchRoot.focused = true
            highlightColor = GlobalConstants.kListFocusColor
            listView.currentIndex = -1
            listView.currentIndex = 0
        }
        onLostFocusd: {
            highlightColor = CommonConstants.Transparent
            searchRoot.focused = false
        }
        onLeftPressed: selected()
        onRightPressed: selected()
        onSpinCwPressed: {
            if (listView.currentIndex < listView.count - 1) {
                ++listView.currentIndex
                var item = listView.currentItem
                highlightColor = GlobalConstants.kListFocusColor
            }
        }
        onSpinCcwPressed: {
            if (0 < listView.currentIndex) {
                --listView.currentIndex
                var item = listView.currentItem
                highlightColor = GlobalConstants.kListFocusColor
            }
        }
        onSelected: {
            var item = listView.currentItem
            item.clickItem()
        }
    }
}


