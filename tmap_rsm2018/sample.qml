import QtQuick 2.0
import QtQuick.Window 2.2

Window {
    id: root
    color: 'white'
    width: 800
    height: 480
    visible: true

    Item {
        anchors.fill: parent
        Text {
            id: text1
            text: qsTr("Please insert usb stick to install navigation data.")
            anchors.fill: parent
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font.pointSize: 20
        }
    }
}
