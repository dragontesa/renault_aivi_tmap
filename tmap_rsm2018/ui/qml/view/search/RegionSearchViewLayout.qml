import QtQuick 2.1
import LayoutConstants 1.0

QtObject {
    property int rowHeight:  64

    Component.onCompleted: {
    }

    function updateLayout( layout ) {
        if ( layout === LayoutConstants.L820x1024 ) {
            rowHeight = 84
        }
    }
}


