import QtQuick 2.0
import HiddenMenuLoginViewEvent 1.0
import ViewRegistry 1.0
import CommonConstants 1.0
import "../../view"
import "../../component"
import "../../GlobalConstants.js" as GlobalConstants
import "../../component/internal/Keyboard.js" as CustomKeyboard
import "../../view/util/CommonUtils.js" as CommonUtils

PopupView {
    id: viewRoot    
    width: parent?parent.width:800
    height: parent?parent.height:480

    property string diagType            : "navi" // navi or vsm or ui
    property bool isReadyLogin          : false
    readonly property int maxDiagMode   : 3 // navi, vsm, ui

    function openHiddenSettingView(pwd) {
        var data = ({})
        data["pwd"] = pwd
        if (CommonUtils.isEqual(diagType,  "navi"))
            data[CommonConstants.Url] = CommonConstants.url(ViewRegistry.HiddenDiagSettingView)
        else if (CommonUtils.isEqual(diagType,"vsm"))
            data[CommonConstants.Url] = CommonConstants.url(ViewRegistry.HiddenVsmDiagSettingView)
        else if (CommonUtils.isEqual(diagType, "bosch"))
            data[CommonConstants.Url] = CommonConstants.url(ViewRegistry.HiddenBoschSettingView)
        else if (CommonUtils.isEqual(diagType, "ui")) {
        }
        else {
            return
        }
        sendEvent(HiddenMenuLoginViewEvent.CreateView, data )
    }

    function __processKeyboard(key, text) {
        switch(key) {
            case CustomKeyboard.CodeDelete:
            case CustomKeyboard.CodeSymbol:
            case CustomKeyboard.CodeKorean:
            case CustomKeyboard.CodeEnglish:
            case CustomKeyboard.CodeShift:
            case CustomKeyboard.CodeClear:
            case CustomKeyboard.CodeSetting:
            case CustomKeyboard.CodeSearch:
            case CustomKeyboard.CodeSpace:
                break;
            case CustomKeyboard.CodeCancel:
                sendEvent(HiddenMenuLoginViewEvent.ClearText)
                sendEvent(HiddenMenuLoginViewEvent.HideKeyboard)
                close()
               break
            case CustomKeyboard.CodeConfirm:
                openHiddenSettingView(hiddenLoginInputField.text)
                sendEvent(HiddenMenuLoginViewEvent.ClearText)
                sendEvent(HiddenMenuLoginViewEvent.HideKeyboard)
                close()
                break
            default:
                hiddenLoginInputField.text = text
                break
        }
    }

    property bool keyboardDown: false
    function __pullDownKeyboard() {
        if (!active)
            return
        keyboardDown = true

        sendEvent(HiddenMenuLoginViewEvent.HideKeyboard)

        loginBox.anchors.centerIn = viewRoot
        loginBox.height =  (viewRoot.height) * 0.6
    }

    function __pullUpKeyboard() {
        if (!active)
            return
        keyboardDown = false
        showKeyboard()
    }

    function showKeyboard() {
        sendEvent(HiddenMenuLoginViewEvent.ShowKeyboard, {
                      owner: objectName,
                      x: pwdKeypad.x,
                      y: pwdKeypad.y,
                      z: pwdKeypad.z+1,
                      width: pwdKeypad.width,
                      height: pwdKeypad.height,
                      searchBtnAlwaysEnable: true,
                      dontUseEditBox: true
                  })

        loginBox.anchors.centerIn = undefined
        loginBox.height =  (viewRoot.height - pwdKeypad.height) * 0.9
        loginBox.y = ((viewRoot.height - pwdKeypad.height) - loginBox.height)/2 + 40
    }

    onMediated: {
        console.log("hideen login diag :"+diagType)
    }
    onDiagTypeChanged: {

        console.log("hideen login diag changed:"+diagType)
    }


    Rectangle {
        anchors.fill: parent
        color: "#454545"
    }

    Rectangle {
        id: loginBox
        width:parent.width*0.8
        height: parent.height * 0.6
        anchors.centerIn: parent
        color: GlobalConstants.kListNormalColor

        MouseArea {
            id: hiddenShuffleBoard
            anchors.fill: parent
            propagateComposedEvents: true
            property int shuffleCnt: 0
            property bool isShuffling: false
            property var beginTime
            property int resetTime: 1000 // reset after 3 seconds
            onPressed: {
                if (!isShuffling) {
                    isShuffling = true
                    shuffleCnt = 0
                }
                if (isShuffling)
                    beginTime = CommonUtils.getTime() // enter login stanby state
            }

            onReleased: {
                if (isShuffling) {
                    if (CommonUtils.getTime() - beginTime <= resetTime)
                        shuffleCnt++
                    else if (shuffleCnt == 2) {
                        diagType = "bosch"
                        loginBox.color = "green"
                    }
                    else if (shuffleCnt > 3) { // enable login state if it has long pressed in more than 3
                        if (CommonUtils.isEqual(diagType , "navi")) {
                            diagType = "vsm"
                            loginBox.color = "blue"
                        }
                        // else if (CommonUtils.isEqual(diagType,  "vsm"))
                        //     diagType = "ui"
                        else {
                            diagType = "navi"
                            loginBox.color = GlobalConstants.kListNormalColor
                        }

                    isShuffling = false
                        console.log("diag mode changed to "+diagType)
                    }
                }
                mouse.accepted = false
            }/*
            onClicked: mouse.accepted = false
            onDoubleClicked: mouse.accepted = false
            onPositionChanged: mouse.accepted = false
            onPressAndHold: mouse.accepted = false*/
        }

        Text {
            id: pwdLabel
            width: (parent.width - 10) * 0.4
            height: 50
            anchors.left: parent.left
            anchors.leftMargin: 4
            anchors.verticalCenter: parent.verticalCenter
            text: "PASSWORD"
            font { family:GlobalConstants.kFontFamily; pointSize:GlobalConstants.kListSubTextFontSize}
            color: GlobalConstants.kListTextColor
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
        }

        Rectangle {
            anchors.fill: hiddenLoginInputField
            color: "#1f1f1f"
        }

        TextInput {
            id:hiddenLoginInputField
            focus: false
            width: (parent.width - 10) * 0.6
            height: 50
            anchors.right: parent.right
            anchors.rightMargin: 4
            anchors.verticalCenter: parent.verticalCenter
            color: "#bbbbbb"
            echoMode: TextInput.Password
            clip: true
            verticalAlignment: Text.AlignVCenter
            onActiveFocusChanged: {
                if (activeFocus) {
                    text = ""
                    sendEvent(HiddenMenuLoginViewEvent.RegistKeyboardInput, {"keyboardInput": hiddenLoginInputField})
                    showKeyboard()
                }
            }
        }

        Item {
            width: parent.width
            height: 50
            anchors.bottom: loginBox.bottom
            anchors.bottomMargin: 20
            TPushButton {
                width: (parent.width-2) / 2
                anchors.left:parent.left
                label: qsTr("취소")
                normalBackgroundColor: "#222222"
                pressedBackgroundColor: "blue"

                onButtonClicked: {
                    sendEvent(HiddenMenuLoginViewEvent.ClearText)
                    sendEvent(HiddenMenuLoginViewEvent.HideKeyboard)
                    close()
                }
            }
            TPushButton {
                id: btnOkay
                width: (parent.width-2) / 2
                anchors.right: parent.right
                label: qsTr("확인")
                normalBackgroundColor: "#222222"
                pressedBackgroundColor: "blue"
                onButtonClicked: {
                    // confirm password
                    openHiddenSettingView(hiddenLoginInputField.text)
                    sendEvent(HiddenMenuLoginViewEvent.ClearText)
                    sendEvent(HiddenMenuLoginViewEvent.HideKeyboard)
                    close()
                }
            }
        }
    }

    Item {
        id: pwdKeypad
        anchors.bottom: parent.bottom
        width: parent.width
        height: 419
        visible: false
    }
}
