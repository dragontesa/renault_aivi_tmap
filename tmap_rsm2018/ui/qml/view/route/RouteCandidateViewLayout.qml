import QtQuick 2.0
import LayoutConstants 1.0
import "../../GlobalConstants.js" as GlobalConstants

QtObject {
    property int itemWidthPadding: 1
    property int itemWidth: 273
    property int itemHeight: 200
    property int internalItemWidth: 273
    property int internalItemHeight: 200
    property int artaryWidth: GlobalConstants.kScreenWidth
    property int artaryHeight: 64
    property real internalItemScale: 1.0
    property int buttonSize: 69

    function updateLayout(resolution) {
        if (resolution === LayoutConstants.L800x480) {
            itemWidth = 267
            internalItemScale = 0.5
            internalItemWidth = itemWidth*2
            internalItemHeight = 200
            itemWidthPadding = 2
            itemHeight = 100
            artaryHeight = 64 * internalItemScale
            buttonSize = 34
        }
    }
}
