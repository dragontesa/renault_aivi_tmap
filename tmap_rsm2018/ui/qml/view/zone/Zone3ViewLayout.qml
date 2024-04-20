import QtQuick 2.1
import LayoutConstants 1.0

QtObject {
    property real ratio: 0.5
    property real iconWidth: 140.0
    property int naviMenuFontPixelSize: 22
    property int menuButtonLineHeight: 37
    property int backButtonLineHeight: 37
    property int optionButtonLineHeight: 37
    property int naviAddressSpacing: 6
    property int naviAddressTextHeight: 26
    property int naviAddressTextPixelSize: 22*0.75
    property int naviAddressRgTextMaxWidth: 216
    property int naviAddressTextMaxWidth: 520+140+-32
    property int naviRouteGuideRowWidth: 6+120+126
    property int naviRouteGuideRowItem1Width: 6
    property int naviRouteGuideRowItem2Width: 120
    property int naviRouteGuideRowItem3Width: 126
    property int naviRouteGuideRemainDistanceX: 6
    property int naviRouteGuideRemainTimeLeftMargin: 120+60
    property int naviRouteGuideArrivalTimeLeftMargin: 120
    property int toolMenuWidth: 312
    property int toolMenuHeight: 32
    property int toolMenuListViewMargins: 2
    property int toolMenuListViewHighlightMargins: 4

    Component.onCompleted: {
    }

    function updateLayout( layout ) {
        if ( layout === LayoutConstants.L820x1024 ) {
            ratio = 1.0
            naviAddressTextPixelSize = 22
            naviAddressTextMaxWidth = 540-32
        }
    }
}


