import QtQuick 2.1
import LayoutConstants 1.0
import "../../GlobalConstants.js" as GlobalConstants

QtObject {
    property int radioboxWidth
    property int radioboxHeight
    property int option2Width
    property int option2Height
    property int option2OnOffWidth
    property int option2OnOffHeight

    Component.onCompleted: {}

    function updateLayout( layout ) {
        switch(layout) {
            case LayoutConstants.L820x1024:
            case LayoutConstants.L1920x720:
                radioboxWidth = 63
                radioboxHeight = 63
                option2Width = 312
                option2Height = 52
                option2OnOffWidth = 140
                option2OnOffHeight = 51
                break
            default:
                radioboxWidth = 16
                radioboxHeight = 16
                option2Width = 100
                option2Height = 30
                option2OnOffWidth = 100
                option2OnOffHeight = 30
                break
        }
    }
}
