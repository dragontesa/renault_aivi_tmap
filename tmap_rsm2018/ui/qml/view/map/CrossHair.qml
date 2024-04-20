import QtQuick 2.0

Item {
    id: root
    anchors.centerIn: parent
    width: image.width * scale
    height: image.height * scale

    property alias scale: image.scale

    Image {
        id: image

        anchors.centerIn: parent
        scale: 0.5
        source: Resource.imageUrl("map_target.png")
    }
}
