import QtQuick 2.1
import LayoutConstants 1.0
import "../../GlobalConstants.js" as GlobalConstants

QtObject {
    property int checkboxWith
    property int checkboxHeight
    property int dialogContentWidth
    property int dialogContentHeight
    property int textLineHeight

    Component.onCompleted: {}

    function updateLayout( layout ) {
        switch(layout) {
            case LayoutConstants.L820x1024:
                checkboxWith = 63
                checkboxHeight = 63
                dialogContentWidth = GlobalConstants.kDialogContentWidth + 50
                dialogContentHeight = GlobalConstants.kDialogContentHeight
                textLineHeight = 34
                break
            case LayoutConstants.L1920x720:
                checkboxWith = 63
                checkboxHeight = 63
                dialogContentWidth = GlobalConstants.kDialogContentWidth
                dialogContentHeight = GlobalConstants.kDialogContentHeight
                textLineHeight = 34
                break
            default:
                checkboxWith = 30
                checkboxHeight = 30
                dialogContentWidth = GlobalConstants.kDialogContentWidth
                dialogContentHeight = GlobalConstants.kDialogContentHeight
                textLineHeight = 34
                break

        }
    }
}
