import QtQuick 2.1
import ViewRegistry 1.0
import CommonConstants 1.0
import ZoneConstants 1.0
import SettingConstants 1.0
import SearchConstants 1.0
import SearchAroundViewEvent 1.0
import SceneGraphRendering 1.0
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

    property bool mouseAreaEnabled                  : false
    property bool backgroundVisible                 : false

    property bool reFocus                           : false
    property bool isShowTopView                     : false


    //onMediated에서 요청 및 응답
    property var __lang                             : null

    property bool needInit                          : true


    property var caller                             : null
    property var relay                              : null
    property var owner                              : null

    property bool isMediated                        : false


    property bool electricVehicle                   : false

    //.RequestAddressType Event요청할 때 결과 값
    property string addressType                     : SearchConstants.None


    property var gridRow                            : null
    property var gridCol                            : null

    property size tileMenuImgSize                   : Qt.size(71,71)

    property bool focused                           : false

    property int frameHeight                        : {
                                                        if (CommonUtils.isIndexOf(caller, [CommonConstants.objectName(ViewRegistry.BookmarkPageView)]) ||
                                                            CommonUtils.isIndexOf(owner , [CommonConstants.objectName(ViewRegistry.BookmarkPageView)]) ||
                                                            CommonUtils.isIndexOf(caller, [CommonConstants.objectName(ViewRegistry.RouteSettingView)]) ||
                                                            CommonUtils.isIndexOf(owner , [CommonConstants.objectName(ViewRegistry.RouteSettingView)])) {
                                                            return height
                                                        }  else  {
                                                            return 337
                                                        }
                                                      }
    property int tileMenuSpaceWidth                 : 0
    property int tileMenuContentButtonYOffset       : 0
    property int tileMenuContentTextYOffset         : 0
    property color tileMenuOutlineColor             : CommonConstants.Transparent
    property color tileMenuBackgroundColor          : CommonConstants.Transparent

    //ID
    property var menu                               : null

    //Event
    property var textEnum                           : ({})
    property var softKeyEnum                        : ({})
    property var categoryEnum                       : ({})
    property var menuEnum                           : ({})

    property var textArr                            : ([])
    property var softKeyMenuArr                     : ([])
    property var categoryMenuArr                    : ([])
    property var menuNameArr                        : ([])
    property var menuArgArr                         : ([])
    property var menuObjArr                         : ([])
    property var menuImgNArr                        : ([])
    property var menuImgPArr                        : ([])
    property var menuImgFArr                        : ([])
    property var menuArr                            : ([])


    function allocateTextEnum(){
        if (CommonUtils.getObjectLen(textEnum) <= 0) {
            textEnum = CommonUtils.createEnum(textEnum)
        }
        var i = 0
        CommonUtils.insertEnum(textEnum , "Zone1Title"     , i++, [CommonConstants.Name] , [qsTr("주변 검색")     ])
        CommonUtils.insertEnum(textEnum , "Slash"          , i++, [CommonConstants.Name] , ["/"                   ])

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
       CommonUtils.insertEnum(categoryEnum , "ParkingSp"         , SearchConstants.CategoryParkingSp          , [CommonConstants.Name] , [qsTr("주차장")       ])
       CommonUtils.insertEnum(categoryEnum , "Bank"              , SearchConstants.CategoryBank               , [CommonConstants.Name] , [qsTr("은행")         ])
       CommonUtils.insertEnum(categoryEnum , "Shopping"          , SearchConstants.CategoryShopping           , [CommonConstants.Name] , [qsTr("쇼핑")         ])
       CommonUtils.insertEnum(categoryEnum , "Restaurant"        , SearchConstants.CategoryRestaurant         , [CommonConstants.Name] , [qsTr("음식점")       ])
       CommonUtils.insertEnum(categoryEnum , "Tourism"           , SearchConstants.CategoryTourism            , [CommonConstants.Name] , [qsTr("관광")         ])
       CommonUtils.insertEnum(categoryEnum , "Medical"           , SearchConstants.CategoryMedical            , [CommonConstants.Name] , [qsTr("병원")         ])
       CommonUtils.insertEnum(categoryEnum , "PublicInstitution" , SearchConstants.CategoryPublicInstitution  , [CommonConstants.Name] , [qsTr("공공기관")     ])
       CommonUtils.insertEnum(categoryEnum , "Finance"           , SearchConstants.CategoryFinance            , [CommonConstants.Name] , [qsTr("금융")         ])
       CommonUtils.insertEnum(categoryEnum , "ChargingEvSt"      , SearchConstants.CategoryChargingEvSt       , [CommonConstants.Name] , [qsTr("전기차 충전소")])

       CommonUtils.fetchArr(categoryEnum , CommonConstants.Name , categoryMenuArr)
    }

   function allocateMenuEnum(){
       if (CommonUtils.getObjectLen(menuEnum) <= 0) {
            menuEnum = CommonUtils.createEnum(menuEnum)
       }

       var args = []
       var i = 0

       if (electricVehicle) {
           args[i] = ({})
           args[i][CommonConstants.Kind      ] = categoryEnum.ChargingEvSt
           CommonUtils.insertEnum(menuEnum, "ChargingEvSt", i
                                          , [  CommonConstants.Name
                                             , CommonConstants.Arg
                                             , CommonConstants.Object
                                             , CommonConstants.Normal
                                             , CommonConstants.Press
                                             , CommonConstants.Focus]
                                          , [  categoryMenuArr[categoryEnum.ChargingEvSt]
                                             , args[i]
                                             , null
                                             , "aroundmenu/facility_ic_9.png"
                                             , "aroundmenu/facility_ic_9.png"
                                             , "aroundmenu/facility_ic_9_f.png"])
       }else {
            args[i] = ({})
            args[i][CommonConstants.Kind      ] = categoryEnum.GasSt
            CommonUtils.insertEnum(menuEnum, "GasSt", i
                                           , [  CommonConstants.Name
                                              , CommonConstants.Arg
                                              , CommonConstants.Object
                                              , CommonConstants.Normal
                                              , CommonConstants.Press
                                              , CommonConstants.Focus]
                                           , [  categoryMenuArr[categoryEnum.GasSt]
                                              , args[i]
                                              , null
                                              , "aroundmenu/facility_ic_1.png"
                                              , "aroundmenu/facility_ic_1.png"
                                              , "aroundmenu/facility_ic_1_f.png"])
            args[++i] = ({})
            args[i][CommonConstants.Kind      ] = categoryEnum.ChargingSt
            CommonUtils.insertEnum(menuEnum, "ChargingSt", i
                                           , [  CommonConstants.Name
                                              , CommonConstants.Arg
                                              , CommonConstants.Object
                                              , CommonConstants.Normal
                                              , CommonConstants.Press
                                              , CommonConstants.Focus]
                                           , [  categoryMenuArr[categoryEnum.ChargingSt]
                                              , args[i]
                                              , null
                                              , "aroundmenu/facility_ic_2.png"
                                              , "aroundmenu/facility_ic_2.png"
                                              , "aroundmenu/facility_ic_2_f.png"])
        }
        args[++i] = ({})
        args[i][CommonConstants.Kind      ] = categoryEnum.ParkingSp
        CommonUtils.insertEnum(menuEnum, "ParkingSp", i
                                       , [  CommonConstants.Name
                                          , CommonConstants.Arg
                                          , CommonConstants.Object
                                          , CommonConstants.Normal
                                          , CommonConstants.Press
                                          , CommonConstants.Focus]
                                       , [  categoryMenuArr[categoryEnum.ParkingSp]
                                          , args[i]
                                          , null
                                          , "aroundmenu/facility_ic_3.png"
                                          , "aroundmenu/facility_ic_3.png"
                                          , "aroundmenu/facility_ic_3_f.png"])
        args[++i] = ({})
        args[i][CommonConstants.Kind      ] = categoryEnum.Bank
        CommonUtils.insertEnum(menuEnum, "Bank", i
                                       , [  CommonConstants.Name
                                          , CommonConstants.Arg
                                          , CommonConstants.Object
                                          , CommonConstants.Normal
                                          , CommonConstants.Press
                                          , CommonConstants.Focus]
                                       , [  categoryMenuArr[categoryEnum.Bank]
                                          , args[i]
                                          , null
                                          , "aroundmenu/facility_ic_4.png"
                                          , "aroundmenu/facility_ic_4.png"
                                          , "aroundmenu/facility_ic_4_f.png"])
        args[++i] = ({})
        args[i][CommonConstants.Kind      ] = categoryEnum.Shopping
        CommonUtils.insertEnum(menuEnum, "Shopping", i
                                       , [  CommonConstants.Name
                                          , CommonConstants.Arg
                                          , CommonConstants.Object
                                          , CommonConstants.Normal
                                          , CommonConstants.Press
                                          , CommonConstants.Focus]
                                       , [  categoryMenuArr[categoryEnum.Shopping]
                                          , args[i]
                                          , null
                                          , "aroundmenu/facility_ic_5.png"
                                          , "aroundmenu/facility_ic_5.png"
                                          , "aroundmenu/facility_ic_5_f.png"])
        args[++i] = ({})
        args[i][CommonConstants.Kind      ] = categoryEnum.Restaurant
        CommonUtils.insertEnum(menuEnum, "Restaurant", i
                                       , [  CommonConstants.Name
                                          , CommonConstants.Arg
                                          , CommonConstants.Object
                                          , CommonConstants.Normal
                                          , CommonConstants.Press
                                          , CommonConstants.Focus]
                                       , [  categoryMenuArr[categoryEnum.Restaurant]
                                          , args[i]
                                          , null
                                          , "aroundmenu/facility_ic_6.png"
                                          , "aroundmenu/facility_ic_6.png"
                                          , "aroundmenu/facility_ic_6_f.png"])
        args[++i] = ({})
        args[i][CommonConstants.Kind      ] = categoryEnum.Tourism
        CommonUtils.insertEnum(menuEnum, "Tourism", i
                                       , [  CommonConstants.Name
                                          , CommonConstants.Arg
                                          , CommonConstants.Object
                                          , CommonConstants.Normal
                                          , CommonConstants.Press
                                          , CommonConstants.Focus]
                                       , [  categoryMenuArr[categoryEnum.Tourism]
                                          , args[i]
                                          , null
                                          , "aroundmenu/facility_ic_7.png"
                                          , "aroundmenu/facility_ic_7.png"
                                          , "aroundmenu/facility_ic_7_f.png"])
        args[++i] = ({})
        args[i][CommonConstants.Kind      ] = categoryEnum.Medical
        CommonUtils.insertEnum(menuEnum, "Medical", i
                                       , [  CommonConstants.Name
                                          , CommonConstants.Arg
                                          , CommonConstants.Object
                                          , CommonConstants.Normal
                                          , CommonConstants.Press
                                          , CommonConstants.Focus]
                                       , [  categoryMenuArr[categoryEnum.Medical]
                                          , args[i]
                                          , null
                                          , "aroundmenu/facility_ic_8.png"
                                          , "aroundmenu/facility_ic_8.png"
                                          , "aroundmenu/facility_ic_8_f.png"])

        CommonUtils.fetchArr(menuEnum , CommonConstants.Name , menuNameArr)
        CommonUtils.fetchArr(menuEnum , CommonConstants.Arg , menuArgArr)
        CommonUtils.fetchArr(menuEnum , CommonConstants.Object , menuObjArr)
        CommonUtils.fetchArr(menuEnum , CommonConstants.Normal , menuImgNArr)
        CommonUtils.fetchArr(menuEnum , CommonConstants.Press , menuImgPArr)
        CommonUtils.fetchArr(menuEnum , CommonConstants.Focus , menuImgFArr)
    }


   function allocateMenuArr(menu){
       var menuAttrEnum = menu.menuAttrEnum
       menu.gridCol = searchRoot.gridCol
       menu.gridRow = searchRoot.gridRow
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

    function viewSizeAndModeChanged(height){
        if (CommonUtils.isEqual(searchRoot.height, height))
            return
        var data =({})
        data[CommonConstants.B] = height
        sendEvent(SearchAroundViewEvent.ViewSizeChanged, data)

        requestDirectionGuid()
    }

    function enableMouseArea(visible) {
        searchRoot.background.visible = backgroundVisible = visible
        searchRoot.mouseArea.enabled  = mouseAreaEnabled = visible
    }


    //유종을 요청
    function requestSetting() {
        var data = ({})
        data[CommonConstants.Section] = SettingConstants.MapSubOilType
        data[CommonConstants.Slot] = "__resultSettingOilType"
        sendEvent(SearchAroundViewEvent.RequestSetting, data )
    }

    //맵모드의 설정값 요청: __resultSettingDirectionGuid함수로 응답
    function requestDirectionGuid() {
        var data = ({})
        data[CommonConstants.Section] = SettingConstants.DirectionGuid
        data[CommonConstants.Slot] = "__resultSettingDirectionGuid"
        sendEvent(SearchAroundViewEvent.RequestSetting, data )
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


    onReFocusChanged: {
        if (!reFocus)
            return
        reFocus = false
        if (isMediated) {
            //viewActivated()
        }
    }

    function __resultSettingOilType(settings) {
        if (CommonUtils.isNull(settings))
            return
        var key = SettingConstants.OilKind
        if (!settings.hasOwnProperty(key))
            return
        var value = settings[key][CommonConstants.Value]
        var index = CommonUtils.number(value)
        electricVehicle = CommonUtils.isEqual(index , SettingConstants.OilTypeElec) ? true : false

        needInit = true
        init()
    }

    function doWorkMenuItem(row,col) {
        var data = ({})
        var k = (row * gridCol) + (col)
        if(!menuNameArr[k].length)
            return
        data [CommonConstants.Url         ] = CommonConstants.url(ViewRegistry.SearchResultView)
        data [SearchConstants.Query       ] = menuNameArr[k]
        data [SearchConstants.TextParam   ] = CommonConstants.Empty
        data [SearchConstants.SearchType  ] = CommonConstants.Around
        //기타
        data [SearchConstants.Favorite    ] = CommonUtils.isIndexOf(owner, [CommonConstants.objectName(ViewRegistry.BookmarkPageView)])
        data [SearchConstants.IsHome      ] = CommonUtils.isIndexOf(owner, [textArr[textEnum.Slash] + SearchConstants.Home])
        data [SearchConstants.IsOffice    ] = CommonUtils.isIndexOf(owner, [textArr[textEnum.Slash] + SearchConstants.Office])
        data [SearchConstants.ViaSetting  ] = CommonUtils.isIndexOf(caller, [CommonConstants.objectName(ViewRegistry.RouteSettingView)])
        var args = menuArgArr[k]
        for (var key in args) {
            data[key] =  args[key]
        }
        data[CommonConstants.Caller       ] = caller
        data[CommonConstants.Owner        ] = CommonConstants.objectName(ViewRegistry.SearchAroundView)
        data[CommonConstants.Visible      ] = SearchConstants.DialogNoResult

        //data[CommonConstants.Z            ] = CommonConstants.MAX_INT_VALUE
        data[CommonConstants.ReFocus      ] = true
        data [CommonConstants.ObjectName  ] = CommonConstants.objectName(ViewRegistry.SearchResultView)
        sendEvent(SearchAroundViewEvent.UpdateView, data )

        sendEvent(SearchAroundViewEvent.CreateView, data )
    }

     function init(){
         if (isMediated) {
             sendEvent(SearchAroundViewEvent.RequestViewList)
//             if (isShowTopView) {
//                 return
//             }
         }
         if (!needInit)
             return
         needInit = false

         allocateTextEnum()
         allocateSoftKeyEnum()
         allocateCategoryEnum()
         allocateMenuEnum()

         if (CommonUtils.isIndexOf(caller, [CommonConstants.objectName(ViewRegistry.BookmarkPageView)]) ||
             CommonUtils.isIndexOf(owner , [CommonConstants.objectName(ViewRegistry.BookmarkPageView)])) {
             tileMenuSpaceWidth = 50; gridCol = 3
             tileMenuContentButtonYOffset = searchRoot.layout.tileMenuExContentButtonYOffset
             tileMenuContentTextYOffset = searchRoot.layout.tileMenuExContentTextYOffset
             tileMenuBackgroundColor = GlobalConstants.kBasicBackgoundColor
             enableMouseArea(true)
         } else if (CommonUtils.isIndexOf(caller, [CommonConstants.objectName(ViewRegistry.RouteSettingView)]) ||
                    CommonUtils.isIndexOf(owner , [CommonConstants.objectName(ViewRegistry.RouteSettingView)])) {
             tileMenuSpaceWidth = 50; gridCol = 3
             tileMenuContentButtonYOffset = searchRoot.layout.tileMenuExContentButtonYOffset
             tileMenuContentTextYOffset = searchRoot.layout.tileMenuExContentTextYOffset
             tileMenuBackgroundColor = GlobalConstants.kBasicBackgoundColor
             enableMouseArea(true)
         }  else  {
             gridCol = 4
             tileMenuContentButtonYOffset = searchRoot.layout.tileMenuContentButtonYOffset
             tileMenuContentTextYOffset = searchRoot.layout.tileMenuContentTextYOffset
             tileMenuOutlineColor = GlobalConstants.kBasicBackgoundColor
             tileMenuBackgroundColor = GlobalConstants.kNormalColor
             enableMouseArea(false)
         }
         if (!CommonUtils.isNull(gridCol)) {
            gridRow = CommonUtils.getGridRowCol(menuNameArr, gridCol)
         } else if (!CommonUtils.isNull(gridRow)) {
            gridCol = CommonUtils.getGridRowCol(menuNameArr, gridRow)
         }
        loader.sourceComponent = undefined
        loader.sourceComponent = main
        if (active) {
             sendEvent(SearchAroundViewEvent.SelectZone1Title, textArr[textEnum.Zone1Title])
        }
     }

    onMediated: {
        sendEvent(SearchAroundViewEvent.RequestLang)
        enableMouseArea(false)
        sendEvent(SearchAroundViewEvent.Mediated )
        requestSetting()
        init()
        isMediated = true
    }

    onActivated: {
        init()
//        if (isShowTopView) {
//            return
//        }
        sendEvent(SearchAroundViewEvent.ResetCsw)
        sendEvent(SearchAroundViewEvent.RegistCsw, [ csw ])
        sendEvent(SearchAroundViewEvent.SelectZone1Title, textArr[textEnum.Zone1Title])
        sendEvent(SearchAroundViewEvent.Activated,[CommonConstants.NoOption])
        viewSizeAndModeChanged(frameHeight)
    }
    onDeactivated: {
        sendEvent(SearchAroundViewEvent.Deactivated)
    }

    Component.onDestruction: {
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


    //언어가 변경되었을 때
    function __retranslateUi(lang) {
        searchRoot.__lang = lang
        needInit = true
        if (active) {
            init()
        }
    }

    function __doSoftKeyFunction(button, menuName, menuIndex) {
        var data = ({})
        switch(button) {
          case softKeyMenuArr[softKeyEnum.Back]:
            if (CommonUtils.isIndexOf(caller, [CommonConstants.objectName(ViewRegistry.BookmarkPageView)])) {
              sendEvent(SearchAroundViewEvent.ReturnToFavorite)
            } else if (CommonUtils.isIndexOf(caller, [CommonConstants.objectName(ViewRegistry.RouteSettingView)])) {
              sendEvent(SearchAroundViewEvent.ReturnToViaSetting)
            } else if (CommonUtils.isIndexOf(caller, [CommonConstants.objectName(ViewRegistry.NaviMenuView)])) {
              sendEvent(SearchAroundViewEvent.ReturnToNaviMenu)
            }
            data[CommonConstants.ObjectName] = CommonConstants.objectName(ViewRegistry.SearchAroundView)
            sendEvent(SearchAroundViewEvent.CloseView, data)
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
            Item {
               width: parent.width
               height: parent.height - frameHeight
            }
            Row {width: parent.width;height:frameHeight -1
                Rectangle {width:tileMenuSpaceWidth;height:parent.height;color:tileMenuBackgroundColor
                    onWidthChanged: CommonUtils.applyRemainWidth(parent.children[1],parent)
                }
                TileMenu {
                    Component.onCompleted: { searchRoot.menu = this
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

                            item.txt.elide          = Text.ElideRight
                            item.txt.font.family    = GlobalConstants.kFontFamily
                            item.txt.font.pixelSize = Resource.pixelToDp(GlobalConstants.kOneLineListTextFontSize)
                            item.txt.y              = tileMenuContentTextYOffset
                            item.txt.lineHeightMode = Text.FixedHeight
                            item.txt.lineHeight     = searchRoot.layout.tileMenuContentTextLineHeight
                            item.outlineColor       = tileMenuOutlineColor
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
                            if (CommonUtils.isEqual(currentItem,  menu.listView.pressedItem)) {
                                return
                            }
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


    Csw {
            id: csw
            section: Csw.Section_GridMenu
            sequence: 0
            enable: searchRoot.visible

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
                var item = menu.listView.currentItem
                var index = menu.listView.currentIndex
                var row = parseInt(index / menu.gridCol)
                var col = index % menu.gridCol
                menu.clickItem(row, col)
            }
    }
}
