import QtQuick 2.0
import QtGraphicalEffects 1.0
import "../../GlobalConstants.js" as GlobalConstants
import '../../component'
import '../../view/util'

Item {
    id: root
    property var layout
    property bool isGuide: false
    property string dispType: ''
    property bool dispTypeChanged: false

    function changeRgDispTypeByTimer() {
        changeRgDispType()
    }
    function changeRgDispTypeByMarquee() {
        changeRgDispType()
    }

    function reset() {
        marquee.stop()
        rgTimer.stop()
        marqueeItem.enableLayer = false
    }

    function restart() {
        marquee.init()
        if (!marquee.marquee) {
            rgTimer.restart()
        } else {
            marquee.start()
        }
    }

    function setIcon(url) {
        icon.source = url
    }

    function setNaviAddress(type, address) {
        var changed = false
        setIcon(Resource.imageUrl('zone/map_ic_' + type + '.png'))
        if (dispType !== type) {
            dispType = type
            changed = true
        }
        if (label.text !== address) {
            label.text = address
            changed = true
        }
        if (changed) {
            restart()
        }
    }

    function setYah(yah) {
        setNaviAddress('yah', yah)
    }
    function setPoi(poi) {
        setNaviAddress('poi', poi)
    }
    function setRoad(road) {
        setNaviAddress('road', road)
    }

    Timer { id : rgTimer; running: false; interval: 5*1000; onTriggered: changeRgDispTypeByTimer() }

    Row {
        id: row
        anchors.centerIn: parent

        spacing: layout.naviAddressSpacing
        Image {
            id: icon
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
            width: isGuide ? Math.min(label2.width, 216)
                           : Math.min(label2.width, 488)
            height: childrenRect.height

            property bool enableLayer: false

            layer.enabled: label2.width > width && enableLayer
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
            Text2 {
                id: label

                width: label2.width
                height: layout.naviAddressTextHeight
                font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(29) }
                color: "#ffffff"
                opacity: 0.85
                verticalAlignment: Text.AlignVCenter
                offsetY: 2
            }
            Text {
                id: label2
                visible: false
                font: label.font
                text: label.text
            }

            TMarquee {
                id: marquee

                parentObj: parent
                textObj: label

                onMarqueeStarted: { rgTimer.stop() }
                onMarqueeFinished: {
                    stop()
                    changeRgDispTypeByMarquee()
                }
                onTextMoved: marqueeItem.enableLayer = true
                onTextStopped: {
                    marqueeItem.enableLayer = false
                }
            }
        }
    }
}
