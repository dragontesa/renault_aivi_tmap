import QtQuick 2.1
import LayoutConstants 1.0
import "../../GlobalConstants.js" as GlobalConstants

QtObject {
    property int checkboxWith
    property int checkboxHeight

    Component.onCompleted: {}

    function updateLayout( layout ) {
        switch(layout) {
            case LayoutConstants.L820x1024:
            case LayoutConstants.L1920x720:
                checkboxWith = 63
                checkboxHeight = 63
                break
            default:
                checkboxWith = 30
                checkboxHeight = 30
                break
        }
    }
}


