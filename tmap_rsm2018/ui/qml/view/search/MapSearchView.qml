import QtQuick 2.0
import ViewRegistry 1.0
import CommonConstants 1.0
import ZoneConstants 1.0
import SearchConstants 1.0
import MapSearchViewEvent 1.0
import SceneGraphRendering 1.0
import Csw 1.0

import "../"
import "../../view/map"
import "../../GlobalConstants.js" as GlobalConstants
import "../../view/util/CommonUtils.js" as CommonUtils

// MapControlView와 동일하나 timer 처리가 없다.

PopupView {
    id: root

    property var caller: null
    property var owner: null
    property bool isOnline: false
    property bool isGadget: false

    property string areaName
    property string areaNameOnline
    property int type
    property string menuText
    property string command
    property int mapX
    property int mapY
    property int curX
    property int curY

    layout: MapSearchViewLayout {}

    background.visible: false
    fader.fadingOpacity: 0
    mouseArea.enabled: false

    Timer {
        id: addrTimer
        interval: 10
        running: false
        repeat: false
        onTriggered: {
            if (isOnline) {
                root.sendEvent( MapSearchViewEvent.RequestSearch)
            }
        }
    }

    property alias address: addressNameText.text
    function __onMapTouch() {
        addrTimer.stop()
        addrTimer.interval = 500
        addrTimer.start()
    }

    function __onResult(result) {
       if ((typeof result.addressInfo === "undefined")          ||
           (typeof result.addressInfo.fullAddress === "undefined")) {
           areaNameOnline = areaName
           return
       }
       areaNameOnline = result.addressInfo.fullAddress
    }

    function retranslateUi() {
        selectSoftkeyMenu()
    }

    function selectSoftkeyMenu() {
        if (caller === CommonConstants.objectName(ViewRegistry.RouteSettingView)) {
            var menu = menuText
            switch (type) {
            case 0: menu = qsTr('출발지로'); break
            case 1: menu = qsTr('경유지로'); break
            case 2: menu = qsTr('목적지로'); break
            }

            sendEvent(MapSearchViewEvent.SelectSoftkeyIndexMenu, {'index':type, 'menu':menu})
        } else if (caller === CommonConstants.objectName(ViewRegistry.BookmarkPageView) ||
                   caller === CommonConstants.objectName(ViewRegistry.NaviMenuView)) {
            sendEvent(MapSearchViewEvent.SelectSoftkeyIndexMenu, {
                          'index':MapSearchViewEvent.AddToBookmark,
                          'menu':qsTr("즐겨찾기 추가")})
        } else if (caller === 'Interface') {
            if (command === 'ShowAdjustCurrentLocationScreen') {
                sendEvent(MapSearchViewEvent.SelectSoftkeyIndexMenu,
                            {
                              'index':MapSearchViewEvent.RequestRoute,
                              'menu':qsTr("목적지로")
                            })
            }
        }
    }
    function closeView() {
        close()
    }
    function enableMapMove(enable) {
        mouseArea.enabled = !enable
    }

    onMediated: {
        sendEvent(MapSearchViewEvent.Mediated, {mapView:mapView})
        if (caller === 'Interface' && command === 'ShowAdjustCurrentLocationScreen') {
            curX = mapX
            curY = mapY
        } else {
            curX = mapX = mapView.center.x
            curY = mapY = mapView.center.y
        }

    }
    onActivated: {
        sendEvent(MapSearchViewEvent.RegistCsw, [crossHairCsw])
        sendEvent(MapSearchViewEvent.SelectCsw, crossHairCsw)
        sendEvent(MapSearchViewEvent.Activated)
        selectSoftkeyMenu()
        root.areaName = mapView.areaName
        mapView.viewMode = Renderer.NorthBound
        mapView.viewLevel = mapScaleControl.viewLevel
        if (0 < mapX && 0 < mapY) {
            sendEvent(MapSearchViewEvent.MoveMapByPos, {x:mapX, y:mapY, anim:false})
        }
    }
    onDeactivated: {
        sendEvent(MapSearchViewEvent.Deactivated)
        mapX = mapView.center.x
        mapY = mapView.center.y
    }

    function updateMapScale(level) {
        mapScaleControl.viewLevel = level
    }

    Rectangle {
        id: addressName
        width: parent.width; height: 70
        color: '#222226'
        opacity: 0.86
        Text {
            id: addressNameText
            anchors.centerIn: parent
            font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(30) }
            color: '#ffffff'
            text: isOnline ? areaNameOnline : areaName
        }
    }

    Connections {
        target: mapView

        onViewLevelChanged: {
            if (visible) {
                mapScaleControl.viewLevel = viewLevel
            }
        }
        onCenterChanged: {
            if (visible) {
                __onMapTouch()
            }
            curX = mapView.center.x
            curY = mapView.center.y
        }
        onAreaNameChanged: {
            if (visible) {
                root.areaName = areaName
            }
        }
    }

    CrossHair {
        id: crossHair

        visible: !isGadget
        scale: layout.scale
        parent: mapView
        Csw {
            id: crossHairCsw
            objectName: 'mapSearchCrossHair'
            enable: parent.visible
            enableKeyProcess: true
            section: Csw.Section_MapCursor
            sequence: 0
            onFocused: enableKeyProcess = true
            onLostFocusd: enableKeyProcess = false

            function onKeyFunction(key) {
                switch (key) {
                case Csw.Up:
                    sendEvent(MapSearchViewEvent.MoveMap, {'y':-1})
                    break
                case Csw.Down:
                    sendEvent(MapSearchViewEvent.MoveMap, {'y':1})
                    break
                case Csw.Left:
                    sendEvent(MapSearchViewEvent.MoveMap, {'x':-1})
                    break
                case Csw.Right:
                    sendEvent(MapSearchViewEvent.MoveMap, {'x':1})
                    break
                case Csw.SpinCcw:
                    setMapScaleDown()
                    break
                case Csw.SpinCw:
                    setMapScaleUp()
                    break
                case Csw.Select:
                    forceNextSection()
                    break
                default:
                    return
                }
                keyProcessed(true)
            }
        }
    }

    function setMapViewLevel(level) {
        mapView.setViewLevel(level, true)
    }
    function setMapScaleUp() {
        var level = mapScaleControl.viewLevel

        if (level < 12) {
            level = level + 1

            setMapViewLevel(level)
        }
    }
    function setMapScaleDown() {
        var level = mapScaleControl.viewLevel

        if (level >= 1) {
            level = level - 1

            setMapViewLevel(level)
        }
    }

    MapScaleControl {
        id: mapScaleControl

        sectionId: Csw.Section_MapCursor
        startSequence: Csw.Sequence_MapScalePlus
        x: layout.mapScaleX //0
        y: GlobalConstants.kScreenContentHeight + layout.mapScaleOffsetY - GlobalConstants.kScreenZone3Height

        visible: true

        onScaleUpClicked: {
            setMapScaleUp()
        }
        onScaleDownClicked: {
            setMapScaleDown()
        }
        onReturnClicked: {
            sendEvent(MapSearchViewEvent.MapScaleReturnButtonClicked, mapView)
        }
        Component.onCompleted: {
            viewLevel = mapView.viewLevel
        }
    }
}
