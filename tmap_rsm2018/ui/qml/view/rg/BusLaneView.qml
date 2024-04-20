import QtQuick 2.1
import BusLaneViewEvent 1.0
import "../../component"
import "../../view"

View {
    id: root
    width: busLimitFreeImage.width
    height: busLimitFreeImage.height
    visible: false
    hasFocus: false
    property int autoHideTimer: 8000
    property string lang;

    function hide() {
        visible = false
        busLimitFree.isLimitFree = false
    }

    onMediated: {
        sendEvent( BusLaneViewEvent.Mediated )
    }

    function retranslateUi(lang) {
        root.lang = lang
    }

    Timer {
        id: timer
        interval: root.autoHideTimer
        running: false
        repeat: false
        onTriggered: {
            root.hide()
        }
    }

    Item  {
        id: busLimitFree
        objectName: "busLimitFree"
        width: busLimitFreeImage.width
        height: busLimitFreeImage.height
        property bool isLimitFree : false
        visible: isLimitFree
        Rectangle {
            anchors.fill: parent
            color: "#2e3239"
            opacity: 0.94
        }
        Image {
            id: busLimitFreeImage
            source: {
                if (lang === "en")
                    return Resource.imageUrl( 'highway', 'highway_bus_en.png' )
                else
                    return Resource.imageUrl( 'highway', 'highway_bus.png' )
            }
            opacity: 0.94
        }

        onVisibleChanged: {
            if ( visible ) {
                timer.running = true
            }
        }

        onIsLimitFreeChanged: {
            root.visible = isLimitFree
        }
    }
}
