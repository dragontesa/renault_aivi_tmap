import QtQuick 2.1
import MapModeOptionViewEvent 1.0
import SettingConstants 1.0
import CommonConstants 1.0
import SceneGraphRendering 1.0
import Csw 1.0

import "../../singleton"
import "../../component"
import "../../view"
import "../../view/util"
import "../../GlobalConstants.js" as GlobalConstants
import "../../view/util/CommonUtils.js" as CommonUtils


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 수정 이력
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// [chunjae.lee] 2018-02-09: TMCRSM-341 - "3D버드뷰" -> "3D" 변경 요청
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PopupView {
    id: optionRoot
    modal: true
    anchors { fill: parent; bottomMargin: GlobalConstants.kScreenHardKeyHeight }

    property var caller                 : null
    property var viewMode               : null
    property var settings               : null
    property var routeOption            : null

    //onMediated에서 요청 및 응답
    property var __lang                 : null

    property bool needInit              : true

    property int mapCol                 : 3
    property int routeRow               : 2
    property int routeCol               : 3
    property int routeBtnCol            : 2

    property bool focused               : false
    property int highlightBorderWidth   : 2
    property color highlightBorderColor : CommonConstants.Transparent
    property color lineColor            : "#a1a2b3"

    property int subTitleHeight         : 90
    property int mapModeHeight          : 198
    property int otherRouteHeight       : 205
    property int botTitleHeight         : 62

    //Timer
    property var timer                  : null


    //ID
    property var mapMode                : null
    property var other                  : null
    property var otherRoute             : null
    property var roteBtn                : null
    property var idCancel               : null

    //Enum
    property var textEnum               : ({})
    property var mapModEnum             : ({})
    property var otherRouteEnum         : ({})
    property var routeBtnEnum           : ({})
    property var sectionEnum            : ({})
    property var keyEnum                : ({})
    property var viewModeEnum           : ({})
    //------------------------------
    property var textArr                : ([])
    property var mapModTxtArr           : ([])
    property var mapModImgArr           : ([])
    property var mapModObjArr           : ([])
    property var otherRouteTxtArr       : ([])
    property var otherRouteObjArr       : ([])
    property var routeBtnTxtArr         : ([])
    property var routeBtnImgArr         : ([])
    property var routeBtnObjArr         : ([])
    property var sectionArr             : ([])
    property var keyArr                 : ([])
    property var viewModeTxtArr         : ([])

    property alias captureEnable : gestureArea.enabled

    function allocateVariant() {
    }

    function allocateTextEnum() {
        if (CommonUtils.getObjectLen(textEnum) <= 0) {
            textEnum = CommonUtils.createEnum(textEnum)
        }
        var i = 0
        CommonUtils.insertEnum(textEnum, "MapMode",     i++, [CommonConstants.Name], ["지도 모드"])
        CommonUtils.insertEnum(textEnum, "AnotherPass", i++, [CommonConstants.Name], ["다른 경로"])
        CommonUtils.insertEnum(textEnum, "Cancel",      i++, [CommonConstants.Name], ["취소"])
        CommonUtils.insertEnum(textEnum, "ToastMessage",i++, [CommonConstants.Name], ["네트워크가 연결되어야 탐색 가능한 경로입니다.<br>최적길, 최단거리, 무료도로 경로를 이용해 주세요."])

        CommonUtils.fetchArr(textEnum,  CommonConstants.Name, textArr)
    }

   function allocateMapModEnum() {
        if (CommonUtils.getObjectLen(mapModEnum) <= 0) {
            mapModEnum = CommonUtils.createEnum(mapModEnum)
        }
        CommonUtils.insertEnum(mapModEnum, "Bird",
                               SettingConstants.MapModeBirdView,
                               [CommonConstants.Name , CommonConstants.Url    , CommonConstants.Object],
                               ["3D"                 , "popup_btn_3d_"        , null                  ]) //TMCRSM-341
        CommonUtils.insertEnum(mapModEnum, "Drive",
                               SettingConstants.MapModeDriveDirection,
                               [CommonConstants.Name , CommonConstants.Url    , CommonConstants.Object],
                               ["주행방향"           , "popup_btn_driving_"   , null                  ])
        CommonUtils.insertEnum(mapModEnum, "North",
                               SettingConstants.MapModeNorthDirection,
                               [CommonConstants.Name , CommonConstants.Url    , CommonConstants.Object],
                               ["정북방향"           , "popup_btn_north_"     , null                  ])

       CommonUtils.fetchArr(mapModEnum,  CommonConstants.Name  , mapModTxtArr)
       CommonUtils.fetchArr(mapModEnum,  CommonConstants.Url   , mapModImgArr)
       CommonUtils.fetchArr(mapModEnum,  CommonConstants.Object, mapModObjArr)
    }

    function allocateOtherRouteEnum() {
        if (CommonUtils.getObjectLen(otherRouteEnum) <= 0) {
            otherRouteEnum = CommonUtils.createEnum(otherRouteEnum)
        }
        CommonUtils.insertEnum(otherRouteEnum, "TMapOptimalPath",
                               SettingConstants.OtherRouteTMapOptimalPath,
                               [CommonConstants.Name , CommonConstants.Object],
                               ["T map<br>최적경로"  , null               ])
        CommonUtils.insertEnum(otherRouteEnum, "MinimalTime",
                               SettingConstants.OtherRouteMinimalTime,
                               [CommonConstants.Name , CommonConstants.Object],
                               ["최소시간"           , null                  ])
        CommonUtils.insertEnum(otherRouteEnum, "FreeRoad",
                               SettingConstants.OtherRouteFreeRoad,
                               [CommonConstants.Name , CommonConstants.Object],
                               ["무료도로"           , null                  ])
        CommonUtils.insertEnum(otherRouteEnum, "ShortestDistance",
                               SettingConstants.OtherRouteShortestDistance,
                               [CommonConstants.Name , CommonConstants.Object],
                               ["최단거리"           , null                  ])
        CommonUtils.insertEnum(otherRouteEnum, "HighwayPriority",
                               SettingConstants.OtherRouteHighwayPriority,
                               [CommonConstants.Name , CommonConstants.Object],
                               ["고속도로우선"       , null                  ])
        CommonUtils.insertEnum(otherRouteEnum, "BeginnerPass",
                               SettingConstants.OtherRouteBeginnerPass,
                               [CommonConstants.Name , CommonConstants.Object],
                               ["초보자경로"         , null                  ])

        CommonUtils.fetchArr(otherRouteEnum,  CommonConstants.Name, otherRouteTxtArr)
        CommonUtils.fetchArr(otherRouteEnum,  CommonConstants.Object, otherRouteObjArr)
    }

    function allocateRouteBtnEnum() {
        if (CommonUtils.getObjectLen(routeBtnEnum) <= 0) {
            routeBtnEnum = CommonUtils.createEnum(routeBtnEnum)
        }
        var i = 0
        CommonUtils.insertEnum(routeBtnEnum, "Summary", i++,
                               [CommonConstants.Name, CommonConstants.Url  , CommonConstants.Object],
                               ["경로 요약"         , "popup_mode_btn_"    , null])
        CommonUtils.insertEnum(routeBtnEnum, "Cancel",  i++,
                               [CommonConstants.Name, CommonConstants.Url  , CommonConstants.Object],
                               ["경로 취소"         , "popup_mode_btn_"    , null])

        CommonUtils.fetchArr(routeBtnEnum,  CommonConstants.Name, routeBtnTxtArr)
        CommonUtils.fetchArr(routeBtnEnum,  CommonConstants.Url, routeBtnImgArr)
        CommonUtils.fetchArr(routeBtnEnum,  CommonConstants.Object, routeBtnObjArr)
    }

    function allocateSectionEnum() {
        if (CommonUtils.getObjectLen(sectionEnum) <= 0) {
            sectionEnum = CommonUtils.createEnum(sectionEnum)
        }
        var i = 0
        CommonUtils.insertEnum(sectionEnum, "DirectionGuid",     i++, [CommonConstants.Name], [SettingConstants.DirectionGuid])

        CommonUtils.fetchArr(sectionEnum,  CommonConstants.Name, sectionArr)
    }

    function allocateKeyEnum() {
        if (CommonUtils.getObjectLen(keyEnum) <= 0) {
            keyEnum = CommonUtils.createEnum(keyEnum)
        }
        var i = 0
        CommonUtils.insertEnum(keyEnum, "MapMode",     i++, [CommonConstants.Name], [SettingConstants.MapMode])
        CommonUtils.insertEnum(keyEnum, "OtherRoute",  i++, [CommonConstants.Name], [SettingConstants.OtherRoute])

        CommonUtils.fetchArr(keyEnum,  CommonConstants.Name, keyArr)
    }

   function allocateViewModeEnum() {
       if (CommonUtils.getObjectLen(viewModeEnum) <= 0) {
            viewModeEnum = CommonUtils.createEnum(viewModeEnum)
       }
       var i = 0
        CommonUtils.insertEnum(viewModeEnum, "Driving",    i++, [CommonConstants.Name], ["Driving"   ])
        CommonUtils.insertEnum(viewModeEnum, "Directions", i++, [CommonConstants.Name], ["Directions"])

       CommonUtils.fetchArr(viewModeEnum,  CommonConstants.Name, viewModeTxtArr)
    }

    function changeMapViewMode(index) {
        switch (index) {
        case mapModEnum.Bird:
            mapView.viewMode = Renderer.BirdView
            break
        case mapModEnum.Drive:
            mapView.viewMode = Renderer.HeadUp
            break
        case mapModEnum.North:
            mapView.viewMode = Renderer.NorthBound
            break
        }
    }

    property var cancelObjArr   : []
    property var cswObjArr      : []
    property int cswObjArrLength: 0
    property int currentIndex   : -1
    property var currentItem    : null
    property var pressedItem    : null
    onCurrentItemChanged: {
        if (CommonUtils.isNull(currentItem) ) {
            highlightBorderWidth = 2
        }
    }

    onCurrentIndexChanged: {
        if (currentIndex < 0 ||  currentIndex >= cswObjArrLength) {
            currentItem = null
            return
        }

        if (cswObjArr[0].length > currentIndex) {
            currentItem =  cswObjArr[0][currentIndex]
        } else if (cswObjArr[0].length + cswObjArr[1].length > currentIndex) {
            currentItem =  cswObjArr[1][currentIndex - cswObjArr[0].length]
        } else if (cswObjArr[0].length + cswObjArr[1].length + cswObjArr[2].length  > currentIndex) {
            currentItem =  cswObjArr[2][currentIndex - (cswObjArr[0].length + cswObjArr[1].length)]
        }  else if (cswObjArr[0].length + cswObjArr[1].length + cswObjArr[2].length + cswObjArr[3].length  > currentIndex) {
            currentItem =  cswObjArr[3][currentIndex - (cswObjArr[0].length + cswObjArr[1].length + cswObjArr[2].length)]
        }
    }

    function allocateCswObjArr(){
        cswObjArr      = other.visible ? [mapModObjArr, otherRouteObjArr, routeBtnObjArr, cancelObjArr] :
                                         [mapModObjArr, cancelObjArr]
        cswObjArrLength= other.visible ? (mapModObjArr.length + otherRouteObjArr.length + routeBtnObjArr.length + cancelObjArr.length) :
                                         (mapModObjArr.length + cancelObjArr.length)
    }

    function showDirectionsMode() {
        other.visible = true
        allocateCswObjArr()
    }

    function showDrivingMode() {
        other.visible = false
        allocateCswObjArr()
    }

    function showToast(message)   {
        var data = ({})
        data[CommonConstants.Message] = message
        data[CommonConstants.Position] = CommonConstants.ToastUp
        data[CommonConstants.Duration] = 3 * 1000
        sendEvent(MapModeOptionViewEvent.ShowToast ,data)
    }


    function init(){
        if (!needInit)
            return
        needInit = false

        allocateVariant()

        allocateTextEnum()
        allocateMapModEnum()
        allocateOtherRouteEnum()
        allocateRouteBtnEnum()
        allocateSectionEnum()
        allocateKeyEnum()
        allocateViewModeEnum()

        subTitleHeight = optionRoot.layout.subTitleHeight
        mapModeHeight =  optionRoot.layout.mapModeHeight
        otherRouteHeight =  optionRoot.layout.otherRouteHeight
        botTitleHeight =  optionRoot.layout.botTitleHeight

        loader.sourceComponent = undefined
        loader.sourceComponent = main


        settings = null
        var data = {section: sectionArr[sectionEnum.DirectionGuid]}
        sendEvent(MapModeOptionViewEvent.RequestSetting, data )

        if (!viewMode ) {
           showDrivingMode()
           return
        }
        if (CommonUtils.isEqual(viewMode , viewModeTxtArr[viewModeEnum.Driving])) {
            showDrivingMode()
        } else if(CommonUtils.isEqual(viewMode , viewModeTxtArr[viewModeEnum.Directions])) {
            showDirectionsMode()
        }
    }

    onMediated: {
        sendEvent(MapModeOptionViewEvent.Mediated)
        sendEvent(MapModeOptionViewEvent.RequestLang)
        init()
    }

    onSettingsChanged: {
        var value, index
        if (CommonUtils.isNull(settings))
            return
        for (var i = 0; i<keyArr.length; i++){
            if (!settings.hasOwnProperty(keyArr[i]))
                continue
            if (CommonUtils.isEqual(i , keyEnum.MapMode)){
                value = settings[keyArr[i]][CommonConstants.Value]
                index = CommonUtils.number(value)
                mapModObjArr[index].box.colorP = Qt.binding(function(){
                    if (optionRoot.focused && CommonUtils.isEqual(mapModObjArr[index] , currentItem))
                        return GlobalConstants.kListFocusColor
                    return MultiSenseCoordinator.kAmbientColor
                })
                mapModObjArr[index].box.opacityP = 1.0
                mapModObjArr[index].checked = CommonConstants.True
            } else if (CommonUtils.isEqual(i , keyEnum.OtherRoute)) {
                if (!CommonUtils.isNull(routeOption)) {
                    index = routeOption
                    otherRouteObjArr[index].box.colorP = Qt.binding(function(){
                        if (optionRoot.focused && CommonUtils.isEqual(otherRouteObjArr[index] , currentItem))
                            return GlobalConstants.kListFocusColor
                        return MultiSenseCoordinator.kAmbientColor
                    })
                    otherRouteObjArr[index].box.opacityP = 1.0
                    otherRouteObjArr[index].checked = CommonConstants.True
                }
            }
        }
    }

    function __setRouteOption(value) {
        var index = routeOption = CommonUtils.number(value)
        if (CommonUtils.isNull(otherRouteObjArr[index]))
            return
        otherRouteObjArr[index].box.colorP = Qt.binding(function(){
            if (optionRoot.focused && CommonUtils.isEqual(otherRouteObjArr[index] , currentItem))
                return GlobalConstants.kListFocusColor
            return MultiSenseCoordinator.kAmbientColor
        })
        otherRouteObjArr[index].box.opacityP = 1.0
        otherRouteObjArr[index].checked = CommonConstants.True
    }

    //언어가 변경되었을 때
    function __retranslateUi(lang) {
        optionRoot.__lang = lang
        needInit = true
        if (active) {
            init()
        }
    }

     onActivated: {
         init()
         sendEvent(MapModeOptionViewEvent.Activated)
         sendEvent(MapModeOptionViewEvent.ResetCsw)
         sendEvent(MapModeOptionViewEvent.RegistCsw, [ csw ])
         sendEvent(MapModeOptionViewEvent.ModalChanged, {modal: true})
     }

     onDeactivated: {
         sendEvent(MapModeOptionViewEvent.ModalChanged, {modal: false})
         sendEvent(MapModeOptionViewEvent.RemoveCsw, { 'csw':csw })
         sendEvent(MapModeOptionViewEvent.ResetCsw)
         sendEvent(MapModeOptionViewEvent.Deactivated)
     }

     Component.onDestruction: {
         timer = CommonUtils.destroy(timer)
     }


     //main
     Loader {
          id: loader
          width: parent.width
          height:parent.height
     }

     Component {
         id: main
         Item {
             id: contentArea
             onParentChanged: {
                 if (CommonUtils.isNull(parent))
                     return
                 width = parent.width
                 height = parent.height
             }
             anchors.centerIn: parent
             Rectangle {
                 width: 640
                 height: top.height +  mapMode.height + ((other && other.visible) ? other.height : 0 )+ bottom.height
                 color: GlobalConstants.kBasicBackgoundColor
                 anchors.centerIn: parent
                 PressBar { currentItem: optionRoot.pressedItem; opacity: 0.3
                     leftMargin: 1;rightMargin: 0;topMargin: 1;bottomMargin: 0
                 }
                 HighlightBar {
                     currentItem: optionRoot.currentItem
                     border.color: highlightBorderColor
                     border.width: highlightBorderWidth
                     leftMargin: 1;rightMargin: 0;topMargin: 1;bottomMargin: 0
                     color: {
                         if (optionRoot.focused) {
                             if (CommonUtils.isEqual(routeBtnObjArr[routeBtnEnum.Summary] , currentItem) ||
                                 CommonUtils.isEqual(routeBtnObjArr[routeBtnEnum.Cancel] , currentItem)  ) {
                                 return CommonConstants.Transparent
                             } else {
                                 return GlobalConstants.kListFocusColor
                             }
                         }  else {
                             return MultiSenseCoordinator.kAmbientColor
                         }
                     }
                 }
                 BorderImage {
                     id: borderImage
                     width: parent.width
                     height: parent.height
                     source: Resource.imageUrl("popup/popup_mode_bg.sci")
                     Column {
                         width: parent.width - ((other && other.visible) ? 40 : 0)
                         height: parent.height
                         anchors.margins: 0
                         anchors.centerIn: parent

                         ///////////////////////////////
                         //지도모드
                         Item {
                             id: top
                             height: subTitleHeight - ((other && other.visible) ? 24 : 0)
                             width: parent.width
                             Text {
                                 anchors.centerIn: parent
                                 font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(32)}
                                 color: GlobalConstants.kListTextColor
                                 text: textArr[textEnum.MapMode]
                             }
                         }
                         Row {
                             Component.onCompleted: optionRoot.mapMode = this
                             width: parent.width
                             height: mapModeHeight -  ((other && other.visible) ? 20 : 0)
                             Repeater {
                                 model: mapCol
                                 delegate: mapModeDelegate
                             }
                         }
                         //지도모드
                         ///////////////////////////////

                         ///////////////////////////////
                         //다른경로
                         Column {
                                 Component.onCompleted: optionRoot.other = this
                                 width: parent.width
                                 visible: false
                                 Item {
                                     height: subTitleHeight - ((other && other.visible) ? 24 : 0)
                                     width: parent.width
                                     Text {
                                         anchors.centerIn: parent
                                         font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(32) }
                                         color: GlobalConstants.kListTextColor
                                         text: textArr[textEnum.AnotherPass]
                                     }
                                 }
                                 Column {
                                     height: 200
                                     width: parent.width
                                     Repeater {
                                         model : routeRow
                                         delegate:
                                              Row {
                                                 Component.onCompleted: optionRoot.otherRoute = this
                                                 width: parent.width
                                                 height: (parent.height)/2
                                                 property int rowIdx: index
                                                 Repeater {
                                                      model: routeCol
                                                      delegate: otherRouteDelegate
                                                 }
                                              }
                                     }
                                 }
                                 Item {width:parent.width;height:24 }
                                 Row {
                                     Component.onCompleted: optionRoot.roteBtn = this
                                     width: parent.width
                                     height: 80
                                     Repeater {
                                         model: routeBtnCol
                                         delegate: routeBtnDelegate
                                     }
                                 }
                                 Item {width:parent.width;height:24 }
                         }
                         //다른경로
                         ///////////////////////////////

                         ///////////////////////////////
                         //취소
                         Column {
                             id: bottom
                             x:(other && other.visible) ? -(40/2)+1 : 1
                             height: botTitleHeight
                             width: borderImage.width - 2
                             Rectangle {width: parent.width;height: (other && other.visible) ? 1 : 0; color: optionRoot.lineColor}
                             Item {
                                 property var childEnum: ({top:0,mid:1,bot:2})
                                 width: parent.width
                                 height: {
                                     var top = CommonUtils.getItemFromObj(bottom, childEnum.top)
                                     var bot = CommonUtils.getItemFromObj(bottom, childEnum.bot)
                                     return botTitleHeight - (top.height+bot.height)
                                 }
                                 TPushButton {
                                    anchors.fill: parent
                                    anchors.margins: 0
                                    enabledOpacity: false
                                    Component.onCompleted: { optionRoot.idCancel = this
                                        box.colorN          = CommonConstants.Transparent
                                        box.colorP          = Qt.binding(function(){
                                            if (optionRoot.focused && CommonUtils.isEqual(cancelObjArr[0] , currentItem))
                                                return GlobalConstants.kListFocusColor
                                            return MultiSenseCoordinator.kAmbientColor
                                        })
                                        box.opacityP        = 0.3
                                        txt.colorN          = Qt.binding(function(){
                                            if (optionRoot.focused && CommonUtils.isEqual(cancelObjArr[0] , currentItem))
                                                return GlobalConstants.kBasicBackgoundColor
                                            return GlobalConstants.kListTextColor
                                        })
                                        txt.colorP          = Qt.binding(function(){
                                            if (optionRoot.focused && CommonUtils.isEqual(cancelObjArr[0] , currentItem))
                                                return GlobalConstants.kBasicBackgoundColor
                                            return GlobalConstants.kListTextColor
                                        })
                                        txt.text            = textArr[textEnum.Cancel]
                                        txt.font.family     = GlobalConstants.kFontFamily
                                        txt.font.pixelSize  = Resource.pixelToDp(32)
                                        cancelObjArr[0]     = this
                                    }

                                    function execButton() {
                                        close()
                                    }

                                    onButtonPressed: {
                                        csw.forceLostFocus()
                                    }

                                    onButtonReleased:{
                                    }

                                    onButtonClicked: {
                                        execButton()
                                    }
                                }
                            }
                            Item {width: parent.width;height: 1}
                         }
                         //취소
                         ///////////////////////////////
                     }
                     Rectangle {
                         anchors.fill: parent
                         border.width: 1
                         border.color: optionRoot.lineColor
                         color: CommonConstants.Transparent
                     }
                 }//Image
             }
         }
    }

    ExclusiveGroup { id: groupMap }
    ExclusiveGroup { id: groupRoute }

    property real mapmodeTxtMaxHeight: 0
    property real mapmodeTxtImgGap   : (other &&other.visible) ? 3 : 14
    Component {
        id: mapModeDelegate
        Item {
            height: mapMode.height
            width: mapMode.width/3
            TPushButton {
               anchors.fill: parent
               exclusiveGroup: groupMap
               checked: CommonConstants.False
               enabledOpacity: false
               property real txtContentHeight : txt.contentHeight
               onTxtContentHeightChanged: {
                   if (mapmodeTxtMaxHeight < txt.contentHeight) {
                       mapmodeTxtMaxHeight = txt.contentHeight
                   }
               }

               Component.onCompleted: {
                   outline.y                    = Qt.binding(function(){ return (box.height - (mapmodeTxtMaxHeight + mapmodeTxtImgGap + img.height))/2})
                   outline.height               = Qt.binding(function(){ return mapmodeTxtMaxHeight})

                   box.margins                  = 1
                   box.colorN                   = CommonConstants.Transparent
                   box.colorP                   = CommonConstants.Transparent

                   txt.colorN                   = Qt.binding(function(){
                       if (optionRoot.focused && CommonUtils.isEqual(mapModObjArr[index] , currentItem))
                           return GlobalConstants.kBasicBackgoundColor
                       return GlobalConstants.kListTextColor
                   })
                   txt.colorP                   = Qt.binding(function(){
                       if (optionRoot.focused && CommonUtils.isEqual(mapModObjArr[index] , currentItem))
                           return GlobalConstants.kBasicBackgoundColor
                       return GlobalConstants.kListTextColor
                   })
                   txt.text                     = mapModTxtArr[index]                   
                   txt.font.family              = GlobalConstants.kFontFamily
                   txt.font.pixelSize           = Resource.pixelToDp(32)
                   txt.anchors.centerIn         = undefined
                   txt.anchors.horizontalCenter = outline.horizontalCenter
                   txt.anchors.verticalCenter   = outline.verticalCenter

                   img.anchors.horizontalCenter = box.horizontalCenter
                   img.sourceN                  = Resource.imageUrl(mapModImgArr[index] + "n.png")
                   img.sourceP                  = Resource.imageUrl(mapModImgArr[index] + "p.png")
                   img.y                        = Qt.binding(function(){ return ((box.height - (mapmodeTxtMaxHeight + mapmodeTxtImgGap + img.height))/2) + mapmodeTxtMaxHeight + mapmodeTxtImgGap})

                   mapModObjArr[index]          = this
               }

                function execButton() {
                    highlightBorderWidth = 0
                    currentIndex =  index

                    changeMapViewMode(index)

                    var sleep =  GlobalConstants.kLongPressedColorExposureTime
                    timer = CommonUtils.destroy(timer)
                    timer = CommonUtils.delay(optionRoot, sleep, function() {
                        try {
                            currentIndex = -1

                            var newSettings = optionRoot.settings
                            var key = keyArr[keyEnum.MapMode]
                            var value = index
                            newSettings[key][CommonConstants.Value] = CommonUtils.string(value)
                            var data = {
                              section: sectionArr[sectionEnum.DirectionGuid],
                              settings: newSettings
                            }
                            optionRoot.sendEvent(MapModeOptionViewEvent.SaveSetting, data )
                            close()
                        } catch(e) {
                        }
                    })
                }

                property var pressed: null
                onButtonPressed: pressed = true
                onButtonReleased: pressed = false
                PressBarMouseSync {
                    cswObj: csw
                    currentItem: optionRoot.pressedItem
                    pressed: parent.pressed
                    onCurrentItemChanged: {
                        if (CommonUtils.isEqual(currentItem , optionRoot.pressedItem)) {
                            return
                        }
                        highlightBorderWidth = 0
                        optionRoot.pressedItem = currentItem
                    }
                }


                onButtonClicked: {
                    execButton()
                }
                Rectangle {x:0;y:0;width:parent.width;height:1;color:optionRoot.lineColor; opacity:0.3}
                Rectangle {x:0;y:1;width:1;height:parent.height-2;color:optionRoot.lineColor; opacity:0.3;visible:(other &&other.visible)? true : index}
                Rectangle {x:parent.width-1;y:1;width:1;height:parent.height-2;color:optionRoot.lineColor; opacity:0.3;visible: index == (mapCol-1)}
                Rectangle {x:0;y:parent.height-1;width:parent.width;height:1;color:optionRoot.lineColor; opacity:(other &&other.visible) ? 0.3 : 1.0}
           }
        }
    }


    Component {
        id: otherRouteDelegate
        Item {
            width: otherRoute.width/3
            height: otherRoute.height
            TPushButton {
               anchors.fill: parent
               exclusiveGroup: groupRoute
               checked: CommonConstants.False
               enabledOpacity: false
               property int rowIdx : {
                   if (CommonUtils.isNull(parent) ||
                       CommonUtils.isNull(parent.parent) ||
                        CommonUtils.isNull(parent.parent.rowIdx))
                       return 0
                   return parent.parent.rowIdx
               }
               Component.onCompleted: {
                   box.margins                                   = 1
                   box.colorN                                    = CommonConstants.Transparent
                   box.colorP                                    = CommonConstants.Transparent
                   txt.colorN                                    = Qt.binding(function(){
                       if (optionRoot.focused && CommonUtils.isEqual(otherRouteObjArr[(rowIdx * routeCol) + index] , currentItem))
                           return GlobalConstants.kBasicBackgoundColor
                       return GlobalConstants.kListTextColor
                   })
                   txt.colorP                                    = Qt.binding(function(){
                       if (optionRoot.focused && CommonUtils.isEqual(otherRouteObjArr[(rowIdx * routeCol) + index] , currentItem))
                           return GlobalConstants.kBasicBackgoundColor
                       return GlobalConstants.kListTextColor
                   })
                   txt.text                                      = otherRouteTxtArr[(rowIdx * routeCol) + index]
                   txt.font.family                               = GlobalConstants.kFontFamily
                   txt.font.pixelSize                            = Resource.pixelToDp(32)
                   txt.lineHeightMode                            = Text.FixedHeight
                   txt.lineHeight                                = optionRoot.layout.contentTextLineHeight
                   txt.horizontalAlignment                       = Text.AlignHCenter

                   otherRouteObjArr[(rowIdx * routeCol) + index] = this
               }

               function execButton() {
                   highlightBorderWidth = 0
                   currentIndex = mapModObjArr.length + (rowIdx * routeCol) + index

                   var sleep =  GlobalConstants.kLongPressedColorExposureTime
                   timer = CommonUtils.destroy(timer)
                   timer = CommonUtils.delay(optionRoot, sleep, function() {
                       try {
                           currentIndex = -1
                           var newSettings = optionRoot.settings
                           var key = keyArr[keyEnum.OtherRoute]
                           var value = (rowIdx * routeCol) + index
                           newSettings[key][CommonConstants.Value] = CommonUtils.string(value)
                           var data = {
                               section: sectionArr[sectionEnum.DirectionGuid],
                               settings: newSettings
                           }
                           sendEvent(MapModeOptionViewEvent.SaveSetting, data )

                           sendEvent(MapModeOptionViewEvent.OtherRouteClicked, value)

                           close()
                       } catch(e) {
                       }
                   })
               }

               property var pressed: null
               onButtonPressed: pressed = true
               onButtonReleased: pressed = false
               PressBarMouseSync {
                   cswObj: csw
                   currentItem: optionRoot.pressedItem
                   pressed: parent.pressed
                   onCurrentItemChanged: {
                       if (CommonUtils.isEqual(currentItem , optionRoot.pressedItem)) {
                           return
                       }
                       highlightBorderWidth = 0
                       optionRoot.pressedItem = currentItem
                   }
               }

               onButtonClicked: {
                   execButton()
               }

               Rectangle {x:0;y:0;width:parent.width;height:1;color:optionRoot.lineColor; opacity:0.3}
               Rectangle {x:0;y:1;width:1;height:parent.height-2;color:optionRoot.lineColor; opacity:0.3;visible:(other &&other.visible)? true : index}
               Rectangle {x:parent.width-1;y:1;width:1;height:parent.height-2;color:optionRoot.lineColor; opacity:0.3;visible: index == (mapCol-1)}
               Rectangle {x:0;y:parent.height-1;width:parent.width;height:1;color:optionRoot.lineColor; opacity:(other &&other.visible) ? 0.3 : 1.0}
           }
        }
    }

    Component {
        id: routeBtnDelegate
        Item {
            width: roteBtn.width/2
            height: roteBtn.height
            TPushButton {
                width: 290
                height:parent.height
                mutisense: true
                overlayColor: optionRoot.focused ? GlobalConstants.kListFocusColor : MultiSenseCoordinator.kAmbientColor
                enabledOpacity: false
                Component.onCompleted: {
                    switch(index) {
                    case routeBtnEnum.Summary: anchors.left = parent.left; break;
                    case routeBtnEnum.Cance: anchors.right = parent.right; break;
                    }
                    box.colorN              = CommonConstants.Transparent
                    box.colorP              = CommonConstants.Transparent
                    img.sourceN             = Qt.binding(function(){
                        if (optionRoot.focused && CommonUtils.isEqual(routeBtnObjArr[index] , currentItem))
                            return Resource.imageUrl(routeBtnImgArr[index] + "p.png")
                        return Resource.imageUrl(routeBtnImgArr[index] + "n.png")
                    })
                    img.sourceP             = Resource.imageUrl(routeBtnImgArr[index] + "p.png")
                    txt.colorN              = Qt.binding(function(){
                        if (optionRoot.focused && CommonUtils.isEqual(routeBtnObjArr[index] , currentItem))
                            return GlobalConstants.kBasicBackgoundColor
                        return GlobalConstants.kListTextColor
                    })
                    txt.colorP              = Qt.binding(function(){
                        if (optionRoot.focused && CommonUtils.isEqual(routeBtnObjArr[index] , currentItem))
                            return GlobalConstants.kBasicBackgoundColor
                        return GlobalConstants.kListTextColor
                    })
                    txt.text                     = routeBtnTxtArr[index]
                    txt.font.family              = GlobalConstants.kFontFamily
                    txt.font.pixelSize           = Resource.pixelToDp(32)
                    txt.anchors.centerIn         = undefined
                    txt.anchors.horizontalCenter = box.horizontalCenter
                    txt.y                        = Qt.binding(function(){return ((box.height - txt.contentHeight)/2)+3})
                    routeBtnObjArr[index]        = this
                }

                function execButton() {
                    switch (index) {
                    case routeBtnEnum.Summary:
                        sendEvent(MapModeOptionViewEvent.RequestRouteSummary)
                        break

                    case routeBtnEnum.Cancel:
                        sendEvent(MapModeOptionViewEvent.CancelRouteGuidance)
                        break
                    }
                    close()
                }

                onButtonPressed: {
                    csw.forceLostFocus()
                }

                onButtonReleased:{
                }

                onButtonClicked: {
                    execButton()
                }
            }
         }
    }


    Csw {
        id: csw

        section: Csw.Section_Dialog
        sequence: 0
        enable: optionRoot.visible

        onFocused: {
            optionRoot.focused = true
            currentIndex = -1
            currentIndex = 0
        }

        onLostFocusd: {
            optionRoot.focused = false
            currentIndex = -1
        }

        onSpinCwPressed: {
            if (currentIndex < cswObjArrLength - 1) {
                ++currentIndex
            }
        }

        onSpinCcwPressed: {
            if (0 < currentIndex) {
                --currentIndex
            }
        }

        onLeftPressed: selected()
        onRightPressed: selected()
        onSelected: {
            var item = currentItem
            if (CommonUtils.isNull(item))
                return
            item.execButton()
        }
    }
    GestureArea {
        id: gestureArea
        anchors.fill: parent
        height: parent.height + GlobalConstants.kScreenZone3Height
        enabled: false
        onIssued: {
            sendEvent(MapModeOptionViewEvent.CaptureScreen)
        }
    }
}
