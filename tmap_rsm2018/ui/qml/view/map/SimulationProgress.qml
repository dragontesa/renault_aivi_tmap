import QtQuick 2.0

Item {
    id: root
    property real progress: 0.0
    property bool pressed: false

    signal progressUpdated(real progress)

    width: 69
    height: 270

    onProgressChanged: {
        if (!pressed) {
            progressBar.internalProgress = progress
        }
    }

    Image {
        id: bg
        anchors.horizontalCenter: parent.horizontalCenter
        source: Resource.imageUrl("route/simulate_progress_bg.png")

        Item {
            id: column
            anchors.fill: parent

            Image {
                id: topItem
                anchors.horizontalCenter: parent.horizontalCenter
                source: Resource.imageUrl("route/simulate_progress_top.png")
            }
            Image {
                id: progressBar
                property real internalProgress: 0.0
                height: 250 * (1.0 - internalProgress)
                anchors.top: topItem.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                source: Resource.imageUrl("route/simulate_progress_middle.png")

                MouseArea {
                    id: mouseArea
                    height: 250
                    width: root.width
                    anchors {
                        top: parent.top
                        horizontalCenter: parent.horizontalCenter
                    }
                    onClicked: {
                        var ratio = mouse.y / mouseArea.height
                        var progress = Math.max(0.0, Math.min(1.0 - ratio, 1.0))
                        root.progressUpdated(progress)
                    }
                }
            }
            Image {
                id: bottomItem
                anchors.top: progressBar.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                source: Resource.imageUrl("route/simulate_progress_bottom.png")
            }
            Image {
                id: car
                anchors.horizontalCenter: progressBar.horizontalCenter
                anchors.verticalCenter: progressBar.bottom
                source: Resource.imageUrl("route/simulate_progress_cavata.png")
            }
        }
    }
}
