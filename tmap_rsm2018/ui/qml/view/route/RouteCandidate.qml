import QtQuick 2.0
import "../../view/util/Formatter.js" as Formatter
import "../../GlobalConstants.js" as GConstants
import "../../component"
import "../../view"
import '../../view/util'

Item {
    id: root

    property bool checked: false
    property alias title: titleText.text
    property int time
    property real distance
    property int fee
    property point minPoint
    property point maxPoint
    property string postfix: "_n"
    property bool seperatorVisible: true
    property bool focused: false
    property bool noData: minPoint.x == 0 && minPoint.y == 0 && maxPoint.x == 0 && maxPoint.y == 0
    property bool selected: false

    property int modelIndex

    signal clicked(int index)
    signal searchClicked(int index)

    function getHour(t) {
        return t / 3600
    }
    function getMin(t) {
        if (t < 3600)
            return t / 60
        else
            return (t % 3600) / 60
    }
    function hideSearchText() {
        searchText.visible = false
    }

    onCheckedChanged: {
        remainTime.setRemainTime(remainTime.hour, remainTime.min)
    }

    function getTextColor() {
        switch (modelIndex) {
        case 0:
            return "#2bb7cd"
        case 1:
            return "#4ca6ff"
        case 2:
            return "#88b800"
        }
    }

    state: {
        if (focused) {
            return selected ? "csw-focus" : "normal"
        } else {
            return selected ? "selected" : "normal"
        }
    }

    states: [
        State {
            name: "normal"
            PropertyChanges { target: background; color: "#3a3a40" }
            PropertyChanges { target: root; postfix: "_n" }
            PropertyChanges { target: distanceText; color: "#abaeb2" }
            PropertyChanges { target: searchText; color: "#abaeb2" }
        },
        State {
            name: "selected"
            PropertyChanges { target: background; color: "#2d2d31" }
            PropertyChanges { target: root; postfix: "_s" }
            PropertyChanges { target: distanceText; color: "#ffffff" }
            PropertyChanges { target: searchText; color: "#ffffff" }
        },
        State {
            name: "csw-focus"
            PropertyChanges { target: background; color: "#ffffff" }
            PropertyChanges { target: root; postfix: "_f"; }
            PropertyChanges { target: distanceText; color: "#000000" }
            PropertyChanges { target: searchText; color: "#000000" }
        }
    ]
    Item {
        id: item

        scale: layout.internalItemScale

        Rectangle {
            id: background
            width: modelIndex !== 2 ? layout.internalItemWidth : layout.internalItemWidth - layout.itemWidthPadding
            height: layout.internalItemHeight
            MouseArea {
                anchors.fill: background
                onClicked: {
                    root.clicked(root.modelIndex)
                }
            }

            Item {
                id: title
                x: 24
                y: 38
                width: 224; height: 30

                Text {
                    id: titleText
                    font { family: GConstants.kFontFamily; pixelSize: Resource.pixelToDp(33) }//pixelSize: 26 }
                    color: getTextColor()
                }
            }
            RemainTime {
                id: remainTime
                visible: !noData
                anchors{ top: title.top; topMargin: 38; left: title.left }
                hourTextSource: lang == 'ko' ? "route/route_num_hour" : "route/route_num_hour_eng"
                minTextSource: lang == 'ko' ? "route/route_num_minute" : "route/route_num_minute_eng"
                numSources: "route/route_num"
                hour: getHour(root.time)
                min: getMin(root.time)
                state: root.state
            }
            Text {
                id: searchText
                anchors{ top: title.top; topMargin: 38; left: title.left }
                visible: noData && (modelIndex != 0)
                font { family: GConstants.kFontFamily; pixelSize: Resource.pixelToDp(50) }
                verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter
                text: qsTr('조회') + Tr.d
                color: '#abaeb2'
                MouseArea { anchors.fill: parent;
                    onClicked: {
                        searchClicked(root.modelIndex)
                    }
                }
            }

            Row {
                anchors.top: remainTime.top
                anchors.topMargin: 60
                anchors.left: title.left
                spacing: 5
                Image {
                    source: Resource.imageUrl("route/route_list_distance" + postfix + ".png")
                }
                Item {
                    width: distanceText.contentWidth
                    height: 28
                    Text2 {
                        id: distanceText
                        anchors.fill: parent
                        verticalAlignment: Text.AlignVCenter
                        offsetY: 2
                        font { family: GConstants.kFontFamilySKP; pixelSize: Resource.pixelToDp(28) }//pixelSize: 23 }
                        text: noData ? qsTr('--km') + Tr.d : Formatter.getDistanceText(root.distance) + Tr.d
                    }
                }
                Image {
                    source: Resource.imageUrl("route/route_list_charge" + postfix + ".png")
                }
                Item {
                    width: feeText.contentWidth
                    height: 28
                    Text2 {
                        id: feeText
                        text: noData ? qsTr('-,---') + Tr.d : Formatter.getFeeText(root.fee, '') + Tr.d
                        anchors.fill: parent
                        verticalAlignment: Text.AlignVCenter
                        offsetY: 2
                        font: distanceText.font
                        color: distanceText.color
                    }
                }
            }
            Image {
                source: Resource.imageUrl("route/route_point.png")
                anchors.bottom: parent.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                visible: checked
            }
        }

        Rectangle {
            anchors.top: background.top
            anchors.bottom: background.bottom
            anchors.right: background.right
            width: 1
            color: "#000000"
            visible: seperatorVisible
        }
    }
}
