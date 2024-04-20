import QtQuick 2.1
//import QtGraphicalEffects 1.0

TFocusItem {
    id: root
    width: parent ? parent.width : undefined
    height: parent ? parent.height : undefined
    property string title
    property alias background: background
    default property alias content: stack.children
    property bool enableAutoClose: true
    property int autoCloseTimer: 5000
    property int contentWidth: width
    property int contentHeight: height

    property bool leftButtonVisible: true
    property bool rightButtonVisible: true
    property Component leftButton: TPushButton {
        label: qsTr("취소")
        onButtonClicked: {
            leftButtonClicked()
            close()
        }
    }
    property Component rightButton: TPushButton {
        label: qsTr("확인")
        onButtonClicked: {
            rightButtonClicked()
            close()
        }
    }

    state: ""
    visible: false

    signal opened()
    signal closed()

    signal leftButtonClicked()
    signal rightButtonClicked()

    function show () {
        if (leftButtonVisible)
            leftButtonLoader.sourceComponent = leftButton
        else
            leftButtonLoader.width = 0
        if (rightButtonVisible)
            rightButtonLoader.sourceComponent = rightButton
        else
            rightButtonLoader.width = 0

        root.visible = true;

        opened()

        if ( enableAutoClose ) {
            timer.start()
        }
    }

    function close() {
        root.visible = false
        root.onDestroy( root )
    }

    Component.onCompleted: {
    }

    Component.onDestruction: {
        closed()
    }

    Timer {
        id: timer
        interval: autoCloseTimer
        running: false
        repeat: false
        onTriggered: {
            close()
        }
    }
	
    TFader {
        id: fadeRect
        anchors.fill: parent

        // Bind main dialog's state with fader's corresponding states.
        state: modal && root.visible ? "faded" : ""
        transitionDuration: 300 // Use the same transition time as for the main dialog
        visible: true
    }	

    // debugging
//    Rectangle {
//        anchors.fill: parent
//        color: 'red'
//    }

    Item {
        id: contentArea
        objectName: 'contentArea'
        width: contentWidth
        height: contentHeight
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter

        Rectangle {
            id: background
            objectName: 'background'
            anchors.fill: parent
            color: 'gray'
            visible: true

            Rectangle {
                id: stack
                width: parent.width
                anchors.top: background.top
                anchors.bottom: buttons.top
                anchors.horizontalCenter: parent.horizontalCenter
                color: "white"
                border.color: 'darkgray'
                border.width: 1
            }

            Rectangle {
                id: buttons
                width: contentWidth
                height: 50
                anchors.bottom: background.bottom
                color: 'lightgray'
                border.color: 'lightgray'
                border.width: 1
                Row {
                    width: parent.width
                    height: parent.height
                    spacing: 1

                    Loader {
                        id: leftButtonLoader
                        width: rightButtonVisible ? parent.width / 2 : parent.width
                        height: parent.height
                    }
                    Loader {
                        id: rightButtonLoader
                        width: leftButtonVisible ? parent.width / 2 : parent.width
                        height: parent.height
                    }
                }
            }
        }
    }
}
