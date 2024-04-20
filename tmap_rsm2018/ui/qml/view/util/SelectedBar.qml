import QtQuick 2.0
import CommonConstants 1.0

import "../../singleton"
import "../../component"
import "../../GlobalConstants.js" as GlobalConstants
import "../../view/util/CommonUtils.js" as CommonUtils

Rectangle {
    property var currentItem    : null
    property real leftMargin    : 0
    property real rightMargin   : 0
    property real topMargin     : 0
    property real bottomMargin  : 0
    property int  zOrderOffset  : -1
    property bool isFocused     : false
    onCurrentItemChanged: {
        parent = Qt.binding(function(){return currentItem ? currentItem.parent : null})
        x = Qt.binding(function(){return currentItem ? currentItem.x + leftMargin : 0})
        y = Qt.binding(function(){return currentItem ? currentItem.y + topMargin : 0})
        z = Qt.binding(function(){return currentItem ? currentItem.z + zOrderOffset : 0})
        width = Qt.binding(function(){return currentItem ? currentItem.width  - (leftMargin + rightMargin) : 0})
        height = Qt.binding(function(){return currentItem ? currentItem.height - (topMargin + bottomMargin) : 0})
        visible = Qt.binding(function(){return currentItem ? currentItem.visible : false})
    }
}
