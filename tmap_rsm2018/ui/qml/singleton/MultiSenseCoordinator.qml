pragma Singleton

import QtQuick 2.1
import MultiSenseConstants 1.0

QtObject {
    property int colorState: MultiSenseManager.colorState
    property var kAmbientColorTable : [
        Qt.rgba(0.0, 0.471, 0.941, 1.0),    // Unavailable
        Qt.rgba(0.549, 0.627, 0.784, 1.0),  // Regular
        Qt.rgba(0.0, 0.471, 0.941, 1.0),    // Comfort
        Qt.rgba(0.471, 0.863, 0.0, 1.0),    // Echo
        Qt.rgba(1.0, 0.0, 0.0, 1.0),        // Dynamic(Sport)
        Qt.rgba(0.588, 0.0, 1.0, 1.0),      // Personal(MySense)
        Qt.rgba(0.0, 0.863, 1.0, 1.0),      // Race
        Qt.rgba(1.0, 0.784, 0.0, 1.0),      // Snow
        Qt.rgba(1.0, 0.353, 0.0, 1.0),
        Qt.rgba(0.0, 0.471, 0.941, 1.0),
        Qt.rgba(0.0, 0.471, 0.941, 1.0),
        Qt.rgba(0.0, 0.471, 0.941, 1.0),
        Qt.rgba(0.0, 0.471, 0.941, 1.0),
        Qt.rgba(0.0, 0.471, 0.941, 1.0),
        Qt.rgba(0.0, 0.471, 0.941, 1.0),
        Qt.rgba(0.0, 0.471, 0.941, 1.0),
    ]
    property color kAmbientColor

    Component.onCompleted: {
        update()
    }

    onColorStateChanged: {
        update()
    }

    function update() {
        console.log('colorState changed = ' + colorState)
        kAmbientColor = kAmbientColorTable[colorState]
    }
    
    function imageUrl(source) {
        var retVal = "";
        return Resource.imageUrl(retVal)
    }
}
