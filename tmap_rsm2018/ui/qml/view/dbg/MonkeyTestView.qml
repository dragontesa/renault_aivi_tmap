import QtQuick 2.1
import CommonConstants 1.0
import SettingConstants 1.0
import ViewRegistry 1.0
import "../../component"
import "../../view"
import "../../GlobalConstants.js" as GlobalConstants
import "../../component/internal/Keyboard.js" as CustomKeyboard
import "../../view/util/CommonUtils.js" as CommonUtils
import MonkeyTestViewEvent 1.0

PopupView {
    id: viewRoot
    anchors.centerIn: parent ? parent : undefined
    width: parent ? parent.width : undefined
    height: parent ? parent.height : undefined
    property int keyboardHeight: 419
    property int repeatsPerSecond
    property int durationMins
    modal: true

    function __processKeyboard(key, text) {
        switch(key) {
        case CustomKeyboard.CodeCancel:
           showKeyboard(false)
            break
        case CustomKeyboard.CodeSymbol:     break
        case CustomKeyboard.CodeKorean:     break
        case CustomKeyboard.CodeEnglish:    break
        case CustomKeyboard.CodeShift:      break
        case CustomKeyboard.CodeClear:      break
        case CustomKeyboard.CodeSetting:    break
        case CustomKeyboard.CodeSearch:     break
        case CustomKeyboard.CodeConfirm:
            showKeyboard(false)
            break
        case CustomKeyboard.CodeSpace:
        case CustomKeyboard.CodeDelete:
        default:
            break
        }
    }

    function showKeyboard(visible, text) {
        var data = ({})
        if (visible) {
            keyboard.height = keyboardHeight
            data[CommonConstants.Owner] = CommonConstants.objectName(ViewRegistry.MonkeyTestView)
            data[CommonConstants.X] = keyboard.x
            data[CommonConstants.Y] = keyboard.y
            data[CommonConstants.Z] = keyboard.z + 1
            data[CommonConstants.Width] = keyboard.width
            data[CommonConstants.Height] = keyboard.height
            data[CommonConstants.SearchBtnAlwaysEnable] = true
            data[CommonConstants.Pressed] = CustomKeyboard.symbolKeybd
            sendEvent(MonkeyTestViewEvent.ShowKeyboard, data)
        } else {
            keyboard.height = 0
            if (!CommonUtils.isNull(text))
                data[CommonConstants.Text] = text
            sendEvent(MonkeyTestViewEvent.HideKeyboard, data)
        }
    }

    function __doSoftKeyFunction(button, menuName, menuIndex) {
        if (CommonUtils.isEqual(button , softKeyMenu[softKeyEnum.Back])) {
            close()
        }
    }

    property bool keyboardDown: false
    function __pullDownKeyboard() {
        keyboardDown = true
        showKeyboard(false)
    }

    function __pullUpKeyboard() {
        keyboardDown = false
        showKeyboard(true)
    }


    onMediated: {
        viewRoot.sendEvent(MonkeyTestViewEvent.Mediated)
    }

    Item {
        y: 10
        width: parent.width
        Text {
            width: 80
            anchors.horizontalCenter: parent.horizontalCenter
            font.bold: false
            font.pixelSize: Resource.pixelToDp(35)
            color: 'white'
            text: qsTr('Monkey Test')
        }
    }

    Item {
        width: 240
        height: 120
        anchors.centerIn: parent
        Column {
            spacing: 3
            Row {
                spacing: 3
                Text {
                    width: 160
                    horizontalAlignment: Text.AlignLeft
                    font.bold: false
                    font.pixelSize: Resource.pixelToDp(35)
                    color: 'white'
                    text: qsTr('RepeatsPerSecond:')
                }
                Rectangle {
                    width: repeatsPerSecondIputField.width
                    height: repeatsPerSecondIputField.height
                    color: 'white'
                    TextInput {
                        id: repeatsPerSecondIputField
                        focus: true
                        clip: true
                        color: 'black'
                        width: 120
                        font.pixelSize: Resource.pixelToDp(35)
                        text: CommonUtils.string(repeatsPerSecond)
                        maximumLength: 80
                        //cursorVisible: true
                        onActiveFocusChanged: {
                            if ( activeFocus ) {
                                    viewRoot.sendEvent(MonkeyTestViewEvent.HideKeyboard)
                                    viewRoot.sendEvent(MonkeyTestViewEvent.RegistKeyboardInput, {"keyboardInput": repeatsPerSecondIputField})
                                    viewRoot.showKeyboard(true)
                            }
                        }
                        onTextChanged: {
                            repeatsPerSecond = repeatsPerSecondIputField.text
                        }
                    }
                }
            }
            Row {
                spacing: 3
                Text {
                    width: 160
                    horizontalAlignment: Text.AlignLeft
                    font.bold: false
                    font.pixelSize: Resource.pixelToDp(35)
                    color: 'white'
                    text: qsTr('Duration Minutes:')
                }
                Rectangle {
                    width: repeatsPerSecondIputField.width
                    height: repeatsPerSecondIputField.height
                    color: 'white'
                    TextInput {
                        id: durationMinsIputField
                        focus: true
                        clip: true
                        color: 'black'
                        width: 120
                        font.pixelSize: Resource.pixelToDp(35)
                        text: CommonUtils.string(durationMins)
                        maximumLength: 80
                        //cursorVisible: true
                        onActiveFocusChanged: {
                            if ( activeFocus ) {
                                    viewRoot.sendEvent(MonkeyTestViewEvent.HideKeyboard)
                                    viewRoot.sendEvent(MonkeyTestViewEvent.RegistKeyboardInput, {"keyboardInput": durationMinsIputField})
                                    viewRoot.showKeyboard(true)
                            }
                        }
                        onTextChanged: {
                            durationMins = durationMinsIputField.text
                        }
                    }
                }
            }
        }
    }

    Row {
        spacing: 3
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        TPushButton {
            label: "Run"
            width: 120
            height: 30
            onButtonClicked: {
                viewRoot.sendEvent(MonkeyTestViewEvent.RandClickTestClicked,{repeatsPerSecond:repeatsPerSecond,durationMins:durationMins})
            }
        }
        TPushButton {
            label: "Close"
            width: 120
            height: 30
            onButtonClicked: {
                close()
            }
        }
    }

    Item {
        id: keyboard
        anchors.bottom: parent.bottom
        width: parent.width
        height: keyboardHeight
        visible: false
        Component.onCompleted: {
        }
        onVisibleChanged: {
        }
    }
}

