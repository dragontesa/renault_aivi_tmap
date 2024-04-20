import QtQuick 2.1
import LayoutConstants 1.0
import "../../GlobalConstants.js" as GlobalConstants

QtObject {
    property int radioboxWidth
    property int radioboxHeight

    Component.onCompleted: {}

    function updateLayout( layout ) {
        switch(layout) {
            case LayoutConstants.L820x1024:
            case LayoutConstants.L1920x720:
                radioboxWidth = 63
                radioboxHeight = 63
                break
            default:
                radioboxWidth = 16
                radioboxHeight = 16
                break
        }
    }
}
