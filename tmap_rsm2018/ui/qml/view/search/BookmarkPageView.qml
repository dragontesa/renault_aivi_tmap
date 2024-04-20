import QtQuick 2.1
import QtGraphicalEffects 1.0
import BookmarkPageViewEvent 1.0
import ViewRegistry 1.0
import CommonConstants 1.0
import LayoutConstants 1.0
import ZoneConstants 1.0
import SearchConstants 1.0
import SearchTabViewEvent 1.0
import SearchResultDetailViewEvent 1.0
import SearchResultViewEvent 1.0
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
    height: parent ? parent.height : undefined

    property var caller: null // 즐겨찾기로 경유지 추가시에만 사용.
    property var favorite: null
    property int index: 0
    property int requestMax : kMaxFavorite
    property bool editMode: false
    property bool blockEvent: false
    property bool isViaSetting: false
    property int rowHeight: editMode ? 115 : 84

    property bool focused: false
    property bool buttonFocused: false

    readonly property int kMaxFavorite: 100
    property bool isCompleted: false
    property int modelCount: 0
    property int checked: 0 // it have to be delivered from normal view to the standalone edit view
    property string optionMenu: '이름순'
    property bool sortByName: optionMenu === '이름순'
    property string lang: 'ko'
    property bool focusCsw: false

    //Timer
    property var timer : null

    //ID
    property var listView: null

    onEditModeChanged: {
        console.log('editMode = ' + editMode)
    }

    // Constants
    readonly property string kDeleteAllMessage: qsTr("즐겨찾기 리스트를\n모두 삭제하시겠습니까?")
    readonly property var kSoftkeys: {
        "Back": ZoneConstants.Back, // 뒤로
        "Option": ZoneConstants.Option, // 옵션
        "Navi": ZoneConstants.Navi, // 네비
        "Menu": ZoneConstants.Menu, // 메뉴
        "Confirm": qsTr("확인"), // 확인
        "AddFavorite": qsTr("즐겨찾기 추가"), // 즐겨찾기 추가
        "EditFavorite": qsTr("즐겨찾기 편집"), // 즐겨찾기 편집
        "DeleteAll": qsTr("전체삭제"), // 전체삭제
        "DeleteSelected": qsTr("삭제"), // 선택삭제
        "Qml": CommonConstants.url(ViewRegistry.BookmarkRegistView), // Qml uri
        'ByOrder': '등록순',
        'ByName': '이름순'
    }

    readonly property var kLabels: {
        "Detail": qsTr("상세"),
        "Edit"  : qsTr("명칭변경"),
        "Home"  : qsTr("집으로"),
        "Office": qsTr("회사로"),
        "Registration": qsTr("등록"),
        "Change": qsTr("변경"),
        "Register": qsTr("(등록이 필요합니다)"),
        "DeleteAll": qsTr("전체삭제"),
        "ChangeName": qsTr("명칭변경")
    }

    layout: QtObject {
        property int checkBoxSize: 63 // 체크박스 크기
            function updateLayout( resolution ) {
            if ( resolution === LayoutConstants.L800x480 ) {
                checkBoxSize = 30
            }
            else if ( resolution === LayoutConstants.L820x1024 ) {
                checkBoxSize = 63
            }
        }
    }

    function retranslateUi() {
        if (isViaSetting) {
            sendEvent(BookmarkPageViewEvent.SelectZone1Title, qsTr('즐겨찾기'))
        } else if (editMode) {
            sendEvent(BookmarkPageViewEvent.SelectZone1Title, qsTr('즐겨찾기 편집'))
        }

        sendEvent(BookmarkPageViewEvent.RetranslateUi)
    }

    function requestViewData()
    {
        // optionMenu가 이름순이라면 현재 적용중인 메뉴는 등록순
        var byOrder = optionMenu === '이름순'
        requestFavorite(byOrder)
    }

    function resetFavoriteList()
    {
        listModel.clear()
        listView.model = null
    }

    function requestFavorite(byOrder) {
        resetFavoriteList()
        var data = {
            max: kMaxFavorite,
            offset: 0,
            type: byOrder ? 0 : 1
        }
        sendEvent(BookmarkPageViewEvent.RequestFavorite, data)
    }

    function pushFavoriteList(newList, cnt)
    {
        listModel.clear()  // allow push type list by lckxx 2017.11.24

        // constantly reserve two items on top for home and office
        var homeData = { type: 2, seq: -1, text: kLabels.Home, x:0, y:0, selected:0 }
        listModel.insert(0, homeData)

        var officeData = { type: 1, seq: -1, text: kLabels.Office, x:0, y:0, selected:0 }
        listModel.insert(1, officeData)

        modelCount = 0

        for (var i=0; i<cnt; i++)
        {
            var dest = newList[i]
            var type = dest[SearchConstants.DestType]
            var text = dest[CommonConstants.Alias].length > 0 ? dest[CommonConstants.Alias] : dest[CommonConstants.Name]
            if (text.length === 0)
               text = dest[CommonConstants.Address]

            if (type === 1 || type === 2) { // home or office
                var subText = dest[CommonConstants.Address]
                if (subText.length === 0)
                    subText = dest[CommonConstants.Name]
                listModel.set (
                    type%2,
                    {
                        type: type,
                        seq: dest[SearchConstants.Id],
                        text: text,
                        subText: subText,
                        x: dest[CommonConstants.X],
                        y: dest[CommonConstants.Y],
                        poiID: parseInt(dest[SearchConstants.PoiID]),
                        selected: 0,
                        navSeq: dest[SearchConstants.NavSeq],
                        rpFlag: dest[SearchConstants.RpFlag],
                        poleCate: dest[SearchConstants.OpiPoleCate]
                    }
                )
                continue;
            }

            listModel.append (
            {
                type: type,
                seq: dest[SearchConstants.Id],
                text: text,
                subText: dest[CommonConstants.Address],
                x: dest[CommonConstants.X],
                y: dest[CommonConstants.Y],
                poiID: parseInt(dest[SearchConstants.PoiID]),
                selected: 0,
                navSeq: dest[SearchConstants.NavSeq],
                rpFlag: dest[SearchConstants.RpFlag],
                poleCate: dest[SearchConstants.OpiPoleCate]
            });
            ++modelCount
        }

        listView.model = null
        listView.model = listModel
        listView.currentIndex = -1
    }

    function activatedPage() {
        // 17.11.8 hskim
        if (isViaSetting)
            sendEvent(BookmarkPageViewEvent.SelectZone1Title, qsTr('즐겨찾기'))
        else
            sendEvent(BookmarkPageViewEvent.SelectZone1TextInput)

        sendEvent(BookmarkPageViewEvent.ResetCsw)

        sendEvent(BookmarkPageViewEvent.UpdateSoftkey)

        var args = [ csw ]
        sendEvent(BookmarkPageViewEvent.RegistCsw, args)
        searchRoot.editMode = false
    }

    function deleteSelected() {
        var list = []
        for (var i=listModel.count-1; i>=0; --i) {
            var item = listModel.get(i)
            if (item.selected === 1) {
                list.push({'type':item.type, 'seq':item.seq})

                if (item.type === 0) {
                    listModel.remove(i, 1)
                } else {
                    item.seq = -1
                    item.x = 0
                    item.y = 0
                    item.selected = 0
                }
            }
        }
        sendEvent(BookmarkPageViewEvent.DeleteSelectedFavoriteItems, list)

        // 삭제 후 즐겨찾기 목록을 재요청. TMCRSM-443
        requestViewData()
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
                item.text = name
                break
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

    function checkedItems() {
        var checked = 0
        for (var i=0; i<listModel.count; i++) {
            if (listModel.get(i).selected)
                ++checked
        }
        return checked
    }

    function favoriteListCount() {
        return searchRoot.listCount()
    }
    function favoriteListCountWithoutHomeAndOffice() {
        // 집,회사는 항상 있기 때문에 2를 뺀다.
        return listModel.count - 2
    }

    function recoverMenuFromEdit() {
        activatedPage()
        uncheckAll()
        sendEvent(BookmarkPageViewEvent.SelectZone1TextInput) /*! Fixed artf258330, recover view title by Leechokang 2017.6.5 */
    }

    function showView( args )
    {
        sendEvent(BookmarkPageViewEvent.CreateView, args)
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
        data[CommonConstants.Url] = kSoftkeys.Qml
        data[CommonConstants.Caller] = CommonConstants.objectName(ViewRegistry.BookmarkPageView)
        data[CommonConstants.Owner] = ownerType
        showView(data )
    }

    function closeView() {
        isCompleted = false

        // 17.11.7 hskim commented the code below not to recreate search tab view
        //sendEvent(BookmarkPageViewEvent.SelectZone1TextInput)
        sendEvent(BookmarkPageViewEvent.ActivatedForNavi)
        var data = ({})
        data[CommonConstants.ObjectName] = CommonConstants.objectName(ViewRegistry.SearchTabView)
        sendEvent(BookmarkPageViewEvent.CloseView, data)
    }

    function onSoftkeyIndexMenuFunction(index) {
        switch (index) {
        case BookmarkPageViewEvent.MenuAddBookmark:
            if (modelCount < kMaxFavorite) {
                showAddFavoriteView(0)
            } else {
                sendEvent(BookmarkPageViewEvent.ShowMaxFavoriteDialog)
            }
            break

        case BookmarkPageViewEvent.MenuEditBookmark:
            if (listModel.count > 0) {
                checked = 0
                for (var i=0; i<listModel.count; i++) {
                    if (listModel.get(i).selected)
                        ++checked
                }

                var data = ({})
                data[SearchConstants.Checked] = checked
                sendEvent(BookmarkPageViewEvent.EditFavoriteClicked, data)
            }
            break

        case BookmarkPageViewEvent.MenuDeleteAll:
            sendEvent(BookmarkPageViewEvent.DeleteAllClicked)
            break

        case BookmarkPageViewEvent.MenuDeleteSelected:
            deleteSelected()
            break

        case BookmarkPageViewEvent.OptionSortByOrder:
            requestFavorite(true) //byOrder = true
            optionMenu = kSoftkeys.ByName
            break

        case BookmarkPageViewEvent.OptionSortByName:
            requestFavorite(false) //byOrder = false
            optionMenu = kSoftkeys.ByOrder
            break
        }
    }

    function __doSoftKeyFunction(button, menuName) {
      if (button === kSoftkeys.Back) {
          if (isViaSetting) {
              // 경유지 추가 모드
              // 17.11.8 hskim
              //close()
              visible = false
              sendEvent(BookmarkPageViewEvent.BackToViaSetting)
          } else {
            // 통합 검색창 내
            if (editMode == false)
               closeView()
            else //  it might be not called because that the mediator has already done
               close();
          }

          // TMCRSM-481. 네비메뉴로부터 즐겨찾기로 왔다면 뒤로가기시 네비메뉴로 복귀
          // 18.03.04 ymhong
          if (caller == CommonConstants.objectName(ViewRegistry.NaviMenuView)) {
              var url = CommonConstants.url(ViewRegistry.NaviMenuView)
              sendEvent(BookmarkPageViewEvent.CreateView, {'url':url})
          }
      }
    }

    function listCount() {
        var count = listModel.count - 2
        for (var i=0; i<2; i++) {
            var item = listModel.get(i)
            if (item && item.x > 0 && item.y > 0)
                ++count
        }
        return count
    }

    function updateItemAlias(index, name) {
        for (var i = 0; i < listModel.count; i++) {
            var item = listModel.get(i)
            if (item.type === 0 && item.seq === index) {
                item.text = name
                break
            }
        }
    }

    Component.onCompleted: {
        isCompleted = true
    }

    onMediated: {
        requestViewData()
        sendEvent(BookmarkPageViewEvent.Mediated)
    }

    onActivated: {
        if (editMode == false) {
            sendEvent(BookmarkPageViewEvent.Activated)

            activatedPage()
            if (focusCsw) {
                sendEvent(BookmarkPageViewEvent.FocusCsw, csw.objectName)
                focusCsw = false
            }
        }
        else { // standalone edit view version
            var data = {}
            data["editMode"] = true;
            data[SearchConstants.Checked] = checkedItems();
            sendEvent(BookmarkPageViewEvent.Activated, data)
            sendEvent(BookmarkPageViewEvent.ResetCsw)
            var args = [ csw ]
            sendEvent(BookmarkPageViewEvent.RegistCsw, args)
            sendEvent(BookmarkPageViewEvent.SelectZone1Title, kSoftkeys.EditFavorite)
        }
    }

    onDeactivated: {
        sendEvent(BookmarkPageViewEvent.Deactivated)

        if (isViaSetting)
            //17.10.24 hskim
            sendEvent(BookmarkPageViewEvent.ResetCsw)

        sendEvent(BookmarkPageViewEvent.ShowZone3)
    }

    Component.onDestruction: {
        timer = CommonUtils.destroy(timer)
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

    Item {
        id: nullItem
    }

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

            states: [
                State {
                    name: 'normal'
                    PropertyChanges {
                        target: mainTxt
                        width: mainTxt.parent.width
                        elide: Text.ElideRight
                    }
                    PropertyChanges {
                        target: marqueeHilight
                        marquee: false
                    }
                },
                State {
                    name: 'focused'
                    PropertyChanges {
                        target: mainTxt
                        width: mainTxt.contentWidth
                        elide: Text.ElideNone
                    }
                    PropertyChanges {
                        target: marqueeHilight
                        marquee: true
                    }
                }
            ]
            function selectItem() {
                if (checkButton.enabled) {
                    checkButton.selected = checkButton.selected ? CommonConstants.False: CommonConstants.True
                }

                listView.currentIndex = model.index
                var index = listView.currentIndex
                if (index < 0)
                    return

                var item = listModel.get(index)
                var canRp = (0 < item.x && 0 < item.y)
                if (canRp && !editMode) {
                    btn.mouseAreaEnabled = false
                } else {
                    sendEvent(BookmarkPageViewEvent.FavoriteItemReleased)
                }

                var sleep =  GlobalConstants.kPressedColorExposureTime
                timer = CommonUtils.destroy(timer)
                timer = CommonUtils.delay(nullItem, sleep, function() {
                    try {
                        if (canRp) {
                            var data = ({})
                            data[CommonConstants.X] = item.x
                            data[CommonConstants.Y] = item.y
                            data[CommonConstants.Name] = item.text
                            data[CommonConstants.Address] = item.subText
                            data[SearchConstants.PoiID] = item.poiID
                            data[SearchConstants.NavSeq] = item.navSeq
                            data[SearchConstants.RpFlag] = item.rpFlag
                            data[CommonConstants.Type] = item.type
                            if (isViaSetting) {
                                sendEvent(BookmarkPageViewEvent.AddVia, data)
                            } else {
                                sendEvent(BookmarkPageViewEvent.FavoriteItemSelectedTwice, data)
                            }
                        }
                    } catch(e) {
                    }
                })
            }

            function execButton() {
                btn.execButton()
            }
            onIsFocusedChanged: {
                if (!isFocused) {
                    isButtonFocused = false
                }
                btn.focused = false
                btn.parentFocused = isFocused
            }
            onIsButtonFocusedChanged: {
                if (isButtonFocused) {
                    btn.focused = true
                } else {
                    btn.focused = false
                }
            }

            Item {
                id: listItem
                width: parent.width
                height: parent.height - splitBar.height
                CommonListBar {
                    id: listBar
                    anchors.fill: parent
                    onRealPressed: {
                        sendEvent(BookmarkPageViewEvent.FavoriteItemPressed)
                    }
                    onPressed: {
                        focused = false; buttonFocused = false
                        listView.currentIndex = index
                        csw.forceLostFocus()
                    }
                    onClicked: {
                        csw.forceLostFocus()
                        selectItem()
                    }
                }

                Row {
                    x: GlobalConstants.kListTextLeftMargin
                    width: parent.width - (x + GlobalConstants.kListTextRightMargin)
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
                    }//Image
                    Item { id: dummy; width: icon.status == Image.Null ? 0 : 20; height: parent.height }

                    Item {
                        id: marqueeItem
                        width: {
                            return parent.width
                            - (dummy.width)
                            - (buttonCheckboxSpace.width)
                            - (icon.width)
                            - (btn.visible ? btn.width : 0)
                            - (checkButton.visible ? checkButton.width : 0)
                            }
                        height: parent.height
                        clip: true

                        Column {
                            id: textItem
                            width: btn.visible ? (parent.width - 40) : parent.width
                            anchors.verticalCenter: parent.verticalCenter
                            spacing: 12

                            TListMarqueeText {
                                id: mainTxt
                                width: parent.width; height: 32
                                fontSize: 35
                                textColor: (isFocused && !isButtonFocused) ? "#000000" : GlobalConstants.kListTextColor
                                textOffsetY: 3
                                enableMarquee: isFocused && !isButtonFocused

                                text: {
                                    switch (model.type) {
                                        case 0: return model.text
                                        case 1: return qsTr('회사로') + Tr.d
                                        case 2: return qsTr('집으로') + Tr.d
                                    }
                                }

                                Text {
                                    id: subText
                                    anchors.left: parent.left
                                    anchors.leftMargin: mainTxt.contentWidth < 80 ? 100 : (mainTxt.contentWidth + 20)
                                    anchors.verticalCenter: parent.verticalCenter
                                    anchors.verticalCenterOffset: lang === 'ko' ? 0 : 4
                                    width: 449
                                    height: 28
                                    font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(29) }//pixelSize: 29 }
                                    color: {
                                        if (isFocused && !isButtonFocused)
                                        return '#000000'
                                        else if (listView.currentIndex == index)
                                        return '#ffffff'
                                        else
                                        return '#8a8a99'
                                    }
                                    text: qsTr('(등록이 필요합니다)') + Tr.d
                                    visible: !searchRoot.editMode && model.seq < 0
                                }//subText
                            }//mainTxt

                            Text2 {
                                id: detailText
                                width: parent.width; height: 28
                                visible: searchRoot.editMode
                                offsetY: 3
                                verticalAlignment: Qt.AlignVCenter
                                font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(29) }//pixelSize: 29 }
                                color: {
                                    if (isFocused && !isButtonFocused)
                                        return '#000000'
                                    else if (listView.currentIndex == index)
                                        return '#ffffff'
                                    else
                                        return '#8a8a99'
                                }
                                text: model.seq < 0
                                      ? qsTr("(등록이 필요합니다)")
                                      : (model.subText ? model.subText : '')
                                elide: Text.ElideRight
                            }//detailText

                            TMarquee {
                                id:marqueeHilight
                                parentObj: marqueeItem
                                textObj: mainTxt
                                marquee: (listView.currentIndex == model.index) && parent.width<textObj.width
                                onMarqueeFinished: {
                                    marquee = false
                                    marquee =  (listView.currentIndex == model.index) && parent.width<textObj.width
                                }
                                onTextMoved: gradientRect.visible = true
                                onTextStopped: gradientRect.visible = false
                            }
                        }//Column
                    }//marqueeItem
                    LinearGradient {
                        id: gradientRect
                        anchors.fill: marqueeItem
                        gradient: Gradient {
                            GradientStop { position: 0; color: Qt.rgba(0,0,0,1) }
                            GradientStop { position: 16/gradientRect.width; color: Qt.rgba(0,0,0,0) }
                            GradientStop { position: (gradientRect.width-16)/gradientRect.width; color: Qt.rgba(0,0,0,0) }
                            GradientStop { position: 1; color: Qt.rgba(0,0,0,1) }
                        }
                        start: Qt.point(0, 0)
                        end: Qt.point(gradientRect.width, 0)
                        visible: false
                    }

                    TTextButton{ // 상세 or 편집 버튼
                        id: btn
                        label: {
                            if (model.seq < 0 && !isViaSetting) {
                                return qsTr('등록') + Tr.d //kLabels.Registration
                            } else  {
                                if (searchRoot.editMode) {
                                    if (0 === model.type)
                                        return qsTr('명칭변경') + Tr.d //kLabels.Edit
                                    else
                                        return qsTr('변경') + Tr.d //kLabels.Change
                                } else if (model.seq >= 0) {
                                    return qsTr('상세') + Tr.d //kLabels.Detail
                                } else {
                                    return ""
                                }
                            }
                        }
                        visible: label.length > 0

                        width: (0 === model.type && searchRoot.editMode) ? 132 : 88
                        height: 58
                        radius: height/2
                        fontPixelSize: Resource.pixelToDp(30);
                        anchors.verticalCenter: parent.verticalCenter
                        parentPressed: listBar.mousePressed

                        function execButton() {
                            csw.forceLostFocus()

                            if (searchRoot.editMode) {
                                if (label === qsTr('명칭변경')+Tr.d) { // kLabels.Edit) {
                                    var data = ({})
                                    data[CommonConstants.Name] = model.text//subText
                                    data[SearchConstants.Id] = model.seq
                                    sendEvent(BookmarkPageViewEvent.EditFavoriteAliasClicked, data)
                                } else if (label === qsTr('등록')+Tr.d || label === qsTr('변경')+Tr.d) { //kLabels.Registration || label === kLabels.Change) {
                                    showAddFavoriteView(model.type)
                                }
                            } else {
                                if (btn.label === qsTr('등록')+Tr.d) { // kLabels.Registration) {
                                    showAddFavoriteView(model.type)
                                } else {
                                    var result = SearchConstants.resultInit()
                                    result[SearchConstants.ResultName                ] = model.text
                                    result[SearchConstants.ResultStdName             ] = model.text
                                    result[SearchConstants.ResultAddress             ] = model.subText
                                    result[SearchConstants.ResultPOIID               ] = model.poiID
                                    result[SearchConstants.ResultRpFlag              ] = model.rpFlag
                                    result[SearchConstants.ResultX                   ] = parseInt(model.x)
                                    result[SearchConstants.ResultY                   ] = parseInt(model.y)
                                    result[SearchConstants.ResultOPIPoleCate         ] = model.poleCate
                                    result[SearchConstants.ResultNavseq              ] = model.navSeq
                                    var data = ({})
                                    data[SearchConstants.Result] = result
                                    data[SearchConstants.DestType] = model.type
                                    sendEvent(BookmarkPageViewEvent.FavoriteDetailClicked, data)
                                }
                            }
                        }

                        onClicked: {
                            execButton()
                        }
                    }//TPushButton
                    Item {
                        id: buttonCheckboxSpace
                        width: searchRoot.editMode ? 20 : 0
                        height: parent.height
                    }

                    TCheckButton {
                        id: checkButton
                        visible: searchRoot.editMode
                        enabled: (visible && 0 <= model.seq)
                        width: layout.checkBoxSize
                        height: layout.checkBoxSize
                        imgCheck: Resource.imageUrl("checkbox_on.png")
                        imgUncheck: Resource.imageUrl("checkbox_off.png")
                        selected: model.selected
                        anchors.verticalCenter: parent.verticalCenter

                        onSelectedChanged:{
                            if (model.selected !== selected) {
                                model.selected = selected

                                // blockEvent가 설정되어 있으면 이벤트를 발생시키지 않는다.
                                if (!searchRoot.blockEvent) {
                                    if (selected == 1)
                                        sendEvent(BookmarkPageViewEvent.FavoriteItemChecked, model.seq)
                                    else
                                        sendEvent(BookmarkPageViewEvent.FavoriteItemUnchecked, model.seq)
                                }
                            }
                        }
                    }
                }
            }//listItem
            Rectangle {
               id: splitBar
               x: GlobalConstants.kListLineLeftMargin
               width: parent.width - (GlobalConstants.kListLineLeftMargin  + GlobalConstants.kListLineRightMargin)
               height: 1
               color: GlobalConstants.kListLineColor
            }
        }//Column
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
                highlight: Item {
                    Rectangle {
                        anchors { left: parent.left; leftMargin: GlobalConstants.kListBackgroundLeftMargin;
                            right: parent.right; rightMargin: GlobalConstants.kListBackgroundRightMargin;
                        }
                        visible: !buttonFocused
                        height: rowHeight
                        color: focused ? GlobalConstants.kListFocusColor : MultiSenseCoordinator.kAmbientColor
                        opacity: focused ? 1.0 : 0.7
                    }
                }
                highlightMoveDuration : 0

                footer: Item {
                    width: listView.width; height: GlobalConstants.kListViewFooterHeight
                }

                property bool zone3Visible: true

                onMovementStarted: {
                    scrollTimer.stop()
                    zone3Visible = false
                    listView.currentIndex = -1
                    sendEvent(BookmarkPageViewEvent.HideZone3)
                    sendEvent(BookmarkPageViewEvent.FavoriteListScrolled)
                }
                onMovementEnded: {
                    scrollTimer.start()
                    sendEvent(BookmarkPageViewEvent.ShowZone3)
                    sendEvent(BookmarkPageViewEvent.FavoriteListScrolled)
                }
                Timer {
                    id: scrollTimer
                    onTriggered: listView.zone3Visible = true
                    interval: 200
                }
                Image {
                      id: imgMask
                      width: parent.width
                      height: 30
                      anchors.bottom: parent.bottom
                      anchors.bottomMargin: GlobalConstants.kScreenZone3Height
                      visible: scrollBar.heightRatio < 1.0 && listView.zone3Visible
                      sourceSize.width: width
                      sourceSize.height: height
                      source: Resource.imageUrl("zone/zone2_img_mask.png")
                }

                TScrollBar {
                    id: scrollBar
                    flickable: parent
                    color: MultiSenseCoordinator.kAmbientColor
                }
            }
        }
    }

    Csw {
        id: csw

        section: Csw.Section_List
        sequence: editMode ? 1 : 0
        enable: searchRoot.visible
        objectName: 'BookmarkCsw'

        onFocused: {
            searchRoot.focused = true
            if (listView.currentIndex < 0)
                listView.currentIndex = 0
        }
        onLostFocusd: {
            searchRoot.focused = false
            searchRoot.buttonFocused = false
        }
        onLeftPressed: {
            var item = listView.currentItem
            if (item.buttonVisible && item.isButtonFocused) {
                item.isButtonFocused = false
                searchRoot.buttonFocused = false
            } else {
                sendEvent( BookmarkPageViewEvent.ChangeSearchTabViewPage,
                    {isCswControl:true, page:1})
            }
        }
        onRightPressed: {
            var item = listView.currentItem
            if (item.buttonVisible) {
                item.isButtonFocused = true
                searchRoot.buttonFocused = true
            }
        }
        onSpinCwPressed: {
            if (listView.currentIndex < listView.count-1) {
                ++listView.currentIndex
                listView.positionViewAtIndex(listView.currentIndex+1, ListView.Contain)

                var item = listView.currentItem
                item.isButtonFocused = searchRoot.buttonFocused
            }
        }
        onSpinCcwPressed: {
            if (0 < listView.currentIndex) {
                --listView.currentIndex
                listView.positionViewAtIndex(listView.currentIndex, ListView.Contain)

                var item = listView.currentItem
                item.isButtonFocused = searchRoot.buttonFocused
            }
        }
        onSelected: {
            var item = listView.currentItem
            if (item.isButtonFocused) {
                item.execButton()
            } else {
                item.selectItem()
            }
        }
    }
}
