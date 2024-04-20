import QtQuick 2.0
import CommonConstants 1.0
import ZoneConstants 1.0
import SearchConstants 1.0
import SettingConstants 1.0
import ViewRegistry 1.0
import NaviMenuViewEvent 1.0
import SceneGraphRendering 1.0
import Csw 1.0

import "../../singleton"
import "../../component"
import "../../view"
import "../../view/util"
import "../../view/setting"
import "../../view/search"
import "../../GlobalConstants.js" as GlobalConstants
import "../../view/util/CommonUtils.js" as CommonUtils

PopupView {
    id: naviMenuRoot

    background.visible              : false
    mouseArea.enabled               : false
    fader.fadingOpacity             : 0

    readonly property int frameHeight: 337
    property bool sync              : true
    property bool isShowTopView     : false

    //언어 관련
    property var __lang             : null
    property bool needInit          : true


    property var result             : null
    property var caller             : null
    property bool isMediated        : false

    property int gridRow            : 2
    property var gridCol            : null
    property size tileMenuImgSize   : Qt.size(71,71)

    property var dialogMessage      : null

    //ID
    property var menu               : null

    //Enum
    property var textEnum           : ({})
    property var menuEnum           : ({})
    //------------------------
    property var textArr            : ([])
    property var menuName           : ([])
    property var menuUrl            : ([])
    property var menuArg            : ([])
    property var menuObj            : ([])
    property var menuImgN           : ([])
    property var menuImgP           : ([])
    property var menuImgF           : ([])
    property var menuArr            : ([])

    function allocateTextEnum(){
        if (CommonUtils.getObjectLen(textEnum) <= 0) {
            textEnum = CommonUtils.createEnum(textEnum)
        }
        var i = 0
        CommonUtils.insertEnum(textEnum, "Zone1Title",      i++, [CommonConstants.Name], [qsTr("내비메뉴")])

        CommonUtils.fetchArr(textEnum, CommonConstants.Name, textArr)
    }

    function allocateMenuEnum(){
        if (CommonUtils.getObjectLen(menuEnum) <= 0) {
            menuEnum = CommonUtils.createEnum(menuEnum)
        }
        var args = []
        var i = 0
        args[i] = ({})
        CommonUtils.insertEnum(menuEnum, "Home",     i,     [CommonConstants.Name,
                                                              CommonConstants.Url,
                                                              CommonConstants.Arg,
                                                              CommonConstants.Object,
                                                              CommonConstants.Normal,
                                                              CommonConstants.Press,
                                                              CommonConstants.Focus]    ,
                                                            [qsTr("집으로"),
                                                             CommonConstants.objectName(ViewRegistry.BookmarkPageView) + "/" + SearchConstants.Home,
                                                             args[i],
                                                             null,
                                                             "navimenu/navimenu_ic_1.png",
                                                             "navimenu/navimenu_ic_1.png",
                                                             "navimenu/navimenu_ic_1_f.png"])
        args[++i] = ({})
        CommonUtils.insertEnum(menuEnum, "Office",    i,     [CommonConstants.Name,
                                                              CommonConstants.Url,
                                                              CommonConstants.Arg,
                                                              CommonConstants.Object,
                                                              CommonConstants.Normal,
                                                              CommonConstants.Press,
                                                              CommonConstants.Focus]    ,
                                                             [qsTr("회사로"),
                                                              CommonConstants.objectName(ViewRegistry.BookmarkPageView) + "/" + SearchConstants.Office,
                                                              args[i],
                                                              null,
                                                              "navimenu/navimenu_ic_2.png",
                                                              "navimenu/navimenu_ic_2.png",
                                                              "navimenu/navimenu_ic_2_f.png"])
        args[++i] = ({})
        args[i][SearchConstants.Page ] = SearchConstants.RecentDestPageIndex
        CommonUtils.insertEnum(menuEnum, "RecentDest",  i,   [CommonConstants.Name,
                                                              CommonConstants.Url,
                                                              CommonConstants.Arg,
                                                              CommonConstants.Object,
                                                              CommonConstants.Normal,
                                                              CommonConstants.Press,
                                                              CommonConstants.Focus],
                                                             [qsTr("최근 목적지"),
                                                              CommonConstants.url(ViewRegistry.SearchTabView),
                                                              args[i],
                                                              null,
                                                              "navimenu/navimenu_ic_3.png",
                                                              "navimenu/navimenu_ic_3.png",
                                                              "navimenu/navimenu_ic_3_f.png"])
        args[++i] = ({})
        args[i][SearchConstants.Page ] = SearchConstants.FavoritePageIndex
        CommonUtils.insertEnum(menuEnum, "Bookmark",    i,   [CommonConstants.Name,
                                                              CommonConstants.Url,
                                                              CommonConstants.Arg,
                                                              CommonConstants.Object,
                                                              CommonConstants.Normal,
                                                              CommonConstants.Press,
                                                              CommonConstants.Focus],
                                                             [qsTr("즐겨찾기"),
                                                              CommonConstants.url(ViewRegistry.SearchTabView),
                                                              args[i],
                                                              null,
                                                              "navimenu/navimenu_ic_4.png",
                                                              "navimenu/navimenu_ic_4.png",
                                                              "navimenu/navimenu_ic_4_f.png"])
        args[++i] = ({})
        CommonUtils.insertEnum(menuEnum, "AroundSearch", i,[CommonConstants.Name,
                                                            CommonConstants.Url,
                                                            CommonConstants.Arg,
                                                            CommonConstants.Object,
                                                            CommonConstants.Normal,
                                                            CommonConstants.Press,
                                                            CommonConstants.Focus],
                                                           [qsTr("주변 검색"),
                                                            CommonConstants.url(ViewRegistry.SearchAroundView),
                                                            args[i],
                                                            null,
                                                            "navimenu/navimenu_ic_5.png",
                                                            "navimenu/navimenu_ic_5.png",
                                                            "navimenu/navimenu_ic_5_f.png"])
        args[++i] = ({})
        CommonUtils.insertEnum(menuEnum, "IndustrySearch", i,[CommonConstants.Name,
                                                              CommonConstants.Url,
                                                              CommonConstants.Arg,
                                                              CommonConstants.Object,
                                                              CommonConstants.Normal,
                                                              CommonConstants.Press,
                                                              CommonConstants.Focus],
                                                             [qsTr("업종 검색"),
                                                              CommonConstants.url(ViewRegistry.IndustrySearchView),
                                                              args[i],
                                                              null,
                                                              "navimenu/navimenu_ic_6.png",
                                                              "navimenu/navimenu_ic_6.png",
                                                              "navimenu/navimenu_ic_6_f.png"])

        args[++i] = ({})
        CommonUtils.insertEnum(menuEnum, "AddressSearch", i, [CommonConstants.Name,
                                                              CommonConstants.Url,
                                                              CommonConstants.Arg,
                                                              CommonConstants.Object,
                                                              CommonConstants.Normal,
                                                              CommonConstants.Press,
                                                              CommonConstants.Focus],
                                                             [qsTr("주소 검색"),
                                                              CommonConstants.url(ViewRegistry.AddressSearchView),
                                                              args[i],
                                                              null,
                                                              "navimenu/navimenu_ic_7.png",
                                                              "navimenu/navimenu_ic_7.png",
                                                              "navimenu/navimenu_ic_7_f.png"])
        args[++i] = ({})
        CommonUtils.insertEnum(menuEnum, "SettingMain",   i, [CommonConstants.Name,
                                                              CommonConstants.Url,
                                                              CommonConstants.Arg,
                                                              CommonConstants.Object,
                                                              CommonConstants.Normal,
                                                              CommonConstants.Press,
                                                              CommonConstants.Focus],
                                                            [qsTr("환경설정"),
                                                             CommonConstants.url(ViewRegistry.SettingMainView),
                                                             args[i],
                                                             null,
                                                             "navimenu/navimenu_ic_8.png",
                                                             "navimenu/navimenu_ic_8.png",
                                                             "navimenu/navimenu_ic_8_f.png"])


        menuSync()
        CommonUtils.fetchArr(menuEnum, CommonConstants.Object, menuObj)
        CommonUtils.fetchArr(menuEnum, CommonConstants.Normal, menuImgN)
        CommonUtils.fetchArr(menuEnum, CommonConstants.Press, menuImgP)
        CommonUtils.fetchArr(menuEnum, CommonConstants.Focus, menuImgF)
    }

    function allocateMenuArr(menu){
        var menuAttrEnum = menu.menuAttrEnum                
        menu.gridCol = naviMenuRoot.gridCol
        menu.gridRow = naviMenuRoot.gridRow
        menuArr = []
        for(var i = 0; i< menu.gridRow; i++) {
            menuArr[i] = []
            for(var j = 0; j<menu.gridCol; j++ ){
                menuArr[i][j] = []
                var k = (i * menu.gridCol) + (j)
                menuArr[i][j][menuAttrEnum.Name ] = menuName[k]
                menuArr[i][j][menuAttrEnum.Key  ] = menuUrl[k]
                menuArr[i][j][menuAttrEnum.Arg  ] = menuArg[k]
                menuArr[i][j][menuAttrEnum.Obj  ] = menuObj[k]
                menuArr[i][j][menuAttrEnum.ImgN ] = menuImgN[k]
                menuArr[i][j][menuAttrEnum.ImgP ] = menuImgP[k]
                menuArr[i][j][menuAttrEnum.ImgF ] = menuImgF[k]
             }
        }
        menu.menuArr = menuArr
    }

    function menuSync() {
        CommonUtils.fetchArr(menuEnum, CommonConstants.Name, menuName)
        CommonUtils.fetchArr(menuEnum, CommonConstants.Url, menuUrl)
        CommonUtils.fetchArr(menuEnum, CommonConstants.Arg, menuArg)
        if (!CommonUtils.isNull(gridCol)) {
           gridRow = CommonUtils.getGridRowCol(menuName, gridCol)
        } else if (!CommonUtils.isNull(gridRow)) {
           gridCol = CommonUtils.getGridRowCol(menuName, gridRow)
        }
    }    

    function requestIndustrySearch() {
        sendEvent(NaviMenuViewEvent.Request1DepthIndustrySearch)
    }

    function viewSizeAndModeChanged(height){
        var data = ({})
        data[CommonConstants.B] = height
        sendEvent(NaviMenuViewEvent.ViewSizeChanged, data)

        requestDirectionGuid()
    }

    function __setHome(m) {
        var x = m[CommonConstants.X]
        var y = m[CommonConstants.Y]

        var data = ({})
        data[CommonConstants.Address] = m[CommonConstants.Address]
        data[CommonConstants.Alias] = m[CommonConstants.Alias]
        CommonUtils.insertEnum(menuEnum, "Home",     menuEnum.Home,
                               [CommonConstants.Name,       CommonConstants.Url, CommonConstants.Arg],
                               [menuName[menuEnum.Home],    x + "," + y,         data               ])
        menuSync()
    }

    function __setOffice(m) {
        var x = m[CommonConstants.X]
        var y = m[CommonConstants.Y]

        var data = ({})
        data[CommonConstants.Address] = m[CommonConstants.Address]
        data[CommonConstants.Alias] = m[CommonConstants.Alias]
        CommonUtils.insertEnum(menuEnum, "Office",     menuEnum.Office,
                               [CommonConstants.Name,       CommonConstants.Url, CommonConstants.Arg],
                               [menuName[menuEnum.Office],  x + "," + y,         data               ])
        menuSync()
    }

    function requestRouteWithQml(name, qml) {
        // 집으로, 회사로 아이콘을 클릭하면 원래는 주석 처리된 아래의 조건으로
        // 좌표가 설정되어 있었는지를 확인하였지만 네비 메뉴의 전체적인 소스 변경으로
        // 동작이 변경되어 우선 임시로 ','가 있는지를 확인하여 있으면
        // 경로 요약화면으로 이동하도록 수정.
        // 17.06.15 ymhong.
        var i = CommonUtils.indexOf(qml, ",")
        var isHome = CommonUtils.isEqual(name , menuName[menuEnum.Home])
        if (i < 0) {
            var data = ({})
            if (isHome)
                sendEvent(NaviMenuViewEvent.ShowRegistHomeDialog, {owner:qml})
            else
                sendEvent(NaviMenuViewEvent.ShowRegistOfficeDialog, {owner:qml})
        } else  {
            var delim = CommonUtils.indexOf(qml, ",")
            var x = CommonUtils.substring(qml, 0, delim)
            var y = CommonUtils.substring(qml, delim + 1, qml.length)
            var m
            if (isHome)
                m = menuArg[menuEnum.Home]
            else
                m = menuArg[menuEnum.Office]
            var address = m[CommonConstants.Address]
            var aliasName = m[CommonConstants.Alias]
            var type = isHome ? 2 : 1

            requestRoute(x, y, aliasName, address, type)
        }
    }

    function requestRoute(x, y, name, address, type) {
        var data = ({})
        data[CommonConstants.Type] = type
        data[CommonConstants.X] = x
        data[CommonConstants.Y] = y
        data[CommonConstants.Name] = name
        data[CommonConstants.Address] = address
        sendEvent(NaviMenuViewEvent.RequestRoute, data)
    }

    //맵모드의 설정값 요청: __resultSettingDirectionGuid함수로 응답
    function requestDirectionGuid() {
        var data = ({})
        data[CommonConstants.Section] = SettingConstants.DirectionGuid
        data[CommonConstants.Slot] = "__resultSettingDirectionGuid"
        sendEvent(NaviMenuViewEvent.RequestSetting, data )
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

    function __openSearchTabPageView(page) {
        openSearchTabPageView(page)
    }

    function openSearchTabPageView(page) {
        var data = ({})
        data[CommonConstants.Url] =  CommonConstants.url(ViewRegistry.SearchTabView)
        data[CommonConstants.Caller] = CommonConstants.objectName(ViewRegistry.NaviMenuView)
        data[SearchConstants.Page] = page
        sendEvent(NaviMenuViewEvent.CreateView, data )
        close()
    }

    function doWorkMenuItem(row, col) {
        var data = ({})
        var index = (row * gridCol) + (col)
        var name = menuName[index]
        var url = menuUrl[index]
        var args = menuArg[index]
        if (CommonUtils.isEqual(name , menuName[menuEnum.Home])) {
            requestRouteWithQml(name, url)
        }
        else if (CommonUtils.isEqual(name , menuName[menuEnum.Office])) {
            requestRouteWithQml(name, url)
        }
        else if (CommonUtils.isEqual(name , menuName[menuEnum.RecentDest])) {
            data[CommonConstants.Owner ] = CommonConstants.objectName(ViewRegistry.NaviMenuView)
            data[CommonConstants.Offset] = 0 //시작 인덱스
            data[CommonConstants.Max   ] = 1 //검색 갯수
            data[CommonConstants.Type  ] = SearchConstants.SortHit
            sendEvent(NaviMenuViewEvent.RequestRecentDestination, data)
        }
        else if (CommonUtils.isEqual(name , menuName[menuEnum.Bookmark])) {
            openSearchTabPageView(SearchConstants.FavoritePageIndex)
        }
        else {
            if(url.length <= 0)
              return
            data[CommonConstants.Url] = url
            for (var key in args) {
               data[key] =  args[key]
            }
            data[CommonConstants.Caller] = CommonConstants.objectName(ViewRegistry.NaviMenuView)
            sendEvent(NaviMenuViewEvent.CreateView, data )

            // 즐겨찾기, 최근목적지일 때는 창을 닫는다.
            if (CommonUtils.indexOf(url, CommonConstants.objectName(ViewRegistry.SearchTabView)) >= 0) {
                close()
            }
        }
    }


    function init(){
        if (isMediated) {
            sendEvent(NaviMenuViewEvent.RequestViewList)
//            if (isShowTopView) {
//                return
//            }
        }
        if (!needInit)
            return
        needInit = false

        allocateTextEnum()
        allocateMenuEnum()

        loader.sourceComponent = undefined
        loader.sourceComponent = main

        if (active) {
            sendEvent(NaviMenuViewEvent.SelectZone1Title, textArr[textEnum.Zone1Title])
        }
    }

    onMediated: {
        sendEvent(NaviMenuViewEvent.RequestLang)
        init()
        isMediated = true
    }

    onActivated: {
        if (!sync) {
            sync = true
            return
        }
        init()
//        if (isShowTopView) {
//            return
//        }
        enabled = true
        sendEvent(NaviMenuViewEvent.ResetCsw)
        sendEvent(NaviMenuViewEvent.RegistCsw, [ csw ])
        sendEvent(NaviMenuViewEvent.SelectZone1Title, textArr[textEnum.Zone1Title])
        sendEvent(NaviMenuViewEvent.Activated,[CommonConstants.NoOption])
        viewSizeAndModeChanged(frameHeight)
    }

    onDeactivated: {
        enabled = false
        sendEvent(NaviMenuViewEvent.Deactivated)
    }

    Component.onDestruction: {
    }

    function __onResultViewList(result) {
        for(var i= 0; i<result.length; i++) {
            if (CommonUtils.isIndexOf(result[i][CommonConstants.ResultViewName],
                                     [CommonConstants.objectName(ViewRegistry.SearchAroundView),
                                      CommonConstants.objectName(ViewRegistry.IndustrySearchView),
                                      CommonConstants.objectName(ViewRegistry.AddressSearchView),
                                      CommonConstants.objectName(ViewRegistry.SettingMainView),
                                      CommonConstants.objectName(ViewRegistry.SearchResultView),
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
        naviMenuRoot.__lang = lang
        needInit = true
        if (active) {
            init()
        }
    }

    function __doSoftKeyFunction(button, menuName, menuIndex) {
      switch (button) {
        case ZoneConstants.Back:
          sendEvent(NaviMenuViewEvent.ActivatedForNavi)
          close()
          break
        case ZoneConstants.Option:
        case ZoneConstants.Navi:
        case ZoneConstants.Menu:
        default:
          break;
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
            Item {
               width: parent.width
               height: parent.height - frameHeight
            }
            TileMenu {
                Component.onCompleted: {
                    naviMenuRoot.menu = this                    
                    allocateMenuArr(menu)
                }
                width: parent.width
                height: frameHeight - 1
                PressBar { currentItem: menu.listView.pressedItem;opacity: 0.3}
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
                         item.img.y = naviMenuRoot.layout.tileMenuContentButtonYOffset

                         item.txt.elide       = Text.ElideRight
                         item.txt.font.family =  GlobalConstants.kFontFamily
                         item.txt.font.pixelSize =  Resource.pixelToDp(GlobalConstants.kOneLineListTextFontSize)
                         item.txt.y = naviMenuRoot.layout.tileMenuContentTextYOffset
                         item.txt.lineHeightMode = Text.FixedHeight
                         item.txt.lineHeight = naviMenuRoot.layout.tileMenuContentTextLineHeight
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
                        if (CommonUtils.isEqual(currentItem,  menu.listView.pressedItem))
                            return
                        menu.listView.pressedItem = currentItem
                        menu.highlightColor = CommonConstants.Transparent
                    }
                }

                onButtonClicked: doWorkMenuItem(row,col)

                function clickItem(row, col)  {
                    buttonPressed (menu.listView.model, row, col)
                    buttonReleased(menu.listView.model, row, col)
                    buttonClicked (menu.listView.model, row, col)
                }

            }
            //그리드 형태의 메뉴의 백그라운드 컬러는#222226, 라인색만 #000000을 사용하고 있으니 주의 필요
            Rectangle{ width: parent.width; height: 1; color: GlobalConstants.kBasicBackgoundColor}                        
        }
    }

    Csw {
            id: csw

            section: Csw.Section_NaviMenuGrid
            sequence: 0
            enable: naviMenuRoot.visible

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
