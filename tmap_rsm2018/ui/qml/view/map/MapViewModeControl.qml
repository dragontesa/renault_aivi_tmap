import QtQuick 2.0

Item {
    id: root

    property int mode: 0
    function setNextViewMode() {
        switch (mode) {
        case 0: mode = 1
            break
        case 1: mode = 2
            break
        case 2: mode = 0
            break
        }

        viewModeChanged(mode)
    }

    signal viewModeChanged(int viewMode)

    width: 32
    height: 32

    Rectangle {
        anchors.fill: parent
        color: "black"

        Image {
            id: image

            property var sources: [
                "map_btn_birdview.png",
                "map_btn_compass.png",
                "map_btn_north.png"
            ]
            anchors.fill: parent
            fillMode: Image.PreserveAspectFit
            source: Resource.imageUrl(sources[mode])

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    QmlAppHelper.debug( "<B>MapViewModeControl!</B>")
                    setNextViewMode()
                }
            }
        }
    }
}
