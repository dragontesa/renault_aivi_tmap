import QtQuick 2.1
import Constants 1.0
import SceneGraphRendering 1.0
import MapViewEvent 1.0
import MapControlViewEvent 1.0
import MapViewMode 1.0
import ViewRegistry 1.0
import Csw 1.0

import "../../component"
import "../../view"
import "../../view/util"
import "../../GlobalConstants.js" as GlobalConstants
import "../../view/util/CommonUtils.js" as CommonUtils
import WidgetConstants 1.0

View {
    id: root
    objectName: "MapView"
    focusName: "MapView"
    layout: MapViewLayout {}
    width: parent ? parent.width : undefined
    height: parent ? parent.height : undefined
    property alias renderer: renderer
    property bool isSimul: renderer.moveMode === Renderer.Simulator
    property var boschInfo : null
    property bool mute: false
    property bool volumeVisible: true

    // the below seven properties are used to display cluster map view's traffic info through notifications
    property bool isNaviOrAndo: true
    property bool isOnline: false
    property bool showTrafficInfo: true // from controller
    property bool enableTrafficInfo: false // when control mode
    property bool showTrafficInfoOnMap: false
    property bool showTrafficInfoOnRouteLine: false
    property bool showTrafficInfoOnRouteLineForce: false // 경로 요약 화면, 경로 상세 화면에서 설정
    property bool isSpeedResponseMapDisable: false
    property string lang
    property bool isRg: false
    property bool isMapForWidget: false
    property bool returnFromWidget: false
    property bool enableShowRoute: true

    function setMapForWidget(data) {
        var type = data['type']
        var size = data['size']
        var mapVisible = true
        var centerFactor = 0.67 // 0.75 is the default center factor of map view quick
        switch (type) {
        case WidgetConstants.SizeXXL:
            if ( renderer.viewMode === Renderer.BirdView ) {
                centerFactor = 0.67
            }
            else {
                centerFactor = 0.61
            }

            applyPadding(size.width, size.height, 0, 0, 0, 0)
            break
        case WidgetConstants.SizeXL:
            if ( renderer.viewMode === Renderer.BirdView ) {
                centerFactor = 0.67
            }
            else {
                centerFactor = 0.61
            }

            applyPadding(size.width, size.height, 0, 0, 0, 0)
            break
        case WidgetConstants.SizeLarge:
            mapVisible = false
            break
        case WidgetConstants.SizeMedium:
            mapVisible = false
            break
        case WidgetConstants.SizeSmall:
            mapVisible = false
            break
        }

        renderer.headUpCenterFactor = Qt.point(0.5, centerFactor);
//        console.debug( "headUpCenterFactor: " + renderer.headUpCenterFactor )

//        console.log('[MapView][setMapForWidget]size: ' + size.width + ', ' + size.height + ' mapVisible: ' + mapVisible)

        rgWrap.visible = false
        buttonWrap.visible = false

        //renderer.visible = mapVisible
        isMapForWidget = true
    }

    function setMapForApp() {        
        renderer.headUpCenterFactor = Qt.point(0.5, 0.75)

        renderer.x = 0
        renderer.y = 0
        renderer.width = Qt.binding(function(){ return rendererWrap.width });
        renderer.height = Qt.binding(function(){ return rendererWrap.height });

        rgWrap.visible = true
        buttonWrap.visible = true

        renderer.visible = true
        renderer.onResume()

        if (isMapForWidget) {
            isMapForWidget = false
            returnFromWidget = true
        }

        gpsState.x = Qt.binding(function() {
            if (summaryTbtVisible) {
                return layout.gpsStateXOnMiddle
            }
            else if (highwayTbtVisible) {
                return layout.gpsStateXOnBottom
            }
            else {
                return layout.gpsStateXOnRight
            }
        })
        gpsState.y = Qt.binding(function() {
            if (highwayTbtVisible) {
                return layout.gpsStateYOnBottom
            }
            else {
                return layout.gpsStateY
            }
        })
    }

    function changeGpsStatePosition( x ) {
        gpsState.x = x
    }

    onIsNaviOrAndoChanged: {
        sendEvent( MapViewEvent.IsNaviOrAndoChanged, isNaviOrAndo )
    }

    onIsOnlineChanged: {
        sendEvent( MapViewEvent.IsOnlineChanged, isOnline )
    }

    onShowTrafficInfoChanged: {
        sendEvent( MapViewEvent.ShowTrafficInfoChanged, showTrafficInfo )
    }

    onEnableTrafficInfoChanged: {
        sendEvent( MapViewEvent.EnableTrafficInfoChanged, enableTrafficInfo )
    }

    onShowTrafficInfoOnMapChanged: {
        sendEvent( MapViewEvent.ShowTrafficInfoOnMapChanged, showTrafficInfoOnMap )
    }

    onShowTrafficInfoOnRouteLineChanged: {
        sendEvent( MapViewEvent.ShowTrafficInfoOnRouteLineChanged, showTrafficInfoOnRouteLine )
    }

    onShowTrafficInfoOnRouteLineForceChanged: {
        sendEvent( MapViewEvent.ShowTrafficInfoOnRouteLineForceChanged, showTrafficInfoOnRouteLineForce )
    }

    property bool enableCallout: true
    onEnableCalloutChanged: {
        renderer.enableCallout = enableCallout
        console.log('enableCallout changed = ' + enableCallout)
    }

    property int skyState: 0
    onSkyStateChanged: {
        renderer.skyState = skyState
        sendEvent( MapViewEvent.SkyStateChanged, skyState )
    }
    property bool nightMode: false
    onNightModeChanged: {
        renderer.nightMode = nightMode
        sendEvent( MapViewEvent.NightModeChanged, nightMode )
    }

    property bool showGpsMapmatch: false  // // engineer mode GPS Mapmatching timestamp
    property bool summaryTbtVisible: false
    property bool highwayTbtVisible: false
    property bool highwayTbtListVisible: false

    /*!
        \brief MapViewMode 변화

        normal => control => normal
        normal => control => inquiry => normal
        normal => control => inquiry => simulation => (cancel) => inquiry => normal
        normal => control => inquiry => simulation => control => simulation => (cancel) => inquiry => normal
        normal => inquiry => navi => control => navi => (cancel) => normal


        \brief MapViewMode 복원

        MapControlView 및 조회 화면 close 후 =>
        Activated => RestoreMapViewMode => ApplicationMediator => ApplyMapViewMode
    */

    function setNormalMode(reason) {
        layout.applyMapViewMode( MapViewMode.Normal )

        renderer.moveMode = Renderer.Navigation
        console.log( "[GlobalMap]setNormalMode moveMode: " + renderer.moveMode)
        enableTrafficInfo = false
        isNaviOrAndo = true

        if ( reason === MapViewMode.MapViewCreated ) {
            sdiViewWrap.visible = false
        }
        else {
            sdiViewWrap.visible = true
        }

        speedMeterViewWrap.visible = true
        tbtViewWrap.visible = false
        laneViewWrap.visible = false
        crossImageViewWrap.visible = false
        highwayTbtViewWrap.visible = false
        summaryTbtViewWrap.visible = false
        hiPassLaneViewWrap.visible = true
        busLaneViewWrap.visible = false
        trafficBarWrap.visible = false

        rerouteButton.visible = false
        skipViaButtonWrap.visible = false
    }

    function setMapControlMode(reason) {
        layout.applyMapViewMode( MapViewMode.MapControl )

        renderer.moveMode = Renderer.Normal
        console.log( "[GlobalMap]setMapControlMode moveMode: " + renderer.moveMode)
        // 통신 상태일 때에만 실시간 교통 정보를 표시한다.
        // 17.07.19 ymhong.
        enableTrafficInfo = true
        isNaviOrAndo = false
        // 17.9.15 hskim change for trafficbar to manage isOnlineStatus
        trafficBarWrap.visible = false

        sdiViewWrap.visible = false
        speedMeterViewWrap.visible = false
        tbtViewWrap.visible = false
        laneViewWrap.visible = false
        crossImageViewWrap.visible = false
        highwayTbtViewWrap.visible = false
        summaryTbtViewWrap.visible = false
        hiPassLaneViewWrap.visible = false
        busLaneViewWrap.visible = false

        rerouteButton.visible = false
        skipViaButtonWrap.visible = false
    }

    function setMapInquiryMode(reason) {
        layout.applyMapViewMode( MapViewMode.MapInquiry )

        // 조회 모드의 level은 조회화면에서 renderer의 boundary를 직접 조정하기 때문에 따로 설정하지 않는다.        
        renderer.viewMode = Renderer.NorthBound
        renderer.moveMode = Renderer.Normal
        console.log( "[GlobalMap]setMapInquiryMode moveMode: " + renderer.moveMode)

        enableTrafficInfo = false
        isNaviOrAndo = false

        sdiViewWrap.visible = false
        speedMeterViewWrap.visible = false
        tbtViewWrap.visible = false
        laneViewWrap.visible = false
        crossImageViewWrap.visible = false
        highwayTbtViewWrap.visible = false
        summaryTbtViewWrap.visible = false
        hiPassLaneViewWrap.visible = false
        busLaneViewWrap.visible = false
        trafficBarWrap.visible = false

        rerouteButton.visible = false
        skipViaButtonWrap.visible = false
    }

    function setNaviMode(reason) {
        layout.applyMapViewMode( MapViewMode.Navi )

        renderer.moveMode = Renderer.Navigation
        console.log( "[GlobalMap]setNaviMode moveMode: " + renderer.moveMode)

        enableTrafficInfo = false
        isNaviOrAndo = true

        sdiViewWrap.visible = true
        speedMeterViewWrap.visible = true
        tbtViewWrap.visible = true
        laneViewWrap.visible = true
        crossImageViewWrap.visible = true
        highwayTbtViewWrap.visible = true
        summaryTbtViewWrap.visible = true
        hiPassLaneViewWrap.visible = true
        busLaneViewWrap.visible = true
        // 17.9.15 hskim change for trafficbar to manage isOnlineStatus
        //trafficBarWrap.visible = isOnline  // 통신모드에서만 교통정보바 표시 by lckxx, 2017.8.7
        trafficBarWrap.visible = true

        rerouteButton.visible = true
        skipViaButtonWrap.visible = true
    }

    // map control

    //  조회 => 주행 level 10

    function setSimulMode(reason) {
        layout.applyMapViewMode( MapViewMode.Simul )

        renderer.moveMode = Renderer.Simulator
        console.log( "[GlobalMap]setSimulMode moveMode: " + renderer.moveMode)

        sdiViewWrap.visible = true
        speedMeterViewWrap.visible = true
        tbtViewWrap.visible = true
        laneViewWrap.visible = true
        crossImageViewWrap.visible = true
        highwayTbtViewWrap.visible = true
        summaryTbtViewWrap.visible = true
        hiPassLaneViewWrap.visible = true
        busLaneViewWrap.visible = true
        trafficBarWrap.visible = true

        rerouteButton.visible = false
        skipViaButtonWrap.visible = false

        enableTrafficInfo = false
        isNaviOrAndo = true//false
    }

    function setSimulMapMode(reason) {
        layout.applyMapViewMode( MapViewMode.MapControl )

        sdiViewWrap.visible = false
        tbtViewWrap.visible = false
        laneViewWrap.visible = false
        speedMeterViewWrap.visible = false
        crossImageViewWrap.visible = false
        highwayTbtViewWrap.visible = false
        summaryTbtViewWrap.visible = false
        hiPassLaneViewWrap.visible = false
        busLaneViewWrap.visible = false
        trafficBarWrap.visible = false

        rerouteButton.visible = false
        skipViaButtonWrap.visible = false

        enableTrafficInfo = true
        isNaviOrAndo = false
    }

    // 17.9.18 hskim
    function setMenuNaviMode(reason) {
        layout.applyMapViewMode( MapViewMode.MenuNavi )

        renderer.moveMode = Renderer.Navigation
        console.log( "[GlobalMap]setMenuNaviMode moveMode: " + renderer.moveMode)

        sdiViewWrap.visible = true
        trafficBarWrap.visible = false
        speedMeterViewWrap.visible = true
        tbtViewWrap.visible = false
        laneViewWrap.visible = false
        crossImageViewWrap.visible = false
        highwayTbtViewWrap.visible = false
        summaryTbtViewWrap.visible = false
        hiPassLaneViewWrap.visible = false
        busLaneViewWrap.visible = false

        rerouteButton.visible = false
        skipViaButtonWrap.visible = false
        isNaviOrAndo = false
    }

    function setMapMode(mode) {
        renderer.viewMode = mode
    }

    function showMapModeDlg(mode) {
            var viewMode = null
            if (mode == "Normal") {
                viewMode = "Driving"
            } else if(mode == "Navi") {
                viewMode = "Directions"
            }
            var data = {
                viewMode: viewMode
            }
            sendEvent( MapViewEvent.CreateMapModeOptionPopup, data )
    }

    function closeMapModeDlg() {
        sendEvent( MapViewEvent.CloseMapModeOptionPopup )
    }

    function restoreDriveMapModeAndLevel( mapModeSetting ) {
        var rendererMapMode = Renderer.BirdView
        if ( mapModeSetting === 0 ) {
            rendererMapMode = Renderer.BirdView
        }
        else if ( mapModeSetting === 1 ) {
            rendererMapMode = Renderer.HeadUp
        }
        else if ( mapModeSetting === 2 ) {
            rendererMapMode = Renderer.NorthBound
        }

        setMapMode( rendererMapMode )
    }

    function getBoschInfo() {
        if ( boschInfo == null ) {
            boschInfo = boschInfoComp.createObject(boschInfoWrap);
        }
        return boschInfo
    }

    function toggleBoschInfo() {
        getBoschInfo()
        boschInfo.visible = !boschInfo.visible
    }

    function applyPadding(width, height, l, r, t, b) {
        if (width !== null) {
            renderer.width = width
        }
        if (height !== null) {
            renderer.height = height
        }

        renderer.y = t
        renderer.height -= t
        renderer.height -= b
        renderer.x = l
        renderer.width -= l
        renderer.width -= r
    }

    function updatePadding(data) {
        var l = (data !== undefined && data.hasOwnProperty("l")) ? data["l"] : 0
        var r = (data !== undefined && data.hasOwnProperty("r")) ? data["r"] : 0
        var t = (data !== undefined && data.hasOwnProperty("t")) ? data["t"] : 0
        var b = (data !== undefined && data.hasOwnProperty("b")) ? data["b"] : 0

        applyPadding(rendererWrap.width, rendererWrap.height, l, r, t, b);
    }

    function updateMap(routeId) {
        console.log('[MapView][updateMap]')

        renderer.drawRouteAll(routeId, renderer.width, renderer.height, 0)
    }
    function pause() {
        renderer.onPause()
    }
    function resume() {
        renderer.onResume()
    }

    function setSkipViaButtonVisible(visible) {
        skipViaButton.visible = visible
    }

    function drawMarkAll(rect) {
        console.log('[MapView][drawMarkAll]')

        var l = rect.left
        var t = rect.top
        var r = rect.right
        var b = rect.bottom
        var paddingY = layout.mapPaddingY
        var rw = rect.width
        var rh = rect.height

        var w = renderer.width
        var h = renderer.height - paddingY

        renderer.drawMBRAll(w, h, l, t, r, b)

        console.log(w + "x" + h + "("+rw+"x"+rh+"), " + l + "," + t + "," + r + "," + b)
    }

    function setSkyboxEnable(enable) {
        renderer.skyVisibility = enable
    }

    function showTrafficInfoOnRouteLine(show) {
        renderer.trafficInfoOnRoute = show
    }

    function showTestKeypad() {
        console.log( "#1 showTestKeypad" )
        var comp = Qt.createComponent("../dbg/TestKeypad.qml");
        var obj = comp.createObject(root, {y: 30});
        obj.owner = root;
    }

    function onMapTouched() {
        mapScaleControl.init()
        root.sendEvent( MapViewEvent.RendererTouched )
        root.sendEvent( MapViewEvent.MapTouched )
    }

    function setMapViewLevel(level) {
        scaleTimer.stop()
        root.sendEvent(MapViewEvent.SpeedResponseMapDisable)
        renderer.setViewLevel(level, true)
        scaleTimer.start()
        isSpeedResponseMapDisable = true
    }
    function setMapViewLevelWithNoAnim(level) {
        renderer.setViewLevel(level, false)
    }

    onMediated: {
        sendEvent(MapViewEvent.Mediated, renderer)
    }

    onActivated: {
        sendEvent(MapViewEvent.Activated, {'isGadget':isMapForWidget})
        sendEvent(MapViewEvent.RegistCsw, mapScaleControl.csws)
        sendEvent(MapViewEvent.RendererSizeChanged, {w:renderer.width, h:renderer.height})

        var args = [skipViaCsw, rerouteCsw, volumeCsw, mapCsw]
        sendEvent(MapViewEvent.RegistCsw, args)
    }

    onDeactivated: {
        var args = [skipViaCsw, rerouteCsw, volumeCsw, mapCsw]
        sendEvent(MapViewEvent.RemoveCsw, {'csws':args})
        mapScaleControl.init()
        rerouteButton.state = 'normal'
        volumeButton.state = 'normal'
        skipViaButton.state = 'normal'
    }

    Rectangle {
        id: rendererWrap
        anchors.fill: parent        
        color: 'black'

        Renderer {
            id: renderer
            objectName: "renderer"
            width: parent.width
            height: parent.height

            moveMode: Renderer.Navigation
            showRoute: enableShowRoute && (isRg || !isMapForWidget)

            onStartedToRender: {
                sendEvent(MapViewEvent.StartedToRender)
            }

//            onXChanged: {
//                console.perf( "[GlobalMap]x: " + renderer.x)
//            }

//            onYChanged: {
//                console.perf( "[GlobalMap]y: " + renderer.y)
//            }

            onWidthChanged: {
                console.perf( "[GlobalMap]width: " + renderer.width)
                sendEvent(MapViewEvent.RendererSizeChanged, {w:width, h:height})
            }
            onHeightChanged: {
                console.perf( "[GlobalMap]height: " + renderer.height)
                sendEvent(MapViewEvent.RendererSizeChanged, {w:width, h:height})
            }

            property bool isMapTouched: enableTrafficInfo

            trafficInfo: (showTrafficInfoOnMap && (isMapTouched || isNaviOrAndo)) && isOnline
            trafficInfoOnRoute: (isMapTouched || showTrafficInfoOnRouteLine || showTrafficInfoOnRouteLineForce) && isOnline

            downloadRouteTile: true
            showRouteObject: Renderer.RouteObjectDefault & ~Renderer.RouteObjectMainRoadName

            Component.onDestruction: {
                root.sendEvent( MapViewEvent.RendererOnDestruction )
            }

            onMoveModeChanged: {
                console.log( "[GlobalMap]moveMode: " + renderer.moveMode)
                root.sendEvent(MapViewEvent.MoveModeChanged, renderer.moveMode)
            }
            onTouchDown: {
                root.sendEvent( MapViewEvent.RendererTouched )
                root.sendEvent( MapViewEvent.MapTouched )
            }
            onTouchMove: {
                root.sendEvent( MapViewEvent.MapDragged )
            }

            onAreaNameChanged: {
                root.sendEvent( MapViewEvent.AreaNameChanged )
            }
            onViewModeChanged: {
                //console.log( "[GlobalMap]viewMode: " + viewMode)
                root.sendEvent(MapViewEvent.ViewModeChanged, viewMode)
            }
            onViewLevelChanged: {
                if (isMapForWidget) {
                    if ( renderer.viewMode === Renderer.BirdView ) {
                        if (viewLevel > 10) {
                            var centerFactor = 0.61
                        }
                        else {
                            centerFactor = 0.67
                        }
                        renderer.headUpCenterFactor = Qt.point(0.5, centerFactor);
                    }
                }

                root.sendEvent( MapViewEvent.ViewLevelChanged, viewLevel)
            }
            onTiltAngleChanged: {
                boschInfo = root.getBoschInfo()
                boschInfo.tiltAngle = tiltAngle
            }
            onObjectHitting: {
                 root.sendEvent(MapViewEvent.ObjectHitting, {hitTestResult: hitTestResult} )
            }
            onHitting: {
                root.sendEvent(MapViewEvent.Hitting)
            }
        }
    }

    Item {
        id: rgWrap
        anchors.fill: rendererWrap

        onVisibleChanged: {
            console.log( '[RG]rgWrap visible: ' + visible )
        }

        Item {
            id: tbtViewWrap
            objectName: 'tbtViewWrap'
            width: childrenRect.width
            height: childrenRect.height
            visible: false
            onVisibleChanged: {
                if ( visible ) {
                    sendEvent( MapViewEvent.CreateRgPopup, {viewId:ViewRegistry.TbtView, parent:tbtViewWrap} )
                }
                else {
                    sendEvent( MapViewEvent.CloseRgPopup, {viewId:ViewRegistry.TbtView} )
                }
            }
        }

        Item {
            id: summaryTbtViewWrap
            objectName: 'summaryTbtViewWrap'
            anchors.right: parent.right
            width: childrenRect.width
            height: childrenRect.height
            visible: false
            onVisibleChanged: {
                if ( visible ) {
                    sendEvent( MapViewEvent.CreateRgPopup, {viewId:ViewRegistry.SummaryTbtView, parent:summaryTbtViewWrap} )
                }
                else {
                    sendEvent( MapViewEvent.CloseRgPopup, {viewId:ViewRegistry.SummaryTbtView} )
                }
            }
        }

        Item {
            id: laneViewWrap
            objectName: 'laneViewWrap'
            width: childrenRect.width
            height: childrenRect.height
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            anchors.bottomMargin: GlobalConstants.kScreenZone3Height + layout.laneViewBottomMargin

            visible: false
            onVisibleChanged: {
                if ( visible ) {
                    sendEvent( MapViewEvent.CreateRgPopup, {viewId:ViewRegistry.LaneView, parent:laneViewWrap} )
                }
                else {
                    sendEvent( MapViewEvent.CloseRgPopup, {viewId:ViewRegistry.LaneView} )
                }
            }
        }

        Item {
            id: sdiViewWrap
            objectName: 'sdiViewWrap'
            y: layout.sdiViewY
            width: layout.sdiViewWidth
            height: layout.sdiViewHeight
            visible: false
            onVisibleChanged: {
                console.log( "#1 sdi visible:" + visible )
                if ( visible ) {
                    sendEvent( MapViewEvent.CreateRgPopup, {viewId:ViewRegistry.SdiView, parent:sdiViewWrap} )
                }
                else {
                    sendEvent( MapViewEvent.CloseRgPopup, {viewId:ViewRegistry.SdiView} )
                }
            }
        }

        Item {
            id: crossImageViewWrap
            objectName: 'crossImageViewWrap'
            y: layout.crossImageViewY
            width: childrenRect.width
            height: childrenRect.height
            anchors.right: parent.right
            visible: false
            onVisibleChanged: {
                if ( visible ) {
                    sendEvent( MapViewEvent.CreateRgPopup, {viewId:ViewRegistry.CrossImageView, parent:crossImageViewWrap} )
                }
                else {
                    sendEvent( MapViewEvent.CloseRgPopup, {viewId:ViewRegistry.CrossImageView} )
                }
            }
        }

        Item {
            id: speedMeterViewWrap
            objectName: 'speedMeterViewWrap'
            y: layout.speedMeterViewY
            width: layout.speedMeterViewWidth
            height: layout.speedMeterViewHeight
            visible: false
            onVisibleChanged: {
                if ( visible ) {
                    sendEvent( MapViewEvent.CreateRgPopup, {viewId:ViewRegistry.SpeedMeterView, parent:speedMeterViewWrap} )
                } else {
                    sendEvent( MapViewEvent.CloseRgPopup, {viewId:ViewRegistry.SpeedMeterView} )
                }
            }
        }

        Item {
            id: highwayTbtViewWrap
            objectName: 'highwayTbtViewWrap'
            x: layout.highwayTbtViewX
            y: layout.highwayTbtViewY
            height: 86 * 5 + 26
            visible: false
            onVisibleChanged: {
                if ( visible ) {
                    sendEvent( MapViewEvent.CreateRgPopup, {viewId:ViewRegistry.HighwayTbtView, parent:highwayTbtViewWrap} )
                }
                else {
                    sendEvent( MapViewEvent.CloseRgPopup, {viewId:ViewRegistry.HighwayTbtView} )
                }
            }
        }

        Item {
            id: hiPassLaneViewWrap
            objectName: 'hiPassLaneViewWrap'
            width: childrenRect.width
            height: childrenRect.height
            anchors.bottom: parent.bottom
            anchors.bottomMargin: GlobalConstants.kScreenZone3Height + layout.hiPassLaneBottomMargin
            anchors.horizontalCenter: parent.horizontalCenter
            visible: false
            onVisibleChanged: {
                if ( visible ) {
                    sendEvent( MapViewEvent.CreateRgPopup, {viewId:ViewRegistry.HiPassLaneView, parent:hiPassLaneViewWrap} )
                }
                else {
                    sendEvent( MapViewEvent.CloseRgPopup, {viewId:ViewRegistry.HiPassLaneView} )
                }
            }
        }

        Item {
            id: busLaneViewWrap
            objectName: 'busLaneViewWrap'
            width: childrenRect.width
            height: childrenRect.height
            anchors.bottom: parent.bottom
            anchors.bottomMargin: GlobalConstants.kScreenZone3Height + layout.hiPassLaneBottomMargin
            anchors.horizontalCenter: parent.horizontalCenter
            visible: false
            onVisibleChanged: {
                if ( visible ) {
                    sendEvent( MapViewEvent.CreateRgPopup, {viewId:ViewRegistry.BusLaneView, parent:busLaneViewWrap} )
                }
                else {
                    sendEvent( MapViewEvent.CloseRgPopup, {viewId:ViewRegistry.BusLaneView} )
                }
            }
        }

        Item {
            id: boschInfoWrap
            width: 100
            height: 50
            visible: true
            z: 1
            Component {
                id: boschInfoComp
                BoschInfo {
                    objectName: 'boschInfo'
                    anchors.fill: boschInfoWrap
                    visible: false
                }
            }
        }

        Item {
            id: trafficBarWrap
            objectName: 'trafficBarWrap'
            width: childrenRect.width
            height: childrenRect.height
            anchors.bottom: parent.bottom
            anchors.bottomMargin: layout.trafficBarBottomMargin/*GlobalConstants.kScreenZone3Height + 10*/
            anchors.right: parent.right
            anchors.rightMargin: 10 // 13 | 16 | 13 + right edge margin 10
            visible: false
            onVisibleChanged: {
                if (visible) {
                    sendEvent( MapViewEvent.CreateRgPopup, {viewId:ViewRegistry.TrafficBarView, parent:trafficBarWrap} )
                }
                else {
                    sendEvent( MapViewEvent.CloseRgPopup, {viewId:ViewRegistry.TrafficBarView} )
                }
            }
        }
    }

    GpsState {
        id: gpsState
        x: {
            if (summaryTbtVisible) {
                return layout.gpsStateXOnMiddle
            }
            else if (highwayTbtVisible) {
                return layout.gpsStateXOnBottom
            }
            else {
                return layout.gpsStateXOnRight
            }
        }
        y: {
            if (highwayTbtVisible) {
                return layout.gpsStateYOnBottom
            }
            else {
                return layout.gpsStateY
            }
        }
        width: childrenRect.width
        height: childrenRect.height        
        objectName: 'gpsState'
        visible: true 
    }

    Item {
        id: buttonWrap
        anchors.fill: parent
        Item {
            id: skipViaButtonWrap
            width: layout.buttonSize
            height: layout.buttonSize
            anchors { bottom: volumeButton.top; bottomMargin: 10; right: parent.right; rightMargin: 10 }

            TImageButton {
                id: skipViaButton
                width: parent.width
                height: parent.height

                bg.anchors.fill: skipViaButton
                icon.anchors.fill: skipViaButton

                visible: false

                onVisibleChanged: {
                    console.log( "onVisibleChanged visible:" + visible )
                }

                bgSources: function (state) {
                    switch (state) {
                    case "normal": return "map_btn_bg_n.png"
                    case "pressed":return "map_btn_bg_p.png"
                    case "focused":return "map_btn_bg_f.png"
                            }
                        }
                iconSources: function (state) {
                    var postfix = lang === 'ko' ? ".png" : "_eng.png"
                    switch (state) {
                        case "normal":
                        case "pressed":
                            return "map_btn_waypoint" + postfix
                        case "focused":
                            return "map_btn_waypoint_f" + postfix
                    }
                }

                onClicked: {
                    root.sendEvent(MapViewEvent.SkipViaButtonClicked)
                    skipViaCsw.forceLostFocus()
                }

                Csw {
                    id: skipViaCsw
                    objectName: "skipVia"
                    section: Csw.Section_Map; sequence: Csw.Sequence_SkipVia; enable: skipViaButton.visible
                    onFocused: skipViaButton.state = "focused"
                    onLostFocusd: skipViaButton.state = "normal"
                    onSelected: root.sendEvent(MapViewEvent.SkipViaButtonClicked)
                }
            }
        }

        TImageButton {
            id: volumeButton
            width: layout.buttonSize
            height: layout.buttonSize

            bg.anchors.fill: volumeButton
            icon.anchors.fill: volumeButton

            anchors.bottom: parent.bottom
            anchors.bottomMargin: GlobalConstants.kScreenZone3Height + 10
            anchors.right: parent.right
            anchors.rightMargin: 10

            visible: volumeVisible && (renderer.moveMode != Renderer.Normal)
            bgSources: function (state) {
                switch (state) {
                case "normal": return "map_btn_bg_n.png"
                case "pressed":return "map_btn_bg_p.png"
                case "focused":return "map_btn_bg_f.png"
                        }
                    }
            iconSources: function (state) {
                var basename = (!root.mute) ? "map_btn_volume" : "map_btn_mute"
                switch (state) {
                    case "normal":
                    case "pressed":
                        return basename + ".png"
                    case "focused":
                        return basename + "_f.png"
                }
            }
            onClicked: {
                root.sendEvent(MapViewEvent.VolumeButtonClicked)
                volumeCsw.forceLostFocus()
            }

            Csw {
                id: volumeCsw
                objectName: "volume"
                section: Csw.Section_Map; sequence: Csw.Sequence_Volume; enable: volumeButton.visible;
                onFocused: volumeButton.state = "focused"
                onLostFocusd: volumeButton.state = "normal"
                onSelected: root.sendEvent(MapViewEvent.VolumeButtonClicked)
            }
        }

        TImageButton {
            id: rerouteButton
            width: layout.buttonSize
            height: layout.buttonSize

            bg.anchors.fill: rerouteButton
            icon.anchors.fill: rerouteButton

            anchors.bottom: parent.bottom
            anchors.bottomMargin: GlobalConstants.kScreenZone3Height + 10
            anchors.left: parent.left
            anchors.leftMargin: 10

            visible: renderer.moveMode == Renderer.Navigation
            bgSources: function (state) {
                switch (state) {
                case "normal": return "map_btn_bg_n.png"
                case "pressed":return "map_btn_bg_p.png"
                case "focused":return "map_btn_bg_f.png"
                }
            }
            iconSources: function (state) {
                switch (state) {
                case "normal":
                case "pressed":
                    return "map_btn_refresh.png"
                case "focused":
                    return "map_btn_refresh_f.png"
                }
            }
            onClicked: {
                root.sendEvent(MapViewEvent.ReRouteButtonClicked)
                rerouteCsw.forceLostFocus()
            }

            Csw {
                id: rerouteCsw
                objectName: "reroute"
                section: Csw.Section_Map; sequence: Csw.Sequence_Reroute; enable: rerouteButton.visible;
                onFocused: rerouteButton.state = "focused"
                onLostFocusd: rerouteButton.state = "normal"
                onSelected: root.sendEvent(MapViewEvent.ReRouteButtonClicked)
            }
        }

        Image {
            id: mapCswArea
            source: Resource.imageUrl('map_csw_f.png')
            anchors.centerIn: parent
            visible: false
            Csw {
                id: mapCsw
                objectName: 'mapCsw'
                section: Csw.Section_Map
                sequence: Csw.Sequence_Map
                onFocused: {
                    mapCswArea.visible = true
                }
                onLostFocusd: {
                    mapCswArea.visible = false
                }
                onSelected: {
                    onMapTouched()
                    forceLostFocus()
                }
            }
        }

        Timer {
            id: scaleTimer
            interval: 1000*3
            running: false
            repeat: false
            onTriggered: {
                root.sendEvent(MapViewEvent.SpeedResponseMapEnable)
                isSpeedResponseMapDisable = false
            }
        }

        MapScaleControl {
            id: mapScaleControl

            sectionId: Csw.Section_Map; startSequence: Csw.Sequence_MapScalePlus
//            undeletable: true

            //TMCRSM-1518 - [#주행중 맵크기 조절후 5초후에 운행모드로 되돌아오기]
            property bool forceSetMoveMode: false
            property var startTimer: null
            property int delayms: 1000*3
            Item {id: timerId;width: 0;height: 0}
            function init() {
                forceSetMoveMode = false
                killTimer()
                initButtonState()
            }

            function killTimer() {
                if (startTimer !== null) {
                    startTimer.stop()
                    startTimer = null
                }
            }

            function setScaleMode() {
                forceSetMoveMode = true
                console.log( "[GlobalMap]setScaleMode moveMode: " + renderer.moveMode)
                renderer.moveMode = Renderer.Normal
            }

            function setNavigationMode() {
                if (!forceSetMoveMode ||
                    renderer.moveMode !== Renderer.Normal) {
                    return
                }
                forceSetMoveMode = false
                console.log( "[GlobalMap]setNavigationMode moveMode: " + renderer.moveMode)
                renderer.moveMode = Renderer.Navigation
            }

            function scaleClick() {
                if (mapMode !== MapViewMode.Navi &&
                    mapMode !== MapViewMode.Normal) {
                    return
                }
                setScaleMode()
                killTimer()
                if(forceSetMoveMode) {
                    startTimer = CommonUtils.sleep(timerId, delayms, function() {
                        setNavigationMode()
                    })
                }
            }

            returnButtonVisible: false
            x: 10
            anchors { bottom: parent.bottom; bottomMargin: GlobalConstants.kScreenZone3Height + 89 }
            visible: mapView.moveMode === Renderer.Navigation || (mapView.moveMode === Renderer.Simulator && volumeVisible)

            onScaleUpClicked: {
                var level = viewLevel
                if (level < 12) {
                    level = level + 1

                    setMapViewLevel(level)
                    viewLevel = level
                    if (isNaviOrAndo) {
                        sendEvent(MapViewEvent.SaveMapViewLevel, viewLevel)
                    }
                }
            }
            onScaleDownClicked: {                                
                var level = viewLevel
                if (level >= 1) {
                    level = level - 1

                    setMapViewLevel(level)
                    viewLevel = level
                    if (isNaviOrAndo) {
                        sendEvent(MapViewEvent.SaveMapViewLevel, viewLevel)
                    }
                }
            }
            Component.onCompleted: viewLevel = renderer.viewLevel
        }
    }

    // Engeneer mode gps mapmatch timestamp
    GpsMapmatchTimestamp {
        anchors.top: parent.top
        anchors.topMargin: 2
        anchors.right: parent.right
        anchors.rightMargin: 2
        visible: false
        objectName: "gpsMapmatchTimestamp"
    }

    Connections {
        target: renderer
        onViewLevelChanged: {
            if (!isSpeedResponseMapDisable) {
                mapScaleControl.viewLevel = viewLevel
            }
        }
    }    
}
