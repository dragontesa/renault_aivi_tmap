import QtQuick 2.1

import "../../component"
import "../../view"
import "../../view/rg"
import "../../view/util"
import "../../GlobalConstants.js" as GlobalConstants
import "../../singleton"
import WidgetLargeViewEvent 1.0
import WidgetConstants 1.0
import TrafficBarViewType 1.0
import SceneGraphRendering 1.0

View {
    id: root
    property var parentWindow
    property bool verticalFlip: false

    property string tbtFirstImageSource
    property var tbtFirstDistNumberImageSources
    property string tbtFirstPointName
    property string tbtSecondImageSource
    property var tbtSecondDistNumberImageSources
    property string tbtSecondPointName
    property bool tbtReset

    property int sdiSignType
    property bool sdiSectionSpeedVisible: false
    property var sdiSectionSpeedAverageNumberImageSources
    property var sdiSectionSpeedTimeImageSources
    property bool sdiSignVisible
    property bool sdiRemainDistVisible
    property string sdiSignImageSource
    property var sdiSignNumberImageSources
    property var sdiRemainDistNumberImageSources
    property bool sdiCurrentSpeedNumberVisible: true
    property var sdiCurrentSpeedNumberImageSources
    property bool sdiReset
    property int sdiSectionSpeedTime
    property bool sdiSectionSpeedToggleAverageAndTime
    property bool sdiSectionSpeedAverageVisible
    property bool sdiSectionSpeedTimeVisible

    property string address
    property string remainDistanceText
    property int remainTime

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
    property bool drawOutline: false

    width: 396
    height: 390

    Component.onDestruction: {
        if ( parentWindow ) {
            parentWindow.close()
        }
    }
    onMediated: {
        sendEvent(WidgetLargeViewEvent.Mediated)
        console.log( "WidgetLargeView")
    }

    function changeRgComponent(isRg) {
        loader.sourceComponent = undefined
        if (isRg) {
            loader.sourceComponent = withRg
            //trafficBar.visible = true
        } else {
            loader.sourceComponent = noRg
            //trafficBar.visible = false
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

            Rectangle {
                width: parent.width
                height: parent.height - statusBar.height
                color: GlobalConstants.kGadgetDarkBgColor
            }

            WidgetSdi {
                appearance: WidgetConstants.SdiCurrentSpeed
                widgetSize: WidgetConstants.SizeLarge

                x: 113 / 2 - width / 2
                y: 10

                isRg: true

                visible: root.sdiCurrentSpeedNumberVisible
                currentSpeedNumberImageSources: root.sdiCurrentSpeedNumberImageSources
                reset: root.sdiReset
            }

            WidgetTbtForLarge {
                id: tbt
                widgetSize: WidgetConstants.SizeLarge

                firstImageSource: root.tbtFirstImageSource
                firstDistNumberImageSources: root.tbtFirstDistNumberImageSources
                firstPointName: root.tbtFirstPointName
                secondImageSource: root.tbtSecondImageSource
                secondDistNumberImageSources: root.tbtSecondDistNumberImageSources
                secondPointName: root.tbtSecondPointName
                reset: root.tbtReset
                z: 1
                appearance: sdi.signVisible ? WidgetConstants.TbtFirst : WidgetConstants.TbtAll
            }

            WidgetSdi {
                id: sdi
                x: 339 - signWidth / 2
                y: 104

                appearance: WidgetConstants.SdiAllForLarge
                widgetSize: WidgetConstants.SizeLarge

                isRg: true

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
                z: 3

                sectionSpeedAverageVisible: root.sdiSectionSpeedAverageVisible
                sectionSpeedTimeVisible: root.sdiSectionSpeedTimeVisible
            }

            WidgetLargeRgStatusBar {
                id: statusBar
                anchors.bottom: parent.bottom
                isRg: true
                address: root.address
                remainDistance: root.remainDistanceText
                remainTime: root.remainTime

                onSearchClicked: sendEvent(WidgetLargeViewEvent.SearchClicked)
                z: 2
            }
        }
    }

    Component {
        id: noRg

        Item {
            id: noRgWrap
            anchors.fill: parent

            Rectangle {
                width: parent.width
                height: parent.height - noRgStatusBar.barHeight
                color: GlobalConstants.kGadgetDarkBgColor
            }

            WidgetSdi {
                id: sdi2
                appearance: WidgetConstants.SdiAllForLarge
                widgetSize: WidgetConstants.SizeLarge

                x: 396 / 2 - signWidth / 2
                y: 144
                isRg: false

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

            WidgetSdi {
                appearance: WidgetConstants.SdiCurrentSpeed
                widgetSize: WidgetConstants.SizeLarge

                x: 396 / 2 - signWidth / 2
                y: 53

                isRg: false

                currentSpeedNumberVisible: root.sdiCurrentSpeedNumberVisible
                currentSpeedNumberImageSources: root.sdiCurrentSpeedNumberImageSources
                reset: root.sdiReset
                visible: !sdi2.signVisible
            }

            WidgetLargeNoRgStatusBar {
                id: noRgStatusBar
                address: root.address
            }
        }
    }
}
