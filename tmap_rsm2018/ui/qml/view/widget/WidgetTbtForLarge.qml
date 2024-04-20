import QtQuick 2.0
import WidgetConstants 1.0
import "../rg"
import "../../component"
import "../../view"
import "../../GlobalConstants.js" as GlobalConstants
import QtGraphicalEffects 1.0

Item {
    id: root

    // XXXL
    property int firstImageTopMargin : 5
    property int firstImageLeftMargin : 7
    property int firstDistTopMargin : 16
    property int firstDistLeftMargin : 5    
    property int firstDistHeight: 54
    property int firstPosLeftMargin : 5
    property int firstPosTopMargin : 5
    property font firstPosFont: Qt.font({
        family: GlobalConstants.kFontFamily,
        pixelSize: Resource.pixelToDp(24),
        bold: true
    });
    property int firstPosWidth: 205
    property int firstContentWidth: 350
    property int secondImageTopMargin : 16
    property int secondDistTopMargin : 11    
    property int secondLeftMargin : 10
    property int secondContentWidth: 92

    property int margueeGradationWidth: 14
    property int margueeGradationHeight: 28
    property color margueeGradationTransparent: "#001E7C4A"
    property color margueeGradationOpaque: "#FF1E7C4A"

    property string firstImageSource
    property string firstBgImageSource
    property string firstPointName
    property var firstDistNumberImageSources
    property string secondImageSource
    property string secondBgImageSource
    property string secondPointName
    property var secondDistNumberImageSources
    property int widgetSize: WidgetConstants.SizeNone
    property int appearance: WidgetConstants.TbtAll

    property bool appRunning: true
    property bool reset: false

    function setToDefault() {        
        firstImageTopMargin = 5
        firstImageLeftMargin = 7
        firstDistTopMargin = 16
        firstDistLeftMargin = 5
        firstDistHeight = 54
        firstPosTopMargin = 5
        firstPosWidth = 205
        firstContentWidth = 350
        secondImageTopMargin = 16
        secondDistTopMargin = 11
        secondLeftMargin = 10
        secondContentWidth = 92
    }

    onResetChanged: {
        if ( reset ) {
            firstWrap.visible = false
            secondWrap.visible = false
        }
    }

    onWidgetSizeChanged: {
        setToDefault()

        firstImageTopMargin = 81
        firstImageLeftMargin = 198 - 86 / 2

        firstDistWrap.anchors.left = undefined
        firstDistWrap.anchors.leftMargin = undefined
        firstDistTopMargin = firstImageTopMargin + 86 + 10

        // center
        firstDistLeftMargin = Qt.binding(function(){return 198 - firstDistWrap.width / 2})
        firstDistHeight = 40
        firstDistWrap.x = Qt.binding(function(){return firstDistLeftMargin})

        firstPosWrap.anchors.left = undefined
        firstPosWrap.anchors.top = undefined
        firstPosWrap.anchors.topMargin = undefined

        //firstPosTopMargin = 7
        firstPosFont = Qt.font({
            family: GlobalConstants.kFontFamily,
            pixelSize: Resource.pixelToDp(29),
            bold: true
        });
        firstPosWidth = 159
        //firstPosWrap.x = 198 - 159 / 2
        firstPosWrap.y = firstDistTopMargin + firstDistHeight + 7

        //firstContentWidth = 286

        secondWrap.anchors.left = undefined
        secondWrap.anchors.leftMargin = undefined
        secondImage.anchors.horizontalCenter = undefined
        secondImage.x = 339 - 54 / 2
        secondImage.y = (390 - 58)/ 2 - (54 + 4 + 26) / 2
        secondDistWrap.anchors.top = undefined
        secondDistWrap.anchors.topMargin = undefined
        secondDistWrap.anchors.horizontalCenter = undefined

        secondDistWrap.x = Qt.binding(function(){return 339 - secondDistWrap.width / 2})
        secondDistWrap.y = secondImage.y + 54 + 4

//            secondImageTopMargin = 22
//            secondDistTopMargin = 4
//            secondContentWidth = 110

        firstWrap.visible = false
        secondWrap.visible = false
    }

    onFirstImageSourceChanged: {
        firstWrap.visible = true
    }

    onSecondImageSourceChanged: {
        console.log( '[WidgetTbt.qml]secondImageSource: ' + secondImageSource)
        if ( appearance == WidgetConstants.TbtAll )  {
            secondWrap.visible = true
        }
        else {
            secondWrap.visible = false
        }
    }

    onAppearanceChanged: {
        if ( appearance == WidgetConstants.TbtAll )  {
            secondWrap.visible = true
        }
        else {
            secondWrap.visible = false
        }
    }

    width: secondBgImage.width
    height: firstBgImage.height

    Image {
        id: secondBgImage
        source: secondBgImageSource
        visible: firstImageSource.length > 0
    }

    Image {
        id: firstBgImage
        source: firstBgImageSource
        visible: secondImageSource.length > 0
    }

    Item {
        id: firstWrap
        objectName: 'first'
        property string image
        property string pointName
        visible: false
        width: firstContentWidth

        Image {
            id: firstImage
            x: firstImageLeftMargin
            y: firstImageTopMargin
            source: root.firstImageSource
        }
        Item {
            id: firstDistWrap
            anchors.left: firstImage.right
            anchors.leftMargin: firstDistLeftMargin
            y: firstDistTopMargin
            height: firstDistHeight
            width: firstDistWrapRow.width
            Row {
                id: firstDistWrapRow
                width: childrenRect.width
                Repeater {
                    id: firstDistNumberRepeater
                    model: firstDistNumberImageSources
                    delegate: Item {
                        width: childrenRect.width
                        height: childrenRect.height
                        property string source: modelData
                        Image {
                            id: firstDistNumberImage
                            source: parent.source
                        }
                    }
                }
            }
        }

        Item {
            id: firstPosWrap
            anchors.horizontalCenter: firstDistWrap.horizontalCenter
            width: firstPosWidth

            TListMarqueeText {
                anchors.horizontalCenter: parent.horizontalCenter
                width: Math.min(contentWidth, parent.width)
                height: 26
                fontSize: 29
                textColor: '#ffffff'
                bold: true
                text: firstPointName
                textOffsetY: 2
                enableMarquee: width <= contentWidth
            }
        }
    }

    Item {
        id: secondWrap
        objectName: 'second'
        anchors.left: firstWrap.right
        anchors.leftMargin: secondLeftMargin
        width: secondContentWidth
        visible: false

        Image {
            id: secondImage
            anchors.horizontalCenter: parent.horizontalCenter
            y: secondImageTopMargin
            source: root.secondImageSource
            opacity: 0.85
        }

        Item {
            id: secondDistWrap
            anchors.top: secondImage.bottom
            anchors.topMargin: secondDistTopMargin
            width: childrenRect.width
            anchors.horizontalCenter: parent.horizontalCenter
            opacity: 0.85

            Row {
                width: childrenRect.width
                Repeater {
                    id: secondDistNumberRepeater
                    model: secondDistNumberImageSources
                    delegate: Item {
                        width: childrenRect.width
                        height: childrenRect.height
                        property string source: modelData
                        Image {
                            id: secondDistNumberImage
                            source: parent.source
                        }
                    }
                }
            }
        }    
    }
}
