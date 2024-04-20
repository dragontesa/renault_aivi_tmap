import QtQuick 2.0
import LayoutConstants 1.0
import "../../GlobalConstants.js" as GlobalConstants

QtObject {
    signal updateLayoutCompleted

    property int screenResolution: 0
    property int scale: 1.0
    property int topMargin: 10
    property int rightMargin: 10

    function updateLayout(resolution) {
        screenResolution = resolution

        if (resolution === LayoutConstants.L800x480) {
            scale = 0.5
            topMargin = -64
            rightMargin = 8
        }

        updateLayoutCompleted()
    }
}
