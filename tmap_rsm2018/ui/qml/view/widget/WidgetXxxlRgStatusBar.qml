import QtQuick 2.0
import "../../component"
import "../../view/zone"
import "../../GlobalConstants.js" as GlobalConstants
import "../../view/util/CommonUtils.js" as CommonUtils
import "../../view/util/Formatter.js" as Formatter
import "../../singleton"

Item {
    id: root

    property bool isRg: false
    property string address
    property string remainDistance
    property int remainTime

    signal searchClicked

    width: 598
    height: 83

    onRemainTimeChanged: {
        var t = gpsTimeUpdated
            ? CommonUtils.getArrivalTimeUsingGpsTime(gpsTime, remainTime)
            : CommonUtils.getArrivalTimeUsingGpsTime(new Date(), remainTime)
        var h = CommonUtils.getHour(t)
        var m = CommonUtils.getMin(t)
        arrivalTime.setArrivalTime(h, m)
    }

    Component {
        id: textComp

        Text {
            height: 26
            verticalAlignment: Text.AlignVCenter
            anchors.verticalCenter: parent.verticalCenter
            elide: Text.ElideRight
            font.family: GlobalConstants.kFontFamily
            font.pixelSize: Resource.pixelToDp(29)//pixelSize: 29
            color: "#ffffff"
            opacity: 0.85
            text: address
        }
    }

    // 좌측 검색 아이콘
    Item {
        id: searchItem

        width: 83
        height: 83

        Rectangle {
            id: searchRect

            anchors.fill: parent
            opacity: 0.85

            state: "normal"
            states: [
                State {
                    name: "normal"
                    PropertyChanges {
                        target: searchRect
                        color: "#222226"
                        opacity: 0.85
                    }
                },
                State {
                    name: "pressed"
                    PropertyChanges {
                        target: searchRect
                        color: MultiSenseCoordinator.kAmbientColor
                        opacity: 0.3
                    }
                }
            ]

            Image {
                id: searchIcon

                anchors.centerIn: parent
                source: Resource.imageUrl("zone/zone3_btn_search.png")
                MouseArea {
                    anchors.fill: parent
                    onPressed: searchRect.state = "pressed"
                    onReleased: searchRect.state = "normal"
                    onClicked: root.searchClicked()
                }
            }
        }
    }

    // 세로선
    Rectangle {
        width: 1
        height: 36
        anchors.right: searchItem.right
        anchors.verticalCenter: searchItem.verticalCenter
    }

    // 목적지 있을 경우의 Rg 상태 정보

    Rectangle {
        anchors { left: searchItem.right; right: parent.right }
        height: parent.height
        color: "#222226"
        opacity: 0.85
    }

    Item {
        id: hasDestinationItem

        visible: isRg
        anchors { left: searchItem.right; leftMargin: 6; right: parent.right }
        height: parent.height

        Row {
            height: parent.height
            Item {
                width: 120; height: parent.height

                SoftkeyRemainDistance {
                    id: remainDistance
                    anchors.centerIn: parent
                    distance: root.remainDistance
                }
            }
            Item {
                width: 126; height: parent.height
                SoftkeyArrivalTime {
                    id: arrivalTime
                    anchors.centerIn: parent
                    format: timeFormat
                }
            }
            Item {
                width: 263; height: parent.height
                Image {
                    id: poiIcon
                    anchors.left: parent.left
                    anchors.leftMargin: 18
                    anchors.verticalCenter: parent.verticalCenter
                    source: Resource.imageUrl("map_ic_yah.png")
                }
                Loader {
                    id: textLoader1
                    width: 195; height: parent.height
                    anchors.left: poiIcon.right
                    anchors.leftMargin: 6
                    sourceComponent: textComp
                }
            }
        }
    }

    // 목적지가 없을 경우의 Rg 상태 정보
    Item {
        id: noneDestinationItem

        visible: !isRg
        anchors.left: searchItem.right
        anchors.right: parent.right
        height: parent.height

        Row {
            id: row
            width: childrenRect.width
            height: childrenRect.height
            spacing: 6
            anchors.centerIn: parent

            Image {
                source: Resource.imageUrl("map_ic_yah.png")
            }
            Loader {
                id: textLoader2
                width: 435; height: 26
                sourceComponent: textComp
            }
        }
    }
}
