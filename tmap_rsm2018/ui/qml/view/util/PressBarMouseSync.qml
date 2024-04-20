import QtQuick 2.0
import CommonConstants 1.0
import "../../GlobalConstants.js" as GlobalConstants
import "../../view/util/CommonUtils.js" as CommonUtils
import "../../component"

Item {
    id: root
    width: parent.width
    height: parent.height
    property var currentItem    : null
    property var timer          : null
    property var pressed        : parent.pressed
    property var object         : root.parent
    property var sleep          : GlobalConstants.kLongPressedColorExposureTime
    property var cswObj         : null

    Component.onDestruction: {
        timer = CommonUtils.destroy(timer)
    }

    function pressedFunction() {
        if (!CommonUtils.isNull(cswObj) ) {
            cswObj.forceLostFocus()
        }
        timer = CommonUtils.destroy(timer)
        timer = CommonUtils.delay(root, sleep, function() {
            try {
                currentItem = object
            } catch(e) {
            }
        }, false)
    }

    function releaseFunction() {
        timer = CommonUtils.destroy(timer)
        currentItem = null
    }

    onPressedChanged: {
        if (CommonUtils.isNull(pressed))
            return
        if (pressed)
            pressedFunction()
        else
            releaseFunction()
    }    

}
