import QtQuick 2.0
import RouteSettingViewEvent 1.0
import Csw 1.0
import "../../component"
import "../../view"
import "../../GlobalConstants.js" as GlobalConstants

PopupView {
    id: root

    property var caller: null
    property bool isViaEmpty: true
    property bool isEnableDrag: true
    property var selectedItem
    property int slot: -1
    property int enableViaItems: 0
    property int editViaIndex: -1
    property bool isEditMode: listViewController.visible
    property bool blockFocusToMain: false
    property string lang

    background.visible: false
    fader.fadingOpacity: 0
    mouseArea.enabled: false
    layout: RouteSettingViewLayout {
        onUpdateLayoutCompleted: {
            listView.model = listModel
            listView.height = layout.itemHeight * listModel.count
            listView.currentIndex = -1
        }
    }

    readonly property int kPoiTypeStart: 0
    readonly property int kPoiTypeVia: 1
    readonly property int kPoiTypeDestination: 2

    onMediated: {
        sendEvent(RouteSettingViewEvent.Mediated)
    }
    onActivated: {
        var viewHeight = layout.itemHeight * 5
        var viewData = { t: viewHeight, b: layout.mapPaddingY }
        sendEvent(RouteSettingViewEvent.ViewSizeChanged, viewData)

        var data = {
            title: qsTr("경유지 설정"),
            menus: [qsTr("경로탐색"), "noOption"]
        }
        sendEvent(RouteSettingViewEvent.SetupTitleAndZoneMenu, data)
        registCsws()

        sendEvent(RouteSettingViewEvent.Activated)

    }
    onDeactivated: {
        sendEvent(RouteSettingViewEvent.Deactivated)
    }

    onVisibleChanged: {
        if (visible)
            sendEvent(RouteSettingViewEvent.VisibleChanged, visible)
    }

    function updateStartPoi(poi) {
        listModel.get(0).poi = poi
    }
    function updateEndPoi(poi) {
        listModel.get(4).poi = poi
    }
    function updateVia1(poi) {
        listModel.get(1).poi = poi
    }
    function updateVia2(poi) {
        listModel.get(2).poi = poi
    }
    function updateVia3(poi) {
        listModel.get(3).poi = poi
    }
    function hide() {
        root.visible = false
    }
    function registCsws() {
        var csws = [csw]
        for (var i=0; i<listViewController.csws.length; i++) {
            csws.push(listViewController.csws[i])
        }
        sendEvent(RouteSettingViewEvent.RegistCSW, csws)
    }

    ListModel {
        id: listModel

        ListElement { title: qsTr("출발지"); poi: ""; poiType: 0; tag: 0 }
        ListElement { title: qsTr("경유지 1"); poi: ""; poiType: 1; tag:1 }
        ListElement { title: qsTr("경유지 2"); poi: ""; poiType: 1; tag:2 }
        ListElement { title: qsTr("경유지 3"); poi: ""; poiType: 1; tag:3 }
        ListElement { title: qsTr("목적지"); poi: ""; poiType: 2; tag:4 }
    }
    Rectangle { anchors.fill: listView; color: GlobalConstants.kBasicBackgoundColor }

    function selectVia(index) {
        if (0 < index && index <5) {
            listView.currentIndex = index
            listView.currentItem.visualItem.state = 'pressed'
            editViaIndex = index
        }
    }
    function updateVisualItems() {
        enableViaItems = 0
        for (var i=0; i<listModel.count; i++) {
            var item = listModel.get(i)
            if (item.poi.length > 0) {
                ++enableViaItems
            }
            item.visualIndex = i
        }
    }
    function deleteVia(index) {
        listModel.get(index).poi = ""
        updateVisualItems()
        sendEvent(RouteSettingViewEvent.PoiDeleteClicked, index)
    }
    function showViaRegistView(index, address) {
        slot = index
        var v = {'index' : slot, 'address': address}
        sendEvent(RouteSettingViewEvent.PoiClicked, v)
    }
    function startEditMode() {
        listViewController.visible = true
        isEnableDrag = false
        listView.currentItem.visualItem.state = "pressed"
        selectedItem = listView.currentItem
    }
    function finishEditMode() {
        isEnableDrag = true
        listViewController.visible = false
        if (selectedItem)
            selectedItem.visualItem.setLostFocus()

        listView.currentIndex = -1
        updateVisualItems()
    }
    function retranslateUi() {
        if (root.visible) {
            var data = {
                title: qsTr("경유지 설정"),
                menus: [qsTr("경로탐색"), "noOption"]
            }
            sendEvent(RouteSettingViewEvent.SetupTitleAndZoneMenu, data)

            listView.model = undefined
            listView.model = listModel
            if (listViewController.visible) {
                finishEditMode()
            }
        }
    }

    ListView {
        id: listView

        width: parent.width
        height: layout.itemHeight * 5

        interactive: false
        currentIndex: -1

        Csw {
            id: csw

            property bool ignoreLostFocus: false

            section: Csw.Section_RouteSetting
            sequence: 0
            enable: !listViewController.visible
            objectName: 'waypointCsw'
            onFocused: {
                // 첫번째 아이템은 출발지이기 때문에 선택이 대상이 되지 않음
                // 두번째 아이템부터 선택이 되도록 currentIndex를 1로 설정한다.
                listView.currentIndex = Math.max(1, extra)
                if (!blockFocusToMain) {
                    listView.currentItem.visualItem.setFocusToMain()
                }
            }
            onLostFocusd: {
                if (!ignoreLostFocus) {
                    if (listView.currentItem)
                        listView.currentItem.visualItem.setLostFocus()
                }
                ignoreLostFocus = false
                listView.currentIndex = -1
            }
            onSpinCcwPressed: {
                if (0 < listView.currentIndex) {
                    listView.currentItem.visualItem.setLostFocus()
                    --listView.currentIndex
                    listView.currentItem.visualItem.setFocusToMain()
                    extra = listView.currentIndex
                }
            }
            onSpinCwPressed: {
                if (4 > listView.currentIndex) {
                    if (0 < listView.currentIndex)
                        listView.currentItem.visualItem.setLostFocus()

                    ++listView.currentIndex
                    if (0 === listView.currentIndex)
                        ++listView.currentIndex

                    listView.currentItem.visualItem.setFocusToMain()
                    extra = listView.currentIndex
                }
            }
            onLeftPressed: {
                if (listView.currentItem && 0 < listView.currentIndex) {
                    listView.currentItem.visualItem.moveToLeft()
                }
            }
            onRightPressed: {
                if (listView.currentItem) {
                    listView.currentItem.visualItem.moveToRight()
                }
            }
            onSelected: {
                if (listView.currentItem) {
                    var item = listView.currentItem
                    if (item.visualItem.isMainFocused()) {
                        if (item.visualItem.isEditButtonFocused()) {
                            var index = listView.currentIndex

                            startEditMode()
                            ignoreLostFocus = true
                            csw.forceNextSection()

                            listView.currentIndex = index
                            selectedItem = listView.currentItem
                            selectedItem.visualItem.setStatePressed()
                            editViaIndex = index
                        } else {
                            showViaRegistView(listView.currentIndex, item.visualItem.address)
                        }
                    } else if (item.visualItem.isDelButtonFocused()) {
                        deleteVia(listView.currentIndex)
                        item.visualItem.setFocusToMain()
                    }
                }
            }
        }

        RouteSettingViewController {
            id: listViewController

            visible: false
            anchors.top: parent.top
            anchors.topMargin: layout.controllerTopMargin
            anchors.right: parent.right

            onOkClicked: {
                finishEditMode()
                if (okClickedByCsw) {
                    blockFocusToMain = true
                    sendEvent(RouteSettingViewEvent.SelectCSW, csw)
                    if (0 <= editViaIndex) {
                        listView.currentIndex = editViaIndex
                        listView.currentItem.visualItem.setFocusToMain()
                        listView.currentItem.visualItem.moveToLeft()
                    }
                    blockFocusToMain = false
                }
                editViaIndex = -1
            }
            onUpClicked: {
                var curIndex = listView.currentIndex

                if (1 < curIndex) {
                    listModel.move(curIndex-1, curIndex, 1)
                    sendEvent(RouteSettingViewEvent.ItemMoveUp, curIndex)
                    selectVia(curIndex-1)
                } else {
                    var item = listModel.get(1)
                    listModel.append(item)
                    listModel.remove(curIndex, 1)
                    sendEvent(RouteSettingViewEvent.ItemMoveUp, curIndex)
                    sendEvent(RouteSettingViewEvent.Sync)
                    selectVia(4)
                }
            }
            onDownClicked: {
                var curIndex = listView.currentIndex

                if (4 > curIndex) {
                    listModel.move(curIndex, curIndex+1, 1)
                    sendEvent(RouteSettingViewEvent.ItemMoveDown, curIndex)
                    selectVia(curIndex+1)
                } else {
                    var item = listModel.get(curIndex)
                    listModel.insert(1, item)
                    listModel.remove(curIndex, 1)
                    sendEvent(RouteSettingViewEvent.ItemMoveDown, curIndex)
                    sendEvent(RouteSettingViewEvent.Sync)
                    selectVia(1)
                }
            }
        }

        Component.onCompleted: {}

        delegate: DragAndDropArea {
            id: dragAndDropArea

            width: layout.draggerWidth + layout.draggerLeftMargin
            height: layout.itemHeight
            view: listView
            acceptSwap: poiType != 0

            enableDrag: isEnableDrag && visualItem.address.length > 0

            visualItem: RouteSettingViewItem {
                width: layout.itemWidth
                height: layout.itemHeight
                Drag.hotSpot.y: layout.itemHeight / 2

                // custom properties
                visualIndex: index
                delVisible: visualIndex !== 0 && address.length > 0
                delEnabled: !isEditMode && enableViaItems > 2//1
                showHorizontalLine: visualIndex < listModel.count-1
                isCurrent: (visualIndex === listView.currentIndex)
                onIsCurrentChanged: {
                    if (!isCurrent) {
                        state = ""
                    }
                }
                address: poi
                addressType: poiType

                onParentChanged: {
                    if (parent === listView) {
                        // Drag가 active되는 순간 부모가 listView로 교체된다.
                        // 이때 VisualItem의 원래 좌표가 0,0이므로 (원 부모는 delegate)
                        // 첫번째 외의 아이템을 드래그하는 순간
                        // 첫번째 아이템(0,0)과 선택한 아이템(0,0)이 충돌 처리 되어
                        // enteredDropArea(source, dest)가 호출 된다.
                        // 부모가 listView인 경우에는 VisualItem의 y를 해당 delegate위치로 맞추어준다.
                        // 18.01.29 ymhong

                        state = "pressed"
                        y = visualIndex * layout.itemHeight
                    } else {
                        state = ""
                        y = 0
                    }
                }
                onDelClicked: {
                    csw.forceLostFocus()

                    deleteVia(index)
                }

                onTitleClicked: {
                    if (!isEditMode) {
                        csw.forceLostFocus()

                        if (address.length > 0) {
                            if (listView.currentItem) {
                                listView.currentItem.visualItem.state = ""
                            }

                            listView.currentIndex = visualIndex
                            startEditMode()
                        }
                    } else {
                        if (listView.currentIndex == visualIndex) {
                            finishEditMode()
                        }
                    }
                }
                onAddressAreaClicked: {
                    csw.forceLostFocus()

                    if (isEditMode) {
                        if (listView.currentIndex == visualIndex) {
                            finishEditMode()
                        }
                    } else {
                        showViaRegistView(visualIndex, address)
                    }
                }
            }

            onEnteredDropArea: {
                var srcIndex = source.visualIndex
                var destIndex = dest.visualIndex
                if (srcIndex !== destIndex) {

                    listModel.move(srcIndex, destIndex, 1)

                    if (srcIndex < destIndex) {
                        sendEvent(RouteSettingViewEvent.ItemMoveDown, srcIndex)
                    } else {
                        sendEvent(RouteSettingViewEvent.ItemMoveUp, srcIndex)
                    }
                }
            }
        } //delegate:
    } // ListView
}
