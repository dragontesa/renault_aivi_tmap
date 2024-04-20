import QtQuick 2.0
import ErrorViewEvent 1.0
import LayoutConstants 1.0
import "../../view"
import "../../component"

PopupView {
    id: root

    property var caller
    onCallerChanged: {
        switch (caller) {
        case "SettingRouteAlternateView":
            errorMessage = "‘최적길’을 제외한 \n다른 경로 2개를 선택해 주세요."
            showErrorCode = false
            break

        case "RouteSettingView":
            showErrorCode = false;
            break;

        default:
            break
        }
    }

    property bool showCancelButton: false
    property bool showCountdown: true

    property bool showErrorCode: true
    property string errorMessage: "탐색된 경로가 없습니다.\n오류 코드 : "
    property int errorCode: 0
    onErrorCodeChanged: {
        switch (errorCode) {
        case -297: errorMessage = "선택한 위치와 동일한 지점이\n등록되어 있습니다.\n다시 확인해 주세요."
            showErrorCode = false
            break
        case -295: errorMessage = "경로탐색 최대거리를 초과하였습니다.\n경로를 재설정 해주세요."
            showErrorCode = false
            break
        }
    }

    function onOk() {
        sendEvent(ErrorViewEvent.OkPressed)
//        switch (errorCode) {
//        case -295:
//        case -297:
//            sendEvent(ErrorViewEvent.RestoreRoute)
//            break
//        }
    }
    function onCancel() {
        sendEvent(ErrorViewEvent.CancelPressed)
    }

    onActivated: sendEvent(ErrorViewEvent.Activated)
    modal: true

    layout: ErrorViewLayout {
        onUpdateLayoutCompleted: {
            switch (screenResolution) {
            case LayoutConstants.L800x480:
                item.anchors.leftMargin = 128
                item.anchors.rightMargin = 128
                item.anchors.topMargin = 24
                item.anchors.bottomMargin = 24
                break

            case LayoutConstants.L820x1024:
                item.anchors.leftMargin = 138
                item.anchors.rightMargin = 138
                item.anchors.topMargin = 215
                item.anchors.bottomMargin = 215
                break
            }
        }
    }

    Item {
        id: item

        anchors.fill: parent

        // BG
        Rectangle {
            id: bg
            anchors.fill: parent
            color: "gray"

            MouseArea { anchors.fill: parent }

            Column {
                id: column

                anchors.fill: bg

                Rectangle {
                    id: header
                    width: parent.width; height: 48
                    color: "lightgray"
                    Text {
                        anchors.centerIn: parent
                        text: "알림"
                    }
                }
                Item {
                    width: column.width; height: column.height - (header.height + okButton.height)

                    Rectangle {
                        anchors.fill: parent
                        anchors.margins: 1
                        color: "white"
                    }
                    Text {
                        anchors.centerIn: parent
                        horizontalAlignment: Text.AlignHCenter
                        text: errorMessage + (showErrorCode ? errorCode : "")
                    }
                }
                Row {
                    width: column.width
                    height: 48
                    Rectangle {
                        id: cancelButton
                        visible: root.showCancelButton
                        width: root.showCancelButton ? column.width/2 : 0
                        height: 48
                        color: "black"
                        Text {
                            anchors.centerIn: parent
                            text: "취소"
                            color: "white"
                        }
                        MouseArea {
                            anchors.fill: parent
                            onClicked: onCancel()
                        }
                        Rectangle { width: 1; height: parent.height; color: "lightgray"; anchors.right: parent.right }
                    }

                    Rectangle {
                        id: okButton
                        width: root.showCancelButton ? column.width/2 : column.width;
                        height: 48
                        color: "black"
                        Text {
                            anchors.centerIn: parent
                            text: "확인"
                            color: "white"
                        }
                        MouseArea {
                            anchors.fill: parent
                            onClicked: onOk()
                        }
                        TRingCountDown {
                            id: countDown

                            visible: root.showCountdown
                            width: 32
                            height: 32
                            x: parent.width / 2 + width
                            anchors { verticalCenter: parent.verticalCenter }
                            time: 5
                            onTimeout: onOk()
                        }
                    }
                }
            }
        }

        Component.onCompleted: {
            if (root.showCountdown)
                countDown.start()
        }
    }
}
