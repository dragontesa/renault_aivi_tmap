import QtQuick 2.0
import LayoutConstants 1.0
import "../../GlobalConstants.js" as GlobalConstants

QtObject {
    property int itemWidth: GlobalConstants.kScreenWidth
    property int itemHeight: 84
    property int iconWidth: 63
    property int iconHeight: 63
    property int draggerWidth: 63
    property int draggerHeight: 63
    property int draggerLeftMargin: 20
    property int leftItemWidth: 20+63+63+10+2
    property int verticalLineWidth: 2
    property int verticalLineHeight: 23
    property int horizontalLineWidth: 790
    property int horizontalLineHeight: 1
    property int horizontalLineLeftMargin: 10
    property int horizontalLineRightMargin: 20
    property int textWidth: 602
    property int textHeight: 32
    property int textLeftMargin: 20
    property int textRightMargin: 40
    property int delButtonWidth: 88
    property int delButtonHeight: 58
    property int delButtonY: 0
    property int delButtonRightMargin: 40
    property int fontSize: 35
    property int moveButtonSize: 85
    property int okButtonWidth: 85
    property int okButtonHeight: 158
    property int controllerVerticalSpacing: 4
    property int controllerTopMargin: 83
    property real arrowIconScale: 1.0
    property int mapPaddingY: 83

    signal updateLayoutCompleted

    function updateLayout(resolution) {
        if (resolution === LayoutConstants.L800x480) {
            itemWidth = 800
            itemHeight = 32
            iconWidth = 24
            iconHeight = 24
            leftItemWidth = 64
            draggerWidth = 24
            draggerHeight = 18
            draggerLeftMargin = 12
            verticalLineWidth = 1
            verticalLineHeight = 23/2
            horizontalLineWidth = GlobalConstants.kScreenWidth - (horizontalLineLeftMargin + horizontalLineRightMargin)
            textRightMargin = 20
            delButtonWidth = 88/2
            delButtonHeight = 58/2
            delButtonY = -(delButtonHeight/2)
            fontSize = 35/2
            moveButtonSize = 85 * 0.39
            okButtonWidth = 85 * 0.39
            okButtonHeight = 158 * 0.39
            controllerVerticalSpacing = 0
            controllerTopMargin = itemHeight
            arrowIconScale = 0.5
            mapPaddingY = 41
        }

        updateLayoutCompleted()
    }
}
