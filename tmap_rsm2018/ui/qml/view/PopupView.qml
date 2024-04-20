import QtQuick 2.1
import Constants 1.0
import "../component"
import "../view"

TPopup {
    id: root
    signal viewVisibleChanged(variant view)
    signal viewDestruction(variant view, variant data)    

    property string className: "__PopupView__"
    property string url
    property int regId
    property bool active: false
    property var layout
    signal mediated
    signal activated
    signal deactivated    
    signal onEvent( int event, var data )

    function viewMediated() {
        root.mediated()
    }

    function viewActivated() {
        active = true
        root.activated()
    }

    function viewDeactivated() {
        active = false
        root.deactivated()
    }

    function sendEvent( event, data ) {
        if ( data == undefined ) {
            data = {}
        }

        root.onEvent( event, data )
    }

    Component.onCompleted: {
    }

    Component.onDestruction: {
        if ( root ) {
            var data = {url: url, view: root}
            root.viewDestruction( root, data )
        }
    }

    onVisibleChanged: {
        if ( root ) {
            root.viewVisibleChanged( root )
        }
    }
}
