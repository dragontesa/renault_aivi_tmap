import QtQuick 2.1
import Constants 1.0

import "../component"

TFocusItem {
    id: root
    signal viewCompleted(variant view, string source)
    signal viewVisibleChanged(variant view)
    signal viewDestruction(variant view, variant data)    

    property string className: "__View__"
    property string url
    property int regId
    signal mediated
    signal activated
    signal deactivated
    property bool active: false
    property var layout

    signal opened()
    signal closed()
    signal closing()
    signal onEvent( int event, var data )
    signal onDestroy( var view )

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

        onEvent( event, data )
    }

    function show() {
        root.visible = true

        opened()
    }

    function close() {
        closing()

        root.onDestroy( root )

        closed()
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
