import QtQuick 2.1
import LayoutConstants 1.0
import "../../GlobalConstants.js" as GlobalConstants

QtObject {
    property int subTitleHeight
    property int mapModeHeight
    property int otherRouteHeight
    property int botTitleHeight
    property int contentTextLineHeight

    Component.onCompleted: {}

    function updateLayout( layout ) {
        switch(layout) {
            case  LayoutConstants.L820x1024:
                subTitleHeight = 90
                mapModeHeight = 198
                otherRouteHeight = 205
                botTitleHeight = 62
                contentTextLineHeight = 32
                break
            default:
                subTitleHeight = 90
                mapModeHeight = 198
                otherRouteHeight = 205
                botTitleHeight = 62
                contentTextLineHeight = 32
                break
        }
    }
}

