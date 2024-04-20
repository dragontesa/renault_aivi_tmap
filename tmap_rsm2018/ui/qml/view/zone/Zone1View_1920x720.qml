import QtQuick 2.0
import "../../GlobalConstants.js" as GlobalConstants
import "../../view"
import ApplicationEvent 1.0
import Zone1ViewEvent 1.0
import Csw 1.0

View {
    id: root

    //objectName: "Zone1View"
    //property real ratio: 800.0 / GlobalConstants.kScreenWidth
    property int screenWidth: GlobalConstants.kScreenWidth
    property string title

    property bool online: false
    property alias onlineStatusVisible: onlineStatus.visible

    width: screenWidth
    //height: GlobalConstants.kScreenZone1_Height * ratio * 0.5
    height: GlobalConstants.kScreenZone1Height

    //2017.11.10 cjlee - str인자 추가
    function setTextInput(str) {
        compTextInput.visible = true
        if(str != undefined)
           compTextInput.text = str
        compTitle.visible = false
    }
    function setTitle(str) {
        compTextInput.visible = false
        compTitle.visible = true
        compTitle.title = str
    }

    function __inputFieldForceActiveFocus() {
            forceActiveFocus()
            inputField.forceActiveFocus()
    }

    onWidthChanged: {
        console.log( "#1 width: " + width )
    }

    onMediated: {
        sendEvent(Zone1ViewEvent.Mediated, {keyboardInput: inputField})
        sendEvent(Zone1ViewEvent.RegistCsw, [csw])
    }

    Rectangle {
        anchors.fill: parent
        color: "black"

        Image {
            id: imageBackground

            anchors.centerIn: parent
            //height: 69
            height: parent.height
            fillMode: Image.PreserveAspectFit
            source: Resource.imageUrl("zone/zone1_dummy.png")

            Item {
                id: compTextInput

                property alias text: inputField.text
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: parent.bottom
                width: childrenRect.width
                height: childrenRect.height

                Image {
                    y: layout.compTextInputBgY
                    width: imageBackground.width
                    height: layout.compTextInputBgHeight

                    fillMode: Image.PreserveAspectFit
                    source: Resource.imageUrl("zone/search_input_bg.png")

                    TextInput {
                        id: inputField

                        Component.onCompleted: {
                            //console.log("compTextInput created...")
                                sendEvent( Zone1ViewEvent.RegistKeyboardInput, {keyboardInput: inputField})
                        }

                        onVisibleChanged: {
                            if (visible)
                                sendEvent( Zone1ViewEvent.RegistKeyboardInput, {keyboardInput: inputField})
                        }

                        focus: false
                        anchors { fill: parent; leftMargin: layout.compTextInputFieldLeftMargin; rightMargin: layout.compTextInputFieldRightMargin }
                        verticalAlignment: Qt.AlignVCenter
                        color: 'black'
                        clip: true

                        onActiveFocusChanged: {
                            console.log( 'TextInput.onActiveFocusChanged #1 activeFocus: ' + activeFocus )

                            if ( activeFocus && visible ) {
                                sendEvent( Zone1ViewEvent.RegistKeyboardInput, {keyboardInput: inputField})
                                sendEvent( Zone1ViewEvent.TextInputFocusActivated )
                            }
                        }

                        onFocusChanged: {
                            //console.log( 'onFocusChanged focus: ' + focus )
                        }

                        Text {
                            anchors.fill: parent
                            anchors.leftMargin: 16
                            verticalAlignment: Text.AlignVCenter
                            text: '명칭/주소/전화번호'
                            color: 'lightgray'
                            visible: inputField.text.length === 0
                        }


                        // 17.11.10 hskim force to send Zone1Constants::FocusZone1TextInput notification
                        // so that search tab view is created even when text input has active focus and it is clicked
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
                    }//inputField

                    Rectangle {
                        id: inputFieldBorder

                        anchors.fill: inputField
                        color: 'transparent'
                        border { color: 'red'; width: 2 }
                        visible: false
                    }
                }//Image
            }//compTextInput
            Item {
                id: compTitle

                property alias title: label.text

                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: parent.bottom
                anchors.bottomMargin: layout.compTitleLabelBottomMargin
                width: childrenRect.width
                height: childrenRect.height

                Text {
                    id: label
                    width: layout.compTitleLabelWidth; height: layout.compTitleLabelHeight
                    verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter
                    font { family: GlobalConstants.kFontFamily; pointSize: layout.compTitleLabelFontPixelSize }//pixelSize
                    color: "#ffffff"
                }
            }//compTilte

            Csw {
                id: csw
                section: Csw.Section_IntegratedSearch
                sequence: 0
                enable: inputField.visible
                onFocused: inputFieldBorder.visible = true
                onLostFocusd: inputFieldBorder.visible = false
                deletable: false
                onSelected: {
                    sendEvent(Zone1ViewEvent.RegistKeyboardInput, {keyboardInput: inputField})
                    sendEvent(Zone1ViewEvent.TextInputFocusActivated)
                    csw.forceLostFocus()
                }
            }
        }
        Rectangle {
            id: onlineStatus
            visible: false
            width: 16
            height: 16
            radius: width/2
            color: online ? "green" : "red"
            anchors { left: parent.left; top: parent.top; leftMargin: 4; topMargin: 4 }
        }
    }
}
