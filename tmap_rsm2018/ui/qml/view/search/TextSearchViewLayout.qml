import QtQuick 2.1
import LayoutConstants 1.0
import "../../GlobalConstants.js" as GConstants

QtObject {
    property int rowHeight

    Component.onCompleted: {
    }

    function updateLayout( layout ) {
        switch(layout) {
            case  LayoutConstants.L820x1024:
                rowHeight = GConstants.kListTextItemHeight
                break
            default:
                rowHeight = 64
                break
        }
    }
}


