import QtQuick 2.0
import "../../view"
import "../../component"
import MapControlViewEvent 1.0
import SceneGraphRendering 1.0
import MapViewMode 1.0
import Csw 1.0
import "../../GlobalConstants.js" as GlobalConstants
import "../../view/util/CommonUtils.js" as CommonUtils

PopupView {
    id: root

    layout: MapControlViewLayout {}

    anchors.fill: parent
    mouseArea.enabled: false
    background.visible: false
    fader.fadingOpacity: 0
    property int lastMapViewMode
    property alias address: addressNameText.text
    property string areaName
    property string roadName
    property bool isSimulationMode: mapView.moveMode === Renderer.Simulator
    property int lastMapViewLevel: mapView.viewLevel
    property bool pinchMode: false
    property bool isOnline: false

    function setInvisible() {
        root.visible = false
    }

    function updateMapScale(level) {
        mapScaleControl.viewLevel = level
    }

    Timer {
        id: addrTimer
        interval: 0
        running: false
        repeat: false
        onTriggered: {
            if (isOnline) {
                root.sendEvent( MapControlViewEvent.RequestSearch)
            }
        }
    }

    function __onMapTouch() {
       addrTimer.stop()
       addrTimer.interval = 500
       addrTimer.start()
    }

    function __onResult(result) {
        if ((typeof result.addressInfo === "undefined")          ||
           (typeof result.addressInfo.fullAddress === "undefined")) {
            roadName =  areaName
        } else {
            roadName = result.addressInfo.fullAddress
        }
    }

    onMediated: {
        sendEvent(MapControlViewEvent.Mediated,{'mapView':mapView})
    }

    onActivated: {
        sendEvent(MapControlViewEvent.Activated,{'lastMapViewMode':lastMapViewMode})
        if (!isSimulationMode) {
            sendEvent(MapControlViewEvent.RegistCSW, [crossHairCsw])
            sendEvent(MapControlViewEvent.SelectCSW, crossHairCsw)
        }
        areaName = mapView.areaName
    }
    onDeactivated: {
        sendEvent(MapControlViewEvent.SaveMapViewLevel, lastMapViewLevel)
        sendEvent(MapControlViewEvent.Deactivated)
    }

    Rectangle {
        id: addressName
        width: parent.width; height: 70
        color: '#222226'
        opacity: 0.86
        visible: lastMapViewMode == MapViewMode.Normal || lastMapViewMode == MapViewMode.Navi/* || lastMapViewMode == MapViewMode.Simul*/
        Text {
            id: addressNameText
            anchors.centerIn: parent
            font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(30) }
            color: '#ffffff'
            text: isOnline ? roadName : areaName// 0 < roadName.length ? roadName : areaName
        }
    }

    Connections {
        target: mapView
        onObjectHitting: {
        }
        onViewLevelChanged: {
            mapScaleControl.viewLevel = viewLevel
        }
        onTouchUp: {
            pinchMode = false
        }
        onTouchMove: {
            pinchMode = true
        }
        onTouchDown: {
            pinchMode = true
        }
        onCenterChanged: {
            __onMapTouch()
        }
        onAreaNameChanged: {
            root.areaName = areaName
        }
    }

    CrossHair {
        id: crossHair

        scale: layout.scale
        parent: mapView
        visible: mapScaleControl.visible && mapView.moveMode !== Renderer.Simulator
        Csw {
            id: crossHairCsw
            enable: parent.visible
            enableKeyProcess: true
            section: Csw.Section_MapCursor
            sequence: 0
            onFocused: enableKeyProcess = true
            onLostFocusd: enableKeyProcess = false

            function onKeyFunction(key) {
                switch (key) {
                case Csw.Up:
                    sendEvent(MapControlViewEvent.MoveMap, {'y':-1})
                    break
                case Csw.Down:
                    sendEvent(MapControlViewEvent.MoveMap, {'y':1})
                    break
                case Csw.Left:
                    sendEvent(MapControlViewEvent.MoveMap, {'x':-1})
                    break
                case Csw.Right:
                    sendEvent(MapControlViewEvent.MoveMap, {'x':1})
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
                sendEvent(MapControlViewEvent.ResetTimer)
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
            // sendEvent(MapControlViewEvent.MapScaleUpClicked, level);  // TMCRSM-550 (From Bosch Issue) ,  it must consider if the view's 3d angle is more than 3d max angle tilt  2018.3.20
                                                                        //  TMCRSM-2716 (From Renault), do hold user's tilt on while controlling map, so this codes get dropped
            sendEvent(MapControlViewEvent.SaveMapViewLevel, level)
        }
        sendEvent(MapControlViewEvent.ResetTimer)
    }
    function setMapScaleDown() {
        var level = mapScaleControl.viewLevel

        if (level >= 1) {
            level = level - 1

            setMapViewLevel(level)
            sendEvent(MapControlViewEvent.SaveMapViewLevel, level)
        }
        sendEvent(MapControlViewEvent.ResetTimer)
    }

    MapScaleControl {
        id: mapScaleControl

        x: layout.mapScaleX
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        sectionId: Csw.Section_Map

        visible: true

        onScaleUpClicked: {
            setMapScaleUp()
        }
        onScaleDownClicked: {
            setMapScaleDown()
        }
        onReturnClicked: {
            if (mapView.moveMode === Renderer.Simulator)
                sendEvent(MapControlViewEvent.RestoreSimulationViewButtonClicked)
            else
                sendEvent(MapControlViewEvent.MapScaleReturnButtonClicked,{mapView:mapView})
        }
        onResetTimer: {
            sendEvent(MapControlViewEvent.ResetTimer)
        }

        Component.onCompleted: viewLevel = mapView.viewLevel
    }
}
