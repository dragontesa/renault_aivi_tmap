import QtQuick 2.1
import LayoutConstants 1.0

QtObject {
    property int tileMenuContentButtonYOffset: 0
    property int mapScaleX: 10
    property int mapScaleOffsetY: -287
    property real scale: 1.0

    Component.onCompleted: {
    }

    function updateLayout( layout ) {
        if ( layout === LayoutConstants.L820x1024 ) {
            tileMenuContentButtonYOffset = 50
        } else if (layout === LayoutConstants.L800x480) {
            scale = 0.5
            mapScaleX = 0
            mapScaleOffsetY = -216
        }
    }
}
