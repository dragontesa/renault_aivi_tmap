import QtQuick 2.0
import LayoutConstants 1.0
import "../../GlobalConstants.js" as GlobalConstants

QtObject {
    signal updateLayoutCompleted

    // default layout is 800 x 480
    property int screenResolution: LayoutConstants.L800x480
    property int viewWidth: GlobalConstants.kScreenWidth
    property int viewHeight: 294
    property int top3Height: 98
    property int gridCellHeight: 83 // 76
    property int gridCellWidth: (viewWidth-GlobalConstants.kScrollBarRightMargin*2+GlobalConstants.kScrollBarWidth-2)/kColumnCnt
    property int gridCellTopMargin: 4
    property int gridCellTopFreeMargin: 8
    property int gridIconSize: 18
    property int gridText1Height: 30
    property int gridText2Height: 16
    property int shadowBorderWidth: viewWidth
    property int shadowBorderHeight: 30
    property int gridFontSize: 28
    property real backgroundOpacity: 0.90 // requested by TMCRSM-600, TMCRSM-739

    function updateLayout(resolution) {
        screenResolution = resolution

        if ( resolution === LayoutConstants.L800x480 ) {
            viewHeight = 294
            top3Height = 98
            gridCellHeight = 83 // 76
            gridCellTopMargin = 4
            gridCellTopFreeMargin = 8
            gridIconSize = 18
            gridText1Height = 30
            gridText2Height = 16
        } else if ( resolution === LayoutConstants.L820x1024 ) {
            viewHeight = 450
            top3Height = 158
            gridCellHeight = 130
            gridCellTopMargin = 16
            gridCellTopFreeMargin = 22
            gridIconSize = 26
            gridText1Height = 56
            gridText2Height = 26
        }

        updateLayoutCompleted()
    }
}
