import QtQuick 2.0
import ViewRegistry 1.0
import CommonConstants 1.0
import ZoneConstants 1.0
import SearchConstants 1.0
import BookmarkRegistViewEvent 1.0
import LayoutConstants 1.0

import "../../component"
import "../../view"
import "../../view/search"
import '../../view/util'
import "../../GlobalConstants.js" as GlobalConstants
import "../../component/internal/Keyboard.js" as CustomKeyboard
import "../../view/util/CommonUtils.js" as CommonUtils

PopupView {
    id: root

    modal: true
    anchors { fill: parent; bottomMargin: GlobalConstants.kScreenHardKeyHeight }

    property string caller
    property string owner
    property string titleText : {
        if (0 <= owner.indexOf('office')) {
            return qsTr('회사 등록') + Tr.d
        } else if (0 <= owner.indexOf('home')) {
            return qsTr('집 등록') + Tr.d
        } else {
            return qsTr('즐겨찾기 추가') + Tr.d
        }
    }

    property var menus: [
       qsTr("통합 검색") + Tr.d,
       qsTr("최근 목적지") + Tr.d,
       qsTr("주변 검색") + Tr.d,
       qsTr("주소 검색") + Tr.d,
       qsTr("업종 검색") + Tr.d,
       qsTr("지도 검색") + Tr.d
    ]
    property alias captureEnable: gestureArea.enabled

    function isActiveMenu(index) {
        console.log("isActiveMenu - owner = " + owner)
        var isHome = owner && owner.indexOf(SearchConstants.Home) >= 0
        var isOffice = owner && owner.indexOf(SearchConstants.Office) >= 0

        switch (index) {
        case 0: // 통합검색
        case 1: // 최근 목적지
        case 2: // 주변 검색
        case 3: // 주소 검색
        case 4: // 업종 검색
        case 5: // 지도 검색
            return true
        default:
            return false
        }
    }
    function goBack() {
        sendEvent(BookmarkRegistViewEvent.CancelClicked)
    }

    onMediated: {
        sendEvent(BookmarkRegistViewEvent.Mediated)
    }

    onActivated: {
        sendEvent(BookmarkRegistViewEvent.Activated)

        var data = ({})
        data[CommonConstants.Modal] = true
        sendEvent(BookmarkRegistViewEvent.ModalChanged, data)
    }

    onDeactivated: {
        sendEvent(BookmarkRegistViewEvent.Deactivated)

        var data = ({})
        data[CommonConstants.Modal] = false
        sendEvent(BookmarkRegistViewEvent.ModalChanged, data)
    }

    layout: BookmarkRegistViewLayout {
        onUpdateLayoutCompleted: {
            switch (screenResolution) {
            case LayoutConstants.L800x480:
                item.anchors.leftMargin = 128
                item.anchors.rightMargin = 128
                item.anchors.topMargin = 24
                item.anchors.bottomMargin = 24
                break

            case LayoutConstants.L820x1024:
                item.anchors.leftMargin = 138
                item.anchors.rightMargin = 138
                item.anchors.topMargin = 215
                item.anchors.bottomMargin = 215
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
            menus : root.menus

            onClicked: {
                sendEvent(BookmarkRegistViewEvent.SearchItemClicked, index)
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
            sendEvent(BookmarkRegistViewEvent.CaptureScreen)
        }
    }
}
