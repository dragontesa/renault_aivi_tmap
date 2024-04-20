import QtQuick 2.0
import "../GlobalConstants.js" as GlobalConstants


Item {
    id: pickerBtn
    width: 130
    height: 20
    property alias label: btnLabel.text
    property alias unitLabel: unitText.text
    property real plusInterval: 100
    property real minusInterval: 100
    property real value: 0
    property real step: 1.0
    property color bgColor: "#212121"
    property color fgColor: "white"
    property int  precision: 1

    signal valueChangedByUi(real val)

    onFgColorChanged: {
       btnLabel.color = fgColor
       upBtnName.color = fgColor
       downBtnName.color = fgColor
    }
    onBgColorChanged: {
        addBtn.color = bgColor
        subtractBtn.color = bgColor
    }

    onValueChanged : {
        valueText.text = value.toFixed(precision)
    }

    Timer {
        id: plusTouchTimer
        interval: plusInterval
        running: false
        repeat: false
        property bool autoStart: true
        onTriggered: {
            if (autoStart) {
                value += step
                valueChangedByUi(value.toFixed(precision))
                start()
            }
        }
    }

    Timer {
        id: minusTouchTimer
        interval: minusInterval
        running: false
        repeat: false
        property bool autoStart: true
        onTriggered: {
            if (autoStart) {
                value -= step
                valueChangedByUi(value.toFixed(precision))
                start()
            }
        }
    }


    Row {
        id:row1
        width: parent.width
        height: parent.height
        spacing: 2
        Column {
            spacing:1
            width: (parent.width - 2) * 0.5
            height: parent.height
            Rectangle {
                id: addBtn
                width: parent.width
                height:parent.height * 0.46
                color: "#212121"
                Text {
                    id: upBtnName
                    anchors.centerIn: parent
                    font { family: GlobalConstants.kFontFamily; pixelSize: GlobalConstants.kListSubTextFontSize }
                    color: GlobalConstants.kListSubTextColor
                    text: "+"
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        value += step
                        valueChangedByUi(value.toFixed(precision))
                     }
                    onPressAndHold: {
                        plusTouchTimer.autoStart = true
                        plusTouchTimer.start()
                    }
                    onPressedChanged: {
                        if (!pressed) {
                            plusTouchTimer.autoStart = false
                            plusTouchTimer.stop()
                        }
                    }
                }
            }

            Rectangle {
                id: subtractBtn
                width: parent.width
                height:parent.height * 0.46
                color: "#212121"
                Text {
                    id: downBtnName
                    anchors.centerIn: parent
                    font { family: GlobalConstants.kFontFamily; pixelSize: GlobalConstants.kListSubTextFontSize }
                    color: GlobalConstants.kListSubTextColor
                    text: "-"
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        value -= step 
                        valueChangedByUi(value.toFixed(precision))
                    }

                    onPressAndHold: {
                        minusTouchTimer.autoStart = true
                        minusTouchTimer.start()
                    }

                    onPressedChanged: {
                        if (!pressed) {
                            minusTouchTimer.autoStart = false
                            minusTouchTimer.stop()
                        }
                    }
                }
            }
        }

        Item {
            width: pickerBtn.width * 0.5 - 2
            height: pickerBtn.height
            Column {
                spacing: 1
                width: (parent.width - 2) * 0.5
                height: parent.height
                Text {
                    id:btnLabel
                    width: parent.width
                    height: parent.height * 0.3
                    font { family: GlobalConstants.kFontFamily; pixelSize: 14; weight:Font.Bold }
                    text:""
                    color: "white"
                    horizontalAlignment:Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                Text {
                    id: valueText
                    width: parent.width
                    height: parent.height * 0.3
                    font { family: GlobalConstants.kFontFamily; pixelSize: 16 }
                    color: "#dddddd"
                    text: value.toFixed(precision)
                    horizontalAlignment:Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
                Text {
                    id: unitText
                    width: parent.width
                    height: parent.height * 0.3
                    font { family: GlobalConstants.kFontFamily; pixelSize: 11; }
                    color: "#dddddd"
                    text:  ""
                    horizontalAlignment:Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }
    }
}

