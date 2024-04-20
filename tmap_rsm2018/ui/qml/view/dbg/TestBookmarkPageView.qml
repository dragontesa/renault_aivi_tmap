import QtQuick 2.1
import TestBookmarkPageViewEvent 1.0
import ViewRegistry 1.0
import CommonConstants 1.0
import ZoneConstants 1.0
import SearchConstants 1.0
//import BookmarkPageViewEvent 1.0
import SearchTabViewEvent 1.0
import SearchResultDetailViewEvent 1.0
import SearchResultViewEvent 1.0
import Csw 1.0

import "../../component"
import "../../view"
import "../../view/search"
import "../../GlobalConstants.js" as GlobalConstants
import "../../component/internal/Keyboard.js" as CustomKeyboard
import "../../view/util/CommonUtils.js" as CommonUtils

PopupView {
    id: searchRoot
    width: parent ? parent.width : undefined
    height: parent ? parent.height : undefined
    objectName: 'BookmarkPageView'

    property var caller: null // 즐겨찾기로 경유지 추가시에만 사용.
    property var favorite: null
    property var tDialog: null
    property int  index: 0
    property int currentCnt : 0
    property int requestMax : kMaxFavorite
    property bool editMode: false
    property string destAddress
    property bool blockEvent: false
    property bool isViaSetting: false
    property int rowHeight: editMode ? 115 : 84

    property bool focused: false
    property color highlightColor: GlobalConstants.kListPressColor

    readonly property int kMaxFavorite: 250
    property bool isCompleted: false

    property var textEnum: (function(){
        textEnum = CommonUtils.createEnum(textEnum)
        CommonUtils.appendEnum(textEnum, "Detail",      [CommonConstants.Name], [qsTr("상세")])
        CommonUtils.appendEnum(textEnum, "Edit",        [CommonConstants.Name], [qsTr("명칭변경")])
        CommonUtils.appendEnum(textEnum, "Home",        [CommonConstants.Name], [qsTr("집으로")])
        CommonUtils.appendEnum(textEnum, "Office",      [CommonConstants.Name], [qsTr("회사로")])
        CommonUtils.appendEnum(textEnum, "Registration",[CommonConstants.Name], [qsTr("등록")])
        CommonUtils.appendEnum(textEnum, "Change",      [CommonConstants.Name], [qsTr("변경")])
        CommonUtils.appendEnum(textEnum, "Register",    [CommonConstants.Name], [qsTr("등록하세요")])
        CommonUtils.appendEnum(textEnum, "NoData",      [CommonConstants.Name], [qsTr("즐겨찾기 리스트가 없습니다.")])
        CommonUtils.appendEnum(textEnum, "DeleteAll", [CommonConstants.Name], [qsTr("전체삭제")])
        CommonUtils.appendEnum(textEnum, "WantToDeleteAll", [CommonConstants.Name], [qsTr("즐겨찾기 리스트를\n모두 삭제하시겠습니까?")])
        CommonUtils.appendEnum(textEnum, "ChangeName", [CommonConstants.Name], [qsTr("명칭변경")])
    }())
    property var textArr: (function() {
        textArr = []
        CommonUtils.addArr(textEnum, CommonConstants.Name, textArr)
    }())


    property var softKeyEnum : (function() {
        softKeyEnum = CommonUtils.createEnum(softKeyEnum)
        CommonUtils.appendEnum(softKeyEnum,"Back"                 ,[CommonConstants.Name], [ZoneConstants.Back] )
        CommonUtils.appendEnum(softKeyEnum,"Option"               ,[CommonConstants.Name], [ZoneConstants.Option])
        CommonUtils.appendEnum(softKeyEnum,"Navi"                 ,[CommonConstants.Name], [ZoneConstants.Navi])
        CommonUtils.appendEnum(softKeyEnum,"Menu"                 ,[CommonConstants.Name], [ZoneConstants.Menu])
        CommonUtils.appendEnum(softKeyEnum,"Confirm"              ,[CommonConstants.Name], [ZoneConstants.Confirm])
        CommonUtils.appendEnum(softKeyEnum,"AddFavorite"          ,[CommonConstants.Name,   CommonConstants.Url], [ZoneConstants.AddToFavourites, CommonConstants.url(ViewRegistry.BookmarkRegistView)] )
        CommonUtils.appendEnum(softKeyEnum,"EditFavorite"         ,[CommonConstants.Name], [ZoneConstants.EditFavorite])
        CommonUtils.appendEnum(softKeyEnum,"DeleteAll"            ,[CommonConstants.Name], [ZoneConstants.DeleteAll])
        CommonUtils.appendEnum(softKeyEnum,"DeleteSelected"       ,[CommonConstants.Name], [ZoneConstants.DeleteSelected + "(0)"])
    }())
    property var softKeyMenu: (function() {
        softKeyMenu = []
        CommonUtils.addArr(softKeyEnum, CommonConstants.Name, softKeyMenu)
    }())
    property var softKeyQml: (function() {
        softKeyQml = []
        CommonUtils.addArr(softKeyEnum, CommonConstants.Url, softKeyQml)
    }())

    function requestViewData()
    {
        var data = ({})
        data[CommonConstants.Max] = requestMax
        data[CommonConstants.Offset] = currentCnt
        sendEvent( TestBookmarkPageViewEvent.RequestFavorite, data)
    }

    function resetFavoriteList()
    {
        listModel.clear()
        listView.model = null
    }

    function pushFavoriteList(newList, cnt)
    {
        listModel.clear()

        var home = false
        var office = false

        for (var i=0; i<cnt; i++)
        {
            var dest = newList[i]
            var type = dest[SearchConstants.DestType]
            if (type === 1)
                office = true
            if (type === 2)
                home = true

            var text = dest[CommonConstants.Alias].length > 0 ? dest[CommonConstants.Alias] : dest[CommonConstants.Name]
            if (text.length === 0)
               text = dest[CommonConstants.Address]

            listModel.append (
            {
                type: type,
                seq: dest[SearchConstants.Id],
                text: text,
                subText: dest[CommonConstants.Address],
                x: dest[CommonConstants.X],
                y: dest[CommonConstants.Y],
                selected: 0
            });
            currentCnt++;
        }

        if (!home) {
            var homeData = { type: 2, seq: -1, text: textArr[textEnum.Home], x:0, y:0, selected:0 }
            listModel.insert(0, homeData)
        }

        if (!office) {
            var officeData = { type: 1, seq: -1, text: textArr[textEnum.Office], x:0, y:0, selected:0 }
            listModel.insert(1, officeData)
        }

        listView.model = listModel
        message.visible = listCount() === 0
    }

    function activatedPage() {
        console.log( "#1" )

        // 17.11.8 hskim
        sendEvent(TestBookmarkPageViewEvent.SelectZone1TextInput)
        hideKeyboard()

        sendEvent(TestBookmarkPageViewEvent.ResetCsw)

        sendEvent(TestBookmarkPageViewEvent.SelectSoftkey, [  softKeyMenu[softKeyEnum.AddFavorite],
                                        softKeyMenu[softKeyEnum.EditFavorite]])

        var args = [ csw ]
        sendEvent(TestBookmarkPageViewEvent.RegistCsw, args)
        searchRoot.editMode = false
    }

    function deleteSelected() {
        var list = []
        for (var i=listModel.count-1; i>=0; --i) {
            var item = listModel.get(i)
            if (item.selected === 1) {
                list.push({'type':item.type, 'seq':item.seq})

                if (item.type === 0) {
                        console.log("delete selected("+i+") : " + item.seq)
                        listModel.remove(i, 1)
                } else {
                    item.seq = -1
                    item.x = 0
                    item.y = 0
                    item.selected = 0
                }
            }
        }
        sendEvent(TestBookmarkPageViewEvent.DeleteSelectedFavoriteItems, list)

        message.visible = listCount() === 0
        updateListView()
    }

    function deleteAll() {
        var length = listModel.count
        listModel.remove(2, length-2)
        for (var i=0; i<listModel.count; i++) {
            var item = listModel.get(i)
            item.seq = -1
            item.x = 0
            item.y = 0
            item.selected = 0
        }

        message.visible = true
        updateListView()
    }

    // 모델의 아이템값을 변경 하여도 실제 뷰의 delegate에 반영이 안됨
    // 예를 들어 selected에 0을 넣어 체크박스를 해제하려고 해도 안되는 문제
    // listView의 모델을 일시적으로 재 설정하면 반영이 된다.
    function updateListView() {
        listView.model = 0
        listView.model = listModel
        listView.currentIndex = -1
    }

    function editFavoriteAlias(args) {
        var name = args[CommonConstants.Name]
        var id = args[SearchConstants.Id]
        for (var i=listModel.count-1; i>=0; --i) {
            var item = listModel.get(i)
            if (item.seq === id) {
                //item.subText = name
                item.text = name
            }
        }
    }

    function uncheckAll() {
        // item.selected 변경 시 noti가 발생하여 이전에 설정한 소프트키가 다시 바뀌는 문제를 막기 위한 코드
        searchRoot.blockEvent = true

        for (var i=0; i<listModel.count; i++) {
            var item = listModel.get(i)
            item.selected = 0
        }
        updateListView()

        searchRoot.blockEvent = false
    }

    property string dialogText
    property string aliasText
    property int aliasId: -1

    function showDeleteAllFavoriteDialog() {
        dialogLoader.sourceComponent = dialogComponent
    }

    function showEditFavoriteAliasDialog(args) {
        console.log( "#1 name: " + args['name'] )
        searchRoot.dialogText = args['name']
        searchRoot.aliasId = args['id']
        dialogLoader.sourceComponent = editFavoriteAliasDialog
    }

    function favoriteListCount() {
        var ret = searchRoot.listCount()
        //console.log("favorite list count : " + ret)
        return ret
    }

    function recoverMenuFromEdit() {
        activatedPage()
        uncheckAll()
        dialogLoader.sourceComponent = undefined
        sendEvent(TestBookmarkPageViewEvent.SelectZone1TextInput) /*! Fixed artf258330, recover view title by Leechokang 2017.6.5 */
    }

    function showView( args )
    {
        sendEvent(TestBookmarkPageViewEvent.CreateView, args)
    }

    function showAddFavoriteView(type) {
        var ownerType
        switch (type) {
        case 0: ownerType = CommonConstants.objectName(ViewRegistry.BookmarkPageView)
            break
        case 1: ownerType = CommonConstants.objectName(ViewRegistry.BookmarkPageView) + "/" + SearchConstants.Office
            break
        case 2: ownerType = CommonConstants.objectName(ViewRegistry.BookmarkPageView) + "/" + SearchConstants.Home
            break
        }

        var data = ({})
        data[CommonConstants.Url] = softKeyQml[softKeyEnum.AddFavorite]
        data[CommonConstants.Caller] = CommonConstants.objectName(ViewRegistry.SearchTabView)
        data[CommonConstants.Owner] = ownerType
        showView(data )
    }

    function hideKeyboard(){
        var data = ({})
        data[CommonConstants.Text] = ""
        sendEvent(TestBookmarkPageViewEvent.HideKeyboard, data)
    }


    function closeView() {
        isCompleted = false
        sendEvent(TestBookmarkPageViewEvent.BackwardClicked, caller)
        hideKeyboard()

        // 17.11.7 hskim commented the code below not to recreate search tab view
        //sendEvent(TestBookmarkPageViewEvent.SelectZone1TextInput)
        sendEvent(TestBookmarkPageViewEvent.ActivatedForNavi)
        var data = ({})
        data[CommonConstants.ObjectName] = CommonConstants.objectName(ViewRegistry.SearchTabView)
        sendEvent(TestBookmarkPageViewEvent.CloseView, data)
    }

    function __processKeyboard(key, text) {
        switch(key) {
        case CustomKeyboard.CodeCancel:
            hideKeyboard()
            dialogLoader.sourceComponent = undefined
            sendEvent(TestBookmarkPageViewEvent.EditFavoriteDone)
            break
        case CustomKeyboard.CodeSymbol:
        case CustomKeyboard.CodeKorean:
        case CustomKeyboard.CodeEnglish:
            break
        case CustomKeyboard.CodeShift:      break
        case CustomKeyboard.CodeClear:      break
        case CustomKeyboard.CodeSetting:    break
        case CustomKeyboard.CodeSearch:     break
        case CustomKeyboard.CodeConfirm:
            break
        case CustomKeyboard.CodeSpace:
        case CustomKeyboard.CodeDelete:
        default:
            dialogText = text
            break
        }
    }

    function __doSoftKeyFunction(button, menuName) {
      if (button === softKeyMenu[softKeyEnum.Back]) {
          if (isViaSetting) {
              // 경유지 추가 모드
              // 17.11.8 hskim
              //close()
              visible = false
          } else {
              // 통합 검색창 내
            closeView()
          }
      } else if (button === softKeyMenu[softKeyEnum.Option]) {
            optionMenu.toggleVisible()
      } else if (button === softKeyMenu[softKeyEnum.Navi]) {

      } else if (button === softKeyMenu[softKeyEnum.Menu]) {
          var key = softKeyMenu[softKeyEnum.DeleteAll]
          switch (menuName) {
            case softKeyMenu[softKeyEnum.AddFavorite]: {
                showAddFavoriteView(0)
                optionMenu.visible = false
              }
              break;

            case softKeyMenu[softKeyEnum.EditFavorite]:
                if (listModel.count > 0) {
                    searchRoot.editMode = true
                    var checked = 0
                    for (var i=0; i<listModel.count; i++) {
                        if (listModel.get(i).selected)
                            ++checked
                    }

                    var data = ({})
                    data[SearchConstants.Checked] = checked
                    sendEvent(TestBookmarkPageViewEvent.EditFavoriteClicked, data)
                    sendEvent(TestBookmarkPageViewEvent.SelectZone1Title, softKeyMenu[softKeyEnum.EditFavorite]) // Fixed artf258330, set view title by Leechokang 2017.6.5
                }
                optionMenu.visible = false
              break

            case softKeyMenu[softKeyEnum.DeleteAll]:
                showDeleteAllFavoriteDialog()
                break;

            default:
                var n = menuName.indexOf(ZoneConstants.DeleteSelected + "(")
                if (n >= 0) {
                    deleteSelected()
                }
                break;
          }
      }
    }

    function listCount() {
        var count = listModel.count - 2
        for (var i=0; i<2; i++) {
            var item = listModel.get(i)
            if (item.x > 0 && item.y > 0)
                ++count
        }
        return count
    }

    function selectItemTwice() {
        var item = listView.currentItem
        if (0 < item.pos.x && 0 < item.pos.y) {
            var data = ({})
            data[CommonConstants.X] = item.pos.x
            data[CommonConstants.Y] = item.pos.y
            data[CommonConstants.Name] = item.name
            if (isViaSetting) {
                sendEvent(TestBookmarkPageViewEvent.AddVia, data)
            } else {
                sendEvent(TestBookmarkPageViewEvent.QueryInDriveMode, data)
            }
        }
    }

    Component.onCompleted: {
        isCompleted = true
    }

    onMediated: {
        requestViewData()
    }

    onVisibleChanged: {
        if (!visible)
            optionMenu.visible = false
    }

    onActivated: {
        console.log( "#1" )

        sendEvent(TestBookmarkPageViewEvent.Activated)

        activatedPage()

        if (isViaSetting) {
            var softkeyMenus = []
            //17.10.24 hskim
            //sendEvent(BookmarkPageViewEvent.Activated, softkeyMenus)
            sendEvent(TestBookmarkPageViewEvent.SelectSoftkey, softkeyMenus)
        }
    }

    onDeactivated: {
        sendEvent(TestBookmarkPageViewEvent.Deactivated)

        if (isViaSetting)
            //17.10.24 hskim
            sendEvent(TestBookmarkPageViewEvent.ResetCsw)            
    }

    onCallerChanged: {
        isViaSetting = caller === CommonConstants.objectName(ViewRegistry.RouteSettingView)
    }

    onFavoriteChanged: {
        if (favorite == null)
            return

        favorite = null
    }

    ListModel { id: listModel   }

    Component {
        id:listDelegate

        Column{
            id: column

            width: parent.width
            height: rowHeight

            property bool isFocused: listView.currentIndex === index && searchRoot.focused
            property bool isButtonFocused: false
            property point pos: Qt.point(model.x, model.y)
            property string name: model.text
            property alias buttonVisible: btn.visible

            function execButton() {
                btn.execButton()
            }

            onIsFocusedChanged: {
                if (isFocused) {
                    btn.txt.color = "#000000"
                } else {
                    btn.txt.color = btn.txt.colorN
                    isButtonFocused = false
                }
            }
            onIsButtonFocusedChanged: {
                if (isButtonFocused) {
                    btn.img.source = Resource.imageUrl("btn_bg_f.png")
                } else {
                    btn.img.source = Resource.imageUrl("btn_bg_n.png")
                }
            }

            Component.onCompleted: {
                switch (model.type) {
                case 0:
                    mainTxt.text = model.text
                    break
                case 1:
                    mainTxt.text = textArr[textEnum.Office]
                    break
                case 2:
                    mainTxt.text = textArr[textEnum.Home]
                    break
                }
            }

            Item {
                width: parent.width
                height: parent.height - splitBar.height
                TMouseArea {
                    width: parent.width
                    height: parent.height
                    onClicked: {
                        csw.forceLostFocus()

                        if (listView.currentIndex !== model.index) {
                            listView.currentIndex = model.index
                        } else {
                            // 선택된 아이템을 다시 클릭하면 경유지 추가 또는 경로 안내 시작.
                            selectItemTwice()
                        }
                    }
                }
                Row {
                    x: GlobalConstants.kListLineLeftMargin
                    width: parent.width - (x + GlobalConstants.kListLineRightMargin)
                    height: parent.height
                    Image {
                        // home, office icon
                        id: icon
                        anchors.verticalCenter: parent.verticalCenter
                        source: {
                            var useFocusedItem = isFocused && !isButtonFocused
                            switch (model.type) {
                            case 1:
                                if (useFocusedItem)
                                    return Resource.imageUrl("bookmark_ic_office_f.png")
                                else
                                    return Resource.imageUrl("bookmark_ic_office.png")

                            case 2:
                                if (useFocusedItem)
                                    return Resource.imageUrl("bookmark_ic_home_f.png")
                                else
                                    return Resource.imageUrl("bookmark_ic_home.png")

                            default:
                                return ""
                            }
                        }
                    }
                    Column{
                        width: {
                            return parent.width
                                    - (icon.width)
                                    - (btn.width+ GlobalConstants.kListLineRightMargin)
                                    - (checkButton.visible ? checkButton.width : 0)
                        }
                        anchors.verticalCenter: parent.verticalCenter
                        spacing: 12
                        Text {
                            id: mainTxt
                            width: parent.width
                            height: 32
                            verticalAlignment: Qt.AlignVCenter
                            font { family: GlobalConstants.kFontFamily; pixelSize: 35 }
                            color: (isFocused && !isButtonFocused) ? "#000000" : GlobalConstants.kListTextColor
                            Text {
                                id: subText
                                anchors.left: parent.left
                                anchors.leftMargin: 100
                                anchors.verticalCenter: parent.verticalCenter
                                width: 449
                                height: 28
                                font { family: GlobalConstants.kFontFamily; pixelSize: 29 }
                                color: "#8a8a99"
                                text: textArr[textEnum.Register]
                                visible: !searchRoot.editMode && model.seq < 0
                            }
                        }
                        Text {
                            id: detailText
                            width: parent.width; height: 28
                            visible: searchRoot.editMode
                            verticalAlignment: Qt.AlignVCenter
                            font { family: GlobalConstants.kFontFamily; pixelSize: 29 }
                            color: "#8a8a99"
                            text: model.seq < 0 ? qsTr("등록하세요") : model.subText
                        }
                    }
                    TPushButton { // 상세 or 편집 버튼
                        id: btn

                        visible: !isViaSetting
                        label: {
                            if (model.seq < 0) {
                                return textArr[textEnum.Registration]
                            } else {
                                if (searchRoot.editMode) {
                                    if (0 === model.type)
                                        return textArr[textEnum.Edit]
                                    else
                                        return textArr[textEnum.Change]
                                } else {
                                    return textArr[textEnum.Detail]
                                }
                            }
                        }
                        width: 88
                        height: 58
                        anchors.verticalCenter: parent.verticalCenter

                        Component.onCompleted: {
                            box.colorN = "transparent"
                            box.colorP = "transparent"
                            img.sourceN = Resource.imageUrl("btn_bg_n.png")
                            img.sourceP = Resource.imageUrl("btn_bg_lp.png")
                            img.sourceSize.width = width
                            img.sourceSize.height = height
                        }

                        function execButton() {
                            csw.forceLostFocus()

                            if (searchRoot.editMode) {
                                if (label === textArr[textEnum.Edit]) {
                                    var data = ({})
                                    data[CommonConstants.Name] = model.text//subText
                                    data[SearchConstants.Id] = model.seq
                                    sendEvent(TestBookmarkPageViewEvent.EditFavoriteAliasClicked, data)

                                    data = ({})
                                    data[CommonConstants.Owner] = CommonConstants.objectName(ViewRegistry.BookmarkPageView)
                                    data[CommonConstants.X] = keyboard.x
                                    data[CommonConstants.Y] = keyboard.y
                                    data[CommonConstants.Z] = keyboard.z + 1
                                    data[CommonConstants.Width] = keyboard.width
                                    data[CommonConstants.Height] = keyboard.height
                                    data[CommonConstants.Text] = ""
                                    sendEvent(TestBookmarkPageViewEvent.ShowKeyboard, data)

                                } else if (label === textArr[textEnum.Registration] || label === textArr[textEnum.Change]) {
                                    showAddFavoriteView(model.type)
                                }
                            } else {
                                if (btn.label === textArr[textEnum.Registration]) {
                                    showAddFavoriteView(model.type)
                                } else {
                                    var result = []
                                    result[SearchResultViewEvent.ResultObject]                = null
                                    result[SearchResultViewEvent.ResultOPIPoleCate]           = -1
                                    result[SearchResultViewEvent.ResultX]                     = parseInt(model.x)
                                    result[SearchResultViewEvent.ResultY]                     = parseInt(model.y)
                                    result[SearchResultViewEvent.ResultStdName]               = model.text
                                    result[SearchResultViewEvent.ResultAddress]               = CommonConstants.Empty
                                    result[SearchResultViewEvent.ResultDistance]              = 0
                                    result[SearchResultViewEvent.ResultOPIPrice]              = -1
                                    result[SearchResultViewEvent.ResultTelNum]                = CommonConstants.Empty
                                    result[SearchResultViewEvent.ResultWebSite]               = SearchConstants.NoneData
                                    result[SearchResultViewEvent.ResultHolliday]              = SearchConstants.NoneData
                                    result[SearchResultViewEvent.ResultWorkTime]              = SearchConstants.NoneData
                                    result[SearchResultViewEvent.ResultIndustry]              = SearchConstants.NoneData
                                    result[SearchResultViewEvent.ResultParking]               = SearchConstants.NoneData
                                    result[SearchResultViewEvent.ResultIntroduction]          = SearchConstants.NoneData

                                    var data = ({})
                                    data[SearchConstants.Result] = result
                                    data[SearchConstants.DestType] = model.type
                                    sendEvent(TestBookmarkPageViewEvent.FavoriteDetailClicked, data)
                                }
                            }
                        }

                        onButtonClicked: {
                            execButton()
                        }
                    }
                    TCheckButton {
                        id: checkButton
                        visible: searchRoot.editMode && 0 <= model.seq
                        width: img.width
                        height: img.height
                        scale: 0.5
                        imgCheck: Resource.imageUrl("checkbox_on.png")
                        imgUncheck: Resource.imageUrl("checkbox_off.png")
                        selected: model.selected
                        anchors.verticalCenter: parent.verticalCenter

                        onChangeValue: {
                            model.selected = selected

                            // blockEvent가 설정되어 있으면 이벤트를 발생시키지 않는다.
                            if (!searchRoot.blockEvent) {
                                if (selected == 1)
                                    sendEvent(TestBookmarkPageViewEvent.FavoriteItemChecked, model.seq)
                                else
                                    sendEvent(TestBookmarkPageViewEvent.FavoriteItemUnchecked, model.seq)
                            }
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

    Component {
          id: highlightBar
          Rectangle {
              visible: listView.currentItem
              x: GlobalConstants.kListBackgroundLeftMargin
              y: listView.currentItem ? listView.currentItem.y : 0;
              width: listView.currentItem ? listView.currentItem.width  - (GlobalConstants.kListBackgroundLeftMargin + GlobalConstants.kListBackgroundRightMargin) : 0
              height: listView.currentItem ? listView.currentItem.height : 0
              color: highlightColor
          }
    }

    Column {
        width: parent.width
        height: parent.height
        Rectangle{
            width: parent.width
            height: parent.height
            color: (listModel.count) ? GlobalConstants.kListNormalColor : "gray"
            ListView {
                id: listView
                width: parent.width
                height: parent.height
                cacheBuffer: rowHeight * 2
                visible: (listModel.count) ? true : false
                clip: true
                model: listModel
                delegate: listDelegate
                currentIndex: -1
                highlight: highlightBar
                highlightFollowsCurrentItem: false

                // masking
                onMovementStarted:  imgMask.visible = true
                onMovementEnded:  imgMask.visible = false
                Image {
                      id: imgMask
                      width: parent.width
                      height: 30
                      y: parent.y + (parent.height - height)
                      visible: false
                      sourceSize.width: width
                      sourceSize.height: height
                      source: Resource.imageUrl("zone/zone2_img_mask.png")
                }

                TScrollBar {
                    flickable: parent
                    width: 2
                    color: GlobalConstants.kScrollBarFrontColor
                    grooveColor: GlobalConstants.kScrollBarBackColor
                    rightMargin: GlobalConstants.kScrollBarRightMargin
                    onScroll:    imgMask.visible = true
                    onMouseRelease:  imgMask.visible = false
                }
            }
            Text {
                id: message
                width: parent.width
                height: parent.height - (rowHeight * 2)
                y: rowHeight * 2
                visible: false
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
                font { family: GlobalConstants.kFontFamily; pixelSize: GlobalConstants.kListTextFontSize * 0.5 }
                color: GlobalConstants.kListTextColor
                text: textArr[textEnum.NoData]
            }
        }
    }

    Item {
        id: keyboard
        width: parent.width
        height: 150
        anchors.bottom: parent.bottom
    }

    BookmarkPageViewOption {
        id: optionMenu
        anchors { bottom: parent.bottom; right: parent.right }
        onByNameClicked: {
            var data = { max: kMaxFavorite, offset: 0, type: 1 }
            resetFavoriteList()

            sendEvent(TestBookmarkPageViewEvent.RequestFavorite, data)
        }
        onByOrderClicked: {
            var data = { max: kMaxFavorite, offset: 0, type: 0 }
            resetFavoriteList()

            sendEvent(TestBookmarkPageViewEvent.RequestFavorite, data)
        }
    }    

    Loader {
        id: dialogLoader

        // 17.11.8 hskim not to be hidden by keyboard view
        //anchors.fill: searchRoot
        width: searchRoot.width
        anchors.top: searchRoot.top
        anchors.bottom: keyboard.top
        //anchors.topMargin: 0

        onLoaded: {
            switch (searchRoot.dialogType) {
            case 0: // delete all favorites.
                item.content.sourceComponent = deleteAllContents
                break
            case 1: // edit favorite alias
                item.content = editFavoriteAliasComponent
                break
            }

            item.show()
        }

        Component {
            id: dialogComponent

            TDialog {
                title: textArr[textEnum.DeleteAll]
                enableAutoClose: false
                contentWidth: GlobalConstants.kDialogContentWidth
                contentHeight: GlobalConstants.kDialogContentHeight
                content: Rectangle {
                    anchors.fill: parent
                    color: 'darkgray'
                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.verticalCenter: parent.verticalCenter
                        font { family: GlobalConstants.kFontFamily; pixelSize: 16 }
                        color: GlobalConstants.kListTextColor
                        horizontalAlignment: Text.AlignHCenter
                        text: textArr[textEnum.WantToDeleteAll]
                    }
                }
                onOpened: {
                    var data = ({})
                    data[CommonConstants.Modal] = true
                    sendEvent( TestBookmarkPageView.ModalChanged, data )
                }
                onClosed: {
                    var data = ({})
                    data[CommonConstants.Modal] = false
                    sendEvent( TestBookmarkPageView.ModalChanged, data )
                }
                onLeftButtonClicked: { close() }
                onRightButtonClicked: {
                    close()
                    sendEvent(TestBookmarkPageView.DeleteAllFavoriteItems)
                }
            }
        }
        Component {
            id: editFavoriteAliasDialog

            TDialog {
                contentWidth: GlobalConstants.kDialogContentWidth
                contentHeight: GlobalConstants.kDialogContentHeight-100
                title: textArr[textEnum.ChangeName]
                enableAutoClose: false
                content: Rectangle {
                    anchors.fill: parent
                    color: 'darkgray'

                    Rectangle {
                        width: parent.width - 32
                        height: 32
                        color: Qt.darker("darkgray")
                        anchors.centerIn: parent
                        Rectangle {
                            anchors.fill: parent
                            anchors.margins: 3
                            color: "gray"

                            Row {
                                anchors.fill: parent
                                TextInput {
                                    id: textInput

                                    width: parent.width - icon.width
                                    height: parent.height
                                    font { family: GlobalConstants.kFontFamily; pixelSize: 16 }
                                    color: GlobalConstants.kListTextColor
                                    text: searchRoot.dialogText
                                    onTextChanged: searchRoot.aliasText = text
                                }
                                Image {
                                    id: icon
                                }
                            }
                        }
                    }
                }

                onLeftButtonClicked: {
                    hideKeyboard()
                    close()
                }
                onRightButtonClicked: {
                    hideKeyboard()
                    close()
                    var data = ({})
                    data[CommonConstants.Name] = searchRoot.aliasText
                    data[SearchConstants.Id] = searchRoot.aliasId
                    sendEvent(TestBookmarkPageView.EditFavoriteAlias, data)
                }
            }
        }
    }

    Csw {
        id: csw

        section: Csw.Section_List
        sequence: 0
        enable: searchRoot.visible

        onFocused: {
            searchRoot.focused = true
            highlightColor = GlobalConstants.kListFocusColor
            if (listView.currentIndex < 0)
                listView.currentIndex = 0
        }
        onLostFocusd: {
            highlightColor = GlobalConstants.kListPressColor
            searchRoot.focused = false
        }
        onLeftPressed: {
            var item = listView.currentItem
            item.isButtonFocused = false
            highlightColor = GlobalConstants.kListFocusColor
            sendEvent( TestBookmarkPageViewEvent.ChangeSearchTabViewPage,
                {isCswControl:true, page:0})
        }
        onRightPressed: {
            var item = listView.currentItem
            if (item.buttonVisible) {
                item.isButtonFocused = true
            }
            highlightColor = "#000000"
        }
        onSpinCwPressed: {
            if (listView.currentIndex < listView.count-1) {
                ++listView.currentIndex

                var item = listView.currentItem
                item.isButtonFocused = false
                highlightColor = GlobalConstants.kListFocusColor
            }
        }
        onSpinCcwPressed: {
            if (0 < listView.currentIndex) {
                --listView.currentIndex
                var item = listView.currentItem
                item.isButtonFocused = false
                highlightColor = GlobalConstants.kListFocusColor
            }
        }
        onSelected: {
            var item = listView.currentItem
            if (item.isButtonFocused) {
                item.execButton()
            } else {
                selectItemTwice()
            }
        }
    }
}

