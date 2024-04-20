import QtQuick 2.0

Item {
    id: root

    property var flickable
    property bool flickerMoving: false
    property real heightRatio: flickable.height / flickable.contentHeight
    property alias color: bar.color
    property alias showGroove: groove.visible
    property alias grooveColor: groove.color
    property alias grooveOpacity: groove.opacity
    property int rightMargin: 9
    property int topMargin: 10
    property int bottomMargin: 10

    property bool handleDrag: true
    property bool dynamicPage: false

    signal scroll(int value, int handleSize)
    signal mouseRelease()
    signal mousePress()
    signal arrivedBelow()
    anchors.right: flickable.right
    anchors.rightMargin: rightMargin
    anchors.top: flickable.top
    anchors.bottom: flickable.bottom
    anchors.topMargin: 10
    anchors.bottomMargin: 10
    width: 2
    height: flickable.height
    visible: (heightRatio < 1.0) && flickerMoving

    onFlickableChanged: {
        if (flickable !== null) {
            flickerMoving = Qt.binding(function(){ return flickable.moving })
        }
    }

    MouseArea {
        id: clicker
        anchors.fill: parent

        drag {
            target: handleDrag ? handle : undefined
            minimumY: 0
            maximumY: root.height - handle.height
            axis: Drag.YAxis
        }         

        onPressed: {
            mousePress()
        }

        onReleased:{
            mouseRelease()
        }

        onClicked: {
            if (handleDrag) {
                flickable.contentY = dynamicPage ? mouse.y / root.height * (flickable.contentHeight - flickable.height) + offsetY :
                                            mouse.y / root.height * (flickable.contentHeight - flickable.height)
            }
        }
    }

    Rectangle {
        id: groove
        visible: true
        anchors.fill: parent
        color: '#4c4c59'
    }

    property real offsetY: 0
    Item {
        id: handle
        anchors.left: parent.left
        anchors.right: parent.right
        clip: true

        height: Math.max(flickable.visibleArea.heightRatio * root.height, 16)
        onYChanged: {
            offsetY = flickable.contentY-Math.round(flickable.visibleArea.yPosition*flickable.contentHeight)
            scroll(y, height)
            if (y + height == root.height) {
                arrivedBelow()
            }
        }

        Rectangle {
            id: bar
            anchors.fill: parent
            color: 'gray'
            opacity: flickable.moving ? 1.0 : 1.0

            Behavior on opacity {
                NumberAnimation { easing.type: Easing.OutCubic; duration: 2000 }
            }
        }
    }

    Binding {
        target: handle
        property: "y"
        value: dynamicPage ? flickable.visibleArea.yPosition * root.height :
                             flickable.contentY * clicker.drag.maximumY / (flickable.contentHeight - flickable.height)
        when: !clicker.drag.active
    }

    Binding {
        target: flickable
        property: "contentY"
        value: dynamicPage ? (handle.y * (flickable.contentHeight - flickable.height) / clicker.drag.maximumY) + offsetY :
                             (handle.y * (flickable.contentHeight - flickable.height) / clicker.drag.maximumY)
        when: (clicker.drag.active || clicker.pressed) && handleDrag
    }
}
