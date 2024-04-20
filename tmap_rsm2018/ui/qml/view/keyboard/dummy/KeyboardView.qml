import QtQuick 2.0
import KeyboardViewEvent 1.0

import "../../../component"
import "../../../view"

View {
    id: root
    visible: false

    property var inputField: null
    property string owner
    property bool dontVisible: false
    property var keyboard: null

    function loadSwKeyboard(){}

    onMediated: {
        sendEvent(KeyboardViewEvent.Mediated)
    }

    onActivated: {
        sendEvent(KeyboardViewEvent.Activated)
    }
}
