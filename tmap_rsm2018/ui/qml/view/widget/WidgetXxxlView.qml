import QtQuick 2.1

import "../../component"
import "../../view"
import "../../view/util"
import "../../view/rg"
import "../../GlobalConstants.js" as GlobalConstants
import WidgetXxxlViewEvent 1.0
import WidgetConstants 1.0
import TrafficBarViewType 1.0
import SceneGraphRendering 1.0

View {
    id: root
    width: 598
    height: 788
    property var parentWindow
    property bool verticalFlip: false
    property bool bgraFormat: false

    property string tbtFirstImageSource
    property var tbtFirstDistNumberImageSources
    property string tbtFirstPointName
    property string tbtSecondImageSource
    property var tbtSecondDistNumberImageSources
    property string tbtSecondPointName
    property bool tbtReset

    property int sdiSignType
    property bool sdiSectionSpeedAverageVisible
    property var sdiSectionSpeedAverageNumberImageSources
    property bool sdiSignVisible
    property bool sdiRemainDistVisible
    property string sdiSignImageSource
    property var sdiSignNumberImageSources
    property var sdiRemainDistNumberImageSources
    property var sdiCurrentSpeedNumberImageSources
    property bool sdiReset

    property bool laneFirstVisible
    property bool laneSecondVisible
    property var laneFirstImageSources
    property var laneFirstDistNumberImageSources    
    property var laneSecondImageSources
    property var laneSecondDistNumberImageSources
    property bool laneReset

    property string address
    property string remainDistanceText
    property int remainTime

    property alias totalDistance: trafficBar.totalDistance
    property alias remainDistance: trafficBar.leftDistance
    property alias trafficGuideList: trafficBar.trafficGuideList

    property bool appRunning: true
    property string timeFormat: 'AP hh:mm'
    property date gpsTime
    property bool gpsTimeUpdated: false
    onGpsTimeChanged: {
        gpsTimeUpdated = true
    }

    property bool isNaviOrAndo: true
    property bool isOnline: false
    property bool showTrafficInfo: true // from controller
    property bool enableTrafficInfo: false // when control mode
    property bool showTrafficInfoOnMap: false
    property bool showTrafficInfoOnRouteLine: false
    property bool showTrafficInfoOnRouteLineForce: false // 경로 요약 화면, 경로 상세 화면에서 설정

    property bool asView: false

    Component.onDestruction: {
        if ( parentWindow ) {
            parentWindow.close()
        }
    }

    onMediated: {
        sendEvent(WidgetXxxlViewEvent.Mediated)
        console.log( "WidgetXxxlView")
    }

    onTbtFirstDistNumberImageSourcesChanged: {        
    }

    onSdiSignVisibleChanged: {
        //console.log( "#1 sdiSignVisible:" + sdiSignVisible)
    }

    onSdiSignImageSourceChanged: {
        //console.log( "#1 ")
    }

    onSdiRemainDistNumberImageSourcesChanged: {
        //console.log( sdiRemainDistNumberImageSources )
    }

    onLaneFirstImageSourcesChanged: {
        //console.log( "#1 ")
    }

    function changeRgComponent(isRg) {
        loader.sourceComponent = undefined
        if (isRg) {
            loader.sourceComponent = withRg
            trafficBar.visible = true
        } else {
            loader.sourceComponent = noRg
            trafficBar.visible = false
        }
    }

    transform: Scale {
        origin.x: width/2; origin.y: height/2
        xScale: 1; yScale: verticalFlip ? -1 : 1
    }
    layer.enabled: bgraFormat
    layer.effect: ShaderEffect {
        fragmentShader: "
            uniform lowp sampler2D source;
            uniform lowp float qt_Opacity;
            varying highp vec2 qt_TexCoord0;
            void main() {
                gl_FragColor = texture2D(source, qt_TexCoord0).bgra * qt_Opacity;
            }
        "
    }

    Item {
        id: rendererWrap
        anchors.fill: parent

        onWidthChanged: {
            QmlAppHelper.debug( "Width: " + rendererWrap.width )
            QmlAppHelper.debug( "Height: " + rendererWrap.height )
        }

        Renderer {
            id: renderer
            objectName: "renderer"
            anchors {
                fill: parent
            }

            moveMode: Renderer.Navigation
            downloadRouteTile: false
            //nightMode: false

            viewMode: Renderer.BirdView
            //theme: Renderer.ThemeCluster
            //clusterPoi: true
            //showBuilding: false
            //skyState: Renderer.SkyCluster
            trafficInfo: (enableTrafficInfo || (showTrafficInfoOnMap && isNaviOrAndo)) && isOnline
            trafficInfoOnRoute: (enableTrafficInfo || showTrafficInfoOnRouteLine || showTrafficInfoOnRouteLineForce) && isOnline
            showRouteObject: Renderer.RouteObjectDefault & ~Renderer.RouteObjectMainRoadName
            headUpCenterFactor: Qt.point(0.5, 0.75);
            showRoute: true

            Component.onCompleted: {
                QmlAppHelper.debug('#1')
            }

            Component.onDestruction: {
                QmlAppHelper.debug( '#1' )
                root.sendEvent( WidgetXxxlViewEvent.RendererOnDestruction, renderer )
            }
        }
    }

    Loader {
        id: loader
        sourceComponent: noRg
        anchors.fill: parent
    }

    Component {
        id: withRg
        Item {
            id: withRgWrap
            anchors.fill: parent

            Rectangle {
                anchors.fill: parent
                color: "#ffffff"
                opacity: 0.05
            }

            WidgetTbt {
                id: tbt
                widgetSize: WidgetConstants.SizeXXXL

                firstImageSource: root.tbtFirstImageSource
                firstDistNumberImageSources: root.tbtFirstDistNumberImageSources
                firstPointName: root.tbtFirstPointName
                secondImageSource: root.tbtSecondImageSource
                secondDistNumberImageSources: root.tbtSecondDistNumberImageSources
                secondPointName: root.tbtSecondPointName
                reset: root.tbtReset
            }

            WidgetLane {
                id: lane
                widgetSize: WidgetConstants.SizeXXXL
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: rgStatusBar.top
                anchors.bottomMargin: 20

                firstVisible: root.laneFirstVisible
                secondVisible: root.laneSecondVisible
                firstImageSources: root.laneFirstImageSources
                firstDistNumberImageSources: root.laneFirstDistNumberImageSources
                secondImageSources: root.laneSecondImageSources
                secondDistNumberImageSources: root.laneSecondDistNumberImageSources
                reset: root.laneReset
            }

            WidgetSdi {
                id: sdi
                appearance: WidgetConstants.SdiAll
                widgetSize: WidgetConstants.SizeXXXL
                y: 140

                signType: root.sdiSignType
                signVisible: root.sdiSignVisible
                signImageSource: root.sdiSignImageSource
                signNumberImageSources: root.sdiSignNumberImageSources
                sectionSpeedAverageVisible: root.sdiSectionSpeedAverageVisible
                sectionSpeedAverageNumberImageSources: root.sdiSectionSpeedAverageNumberImageSources
                remainDistVisible: root.sdiRemainDistVisible
                remainDistNumberImageSources: root.sdiRemainDistNumberImageSources
                currentSpeedNumberImageSources: root.sdiCurrentSpeedNumberImageSources
                reset: root.sdiReset

                onRemainDistVisibleChanged: {
                    console.log( "#1 remainDistVisible: " + remainDistVisible )
                }
            }

            WidgetXxxlRgStatusBar {
                id: rgStatusBar
                anchors.bottom: parent.bottom
                isRg: true
                address: root.address
                remainDistance: root.remainDistanceText
                remainTime: root.remainTime

                onSearchClicked: sendEvent(WidgetXxxlViewEvent.SearchClicked)
            }
        }
    }

    Component {
        id: noRg

        Item {
            id: noRgWrap
            anchors.fill: parent

            WidgetSdi {
                id: sdi
                appearance: WidgetConstants.SdiAll
                widgetSize: WidgetConstants.SizeXXXL
                y: 140

                signType: root.sdiSignType
                sectionSpeedAverageVisible: root.sdiSectionSpeedAverageVisible
                sectionSpeedAverageNumberImageSources: root.sdiSectionSpeedAverageNumberImageSources
                signVisible: root.sdiSignVisible
                signImageSource: root.sdiSignImageSource
                signNumberImageSources: root.sdiSignNumberImageSources
                remainDistVisible: root.sdiRemainDistVisible
                remainDistNumberImageSources: root.sdiRemainDistNumberImageSources
                currentSpeedNumberImageSources: root.sdiCurrentSpeedNumberImageSources
                reset: root.sdiReset
            }

            WidgetXxxlRgStatusBar {
                anchors.bottom: parent.bottom
                isRg: false
                address: root.address

                onSearchClicked: sendEvent(WidgetXxxlViewEvent.SearchClicked)
            }
        }
    }

    TrafficBarView {
        id: trafficBar

        anchors.right: parent.right
        anchors.rightMargin: 10
        anchors.top: parent.top
        anchors.topMargin: 151
        viewType: TrafficBarViewType.Widget_XXXL
    }
}

