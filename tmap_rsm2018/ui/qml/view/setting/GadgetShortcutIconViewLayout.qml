import QtQuick 2.1
import LayoutConstants 1.0
import "../../GlobalConstants.js" as GlobalConstants

QtObject {
    property int tileMenuExContentButtonYOffset
    property int tileMenuExContentTextYOffset

    Component.onCompleted: {
    }

    function updateLayout( layout ) {
        switch(layout) {
            case  LayoutConstants.L820x1024:
                tileMenuExContentButtonYOffset = 44
                tileMenuExContentTextYOffset = 30
                break
            default:
                tileMenuExContentButtonYOffset = 0
                tileMenuExContentTextYOffset = 70
                break
        }
    }
}


