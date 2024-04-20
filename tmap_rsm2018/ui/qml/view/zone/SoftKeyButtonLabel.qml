import QtQuick 2.0
import "../../GlobalConstants.js" as GlobalConstants

Text {
    anchors.fill: parent
    horizontalAlignment: Text.AlignHCenter
    verticalAlignment: Text.AlignVCenter

    font.family: GlobalConstants.kFontFamily
    font.pointSize: Resource.pixelToDp(30)
    color: "white"
}
