import QtQuick 2.0
import LayoutConstants 1.0
import "../../GlobalConstants.js" as GlobalConstants

QtObject {
    property int viewWidth: GlobalConstants.kScreenWidth
    property int viewHeight: 377
    property int summaryBarWidth: GlobalConstants.kScreenWidth
    property int summaryBarHeight: 56
    property int summaryItemWidth: GlobalConstants.kScreenWidth
    property int summaryItemHeight: 84
    property int screenResolution: 0
    property int rightPadding: 0
    property int bottomPadding: summaryItemHeight * 4 + GlobalConstants.kScreenZone3Height
    property var routeSummaryView: parent
    property real scale: 1.0
    property int destTextX: viewWidth - 225

    signal updateLayoutCompleted

    function updateLayout(resolution) {
        screenResolution = resolution

        if (resolution === LayoutConstants.L800x480) {
            viewWidth = GlobalConstants.kScreenWidth / 2
            summaryBarWidth = viewWidth
            summaryBarHeight = 32
            summaryItemWidth = viewWidth
            summaryItemHeight = 84/2
            scale = 0.5
            rightPadding = viewWidth
            bottomPadding = 0
            destTextX = viewWidth + 150
        }

        updateLayoutCompleted()
    }
}
