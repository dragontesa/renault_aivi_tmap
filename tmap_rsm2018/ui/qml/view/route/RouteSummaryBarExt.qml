import QtQuick 2.0

Item {
    property alias distanceText: distance.text
    property alias remainTimeText: remainTime.text
    property alias feeText: fee.text

    Rectangle {
        anchors.fill: parent
        radius: 4
        color: Qt.lighter("gray", 1.05)
        opacity: 0.75
    }

    RouteSummaryText {
        id: distance

        anchors.left: parent.left
        anchors.leftMargin: 4
        width: 256
        height: parent.height
    }
    RouteSummaryText {
        id: remainTime

        anchors.left: distance.right
        width: 64
        height: parent.height
    }
    RouteSummaryText {
        id: fee

        anchors.left: remainTime.right
        height: parent.height
    }
}
