import QtQuick 2.0
import "../../view/util/CommonUtils.js" as CommonUtils
import "../../view/util/Formatter.js" as Formatter
import "../../GlobalConstants.js" as GlobalConstants

Item {
    id: root

    property string remainDistance
    property int remainTime

    width: 194
    height: 43

    onRemainTimeChanged: {
        var t = gpsTimeUpdated
                ? CommonUtils.getArrivalTimeUsingTime(gpsHour, gpsMin, gpsSec, remainTime)
                : CommonUtils.getArrivalTimeUsingDate(new Date(), remainTime)
        var h = CommonUtils.getHour(t)
        var m = CommonUtils.getMin(t)
        arrivalTime.setArrivalTime(h, m)
    }

    Rectangle {
        anchors.fill: parent
        color: GlobalConstants.kGadgetBrightBgColor
    }

    WidgetMediumRemainDistance {
        id: remainDistance

        anchors.left: parent.left
        anchors.leftMargin: 9
        anchors.top: parent.top
        anchors.topMargin: 7

        distance: root.remainDistance
    }
    WidgetMediumArrivalTime {
        id: arrivalTime

        anchors.left: parent.left
        anchors.leftMargin: 92
        anchors.top: remainDistance.top
        format: timeFormat
    }
}
