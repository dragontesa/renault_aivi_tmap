import QtQuick 2.0

import CommonConstants 1.0
import Constants 1.0
import Csw 1.0
import LayoutConstants 1.0
import RouteSummaryViewEvent 1.0
import SceneGraphRendering 1.0
import ViewRegistry 1.0

import "../../singleton"
import "../../component"
import "../../view"
import "../util/Formatter.js" as Formatter
import "../../GlobalConstants.js" as GlobalConstants

PopupView {
    id: root

    property var caller: null
    property color highlightColor: MultiSenseCoordinator.kAmbientColor
    property real highlightOpacity: 0.0
    property bool highlightVisible: true
    property int routeId: 0
    property int routeOption: 1
    property int itemCount: 0
    property bool focused: false
    property bool isInMap: true
    property bool blockCheckInMap: false
    property bool isServerData: false
    property size mapSize
    property int pressedIndex: -1
    property string filename
    property string lang

    background.visible: false
    mouseArea.enabled: false

    property int screenResolution: 0
    layout: RouteSummaryViewLayout {
        onUpdateLayoutCompleted: {
            root.screenResolution = screenResolution

            if (screenResolution === LayoutConstants.L800x480) {
                routeSummaryBar.anchors.top = mapPart.top
                routeSummaryBar.anchors.left = mapPart.left
                routeSummaryBar.anchors.right = mapPart.right

                uiSide.anchors.left = mapPart.right
                uiSide.anchors.top = mapPart.top
                uiSide.anchors.right = view.right
                uiSide.height = layout.summaryItemHeight * 8
            } else if (screenResolution === LayoutConstants.L820x1024 || screenResolution === LayoutConstants.L1920x720) {
                routeSummaryBar.anchors.top = mapPart.top
                routeSummaryBar.anchors.left = mapPart.left
                routeSummaryBar.anchors.right = mapPart.right

                uiSide.anchors.left = view.left
                uiSide.anchors.right = view.right
                uiSide.anchors.top = mapPart.top
                uiSide.anchors.topMargin = 377
                uiSide.height = layout.summaryItemHeight * 4
            }
        }
    }

    signal mapPositionIssued(int x, int y)

    function retranslateUi() {
        if (root.visible) {
            var title = qsTr('경로 요약 | ') + Formatter.getRouteOptionText(routeOption)
            var data = {'title':title}
            if (caller === 'MapModeOptionView') {
                data.menus = ["noOption"]
            } else {
                data.menus = [qsTr("모의 주행"), qsTr("안내 시작"), "noOption"]
            }
            sendEvent(RouteSummaryViewEvent.RetranslateUi, data)
            routeSummaryBar.retranslationUi()

            var model = routeSummaryListView.model
            routeSummaryListView.model = undefined
            routeSummaryListView.model = model
        }
    }

    function initialize() {
        mapView.viewMode = Renderer.NorthBound
        mapView.moveMode = Renderer.Normal

        routeSummaryListView.currentIndex = -1
        pressedIndex = -1
    }

    function finalize() {
        mapView.viewMode = Renderer.NorthBound
    }

    function drawRouteAll() {
        var w = mapView.width
        var h = mapView.height

        mapView.drawRouteAll(routeId, w, h, 0)
    }

    Timer {
        id: drawTimer;
        interval: 50; running: false; repeat: false
        onTriggered: {
            sendEvent(RouteSummaryViewEvent.DrawRouteAll)
        }
    }

    onMediated: {
        console.log("[Perf][RouteSummaryView]onMediated");
        sendEvent(RouteSummaryViewEvent.Mediated, mapView)
    }
    onActivated: {
        var title = qsTr('경로 요약 | ') + Formatter.getRouteOptionText(routeOption)
        var data = { title: title }
        if (caller === 'MapModeOptionView') {
            data.menus = ["noOption"]
        } else {
            data.menus = [qsTr("모의 주행"), qsTr("안내 시작"), "noOption"]
        }

        sendEvent(RouteSummaryViewEvent.Activated, data)

        var padding = { r : layout.rightPadding, b : layout.bottomPadding }
        sendEvent(RouteSummaryViewEvent.ViewChanged, padding)

        sendEvent(RouteSummaryViewEvent.RegistCSW, csw)

        initialize()

        drawTimer.start()
    }
    onDeactivated: {
        sendEvent(RouteSummaryViewEvent.RemoveCSW, csw)
        finalize()
        sendEvent(RouteSummaryViewEvent.Deactivated)
    }

    function updateRouteSummarySimple(data)
    {
        var distance = data['distance']
        var fee = data['fee']
        var remainTime = data['remainTime']

        routeSummaryBar.updateValues(distance, remainTime, fee)

        console.log("[Perf][RouteSummaryView]updateRouteSummarySimple");
    }

    function updateRouteSummaryDetail(data)
    {
        var model = data['model']
        var remainDistance = data['remainDistance']
        var selectId = data['select']
        root.itemCount = data['itemCount']

        routeSummaryListView.model = model

        console.log("[Perf][RouteSummaryView]updateRouteSummaryDetail");
    }

    function getItemType(index, turn, listCount) {
        var ret = 1 // default

        if (index === 0)
            ret = 0
        if (index === listCount-1)
            ret = 2
        switch (turn) {
        case 25:
        case 26:
        case 27:
            ret = 3
            break
        }

        return ret;
    }
    function getRouteText(index, listCount, turn, text) {
        switch (index) {
        case 0:
            return qsTr("출발지: ") + text
        case 1:
            return text.length > 0 ? text : qsTr("출발지")
        default:
            if (index === listCount-1)
                return qsTr("도착지: ") + text
            else
                return text;
        }
    }

    Connections {
        target: mapView
        onCenterChanged: {
            var curItem = routeSummaryListView.currentItem
            if (curItem != null) {
                // 리스트 아이템 선택 시 지도 이동에 의해 onCenterChanged 호출
                // 리스트 아이템 이후 처음 한번은 체크하지 않도록 처리
                if (blockCheckInMap) {
                    blockCheckInMap = false
                } else {
                    sendEvent(RouteSummaryViewEvent.CheckItemInMapView, {x:curItem.mapX, y:curItem.mapY})
                }
            }
        }
    }

    Item {
        id: view

        anchors.fill: parent

        Component.onDestruction: {
            mapView.setMapCenterByScreen(GlobalConstants.kScreenWidth/2, GlobalConstants.kScreenWidth/2)
        }

        Item {
            id: mapPart
            width: layout.viewWidth
            height: layout.viewHeight

            RouteSummaryBar {
                id: routeSummaryBar

                width: layout.summaryBarWidth
                height: layout.summaryBarHeight
            }
        }
        Item {
            id: uiSide

            Rectangle {
                anchors.fill: routeSummaryListView
                color: GlobalConstants.kListNormalColor
            }
            ListView {
                id: routeSummaryListView
                objectName: "ListView"

                function setCurrentIndex(index)
                {
                    if (mapView.moveMode !== Renderer.Normal) {
                    }
                    routeSummaryListView.currentIndex = index
                }
                function mapMoveTo(index) {
                    mapView.viewLevel = 14

                    var x = routeSummaryListView.currentItem.mapX
                    var y = routeSummaryListView.currentItem.mapY
                    // for summary
                    mapView.moveTo(x, y, false)
                }

                onContentYChanged: {
                    // scroll
                    highlightOpacity = 0.0
                    pressedIndex = -1
                }

                anchors.fill: parent
                clip: true

                Csw {
                    id: csw
                    section: Csw.Section_RouteSummary
                    sequence: 0
                    onFocused: {
                        routeSummaryListView.currentIndex = 0
                        routeSummaryListView.setCurrentIndex(routeSummaryListView.currentIndex)
                        root.highlightColor = GlobalConstants.kListFocusColor
                        root.focused = true
                    }
                    onLostFocusd: {
                        root.highlightColor = MultiSenseCoordinator.kAmbientColor
                        root.focused = false
                    }
                    onSpinCcwPressed: {
                        if (0 < routeSummaryListView.currentIndex) {
                            --routeSummaryListView.currentIndex
                            routeSummaryListView.setCurrentIndex(routeSummaryListView.currentIndex)
                        }
                    }
                    onSpinCwPressed: {
                        var model = routeSummaryListView.model
                        if (routeSummaryListView.currentIndex < routeSummaryListView.count-1) {
                            ++routeSummaryListView.currentIndex
                            routeSummaryListView.setCurrentIndex(routeSummaryListView.currentIndex)
                        }
                    }
                }

                delegate: RouteSummaryListItem {
                    objectName: "delegate" + index
                    width: layout.summaryItemWidth
                    height: layout.summaryItemHeight
                    imageAsyncLoading: true

                    mapX: X
                    mapY: Y
                    trafficIndex: Traffic
                    directionIndex: Turn //== 0 ? 1 : Turn
                    type: getItemType(index, Turn, routeSummaryListView.count)
                    routeBeginText: getRouteText(index, routeSummaryListView.count, Turn, From) + Tr.d
                    routeEndText: To
                    averageSpeed: Speed
                    averageTime: RemainTime
                    remainDistance: TurnRemainDistance
                    focused: root.focused
                    isCurrentItem: index === routeSummaryListView.currentIndex
                    isDeparture: (index === 0)
                    isArrival: index === root.itemCount - 1
                    itemBackgroundVisible: (pressedIndex === index)
                    displaySpeedAndTime: isServerData

                    MouseArea {
                        anchors.fill: parent
                        onPressed: {
                            csw.forceLostFocus()

                            root.highlightColor = MultiSenseCoordinator.kAmbientColor
                            root.highlightOpacity = 0.7
                            root.focused = false

                            if (pressedIndex === index) {
                                highlightVisible = false
                            } else {
                                pressedIndex = index
                            }
                        }
                        onClicked: {
                            blockCheckInMap = true
                            routeSummaryListView.setCurrentIndex(index)
                            routeSummaryListView.mapMoveTo(index)
                            isInMap = true
                            highlightVisible = true
                        }
                    }
                }
                highlight: Item {
                    width: layout.summaryItemWidth
                    Rectangle {
                        anchors.left: parent.left
                        anchors.leftMargin: GlobalConstants.kListBackgroundLeftMargin
                        anchors.right: parent.right
                        anchors.rightMargin: GlobalConstants.kListBackgroundRightMargin
                        height: layout.summaryItemHeight
                        color: highlightColor
                    }
                    visible: root.isInMap && highlightVisible
                }
                highlightMoveDuration: 1

                Image {
                    anchors.bottom: parent.bottom
                    width: parent.width; height: 30
                    visible: scrollBar.heightRatio < 1.0
                    source: Resource.imageUrl("zone/zone2_img_mask.png")
                }

                TScrollBar {
                    id: scrollBar
                    flickable: routeSummaryListView
                    width: 2
                    color: MultiSenseCoordinator.kAmbientColor
                    grooveColor: GlobalConstants.kScrollBarBackColor
                    rightMargin: GlobalConstants.kScrollBarRightMargin
                }
            }
        }
    }
}
