import QtQuick 2.0
import ViewRegistry 1.0
import CommonConstants 1.0
import ZoneConstants 1.0
import SearchConstants 1.0
import RecentDestinationEditViewEvent 1.0
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
                listRowHeight = 115
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
    property bool isCswFocused: false
    property real highlightOpacity: 1.0
    property color highlightColor: CommonConstants.Transparent
    property color highlightItemColor: isCswFocused ? GlobalConstants.kFocusedColor : highlightColor

    //Timer
    property var timer : null

    //ID
    property var listView:null
    property var message:null

    // Constants
    readonly property string kZone1Title: qsTr("최근 목적지 편집") + Tr.d
    readonly property string kDeleteAllMessage: "최근 목적지를 모두 삭제하시겠습니까?" // popup don't need multiple language TMCRSM-2500
    readonly property string kNoDataMessage: qsTr("최근 목적지가 없습니다") + Tr.d
    readonly property var kSoftkeys: {
        "Back": ZoneConstants.Back, // 뒤로
        "Option": ZoneConstants.Option, // 옵션
        "Navi": ZoneConstants.Navi, // 네비
        "Menu": ZoneConstants.Menu, // 메뉴
        "Confirm": qsTr("확인"), // 확인
        "DeleteAll": qsTr("전체삭제")+Tr.d, // 전체삭제
        "DeleteSelected": qsTr("삭제")+Tr.d, // 선택삭제
    }
    readonly property var kHeader: {
        "Id": 0, // 아이디
        "Name": 1, // 명칭
        "Address": 2 // 주소
    }

    function retranslateUi() {
        sendEvent(RecentDestinationEditViewEvent.SelectZone1Title, kZone1Title)
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
        var deleteBtnName = kSoftkeys.DeleteSelected
        if (selectCount)
            deleteBtnName += " (" + selectCount.toString()+ ")"


        // checkup if button disabled
        if (selectCount == 0)
           deleteBtnName +=  " (0):disabled"
//        if (listModel.count-selectCount == 0)   // fixed! bug which all deletion button become dim (artf258749)
//           deleteAllBtnName += ":disabled"

//        sendEvent(RecentDestinationEditViewEvent.EditSoftkeyMenu, {index : 0, menu: deleteAllBtnName})
        var softmenu = {}
        softmenu["menus"] = [deleteAllBtnName,deleteBtnName,"noOption"]
        sendEvent(RecentDestinationEditViewEvent.EditSoftkeyMenu, softmenu)
    }

    function deleteSelectedItems(items)
    {
        sendEvent(RecentDestinationEditViewEvent.DeleteRecentDestination, items)
        //var msg = "선택한 검색어("+items.length+")가 삭제되었습니다."
        var msg = qsTr("선택한 최근 목적지가 삭제되었습니다.").arg(items.length)

        showToast(msg);
        listModel.clear();
        currentCnt = 0
        searchPageObj.updateListFromEditView();

        displaySoftMenu(0) // reset softkey menu
        back(); // move back to list view
    }

    function showToast(msg)
    {
        var data = ({})
        data[CommonConstants.Message] = msg
        data[CommonConstants.Position] = CommonConstants.ToastUp
        data[CommonConstants.Duration] = 5000

        sendEvent(RecentDestinationEditViewEvent.ShowToast,data)
    }

    function showDeleteAllDlg() {
        var data = ({})
        data[CommonConstants.Url    ] = CommonConstants.url(ViewRegistry.CommonDialogView)
        data[CommonConstants.Title  ] = "전체삭제"
        data[CommonConstants.Message] = kDeleteAllMessage
        data[CommonConstants.Menus  ] = [CommonConstants.Cancel, CommonConstants.Ok]

        var arg = ({})
        arg [CommonConstants.Caller ] = objectName
        data[CommonConstants.Arg    ] = arg
        sendEvent(RecentDestinationEditViewEvent.CreateView, data)

        doModal(true)
    }

    function doneDeleteAllDlg(yes) {
        //dialogLoader.sourceComponent = null
        doModal(false)

        if (yes){
            var data = ({})
            sendEvent(RecentDestinationEditViewEvent.SelectZone1TextInput)
            sendEvent(RecentDestinationEditViewEvent.ClearRecentDestination)
            listModel.clear()
            searchPageObj.updateListFromEditView()
            back();
        }
        else
            displayDeleteCount();   // fix TMCRSM-3599: softmenu of selection get reset when modal is closed
    }

    function back() {
        //2017.07.04 cjlee 추가
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
              if (menuName.indexOf(kSoftkeys.DeleteSelected) >= 0)
              {
                  deleteSelectedItems(deleteArr)
              }
              break;
          }
      }
    }

    function pushRecentDestList(newList)
    {
        for (var p in newList)
        {
            var dest = newList[p];
              listModel.append (
                  {
                      seq      : dest[kHeader.Id],
                      name     : dest[kHeader.Name],
                      address  : dest[kHeader.Address],
                      checked  : 0
                  });
            currentCnt++;
        }

        listView.model = listModel
        if (message != null)
            message.visible = (listModel.count == 0)
    }

    function resetRecentDestList() {
        listModel.clear()
        currentCnt = 0
        listView.model = null
    }

    function doModal(modal) {
        var data = ({})
        data[CommonConstants.Modal] = modal
        sendEvent( RecentDestinationEditViewEvent.ModalChanged, data)
    }

    onMediated: {
        loader.sourceComponent = main
        // set view title, Fixed artf258330
        sendEvent(RecentDestinationEditViewEvent.SelectZone1Title, kZone1Title)
        if (searchPageObj != null) {
            searchPageObj.editView = searchRoot
        }
    }

    onActivated: {
        // checkup if button disabled
       var deleteAllBtnName = kSoftkeys.DeleteAll
       var deleteBtnName = kSoftkeys.DeleteSelected
        deleteBtnName += " (0):disabled" //  fix TMCRSM-347
       if (listModel.count == 0) {
          deleteAllBtnName += ":disabled"
       }

       var softmenu = {}
       softmenu["menus"] = [deleteAllBtnName,deleteBtnName,"noOption"];

       sendEvent(RecentDestinationEditViewEvent.Activated, softmenu)

       sendEvent(RecentDestinationEditViewEvent.ResetCsw)
       sendEvent(RecentDestinationEditViewEvent.RegistCsw, [ csw ])
    }

    onDeactivated: {
        sendEvent(RecentDestinationEditViewEvent.RemoveCsw, csw)
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
            if (result !== null)
              pushRecentDestList(result)
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
//                    highlight: highlightBar
//                    highlightFollowsCurrentItem: false

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
                        color: highlightItemColor
                    }
                    highlightFollowsCurrentItem: false


                    onMovementStarted:  {
                        sendEvent(RecentDestinationEditViewEvent.HideZone3)
                    }
                    onMovementEnded:  {
                        sendEvent(RecentDestinationEditViewEvent.ShowZone3)
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
                        id: scrollBar
                        flickable: listView
                        width: 2
                        color: MultiSenseCoordinator.kAmbientColor
                        grooveColor: GlobalConstants.kScrollBarBackColor
                        rightMargin: GlobalConstants.kScrollBarRightMargin
                    }

                    onAtYEndChanged: {
                        if ( contentHeight > 0
                            && atYEnd 
                            && contentY > 0
                            && contentY+height >= contentHeight) {
                            // cancelFlick()
                            // next list up
                            if (searchPageObj !== null && searchPageObj !== undefined)
                                 searchPageObj.requestViewData()
                        }
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
                        text:kNoDataMessage
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
                if (checkButton.enabled) {
                    checkButton.selected = checkButton.selected ? CommonConstants.False: CommonConstants.True
                }
                listView.currentIndex = model.index
                highlightColor = Qt.binding(function(){ return MultiSenseCoordinator.kAmbientColor})
                var sleep =  GlobalConstants.kPressedColorExposureTime
                timer = CommonUtils.destroy(timer)
                timer = CommonUtils.delay(searchRoot, sleep, function() {
                    try {
                        highlightColor = CommonConstants.Transparent
                        displayDeleteCount()
                    } catch(e) {
                    }
                })
            }

            Item {
                id: mouseArea
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
                    Column {
                        y: 21 // (115 - 32 - 28 - 12)/2
                        width: parent.width - layout.checkBoxSize
                        height: parent.height
                        spacing: 12
                        Text {
                            width: parent.width
                            height: GlobalConstants.kListTextHeight
                            horizontalAlignment: Qt.AlignLeft
                            verticalAlignment: Qt.AlignVCenter
                            elide: Text.ElideRight
                            font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(GlobalConstants.kListTextHeight)}
                            color: (index == listView.currentIndex && isCswFocused) ? GlobalConstants.kListTextFocusColor : GlobalConstants.kListTextColor
                            text:  (model.name != "")?model.name:model.address
                        }
                        Text {
                            width: parent.width
                            height: GlobalConstants.kListSubTextHeight
                            horizontalAlignment: Qt.AlignLeft
                            verticalAlignment: Qt.AlignVCenter
                            elide: Text.ElideRight
                            font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(GlobalConstants.kListSubTextHeight)}
                            color: (index == listView.currentIndex && isCswFocused) ? GlobalConstants.kListTextFocusColor : GlobalConstants.kListSubTextColor
                            text: model.address
                        }
                    }
                    TCheckButton {
                        id: checkButton
                        width: layout.checkBoxSize
                        height: layout.checkBoxSize
                        img.width: width
                        img.height: height
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
