import QtQuick 2.1
import QtGraphicalEffects 1.0
import "../../component"
import "../../view"
import "../../view/util"
import "../../GlobalConstants.js" as GlobalConstants
import "../../singleton"
import WidgetMediumViewEvent 1.0
import WidgetConstants 1.0

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
    property bool sdiSectionSpeedVisible
    property bool sdiSignVisible
    property bool sdiRemainDistVisible
    property string sdiSignImageSource
    property var sdiSignNumberImageSources
    property var sdiRemainDistNumberImageSources
    property var sdiCurrentSpeedNumberImageSources
    property bool sdiReset
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

    property bool asView: false
    property bool isGuide: false
    property string lang
    property bool drawOutline: false

    width: 396
    height: 191

    Component.onDestruction: {
        if ( parentWindow ) {
            parentWindow.close()
        }
    }

    onMediated: {
        sendEvent(WidgetMediumViewEvent.Mediated)
        console.log( "WidgetMediumView")
    }

    function changeRgComponent(isRg) {
        loader.sourceComponent = undefined
        if (isRg) {
            loader.sourceComponent = withRg
        } else {
            loader.sourceComponent = noRg
        }
        isGuide = isRg
    }

    transform: Scale {
        origin.x: width/2; origin.y: height/2
        xScale: 1; yScale: verticalFlip ? -1 : 1
    }

    Rectangle {
        visible: asView
        color: 'black'
        anchors.fill: parent
    }

    Loader {
        id: loader
        sourceComponent: withRg
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
            //anchors.leftMargin: 10

            Rectangle {
                width: parent.width
                height: parent.height - statusBar.height
                color: GlobalConstants.kGadgetDarkBgColor
            }

            WidgetTbt {
                id: tbt
                widgetSize: WidgetConstants.SizeMedium

                firstImageSource: root.tbtFirstImageSource
                firstDistNumberImageSources: root.tbtFirstDistNumberImageSources
                firstPointName: root.tbtFirstPointName
                secondImageSource: root.tbtSecondImageSource
                secondDistNumberImageSources: root.tbtSecondDistNumberImageSources
                secondPointName: root.tbtSecondPointName
                appearance: sdi.hasContent ? WidgetConstants.TbtFirst : WidgetConstants.TbtAll
                reset: root.tbtReset
            }

            WidgetSdi {
                id: sdi
                appearance: WidgetConstants.SdiSignAndRemainDist
                widgetSize: WidgetConstants.SizeMedium
                x: 286 + 2
                y: 1
                isRg: true

                signType: root.sdiSignType
                sectionSpeedVisible: false
                signVisible: root.sdiSignVisible
                signImageSource: root.sdiSignImageSource
                signNumberImageSources: root.sdiSignNumberImageSources
                remainDistVisible: root.sdiRemainDistVisible
                remainDistNumberImageSources: root.sdiRemainDistNumberImageSources
                currentSpeedNumberImageSources: root.sdiCurrentSpeedNumberImageSources
                reset: root.sdiReset

                sectionSpeedAverageVisible: root.sdiSectionSpeedAverageVisible
                sectionSpeedTimeVisible: root.sdiSectionSpeedTimeVisible
            }

            WidgetMediumRgStatusBar {
                id: statusBar
                anchors.bottom: parent.bottom
                isRg: true
                remainDistance: root.remainDistanceText
                remainTime: root.remainTime

                onClicked: sendEvent(WidgetMediumViewEvent.SearchClicked)
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
                height: parent.height - statusBar.height
                color: GlobalConstants.kGadgetBrightBgColor

                Rectangle {
                    width: parent.width; height: 1
                    anchors.bottom: parent.bottom
                    color: Qt.hsva(0, 0, 1, 0.35)
                }
            }

            Item {
                id: textBg
                width: parent.width
                height: 43
            }
            Item {
                width: 178; height: 32
                anchors.centerIn: textBg
                Text {
                    anchors { fill: parent; leftMargin: 8; rightMargin: 8 }
                    font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(35) }
                    color: '#ffffff'
                    text: 'Navigation'
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                }
            }

            WidgetSdi {
                id: sdi
                appearance: WidgetConstants.SdiSignOrCurrentSpeed
                widgetSize: WidgetConstants.SizeMedium
                y: 43 - 8
                isRg: false

                signType: root.sdiSignType
                sectionSpeedVisible: root.sdiSectionSpeedVisible
                signVisible: root.sdiSignVisible
                signImageSource: root.sdiSignImageSource
                signNumberImageSources: root.sdiSignNumberImageSources
                remainDistVisible: root.sdiRemainDistVisible
                remainDistNumberImageSources: root.sdiRemainDistNumberImageSources
                currentSpeedNumberImageSources: root.sdiCurrentSpeedNumberImageSources
                reset: root.sdiReset

                sectionSpeedAverageVisible: root.sdiSectionSpeedAverageVisible
                sectionSpeedTimeVisible: root.sdiSectionSpeedTimeVisible
            }

            Item {
                id: row
                x: 110; y: 43
                width: 286; height: 90

                Image {
                    id: icon
                    anchors { left: parent.left; leftMargin: 6; verticalCenter: parent.verticalCenter }
                    source: Resource.imageUrl("map_ic_yah.png")
                }
                LinearGradient {
                    id: gradientRect
                    anchors.fill: marqueeItem
                    gradient: Gradient {
                        GradientStop { position: 0; color: Qt.rgba(0,0,0,0) }
                        GradientStop { position: 16/gradientRect.width; color: Qt.rgba(0,0,0,1) }
                        GradientStop { position: (gradientRect.width-16)/gradientRect.width; color: Qt.rgba(0,0,0,1) }
                        GradientStop { position: 1; color: Qt.rgba(0,0,0,0) }
                    }
                    start: Qt.point(0, 0)
                    end: Qt.point(gradientRect.width, 0)
                    visible: false
                    layer.enabled: true
                    layer.smooth: true
                }
                Item {
                    id: marqueeItem
                    clip: true
                    width: 236; height: 26
                    anchors{ left: icon.right; leftMargin: 6; verticalCenter: parent.verticalCenter }

                    property bool enableLayer: false

                    layer.enabled: childrenRect.width > width && enableLayer
                    layer.samplerName: "textSource"
                    layer.effect: ShaderEffect {
                        property var colorSource: gradientRect;
                        fragmentShader: "
                                    uniform lowp sampler2D colorSource;
                                    uniform lowp sampler2D textSource;
                                    uniform lowp float qt_Opacity;
                                    varying highp vec2 qt_TexCoord0;
                                    void main() {
                                        gl_FragColor =
                                            texture2D(colorSource, qt_TexCoord0).a
                                            * texture2D(textSource, qt_TexCoord0)
                                            * qt_Opacity;
                                    }
                                "
                    }
                    Text {
                        id: addressText
                        width: contentWidth;height: 26
                        font {family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(29)}
                        color: "#ffffff"
                        text: address
                    }
                    TMarquee {
                        id: marquee

                        parentObj: parent
                        textObj: addressText

                        onTextMoved: marqueeItem.enableLayer = true
                        onTextStopped: {
                            marqueeItem.enableLayer = false
                        }
                        onMarqueeFinished: {
                            start()
                        }
                    }
                }
            }

            WidgetMediumRgStatusBar {
                id: statusBar
                anchors.bottom: parent.bottom
                isRg: false

                onClicked: sendEvent(WidgetMediumViewEvent.SearchClicked)
            }
        }
    }
}
