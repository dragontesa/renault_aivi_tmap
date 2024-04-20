import QtQuick 2.1
import LayoutConstants 1.0
import "../../GlobalConstants.js" as GlobalConstants

QtObject {
    property int tileContentsHeight
    property int tileMenuContentButtonYOffset
    property int tileMenuContentTextYOffset
    property int tileMenuContentTextLineHeight
    property int dialogContentWidth
    property int dialogContentHeight

    Component.onCompleted: {}

    function updateLayout( layout ) {
        switch(layout) {
            case LayoutConstants.L820x1024:
                tileContentsHeight = 337
                tileMenuContentButtonYOffset = 11
                tileMenuContentTextYOffset = 95
                tileMenuContentTextLineHeight = 32
                dialogContentWidth = GlobalConstants.kDialogContentWidth + 100
                dialogContentHeight = GlobalConstants.kDialogContentHeight
                break
            case LayoutConstants.L1920x720:
                tileContentsHeight = 272
                tileMenuContentButtonYOffset = 0
                tileMenuContentTextYOffset = 70
                tileMenuContentTextLineHeight = 32
                dialogContentWidth = GlobalConstants.kDialogContentWidth
                dialogContentHeight = GlobalConstants.kDialogContentHeight
                break
            default:
                tileContentsHeight = (GlobalConstants.kScreenContentHeight/2)
                tileMenuContentButtonYOffset = 0
                tileMenuContentTextYOffset = 70
                tileMenuContentTextLineHeight = 32
                dialogContentWidth = GlobalConstants.kDialogContentWidth
                dialogContentHeight = GlobalConstants.kDialogContentHeight
                break
        }
    }
}


