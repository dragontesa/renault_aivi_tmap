import QtQuick 2.1
import LayoutConstants 1.0
import "../../GlobalConstants.js" as GlobalConstants

QtObject {
    Component.onCompleted: {}

    function updateLayout( layout ) {
        switch(layout) {
            case LayoutConstants.L820x1024:
            case LayoutConstants.L1920x720:
                break
            default:
                break
        }
    }
}
