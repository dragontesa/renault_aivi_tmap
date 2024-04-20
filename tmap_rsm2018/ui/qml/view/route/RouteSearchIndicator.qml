import QtQuick 2.0

Item {
    property int index: 0

    visible: index < 3

    width: index !== 2 ? layout.itemWidth : layout.itemWidth - layout.itemWidthPadding
    height: layout.itemHeight

    Image {
        id: loadingImg

        anchors.centerIn: parent
        source: Resource.imageUrl("util/loading.png")

        NumberAnimation on rotation {
             from: 0
             to: 360
             duration: 1000
             running: loadingImg.visible
             loops: Animation.Infinite
         }
    }
}
