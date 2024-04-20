import QtQuick 2.0

Item {
    id: root

    property var visualItem
    property var view
    property bool acceptSwap: true
    property bool enableDrag: true

    signal enteredDropArea(var source, var dest)

    onVisualItemChanged: {
        if (visualItem !== undefined) {
            visualItem.parent = mouseArea
            mouseArea.drag.target = visualItem
        }
    }

    MouseArea {
        id: mouseArea

        anchors.fill: parent
        drag.axis: Drag.YAxis
        drag.minimumY: height
        drag.maximumY: view.height - height
        enabled: root.enableDrag

        Component.onCompleted: {
            if (!acceptSwap)
                mouseArea.drag.target = undefined
        }
        onReleased: {
            if (acceptSwap && visualItem !== undefined) {
                visualItem.parent = mouseArea
                visualItem.y = 0
            }
        }

        states: [
            State {
                when: mouseArea.pressed

                PropertyChanges {
                    target: visualItem
                    parent: view
                    Drag.active: mouseArea.drag.active
                }
            }
        ]
    } //MouseArea

    DropArea {
        id: dropArea

        anchors.fill: parent

        onEntered: {
            if (acceptSwap) {
                var src = drag.source
                var dest = visualItem

                root.enteredDropArea(drag.source, visualItem)
            }
        }
    }
} //Item
