import QtQuick 2.1
import LayoutConstants 1.0
import "../../GlobalConstants.js" as GlobalConstants

QtObject {
    property int tileContentsHeight
    property int tileMenuContentButtonYOffset
    property int tileMenuContentTextYOffset
    property int tileMenuExContentButtonYOffset
    property int tileMenuExContentTextYOffset
    property int tileMenuContentTextLineHeight
    property int rowHeight

    Component.onCompleted: {
    }

    function updateLayout( layout ) {
        switch(layout) {
            case LayoutConstants.L820x1024:
                tileContentsHeight = 337
                tileMenuContentButtonYOffset = 11
                tileMenuContentTextYOffset = 95
                tileMenuExContentButtonYOffset = 58
                tileMenuExContentTextYOffset = 151
                tileMenuContentTextLineHeight = 32
                rowHeight = 84                
                break                
            default:
                tileContentsHeight = (GlobalConstants.kScreenContentHeight/2)
                tileMenuContentButtonYOffset = 0
                tileMenuContentTextYOffset = 70
                tileMenuExContentButtonYOffset = 0
                tileMenuExContentTextYOffset = 70
                tileMenuContentTextLineHeight = 32
                rowHeight = 64                
                break
        }
    }
}


