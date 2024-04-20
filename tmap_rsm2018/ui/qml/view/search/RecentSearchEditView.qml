import QtQuick 2.0
import ViewRegistry 1.0
import CommonConstants 1.0
import ZoneConstants 1.0
import SearchConstants 1.0
import RecentSearchEditViewEvent 1.0
import LayoutConstants 1.0
import Csw 1.0

import "../../singleton"
import "../../component"
import "../../view"
import "../../view/util"
import "../../view/search"
import "../../GlobalConstants.js" as GlobalConstants
import "../../view/util/CommonUtils.js" as CommonUtils

PopupView {
    id: searchRoot
    width: parent ? parent.width : undefined
    height: parent ? parent.height + GlobalConstants.kScreenZone3Height : undefined
    
    layout: QtObject {
        property int listRowHeight : 83  // 리스트 높이
        property int listMaskImgHeight : 30 // 리스트 마스크 높이
        property int checkBoxSize: 63 // 체크박스 크기

        // callback when layout updated
        function updateLayout( lay ) {
            if ( lay === LayoutConstants.L800x480 ) {
                listRowHeight = 50
                listMaskImgHeight = 30
                checkBoxSize = 30
            }
            else if ( lay === LayoutConstants.L820x1024 ) {
                listRowHeight = 83
                listMaskImgHeight = 30
                checkBoxSize = 63
            }
        }
    }

    property var caller: null
    property var result: null
    property var deleteArr: null
    property var searchPageObj: null
    property int currentCnt : 0
    property int requestMax : 20
    property bool request: false

    //Timer
    property var timer : null

    //ID
    property var listView: null
    property var message: null

        // Constants
    
    readonly property string kLabelTitle: qsTr("최근 검색 편집")+Tr.d
    readonly property string kLabelNoHistory: qsTr("최근 검색어가 없습니다.")+Tr.d
    readonly property var kSoftkeys: {
        "Back": ZoneConstants.Back, // 뒤로
        "Option": ZoneConstants.Option, // 옵션
        "Navi": ZoneConstants.Navi, // 네비
        "Menu": ZoneConstants.Menu, // 메뉴
        "Confirm": qsTr("확인"), // 확인
        "DeleteAll": qsTr("전체삭제")+Tr.d, // 전체삭제
        "Delete": qsTr("삭제")+Tr.d, // 선택삭제
    }
    readonly property var kHeader: {
        "Id": 0, // 아이디
        "Keyword": 1 // 검색어
    }

    property bool isCswFocused: false
    property real highlightOpacity: 1.0
    property color highlightColor: CommonConstants.Transparent
    property color hightligtItemColor: isCswFocused ? GlobalConstants.kFocusedColor : highlightColor


    function retranslateUi() {
        sendEvent(RecentSearchEditViewEvent.SelectZone1Title,kLabelTitle)
        displayDeleteCount();
    }

    function displayDeleteCount() {
        var selectCount = 0
        deleteArr = []
        for (var i=0; i<listModel.count; i++) {
            var obj = listModel.get(i)
            var isChecked = obj.checked
            if (isChecked){
                selectCount++
                deleteArr.push(obj.seq)
            }
        }
        displaySoftMenu(selectCount)
    }

    function displaySoftMenu(selectCount) {
        var deleteAllBtnName = kSoftkeys.DeleteAll
        var deleteBtnName = kSoftkeys.Delete
        if (selectCount)
            deleteBtnName += " (" + selectCount.toString()+ ")"

        // checkup if button disabled
        if (selectCount == 0)
           deleteBtnName +=  " (0):disabled" // fix TMCRSM-347

        sendEvent(RecentSearchEditViewEvent.EditSoftkeyMenu, {"menus":[deleteAllBtnName, deleteBtnName, "noOption"]})
    }

    function deleteSelectedItems(items)
    {
        sendEvent(RecentSearchEditViewEvent.DeleteRecentSearch, items)
        //var msg = "선택한 검색어("+items.length+")가 삭제되었습니다."
        var msg = qsTr("선택한 검색어가 삭제되었습니다.").arg(items.length)
        showToast(msg);
        listModel.clear();
        currentCnt = 0
        searchPageObj.updateListFromEditView();
//        TMap.showToast(msg, ToastK.center)
         displaySoftMenu(0) // reset softkey menu
        back(); // move back to list view
    }

    function showToast(msg)
    {
        var data = ({})
        data[CommonConstants.Message] = msg
        data[CommonConstants.Position] = CommonConstants.ToastUp
        data[CommonConstants.Duration] = 5000
        sendEvent(RecentSearchEditViewEvent.ShowToast,data)
    }

    function showDeleteAllDlg() {
        var data = ({})
        data[CommonConstants.Url    ] = CommonConstants.url(ViewRegistry.CommonDialogView)
        data[CommonConstants.Message] = "최근 검색어를 모두 삭제하시겠습니까?" // popup don't need multiple language TMCRSM-2500
        data[CommonConstants.Title  ] = "전체삭제"
        data[CommonConstants.Menus  ] = [CommonConstants.Cancel, CommonConstants.Ok]
        var arg = ({})
        arg [CommonConstants.Caller ] = objectName
        data[CommonConstants.Arg    ] = arg
        sendEvent(RecentSearchEditViewEvent.CreateView, data)

        doModal(true)
    }

    function doneDeleteAllDlg(yes) {
        doModal(false)
        if (yes){
            sendEvent(RecentSearchEditViewEvent.SelectZone1TextInput);
            sendEvent(RecentSearchEditViewEvent.ClearRecentSearch)
            listModel.clear();
            searchPageObj.updateListFromEditView();
            back();
        }
        else
            displayDeleteCount();   // fix TMCRSM-4130: softmenu of selection get reset when modal is closed

    }

    function doModal(modal) {
        var data = ({})
        data[CommonConstants.Modal] = modal
        sendEvent( RecentSearchEditViewEvent.ModalChanged, data)
    }

    function back() {
        if (searchPageObj != null &&  typeof searchPageObj.editView != "undefined")
               searchPageObj.editView = null
        close()
    }

    function __doSoftKeyFunction(button, menuName, menuIndex) {
      if (button == kSoftkeys.Back) {
          back()
      } else if (button == kSoftkeys.Option) {

      } else if (button == kSoftkeys.Navi) {

      } else if (button == kSoftkeys.Menu) {
          switch (menuName) {
          case kSoftkeys.DeleteAll:
               showDeleteAllDlg()
              break
          default:
              if (menuName.indexOf(kSoftkeys.Delete) >= 0)
              {
                  deleteSelectedItems(deleteArr)
              }
              break;
          }
      }
    }

    function pushRecentSearchList(newList)
    {
        for (var p in newList)
        {
            var dest = newList[p];
              listModel.append (
                  {
                      seq      : dest[kHeader.Id],
                      text     : dest[kHeader.Keyword],
                      checked  : 0
                  });
            currentCnt++;
        }
    }

    onMediated: {
        loader.sourceComponent = main
        // set view title, Fixed artf258330
        sendEvent(RecentSearchEditViewEvent.SelectZone1Title,kLabelTitle)
        if (searchPageObj != null)
        {
            searchPageObj.editView = searchRoot
        }
    }

    onActivated: {
         // checkup if button disabled
        var deleteAllBtnName = kSoftkeys.DeleteAll
        var deleteBtnName = kSoftkeys.Delete
         deleteBtnName +=  " (0):disabled"  // fix TMCRSM-347
        if (listModel.count == 0)
           deleteAllBtnName += ":disabled"

        sendEvent(RecentSearchEditViewEvent.Activated, {"menus":[deleteAllBtnName,deleteBtnName,"noOption"]})
        sendEvent(RecentSearchEditViewEvent.ResetCsw)
        sendEvent(RecentSearchEditViewEvent.RegistCsw, [ csw ])

        if (searchPageObj != null) {
            searchPageObj.visible = false
        }
    }

    onDeactivated: {
        sendEvent(RecentSearchEditViewEvent.RemoveCsw, csw)
    }

    Component.onDestruction: {
        timer = CommonUtils.destroy(timer)
    }

    //main
    Loader {
         id: loader
         width: parent.width
         height: parent.height
         onLoaded: {
             if (result !== null) pushRecentSearchList(result)
             message.visible = ( listModel.count == 0)
         }
    }

    Component {
        id: main
            Rectangle{
                color: GlobalConstants.kListNormalColor
                ListView {
                    id: listView
                    Component.onCompleted: searchRoot.listView = this
                    width: parent.width
                    height: parent.height - GlobalConstants.kScreenZone3Height
                    property int rowHeight: layout.listRowHeight
                    cacheBuffer: rowHeight * 2
                    clip: true
                    model: listModel
                    delegate: listDelegate
                    currentIndex: -1

                    property var pressedItem: null
                    PressBar {
                        currentItem: listView.pressedItem
                        leftMargin: GlobalConstants.kListBackgroundLeftMargin
                        rightMargin: GlobalConstants.kListBackgroundRightMargin
                    }
                    highlight: HighlightBar {
                        currentItem: listView.currentItem
                        leftMargin: GlobalConstants.kListBackgroundLeftMargin
                        rightMargin: GlobalConstants.kListBackgroundRightMargin
                        color: hightligtItemColor// highlightColor
                    }
                    highlightFollowsCurrentItem: false


                    onMovementStarted: {
                      sendEvent(RecentSearchEditViewEvent.HideZone3)
                    }
                    onMovementEnded:  {
                        sendEvent(RecentSearchEditViewEvent.ShowZone3)
                    }

                    Image {
                          id: imgMask
                          width: parent.width
                          height: layout.listMaskImgHeight
                          y: parent.y + (parent.height - height)
                          visible: scrollBar.heightRatio < 1.0 && !listView.dragging
                          sourceSize.width: width
                          sourceSize.height: height
                          source: Resource.imageUrl("zone/zone2_img_mask.png")
                    }

                    TScrollBar {
                        id:scrollBar
                        flickable: listView
                        width: 2
                        color: MultiSenseCoordinator.kAmbientColor
                        grooveColor: GlobalConstants.kScrollBarBackColor
                        rightMargin: GlobalConstants.kScrollBarRightMargin
                    }
                }

                Rectangle {
                    id: message
                    Component.onCompleted: searchRoot.message = this
                    width: parent.width
                    height: parent.height
                    visible: false
                    color: GlobalConstants.kBasicBackgoundColor
                    Text {
                        anchors.fill: parent
                        horizontalAlignment: Qt.AlignHCenter
                        verticalAlignment: Qt.AlignVCenter
                        font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(GlobalConstants.kGeneralMessageFontSize) }
                        color: GlobalConstants.kListTextColor
                        text: kLabelNoHistory
                    }
                }
            }
    }

    ListModel { id: listModel   }

    Component {
        id: listDelegate
        Column {
            id:column
            width: listView.width
            height: listView.rowHeight
            function itemSelected() {
                listView.currentIndex = model.index
                highlightColor = Qt.binding(function(){ return MultiSenseCoordinator.kAmbientColor})
                var sleep =  GlobalConstants.kPressedColorExposureTime
                timer = CommonUtils.destroy(timer)
                timer = CommonUtils.delay(searchRoot, sleep, function() {
                    try {
                        highlightColor = CommonConstants.Transparent
                        if (checkButton.enabled) {
                            checkButton.selected = checkButton.selected ? CommonConstants.False: CommonConstants.True
                        }
                        displayDeleteCount()
                    } catch(e) {
                    }
                })
            }

            Item {
                id:mouseArea
                width: parent.width
                height: parent.height - splitBar.height
                MouseArea {
                    width: parent.width
                    height: parent.height

                    PressBarMouseSync {
                        cswObj: csw
                        currentItem: listView.pressedItem
                        onCurrentItemChanged: {
                            if (CommonUtils.isEqual(currentItem, listView.pressedItem)) {
                                return
                            }
                            listView.pressedItem = currentItem
                        }
                    }

                    onClicked: {
                        itemSelected()
                    }
                }
                Row {
                    x: GlobalConstants.kListTextLeftMargin
                    width: parent.width - (x + GlobalConstants.kListButtonRightMargin)
                    height: parent.height
                    Text {
                        id: desc
                        width: parent.width -  checkButton.width
                        height: parent.height
                        horizontalAlignment: Qt.AlignLeft
                        verticalAlignment: Qt.AlignVCenter
                        elide: Text.ElideRight
                        font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(GlobalConstants.kOneLineListTextFontSize)}
                        color: (index == listView.currentIndex && isCswFocused) ? GlobalConstants.kListTextFocusColor : GlobalConstants.kListTextColor
                        text:  model.text
                    }
                    TCheckButton {
                        id: checkButton
                        width: layout.checkBoxSize
                        height: layout.checkBoxSize
                        imgCheck:  Resource.imageUrl("checkbox_on.png")
                        imgUncheck: Resource.imageUrl("checkbox_off.png")
                        anchors.verticalCenter: parent.verticalCenter
                        property int seq : model.seq
                        selected: model.checked
                        onSelectedChanged: {
                            model.checked = selected
                        }
                        onClicked: {
                            listView.currentIndex = model.index
                            displayDeleteCount()
                        }
                    }
                }
            }
            Rectangle {
               id: splitBar
               x: GlobalConstants.kListLineLeftMargin
               width: parent.width - (GlobalConstants.kListLineLeftMargin  + GlobalConstants.kListLineRightMargin)
               height: 1
               color: GlobalConstants.kListLineColor
            }
        }
    }

    Csw {
        id: csw
        section: Csw.Section_List
        sequence: 0
        function goPrev() {
            if (0 <listView.currentIndex) {
                listView.currentItem.state = "normal"
                --listView.currentIndex
                listView.currentItem.state = "focused"
            }
        }
        function goNext() {
            if (listView.currentIndex < listModel.count -1) {
                listView.currentItem.state = "normal"
                ++listView.currentIndex
                listView.currentItem.state = "focused"
            }
        }
        onFocused: {
            searchRoot.focus = true
            isCswFocused = true
            if (0 == listModel.count) {
                forceNextSection()
            } else {
                if (listView.currentIndex < 0)
                    listView.currentIndex = 0
                listView.currentItem.state = "focused"
            }
        }
        onLostFocusd: {
            searchRoot.focus = false
            isCswFocused = false
            if (listView.currentItem !== undefined)
                listView.currentItem.state = "normal"
        }
        onSpinCcwPressed: {
            goPrev()
        }
        onSpinCwPressed: {
            goNext()
        }
        onSelected: {
            if (listView.currentItem !== undefined) {
                listView.currentItem.itemSelected()
            }
        }
    }
}
