import QtQuick 2.1

TFocusItem {
    id: root
    property string title
    property alias background: background
    property alias backgroundImage: backgroundImage
    property alias fader: fadeRect
    property alias content: stack.children
	state: ""
	visible: false	
    width: parent ? parent.width : undefined
    height: parent ? parent.height : undefined
    property bool modal: false
    property bool enableFaderClose: false

	signal opened()
	signal closed()
    signal closing()
    signal onDestroy( var view )

	function show () {
        root.visible = true;

		opened()
	}

    function close() {
        closing()

        root.visible = false
        root.onDestroy( root )

		closed()
	}

    Component.onCompleted: {

    }

    TFader {
        id: fadeRect
        anchors.fill: parent

        // Bind main dialog's state with fader's corresponding states.
        state: modal && root.visible ? "faded" : ""
        transitionDuration: 300 // Use the same transition time as for the main dialog        
        visible: modal ? true : false

        onClickCaptured: {
            if (enableFaderClose)
                close()
        }
    }

    Item {
        id: contentArea
        anchors.fill: parent

        Rectangle {
            id: background
            anchors.fill: parent
            color: 'white'
            visible: false

            Image {
                id: backgroundImage
                anchors.fill: parent;
                //fillMode: Image.PreserveAspectFit
                fillMode: Image.Stretch

                onSourceChanged:  {
                }
            }
        }
        Item {
            id: stack
            anchors.fill: background
        }
    }
}
