import QtQuick 2.0
import ViewRegistry 1.0
import CommonConstants 1.0
import ZoneConstants 1.0
import SettingConstants 1.0
import SettingMainViewEvent 1.0
import SceneGraphRendering 1.0
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

    background.visible                  : false
    fader.fadingOpacity                 : 0
    mouseArea.enabled                   : false

    readonly property int frameHeight   : 337
    property int  dialogContentWidth    : GlobalConstants.kDialogContentWidth
    property int  dialogContentHeight   : GlobalConstants.kDialogContentHeight

    property bool sync                  : true

    //언어 관련
    property var __lang                 : null
    property bool needInit              : true

    property var caller                 : null

    property bool focused               : false

    property size tileMenuImgSize       : Qt.size(71,71)

    property int gridRow                : 2
    property var gridCol                : null


    //ID
    property var menu                   : null

    //Enum
    property var textEnum               : ({})
    property var menuEnum               : ({})

    property var textArr                : ([])
    property var menuName               : ([])
    property var menuUrl                : ([])
    property var menuArg                : ([])
    property var menuObj                : ([])
    property var menuImgN               : ([])
    property var menuImgP               : ([])
    property var menuImgF               : ([])
    property var menuArr                : ([])

    function allocateVariant() {
    }

    function allocateTextEnum(){
        if (CommonUtils.getObjectLen(textEnum) <= 0) {
            textEnum = CommonUtils.createEnum(textEnum)
        }
        var i = 0
        CommonUtils.insertEnum(textEnum, "Zone1Title",      i++, [CommonConstants.Name], [qsTr("환경설정")])
        CommonUtils.insertEnum(textEnum, "ToastMessage",    i++, [CommonConstants.Name], [qsTr("환경설정이 초기화 되었습니다")])
        CommonUtils.insertEnum(textEnum, "DialogTitle",     i++, [CommonConstants.Name], [qsTr("환경설정 초기화")])
        CommonUtils.insertEnum(textEnum, "DialogMessage",   i++, [CommonConstants.Name], [qsTr("환경설정을 초기화하시겠습니까?<br>기존 설정정보가 모두 초기화됩니다.")])

        //한글
        CommonUtils.insertEnum(textEnum, "DialogTitleKo",   i++, [CommonConstants.Name], ["환경설정 초기화"])
        CommonUtils.insertEnum(textEnum, "DialogMessageKo", i++, [CommonConstants.Name], ["환경설정을 초기화하시겠습니까?<br>기존 설정정보가 모두 초기화됩니다."])
        CommonUtils.insertEnum(textEnum, "ToastMessageKo",  i++, [CommonConstants.Name], ["환경설정이 초기화 되었습니다"])


        CommonUtils.fetchArr(textEnum, CommonConstants.Name, textArr)
    }

    function allocateMenuEnum(){
        if (CommonUtils.getObjectLen(menuEnum) <= 0) {
            menuEnum = CommonUtils.createEnum(menuEnum)
        }
        var i = 0
        CommonUtils.insertEnum(menuEnum, "Map",             i++, [CommonConstants.Name,
                                                                  CommonConstants.Url,
                                                                  CommonConstants.Arg,
                                                                  CommonConstants.Object,
                                                                  CommonConstants.Normal,
                                                                  CommonConstants.Press,
                                                                  CommonConstants.Focus]    ,
                                                                 [qsTr("지도 설정"),
                                                                  CommonConstants.url(ViewRegistry.SettingMapView),
                                                                  null,
                                                                  null,
                                                                  "settingmenu/setting_ic_1.png",
                                                                  "settingmenu/setting_ic_1.png",
                                                                  "settingmenu/setting_ic_1_f.png"
                                                                 ])
        CommonUtils.insertEnum(menuEnum, "RouteSearch",     i++, [CommonConstants.Name,
                                                                  CommonConstants.Url,
                                                                  CommonConstants.Arg,
                                                                  CommonConstants.Object,
                                                                  CommonConstants.Normal,
                                                                  CommonConstants.Press,
                                                                  CommonConstants.Focus]    ,
                                                                 [qsTr("경로탐색 설정"),
                                                                  CommonConstants.url(ViewRegistry.SettingRouteSearchView),
                                                                  null,
                                                                  null,
                                                                  "settingmenu/setting_ic_2.png",
                                                                  "settingmenu/setting_ic_2.png",
                                                                  "settingmenu/setting_ic_2_f.png"
                                                                 ])
        CommonUtils.insertEnum(menuEnum, "GpsInfo",         i++, [CommonConstants.Name,
                                                                  CommonConstants.Url,
                                                                  CommonConstants.Arg,
                                                                  CommonConstants.Object,
                                                                  CommonConstants.Normal,
                                                                  CommonConstants.Press,
                                                                  CommonConstants.Focus]    ,
                                                                 [qsTr("GPS 정보"),
                                                                  CommonConstants.url(ViewRegistry.SettingGpsInfoView),
                                                                  null,
                                                                  null,
                                                                  "settingmenu/setting_ic_3.png",
                                                                  "settingmenu/setting_ic_3.png",
                                                                  "settingmenu/setting_ic_3_f.png"
                                                                 ])
        CommonUtils.insertEnum(menuEnum, "NaviInfo",        i++, [CommonConstants.Name,
                                                                  CommonConstants.Url,
                                                                  CommonConstants.Arg,
                                                                  CommonConstants.Object,
                                                                  CommonConstants.Normal,
                                                                  CommonConstants.Press,
                                                                  CommonConstants.Focus]    ,
                                                                 [qsTr("내비게이션<br>정보"),
                                                                  CommonConstants.url(ViewRegistry.SettingNaviInfoView),
                                                                  null,
                                                                  null,
                                                                  "settingmenu/setting_ic_4.png",
                                                                  "settingmenu/setting_ic_4.png",
                                                                  "settingmenu/setting_ic_4_f.png"
                                                                 ])
        CommonUtils.insertEnum(menuEnum, "Guid",            i++, [CommonConstants.Name,
                                                                  CommonConstants.Url,
                                                                  CommonConstants.Arg,
                                                                  CommonConstants.Object,
                                                                  CommonConstants.Normal,
                                                                  CommonConstants.Press,
                                                                  CommonConstants.Focus]    ,
                                                                 [qsTr("안내 설정"),
                                                                  CommonConstants.url(ViewRegistry.SettingGuidView),
                                                                  null,
                                                                  null,
                                                                  "settingmenu/setting_ic_5.png",
                                                                  "settingmenu/setting_ic_5.png",
                                                                  "settingmenu/setting_ic_5_f.png"
                                                                 ])
        CommonUtils.insertEnum(menuEnum, "MapUpdate",       i++, [CommonConstants.Name,
                                                                  CommonConstants.Url,
                                                                  CommonConstants.Arg,
                                                                  CommonConstants.Object,
                                                                  CommonConstants.Normal,
                                                                  CommonConstants.Press,
                                                                  CommonConstants.Focus]    ,
                                                                 [qsTr("지도 업데이트"),
                                                                  CommonConstants.url(ViewRegistry.SettingMapUpdateView),
                                                                  null,
                                                                  null,
                                                                  "settingmenu/setting_ic_6.png",
                                                                  "settingmenu/setting_ic_6.png",
                                                                  "settingmenu/setting_ic_6_f.png"
                                                                 ])
        CommonUtils.insertEnum(menuEnum, "MobileTmap",      i++, [CommonConstants.Name,
                                                                  CommonConstants.Url,
                                                                  CommonConstants.Arg,
                                                                  CommonConstants.Object,
                                                                  CommonConstants.Normal,
                                                                  CommonConstants.Press,
                                                                  CommonConstants.Focus]    ,
                                                                 [qsTr("모바일 T map<br>연동"),
                                                                  CommonConstants.url(ViewRegistry.SettingMobileTmapView),
                                                                  null,
                                                                  null,
                                                                  "settingmenu/setting_ic_8.png",
                                                                  "settingmenu/setting_ic_8.png",
                                                                  "settingmenu/setting_ic_8_f.png"
                                                                 ])
        CommonUtils.insertEnum(menuEnum, "Init",            i++, [CommonConstants.Name,
                                                                  CommonConstants.Url,
                                                                  CommonConstants.Arg,
                                                                  CommonConstants.Object,
                                                                  CommonConstants.Normal,
                                                                  CommonConstants.Press,
                                                                  CommonConstants.Focus]    ,
                                                                 [qsTr("설정 초기화"),
                                                                  CommonConstants.Empty,
                                                                  null,
                                                                  null,
                                                                  "settingmenu/setting_ic_7.png",
                                                                  "settingmenu/setting_ic_7.png",
                                                                  "settingmenu/setting_ic_7_f.png"
                                                                 ])


        CommonUtils.fetchArr(menuEnum, CommonConstants.Name, menuName)
        CommonUtils.fetchArr(menuEnum, CommonConstants.Url, menuUrl)
        CommonUtils.fetchArr(menuEnum, CommonConstants.Arg, menuArg)
        CommonUtils.fetchArr(menuEnum, CommonConstants.Object, menuObj)
        CommonUtils.fetchArr(menuEnum, CommonConstants.Normal, menuImgN)
        CommonUtils.fetchArr(menuEnum, CommonConstants.Press, menuImgP)
        CommonUtils.fetchArr(menuEnum, CommonConstants.Focus, menuImgF)

        if (!CommonUtils.isNull(gridCol)) {
           gridRow = CommonUtils.getGridRowCol(menuName, gridCol)
        } else if (!CommonUtils.isNull(gridRow)) {
           gridCol = CommonUtils.getGridRowCol(menuName, gridRow)
        }
    }

    function allocateMenuArr(menu){
        var menuAttrEnum = menu.menuAttrEnum
        menu.gridCol = settingRoot.gridCol
        menu.gridRow = settingRoot.gridRow
        menuArr = []
        for(var i = 0; i< menu.gridRow; i++) {
            menuArr[i] = []
            for(var j = 0; j<menu.gridCol; j++ ){
                menuArr[i][j] = []
                var k = (i * menu.gridCol) + (j)
                menuArr[i][j][menuAttrEnum.Name ] = menuName[k]
                menuArr[i][j][menuAttrEnum.Key  ] = menuName[k]
                menuArr[i][j][menuAttrEnum.Arg  ] = menuArg[k]
                menuArr[i][j][menuAttrEnum.Obj  ] = menuObj[k]
                menuArr[i][j][menuAttrEnum.ImgN ] = menuImgN[k]
                menuArr[i][j][menuAttrEnum.ImgP ] = menuImgP[k]
                menuArr[i][j][menuAttrEnum.ImgF ] = menuImgF[k]
             }
        }
        menu.menuArr = menuArr
    }    

    function viewSizeAndModeChanged(height){
        var data = ({})
        data[CommonConstants.B] = height
        sendEvent(SettingMainViewEvent.ViewSizeChanged, data)

        requestDirectionGuid()
    }

    function showToast(message)   {
        var data = ({})
        data[CommonConstants.Message] = message
        data[CommonConstants.Position] = CommonConstants.ToastUp
        data[CommonConstants.Duration] = 3 * 1000
        sendEvent(SettingMainViewEvent.ShowToast ,data)
    }


    //맵모드의 설정값 요청: __resultSettingDirectionGuid함수로 응답
    function requestDirectionGuid() {
        var data = ({})
        data[CommonConstants.Section] = SettingConstants.DirectionGuid
        data[CommonConstants.Slot] = "__resultSettingDirectionGuid"
        sendEvent(SettingMainViewEvent.RequestSetting, data )
    }

    //맵모드의 설정값 (requestDirectionGuid)요청에 대한 응답
    function __resultSettingDirectionGuid(settings) {
        if (CommonUtils.isNull(settings))
            return
        if (!settings.hasOwnProperty(SettingConstants.MapMode))
            return
        var value = settings[SettingConstants.MapMode][CommonConstants.Value]
        var index = CommonUtils.number(value)
        changeMapViewMode(index)
    }

  function changeMapViewMode(index) {
      switch (index) {
      case SettingConstants.MapModeBirdView:
          mapView.viewMode = Renderer.BirdView
          break
      case SettingConstants.MapModeDriveDirection:
          mapView.viewMode = Renderer.HeadUp
          break
      case SettingConstants.MapModeNorthDirection:
          mapView.viewMode = Renderer.NorthBound
          break
      }
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
            arg [CommonConstants.Caller ] = CommonConstants.objectName(ViewRegistry.SettingMainView)
            data[CommonConstants.Arg    ] = arg
            sendEvent(SettingMainViewEvent.CreateView, data)
        }

        function __closeDialog(yes) {
            var uniqNum = dlgUniqueNum
            closeDialog(uniqNum, yes)
        }

        function closeDialog(uniqNum, yes) {
            if (yes){
               sendEvent(SettingMainViewEvent.InitSetting)
               showToast(textArr[textEnum.ToastMessageKo])
            }
            if (CommonUtils.isEqual(uniqNum, dlgUniqueNum)) {
                dlgUniqueNum = -1
            }
        }
    //}Dialog사용
    //////////////////////////////////////

    function doWorkMenuItem(row, col) {
        var index = (row * gridCol) + (col)
        var name = menuName[index]
        if(name.length <= 0) {
          return
        }

        if (CommonUtils.isEqual(index , menuEnum.Init)){
             showDialog(textEnum.DialogMessageKo, textArr[textEnum.DialogTitleKo], textArr[textEnum.DialogMessageKo], true)
        } else {
            var data = ({})
            if (CommonUtils.isEqual(index , menuEnum.MapUpdate)) {
                sendEvent(SettingMainViewEvent.RequestViewList)
                //맵업데이트 중인 경우
                if (visibleSettingMapUpdateView) {
                    data[CommonConstants.ObjectName] = CommonConstants.objectName(ViewRegistry.SettingMapUpdateView)
                    data[CommonConstants.Visible] = true
                    //data[CommonConstants.Z] = CommonConstants.MAX_INT_VALUE
                    sendEvent(SettingMainViewEvent.UpdateView, data)
                    data = ({})
                }
            }
            data[CommonConstants.Url   ] = menuUrl[index]
            data[CommonConstants.Caller] = CommonConstants.objectName(ViewRegistry.SettingMainView)
            sendEvent(SettingMainViewEvent.CreateView, data )
        }
    }

    function init(){
        if (!needInit)
            return
        needInit = false

        allocateVariant()

        allocateSoftKeyEnum()
        allocateTextEnum()
        allocateMenuEnum()

        dialogContentWidth =  settingRoot.layout.dialogContentWidth
        dialogContentHeight =  settingRoot.layout.dialogContentHeight

        loader.sourceComponent = undefined
        loader.sourceComponent = main

        if (active) {
            sendEvent(SettingMainViewEvent.SelectZone1Title, textArr[textEnum.Zone1Title])
        }
    }

    onMediated: {
        sendEvent(SettingMainViewEvent.RequestLang)
        init()
    }

    Component.onDestruction: {
    }

    onActivated: {
        if (!sync) {
            sync = true
            return
        }
        init()
        enabled = true
        sendEvent(SettingMainViewEvent.ResetCsw)
        sendEvent(SettingMainViewEvent.RegistCsw, [ csw ])

        sendEvent(SettingMainViewEvent.SelectZone1Title, textArr[textEnum.Zone1Title])
        sendEvent(SettingMainViewEvent.Activated,[CommonConstants.NoOption])
        viewSizeAndModeChanged(frameHeight)
    }

    onDeactivated: {
        enabled = false
        sendEvent(SettingMainViewEvent.Deactivated)
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
              data[CommonConstants.ObjectName] = caller
              data[CommonConstants.Sync] = true
              sendEvent(SettingMainViewEvent.UpdateView, data)
              data = ({})
              data[CommonConstants.ObjectName] = CommonConstants.objectName(ViewRegistry.SettingMainView)
              sendEvent(SettingMainViewEvent.CloseView, data)
              break
           case softKeyMenu[softKeyEnum.Option]:
           case softKeyMenu[softKeyEnum.Navi]:
           case softKeyMenu[softKeyEnum.Menu]:
           default:
               break
        }
    }

    property var visibleSettingMapUpdateView : null
    function __onResultViewList(result) {
        for(var i= 0; i<result.length; i++) {
            if (CommonUtils.isEqual(result[i][CommonConstants.ResultViewId], ViewRegistry.SettingMapUpdateView)) {
                visibleSettingMapUpdateView = true
                return
            }
        }
        visibleSettingMapUpdateView = false
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
            Item {
               width: parent.width
               height: parent.height - frameHeight
            }
            TileMenu {
                Component.onCompleted: {settingRoot.menu = this
                    allocateMenuArr(menu)
                }
                width: parent.width
                height: frameHeight - 1
                PressBar { currentItem: menu.listView.pressedItem;opacity: 0.3 }
                highlight: HighlightBar {
                    currentItem: menu.listView.currentItem
                    color: menu.highlightColor
                }
                backgroundColor: GlobalConstants.kNormalColor
                onButtonCompleted: {
                    var item = menuArr[row][col][menuAttrEnum.Obj]
                    if (!CommonUtils.isNull(item)) {
                        item.img.width = tileMenuImgSize.width
                        item.img.height = tileMenuImgSize.height
                        item.img.y = settingRoot.layout.tileMenuContentButtonYOffset

                        item.txt.elide       = Text.ElideRight
                        item.txt.font.family =  GlobalConstants.kFontFamily
                        item.txt.font.pixelSize = Resource.pixelToDp(GlobalConstants.kOneLineListTextFontSize)
                        item.txt.y =  settingRoot.layout.tileMenuContentTextYOffset
                        item.txt.lineHeightMode = Text.FixedHeight
                        item.txt.lineHeight = settingRoot.layout.tileMenuContentTextLineHeight

                        item.outlineColor = GlobalConstants.kBasicBackgoundColor
                    }
                }

                onButtonFocused: {
                    var item = menuArr[row][col][menuAttrEnum.Obj]
                     if (!CommonUtils.isNull(item)) {
                         item.txt.colorN = GlobalConstants.kBasicBackgoundColor
                         var imgF = menuArr[row][col][menuAttrEnum.ImgF]
                         item.img.sourceN  = Resource.imageUrl(imgF)
                     }
                }

                onButtonLostFocused: {
                    var item = menuArr[row][col][menuAttrEnum.Obj]
                     if (!CommonUtils.isNull(item)) {
                         item.txt.colorN =  GlobalConstants.kListTextColor
                         var imgN = menuArr[row][col][menuAttrEnum.ImgN]
                         item.img.sourceN  = Resource.imageUrl(imgN)
                     }
                }

                property var pressed: null
                property var currRow: null
                property var currCol: null
                onButtonPressed:  {
                    currRow = row
                    currCol = col
                    pressed = true

                    var index = (row * gridCol) + (col)
                    var name = menuName[index]
                    if(name.length <= 0) {
                      return
                    }
                    listView.currentIndex = index
                    if (!menu.focused) {
                        menu.highlightColor = Qt.binding(function(){ return MultiSenseCoordinator.kAmbientColor})
                    }
                }
                onButtonReleased: {
                    currRow = row
                    currCol = col
                    pressed = false
                    menu.highlightColor = CommonConstants.Transparent
                    listView.currentIndex = -1
                }
                PressBarMouseSync {
                    cswObj: csw
                    pressed: parent.pressed
                    currentItem: {
                        if (CommonUtils.isNull(menu.listView) ||
                            CommonUtils.isNull(menu.listView.pressedItem)) {
                            return null
                        }
                        return menu.listView.pressedItem
                    }
                    object: {
                        if (CommonUtils.isNull(parent.currRow) ||
                            CommonUtils.isNull(parent.currCol)) {
                            return null
                        }
                        return menuArr[parent.currRow][parent.currCol][parent.menuAttrEnum.Obj]
                    }
                    onCurrentItemChanged: {
                        if (CommonUtils.isNull(menu.listView))
                            return
                        if (CommonUtils.isEqual(currentItem,  menu.listView.pressedItem))
                            return
                        menu.listView.pressedItem = currentItem
                        menu.highlightColor = CommonConstants.Transparent
                    }
                }

                onButtonClicked: doWorkMenuItem(row, col)

                function clickItem(row, col)  {
                    buttonPressed (menu.listView.model, row, col)
                    buttonReleased(menu.listView.model, row, col)
                    buttonClicked (menu.listView.model, row, col)
                }
            }
            Rectangle{ width: parent.width; height: 1; color: GlobalConstants.kBasicBackgoundColor}
        }
    }


    Csw {
            id: csw

            section: Csw.Section_Setting
            sequence: 0
            enable: (!CommonUtils.isNull(menu)) ? menu.visible : false

            onFocused: {
                menu.focused = true
                menu.highlightColor = GlobalConstants.kListFocusColor
                menu.listView.currentIndex = -1
                menu.listView.currentIndex = 0
            }
            onLostFocusd: {
                menu.highlightColor = CommonConstants.Transparent
                menu.focused = false
            }
            onLeftPressed: selected()
            onRightPressed: selected()
            onSpinCwPressed: {
                while(true) {
                    if (menu.listView.currentIndex < menu.listView.count - 1) {
                        ++menu.listView.currentIndex
                    } else {
                        menu.listView.currentIndex = 0
                    }
                    var item = menu.listView.currentItem
                    var label = CommonUtils.trim(item.label)
                    if (CommonUtils.isEmpty(label))
                        continue
                    break
                }
                menu.highlightColor = GlobalConstants.kListFocusColor
            }
            onSpinCcwPressed: {
                while(true) {
                    if (0 >= menu.listView.currentIndex) {
                        menu.listView.currentIndex = menu.listView.count - 1
                    } else if (0 < menu.listView.currentIndex) {
                        --menu.listView.currentIndex
                    } else {
                        menu.listView.currentIndex = 0
                    }
                    var item = menu.listView.currentItem
                    var label = CommonUtils.trim(item.label)
                    if (CommonUtils.isEmpty(label))
                        continue
                    break
                }
                menu.highlightColor = GlobalConstants.kListFocusColor
            }
            onSelected: {
                var index = menu.listView.currentIndex
                var row = parseInt(index / menu.gridCol)
                var col = index % menu.gridCol
                menu.clickItem(row, col)
            }
        }

}
