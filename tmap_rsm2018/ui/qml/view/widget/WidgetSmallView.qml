import QtQuick 2.1

import "../../component"
import "../../view"
import "../../view/util"
import "../../GlobalConstants.js" as GlobalConstants
import "../../singleton"
import WidgetSmallViewEvent 1.0

View {
    id: root
    width: 194
    height: 191
    property var parentWindow
    property bool verticalFlip: false
    property string tbtFirstImageSource
    property var tbtFirstDistNumberImageSources
    property bool tbtReset
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
    property string lang: 'ko'
    property bool drawOutline: false

    Component.onDestruction: {
        if ( parentWindow ) {
            parentWindow.close()
        }
    }

    onMediated: {
        sendEvent(WidgetSmallViewEvent.Mediated)
        console.log( "WidgetSmallView")
    }
	
    function changeRgComponent(isRg) {
        loader.sourceComponent = undefined
        if (isRg) {
            loader.sourceComponent = withRg
        } else {
            loader.sourceComponent = noRg
        }
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
			WidgetSmallTbt {
				id: tbt
                firstImageSource: root.tbtFirstImageSource
                firstDistNumberImageSources: root.tbtFirstDistNumberImageSources
                reset: root.tbtReset
			}
            WidgetSmallRgStatusBar {
                id: statusBar
                anchors.bottom: parent.bottom
                remainDistance: root.remainDistanceText
                remainTime: root.remainTime
            }
        }
    }

    Component {
        id: noRg
        Item {
            anchors.fill: parent

            Rectangle {
                id: textBg
                width: parent.width
                height: 43
                color: GlobalConstants.kGadgetBrightBgColor
            }
            Rectangle {
                width: parent.width; height: parent.height - textBg.height
                anchors.top: textBg.bottom
                color: GlobalConstants.kGadgetBrightBgColor
            }

            Item {
                width: 178; height: 32
                anchors.centerIn: textBg
                Text {
                    anchors { fill: parent; leftMargin: 8; rightMargin: 8 }
                    font {family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(35)}
                    color: '#ffffff'
                    text: 'Navigation'
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                }
            }

            Item {
                width: 194; height: 148
                anchors.bottom: parent.bottom
                anchors.horizontalCenter: parent.horizontalCenter

                Image {
                    anchors.centerIn: parent
                    source: Resource.imageUrl( 'wid_ic_navi.png' )
                }
            }
        }
    }
}
