import QtQuick 2.1
import LayoutConstants 1.0

QtObject {
    property int compTextInputBgY: -4
    property int compTextInputBgHeight: 28

    property int compTextInputFieldLeftMargin: 72
    property int compTextInputFieldRightMargin: 72

    property int compTitleLabelWidth: 500
    property int compTitleLabelHeight: 32
    property int compTitleLabelFontPixelSize: 28*0.5
    property int compTitleLabelBottomMargin: 0

    Component.onCompleted: {        
    }

    function updateLayout( layout ) {
        if ( layout === LayoutConstants.L820x1024 ) {
            compTextInputBgY = -9
            compTextInputBgHeight = 52
            compTitleLabelWidth = 502
            compTitleLabelFontPixelSize = 35
            compTextInputFieldLeftMargin = 180
            compTextInputFieldRightMargin = 180
            compTitleLabelBottomMargin = 18
        }
    }
}

