import QtQuick 2.0

Item {
    property alias text: label.text
    property alias horizontalAlignment: label.horizontalAlignment
    property alias contentWidth: label.contentWidth
    property alias color: label.color

    Text {
        id: label

        anchors.fill: parent
        font.pixelSize: Resource.pixelToDp(20)
        color: "white"
        verticalAlignment: Text.AlignVCenter
    }
}
