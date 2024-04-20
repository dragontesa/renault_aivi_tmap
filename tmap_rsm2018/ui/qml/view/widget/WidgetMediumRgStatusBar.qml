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

    property string remainDistance
    property int remainTime

    signal clicked

    width: 396; height: 58

    onRemainTimeChanged: {
        var t = gpsTimeUpdated
                ? CommonUtils.getArrivalTimeUsingTime(gpsHour, gpsMin, gpsSec, remainTime)
                : CommonUtils.getArrivalTimeUsingDate(new Date(), remainTime)
        var h = CommonUtils.getHour(t)
        var m = CommonUtils.getMin(t)
        arrivalTime.setArrivalTime(h, m)
    }

    // 좌측 검색 아이콘
    Component {
        id: searchComp

        Item {
            anchors.fill: parent

            Image {
                id: searchIcon

                anchors.fill: parent

                source: Resource.imageUrl("zone/zone3_btn_search.png")
                MouseArea {
                    anchors.fill: parent
                    onClicked: root.clicked()
                }
            }
        }
    }

    // 목적지 있을 경우의 Rg 상태 정보

    Item {
        id: rgItem

        visible: isRg
        anchors.fill: parent

        Loader {
            id: searchItemLoader

            width: 63; height: 58
            sourceComponent: searchComp
        }

        // 세로선
        Rectangle {
            width: 1
            height: 36
            anchors.right: searchItemLoader.right
            anchors.verticalCenter: searchItemLoader.verticalCenter
        }

        Rectangle {
            width: parent.width
            height: parent.height

            color: GlobalConstants.kGadgetBrightBgColor
        }

        Row {
            anchors.left: searchItemLoader.right
            height: parent.height

            Item {
                width: 140; height: parent.height

                SoftkeyRemainDistance {
                    id: remainDistance
                    anchors.centerIn: parent
                    distance: root.remainDistance
                }
            }
            Item {
                width: 146; height: parent.height
                SoftkeyArrivalTime {
                    id: arrivalTime
                    anchors.centerIn: parent
                    format: timeFormat
                }
            }
        }
    }

    // 목적지가 없을 경우의 Rg 상태 정보
    Rectangle {
        id: noRgItem

        visible: !isRg
        anchors.fill: parent
        color: GlobalConstants.kGadgetBrightBgColor
    }
    Loader {
        visible: !isRg
        width: 63; height: 58
        sourceComponent: searchComp
        anchors.centerIn: noRgItem
    }
}
