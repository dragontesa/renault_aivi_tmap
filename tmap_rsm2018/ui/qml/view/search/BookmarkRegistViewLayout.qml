import QtQuick 2.0
import LayoutConstants 1.0
import "../../GlobalConstants.js" as GlobalConstants

QtObject {
    signal updateLayoutCompleted

    property int screenResolution: 0

    function updateLayout(resolution) {
        screenResolution = resolution

        updateLayoutCompleted()
    }
}
