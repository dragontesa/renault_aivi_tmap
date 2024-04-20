import QtQuick 2.1
import LayoutConstants 1.0
import "../../GlobalConstants.js" as GlobalConstants

QtObject {
    property int rowHeight
    property int titleRange
    property int telBtnSize
    property int stationIconWidth
    property int stationIconHeight
    property int listItemTopMargin
    property int listItemBotMargin
    property int mapviewHeight


    Component.onCompleted: {
    }

    function updateLayout( layout ) {
        switch(layout) {
            case  LayoutConstants.L820x1024:
                rowHeight = 50
                titleRange = GlobalConstants.kListTwoTextItemHeight
                telBtnSize = 63
                stationIconWidth = 100
                stationIconHeight = 100
                listItemTopMargin = 20
                listItemBotMargin = 23
                mapviewHeight = 310
                break
            default:
                rowHeight = 50
                titleRange = GlobalConstants.kListTwoTextItemHeight
                telBtnSize = 43
                stationIconWidth = 100
                stationIconHeight = 100
                listItemTopMargin = 15
                listItemBotMargin = 15
                mapviewHeight = (GlobalConstants.kScreenContentHeight/3)
                break
        }
    }
}


