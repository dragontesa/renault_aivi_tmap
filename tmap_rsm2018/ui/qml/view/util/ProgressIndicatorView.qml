import QtQuick 2.1

import "../../component"
import "../../view"

PopupView {
    id: root

    property bool isReroute: false

    width: parent.width
    height: parent.height
    hasFocus: false
    modal: !isReroute

    MouseArea {
        anchors.fill: parent
        enabled: isReroute

        onPressed: {}
        onReleased: {}
        onClicked: {}
    }
	Image {
		id: loading
		anchors.centerIn: parent
        source: Resource.imageUrl( 'util', 'loading.png' )
		NumberAnimation on rotation {
			 from: 0
			 to: 360
             duration: 1000
			 running: loading.visible == true
			 loops: Animation.Infinite
		 }
	}
}
