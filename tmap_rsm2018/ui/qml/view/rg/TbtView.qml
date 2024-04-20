import QtQuick 2.1
import MapViewMode 1.0
import TbtViewEvent 1.0
import QtGraphicalEffects 1.0
import Csw 1.0
import "../../component"
import "../../view"
import "../../view/util"
import "../../GlobalConstants.js" as GlobalConstants

View {
    id: root	
    hasFocus: false

    property int firstImageTopMargin : 5
    property int firstImageLeftMargin : 7
    property int firstDistTopMargin : 16
    property int firstDistLeftMargin : 5
    property int firstDistMaxNumbers: 3
    property int firstDistHeight: 54
    property int firstPosTopMargin : 5 + 6 // TMCRSM-1748
    property font firstPosFont: Qt.font({
        family: GlobalConstants.kFontFamily,
        pixelSize: Resource.pixelToDp(30),
        bold: true
    });
    property int firstPosNameWidth: 205
    property int margueeGradationWidth: 14
    property int margueeGradationHeight: 28
    property color margueeGradationTransparent: "#001E7C4A"
    property color margueeGradationOpaque: "#FF1E7C4A"
    property int secondImageTopMargin : 16
    property int secondDistTopMargin : 11
    property int secondDistMaxNumbers: 3
    property int secondContentWidth: 110
    property int secondTouchAreaWidth: 148
    property int secondTouchAreaHeight: 140

    property int leftArrowY: 11

    property int listTopMargin: 11
    property int arrowWidth: 38
    property int progressWidth: 22
    property int progressRightMargin: 20
    property int panelBgWidth: 303
    property int panelWidth: panelBgWidth + progressWidth + progressRightMargin
    property int panelHeight: 86
    property int listWidth: arrowWidth + panelWidth
    property int turnIconLeftMargin: 16
    property int turnIconTopMargin: 12
    property int distanceLeftMargin: 9
    property int distanceTopMargin: 12
    property int distanceWidth: 188
    property int distanceHeight: 25
    property int nameTopMargin: 7
    property int nameY: turnIconTopMargin + distanceHeight + nameTopMargin
    property font nameTextFont: Qt.font({
        family: GlobalConstants.kFontFamily,
        pixelSize: Resource.pixelToDp(24)
    });
    property color nameTextColor: '#ffffff'
    property bool appRunning: true

    width: secondImgBg.width
    height: firstImgBg.height
    visible: false
    property bool isBuildTarget: false

    property var csws: [leftArrowCsw]

    onMediated: {
        sendEvent( TbtViewEvent.Mediated )
        sendEvent(TbtViewEvent.RegistCsw, csws)
        //sendEvent(TbtViewEvent.SelectCsw, csws)
    }

    onVisibleChanged: {
        //console.log( "#1 visible: " + visible )
        sendEvent( TbtViewEvent.VisibleChanged, {visible: visible} )

        if ( visible ) {
            //sendEvent(TbtViewEvent.SelectCsw, csws)
        }
        else {
            //sendEvent(TbtViewEvent.RemoveCsw, csws)
        }
    }

    function setFirstDistNumberImages( images ) {
        firstDistMaxNumbers = images.length
        var firstDistNumberRepeaterItemAt = firstDistNumberRepeater.itemAt
        for ( var i = 0; i < images.length; i++ ) {
            firstDistNumberRepeaterItemAt( i ).source = images[i]
        }
    }

    function setSecondDistNumberImages( images ) {
        secondDistMaxNumbers = images.length
        var secondDistNumberRepeaterItemAt = secondDistNumberRepeater.itemAt
        for ( var i = 0; i < images.length; i++ ) {
            secondDistNumberRepeaterItemAt( i ).source = images[i]
        }
    }

    function retranslateUi() {        
    }

    function reset() {
        firstWrap.visible = false
        firstWrap.image = ''
        firstWrap.pointName = ''
        for ( var i = 0; i < firstDistNumberRepeater.count; i++ ) {
            firstDistNumberRepeater.itemAt( i ).source = ''
        }

        secondWrap.visible = false
        secondWrap.image = ''
        for ( i = 0; i < secondDistNumberRepeater.count; i++ ) {
            secondDistNumberRepeater.itemAt( i ).source = ''
        }
    }

    Image {
        id: secondImgBg
        source: Resource.imageUrl( "tbt", "tbt_2nd_bg.png" )
        visible: secondWrap.visible
    }

    Image {
        id: firstImgBg
        source: Resource.imageUrl( "tbt", "tbt_1st_bg.png" )
        visible: firstWrap.visible
    }

    Item {
        id: firstWrap
        objectName: 'first'
        property string image
        property string pointName
        visible: false
        width: firstImgBg.width

        Image {
            id: firstImage
            x: firstImageTopMargin
            y: firstImageLeftMargin
            source: firstWrap.image
        }
        Item {
            id: firstDistWrap
            anchors.left: firstImage.right
            anchors.leftMargin: firstDistLeftMargin
            y: firstDistTopMargin
            height: firstDistHeight
            Row {
                Repeater {
                    id: firstDistNumberRepeater
                    model: firstDistMaxNumbers
                    delegate: Item {
                        width: childrenRect.width
                        height: childrenRect.height
                        property string source
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
                id: marquee
                text: firstWrap.pointName
                textColor: 'white'
                font: firstPosFont
                width: firstPosNameWidth
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
                    x: firstPosNameWidth - margueeGradationWidth
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
        property string image
        visible: false
        anchors.left: firstWrap.right
        width: secondContentWidth

        Image {
            id: secondNameImage
            anchors.horizontalCenter: parent.horizontalCenter
            y: secondImageTopMargin
            source: secondWrap.image
        }

        Item {
            id: secondDistWrap
            anchors.top: secondNameImage.bottom
            anchors.topMargin: secondDistTopMargin
            width: childrenRect.width
            anchors.horizontalCenter: parent.horizontalCenter
            Row {
                width: childrenRect.width
                Repeater {
                    id: secondDistNumberRepeater
                    model: secondDistMaxNumbers
                    delegate: Item {
                        width: childrenRect.width
                        height: childrenRect.height
                        property string source
                        Image {
                            id: secondDistNumberImage
                            source: parent.source
                        }
                    }
                }
            }
        }
    }

    Image {
        id: cswFocusMark
        source: Resource.imageUrl( 'tbt', 'map_tbt_csw_f.png' )
        anchors.left: firstWrap.right
        visible: false
        z: 99
    }

    Image {
        id: leftArrowImg
        objectName: 'leftArrow'
        anchors.left: secondWrap.right
        y: leftArrowY
        source: Resource.imageUrl( 'tbt', 'tbt_arw_l.png' )
        visible: true

        Csw {
            id: leftArrowCsw
            objectName: 'leftArrowCsw'
            section: Csw.Section_Map
            sequence: Csw.Sequence_TbtArrow
            deletable: false
            enable: leftArrowImg.visible
            onEnableChanged: {
                console.log('[CSW][TbtView]onEnableChanged enable: ' + enable)
            }

            onFocused: {
                console.log('[CSW][TbtView]onFocused')
                cswFocusMark.visible = true
            }
            onLostFocusd: {
                console.log('[CSW][TbtView]onLostFocusd')
                cswFocusMark.visible = false
            }
            onSelected: {
                console.log('[CSW][TbtView]onSelected')
                sendEvent( TbtViewEvent.ArrowClicked, {byCsw:true} )
                cswFocusMark.visible = false
            }
        }
    }

    TMouseArea {
        anchors.left: firstWrap.right
        width: 148
        height: 140

        onClicked: {
            if ( leftArrowImg.visible ) {
                sendEvent( TbtViewEvent.ArrowClicked )
                leftArrowCsw.forceLostFocus()
            }
        }
    }
}
