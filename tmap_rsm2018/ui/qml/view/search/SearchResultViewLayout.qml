import QtQuick 2.1
import LayoutConstants 1.0
import "../../GlobalConstants.js" as GlobalConstants

QtObject {
    property int rowHeight
    property int detailBtnWidth
    property int detailBtnHeight
    property int seqImageWidth
    property int seqImageHeight
    property int listItemTopMargin
    property int listItemBotMargin
    property int mapviewHeight

    Component.onCompleted: {}

    function updateLayout( layout ) {
        switch(layout) {
            case LayoutConstants.L820x1024:
                rowHeight = GlobalConstants.kListTwoTextItemHeight
                detailBtnWidth = 88
                detailBtnHeight = 58
                seqImageWidth = 63
                seqImageHeight = 63
                listItemTopMargin = 20
                listItemBotMargin = 23
                mapviewHeight = 310
                break
            default:
                rowHeight = GlobalConstants.kListTwoTextItemHeight
                detailBtnWidth = 80
                detailBtnHeight = 30
                seqImageWidth = 63
                seqImageHeight = 63
                listItemTopMargin = 12
                listItemBotMargin = 15
                mapviewHeight = (GlobalConstants.kScreenContentHeight/3)
                break
        }
    }
}


