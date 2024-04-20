import QtQuick 2.1

import "../../component"
import "../../view"
import "../../view/rg"
import "../../view/util"
import "../../GlobalConstants.js" as GlobalConstants
import "../../singleton"
import WidgetXlViewEvent 1.0
import WidgetConstants 1.0
import TrafficBarViewType 1.0
import SceneGraphRendering 1.0

View {
    id: root
    width: 598
    height: 390
    property var parentWindow
    property bool verticalFlip: false

    property string tbtFirstImageSource
    property var tbtFirstDistNumberImageSources
    property string tbtFirstPointName
    property string tbtSecondImageSource
    property var tbtSecondDistNumberImageSources
    property string tbtSecondPointName
    property bool tbtReset

    property bool laneFirstVisible
    property bool laneSecondVisible
    property var laneFirstImageSources
    property var laneFirstDistNumberImageSources
    property var laneSecondImageSources
    property var laneSecondDistNumberImageSources
    property bool laneReset

    property int sdiSignType
    property bool sdiSectionSpeedVisible
    property var sdiSectionSpeedAverageNumberImageSources
    property var sdiSectionSpeedTimeImageSources
    property bool sdiSignVisible
    property bool sdiRemainDistVisible
    property string sdiSignImageSource
    property var sdiSignNumberImageSources
    property var sdiRemainDistNumberImageSources
    property var sdiCurrentSpeedNumberImageSources
    property bool sdiReset
    property int sdiSectionSpeedTime
    property bool sdiSectionSpeedToggleAverageAndTime
    property bool sdiSectionSpeedAverageVisible
    property bool sdiSectionSpeedTimeVisible

    property string address
    property string remainDistanceText
    property int remainTime

    property alias totalDistance: trafficBar.totalDistance
    property alias remainDistance: trafficBar.leftDistance
    property alias trafficGuideList: trafficBar.trafficGuideList
    property alias skyState: trafficBar.skyState

    property string timeFormat: 'AP hh:mm'
    property date gpsTime
    property bool gpsTimeUpdated: false
    onGpsTimeChanged: {
        gpsTimeUpdated = true
    }
    property int gpsHour
    property int gpsMin
    property int gpsSec

    property bool isNaviOrAndo: true
    property bool isOnline: false
    property bool showTrafficInfo: true // from controller
    property bool enableTrafficInfo: false // when control mode
    property bool showTrafficInfoOnMap: false
    property bool showTrafficInfoOnRouteLine: false
    property bool showTrafficInfoOnRouteLineForce: false // 경로 요약 화면, 경로 상세 화면에서 설정

    property bool asView: false
    property string lang: 'ko'

    property bool sdiCurrentSpeedNumberVisible: true
    property bool drawOutline: false

    Component.onDestruction: {
        if ( parentWindow ) {
            parentWindow.close()
        }
    }

    onXChanged: {
        console.log( "[GadgetTest]WidgetXlView.qml XChanged x: " + x + " y: " + y )
    }

    onYChanged: {
        console.log( "[GadgetTest]WidgetXlView.qml YChanged x: " + x + " y: " + y )
    }

    onMediated: {        
        sendEvent(WidgetXlViewEvent.Mediated)
        console.log( "WidgetXlView")
    }

    onActivated: {
        sendEvent(WidgetXlViewEvent.Activated)
    }

    onDeactivated: {
        sendEvent(WidgetXlViewEvent.Deactivated)
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

    Loader {
        id: loader
        sourceComponent: noRg
        anchors.fill: parent
    }
    Rectangle {
        anchors.fill: parent
        color: 'transparent'
        border.color: MultiSenseCoordinator.kAmbientColor
        border.width: 2
        visible: drawOutline
    }

    Component {
        id: withRg
        Item {
            id: withRgWrap
            anchors.fill: parent

            WidgetTbt {
                id: tbt
                widgetSize: WidgetConstants.SizeXL

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
                widgetSize: WidgetConstants.SizeXL
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: rgStatusBar.top
                anchors.bottomMargin: 0

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
                widgetSize: WidgetConstants.SizeXL
                y: 104

                signType: root.sdiSignType
                sectionSpeedVisible: root.sdiSectionSpeedVisible
                sectionSpeedTimeImageSources: root.sdiSectionSpeedTimeImageSources
                sectionSpeedAverageNumberImageSources: root.sdiSectionSpeedAverageNumberImageSources
                sectionSpeedToggleAverageAndTime: root.sdiSectionSpeedToggleAverageAndTime
                signVisible: root.sdiSignVisible
                signImageSource: root.sdiSignImageSource
                signNumberImageSources: root.sdiSignNumberImageSources
                remainDistVisible: root.sdiRemainDistVisible
                remainDistNumberImageSources: root.sdiRemainDistNumberImageSources
                currentSpeedNumberVisible: root.sdiCurrentSpeedNumberVisible
                currentSpeedNumberImageSources: root.sdiCurrentSpeedNumberImageSources
                reset: root.sdiReset

                sectionSpeedAverageVisible: root.sdiSectionSpeedAverageVisible
                sectionSpeedTimeVisible: root.sdiSectionSpeedTimeVisible
            }

            WidgetXlRgStatusBar {
                id: rgStatusBar
                anchors.bottom: parent.bottom
                isRg: true
                address: root.address
                remainDistance: root.remainDistanceText
                remainTime: root.remainTime

                onSearchClicked: sendEvent(WidgetXlViewEvent.SearchClicked)
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
                widgetSize: WidgetConstants.SizeXL
                y: 104

                signType: root.sdiSignType
                sectionSpeedVisible: root.sdiSectionSpeedVisible
                sectionSpeedTimeImageSources: root.sdiSectionSpeedTimeImageSources
                sectionSpeedAverageNumberImageSources: root.sdiSectionSpeedAverageNumberImageSources
                sectionSpeedToggleAverageAndTime: root.sdiSectionSpeedToggleAverageAndTime
                signVisible: root.sdiSignVisible
                signImageSource: root.sdiSignImageSource
                signNumberImageSources: root.sdiSignNumberImageSources
                remainDistVisible: root.sdiRemainDistVisible
                remainDistNumberImageSources: root.sdiRemainDistNumberImageSources
                currentSpeedNumberVisible: root.sdiCurrentSpeedNumberVisible
                currentSpeedNumberImageSources: root.sdiCurrentSpeedNumberImageSources
                reset: root.sdiReset

                sectionSpeedAverageVisible: root.sdiSectionSpeedAverageVisible
                sectionSpeedTimeVisible: root.sdiSectionSpeedTimeVisible
            }

            WidgetXlRgStatusBar {
                anchors.bottom: parent.bottom
                isRg: false
                address: root.address

                onSearchClicked: sendEvent(WidgetXlViewEvent.SearchClicked)
            }
        }
    }

    TrafficBarView {
        id: trafficBar

        anchors.right: parent.right
        anchors.rightMargin: 10
        anchors.top: parent.top
        anchors.topMargin: 20
        viewType: TrafficBarViewType.Widget_XL
        onlineState: isOnline
    }
}

