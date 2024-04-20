import QtQuick 2.1
import '../../GlobalConstants.js' as GlobalConstants
Item {
    id: root
    width: parent ? parent.width : undefined
    height: 136
    anchors.horizontalCenter: parent ? parent.horizontalCenter : undefined

    property string message
    property bool enableAutoClose: true
    property int autoCloseTimerInterval: 3000

    state: "invisible"
    states: [
        State {
            name: "invisible"
            PropertyChanges { target: messageText; opacity: 0.0 }
        },
        State {
            name: "visible"
            PropertyChanges { target: messageText; opacity: 1.0 }
        }
    ]

    Rectangle {
        anchors.fill: parent
        color: "#000000"
        opacity: 0.95
        Text {
            id: messageText

            anchors { left: parent.left; leftMargin: 72;
                right: parent.right; rightMargin: 72;
                top: parent.top; topMargin: 18 }
            height: 96
            font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(36) }
            color: "#ffffff"
            text: root.message
            horizontalAlignment: Text.AlignHCenter

            transitions: Transition {
                NumberAnimation { property: "opacity"; easing: Easing.InQuad; duration: 50 }
            }
        }
    }
    Rectangle {
        anchors { bottom: parent.bottom }
        width: parent.width; height: 1
        color: '#ffffff'
    }

    Timer {
        id: autoCloseTimer
        interval: root.autoCloseTimerInterval
        running: false
        repeat: false
        onTriggered: {
            root.close()
        }
    }

    Timer {
        id: closeTimer
        interval: 3500
        running: false
        repeat: false
        onTriggered: {
            root.hide()
        }
    }

    function show () {
        root.visible = true;

        if ( enableAutoClose ) {
            autoCloseTimer.running = true
        } else {
            closeTimer.running = true
        }
    }

    function close() {
        if ( root ) {
            root.destroy()
        }
    }
    function hide() {
        root.visible = false
    }
}
