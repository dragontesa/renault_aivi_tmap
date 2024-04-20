import QtQuick 2.0
import ViewRegistry 1.0
import CommonConstants 1.0
import ZoneConstants 1.0
import SettingConstants 1.0
import SettingRouteAlternateViewEvent 1.0
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

    property int  dialogContentWidth        : GlobalConstants.kDialogContentWidth
    property int  dialogContentHeight       : GlobalConstants.kDialogContentHeight

    //언어 관련
    property var __lang                     : null
    property bool needInit                  : true

    property var caller                     : null

    //[주의] caller가 RouteCandidateView인 경우에만 settings사용,
    //       이외에는 __updateModel(setting)함수를 사용
    property var settings                   : null
    property var newSettings                : settings

    //최적길은 Default로 체크되어 있으므로 제외하고 체크하도록 함
    readonly property int startIdx          : 1
    readonly property int maxSelect         : 2


    property bool focused                   : false
    property color highlightColor           : CommonConstants.Transparent

    //Timer
    property var timer                      : null

    //ID
    property var settingList                : null

    //Event
    property var textEnum                   : ({})
    property var listAttrEnum               : ({})

    property var textArr                    : ([])
    property var listArr                    : ([])

    function allocateVariant() {
    }

    function allocateTextEnum(){
        if (CommonUtils.getObjectLen(textEnum) <= 0) {
            textEnum = CommonUtils.createEnum(textEnum)
        }
        var i = 0
        CommonUtils.insertEnum(textEnum, "Zone1Title",          i++, [CommonConstants.Name], [qsTr("경로 대안 선택")])
        CommonUtils.insertEnum(textEnum, "OtherRoute",          i++, [CommonConstants.Name], [qsTr("다른 경로")])
        CommonUtils.insertEnum(textEnum, "DialogTitle",         i++, [CommonConstants.Name], [qsTr("알림")])
        CommonUtils.insertEnum(textEnum, "DialogMessage",       i++, [CommonConstants.Name], [qsTr("'최적길’을 제외한<br>다른 경로 2개를 선택해 주세요.")])
        CommonUtils.insertEnum(textEnum, "DialogMessageNoResp", i++, [CommonConstants.Name], [qsTr("설정값이 반영되지 않은 상태입니다.<br>")])
        CommonUtils.insertEnum(textEnum, "Confirm",             i++, [CommonConstants.Name], [qsTr("확인")])
        CommonUtils.insertEnum(textEnum, "Notice",              i++, [CommonConstants.Name], [qsTr("네트워크가 연결되지 않은 상태에서는 최적길, 최단거리,무료도로가<br>경로 대안으로 제공됩니다.")])

        //한글
        CommonUtils.insertEnum(textEnum, "DialogTitleKo",       i++, [CommonConstants.Name], ["알림"])
        CommonUtils.insertEnum(textEnum, "DialogMessageKo",     i++, [CommonConstants.Name], ["'최적길’을 제외한<br>다른 경로 2개를 선택해 주세요."])

        CommonUtils.fetchArr(textEnum, CommonConstants.Name, textArr)
    }

    function allocateListAttrEnum(){
        if (CommonUtils.getObjectLen(listAttrEnum) <= 0) {
            listAttrEnum = CommonUtils.createEnum(listAttrEnum)
        }
        var i = 0
        CommonUtils.insertEnum(listAttrEnum, "CName"     , i++)
        CommonUtils.insertEnum(listAttrEnum, "CId"       , i++)
        CommonUtils.insertEnum(listAttrEnum, "Name"      , i++)
        CommonUtils.insertEnum(listAttrEnum, "Section"   , i++)
        CommonUtils.insertEnum(listAttrEnum, "Key"       , i++)
        CommonUtils.insertEnum(listAttrEnum, "Object"    , i++)

        listArr = [
            ["TextWithCheck", textWithCheck, qsTr("최적길")      ,SettingConstants.RouteAlternateSelection ,SettingConstants.BestPath              , null   ],
            ["TextWithCheck", textWithCheck, qsTr("최소시간")    ,SettingConstants.RouteAlternateSelection ,SettingConstants.MinimumTime           , null   ],
            ["TextWithCheck", textWithCheck, qsTr("무료도로")    ,SettingConstants.RouteAlternateSelection ,SettingConstants.FreeRoad              , null   ],
            ["TextWithCheck", textWithCheck, qsTr("최단거리")    ,SettingConstants.RouteAlternateSelection ,SettingConstants.ShortestDistance      , null   ],
            ["TextWithCheck", textWithCheck, qsTr("고속도로우선"),SettingConstants.RouteAlternateSelection ,SettingConstants.ExpresswayPriority    , null   ],
            ["TextWithCheck", textWithCheck, qsTr("초보자경로")  ,SettingConstants.RouteAlternateSelection ,SettingConstants.BeginnerPass          , null   ],
        ]
    }

    //////////////////////////////////////
    //{Dialog사용
        property int dlgUniqueNum: -1
        function isShowDialog() {
            if (dlgUniqueNum >= 0)
                return true
            return false
        }

        function showDialog(uniqueNum, title,context,cancel) {
            if (CommonUtils.isEqual(dlgUniqueNum,uniqueNum)) {
                return
            }
             dlgUniqueNum = uniqueNum
             var data = ({})
             var isSetting = CommonUtils.isEqual(caller , CommonConstants.objectName(ViewRegistry.SettingMainView))

             data[CommonConstants.Url    ] = CommonConstants.url(ViewRegistry.CommonDialogView)
             data[CommonConstants.ParentObjectName] = CommonConstants.ContentRootForCommonDialog
             data[CommonConstants.Title  ] = title
             data[CommonConstants.Message] = context
             data[CommonConstants.Menus  ] = cancel ? [CommonConstants.Cancel, CommonConstants.Ok]
                                                    : [CommonConstants.Ok]
            var type = CommonConstants.Empty
            var s = CommonUtils.split(context, "<br>")
            switch(s.length) {
             case 0:
             case 1:
             case 2:
                 type =  CommonConstants.TypeB
                 break
             case 3:
                 type =  CommonConstants.TypeA
                 break
             default:
                 type =  CommonConstants.TypeC
                 break
            }
            data[CommonConstants.Type   ] = type
             var arg = ({})
             arg [CommonConstants.Caller ] = CommonConstants.objectName(ViewRegistry.SettingRouteAlternateView)
             data[CommonConstants.Arg    ] = arg
             data['showTimer'] = !isSetting

             sendEvent(SettingRouteAlternateViewEvent.CreateView, data)
        }

        function __closeDialog(yes) {
            var uniqNum = dlgUniqueNum
            closeDialog(uniqNum, yes)
        }

        function closeDialog(uniqNum, yes) {
            if (CommonUtils.isEqual(uniqNum, dlgUniqueNum)) {
                dlgUniqueNum = -1
            }
        }
    //}Dialog사용
    //////////////////////////////////////

    function init(){
        if (!needInit)
            return
        needInit = false

        allocateVariant()

        allocateSoftKeyEnum()
        allocateTextEnum()
        allocateListAttrEnum()


        dialogContentWidth = settingRoot.layout.dialogContentWidth
        dialogContentHeight = settingRoot.layout.dialogContentHeight
        loader.sourceComponent = undefined
        loader.sourceComponent = main

        if (active) {
            if (CommonUtils.isIndexOf(caller, CommonConstants.objectName(ViewRegistry.RouteCandidateView))) {
                sendEvent(SettingRouteAlternateViewEvent.SelectZone1Title, textArr[textEnum.OtherRoute])
            } else {
                sendEvent(SettingRouteAlternateViewEvent.SelectZone1Title, textArr[textEnum.Zone1Title])
            }
            resetCreateModel()
        }
    }

    onMediated: {
        sendEvent(SettingRouteAlternateViewEvent.RequestLang)
        init()
        createModel()
    }

    property bool isActivated: false
    onActivated: {
        init()
        sendEvent(SettingRouteAlternateViewEvent.ResetCsw)
        sendEvent(SettingRouteAlternateViewEvent.RegistCsw, [ csw ])
        if (CommonUtils.isIndexOf(caller, CommonConstants.objectName(ViewRegistry.RouteCandidateView))) {
            sendEvent(SettingRouteAlternateViewEvent.SelectZone1Title, textArr[textEnum.OtherRoute])
        } else {
            sendEvent(SettingRouteAlternateViewEvent.SelectZone1Title, textArr[textEnum.Zone1Title])
        }
        sendEvent(SettingRouteAlternateViewEvent.Activated, [CommonConstants.NoOption])

        if (isActivated)
            return
        isActivated = true

        if (caller === CommonConstants.objectName(ViewRegistry.RouteCandidateView)) {
            __updateModel(settings)
        } else {
            requestSetting()
        }
    }

    onDeactivated: {
        sendEvent(SettingRouteAlternateViewEvent.Deactivated)
    }

    Component.onDestruction: {
        timer = CommonUtils.destroy(timer)
    }

    function doEnableDisable() {
        var obj  = null
        var currSelectCount = 0
        var i = 0
        for (i=startIdx; i<listModel.count; i++) {
            obj = listArr[i][listAttrEnum.Object]
            if (CommonUtils.isNull(obj))
                continue
            if (obj.selected){
                currSelectCount++
            }
        }
        if (currSelectCount >= maxSelect) {
            for (i=startIdx; i<listModel.count; i++) {
                obj = listArr[i][listAttrEnum.Object]
                if (CommonUtils.isNull(obj))
                    continue
                if (!obj.selected) {
                    obj.enabled = false
                }
            }
        }
        else{
            for (i=startIdx; i<listModel.count; i++) {
                 obj = listArr[i][listAttrEnum.Object]
                 if (CommonUtils.isNull(obj))
                    continue
                 obj.enabled = true
            }
        }
    }

    function applySetting() {
        if (CommonUtils.isEqual(checkedNum() , maxSelect)) {
            var setting = []
            for (var i = 0; i < listArr.length; i++) {
                var key = listArr[i][listAttrEnum.Key]
                var value = CommonUtils.number(newSettings[key][CommonConstants.Value])
                if (value)
                    setting.push(key)
            }
            sendEvent(SettingRouteAlternateViewEvent.ApplySetting, setting)
        } else {
            showDialog(textEnum.DialogMessageKo, textArr[textEnum.DialogTitleKo], textArr[textEnum.DialogMessageKo], false)
        }
    }

    function requestSetting() {
        settings = null
        var data = ({})
        data[CommonConstants.Section] = listArr[0][listAttrEnum.Section]
        sendEvent(SettingRouteAlternateViewEvent.RequestSetting, data )

        resetCreateModel()
    }

    function saveSetting() {
        //[*주의*] DB의 최적길 값과 관계 없이 항상 최적길을 check되도록 ...
        newSettings[SettingConstants.BestPath][CommonConstants.Value] = CommonUtils.string(CommonConstants.True)

        var data = ({})
        data[CommonConstants.Section]= listArr[0][listAttrEnum.Section]
        data[CommonConstants.Settings] = newSettings
        sendEvent(SettingRouteAlternateViewEvent.SaveSetting, data )
    }

    function changedValue(index, value) {
        doEnableDisable()
        var key = listArr[index][listAttrEnum.Key]
        if (!newSettings.hasOwnProperty(key)) {
            return
        }
        newSettings[key][CommonConstants.Value] = CommonUtils.string(value)
        if (CommonUtils.isEqual(checkedNum() , maxSelect)) {
            saveSetting()
        }
    }

    function __updateModel(setting) {
        if (CommonUtils.isNull(setting))
            return
        for (var i = 0; i < listArr.length; i++) {
            var key = listArr[i][listAttrEnum.Key]
            if (!setting.hasOwnProperty(key))
                continue
            var value = setting[key][CommonConstants.Value]
            //[*주의*]  DB의 최적길 값과 관계 없이 항상 최적길을 check되도록 ...
            if (CommonUtils.isEqual(key , SettingConstants.BestPath)) {
                value = CommonUtils.string(CommonConstants.True)
                setting[SettingConstants.BestPath][CommonConstants.Value] = value
            }
            listModel.setProperty(i, CommonConstants.Value, value)
        }
        newSettings = setting
    }

    function createModel(){
        for (var i = 0; i<listArr.length; i++) {
            var componentName = listArr[i][listAttrEnum.CName]
            settingList.rowComponent[componentName] = listArr[i][listAttrEnum.CId]
            listModel.set(i,
            {
                component: componentName,
                idx : i,
                text: listArr[i][listAttrEnum.Name],
                value: (listModel.count > i) ? listModel.get(i).value : CommonUtils.string(0)
            })
        }
    }

    function resetCreateModel() {
        createModel()
        settingRoot.settingList.list.model = null
        settingRoot.settingList.list.model = listModel
    }

    function doConfirm() {
        if (CommonUtils.isEqual(checkedNum() , maxSelect)) {
            var data = ({})
            data[CommonConstants.ObjectName] = CommonConstants.objectName(ViewRegistry.SettingRouteAlternateView)
            sendEvent(SettingRouteAlternateViewEvent.CloseView, data)

            if ( CommonUtils.isEqual(caller , CommonConstants.objectName(ViewRegistry.RouteCandidateView))) {
                var data2 = ({})
                data2[CommonConstants.ObjectName] = caller

                sendEvent(SettingRouteAlternateViewEvent.ShowView, data2)
            }

            return true
        } else {
            showDialog(textEnum.DialogMessageKo, textArr[textEnum.DialogTitleKo], textArr[textEnum.DialogMessageKo], false)
            return false
        }
    }
    function checkedNum() {
        var count = 0
        var checked = []
        for (var i = startIdx; i < listArr.length; i++) {
            var key = listArr[i][listAttrEnum.Key]
            if (!newSettings.hasOwnProperty(key))
                continue
            var value = CommonUtils.number(newSettings[key][CommonConstants.Value])
            if (value) {
                ++count
                checked.push(i)
            }
        }
        console.log("checked list : " + checked)
        return count
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
        switch(button) {
          case softKeyMenu[softKeyEnum.Back]:
              if (!CommonUtils.isEqual(checkedNum() , maxSelect)) {
                  var showCancelButton = false
                  showDialog(textEnum.DialogMessageKo, textArr[textEnum.DialogTitleKo], textArr[textEnum.DialogMessageKo], showCancelButton)
                  break
              }
              doConfirm()
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
        Rectangle {
            onParentChanged: {
                if (CommonUtils.isNull(parent))
                    return
                width = parent.width
                height = parent.height
            }
            color: GlobalConstants.kBasicBackgoundColor
            Column {
                width: parent.width
                height: parent.height
                SettingList {
                    Component.onCompleted: settingRoot.settingList = this
                    width: parent.width
                    height: GlobalConstants.kListTextItemHeight * 6
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
                Item { width: parent.width; height: 40}
                Row {
                    width: parent.width
                    height: 62
                    Item {width:60;height: parent.height}
                    Text {
                        width: parent.width - (60*2)
                        height: parent.height

                        //데스크탑과 타겟에서 보여지는 폰트의 크기가 달라 텍스트가 길어질 경우 개행을 하도록 모드를 추가함
                        wrapMode: Text.WrapAnywhere

                        horizontalAlignment: Qt.AlignHCenter
                        verticalAlignment: Qt.AlignVCenter
                        font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(30)}
                        color: GlobalConstants.kListSubTextColor
                        text: CommonUtils.isEqual(caller , CommonConstants.objectName(ViewRegistry.SettingMainView)) ? textArr[textEnum.Notice] : CommonConstants.Empty
                        lineHeightMode: Text.FixedHeight
                        lineHeight: settingRoot.layout.textLineHeight
                    }
                    Item {width:60;height:parent.height}
                }
            }
         }
    }

    ListModel { id: listModel   }

    Component {
        id: textWithCheck
        Column {
            width: settingList.width
            height: GlobalConstants.kListTextItemHeight
            property var itemModel

            property bool isFocused: CommonUtils.isEqual(settingList.list.currentIndex , itemModel.idx) && settingRoot.focused
            property bool isButtonFocused: false
            property alias buttonVisible: checkButton.visible

            onItemModelChanged: {
                if (CommonUtils.isUndefined(typeof itemModel) ||
                    CommonUtils.isNull(itemModel)             ||
                    CommonUtils.isUndefined(typeof itemModel.idx)) {
                    return
                }
                var i = itemModel.idx
                var obj = listArr[i][listAttrEnum.Object] = checkButton
                switch (i) {
                  case 0://기본값 처리
                      obj.enabled = false
                      break
                   default:
                       break
                }
                if (CommonUtils.isEqual(i , (listArr.length - 1)))
                    doEnableDisable()
            }


            function execButton() {
                settingList.list.currentIndex = itemModel.idx
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

            function clickItem() {
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
                        if (checkButton.enabled) {
                            checkButton.selected = checkButton.selected ? CommonConstants.False: CommonConstants.True
                        }
                    } catch(e) {
                    }
                })
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
                    TCheckButton {
                        id: checkButton
                        width: settingRoot.layout.checkboxWith
                        height: settingRoot.layout.checkboxHeight
                        selected: CommonUtils.number(itemModel.value)
                        img.width:  width
                        img.height: height
                        imgCheck:  Resource.imageUrl("checkbox_on.png")
                        imgUncheck: Resource.imageUrl("checkbox_off.png")
                        anchors.verticalCenter: parent.verticalCenter
                        onSelectedChanged: {
                            itemModel.value = CommonUtils.string(selected)
                            changedValue(itemModel.idx, selected)
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
        }
        onLeftPressed: selected()
        onRightPressed: selected()
        onSpinCwPressed: {
            if (settingList.list.currentIndex < settingList.list.count - 1) {
                ++settingList.list.currentIndex
                var item = settingList.list.currentItem.children[0]
                if (!CommonUtils.isNull(item.isButtonFocused)  && item.isButtonFocused) {
                    item.isButtonFocused = false
                }
                highlightColor = GlobalConstants.kListFocusColor
            }
        }
        onSpinCcwPressed: {
            if (0 < settingList.list.currentIndex) {
                --settingList.list.currentIndex
                var item = settingList.list.currentItem.children[0]
                if (!CommonUtils.isNull(item.isButtonFocused))
                    item.isButtonFocused = false
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
