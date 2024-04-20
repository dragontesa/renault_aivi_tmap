import QtQuick 2.0

Item {
    id: root
    property var layout

    signal clicked
    signal timeClicked
    signal areaClicked
    signal distanceClicked

    property alias remainDistance: remainDistance.distance
    property alias navi: naviAddress
    property int remainTime: -1
    property int arrivalTime: -1

    onRemainTimeChanged: {
        var t = remainTime
        var h = Math.floor((t/3600)).toFixed(0)
        var m = Math.floor(((t%3600)/60)).toFixed(0)
        remainTimeComp.setRemainTime(h, m)
        remainTimeComp.visible = true
    }
    onArrivalTimeChanged: {
        var t = arrivalTime
        var h = Math.floor((t/3600)).toFixed(0)
        var m = Math.floor(((t%3600)/60)).toFixed(0)
        arrivalTimeComp.setArrivalTime(h, m)
        arrivalTimeComp.visible = true
    }

    function setRemainItemVisible(b) {
        remainDistance.visible = b
        remainTimeComp.visible = b
        arrivalTimeComp.visible = false
        naviAddress.visible = b
    }
    function setArrivalItemVisible(b) {
        remainDistance.visible = b
        remainTimeComp.visible = false
        arrivalTimeComp.visible = b
        naviAddress.visible = b
    }
    function setRouteGuideByCurrentPosition(address) {
        naviAddress.setYah(address)
    }
    function setRouteGuideByCurrentRoad(road) {
        naviAddress.setRoad(road)
    }
    function setRouteGuideByPoi(poi) {
        naviAddress.setPoi(poi)
    }

    Row {
        id: row
        width: layout.naviRouteGuideRowWidth
        height: parent.height

        Item { width: layout.naviRouteGuideRowItem1Width; height: parent.height }
        Item { width: layout.naviRouteGuideRowItem2Width; height: parent.height }
        Item { width: layout.naviRouteGuideRowItem3Width; height: parent.height }
    }
    SoftkeyRemainDistance {
        id: remainDistance
        x: layout.naviRouteGuideRemainDistanceX
        width: layout.naviRouteGuideRowItem2Width; height: 25

        anchors.verticalCenter: parent.verticalCenter
    }
    SoftkeyRemainTime {
        id: remainTimeComp
        anchors.left: remainDistance.left
        anchors.leftMargin: layout.naviRouteGuideRemainTimeLeftMargin
        anchors.verticalCenter: parent.verticalCenter
        visible: true
    }
    SoftkeyArrivalTime {
        id: arrivalTimeComp
        anchors.left: remainDistance.left
        anchors.leftMargin: layout.naviRouteGuideArrivalTimeLeftMargin
        anchors.verticalCenter: parent.verticalCenter
        format: timeFormat

        visible: remainTimeComp.visible == false
    }
    Zone3NaviAddress {
        id: naviAddress
        layout: root.layout

        height: parent.height
        anchors.left: row.right
        anchors.right: parent.right
        isGuide: true
    }
    MouseArea {
        anchors.fill: parent
        onClicked: {
            if (mouse.x <= (remainDistance.width + layout.naviRouteGuideRemainDistanceX*2)) {
                root.distanceClicked()
            } else if (mouse.x < naviAddress.x) {
                root.timeClicked()
            } else {
                root.areaClicked()
            }
        }
    }
}
