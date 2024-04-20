import QtQuick 2.1
import ViewRegistry 1.0
import CommonConstants 1.0
import ZoneConstants 1.0
import SettingConstants 1.0
import SearchConstants 1.0
import SearchResultViewEvent 1.0
import GadgetShortcutIconViewEvent 1.0

import "../../singleton"
import "../../component"
import "../../view"
import "../../view/util"
import "../../view/search"
import "../../GlobalConstants.js" as GlobalConstants
import "../../view/util/CommonUtils.js" as CommonUtils

PopupView {
    id: settingRoot

    property bool mouseAreaEnabled              : false
    property bool backgroundVisible             : false


    //onMediated에서 요청 및 응답
    property var __lang                         : null

    property bool needInit                      : true

    property var caller                         : null
    property var relay                          : null
    property var owner                          : null


    property int gridRow                        : 3
    property int gridCol                        : 2

    property size tileMenuImgSize               : Qt.size(194,148)

    property bool focused                       : false

    property int tileMenuSpaceWidth             : 0
    property int tileMenuContentButtonYOffset   : 0
    property int tileMenuContentTextYOffset     : 0
    property color tileMenuOutlineColor         : CommonConstants.Transparent
    property color tileMenuBackgroundColor      : CommonConstants.Transparent


    //ID
    property var menu                           : null

    //Event
    property var textEnum                       : ({})
    property var softKeyEnum                    : ({})
    property var categoryEnum                   : ({})
    property var menuEnum                       : ({})

    property var textArr                        : ([])
    property var softKeyMenuArr                 : ([])
    property var categoryMenuArr                : ([])
    property var menuNameArr                    : ([])
    property var menuArgArr                     : ([])
    property var menuObjArr                     : ([])
    property var menuImgNArr                    : ([])
    property var menuImgPArr                    : ([])
    property var menuImgFArr                    : ([])
    property var menuArr                        : ([])


    function allocateTextEnum(){
        if (CommonUtils.getObjectLen(textEnum) <= 0) {
            textEnum = CommonUtils.createEnum(textEnum)
        }
        var i = 0
        CommonUtils.insertEnum(textEnum , "Zone1Title"     , i++, [CommonConstants.Name] , [qsTr("Gadget shortcut icon")    ])
        CommonUtils.insertEnum(textEnum , "Slash"          , i++, [CommonConstants.Name] , ["/"                             ])

        CommonUtils.fetchArr(textEnum , CommonConstants.Name , textArr)
    }

    function allocateSoftKeyEnum(){
        if (CommonUtils.getObjectLen(softKeyEnum) <= 0) {
            softKeyEnum = CommonUtils.createEnum(softKeyEnum)
        }
        var i = 0
        CommonUtils.insertEnum(softKeyEnum ,"Back"         , i++, [CommonConstants.Name] , [ZoneConstants.Back   ])
        CommonUtils.insertEnum(softKeyEnum ,"Option"       , i++, [CommonConstants.Name] , [ZoneConstants.Option ])
        CommonUtils.insertEnum(softKeyEnum ,"Navi"         , i++, [CommonConstants.Name] , [ZoneConstants.Navi   ])
        CommonUtils.insertEnum(softKeyEnum ,"Menu"         , i++, [CommonConstants.Name] , [ZoneConstants.Menu   ])

        CommonUtils.fetchArr(softKeyEnum , CommonConstants.Name , softKeyMenuArr)
    }

    function allocateCategoryEnum(){
        if (CommonUtils.getObjectLen(categoryEnum) <= 0) {
            categoryEnum = CommonUtils.createEnum(categoryEnum)
        }
       CommonUtils.insertEnum(categoryEnum , "GasSt"             , SearchConstants.CategoryGasSt              , [CommonConstants.Name] , [qsTr("주유소")       ])
       CommonUtils.insertEnum(categoryEnum , "ChargingSt"        , SearchConstants.CategoryChargingSt         , [CommonConstants.Name] , [qsTr("충전소")       ])
       CommonUtils.insertEnum(categoryEnum , "ChargingEvSt"      , SearchConstants.CategoryChargingEvSt       , [CommonConstants.Name] , [qsTr("전기차 충전소")])
       CommonUtils.insertEnum(categoryEnum , "ParkingSp"         , SearchConstants.CategoryParkingSp          , [CommonConstants.Name] , [qsTr("주차장")       ])
       CommonUtils.insertEnum(categoryEnum , "Aqi"               , SearchConstants.CategoryEnd                , [CommonConstants.Name] , [qsTr("미세먼지")     ])

       CommonUtils.fetchArr(categoryEnum , CommonConstants.Name , categoryMenuArr)
    }

   function allocateMenuEnum(){
       if (CommonUtils.getObjectLen(menuEnum) <= 0) {
            menuEnum = CommonUtils.createEnum(menuEnum)
       }

       var args = []
       var i = 0

        args[i] = ({})
        args[i][CommonConstants.Kind      ] = categoryEnum.GasSt
        CommonUtils.insertEnum(menuEnum, "GasSt", i
                                       , [  CommonConstants.Name
                                          , CommonConstants.Url
                                          , CommonConstants.Arg
                                          , CommonConstants.Object
                                          , CommonConstants.Normal
                                          , CommonConstants.Press
                                          , CommonConstants.Focus]
                                       , [  categoryMenuArr[categoryEnum.GasSt]
                                          , CommonConstants.url(ViewRegistry.SearchResultView)
                                          , args[i]
                                          , null
                                          , "setting/gad_fuel.png"
                                          , "setting/gad_fuel.png"
                                          , null])
        args[++i] = ({})
        args[i][CommonConstants.Kind      ] = categoryEnum.ChargingSt
        CommonUtils.insertEnum(menuEnum, "ChargingSt", i
                                       , [  CommonConstants.Name
                                          , CommonConstants.Url
                                          , CommonConstants.Arg
                                          , CommonConstants.Object
                                          , CommonConstants.Normal
                                          , CommonConstants.Press
                                          , CommonConstants.Focus]
                                       , [  categoryMenuArr[categoryEnum.ChargingSt]
                                          , CommonConstants.url(ViewRegistry.SearchResultView)
                                          , args[i]
                                          , null
                                          , "setting/gad_lpg.png"
                                          , "setting/gad_lpg.png"
                                          , null])
       args[++i] = ({})
       args[i][CommonConstants.Kind      ] = categoryEnum.Bank
       CommonUtils.insertEnum(menuEnum, "ChargingEvSt", i
                                      , [  CommonConstants.Name
                                         , CommonConstants.Url
                                         , CommonConstants.Arg
                                         , CommonConstants.Object
                                         , CommonConstants.Normal
                                         , CommonConstants.Press
                                         , CommonConstants.Focus]
                                      , [  categoryMenuArr[categoryEnum.ChargingEvSt]
                                         , CommonConstants.url(ViewRegistry.SearchResultView)
                                         , args[i]
                                         , null
                                         , "setting/gad_ev.png"
                                         , "setting/gad_ev.png"
                                         , null])

        args[++i] = ({})
        args[i][CommonConstants.Kind      ] = categoryEnum.ParkingSp
        CommonUtils.insertEnum(menuEnum, "ParkingSp", i
                                       , [  CommonConstants.Name
                                          , CommonConstants.Url
                                          , CommonConstants.Arg
                                          , CommonConstants.Object
                                          , CommonConstants.Normal
                                          , CommonConstants.Press
                                          , CommonConstants.Focus]
                                       , [  categoryMenuArr[categoryEnum.ParkingSp]
                                          , CommonConstants.url(ViewRegistry.SearchResultView)
                                          , args[i]
                                          , null
                                          , "setting/gad_parking.png"
                                          , "setting/gad_parking.png"
                                          , null])
        args[++i] = ({})
        args[i][CommonConstants.Kind      ] = categoryEnum.Aqi
        CommonUtils.insertEnum(menuEnum, "Aqi", i
                                       , [  CommonConstants.Name
                                          , CommonConstants.Url
                                          , CommonConstants.Arg
                                          , CommonConstants.Object
                                          , CommonConstants.Normal
                                          , CommonConstants.Press
                                          , CommonConstants.Focus]
                                       , [  categoryMenuArr[categoryEnum.Aqi]
                                          , CommonConstants.url(ViewRegistry.SettingAQIdetailView)
                                          , args[i]
                                          , null
                                          , "setting/gad_aqi.png"
                                          , "setting/gad_aqi.png"
                                          , null])


        CommonUtils.fetchArr(menuEnum , CommonConstants.Name , menuNameArr)
        CommonUtils.fetchArr(menuEnum , CommonConstants.Arg , menuArgArr)
        CommonUtils.fetchArr(menuEnum , CommonConstants.Object , menuObjArr)
        CommonUtils.fetchArr(menuEnum , CommonConstants.Normal , menuImgNArr)
        CommonUtils.fetchArr(menuEnum , CommonConstants.Press , menuImgPArr)
        CommonUtils.fetchArr(menuEnum , CommonConstants.Focus , menuImgFArr)

       if (!CommonUtils.isNull(gridCol)) {
          gridRow = CommonUtils.getGridRowCol(menuNameArr, gridCol)
       } else if (!CommonUtils.isNull(gridRow)) {
          gridCol = CommonUtils.getGridRowCol(menuNameArr, gridRow)
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
               menuArr[i][j][menuAttrEnum.Name ] = menuNameArr[k]
               menuArr[i][j][menuAttrEnum.Key  ] = menuNameArr[k]
               menuArr[i][j][menuAttrEnum.Arg  ] = menuArgArr[k]
               menuArr[i][j][menuAttrEnum.Obj  ] = menuObjArr[k]
               menuArr[i][j][menuAttrEnum.ImgN ] = menuImgNArr[k]
               menuArr[i][j][menuAttrEnum.ImgP ] = menuImgPArr[k]
               menuArr[i][j][menuAttrEnum.ImgF ] = menuImgFArr[k]
            }
       }
       menu.menuArr = menuArr
   }

    function enableMouseArea(visible) {
        settingRoot.background.visible = backgroundVisible = visible
        settingRoot.mouseArea.enabled  = mouseAreaEnabled = visible
    }

    function doWorkMenuItem(row,col) {
        var index = (row * gridCol) + (col)
        var name = menuNameArr[index]
        if (name.length <= 0)
            return
        var data = ({})
        switch(name) {
        case categoryMenuArr[categoryEnum.GasSt]:
            data[SearchConstants.Kind] = categoryEnum.GasSt
            sendEvent(GadgetShortcutIconViewEvent.TouchGadgetShotcutIcon, data)
            break
        case categoryMenuArr[categoryEnum.ChargingSt]:
            data[SearchConstants.Kind] = categoryEnum.ChargingSt
            sendEvent(GadgetShortcutIconViewEvent.TouchGadgetShotcutIcon, data)
            break
        case categoryMenuArr[categoryEnum.ChargingEvSt]:
            data[SearchConstants.Kind] = categoryEnum.ChargingEvSt
            sendEvent(GadgetShortcutIconViewEvent.TouchGadgetShotcutIcon, data)
            break
        case categoryMenuArr[categoryEnum.ParkingSp]:
            data[SearchConstants.Kind] = categoryEnum.ParkingSp
            sendEvent(GadgetShortcutIconViewEvent.TouchGadgetShotcutIcon, data)
            break
        case categoryMenuArr[categoryEnum.Aqi]:
            data[SearchConstants.Kind] = categoryEnum.Aqi
            sendEvent(GadgetShortcutIconViewEvent.TouchGadgetShotcutIcon, data)
            break
        }
    }

     function init(){
         if (!needInit)
             return
         needInit = false

         allocateTextEnum()
         allocateSoftKeyEnum()
         allocateCategoryEnum()
         allocateMenuEnum()

         tileMenuSpaceWidth = 50
         tileMenuContentButtonYOffset = settingRoot.layout.tileMenuExContentButtonYOffset
         tileMenuContentTextYOffset = settingRoot.layout.tileMenuExContentTextYOffset
         tileMenuBackgroundColor = GlobalConstants.kBasicBackgoundColor
         enableMouseArea(true)

         loader.sourceComponent = undefined
         loader.sourceComponent = main

         if (active) {
             sendEvent(GadgetShortcutIconViewEvent.SelectZone1Title, textArr[textEnum.Zone1Title])
         }
     }

    onMediated: {
        sendEvent(GadgetShortcutIconViewEvent.RequestLang)
        enableMouseArea(false)
        sendEvent(GadgetShortcutIconViewEvent.Mediated )
        init()
    }

    onActivated: {
        init()
        enabled = true

        sendEvent(GadgetShortcutIconViewEvent.ResetCsw)
        sendEvent(GadgetShortcutIconViewEvent.SelectZone1Title, textArr[textEnum.Zone1Title])
        sendEvent(GadgetShortcutIconViewEvent.Activated,[CommonConstants.NoOption])
    }
    onDeactivated: {
        enabled = false
        sendEvent(GadgetShortcutIconViewEvent.Deactivated)
    }

    Component.onDestruction: {
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
          case softKeyMenuArr[softKeyEnum.Back]:
            data[CommonConstants.ObjectName] = CommonConstants.objectName(ViewRegistry.GadgetShortcutIconView)
            sendEvent(GadgetShortcutIconViewEvent.CloseView, data)
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
            Row {width: parent.width;height: parent.height
                Rectangle {width:tileMenuSpaceWidth;height:parent.height;color:tileMenuBackgroundColor
                    onWidthChanged: CommonUtils.applyRemainWidth(parent.children[1],parent)
                }
                TileMenu {
                    Component.onCompleted: { settingRoot.menu = this
                        CommonUtils.applyRemainWidth(this,parent)
                        allocateMenuArr(menu)
                    }
                    width: parent.width
                    height: parent.height
                    PressBar { currentItem: menu.listView.pressedItem;opacity: 0.3}
                    highlight: HighlightBar {
                        currentItem: menu.listView.currentItem
                        color: menu.highlightColor
                    }
                    backgroundColor: tileMenuBackgroundColor
                    onButtonCompleted: {
                        var item = menuArr[row][col][menuAttrEnum.Obj]
                        if (!CommonUtils.isNull(item)) {
                            item.img.width = tileMenuImgSize.width
                            item.img.height = tileMenuImgSize.height
                            item.img.y = tileMenuContentButtonYOffset

                            item.txt.clip        = true
                            item.txt.elide       = Text.ElideRight
                            item.txt.font.family =  GlobalConstants.kFontFamily
                            item.txt.font.pixelSize = Resource.pixelToDp(GlobalConstants.kOneLineListTextFontSize)
                            item.txt.y =  tileMenuContentTextYOffset
                            item.outlineColor = tileMenuOutlineColor
                        }
                    }

                    onButtonFocused: {
                        var item = menuArr[row][col][menuAttrEnum.Obj]
                         if (!CommonUtils.isNull(item)) {
                             item.txt.colorN = GlobalConstants.kBasicBackgoundColor
                             var imgP = menuArr[row][col][menuAttrEnum.ImgP]
                             item.img.sourceN  = Resource.imageUrl(imgP)
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
                                CommonUtils.isNull(parent.currCol) ||
                                CommonUtils.isUndefined(typeof menuArr[parent.currRow]) ||
                                CommonUtils.isUndefined(typeof menuArr[parent.currRow][parent.currCol]) ||
                                CommonUtils.isUndefined(typeof menuArr[parent.currRow][parent.currCol][parent.menuAttrEnum.Obj])) {
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

                    function clickItem(row, col) {
                        buttonPressed (menu.listView.model, row, col)
                        buttonReleased(menu.listView.model, row, col)
                        buttonClicked (menu.listView.model, row, col)
                    }
                }
                Rectangle {width:tileMenuSpaceWidth;height:parent.height;color:tileMenuBackgroundColor}
            }
            Rectangle{ width: parent.width; height: 1; color: GlobalConstants.kBasicBackgoundColor}
        }
    }
}
