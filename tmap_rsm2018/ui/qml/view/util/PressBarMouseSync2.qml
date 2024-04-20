import QtQuick 2.0
import "../../GlobalConstants.js" as GlobalConstants
import "../../view/util/CommonUtils.js" as CommonUtils

Item {
    id: root
    property var targetItem : null
    property var timer      : null
    property var pressed    : parent.pressed
    property var object     : parent
    property var sleep      : GlobalConstants.kLongPressedColorExposureTime

    Component.onDestruction: {
        timer = CommonUtils.destroy(timer)
    }

    function pressedFunction() {
        timer = CommonUtils.destroy(timer)
        timer = CommonUtils.delay(root, sleep, function() {
            try {
                if (targetItem) {
                    targetItem.state = 'pressed'
                }
            } catch(e) {
            }
        }, false)
    }

    function releaseFunction() {
        timer = CommonUtils.destroy(timer)
        if (targetItem) {
            targetItem.state = 'normal'
        }
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
