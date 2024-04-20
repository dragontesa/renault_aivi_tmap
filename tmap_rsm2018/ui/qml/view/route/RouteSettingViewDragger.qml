import QtQuick 2.0
import "../../GlobalConstants.js" as GlobalConstants

Item {
    id: root

    property bool focused: false

    width: layout.draggerWidth
    height: layout.draggerHeight

    signal draggerClicked()

    Image {
        anchors.centerIn: parent
        width: layout.draggerWidth
        height: layout.draggerHeight
        source: focused ? Resource.imageUrl("route/route_ic_move_f.png")
                        : Resource.imageUrl("route/route_ic_move.png")
        opacity: root.enabled ? 1.0 : 0.3
    }
}
