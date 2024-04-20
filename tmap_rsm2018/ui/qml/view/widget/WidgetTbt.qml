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

        if ( widgetSize == WidgetConstants.SizeXXXL ) {
            margueeGradationTransparent =  "#001E7C4A"
            margueeGradationOpaque = "#FF1E7C4A"

            firstBgImageSource = Resource.imageUrl( "tbt", "tbt_1st_bg.png" )
            secondBgImageSource = Resource.imageUrl( "tbt", "tbt_2nd_bg.png" )
        }
        else if ( widgetSize == WidgetConstants.SizeXXL
                 || widgetSize == WidgetConstants.SizeXL        
                 ) {
            margueeGradationTransparent =  "#001E7C4A"
            margueeGradationOpaque = "#FF1E7C4A"

            firstBgImageSource = Resource.imageUrl( "widgettbt", "wid_tbt_1st_bg.png" )
            secondBgImageSource = Resource.imageUrl( "widgettbt", "wid_tbt_2nd_bg.png" )
//            firstImageSource = Resource.imageUrl( "widgettbt", "wid_tbt_1st_05.png" )

            firstImageTopMargin = 4
            firstImageLeftMargin = 4
            firstDistTopMargin = 12
            firstDistLeftMargin = 4
            firstDistHeight = 40
            firstPosTopMargin = 7
            firstPosFont = Qt.font({
                family: GlobalConstants.kFontFamily,
                pixelSize: Resource.pixelToDp(23),
                bold: true
            });
            firstPosWidth = 159
            firstContentWidth = 263
            secondLeftMargin = 0
            secondImageTopMargin = 12
            secondDistTopMargin = 7
            secondContentWidth = 82

            firstWrap.visible = true
            secondWrap.visible = true
        }
        else if ( widgetSize == WidgetConstants.SizeLarge ) {
            margueeGradationTransparent =  "#00FFFFFF"
            margueeGradationOpaque = "#FFFFFFFF"

            firstImageTopMargin = 81
            firstImageLeftMargin = 198 - 86 / 2

            firstDistWrap.anchors.left = undefined
            firstDistWrap.anchors.leftMargin = undefined
            firstDistTopMargin = firstImageTopMargin + 86 + 10
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
            firstPosWrap.x = 198 - 159 / 2
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
        else if ( widgetSize == WidgetConstants.SizeMedium ) {
            margueeGradationTransparent =  "#00000000"
            margueeGradationOpaque = "#FF000000"

            firstImageTopMargin = 23
            firstImageLeftMargin = 6
            firstDistTopMargin = 30
            firstDistLeftMargin = 4
            firstDistHeight = 40
            firstPosTopMargin = 10
            firstPosFont = Qt.font({
                family: GlobalConstants.kFontFamily,
                pixelSize: Resource.pixelToDp(29),
                bold: true
            });
            firstPosWidth = 180
            firstContentWidth = 286
            secondImageTopMargin = 22
            secondDistTopMargin = 4
            secondContentWidth = 110

            firstWrap.visible = false
            secondWrap.visible = false
        }

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
            anchors.left: firstDistWrap.left
            anchors.top: firstDistWrap.bottom
            anchors.topMargin: firstPosTopMargin

            MarqueeText {
                text: root.firstPointName
                textColor: 'white'
                font: firstPosFont
                width: firstPosWidth
                running: appRunning
                onScrollingChanged: {
                    marqueeHighlight.visible = scrolling
                }
            }

            Item {
                id: marqueeHighlight
                visible: false
                LinearGradient {
                    width: margueeGradationWidth
                    height: margueeGradationHeight
                    start: Qt.point(0, 0)
                    end: Qt.point(margueeGradationWidth, 0)
                    gradient: Gradient {
                        GradientStop { position: 0.0; color: margueeGradationOpaque }
                        GradientStop { position: 1.0; color: margueeGradationTransparent }
                    }
                }

                LinearGradient {
                    x: firstPosWidth - margueeGradationWidth
                    width: margueeGradationWidth
                    height: margueeGradationHeight
                    start: Qt.point(0, 0)
                    end: Qt.point(margueeGradationWidth, 0)
                    gradient: Gradient {
                        GradientStop { position: 0.0; color: margueeGradationTransparent }
                        GradientStop { position: 1.0; color: margueeGradationOpaque }
                    }
                }
            }
        }
    }

    Item {
//        Rectangle {
        id: secondWrap
        objectName: 'second'
//        visible: true
        anchors.left: firstWrap.right
        anchors.leftMargin: secondLeftMargin
        width: secondContentWidth
        visible: false

        Image {
            id: secondImage
            anchors.horizontalCenter: parent.horizontalCenter
            y: secondImageTopMargin
            source: root.secondImageSource
        }

        Item {
            id: secondDistWrap
            anchors.top: secondImage.bottom
            anchors.topMargin: secondDistTopMargin
            width: childrenRect.width
            anchors.horizontalCenter: parent.horizontalCenter

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
