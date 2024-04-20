import QtQuick 2.0
import ViewRegistry 1.0
import CommonConstants 1.0
import ZoneConstants 1.0
import SettingConstants 1.0
import SettingRouteSearchViewEvent 1.0
import Csw 1.0

import "../../singleton"
import "../../component"
import "../../view"
import "../../view/util"
import "../../view/setting"
import "../../GlobalConstants.js" as GlobalConstants
import "../../view/util/CommonUtils.js" as CommonUtils


SettingView {
    id: settingRoot


    //언어 관련
    property var __lang                 : null
    property bool needInit              : true

    property var caller                 : null
    property var settings               : null
    property var settingsMain           : null
    property int reqIdx                 : 0
    property bool reqContinue           : true
    property bool isActivated           : false


    property bool focused               : false
    property color highlightColor       : CommonConstants.Transparent
    property color selectcolor          : MultiSenseCoordinator.kAmbientColor

    //Timer
    property var timer                  : null

    //ID
    property var settingList            : null

    //Enum
    property var textEnum               : ({})
    property var componentEnum          : ({})
    property var reqEnum                : ({})
    property var listAttrEnum           : ({})
    property var alternateKeyEnum       : ({})


    property var textArr                : ([])
    property var componentCNameArr      : ([])
    property var componentCIDArr        : ([])
    property var listCNameArr           : ([])
    property var listCIdArr             : ([])
    property var listNameArr            : ([])
    property var listSectionArr         : ([])
    property var listKeyArr             : ([])
    property var listArg0Arr            : ([])
    property var listArg1Arr            : ([])
    property var listUrlArr             : ([])
    property var alternateKeyArr        : ([])
    property var alternateTextArr       : ([])
    property var listObjArr             : ([])
    property var subObjArr              : ([])

    function allocateVariant() {
    }

    function allocateTextEnum(){
        if (CommonUtils.getObjectLen(textEnum) <= 0) {
            textEnum = CommonUtils.createEnum(textEnum)
        }
        var i = 0
        CommonUtils.insertEnum(textEnum, "Zone1Title"               , i++, [CommonConstants.Name], [qsTr("경로탐색 설정")])
        //------------------------------------------------------------------------------------------------------------
        CommonUtils.insertEnum(textEnum, "RouteAlternateSelection"  , i++, [CommonConstants.Name], [qsTr("경로 대안 선택")])
        CommonUtils.insertEnum(textEnum, "BestPath"                 , i++, [CommonConstants.Name], [qsTr("최적길")])
        CommonUtils.insertEnum(textEnum, "MinimumTime"              , i++, [CommonConstants.Name], [qsTr("최소시간")])
        CommonUtils.insertEnum(textEnum, "FreeRoad"                 , i++, [CommonConstants.Name], [qsTr("무료도로")])
        CommonUtils.insertEnum(textEnum, "ShortestDistance"         , i++, [CommonConstants.Name], [qsTr("최단거리")])
        CommonUtils.insertEnum(textEnum, "ExpresswayPriority"       , i++, [CommonConstants.Name], [qsTr("고속도로우선")])
        CommonUtils.insertEnum(textEnum, "BeginnerPass"             , i++, [CommonConstants.Name], [qsTr("초보자경로")])
        //------------------------------------------------------------------------------------------------------------
        CommonUtils.insertEnum(textEnum, "AutoRedetection"          , i++, [CommonConstants.Name], [qsTr("자동 재탐색")])
        CommonUtils.insertEnum(textEnum, "RedetectRouteVeerOff"     , i++, [CommonConstants.Name], [qsTr("경로 이탈 재탐색")])
        CommonUtils.insertEnum(textEnum, "HipassMount"              , i++, [CommonConstants.Name], [qsTr("하이패스 장착")])
        //------------------------------------------------------------------------------------------------------------
        CommonUtils.insertEnum(textEnum, "Space"                    , i++, [CommonConstants.Name], [String.fromCharCode(0x20)])


        CommonUtils.fetchArr(textEnum, CommonConstants.Name, textArr)
    }

    function allocateComponentEnum(){
        if (CommonUtils.getObjectLen(componentEnum) <= 0) {
            componentEnum = CommonUtils.createEnum(componentEnum)
        }
        var i = 0
        CommonUtils.insertEnum(componentEnum, "SubMenu", i++, [CommonConstants.Name, CommonConstants.Kind], ["TextWithSubMenu", textWithSubMenu])
        CommonUtils.insertEnum(componentEnum, "Option2", i++, [CommonConstants.Name, CommonConstants.Kind], ["TextWithOption2", textWithOption2])

        CommonUtils.fetchArr(componentEnum, CommonConstants.Name, componentCNameArr)
        CommonUtils.fetchArr(componentEnum, CommonConstants.Kind, componentCIDArr)
    }

    function allocateListAttrEnum(){
        if (CommonUtils.isEmpty(textArr)) { allocateTextEnum() }
        if (CommonUtils.isEmpty(toggleArr)) { allocateToggleEnum() }

        if (CommonUtils.getObjectLen(listAttrEnum) <= 0) {
            listAttrEnum = CommonUtils.createEnum(listAttrEnum)
        }
        var i = 0
        CommonUtils.insertEnum(listAttrEnum, "RouteAlternateSelection", i++,
                               ["CName", "CId",CommonConstants.Name,  CommonConstants.Section, CommonConstants.Key, CommonConstants.Arg+"0", CommonConstants.Arg+"1", CommonConstants.Url],
                               [componentCNameArr[componentEnum.SubMenu], componentCIDArr[componentEnum.SubMenu], textArr[textEnum.RouteAlternateSelection]  , SettingConstants.RouteAlternateSelection, null,  null, null, CommonConstants.url(ViewRegistry.SettingRouteAlternateView)])
        CommonUtils.insertEnum(listAttrEnum, "AutoRedetection",         i++,
                               ["CName", "CId",CommonConstants.Name,  CommonConstants.Section, CommonConstants.Key, CommonConstants.Arg+"0", CommonConstants.Arg+"1", CommonConstants.Url],
                               [componentCNameArr[componentEnum.Option2], componentCIDArr[componentEnum.Option2], textArr[textEnum.AutoRedetection]          , SettingConstants.RouteSearch,   SettingConstants.AutoRedetection,        toggleArr[toggleEnum.Off], toggleArr[toggleEnum.On], null])
        CommonUtils.insertEnum(listAttrEnum, "RedetectRouteVeerOff",    i++,
                               ["CName", "CId",CommonConstants.Name,  CommonConstants.Section, CommonConstants.Key, CommonConstants.Arg+"0", CommonConstants.Arg+"1", CommonConstants.Url],
                               [componentCNameArr[componentEnum.Option2], componentCIDArr[componentEnum.Option2], textArr[textEnum.RedetectRouteVeerOff]     , SettingConstants.RouteSearch,   SettingConstants.RedetectRouteVeerOff,   toggleArr[toggleEnum.Off], toggleArr[toggleEnum.On], null])
        CommonUtils.insertEnum(listAttrEnum, "HipassMount",             i++,
                               ["CName", "CId",CommonConstants.Name,  CommonConstants.Section, CommonConstants.Key, CommonConstants.Arg+"0", CommonConstants.Arg+"1", CommonConstants.Url],
                               [componentCNameArr[componentEnum.Option2], componentCIDArr[componentEnum.Option2], textArr[textEnum.HipassMount]              , SettingConstants.RouteSearch,   SettingConstants.HipassMount,             toggleArr[toggleEnum.Off], toggleArr[toggleEnum.On], null])

        CommonUtils.fetchArr(listAttrEnum, "CName", listCNameArr)
        CommonUtils.fetchArr(listAttrEnum, "CId", listCIdArr)
        CommonUtils.fetchArr(listAttrEnum, CommonConstants.Name, listNameArr)
        CommonUtils.fetchArr(listAttrEnum, CommonConstants.Section, listSectionArr)
        CommonUtils.fetchArr(listAttrEnum, CommonConstants.Key, listKeyArr)
        CommonUtils.fetchArr(listAttrEnum, CommonConstants.Arg+"0", listArg0Arr)
        CommonUtils.fetchArr(listAttrEnum, CommonConstants.Arg+"1", listArg1Arr)
        CommonUtils.fetchArr(listAttrEnum, CommonConstants.Url,     listUrlArr)

        var len = listNameArr.length
        for(i=0; i<len;i++){
            listObjArr[i] = null
            subObjArr[i] = null
        }
    }


    function allocateAlternateKeyEnum() {
        if (CommonUtils.isEmpty(textArr)) { allocateTextEnum() }
        if (CommonUtils.getObjectLen(alternateKeyEnum) <= 0) {
            alternateKeyEnum = CommonUtils.createEnum(alternateKeyEnum)
        }
        var i = 0
        CommonUtils.insertEnum(alternateKeyEnum, "BestPath",            i++, [CommonConstants.Name, CommonConstants.Key], [textArr[textEnum.BestPath],           SettingConstants.BestPath])
        CommonUtils.insertEnum(alternateKeyEnum, "MinimumTime",         i++, [CommonConstants.Name, CommonConstants.Key], [textArr[textEnum.MinimumTime],        SettingConstants.MinimumTime])
        CommonUtils.insertEnum(alternateKeyEnum, "FreeRoad",            i++, [CommonConstants.Name, CommonConstants.Key], [textArr[textEnum.FreeRoad],           SettingConstants.FreeRoad])
        CommonUtils.insertEnum(alternateKeyEnum, "ShortestDistance",    i++, [CommonConstants.Name, CommonConstants.Key], [textArr[textEnum.ShortestDistance],   SettingConstants.ShortestDistance])
        CommonUtils.insertEnum(alternateKeyEnum, "ExpresswayPriority",  i++, [CommonConstants.Name, CommonConstants.Key], [textArr[textEnum.ExpresswayPriority], SettingConstants.ExpresswayPriority])
        CommonUtils.insertEnum(alternateKeyEnum, "BeginnerPass",        i++, [CommonConstants.Name, CommonConstants.Key], [textArr[textEnum.BeginnerPass],       SettingConstants.BeginnerPass])

        CommonUtils.fetchArr(alternateKeyEnum, CommonConstants.Name, alternateTextArr)
        CommonUtils.fetchArr(alternateKeyEnum, CommonConstants.Key, alternateKeyArr)
    }

    function init(){
        if (!needInit)
            return
        needInit = false

        allocateVariant()

        allocateSoftKeyEnum()
        allocateToggleEnum()

        allocateTextEnum()
        allocateComponentEnum()
        allocateListAttrEnum()
        allocateAlternateKeyEnum()


        loader.sourceComponent = undefined
        loader.sourceComponent = main

        if (active) {
            sendEvent(SettingRouteSearchViewEvent.SelectZone1Title, textArr[textEnum.Zone1Title])
            requestSetting()
        }
    }

    onMediated: {
        sendEvent(SettingRouteSearchViewEvent.RequestLang)
        init()
        createModel()
    }

    onActivated: {
        init()
        enabled = true
        sendEvent(SettingRouteSearchViewEvent.ResetCsw)
        sendEvent(SettingRouteSearchViewEvent.RegistCsw, [ csw ])

        sendEvent(SettingRouteSearchViewEvent.SelectZone1Title, textArr[textEnum.Zone1Title])
        sendEvent(SettingRouteSearchViewEvent.Activated, [CommonConstants.NoOption])

        if (isActivated)
            return
        isActivated = true

        requestSetting()
    }

    onDeactivated: {
        enabled = false
        sendEvent(SettingRouteSearchViewEvent.Deactivated)
    }

    Component.onDestruction: {
        timer = CommonUtils.destroy(timer)
    }

    function preSet(index) {
        settingRoot.reqIdx = index
        settingRoot.reqContinue = false
    }

    function requestSetting() {
        settings = null
        reqIdx =  0
        reqContinue = true

        var data = ({})
        data[CommonConstants.Section] = listSectionArr[reqIdx]
        sendEvent(SettingRouteSearchViewEvent.RequestSetting, data )

        resetCreateModel()
    }

    function createModel(){
        for (var i = 0; i<listCNameArr.length; i++) {
            var componentName = listCNameArr[i]
            settingList.rowComponent[componentName] = listCIdArr[i]
            if (CommonUtils.isEqual(componentName , componentCNameArr[componentEnum.SubMenu])) {
                listModel.set(i,
                {
                    component: componentName,
                    idx : i,
                    text: listNameArr[i],
                    value: (listModel.count > i) ? listModel.get(i).value : CommonConstants.Empty
                })
            } else  if (CommonUtils.isEqual(componentName , componentCNameArr[componentEnum.Option2])) {
                listModel.set(i,
                {
                    component: componentName,
                    idx : i,
                    text: listNameArr[i],
                    option1: listArg0Arr[i],
                    option2: listArg1Arr[i],
                    value: (listModel.count > i) ? listModel.get(i).value : CommonUtils.string(0)
                })
            }
        }
    }

    function resetCreateModel() {
        createModel()
        settingRoot.settingList.list.model = null
        settingRoot.settingList.list.model = listModel
    }


    onSettingsChanged: {
        var i = null
        var key = null
        var value = null
        if (CommonUtils.isNull(settings))
            return
        if (CommonUtils.isEqual(reqIdx , listAttrEnum.RouteAlternateSelection)) {
            var routeAlternate = CommonConstants.Empty
            for (i=listAttrEnum.RouteAlternateSelection; i<alternateKeyArr.length; i++) {
                key = alternateKeyArr[i]
                if (!settings.hasOwnProperty(key))
                    continue
                value = settings[key][CommonConstants.Value]
                if (CommonUtils.number(value)) {
                    if (!CommonUtils.isEmpty(routeAlternate))
                        routeAlternate += "," + textArr[textEnum.Space]
                    routeAlternate += alternateTextArr[i]
                }
            }
            listModel.setProperty(reqIdx, CommonConstants.Value,  routeAlternate)
            if (!reqContinue){
                if (CommonUtils.isNull(subObjArr[reqIdx]))
                    return
                subObjArr[reqIdx].text = routeAlternate
                return
            }
            var data = ({})
            data[CommonConstants.Section] = listSectionArr[++reqIdx]
            sendEvent(SettingRouteSearchViewEvent.RequestSetting, data )
        } else if (CommonUtils.isEqual(reqIdx , listAttrEnum.AutoRedetection)){
            for (i = listAttrEnum.AutoRedetection; i< listKeyArr.length; i++) {
                key = listKeyArr[i]
                if (!settings.hasOwnProperty(key))
                    continue
                value = settings[key][CommonConstants.Value]
                listModel.setProperty(i, CommonConstants.Value,  value)
            }
            settingsMain = settings
            ++reqIdx
        }
    }

    function createView(index) {
        isActivated = false

        var data = ({})
        data[CommonConstants.Url] = listUrlArr[index]
        data[CommonConstants.Caller] = caller
        sendEvent(SettingRouteSearchViewEvent.CreateView, data )
        preSet(index)
    }

    //언어가 변경되었을 때
    function __retranslateUi(lang) {
        settingRoot.__lang = lang
        needInit = true
        if (active) {
            init()
        }
    }

    function __doSoftKeyFunction(button, menuName, menuIndex) {
        var data = ({})
        switch(button) {
          case softKeyMenu[softKeyEnum.Back]:
              data[CommonConstants.ObjectName] = CommonConstants.objectName(ViewRegistry.SettingRouteSearchView)
              sendEvent(SettingRouteSearchViewEvent.CloseView, data)
              break
          case softKeyMenu[softKeyEnum.Option]:
          case softKeyMenu[softKeyEnum.Navi]:
          case softKeyMenu[softKeyEnum.Menu]:
          default:
              break
        }
    }


    //main
    Loader {
         id: loader
         width: parent.width
         height:parent.height
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
                SettingList {
                    Component.onCompleted: settingRoot.settingList = this
                    width: parent.width
                    height: parent.height
                    list.clip: true
                    list.model: listModel
                    list.currentIndex: -1
                    list.interactive: false

                    property var pressedItem: null
                    PressBar {
                        currentItem: settingList.pressedItem
                        leftMargin: GlobalConstants.kListBackgroundLeftMargin
                        rightMargin: GlobalConstants.kListBackgroundRightMargin
                    }
                    list.highlight: HighlightBar {
                        currentItem: settingList.list.currentItem
                        leftMargin: GlobalConstants.kListBackgroundLeftMargin
                        rightMargin: GlobalConstants.kListBackgroundRightMargin
                        color: highlightColor
                    }
                    list.highlightFollowsCurrentItem: false

                    TScrollBar {
                        Component.onCompleted: parent.scrollbar = this
                        flickable: parent.list
                        width: 2
                        color: MultiSenseCoordinator.kAmbientColor
                        grooveColor: GlobalConstants.kScrollBarBackColor
                        rightMargin: GlobalConstants.kScrollBarRightMargin
                        onScroll:    settingList.imgMask.visible = true
                        onMouseRelease:  settingList.imgMask.visible = false
                    }
                }
            }
         }
    }


    ListModel { id: listModel   }

    Component {
        id: textWithSubMenu
        Column {
            width: settingList.width
            height: GlobalConstants.kListTwoTextItemHeight

            property var itemModel: null
            property var subObj: null

            property bool isSelected: CommonUtils.isEqual(settingList.list.currentIndex , itemModel.idx) && CommonUtils.isEqual(highlightColor , selectcolor)
            property bool isFocused: CommonUtils.isEqual(settingList.list.currentIndex , itemModel.idx) && settingRoot.focused
            property bool isButtonFocused: false
            property alias buttonVisible: rightColumn.visible

            //Normal
            property color mainTxtColor      : GlobalConstants.kListTextColor
            property color subTxtColor       : GlobalConstants.kListSubTextColor

            onItemModelChanged: {
                if (CommonUtils.isUndefined(typeof itemModel) ||
                    CommonUtils.isNull(itemModel)             ||
                    CommonUtils.isUndefined(typeof itemModel.idx)) {
                    return
                }
                var idx = itemModel.idx
                listObjArr[idx] = this
                subObjArr[idx] = subObj
            }

            function execButton() {
                var idx = itemModel.idx
                settingList.list.currentIndex = idx
                createView(idx)
            }

            function clickItem() {
                var idx = itemModel.idx
                settingList.list.currentIndex = idx
                if (!settingRoot.focused) {
                    highlightColor = Qt.binding(function(){ return MultiSenseCoordinator.kAmbientColor})
                }
                var sleep =  GlobalConstants.kPressedColorExposureTime
                timer = CommonUtils.destroy(timer)
                timer = CommonUtils.delay(settingRoot, sleep, function() {
                    try {
                        if (settingRoot.focused) {
                            highlightColor = GlobalConstants.kListFocusColor
                        } else {
                            highlightColor = CommonConstants.Transparent
                            //settingList.list.currentIndex = -1
                        }
                        createView(idx)
                    } catch(e) {
                    }
                })
            }

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
                    isButtonFocused = false
                }
            }

            onIsFocusedChanged: {
                if (isFocused) {
                    focusState()
                } else if (isSelected) {
                    selectState()
                } else {
                    normalState()
                    isButtonFocused = false
                }
            }

            onIsButtonFocusedChanged: {
                if (isButtonFocused) {
                    normalState()
                } else {
                    if (isFocused) {
                        focusState()
                    } else if (isSelected) {
                        selectState()
                    }
                }
            }


            Item {
                width: parent.width
                height: parent.height - splitBar.height
                MouseArea {
                    width: parent.width
                    height: parent.height

                    PressBarMouseSync {
                        cswObj: csw
                        currentItem: settingList.pressedItem
                        onCurrentItemChanged: {
                            if (CommonUtils.isEqual(currentItem, settingList.pressedItem)) {
                                return
                            }
                            settingList.pressedItem = currentItem
                        }
                    }

                    onClicked: clickItem()
                }
                Row {
                    width: parent.width
                    height: parent.height
                    Item {width:GlobalConstants.kListTextLeftMargin;height:parent.height}
                    Item {width:CommonUtils.applyRemainWidth(this,parent);height:parent.height
                        Column {
                              width: parent.width
                              height: CommonUtils.applyHeight(this)
                              anchors.verticalCenter: parent.verticalCenter
                               Item {width: parent.width; height: 32
                                   Text {
                                      width: parent.width
                                      height: parent.height
                                      horizontalAlignment: Qt.AlignLeft
                                      verticalAlignment: Qt.AlignVCenter
                                      font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(GlobalConstants.kOneLineListTextFontSize)}
                                      color: mainTxtColor
                                      text: itemModel.text
                                    }
                               }
                               Item {width: parent.width; height: 12
                                   Component.onCompleted: CommonUtils.applyRemainHeight(this,parent)
                               }
                               Item {width: parent.width; height: 28
                                   Text {
                                       width: parent.width
                                       height: parent.height
                                       horizontalAlignment: Qt.AlignLeft
                                       verticalAlignment: Qt.AlignVCenter
                                       font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(GlobalConstants.kSecondLineListTextFontSize)}
                                       color: subTxtColor
                                       text: itemModel.value
                                       Component.onCompleted:  subObj = this
                                   }
                               }
                        }
                    }
                    TImageButton {
                          id: rightColumn
                          width: 63
                          height: 63
                          bg.sourceSize.width: width
                          bg.sourceSize.height: height
                          anchors.verticalCenter : parent.verticalCenter
                          bgSources: function (state) {
                              switch (state) {
                              case CommonConstants.Normal:  return isFocused ? "list_ic_more_f.png": "list_ic_more.png"
                              case CommonConstants.Pressed: return isFocused ? "list_ic_more_f.png": "list_ic_more.png"
                              }
                          }
                          onClicked: execButton()
                   }
                   Item {width:GlobalConstants.kListTextRightMargin;height:parent.height}
                }
            }
            Rectangle {
               id: splitBar
               x: GlobalConstants.kListLineLeftMargin
               width: parent.width - (GlobalConstants.kListLineLeftMargin  + GlobalConstants.kListLineRightMargin)
               height: CommonUtils.isEqual(itemModel.idx , 0) ? 2 : 1
               color: GlobalConstants.kListLineColor
            }
        }
    }

    Component {
        id: textWithOption2
        Column {
            width: settingList.width
            //서브텍스트가 있는 두 줄 리스트가 있을 때, 한 줄 리스트도 두 줄 리스트 높이에 맞게 조정되어야 합니다
            height: GlobalConstants.kListTwoTextItemHeight

            property var itemModel

            property bool isFocused: CommonUtils.isEqual(settingList.list.currentIndex , itemModel.idx) && settingRoot.focused
            property bool isButtonFocused: false
            property alias buttonVisible: options.visible
            property alias button: options

            function execButton() {
                var idx = itemModel.idx
                var newSettings = settingsMain
                var key = listKeyArr[idx]
                var value = options.itemIndex
                if (CommonUtils.isEqual(CommonUtils.number(itemModel.value),  value))
                    return
                itemModel.value = CommonUtils.string(value)
                newSettings[key][CommonConstants.Value] = CommonUtils.string(value)
                var data =({})
                data[CommonConstants.Section] = listSectionArr[idx]
                data[CommonConstants.Settings] = newSettings
                sendEvent(SettingRouteSearchViewEvent.SaveSetting, data )
            }

            function clickItem(){
                settingList.list.currentIndex = itemModel.idx
                if (!settingRoot.focused) {
                    highlightColor = Qt.binding(function(){ return MultiSenseCoordinator.kAmbientColor})
                }
                var sleep =  GlobalConstants.kPressedColorExposureTime
                timer = CommonUtils.destroy(timer)
                timer = CommonUtils.delay(settingRoot, sleep, function() {
                    try {
                        if (settingRoot.focused) {
                            highlightColor = GlobalConstants.kListFocusColor
                        } else {
                            highlightColor = CommonConstants.Transparent
                            //settingList.list.currentIndex = -1
                        }

                        var itemIndex = options.itemIndex
                        options.itemIndex = (++itemIndex % 2)
                    } catch(e) {
                    }
                })

            }

            function isBtnOnOff() {
                if (CommonUtils.isEqual(itemModel.option1 , toggleArr[toggleEnum.Off]) &&
                    CommonUtils.isEqual(itemModel.option2 , toggleArr[toggleEnum.On])) {
                    return true
                }
                return false
            }

            onIsFocusedChanged: {
                if (isFocused) {
                } else {
                    isButtonFocused = false
                }
            }
            onIsButtonFocusedChanged: {
                if (isButtonFocused) {
                } else {
                }
            }

            Item {
                width: parent.width
                height: parent.height - splitBar.height
                MouseArea {
                    width: parent.width
                    height: parent.height

                    PressBarMouseSync {
                        cswObj: csw
                        currentItem: settingList.pressedItem
                        onCurrentItemChanged: {
                            if (CommonUtils.isEqual(currentItem, settingList.pressedItem)) {
                                return
                            }
                            settingList.pressedItem = currentItem
                        }
                    }

                    onClicked: clickItem()
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
                        font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(GlobalConstants.kOneLineListTextFontSize)}
                        color: (isFocused && !isButtonFocused) ? GlobalConstants.kBasicBackgoundColor : GlobalConstants.kListTextColor
                        text: itemModel.text
                    }
                    OptionButton {
                        id: options
                        width:            isBtnOnOff() ? settingRoot.layout.option2OnOffWidth : settingRoot.layout.option2Width
                        height:           isBtnOnOff() ? settingRoot.layout.option2OnOffHeight : settingRoot.layout.option2Height
                        onWidthChanged:   CommonUtils.applyRemainWidth(parent.children[1],parent)
                        items:            [itemModel.option1,itemModel.option2]
                        images:           isBtnOnOff() ? [Resource.imageUrl("setting", "toggle_off.png"), Resource.imageUrl("setting", "toggle_on.png")] : []//[Resource.imageUrl("setting", "toggle_left.png"), Resource.imageUrl("setting", "toggle_right.png")]
                        bgSource:         isBtnOnOff() ? CommonConstants.Empty : Resource.imageUrl('toggle_btn_bg_2.png')
                        selectSource:     isBtnOnOff() ? CommonConstants.Empty : Resource.imageUrl('toggle_btn_slide_1.png')
                        normalColors:     isBtnOnOff() ? [CommonConstants.Transparent, CommonConstants.Transparent] : [GlobalConstants.kListTextColor, GlobalConstants.kListTextColor]
                        selectColors:     isBtnOnOff() ? [CommonConstants.Transparent, CommonConstants.Transparent] : [GlobalConstants.kBasicBackgoundColor, GlobalConstants.kBasicBackgoundColor]

                        cswObj: csw
                        box.radius: height/2
                        backgroundColor: GlobalConstants.kBasicBackgoundColor


                        highlightBorderWidth: 2
                        highlightBorderColor: CommonConstants.Transparent
                        highlightColor : CommonConstants.Transparent


                        itemIndex: CommonUtils.number(itemModel.value)
                        textFont { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(GlobalConstants.kToggleBTextFontSize)}
                        anchors.verticalCenter: parent.verticalCenter

                        onItemIndexChanged: execButton()
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

        section: Csw.Section_Setting
        sequence: 0
        enable: settingRoot.visible

        onFocused: {
            settingRoot.focused = true
            highlightColor = GlobalConstants.kListFocusColor
            settingList.list.currentIndex = -1
            settingList.list.currentIndex = 0
        }
        onLostFocusd: {
            highlightColor = CommonConstants.Transparent
            settingRoot.focused = false
            var item = settingList.list.currentItem.children[0]
            if (!CommonUtils.isNull(item.isButtonFocused)) {
                if (!CommonUtils.isNull(item.button)) {
                    if (!CommonUtils.isNull(item.button.items)) {
                          item.button.list.currentIndex = -1
                    }
                }
            }

        }
        onLeftPressed: selected()
        onRightPressed: selected()
        onSpinCwPressed: {
            if (settingList.list.currentIndex < settingList.list.count - 1) {
                var item = settingList.list.currentItem.children[0]
                if (!CommonUtils.isNull(item.isButtonFocused)  && item.isButtonFocused) {
                    if (!CommonUtils.isNull(item.button)) {
                        if (!CommonUtils.isNull(item.button.items)) {
                            item.button.list.currentIndex = -1
                        }
                    }
                    item.isButtonFocused = false
                }
                ++settingList.list.currentIndex
                highlightColor = GlobalConstants.kListFocusColor
            }
        }
        onSpinCcwPressed: {
            if (0 < settingList.list.currentIndex) {
                var item = settingList.list.currentItem.children[0]
                if (!CommonUtils.isNull(item.isButtonFocused) && item.isButtonFocused) {
                    if (!CommonUtils.isNull(item.button)) {
                        if (!CommonUtils.isNull(item.button.items)) {
                            item.button.list.currentIndex = -1
                        }
                    }
                    item.isButtonFocused = false
                }
                --settingList.list.currentIndex
                highlightColor = GlobalConstants.kListFocusColor
            }
        }
        onSelected: {
            var item = settingList.list.currentItem.children[0]
            if (!CommonUtils.isNull(item.isButtonFocused)  && item.isButtonFocused) {
                item.execButton()
            } else {
                item.clickItem()
            }
        }
    }

}

