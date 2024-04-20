import QtQuick 2.1
import QtQuick.Window 2.1
import ApplicationEvent 1.0
import ApplicationConstants 1.0
import CommonConstants 1.0
import TestConstants 1.0
import "GlobalConstants.js" as GlobalConstants
import QtTest 1.0
import SceneGraphRendering 1.0

import "./view/map"

Rectangle {
    id: mapInClusterRoot
    objectName: "mapInClusterRoot"
    width: 800
    height: 400
    color: 'black'
    Item {
        id: mapInClusterWrap
        objectName: "mapInClusterWrap"
        anchors.fill: parent
        visible: true
        Component.onDestruction: {
        }
    }
}
