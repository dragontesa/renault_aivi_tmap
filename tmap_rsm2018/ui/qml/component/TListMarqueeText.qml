import QtQuick 2.0
import QtGraphicalEffects 1.0
import '../view/util'
import "../GlobalConstants.js" as GlobalConstants

Item {
    id: root

    property bool enableMarquee: false
    property int fontSize
    property int textOffsetY
    property bool bold: false
    property alias textColor: label.color
    property alias text: label.text
    property alias textOpacity: label.opacity
    property alias contentWidth: label.contentWidth

    onEnableMarqueeChanged: {
        if (enableMarquee === false) {
            marqueeItem.enableEffect = false
            marquee.marqueeAnim.init()
        } else {
            marqueeItem.enableEffect = width<contentWidth
            if (marqueeItem.enableEffect) {
                marquee.marqueeAnim.start()
            }
        }
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
        layer.enabled: enableMarquee
        layer.smooth: enableMarquee
    }
    Item {
        id: marqueeItem
        clip: true
        anchors.fill: parent

        property bool enableEffect: false

        layer.enabled: label2.width > width && enableMarquee && enableEffect
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

            width: enableMarquee ? label2.contentWidth : parent.width
            height: root.height
            font {
                family: GlobalConstants.kFontFamily;
                pixelSize: Resource.pixelToDp(root.fontSize);
                bold: root.bold
            }
            color: root.textColor
            verticalAlignment: Text.AlignVCenter
            offsetY: root.textOffsetY
            elide: enableMarquee ? Text.ElideNone : Text.ElideRight
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

            onMarqueeStarted: {  }
            onMarqueeFinished: {
                label.x = 0
                marqueeAnim.restart()
            }
            onTextMoved: marqueeItem.enableEffect = true
            onTextStopped: {
                marqueeItem.enableEffect = false
            }
        }
    }
}
