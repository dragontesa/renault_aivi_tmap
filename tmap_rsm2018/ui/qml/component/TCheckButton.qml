import QtQuick 2.0

Item {
    id: root

    width: img.width + input.width + 10
    height: Math.max(img.height, input.height)

    //ID
    property alias input                : input
    property alias img                  : img

    //값
    property int    selected            : 0
    property string imgCheck            : ""
    property string imgUncheck          : ""

    signal clicked()

    opacity: (!enabled) ? 0.4 : 1.0 // effect dimming
    state: "normal"
    

    Row {
        id: idRow

        anchors.centerIn: parent
        spacing: 3
        scale: root.state === "pressed" ? 0.85 : 1.0
        Image {
            id : img
            anchors.verticalCenter: parent.verticalCenter
            source: (root.selected > 0) ?  imgCheck : imgUncheck
        }

        Text {
            id: input

            anchors.verticalCenter: parent.verticalCenter
            font.pointSize: 14//pixelSize: 14
            color: "#242424"
        }
    }   

    MouseArea {
        anchors.fill: idRow
        onPressedChanged: {
            if (pressed) {
                root.state = "pressed"
            } else {
                 root.state = "normal"
            }
        }
        onClicked: {
            root.selected = !root.selected
            root.clicked()
        }
    }
}
