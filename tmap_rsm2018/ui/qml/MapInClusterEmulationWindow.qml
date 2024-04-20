import QtQuick 2.1
import QtQuick.Window 2.1
import "GlobalConstants.js" as GlobalConstants

Window {
    id: mapInClusterRoot
    objectName: "mapInClusterRoot"
    width: 800
    height: 400
    color: 'black'
    visible: true

    Item {
        id: mapInClusterWrap
        objectName: "mapInClusterWrap"
        anchors.fill: parent
        visible: true
        Component.onDestruction: {
        }
    }
}
