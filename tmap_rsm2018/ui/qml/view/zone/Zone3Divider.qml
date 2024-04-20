import QtQuick 2.0

Item {
    id: root
    property var layout
    property alias lineHeight: divider.height

    Rectangle {
        id: divider

        width: 1
        height: 37
        color: '#8a8a99'
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
    }
}
