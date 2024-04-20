import QtQuick 2.1
import LayoutConstants 1.0
import "../../GlobalConstants.js" as GlobalConstants

QtObject {
    property int radioboxWidth
    property int radioboxHeight
    property int carIconWidth
    property int carIconHeight

    Component.onCompleted: {}

    function updateLayout( layout ) {
        switch(layout) {
            case LayoutConstants.L820x1024:
            case LayoutConstants.L1920x720:
                radioboxWidth = 63
                radioboxHeight = 63
                carIconWidth =  170
                carIconHeight = 200
                break
            default:
                radioboxWidth = 16
                radioboxHeight = 16
                carIconWidth =  120
                carIconHeight = 150
                break
        }
    }
}


