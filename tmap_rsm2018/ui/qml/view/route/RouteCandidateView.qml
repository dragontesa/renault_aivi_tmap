import QtQuick 2.0
import SceneGraphRendering 1.0
import "../../view/util/Formatter.js" as Formatter
import "../../GlobalConstants.js" as GConstants
import "../../component"
import "../../view"
import ApplicationEvent 1.0
import RouteCandidateViewEvent 1.0
import Csw 1.0

PopupView {
    id: root

    property int modelCount: 3
    property int selectedIndex: -1
    property bool focused: false
    property alias indicatorVisible: indicator.visible
    property int paddingTop: layout.itemHeight + routeCandidateArtary.height
    property int paddingBottom: GConstants.kScreenZone3Height
    property bool dataLoaded: false
    property bool isOnline: false
    property bool isRequested: false
    property string caller
    property string lang: 'ko'

    mouseArea.enabled: false
    layout: RouteCandidateViewLayout {}

    function retranslateUi() {
        if (root.visible) {
            var routeMenu = dataLoaded ? qsTr("안내시작") : qsTr("안내시작:disabled")
            var data = {
                title: qsTr("경로 탐색 결과"),
                menus:[qsTr("다른 경로"), qsTr("경유지 설정"), routeMenu],
            };
            sendEvent(RouteCandidateViewEvent.RetranslateUi, data)
        }
    }

    function updateMap(routeId) {
        if (root.visible) {
            console.log('[RouteCandidate]updateMap viewMode: ' + viewMode + ' moveMode: ' + moveMode)
            mapView.viewMode = Renderer.NorthBound
            mapView.moveMode = Renderer.Normal
            mapView.positionIcon = Renderer.PositionIconNone
        }
    }

    function selectRouteCandidate(routeId) {
        selectedIndex = routeId
        listViewLoader.item.currentIndex = routeId
        var currentItem = listViewLoader.item.currentItem

        var data = {
            min: currentItem.minPoint,
            max: currentItem.maxPoint,
            distance: currentItem.distance,
            fee: currentItem.fee,
            remainTime: currentItem.time
        }

        if (0 < currentItem.distance) {
            view.selectRoute(routeId, data)
        }
    }

    function setCurrentRouteCandidate(routeId) {
        listViewLoader.item.currentIndex = routeId
    }

    function spinCw() {
        if (selectedIndex < listViewLoader.item.count -1) {
            selectedIndex++
            setCurrentRouteCandidate(selectedIndex)
            focused = true
        }
    }
    function spinCcw() {
        if (0 < selectedIndex) {
            selectedIndex--
            setCurrentRouteCandidate(selectedIndex)
            focused = true
        }
    }
    function cancelDrawRouteAll() {
        mapView.positionIcon = Renderer.PositionIconCarvatar
        mapView.cancelDrawRouteAll()
    }

    onMediated: {
        sendEvent(RouteCandidateViewEvent.Mediated)
    }
    onActivated: {
        var model = listViewLoader.model
        var routeMenu = dataLoaded ? qsTr("안내시작") : qsTr("안내시작:disabled")
        var data = {
            title: qsTr("경로 탐색 결과"),
            menus:[qsTr("다른 경로"), qsTr("경유지 설정"), routeMenu],
            width: root.width,
            height: root.height
        };
        sendEvent(RouteCandidateViewEvent.Activated, data)

        sendEvent(RouteCandidateViewEvent.ViewSizeChanged)
    }
    onDeactivated: {
        sendEvent(RouteCandidateViewEvent.Deactivated)
    }

    function showOptionMenu() {
        optionMenu.visible = true
    }

    function getRouteTypeText(type) {
        if (type === "BestPath")
            return qsTr("최적길") + Tr.d
        else if (type === "MinimumTime")
            return qsTr("최소시간") + Tr.d
        else if (type === "FreeRoad")
            return qsTr("무료도로") + Tr.d
        else if (type === "ShortestDistance")
            return qsTr("최단거리") + Tr.d
        else if (type === "ExpresswayPriority")
            return qsTr("고속도로우선") + Tr.d
        else
            return qsTr("초보자경로") + Tr.d
    }

    function resetModel(model) {
        listViewLoader.sourceComponent = null
        listViewBG.visible = false

        listViewLoader.model = model
        listViewLoader.sourceComponent = compListView
        selectedIndex = -1
        indicator.index = 0
        indicator.x = 0
        indicator.visible = true
        dataLoaded = false

        routeCandidateArtary.setArtary('', '')
    }

    function showIndicator(index) {
        indicator.visible = true
        indicator.x = index * layout.itemWidth
    }

    function updateRouteSummary(data, routeCount) {
        var model = data['model']
        var from = data['from']
        var to = data['to']
        listViewLoader.sourceComponent = null
        listViewLoader.model = model

        indicator.visible = false
        dataLoaded = true
        isRequested = false

        routeCandidateArtary.setArtary(from, to)
        listViewLoader.sourceComponent = compListView
    }
    function updateRoadInfo(from, to) {
        routeCandidateArtary.setArtary(from, to)
    }

    Component {
        id: compListView

        ListView {
            id: listView

            orientation: ListView.Horizontal
            interactive: false

            delegate: RouteCandidate {
                modelIndex: index
                width: index !== 2 ? layout.itemWidth : layout.itemWidth - layout.itemWidthPadding
                height: layout.itemHeight
                seperatorVisible: index !== 2
                title: getRouteTypeText(Title)
                time: RemainTimeValue
                distance: RemainDistance
                fee: Fee
                minPoint: MinPoint
                maxPoint: MaxPoint
                selected: index === listView.currentIndex
                focused: root.focused

                function requestRoute(index) {
                    var param = {'index' : index}
                    sendEvent(RouteCandidateViewEvent.RequestRoute, param)

                    isRequested = true

                    showIndicator(index)
                    checked = true
                    hideSearchText()

                    var menus = [qsTr("다른 경로"), qsTr("경유지 설정"), qsTr("안내시작:disabled")]
                    sendEvent(RouteCandidateViewEvent.SetupSoftkeyMenu, menus)
                }

                onClicked: {
                    csw.forceLostFocus()

                    var requested = false

                    if (minPoint.x !== 0 && minPoint.y !== 0 && maxPoint.x !== 0 && maxPoint.y !== 0) {
                        selectRouteCandidate(index)
                        checked = true
                        hideSearchText()
                    } else {
                        if (!isRequested) {
                            requestRoute(index)                            
                            requested = true
                        }
                    }

                    if (!requested && !isRequested) {
                        sendEvent(RouteCandidateViewEvent.RequestCancelCountdown)
                    }
                }
                onSearchClicked: {
                    csw.forceLostFocus()

                    if (!isRequested) {                        
                        requestRoute(index)
                    }
                }

                onCheckedChanged: {
                    if (checked) {
                        var data = {
                            min: minPoint,
                            max: maxPoint,
                            distance: distance,
                            fee: fee,
                            remainTime: time
                        }
                    }
                }
            }
        }
    }

    Item {
        id: view

        y: 0
        anchors.fill: parent

        function selectRoute(routeId, data) //p0, p1, distance)
        {
            var p0 = data['min']
            var p1 = data['max']
            var distance = data['distance']
            var fee = data['fee']
            var remainTime = data['remainTime']
            var x = p0.x + (p1.x - p0.x) / 2
            var y = p0.y + (p1.y - p0.y) / 2

            sendEvent(RouteCandidateViewEvent.RouteSelected,
                      { "routeId":routeId,
                        "w":GConstants.kScreenWidth,
                        "h":GConstants.kScreenHeight,
                        "x":x,
                        "y":y,
                        "distance":distance,
                        "fee":fee,
                        "remainTime":remainTime
                      })

            //optionMenu.visible = false
        }

        Rectangle {
            id: listViewBG
            anchors.fill: listViewLoader
            color: "#000000" //"#3a3a40"
            visible: true
            onHeightChanged: {
                if (height) {
                    var data = { t: height }
                    sendEvent(RouteCandidateViewEvent.ViewSizeChanged, data)
                }
            }
        }

        Loader {
            id: listViewLoader

            property var model
            width: GConstants.kScreenWidth
            height: layout.itemHeight

            onLoaded: {
                item.model = model
                listViewBG.visible = true
            }

        }
        RouteSearchIndicator {
            id: indicator
            x: 0
            visible: true
        }
        Csw {
            id: csw

            section: Csw.Section_RouteCandidate
            sequence: 0
            onSpinCwPressed: {
                spinCw()
            }
            onSpinCcwPressed: {
                spinCcw()
            }
            onFocused: {
                sendEvent(RouteCandidateViewEvent.RequestCancelCountdown)
                root.focused = true
                if (root.selectedIndex < 0) {
                    root.selectedIndex = 0
                    selectRouteCandidate(root.selectedIndex)
                }
            }
            onLostFocusd: {
                root.focused = false
            }
            onSelected: {
                var listView = listViewLoader.item
                var index = listView.currentIndex
                if (0 < listView.currentItem.distance) {
                    selectRouteCandidate(index)
                } else {
                    listView.currentItem.clicked(index)
                }
            }
        }

        RouteCandidateArtary {
            id: routeCandidateArtary
            anchors.top: parent.top
            anchors.topMargin: layout.itemHeight - 1
        }
        Connections {
            target: mapView
            onHitting: {
                if (root.visible && !isRequested)
                    sendEvent(RouteCandidateViewEvent.RequestCancelCountdown)
            }
            onTouchDown: {
                if (root.visible && !isRequested)
                    sendEvent(RouteCandidateViewEvent.RequestCancelCountdown)
            }
        }
    }//BaseView

    TImageButton {
        id: restorationButton
        width: layout.buttonSize
        height: layout.buttonSize

        bg.anchors.fill: restorationButton
        icon.anchors.fill: restorationButton

        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        anchors.left: parent.left
        anchors.leftMargin: 10

        Csw {
            id: restorationCsw
            section: Csw.Section_Map
            sequence: 0
            onFocused: {
                restorationButton.state = 'focused'
            }
            onLostFocusd: {
                restorationButton.state = 'normal'
            }
            onSelected: {
                sendEvent(RouteCandidateViewEvent.ReturnClicked, isRequested)
            }
        }

        bgSources: function (state) {
            switch (state) {
            case "normal": return "map_btn_bg_n.png"
            case "pressed":return "map_btn_bg_p.png"
            case "focused":return "map_btn_bg_f.png"
            }
        }
        iconSources: function (state) {
            switch (state) {
            case "normal":
            case "pressed":
                return "map_btn_restoration.png"
            case "focused":
                return "map_btn_restoration_f.png"
            }
        }
        onClicked: {
            sendEvent(RouteCandidateViewEvent.ReturnClicked, isRequested)
        }
    }
}//TPopup
