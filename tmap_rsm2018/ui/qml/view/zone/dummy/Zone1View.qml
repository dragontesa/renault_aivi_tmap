import QtQuick 2.0
import "../../../GlobalConstants.js" as GlobalConstants
import "../../../view"
import Zone1ViewEvent 1.0

View {
    id: root

    objectName: "Zone1View"
    property int screenWidth: GlobalConstants.kScreenWidth
    property bool noMap: false

    width: screenWidth
    height: GlobalConstants.kScreenZone1Height

    function setTextInput(str) {
        compTextInput.visible = true
    }
    function setTitle(str) {
        compTextInput.visible = false
    }
    function __inputFieldForceActiveFocus() {
        forceActiveFocus()
        inputField.forceActiveFocus()
    }
    onMediated: {
        sendEvent(Zone1ViewEvent.Mediated, {keyboardInput: inputField})
        sendEvent(Zone1ViewEvent.RegistCommonKeyboardInput, {keyboardInput: inputField})
    }
    Rectangle {
        anchors.fill: parent

        Item {
            id: imageBackground

            width: parent.width
            height: parent.height

            Item {
                id: compTextInput

                property alias text: inputField.text
                property alias maximumLength : inputField.maximumLength
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: parent.bottom
                width: childrenRect.width
                height: childrenRect.height

                Item {
                    y: -9
                    width: imageBackground.width
                    height: 52

                    TextInput {
                        id: inputField

                        Component.onCompleted: {
                            sendEvent( Zone1ViewEvent.RegistKeyboardInput, {keyboardInput: inputField})
                        }
                        onVisibleChanged: {
                            if (visible)
                                sendEvent( Zone1ViewEvent.RegistKeyboardInput, {keyboardInput: inputField})
                        }
                        focus: false
                        anchors { fill: parent; leftMargin: 180; rightMargin: 180 }
                        onActiveFocusChanged: {
                            if ( activeFocus && visible ) {
                                sendEvent( Zone1ViewEvent.RegistKeyboardInput, {keyboardInput: inputField})
                                sendEvent( Zone1ViewEvent.TextInputFocusActivated )
                            }
                        }
                        FocusScope {
                            id: inputFieldFocusScope
                            focus: true
                            anchors.fill: parent
                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    inputFieldFocusScope.forceActiveFocus()
                                    inputField.forceActiveFocus()
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
