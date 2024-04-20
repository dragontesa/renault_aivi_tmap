import QtQuick 2.0
import "../../GlobalConstants.js" as GConstants
import "../../view/util/Formatter.js" as Formatter
import "../../component"

Item {
    id: root

    property bool displaySpeedAndTime: true
    property int trafficIndex: -1
    property int directionIndex
    property int remainDistance
    property string routeBeginText
    property string routeEndText
    property int averageSpeed
    property int averageTime: 3650
    property alias dirSource: directionIcon.source
    property int mapX
    property int mapY
    property bool imageAsyncLoading: false
    property bool isCurrentItem: false
    property alias trafficVisible: trafficIcon.visible
    property bool isDeparture: false
    onIsDepartureChanged: directionIndex = 23//24
    property bool isArrival: false
    onIsArrivalChanged: directionIndex = 24//23
    property int type: -1   // 0 (departure)
                            // 1 (item)
                            // 2 (arrival)
                            // 3 (via)
    property bool focused: false
    property bool displayAddress: routeEndText.length === 0
    property color textColor: (focused && isCurrentItem) ? "#000000" : "#ffffff"
    property alias itemBackgroundVisible: itemBackground.visible

    onTypeChanged: {
        switch (type) {
        case 0:
        case 2:
            routeLoader.sourceComponent = compRouteOneItem
            break
        case 3:
            if (displayAddress)
                routeLoader.sourceComponent = compRouteOneItem
            else
                routeLoader.sourceComponent = compRouteTwoItems
            break
        case 1:
            routeLoader.sourceComponent = compRouteTwoItems
            break
        default:
            break
        }
    }

    function getDirectionIcon(dir, traffic) {
        switch (dir) {
        case 23: return "route/route_rg_bk_23.png"
        case 24: return "route/route_rg_bk_24.png"
        case 25: return "route/route_rg_passby1.png"
        case 26: return "route/route_rg_passby2.png"
        case 27: return "route/route_rg_passby3.png"

        default:
            break
        }

        var source = "route/route_rg_"
        switch (traffic) {
        case 0: source += "g_"
            break;
        case 1: source += "gr_"
            break;
        case 2: source += "y_"
            break;
        case 3: source += "r_"
            break;
        }

        if (dir < 10)
            source += "0"

        source += dir + ".png"
        return source
    }
    function getTrafficIcon(traffic) {
        var postfix = (lang === 'ko' ? '.png' : '_eng.png')
        switch (traffic) {
        case 0:
            return "route/route_tag_gray" + postfix
        case 1:
            return "route/route_tag_green" + postfix
        case 2:
            return "route/route_tag_yellow" + postfix
        case 3:
            return "route/route_tag_red" + postfix
        }
    }

    Component {
        id: compText
        Text {
            font { family: GConstants.kFontFamily; pixelSize: Resource.pixelToDp(35) }//pixelSize: 28 }
            color: root.textColor
            text: parent.text
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
        }
    }
    Component {
        id: compText2
        Text {
            font { family: GConstants.kFontFamily; pixelSize: Resource.pixelToDp(30) }//pixelSize: 24 }
            color: {
                if (focused && isCurrentItem)
                    return '#000000'
                else
                    return isCurrentItem ? root.textColor : '#8a8a99'
            }
            text: parent.text
            horizontalAlignment: Text.AlignRight
            verticalAlignment: Text.AlignVCenter
        }
    }

    Item {
        scale: layout.scale

        Item {
            width: GConstants.kScreenWidth
            height: GConstants.kListTextItemHeight

            Rectangle {
                id: itemBackground
                anchors.left: parent.left
                anchors.leftMargin: GConstants.kListBackgroundLeftMargin
                anchors.right: parent.right
                anchors.rightMargin: GConstants.kListBackgroundRightMargin
                height: layout.summaryItemHeight
                color: highlightColor; opacity: highlightOpacity
            }

            Image {
                id: directionIcon
                visible: !displayAddress
                x: 20
                width: 63
                fillMode: Image.PreserveAspectFit
                anchors.verticalCenter: parent.verticalCenter
                source: Resource.imageUrl(getDirectionIcon(directionIndex, trafficIndex))
            }
            Text {
                visible: displayAddress
                x: 20
                width: 63; height: parent.height
                font { family: GConstants.kFontFamily; pixelSize: Resource.pixelToDp(35) }//pixelSize: 28 }
                color: root.textColor
                text: {
                    switch (type) {
                    case 3: return qsTr("경유지")
                    default:
                        return ""
                    }
                }
                verticalAlignment: Text.AlignVCenter; horizontalAlignment: Text.AlignHCenter
            }

            Image {
                id: trafficIcon
                anchors.left: directionIcon.right
                anchors.leftMargin: 14
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 11
                width: 68
                visible: root.type == 1
                source: Resource.imageUrl(getTrafficIcon(trafficIndex))
            }
            Distance {
                y: 10
                anchors.left: trafficIcon.left
                visible: root.type == 1
                distance: root.remainDistance
                traffic: {
                    switch (root.trafficIndex) {
                    case 0:
                        return "gray"
                    case 1:
                        return "green"
                    case 2:
                        return "yellow"
                    case 3:
                        return "red"
                    default:
                        return "gray"
                    }
                }

                getNumSources: function(index) {
                    return "route/route_num_" + traffic + "_" + index + ".png"
                }
                m: "route/route_num_" + traffic + "_m.png"
                km: "route/route_num_" + traffic + "_km.png"
                dot: "route/route_num_" + traffic + "_dot.png"
            }

            Item {
                id: routeText

                width: childrenRect.width
                height: childrenRect.height
                anchors.left: trafficIcon.right
                anchors.leftMargin: 30
                anchors.verticalCenter: parent.verticalCenter

                // 시작 -> 끝
                Component {
                    id: compRouteTwoItems
                    Item {
                        width: 449
                        height: 32

                        Loader {
                            id: textLoader
                            sourceComponent: compText
                            property string text: routeBeginText
                        }
                        Image {
                            id: arrow
                            anchors.left: textLoader.right
                            source: Resource.imageUrl("route/route_list_arw.png")
                        }
                        Loader {
                            id: textLoader2
                            anchors.left: arrow.right
                            sourceComponent: compText
                            property string text: routeEndText
                        }
                        Component.onCompleted: {
                            textLoader2.visible = textLoader2.x < width
                            arrow.visible = (arrow.x + arrow.width) < width
                            if (textLoader2.x + textLoader2.width > width) {
                                textLoader2.width = width - textLoader2.x
                            }
                            if (textLoader.x + textLoader.width > width) {
                                textLoader.width = width - textLoader.x
                            }
                        }
                    }
                }
                // 주소만 있는 경우
                Component {
                    id: compRouteOneItem
                    Loader {
                        width: 449
                        height: 32

                        sourceComponent: compText
                        property string text: routeBeginText
                    }
                }

                Loader {
                    id: routeLoader
                }
            }//id: routeText

            Loader {
                id: averageSpeedText
                property string text: root.averageSpeed + "Km/h"
                sourceComponent: compText2
                anchors.right: parent.right
                anchors.rightMargin: 40
                y: 11
                width: 106
                height: 28
                visible: displaySpeedAndTime && (root.type == 1)
            }
            Loader {
                id: averageTimeText
                property string text: Formatter.getTimeText(averageTime)
                sourceComponent: compText2
                anchors.left: averageSpeedText.left
                anchors.top: averageSpeedText.bottom
                anchors.topMargin: 4
                width: 106
                height: 28
                visible: displaySpeedAndTime && (root.type == 1)
            }
        }
    }
    Rectangle {
        id: underline
        visible: !root.isCurrentItem || !isInMap
        anchors.left: parent.left
        anchors.leftMargin: GConstants.kListLineLeftMargin
        anchors.right: parent.right
        anchors.rightMargin: GConstants.kListLineRightMargin
        anchors.bottom: parent.bottom

        height: 1
        color: GConstants.kListLineColor
    }
}
