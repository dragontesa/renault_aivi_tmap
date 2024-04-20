import QtQuick 2.0

Item  {
    id: root
    width  : 69
    height : 60
    property alias scaleText: scaleText.text
    property alias unitText : unitText.text
    readonly property int baseMargin : 2
    property string  lineColor: "white"
    property int lineWidth  : 1
    property int lineHeight : 6


    // rectangle box
    Rectangle {
        id: rootBody
        width: parent.width
        height: parent.height
        color: "#434346"
        border.width: 0

        // text boxunit
        Rectangle
        {
            id : textBox
            width : parent.width - 8
            height : 30
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.leftMargin : baseMargin
            anchors.rightMargin : baseMargin
            border.width : 0
            color : rootBody.color

            Text {
                id: scaleText
                anchors.right : unitText.left
                anchors.rightMargin : baseMargin
                font.pixelSize : Resource.pixelToDp(22)
                font.bold : true
                font.family: "Read Condensed"// "RC_Regular"
                color: 'white'
                text: "51"
            }
            Text {
                id : unitText
                anchors.right : parent.right
                anchors.rightMargin : 4
                anchors.bottom : scaleText.bottom
                anchors.bottomMargin : 0
                font.pixelSize : Resource.pixelToDp(18)
                font.family: "Read Condensed" //"RC_Regular"
                color : 'white'
                text : "km"
            }
        } // text box

        // unit line
        Rectangle {
            id : unitLine
            width : textBox.width
            height : lineHeight // side line height
            border.width : 0
            anchors.top : textBox.bottom
            anchors.topMargin : -6
            anchors.horizontalCenter : parent.horizontalCenter
            color : rootBody.color

            Component.onCompleted: console.log("MapScale.qml.unitLine.width = " + width)

//            Row {
//                spacing : 0

                Rectangle {
                    id : lineLeft
                    width : lineWidth
                    height : lineHeight
                    anchors.bottom : parent.bottom
                    anchors.bottomMargin : 0
                    border.width: 0
                    color : lineColor
                }

                Rectangle  {
                    id : lineCenter
                    width: unitLine.width - lineWidth - lineWidth
                    height : lineWidth
                    anchors.left : lineLeft.right
                    anchors.leftMargin : 0
                    anchors.bottom : parent.bottom
                    anchors.bottomMargin : 0
                    border.width : 0
                    color : lineColor
                }

                Rectangle {
                    id : lineRight
                    width : lineWidth
                    height : lineHeight
                    anchors.left : lineCenter.right
                    anchors.leftMargin : 0
                    anchors.bottom : parent.bottom
                    anchors.bottomMargin : 0
                    border.width : 0
                    color : lineColor
                }
//            }
        } // end unit line

    } // rect box
}
