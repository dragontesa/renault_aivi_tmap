import QtQuick 2.1
import QtQuick.Window 2.1
import "GlobalConstants.js" as GlobalConstants
import WidgetConstants 1.0
import ApplicationEvent 1.0

Window {
    id: gadgetRoot
    objectName: "gadgetRoot"
    width: 800
    height: 1024
    //color: 'red'
    color: 'black'
    visible: true    
    property int mapType
    property var appObj

    function clear() {
        gadgetXxl.source = ""
        gadgetXl.source = ""
        gadgetL.source = ""
        gadgetM.source = ""
        gadgetS.source = ""
        gadgetTest.source = ""
    }

    function clearXxl() {
        gadgetXxl.source = ""
    }

    function clearXl() {
        gadgetXl.source = ""
    }

    function clearL() {
        gadgetL.source = ""
    }

    function clearM() {
        gadgetM.source = ""
    }

    function clearS() {
        gadgetS.source = ""
    }

    onMapTypeChanged: {
        if ( mapType === WidgetConstants.SizeXXL ) {
            gadgetXxl.z = 100
            gadgetXl.z = 1
            gadgetXl.source = ""
        }
        else if ( mapType === WidgetConstants.SizeXL ) {
            gadgetXxl.z = 1
            gadgetXl.z = 100
            gadgetXxl.source = ""
        }
    }

    Image {
        id: bg
        source: Resource.imageUrl("gadget/hmi_bg.bmp")
        //z: 999

        onSourceChanged: {
            //console.log('[Gadget]source: ' + source);
        }
    }

    Image {
        id: gadgetXxl
        objectName: "gadgetXxl"
        //source: "image://ImageProvider/gadgetImage"
        y: 1024 - 390

        onSourceChanged: {
            //console.log('[Gadget]source: ' + source);
        }

        MouseArea {
            anchors.fill: parent
            onPressedChanged: {
                appObj.__sendEvent( ApplicationEvent.TestGadgetPressed, {type: WidgetConstants.SizeXXL, pressed: pressed} )
            }
        }
    }

    Image {
        id: gadgetXl
        objectName: "gadgetXl"
        y: 1024 - 390

        onSourceChanged: {
            //console.log('[Gadget]source: ' + source);
        }

        MouseArea {
            anchors.fill: parent
            onPressedChanged: {
                appObj.__sendEvent( ApplicationEvent.TestGadgetPressed, {type: WidgetConstants.SizeXL, pressed: pressed} )
            }
        }
    }

    Image {
        id: gadgetL
        objectName: "gadgetL"
        y: 1024 - 390 - 390

        onSourceChanged: {
            //console.log('[Gadget]source: ' + source);
        }

        MouseArea {
            anchors.fill: parent
            onPressedChanged: {
                appObj.__sendEvent( ApplicationEvent.TestGadgetPressed, {type: WidgetConstants.SizeLarge, pressed: pressed} )
            }
        }
    }

    Image {
        id: gadgetM
        objectName: "gadgetM"
        x: 396
        y: 1024 - 390 - 191

        onSourceChanged: {
            //console.log('[Gadget]source: ' + source);
        }

        MouseArea {
            anchors.fill: parent
            onPressedChanged: {
                appObj.__sendEvent( ApplicationEvent.TestGadgetPressed, {type: WidgetConstants.SizeMedium, pressed: pressed} )
            }
        }
    }

    Image {
        id: gadgetS
        objectName: "gadgetS"
        x: 396
        y: 1024 - 390 - 191 - 191

        onSourceChanged: {
            //console.log('[Gadget]source: ' + source);
        }

        MouseArea {
            anchors.fill: parent
            onPressedChanged: {
                appObj.__sendEvent( ApplicationEvent.TestGadgetPressed, {type: WidgetConstants.SizeSmall, pressed: pressed} )
            }
        }
    }

    Image {
        id: gadgetTest
        objectName: "gadgetTest"
        //source: "image://ImageProvider/gadgetImage"
        //y: 1024 - 390

        onSourceChanged: {
            //console.log('[Gadget]source: ' + source);
        }

        z: 999
    }
}
