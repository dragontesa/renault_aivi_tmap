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
    property bool displayAddress: false
    property bool forTest: false

    onIsRgChanged: {
        if (isRg) {
            displayAddress = false
            timer.running = true
        } else {
            timer.running = false
        }
    }

    signal searchClicked

    width: 396
    height: 58

    Timer {
        id: timer
        repeat: true
        interval: 5*1000
        onTriggered: {
            displayAddress = !displayAddress
        }
    }

    function toggleRgStatusBar(toggle) {
        if (toggle) {
            hasDestinationItem.visible = true
            noneDestinationItem.visible = false
        } else {
            hasDestinationItem.visible = false
            noneDestinationItem.visible = true
        }
    }

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
            width: Math.min(contentWidth, 265)
            height: 26
            fontSize: 29
            textColor: '#ffffff'
            textOpacity: 0.85
            text: address
            textOffsetY: 2
            enableMarquee: width <= contentWidth
        }
    }

    Rectangle {
        anchors.fill: parent
        color: GlobalConstants.kGadgetBrightBgColor
    }

    Item {
        anchors.fill: parent

        // 좌측 검색 아이콘
        Item {
            id: searchItem

            width: 63
            height: parent.height

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

        // 세로선
        Rectangle {
            width: 1
            height: 36
            anchors.right: searchItem.right
            anchors.verticalCenter: searchItem.verticalCenter
        }

        // 목적지 있을 경우의 Rg 상태 정보
        Item {
            id: hasDestinationItem

            visible: isRg && !displayAddress
            anchors { left: searchItem.right; right: parent.right }
            height: parent.height

            Row {
                x: 16
                height: parent.height

                Item {
                    width: 146; height: parent.height

                    SoftkeyRemainDistance {
                        id: remainDistance
                        anchors.centerIn: parent
                        distance: root.remainDistance
                    }
                }
                Item {
                    width: 154; height: parent.height
                    SoftkeyArrivalTime {
                        id: arrivalTime
                        anchors.centerIn: parent
                        format: timeFormat
                    }
                }
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    if (isRg) toggleRgStatusBar(false)
                }
            }
        }

        // 목적지가 없을 경우의 Rg 상태 정보
        Item {
            id: noneDestinationItem

            visible: !isRg || displayAddress
            anchors { left: searchItem.right; right: parent.right }
            height: parent.height

            Row {
                id: row
                x: 18
                width: childrenRect.width
                height: childrenRect.height
                spacing: 6
                anchors.centerIn: parent

                Image {
                    source: Resource.imageUrl("map_ic_yah.png")
                }
                Loader {
                    id: textLoader2
                    height: 26
                    sourceComponent: textComp
                }
            }
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    if (isRg) toggleRgStatusBar(true)
                }
            }
        }
    }
}
