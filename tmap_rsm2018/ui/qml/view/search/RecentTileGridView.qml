import QtQuick 2.0
import Constants 1.0
import ViewRegistry 1.0
import CommonConstants 1.0
import ZoneConstants 1.0
import SearchConstants 1.0
import LayoutConstants 1.0

import "../../singleton"
import "../../component"
import "../../view"
import "../../view/search"
import "../../GlobalConstants.js" as GlobalConstants
import "../../view/util/Formatter.js" as Formatter
import RecentTileGridViewEvent 1.0
import Csw 1.0

PopupView {
    id: root
    focusName: "RecentTileGridView"
    focus: true
    visible: true
    layer.enabled: true // enable layered opacity
    opacity:  layout.backgroundOpacity

    readonly property int kColumnCnt : 3
    readonly property int kRowCnt    : 3

    property bool cswFocused: false
    property int currentCnt : 0
    property int currentIndex: -1
    property int requestMax : 20
    property Item paddingGrids
    property Timer idleTrigger: idleElapsedTimer // time elapsed while user no input

    layout: RecentTileGridViewLayout {}

    width: parent ? parent.width : layout.viewWidth
    height: layout.viewHeight

    //chunjae.lee 2018.03.21 - "목록이 없습니다."를 아래의 문구로 수정 UID v1.2 83p
    readonly property string kNoDataMessage: qsTr("길 안내를 이용하시면, 자주가는 목적지\n및 최근목적지가 자동으로 표시됩니다.      ") + Tr.d

    readonly property var kHeader: {
        "Id": 0, // 아이디
                "Name":1, // 명칭
                "Address": 2, // 주소
                "Type":3, // 타입
                "MapX":4, // 좌표x
                "MapY": 5, // 좌표 y
                "Hit": 6, // 빈도
                "ArrivalTime":7, // 소요시간
                "PoiId":8,
    }

    property var top3Ids: []

    signal contentLoadCompleted()

    function changeRecentTileGridFontSize(size)
    {
        layout.gridFontSize = size
    }

    // called from mediator
    function resetViewData() {
        listModel.clear()
        listLoader.item.model = null
        currentCnt = 0
        currentIndex = -1
    }


    function pushRecentDestList(newList,cnt)
    {
        for (var p in newList)
        {
            var dest = newList[p];
            if ( listModel.count > currentCnt )
            {
                listModel.set(
                            currentCnt,
                            {
                                seq      : dest[kHeader.Id],
                                name     : dest[kHeader.Name],
                                address  : dest[kHeader.Address],
                                destType : dest[kHeader.Type],
                                mapX: dest[kHeader.MapX],
                                mapY: dest[kHeader.MapY],
                                arrivalTime: parseInt(dest[kHeader.ArrivalTime]),
                                poiId : dest[kHeader.PoiId]
                            });
            }
            else
            {
                listModel.append (
                            {
                                seq      : dest[kHeader.Id],
                                name     : dest[kHeader.Name],
                                address  : dest[kHeader.Address],
                                destType : dest[kHeader.Type],
                                mapX: dest[kHeader.MapX],
                                mapY: dest[kHeader.MapY],
                                arrivalTime: parseInt(dest[kHeader.ArrivalTime]),
                                poiId : dest[kHeader.PoiId]
                            });
            }

            // keep top3 ids
            if (currentCnt < 3) top3Ids[currentCnt] = dest[kHeader.Id];
            currentCnt++;
        }

        if (listModel.count == 0 || cnt > 0) {
            // padding empty item
            var unitSize = kColumnCnt*kRowCnt;
            var itemCnt = listModel.count;
            var paddingCnt   =  (itemCnt < unitSize ) ? (unitSize-itemCnt) : (itemCnt % kColumnCnt > 0) ? kColumnCnt-itemCnt%kColumnCnt: 0

            for (var i=0;i<paddingCnt;i++)
            {
                listModel.append(
                            {
                                seq  : 0,
                                name :  "",
                                address : "",
                                destType : 0,
                                mapX:-1,
                                mapY:-1,
                                arrivalTime:  0,
                                poiId: 0
                            });
            }
            listLoader.updateListComp()
        }
        if (currentCnt == 0) {
            message.visible = true
            listLoader.visible = false
        }
        scrollbar.visible = currentCnt > kColumnCnt*kRowCnt

        if (cnt > 0) {
            // request route time for top3
            requestTop3RouteTime();
        }
    }

    function requestViewData()
    {
        var data = ({})
        data[CommonConstants.Max] = requestMax
        data[CommonConstants.Offset] = currentCnt
        data[CommonConstants.Type] = (currentCnt == 0) ? 2 : 0   // sort type 0: regist desc 1: hit desc 2: hybrid
        data[CommonConstants.Owner] =  CommonConstants.objectName(ViewRegistry.RecentTileGridView)
        sendEvent( RecentTileGridViewEvent.RequestRecentDestination, data)
    }

    // retrieve route simple info, summary: distance, spentTime,
    function updateRouteSimpleInfo(tpX,tpY,summary)
    {
        for (var i=0; i<top3Ids.length; i++) {
            var item = listModel.get(i)
            var id   = summary[0];
            var distance = (summary.length > 0)?summary[1] : 0;
            var arrivalT = (summary.length > 1)?summary[2] : 0;
            if (item.seq === id && item.mapX === tpX && item.mapY === tpY) {
                listModel.setProperty(i,"arrivalTime",arrivalT);
                //                 item.arrivalTime = summary[1]
                break
            }
        }
        listLoader.updateListComp()
    }

    // 경로 요약정보 요청
    function requestTop3RouteTime()
    {
        for (var i=0;i<top3Ids.length;i++) {
            var reqId = listModel.get(i).seq;
            var mapX = listModel.get(i).mapX
            var mapY = listModel.get(i).mapY
            var data = {}
            data[SearchConstants.Id] = reqId
            data[CommonConstants.X] = mapX
            data[CommonConstants.Y] = mapY
            sendEvent( RecentTileGridViewEvent.RequestRouteSimpleInfo, data );
        }
    }

    // 목적지 선택
    function requestRoute(seq, poiId, mapX, mapY, address)
    {
        var data = ({})
        data[SearchConstants.Id] = seq
        data[SearchConstants.PoiID] = poiId
        data[CommonConstants.X] = mapX
        data[CommonConstants.Y] = mapY
        data[CommonConstants.Address] = address
        sendEvent( RecentTileGridViewEvent.RequestRoute, data);
    }

    function stopTimer() {
        idleTrigger.stop()
    }
    function startTimer() {
        idleTrigger.restart()
    }

    onMediated: {
        sendEvent(RecentTileGridViewEvent.Mediated)

        requestViewData()

        idleTrigger.stop()
        idleTrigger.start()
    }

    onActivated: {
        forceActiveFocus()
        sendEvent(RecentTileGridViewEvent.Activated, [csw])
    }
    onDeactivated: {
        sendEvent(RecentTileGridViewEvent.Deactivated, {csw:csw})
    }

    onActiveFocusChanged: {
        var data = ({})
        data["activeFocus"] = activeFocus
        sendEvent( RecentTileGridViewEvent.ActiveFocusChanged, data )

        if ( !activeFocus ) {
           close()
        }
    }

    onClosed: {
        idleTrigger.stop()
    }

    Rectangle {
        width: parent.width
        height: parent.height - layout.shadowBorderHeight
        anchors.top: parent.top
        anchors.topMargin:0
        color: GlobalConstants.kNormalColor
    }

    ListModel { id: listModel }

    Loader {
        id: listLoader
        width: layout.viewWidth // viewWidth -  scrollbar width
        height: layout.viewHeight - layout.shadowBorderHeight // viewHeight - mask image height
        property ListModel model
        function updateListComp() {
            model = null
            model = listModel
            sourceComponent = null
            sourceComponent = listComp
        }
        onLoaded: {
            item.model = null
            item.model = model
            item.vscrollbar = scrollbar
            item.update()
            // scrollbar.height = item.height
            scrollbar.flickable = item
            csw.grid = item
        }
    }

    Item {
        id: scrollbar
        property Flickable flickable
        property real ratio
        property real contentHeight
        property real contentY
        property alias drag: clicker.drag
        signal scroll(int value, int handleSize)
        width: GlobalConstants.kScrollBarWidth
        height: parent.height - layout.shadowBorderHeight - 20 //  viewHeight - mask image height - top/bottom margin
        anchors { top: parent.top; right:parent.right; rightMargin: GlobalConstants.kScrollBarRightMargin;leftMargin: GlobalConstants.kScrollBarRightMargin;topMargin:10;bottomMargin:10 }

        onFlickableChanged: {
            ratio = flickable.visibleArea.heightRatio
            contentHeight = flickable.contentHeight
            contentY = flickable.contentY
//            handle.y = scrollbar.contentY / (scrollbar.contentHeight - scrollbar.height) * clicker.drag.maximumY
        }

        onContentYChanged: {
            // by scrollbar instead of swipe
            if (clicker.drag.active || clicker.pressed)
                flickable.contentY = contentY
        }

        Rectangle {
            id:background
            anchors.fill: parent
            color: Qt.darker(GlobalConstants.kScrollBarBackColor)
        }
        Item {
            id: handle
            anchors.left: parent.left
            anchors.right: parent.right
            height: Math.max(scrollbar.ratio * scrollbar.height, 16)
            onYChanged: {
                scrollbar.scroll(y, height)
                if (clicker.drag.active)
                    scrollbar.contentY = this.y / clicker.drag.maximumY * (scrollbar.contentHeight - scrollbar.height)
            }
            Rectangle {
                id: bar
                anchors.fill: parent
                width:parent.width
                height:scrollbar.ratio * (scrollbar.height-2)
                color:MultiSenseCoordinator.kAmbientColor
            }
        }

        MouseArea {
            id: clicker
            anchors.fill: parent

            drag {
                target: handle
                minimumY: 0
                maximumY: scrollbar.height - handle.height
                axis: Drag.YAxis
            }
            onClicked: {
                scrollbar.flickable.contentY = mouse.y / scrollbar.height * (scrollbar.contentHeight - scrollbar.height)
            }
        }

        Binding {
            target: handle
            property: "y"
            value: scrollbar.contentY / (scrollbar.contentHeight - scrollbar.height) * clicker.drag.maximumY
            when: !clicker.drag.active
        }
    }

    Image {
        width: parent.width
        height: layout.shadowBorderHeight
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 0
        source: Resource.imageUrl("destination_img_shadow.png")
    }

    Component {
        id: listComp
        Flickable {
            id: gridView
            clip:true
            width: layout.viewWidth - 20 // viewWidth -  scrollbar width
            height: layout.viewHeight - layout.shadowBorderHeight // viewHeight - mask image height
            property ListModel model
            property var vscrollbar
            property int lastContentHeight:0
            function update() {
                width = layout.viewWidth - 20
                height = layout.viewHeight - layout.shadowBorderHeight
                var rowCnt = parseInt((model.count-1)/3) + 1
                list.model = rowCnt
                contentHeight = (rowCnt > 0) * layout.top3Height + (rowCnt > 0) * layout.gridCellHeight*(rowCnt-1)
                // get continue at earlier scrolling position
                contentY = scrollbar.contentY
                var d0 = Math.floor(root.currentIndex/3) - 2
                if (d0 > 0)
                    contentY = d0*layout.gridCellHeight

            }

            function itemClicked(index) {
                if (index < 0 || index >= gridView.model.count) return;
                var item = gridView.model.get(index)
                var isAddressEmpty = (item.address.length === 0 && item.name.length === 0)
                if (!isAddressEmpty && item.mapX >= 0 && item.mapY >= 0) {
                    var addressText
                    if (0 < item.name.length) {
                        addressText = item.name
                    } else if (0 < item.address.length) {
                        addressText = item.address
                    }

                    requestRoute(index, item.poiId, item.mapX, item.mapY, addressText);
                }
            }

            Column {
                Repeater {
                    id: list
                    delegate: Row {
                        id: listRow
                        height: (index == 0) ? layout.top3Height:layout.gridCellHeight
                        property int rowIndex: index
                        Repeater {
                            id: rowItems
                            model: 3
                            delegate:  RecentTileGrid {
                                property int itemIndex: rowIndex*3+index
                                property var item: gridView.model.get(itemIndex)

                                width: layout.gridCellWidth
                                height: listRow.height
                                opacity: layout.backgroundOpacity
                                color: GlobalConstants.kNormalColor // Qt.rgba(238/255.0, 236/255.0, 225/255.0, 1.0)
                                borderColor: "#000000"
                                type: {
                                        if (item !== undefined && item.destType !== undefined)
                                           return item.destType
                                    return 0
                                }
                                nameText: {
                                    if (item !== undefined) {
                                        if (item.name !== undefined && item.name.length > 0)
                                            return item.name
                                        else if (item.address !== undefined)
                                            return item.address
                                    }
                                    return ""
                                }
                                arrivalTimeText: {
                                    if (listRow.rowIndex == 0 && index < currentCnt && item !== undefined && item.arrivalTime !== undefined)
                                        return Formatter.getTimeText(item.arrivalTime);
                                    return ""
                                }
                                rank: itemIndex
                                fontSize: layout.gridFontSize
                                isCurrent: (cswFocused === true && root.currentIndex === itemIndex)
                                onClicked: {
                                    gridView.itemClicked(itemIndex)
                                }
                                onPressed: {
                                    csw.forceLostFocus()
                                }
                            }
                        }
                    }
                }
            }

            onContentYChanged: {
                idleElapsedTimer.restart()
            }

            onAtYEndChanged: {
                if (contentHeight != lastContentHeight
                        && atYEnd
                        && contentHeight> 0
                        && contentY+height >= contentHeight
                        ) {
                    // next list up
                    lastContentHeight = contentHeight
                    cancelFlick()
                    requestViewData()
                }
            }

            // scrolling by swipe
            Binding {
                target: gridView.vscrollbar
                property: "contentY"
                value: gridView.contentY
                when: (gridView.movingVertically && !gridView.vscrollbar.drag.active)
            }
        }
    }

    Csw {
        id: csw
        section: Csw.Section_GridMenu
        sequence: 0
        enable: 0 < currentCnt
        property var grid
        function goPrev() {
            if (currentCnt == 0) return;

            var d0 = Math.floor(currentIndex/3) - 2
            if (0 < currentIndex) {
                --currentIndex
            }
            else {
                currentIndex = listModel.count - 1;  // support looping, go to first TMCRSM-3166
            }
//            else return 0;

            var d1 = Math.floor(currentIndex/3) - 2

            if (d0 != d1) {
                if (d1 > 0) {
                    grid.contentY = d1*layout.gridCellHeight
                    scrollbar.contentY = grid.contentY
                }
                else {
                    if (grid.contentY != 0) {
                        grid.contentY = 0
                        scrollbar.contentY = 0
                    }
                }
            }

        }

        function goNext() {
            if (currentCnt == 0) return;
            var d0 = Math.floor(currentIndex/3) - 2
            if (currentIndex < listModel.count -1) {
                ++currentIndex
            } else {
                currentIndex = 0; // support looping, go to first TMCRSM-3166
            }
//            else return;

            var d1 = Math.floor(currentIndex/3) - 2

            // scrolling
            if (d0 != d1) {
                if (d1 > 0) {
                    grid.contentY = d1*layout.gridCellHeight
                    scrollbar.contentY = grid.contentY
                }
                else {
                    if (grid.contentY != 0) {
                        grid.contentY = 0
                        scrollbar.contentY = 0
                    }
                }
            }
        }

        onFocused: {
            cswFocused = true
            if (currentIndex < 0 || currentIndex >= listModel.count) {
                currentIndex = 0
            }
            if (0 == listModel.count)
                forceNextSection()

            idleTrigger.restart()
        }
        onLostFocusd: {
            cswFocused = false
        }
        onSpinCcwPressed: {
            goPrev()
            idleTrigger.restart()
        }
        onSpinCwPressed: {
            goNext()
            idleTrigger.restart()
        }
        onSelected: {
            grid.itemClicked(currentIndex)
        }
    }

    Text {
        id: message
        width: parent.width
        height: parent.height
        visible: false
        horizontalAlignment: Qt.AlignHCenter
        verticalAlignment: Qt.AlignVCenter
        font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(GlobalConstants.kGeneralMessageFontSize) }
        color: GlobalConstants.kListTextColor
        text: kNoDataMessage
    }

    Timer {
        id: idleElapsedTimer
        interval: 10000 // 10 secs
        onTriggered: {
            root.close()
        }
        Component.onCompleted: {
            stop()
        }
    }
}
