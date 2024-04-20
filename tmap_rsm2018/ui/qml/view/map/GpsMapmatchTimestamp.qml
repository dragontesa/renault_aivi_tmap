import QtQuick 2.0
import CommonConstants 1.0
import "../../GlobalConstants.js" as GlobalConstants
import "../../view/util/CommonUtils.js" as CommonUtils

Item {
    id: root
    width: 320
    height: 30
    property string gpsTimestampData        : CommonConstants.Empty
    property string gpsMapmatchFeedbackData : CommonConstants.Empty
    property string gpsMmfData1st           : CommonConstants.Empty
    property string gpsMmfData2nd           : CommonConstants.Empty
    property bool gpsTimestampVisible       : false
    property bool gpsMapmatchFeedbackVisible: false
    readonly property color bgcolorTimeStamp: (gpsTimestampVisible        && !CommonUtils.isEmpty(gpsTimestampData))
                                              ? CommonConstants.SharpBlack : CommonConstants.Transparent
    readonly property color bgcolorMmfb     : (gpsMapmatchFeedbackVisible && !CommonUtils.isEmpty(gpsMapmatchFeedbackData))
                                              ? CommonConstants.SharpBlack : CommonConstants.Transparent
    readonly property real bgOpacity        : 1.0

    onGpsMapmatchFeedbackDataChanged: {
        if (CommonUtils.isEmpty(gpsMapmatchFeedbackData)) {
            gpsMmfData1st = CommonConstants.Empty
            gpsMmfData2nd = CommonConstants.Empty
            return
        }
        var p1st = CommonUtils.indexOf(gpsMapmatchFeedbackData, "\n----")
        var p2nd = CommonUtils.indexOf(gpsMapmatchFeedbackData, "----")
        gpsMmfData1st = CommonUtils.substring(gpsMapmatchFeedbackData, 0, p1st)
        gpsMmfData2nd = CommonUtils.substring(gpsMapmatchFeedbackData, p2nd)
    }
    Column {
        width: parent.width
        height: parent.height
        Item {width: parent.width;height:73}
        Rectangle {
            id: gpsTimestampWrap
            width: parent.width
            height: gpsTimestamp.contentHeight
            color: bgcolorTimeStamp
            opacity: bgOpacity
            Text {
                id: gpsTimestamp
                width: parent.width
                height: 31
                font { family: GlobalConstants.kFontFamily; pixelSize: GlobalConstants.kListTextFontSize}
                color: "yellow"
                horizontalAlignment: Text.AlignRight
                text: gpsTimestampData
                visible: gpsTimestampVisible
            }
        }
        Rectangle {
            id: gpsMmfbWrap
            width: parent.width
            height: gpsMmfb1st.contentHeight
            color: bgcolorMmfb
            opacity: bgOpacity
            Text {
                id: gpsMmfb1st
                width: parent.width
                height: 31
                wrapMode: Text.WordWrap
                horizontalAlignment: Text.AlignRight
                font { family: GlobalConstants.kFontFamily; pixelSize: GlobalConstants.kListSubTextFontSize}
                color: "yellow"
                text: gpsMmfData1st
                visible: gpsMapmatchFeedbackVisible
            }
        }
        Text {
            id: gpsMmfb2nd
            width: parent.width
            height: 31
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignRight
            font { family: GlobalConstants.kFontFamily; pixelSize: GlobalConstants.kListSubTextFontSize}
            color: "#1e90ff"
            text: gpsMmfData2nd
            visible: gpsMapmatchFeedbackVisible
        }
    }
}
