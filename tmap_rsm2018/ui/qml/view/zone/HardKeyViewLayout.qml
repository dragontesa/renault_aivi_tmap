import QtQuick 2.1
import LayoutConstants 1.0

QtObject {
    property int buttonWidth: 40
    property int buttonHeight: 30
    property int buttonRadius: 5

    Component.onCompleted: {
    }

    function updateLayout( layout ) {
        if ( layout === LayoutConstants.L820x1024 ) {
            buttonWidth = 45
            buttonHeight = 45
        }
    }
}

