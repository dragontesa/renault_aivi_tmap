import QtQuick 2.0
import MapUpdateDialogEvent 1.0
import LayoutConstants 1.0
import "../../view"
import "../../component"

PopupView {
    id: root

    property string errorMessage
    property int errorCode

    function onOk() {
    }
    function onCancel() {
        sendEvent(MapUpdateDialogEvent.CancelPressed)
    }

    onMediated: {
        sendEvent(MapUpdateDialogEvent.Mediated)
    }

    onActivated: {
        sendEvent(MapUpdateDialogEvent.Activated)
    }

    modal: true

//    layout: ErrorViewLayout {
//        onUpdateLayoutCompleted: {
//            switch (screenResolution) {
//            case LayoutConstants.L800x480:
//                item.anchors.leftMargin = 128
//                item.anchors.rightMargin = 128
//                item.anchors.topMargin = 24
//                item.anchors.bottomMargin = 24
//                break

//            case LayoutConstants.L820x1024:
//                item.anchors.leftMargin = 138
//                item.anchors.rightMargin = 138
//                item.anchors.topMargin = 215
//                item.anchors.bottomMargin = 215
//                break
//            }
//        }
//    }

    Item {
        id: item

        anchors.fill: parent

        // BG
        Rectangle {
            id: bg
            width: parent.width / 2
            height: parent.height / 2
            anchors.centerIn: parent
            color: "gray"

            Column {
                id: column

                anchors.fill: bg

                Rectangle {
                    id: header
                    width: parent.width; height: 48
                    color: "lightgray"
                    Text {
                        anchors.centerIn: parent
                        text: qsTr("지도 업데이트")
                    }
                }
                Item {
                    width: column.width; height: column.height - (header.height + footer.height)

                    Rectangle {
                        anchors.fill: parent
                        anchors.margins: 1
                        color: "lightsteelblue"
                    }
                    Text {
                        anchors.centerIn: parent
                        horizontalAlignment: Text.AlignHCenter
                        text: errorMessage
                        color: "black"
                    }
                }

                Rectangle {
                    id: footer
                    width: parent.width; height: 48
                    color: "black"
                    Text {
                        anchors.centerIn: parent
                        text: qsTr("확인")
                        color: "white"
                    }
                    MouseArea {
                        anchors.fill: parent

                        onClicked: {
                            console.log("sendEvent(MapUpdateDialogEvent.OkPressed)")
                            sendEvent(MapUpdateDialogEvent.OkPressed)

                            errorMessage = "OK를 눌렀음."

                            //console.log("MapUpdateDialog.close()")
                            //close()
                        }
                    }
                }
            }
        }

        //Component.onCompleted: countDown.start()
    }
}
