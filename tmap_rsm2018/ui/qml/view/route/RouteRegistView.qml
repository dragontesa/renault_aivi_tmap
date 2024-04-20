import QtQuick 2.0
import RouteRegistViewEvent 1.0
import LayoutConstants 1.0
import "../../view"
import '../../view/util'
import '../../GlobalConstants.js' as GlobalConstants

PopupView {
    id: root

    property int viaIndex
    property string titleText: qsTr("경유지 설정") + Tr.d
    property alias captureEnable: gestureArea.enabled

    anchors { fill: parent; bottomMargin: GlobalConstants.kScreenHardKeyHeight }

    property var menus: [
        qsTr("통합 검색") + Tr.d,
        qsTr("최근 목적지") + Tr.d,
        qsTr("즐겨찾기") + Tr.d,
        qsTr("주변 검색") + Tr.d,
        qsTr("주소 검색") + Tr.d,
        qsTr("업종 검색") + Tr.d,
        qsTr("지도 검색") + Tr.d,
        qsTr('')
    ]

    modal: true

    function isActiveMenu(index) {
        switch (index) {
        case 0: // 통합검색
            //return false
        case 1: // 최근 목적지
        case 2: // 즐겨 찾기
        case 3: // 주변 찾기
        case 4: // 주소 찾기
        case 5: // 업종 찾기
        case 6: // 지도 찾기
            return true
        }

        return false
    }
    function goBack() {
        sendEvent(RouteRegistViewEvent.CancelClicked)
    }

    onMediated: {
        sendEvent(RouteRegistViewEvent.Mediated)
        sendEvent(RouteRegistViewEvent.RegistCSW, [gridPopup.csw, gridPopup.buttonCsw])
    }

    onActivated: {
        sendEvent(RouteRegistViewEvent.ModalChanged, {modal: true})
    }

    onDeactivated: {
        sendEvent(RouteRegistViewEvent.ModalChanged, {modal: false})
    }

    layout: RouteRegistViewLayout {
        onUpdateLayoutCompleted: {
            switch (screenResolution) {
            case LayoutConstants.L800x480:
                item.anchors.leftMargin = 128
                item.anchors.rightMargin = 128
                item.anchors.topMargin = 24
                item.anchors.bottomMargin = 24
                console.log("item.size" + item.width + "x" + item.height)
                break

            case LayoutConstants.L820x1024:
                item.anchors.leftMargin = 138
                item.anchors.rightMargin = 138
                item.anchors.topMargin = 215
                item.anchors.bottomMargin = 215
                console.log("item.size" + item.width + "x" + item.height)
                break
            }
        }
    }

    Item {
        id: item
        anchors.fill: parent

        CommonGridPopup {
            id: gridPopup
            anchors.centerIn: parent
            title: titleText
            buttonText: qsTr('취소') + Tr.d
            menus: root.menus

            onClicked: {
                var data = {'index':index}

                sendEvent(RouteRegistViewEvent.SearchItemClicked, data)
            }
            onCancelClicked: {
                goBack()
            }
        }
    }// item
    GestureArea {
        id: gestureArea
        anchors.fill: parent
        enabled: false
        onIssued: {
            sendEvent(RouteRegistViewEvent.CaptureScreen)
        }
    }
}
