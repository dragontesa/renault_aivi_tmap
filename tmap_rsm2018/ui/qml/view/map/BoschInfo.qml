import QtQuick 2.1

Item {
    id: root
    property string tiltAngle
    Rectangle {
        anchors.fill: parent
        color: 'black'
        opacity: 0.25
    }

    Text {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        text: 'Tilt Angle: ' + root.tiltAngle
        font.bold: true
        font.pointSize: 10
        color: 'red'
    }
}

