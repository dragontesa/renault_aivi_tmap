import QtQuick 2.0
import "../../GlobalConstants.js" as GlobalConstants
import '../../view/util'
Item {
    property string address

    width: 396; height: 390
    property int barHeight: 58
    property bool forTest: false

    Item {
        anchors.fill: parent

        Image {
            y: 45
            anchors.horizontalCenter: parent.horizontalCenter
            source: Resource.imageUrl("map_ic_yah.png")
        }
        Text2 {
            y:83
            height: 63
            anchors {
                left: parent.left; leftMargin: 40
                right: parent.right; rightMargin: 40
            }
            font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(29) }
            color: '#ffffff'
            text: address
            maximumLineCount: 2
            lineHeight: 36
            lineHeightMode: Text.FixedHeight
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap
            offsetY: 3
        }

        Rectangle {
            id: bg
            width: parent.width; height: barHeight
            anchors.bottom: parent.bottom
            color: GlobalConstants.kGadgetBrightBgColor
        }
        Image {
            id: searchIcon
            anchors.horizontalCenter: bg.horizontalCenter
            anchors.verticalCenter: bg.verticalCenter
            source: Resource.imageUrl("zone/zone3_btn_search.png")
        }
    }
}
