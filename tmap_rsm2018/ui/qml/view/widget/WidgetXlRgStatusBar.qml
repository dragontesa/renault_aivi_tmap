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
    height: 58

    onRemainTimeChanged: {
        var t = gpsTimeUpdated
                ? CommonUtils.getArrivalTimeUsingTime(gpsHour, gpsMin, gpsSec, remainTime)
                : CommonUtils.getArrivalTimeUsingDate(new Date(), remainTime)
        var h = CommonUtils.getHour(t)
        var m = CommonUtils.getMin(t)
        arrivalTime.setArrivalTime(h, m)
    }

    Component {
        id: textComp

        TListMarqueeText {
            width: isRg ? Math.min(contentWidth, 215) : Math.min(contentWidth, 467)
            height: 26
            fontSize: 29
            textColor: '#ffffff'
            textOpacity: 0.85
            text: address
            textOffsetY: 2
            enableMarquee: width <= contentWidth
        }

    }

    // 좌측 검색 아이콘
    Item {
        id: searchItem

        width: 63
        height: 58

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

    Rectangle {
        anchors { left: searchItem.right; right: parent.right }
        height: parent.height
        color: "#222226"
        opacity: 0.85
    }

    // 목적지 있을 경우의 Rg 상태 정보

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
                width: 283; height: parent.height
                Image {
                    id: poiIcon
                    anchors.left: parent.left
                    anchors.leftMargin: 18
                    anchors.verticalCenter: parent.verticalCenter
                    source: Resource.imageUrl("map_ic_yah.png")
                }
                Loader {
                    id: textLoader1
                    height: parent.height
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
        anchors { left: searchItem.right; leftMargin: 18; right: parent.right; rightMargin: 18 }
        height: parent.height

        Row {
            id: row
            width: childrenRect.width
            height: parent.height
            spacing: 6
            anchors.centerIn: parent

            Image {
                source: Resource.imageUrl("map_ic_yah.png")
                anchors.verticalCenter: parent.verticalCenter
            }
            Loader {
                id: textLoader2
                height: parent.height
                sourceComponent: textComp
            }
        }
    }
}
