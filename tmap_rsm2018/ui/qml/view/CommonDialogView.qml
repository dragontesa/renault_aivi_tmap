import QtQuick 2.0
import QtQuick.Layouts 1.0
import ViewRegistry 1.0
import CommonConstants 1.0
import ZoneConstants 1.0
import SearchConstants 1.0
import CommonDialogViewEvent 1.0
import "../component"
import "../singleton"
import "../GlobalConstants.js" as GlobalConstants
import '../view/util'
import Csw 1.0

PopupView {
    id: root

    modal: true

    anchors { fill: parent; bottomMargin: GlobalConstants.kScreenHardKeyHeight }

    property var parentObj: null
    property var view
    property alias contentsArea: dialogLoader
    property string cancelCallback
    property string okCallback: "close"

    property string title
    property string message
    property string message2
    property int messageType: -1
    property var menus: [qsTr('확인') + Tr.d]
    property bool isMapUpdate: false
    property bool isChangeDestination: false
    property bool isSystemPopup: false
    property int popupType: -1
    property int popupWidth: 520
    property int popupHeight: (isMapUpdate || isChangeDestination) ? 350 : 286
    property bool showTimer: false
    property string type: 'a'
    property bool embeded: false
    property string caller
    property string okText: qsTr('확인') + Tr.d
    property string cancelText: qsTr('취소') + Tr.d

    property int currentIndex: 0
    property bool cswEnable: false
    property alias captureEnable: gestureArea.enabled
    property bool enableTimer: true
    property int messageLineCount: 0

    property alias csw: csw

    onTypeChanged: {
        if (type === 'c') {
            isMapUpdate = true
            var lineCount = getLineCount(message)
            if (3 < lineCount) {
                messageLineCount = lineCount
            }
        }
    }

    //chunjae.lee - 2018.06.01
    property var arg: null

    signal accepted(var arg)
    signal rejected()
    signal menuClicked(string menu)
    signal captureScreen()

    property int event: -1

    onMediated: {
        if (!embeded) {
            sendEvent(CommonDialogViewEvent.Mediated)
            sendEvent(CommonDialogViewEvent.RegistCSW, [csw])
        }
    }
    onActivated: {
    }
    onDeactivated: {
    }

    Timer {
        id: closeTimer
        interval: 100
        onTriggered: {
            switch (event) {
            case CommonDialogViewEvent.Accepted:
                if (embeded) {
                    root.accepted(arg)
                } else {
                    sendEvent(event, arg)
                }
                break
            case CommonDialogViewEvent.Canceled:
                if (embeded) {
                    root.rejected()
                } else {
                    sendEvent(event, arg)
                }
                break
            case CommonDialogViewEvent.MenuClicked:
                if (embeded) {
                    var menu = arg['menu']
                    root.menuClicked(menu)
                } else {
                    sendEvent(event, arg)
                }
                break
            }

            close()
        }
    }

    function getLineCount(str) {
        var count = str.split('\n').length - 1
        count += str.split('<br>').length - 1
        return count + 1
    }

    function showDialog() {
        dialogLoader.sourceComponent = dialogComponent
    }
    function closeDialog() {
        dialogLoader.sourceComponent = undefined
    }
    function closeThisView() {
        root.visible = false
        closeTimer.start()
    }
    function processMenu(menu) {
        if (isOk(menu)) {
            enableTimer = false

            event = CommonDialogViewEvent.Accepted
        } else if (isCancel(menu)) {
            event = CommonDialogViewEvent.Canceled
        } else {
            event = CommonDialogViewEvent.MenuClicked
            arg['menu'] = menu
        }
    }
    function goBack() {
        event = CommonDialogViewEvent.Canceled
        closeThisView()
    }
    function processDefaultMenu() {
        var menu = menus[0]
        processMenu(menu)
        closeThisView()
    }

    Csw {
        id: csw
        section: Csw.Section_Dialog
        sequence: 0
        onFocused: {
            cswEnable = true
            currentIndex = 0
            showTimer = false
        }
        onLostFocusd: {
            cswEnable = false
        }

        onSpinCcwPressed: {
            if (0 < currentIndex)
                currentIndex -= 1
            else
                currentIndex = menus.length - 1
        }
        onSpinCwPressed: {
            if (currentIndex < menus.length - 1)
                currentIndex += 1
            else
                currentIndex = 0
        }
        onSelected: {
            processMenu(menus[currentIndex])
            closeThisView()
        }
    }
    GestureArea {
        id: gestureArea
        anchors.fill: parent
        enabled: false
        onIssued: {
            if (embeded) {
                root.captureScreen()
            } else {
                sendEvent(CommonDialogViewEvent.CaptureScreen)
            }
        }
    }

    function isOk(text) {
        return (text === qsTr('확인') || text === 'OK')
    }
    function isCancel(text) {
        return (text === qsTr('취소') || text === 'Cancel')
    }

    Loader {
        id: dialogLoader

        width: popupWidth; height: popupHeight
        anchors.centerIn: parent
    }

    Component {
        id: ringCountDown

        TRingCountDown {
            time: 5

            onTimeout: {
                closeThisView()
                if (enableTimer) {
                    event = CommonDialogViewEvent.Accepted
                }
            }
            Component.onCompleted: {
                start()
            }
        } // TRingCountDown
    }

    Component {
        id: dialogComponent

        Rectangle {
            anchors.fill: parent
            color: '#222226'; border.color: '#a1a2b3'
            Text2 {
                id: titleText
                anchors {
                    top: parent.top; topMargin: 30;
                    left: parent.left; leftMargin: 30;
                    right: parent.right; rightMargin: 30 }
                height: 30
                offsetY: 3
                font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(32) }
                text: title
                color: '#ffffff'
                verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter
            }
            Item {
                id: contents
                anchors { left: titleText.left; right: titleText.right; top: titleText.bottom; topMargin: 30 }
                height: isMapUpdate ? 150 : 90
                visible: messageLineCount < 4

                Item {
                    width: parent.width; height: parent.height
                    anchors.verticalCenter: parent.verticalCenter
                    Column {
                        width: parent.width
                        anchors.centerIn: parent

                        Text2 {
                            id: contentsText
                            width: parent.width;  height: contentHeight
                            offsetY: 4
                            text: message
                            color: '#ffffff'
                            maximumLineCount: 3
                            lineHeight: 26+6; lineHeightMode: Text.FixedHeight
                            wrapMode: Text.WordWrap
                            font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(27) }
                            horizontalAlignment: Text.AlignHCenter
                        }
                        Item { width: parent.width; height: message2.length > 0 ? 16 : 0 }
                        Text2 {
                            visible: message2.length > 0
                            width: parent.width; height: 30
                            offsetY: 1
                            font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(27) }
                            text: message2
                            color: '#ffffff'
                            verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter
                            elide: Text.ElideRight
                        }
                    }
                }
            }

            Item {
                id: contents4
                visible: 4 <= messageLineCount
                anchors { left: titleText.left; right: titleText.right; top: titleText.bottom; topMargin: 30 }
                height: 150
                Item {
                    width: parent.width; height: contents4Text.height
                    anchors.centerIn: parent
                    Text2 {
                        id: contents4Text
                        width: parent.width;  height: contentHeight
                        offsetY: 4
                        text: message
                        color: '#ffffff'
                        maximumLineCount: 4
                        lineHeight: 26+6; lineHeightMode: Text.FixedHeight
                        wrapMode: Text.WordWrap
                        font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(27) }
                        horizontalAlignment: Text.AlignHCenter
                    }
                }
            }

            ListView {
                id: buttons
                width: parent.width; height: 60;
                anchors { bottom: parent.bottom; bottomMargin: 1 }
                model: menus
                orientation: ListView.Horizontal
                interactive: false

                delegate: Item {
                    id: menuItem

                    width : dialogLoader.width / menus.length; height: 60
                    state: 'normal'
                    states: [
                        State {
                            name: 'normal'
                            PropertyChanges {
                                target: bg
                                color: '#ffffff'
                                opacity: (cswEnable && currentIndex == index) ? 1.0 : 0.05
                            }
                            PropertyChanges {
                                target: buttonText
                                scale: 1.0
                            }
                        },
                        State {
                            name: 'pressed'
                            PropertyChanges {
                                target: bg
                                color: MultiSenseCoordinator.kAmbientColor
                                opacity: 0.3
                            }
                            PropertyChanges {
                                target: buttonText
                                scale: 0.85
                            }
                        }
                    ]
                    Rectangle {
                        id: bg
                        anchors.fill: parent
                    }

                    Text2 {
                        id: buttonText
                        anchors{ centerIn: parent }
                        width: contentWidth; height: contentHeight;
                        font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(32) }
                        color: (cswEnable && currentIndex === index) ? '#000000' : '#ffffff'
                        text: modelData
                        offsetY: 3
                    }
                    Loader {
                        width: 32; height: 32
                        anchors { left: buttonText.right; leftMargin: 8; verticalCenter: parent.verticalCenter }
                        sourceComponent: (showTimer && modelData === qsTr('확인')) ? ringCountDown : null
                    }

                    Rectangle { width: 1; height: parent.height; anchors.right: parent.right;
                        color: '#a1a2b3'; visible: index != menus.length-1 }
                    MouseArea {
                        anchors.fill: parent
                        onPressed: menuItem.state = 'pressed'
                        onReleased: menuItem.state = 'normal'
                        onClicked: {
                            processMenu(modelData)
                            closeThisView()
                        }
                    }
                }// delegate
                Rectangle { width: parent.width; height: 1; anchors.top: parent.top; color: '#a1a2b3' }
            }
        }
    }//Component
}
