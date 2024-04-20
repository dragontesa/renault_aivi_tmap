import QtQuick 2.1
import LayoutConstants 1.0
import "../../GlobalConstants.js" as GlobalConstants

QtObject {
    property int option2Width
    property int option2Height
    property int option2OnOffWidth
    property int option2OnOffHeight
    property int listItemTopMargin
    property int listItemBotMargin


    Component.onCompleted: {}

    function updateLayout( layout ) {
        switch(layout) {
            case LayoutConstants.L820x1024:
            case LayoutConstants.L1920x720:
                option2Width = 322
                option2Height = 51
                option2OnOffWidth = 140
                option2OnOffHeight = 51
                listItemTopMargin = 20
                listItemBotMargin = 23
                break
            default:
                option2Width = 100
                option2Height = 30
                option2OnOffWidth = 100
                option2OnOffHeight = 30
                listItemTopMargin = 12
                listItemBotMargin = 15
                break
        }
    }
}
