import QtQuick 2.0
import ViewRegistry 1.0
import CommonConstants 1.0
import ZoneConstants 1.0
import SettingConstants 1.0
import SearchConstants 1.0
import TextSearchViewEvent 1.0
import Csw 1.0

import "../../singleton"
import "../../component"
import "../../view"
import "../../view/util"
import "../../view/search"
import "../../GlobalConstants.js" as GlobalConstants
import "../../component/internal/Keyboard.js" as CustomKeyboard
import "../../view/util/CommonUtils.js" as CommonUtils

PopupView {
    id: searchRoot

    visible                         : true

    property bool isMediated        : false
    property bool isActivated       : false
    property bool reFocus           : false
    property bool isClosed          : false
    property bool syncLock          : false

    //onMediated에서 요청 및 응답
    property var __lang             : null
    property bool online            : false
    property bool isRg              : false
    property string addressType     : SearchConstants.None

    //뷰호출객체이름
    property var caller             : null
    property var relay              : null
    property var owner              : null

    //검색을 위한 기본 정보
    property var query              : null       //현재검색 요청 텍스트
    property var lastQuery          : null       //최종검색 요청 텍스트
    property var search             : null       //현재검색 수신 텍스트
    property var result             : ([])       //결과값: (캐시+즐겨찾기+최근목적지+검색결과)리스트와 일치
    property var resultCache        : ([])
    property var resultFavor        : ([])
    property var resultRcntDest     : ([])
    property var resultRcntSearch   : ([])
    property var requestPageIndex   : null
    property bool reqSearch         : false

    property bool isFavorite        : false
    property bool isViaSetting      : false
    property bool focused           : false
    property bool messageVisible    : false
    property bool textlimitToastShow: false
    property bool keyboardDown      : false
    property bool showWaitIndicator : false

    property string messageText     : CommonConstants.Empty
    property color highlightColor   : CommonConstants.Transparent

    property int dlgUniqueNum       : -1
    property int frameHeight        : 0
    property int startCount         : 0
    property int queueCount         : 0
    property var updateCount        : null

    //타이머
    property var timerProg          : null
    property var timerClick         : null

    //ID
    property var listView           : null
    property var message            : null
    property var keyboard           : null

    //Enum
    property var textEnum           : ({})
    property var softKeyEnum        : ({})
    property var resultTypeEnum     : ({})

    property var textArr            : ([])
    property var softKeyMenuArr     : ([])
    property var resultImgArr       : ([])


    //readonly
             property int keyboardHeight        : 427 //타겟일 때 값을 바꾸고 있으므로 readonly 속성을 제거함
    readonly property int defaultCurrentIndex   : -1
    readonly property int requestCacheMaxCount  : 1
    readonly property int requestFavorMaxCount  : 1
    readonly property int requestRdestMaxCount  : 2
    readonly property int searchMaxLen          : 20
    readonly property var resultTextEnum        : ({matchText:0,remainText:1})


    function setKeyboardHeight(height) {
        if (keyboard) {
            keyboard.height = height
        }
        frameHeight = loader.height - height
    }

    function allocateVariant() {
        isActivated         = false
        syncLock            = false

        caller              = null
        relay               = null
        owner               = null

        query               = null
        lastQuery           = null
        search              = null
        result              = ([])
        resultCache         = ([])
        resultFavor         = ([])
        resultRcntDest      = ([])
        resultRcntSearch    = ([])
        requestPageIndex    = null
        reqSearch           = false

        isFavorite          = false
        isViaSetting        = false
        focused             = false
        messageVisible      = false
        textlimitToastShow  = false
        keyboardDown        = false
        showWaitIndicator   = false

        messageText         = CommonConstants.Empty
        highlightColor      = CommonConstants.Transparent

        dlgUniqueNum        = -1
        frameHeight         = 0
        startCount          = 0
        queueCount          = 0
        updateCount         = null

        listModel.clear()
        unloadMainComponent()
    }

    function allocateTextEnum(){
        if (CommonUtils.getObjectLen(textEnum) <= 0) {
            textEnum = CommonUtils.createEnum(textEnum)
        }
        var i = 0
        CommonUtils.insertEnum(textEnum , "NoData"          , i++, [CommonConstants.Name] , [qsTr("검색 결과가 없습니다.")])
        CommonUtils.insertEnum(textEnum , "DialogTitle"     , i++, [CommonConstants.Name] , ["알림"])
        CommonUtils.insertEnum(textEnum , "Dialog0Char"     , i++, [CommonConstants.Name] , ["검색어를 입력 하세요."])
        CommonUtils.insertEnum(textEnum , "Dialog1Char"     , i++, [CommonConstants.Name] , ["초성 검색은 2글자 이상<br>입력하여야 가능합니다."])
        CommonUtils.insertEnum(textEnum , "Text20Length"    , i++, [CommonConstants.Name] , ["검색어는 20자까지 유효합니다."])
        CommonUtils.insertEnum(textEnum , "Slash"           , i++, [CommonConstants.Name] , ["/"])
        CommonUtils.insertEnum(textEnum , "Underbar"        , i++, [CommonConstants.Name] , ["_"])
        CommonUtils.insertEnum(textEnum , "Focus"           , i++, [CommonConstants.Name] , ["focus"])
        CommonUtils.insertEnum(textEnum , "Dot"             , i++, [CommonConstants.Name] , ["."])
        CommonUtils.insertEnum(textEnum , "Png"             , i++, [CommonConstants.Name] , ["png"])


        CommonUtils.fetchArr(textEnum ,  CommonConstants.Name , textArr)
    }

    function allocateSoftKeyEnum() {
        if (CommonUtils.getObjectLen(softKeyEnum) <= 0) {
            softKeyEnum = CommonUtils.createEnum(softKeyEnum)
        }
        var i = 0
        CommonUtils.insertEnum(softKeyEnum ,"Back"           , i++, [CommonConstants.Name] , [ZoneConstants.Back  ])
        CommonUtils.insertEnum(softKeyEnum ,"Option"         , i++, [CommonConstants.Name] , [ZoneConstants.Option])
        CommonUtils.insertEnum(softKeyEnum ,"Navi"           , i++, [CommonConstants.Name] , [ZoneConstants.Navi  ])
        CommonUtils.insertEnum(softKeyEnum ,"Menu"           , i++, [CommonConstants.Name] , [ZoneConstants.Menu  ])

        CommonUtils.fetchArr(softKeyEnum, CommonConstants.Name, softKeyMenuArr)
    }

    function allocateResultTypeEnum(){
        if (CommonUtils.getObjectLen(resultTypeEnum) <= 0) {
            resultTypeEnum = CommonUtils.createEnum(resultTypeEnum)
        }
        var i = 0
        CommonUtils.insertEnum(resultTypeEnum , "Normal"      , SearchConstants.ResultTypeNormal    , [CommonConstants.Image], [CommonConstants.Empty] )
        CommonUtils.insertEnum(resultTypeEnum , "Cache"       , SearchConstants.ResultTypeCache     , [CommonConstants.Image], [CommonConstants.Empty] )
        CommonUtils.insertEnum(resultTypeEnum , "Favor"       , SearchConstants.ResultTypeFavor     , [CommonConstants.Image], ["search/bookmark_star"] )
        CommonUtils.insertEnum(resultTypeEnum , "RcntDest"    , SearchConstants.ResultTypeRcntDest  , [CommonConstants.Image], ["search/bookmark_pin"] )

        CommonUtils.fetchArr(resultTypeEnum, CommonConstants.Image, resultImgArr)
    }

    //////////////////////////////////////
    //{Dialog사용
        function isShowDialog() {
            if (dlgUniqueNum >= 0)
                return true
            return false
        }

        function showDialog(uniqueNum, title,context,cancel) {
            if (CommonUtils.isEqual(dlgUniqueNum,uniqueNum)) {
                return
            }
            dlgUniqueNum = uniqueNum
            var data = ({})
            data[CommonConstants.Url    ] = CommonConstants.url(ViewRegistry.CommonDialogView)
            data[CommonConstants.ParentObjectName] = CommonConstants.ContentRootForCommonDialog
            data[CommonConstants.Title  ] = title
            data[CommonConstants.Message] = context
            data[CommonConstants.Menus  ] = cancel ? [CommonConstants.Cancel, CommonConstants.Ok]
                                                   : [CommonConstants.Ok]
            var type = CommonConstants.Empty
            var s = CommonUtils.split(context, "<br>")
            switch(s.length) {
             case 0:
             case 1:
             case 2:
                 type =  CommonConstants.TypeB
                 break
             case 3:
                 type =  CommonConstants.TypeA
                 break
             default:
                 type =  CommonConstants.TypeC
                 break
            }
            data[CommonConstants.Type   ] = type
            var arg = ({})
            arg [CommonConstants.Caller ] = CommonConstants.objectName(ViewRegistry.TextSearchView)
            data[CommonConstants.Arg    ] = arg
            sendEvent(TextSearchViewEvent.CreateView, data)

            pullDownKeyboard()
        }

        function __closeDialog(yes) {
            closeDialog(dlgUniqueNum, yes)
        }

        function closeDialog(uniqNum, yes) {
            if (yes){
                switch(uniqNum) {
                   default:
                       break
                }
            } else {
                switch(uniqNum) {
                    default:
                        break
                }
            }
            if (CommonUtils.isEqual(uniqNum, dlgUniqueNum)) {
                dlgUniqueNum = -1
            }
        }
    //}Dialog사용
    //////////////////////////////////////

    function changeColor(modelText, searchText) {
        var ret = [CommonConstants.Empty, CommonConstants.Empty]
        if (CommonUtils.isNull(searchText))
            return ret
        var len = searchText.length
        ret[resultTextEnum.matchText ] = CommonUtils.substring(modelText, 0, len)
        ret[resultTextEnum.remainText] = CommonUtils.substring(modelText, len)
        return ret
    }

    onCallerChanged: {
        if (CommonUtils.isNull(caller))
            return
        isViaSetting = CommonUtils.isIndexOf(caller, [CommonConstants.objectName(ViewRegistry.RouteSettingView)])
        isFavorite   = CommonUtils.isIndexOf(caller, [CommonConstants.objectName(ViewRegistry.BookmarkPageView)]) ||
                       CommonUtils.isIndexOf(caller, [CommonConstants.objectName(ViewRegistry.NaviMenuView)])
    }

    onOwnerChanged: {
        if (CommonUtils.isNull(owner))
            return
    }

    function showProgressView() {
        reqSearch = true
        sendEvent(TextSearchViewEvent.ShowProgressIndicator)
        showWaitIndicator = true
    }

    function hideProgressView() {
        sendEvent(TextSearchViewEvent.HideProgressIndicator)
        showWaitIndicator = false
    }

    function stopProgressView() {
        reqSearch = false
        timerProg = CommonUtils.destroy(timerProg)
        hideProgressView()
    }

    function getResultOnline() {
        if (CommonUtils.isNull(result))
            return online
        for(var i = 0; i< result.length; i++) {
            if (CommonUtils.isEqual(result[i][SearchConstants.ResultType],  resultTypeEnum.Normal)) {
                return result[i][SearchConstants.ResultOnline]
            }
        }
        return online
    }

    function requestRecentSearch() {
        var data = ({})
        data[CommonConstants.Offset] = 0
        data[CommonConstants.Max] = SearchConstants.RequestMaxCount
        sendEvent(TextSearchViewEvent.RequestRecentSearch)
    }

    function requestLocalSearch(search, result){
        initSearch()
        requestSearch(CommonUtils.isNull(lastQuery) ? search : lastQuery,
                      0, listView.maxCount, CommonUtils.isEqual(search, lastQuery)? false : online)
    }

    function requestSearch(query, requestPageIndex, maxCount, online) {
        if (CommonUtils.isEqual(searchRoot.query, query) &&
            CommonUtils.isEqual(searchRoot.requestPageIndex, requestPageIndex)) {
            if (query.length > 0)
                return
        }
        //20자 넘게 입력되어 들어오는 경우 검색을 하지 않도록 수정
        var text = SearchConstants.normalized(query)
        if (text.length > searchMaxLen) {
            if (!textlimitToastShow ) {
                textlimitToastShow = true
                showToast(textArr[textEnum.Text20Length])
            }
            query = getLimitText(text, searchMaxLen)
        }
        loadMainComponent()
        searchRoot.query = query
        searchRoot.requestPageIndex = requestPageIndex
        var temp = CommonUtils.trim(query)
        if (CommonUtils.isEmpty(temp) || temp.length <= 0) {
            //최근검색어 요청
            if (isViaSetting || isFavorite)  {
                processResult(CommonConstants.Empty,resultRcntSearch,resultRcntSearch.length, 0)
                return
            }
            visibleKeyboard(false, false, CommonConstants.Empty)
            backToSearchTabView()
            return
        }
        sendEvent(TextSearchViewEvent.RemoveSearchQueue)
        showProgressView()

        var params = ({})
        params[SearchConstants.StartIndex] = (requestPageIndex * listView.maxCount)
        params[SearchConstants.Count     ] = maxCount
        params[SearchConstants.CacheCount] = requestCacheMaxCount
        params[SearchConstants.FavorCount] = requestFavorMaxCount
        params[SearchConstants.DestiCount] = requestRdestMaxCount
        params[SearchConstants.AddrCode  ] = 0
        params[SearchConstants.SearchType] = online
        var data =({})
        data[SearchConstants.Query       ] = query
        data[SearchConstants.SearchType  ] = CommonConstants.Name
        data[SearchConstants.AddressType ] = addressType
        //부가 정보
        data[SearchConstants.Params      ] = params
        sendEvent(TextSearchViewEvent.RequestSearch, data )

        //[주의] 검색 요청이 블럭되는 경우 처리를 위하여 ...
        queueChanged(0)
    }

    function getStdName(result) {
        var titleName = result[SearchConstants.ResultStdName]
        if (CommonUtils.isEmpty(titleName))
            titleName = result[SearchConstants.ResultName]
        return titleName
    }

    function getName(result) {
        var titleName = result[SearchConstants.ResultName]
        if (CommonUtils.isEmpty(titleName))
            titleName = result[SearchConstants.ResultStdName]
        return titleName
    }


    function queryInDriveMode() {
        var index = listView.currentIndex
        if (index < 0)  return
        var data = ({})
        data[CommonConstants.X          ] = result[index][SearchConstants.ResultX]
        data[CommonConstants.Y          ] = result[index][SearchConstants.ResultY]
        data[CommonConstants.Name       ] = getStdName(result[index])
        data[CommonConstants.Address    ] = result[index][SearchConstants.ResultAddress]
        data[SearchConstants.PoiID      ] = result[index][SearchConstants.ResultPOIID]
        data[SearchConstants.NavSeq     ] = result[index][SearchConstants.ResultNavseq]
        data[SearchConstants.RpFlag     ] = result[index][SearchConstants.ResultRpFlag]
        data[SearchConstants.OpiPoleCate] = result[index][SearchConstants.ResultOPIPoleCate]
        sendEvent(TextSearchViewEvent.QueryInDriveMode, data)
    }


    function isHangulChar(c) {
         if( 0x1100<=c && c<=0x11FF ) return true;
         if( 0x3130<=c && c<=0x318F ) return true;
         if( 0xAC00<=c && c<=0xD7A3 ) return true;
         return false;
    }

    function getSyllables(c) {
        var choArr  = [ "ㄱ", "ㄲ", "ㄴ", "ㄷ", "ㄸ", "ㄹ", "ㅁ", "ㅂ", "ㅃ", "ㅅ", "ㅆ", "ㅇ", "ㅈ", "ㅉ", "ㅊ", "ㅋ", "ㅌ", "ㅍ", "ㅎ" ]
        var jungArr = [ "ㅏ", "ㅐ", "ㅑ", "ㅒ", "ㅓ", "ㅔ", "ㅕ", "ㅖ", "ㅗ", "ㅘ", "ㅙ", "ㅚ", "ㅛ", "ㅜ", "ㅝ", "ㅞ", "ㅟ", "ㅠ", "ㅡ", "ㅢ", "ㅣ" ]
        var jongArr = [ "", "ㄱ", "ㄲ", "ㄳ", "ㄴ", "ㄵ", "ㄶ", "ㄷ", "ㄹ", "ㄺ", "ㄻ", "ㄼ", "ㄽ", "ㄾ", "ㄿ", "ㅀ", "ㅁ", "ㅂ", "ㅄ", "ㅅ", "ㅆ", "ㅇ", "ㅈ", "ㅊ", "ㅋ", "ㅌ", "ㅍ", "ㅎ" ]
        var code = c - 0xAC00
        var jong = code % 28
        var jung = ((code - jong) / 28 ) % 21
        var cho = (((code - jong) / 28 ) - jung ) / 21
        var chars = []
        if (cho >= 0) {
            chars.push(choArr[cho])
        }
        if (jung >= 0) {
            chars.push(jungArr[jung])
        }
        if (jong >= 0) {
            if (CommonUtils.isEmpty(jongArr[jong])) {
              chars.push(jongArr[jong])
            }
        }
        return chars
    }

    function getLimitText(text, maxLen) {
        if (text.length > maxLen) {
            text = CommonUtils.substring(text, 0, maxLen)
        }
        return text
    }

    function showSearchResultView(text, fromSearchBtn){
        if (reqSearch) {
            return
        }
        text = getLimitText(text, searchMaxLen)
        //초성 1개 입력후 검색 버튼 Tab시
        text = CommonUtils.trim(text)
        if (fromSearchBtn && text.length <= 1) {
            if (text.length <= 0) {
                return
            } else {
                var c = text.charCodeAt(0)
                var isHan = isHangulChar(c)
                if (isHan) {
                    var syllable = getSyllables(c)
                    if (syllable.length <= 1)  {
                        showDialog(textEnum.Dialog1Char, textArr[textEnum.DialogTitle],textArr[textEnum.Dialog1Char],false)
                        return
                    }
                }
            }
        }

        if (isSyncLock()) {
            return
        }
        setSyncLock(true)

        var isFavorite = CommonUtils.isIndexOf(owner, [CommonConstants.objectName(ViewRegistry.BookmarkPageView)])
        var isHome     = CommonUtils.isIndexOf(owner, [textArr[textEnum.Slash] + SearchConstants.Home])
        var isOffice   = CommonUtils.isIndexOf(owner, [textArr[textEnum.Slash] + SearchConstants.Office])


        var data = ({})
        data[CommonConstants.Text] = text

        if (isViaSetting) {
           data = ({})
           data[CommonConstants.ObjectName  ] = CommonConstants.objectName(ViewRegistry.RouteSettingView)
           data[CommonConstants.Visible     ] = false
           sendEvent(TextSearchViewEvent.RouteSettingUpdateView, data)
        }

        data = ({})
        data[CommonConstants.ObjectName ] = CommonConstants.objectName(ViewRegistry.SearchResultView)
        sendEvent(TextSearchViewEvent.CloseView, data)

        data = ({})
        data[CommonConstants.Url        ] = CommonConstants.url(ViewRegistry.SearchResultView)
        data[CommonConstants.Caller     ] = caller
        data[CommonConstants.Relay      ] = owner
        data[CommonConstants.Owner      ] = CommonConstants.objectName(ViewRegistry.TextSearchView)
        data[CommonConstants.Visible    ] = fromSearchBtn ? SearchConstants.DialogNoResult : SearchConstants.ViewNoResult
        data[SearchConstants.Favorite   ] = isFavorite
        data[SearchConstants.Query      ] = text
        data[SearchConstants.IsHome     ] = isHome
        data[SearchConstants.IsOffice   ] = isOffice
        data[SearchConstants.ViaSetting ] = isViaSetting
        data[SearchConstants.SearchType ] = CommonConstants.Name

        //data[CommonConstants.Z          ] = CommonConstants.MAX_INT_VALUE
        data[CommonConstants.ReFocus    ] = true
        data[CommonConstants.ObjectName ] = CommonConstants.objectName(ViewRegistry.SearchResultView)
        sendEvent(TextSearchViewEvent.UpdateView, data )

        console.log("[Perf][TextSearchView][CreateView]");
        sendEvent(TextSearchViewEvent.CreateView, data )
    }


    function showKeyboard(defaultKeyboard) {
        setKeyboardHeight(keyboardHeight)
        var data = ({})
        data[CommonConstants.Owner                ] = CommonConstants.objectName(ViewRegistry.TextSearchView)
        data[CommonConstants.X                    ] = keyboard.x
        data[CommonConstants.Y                    ] = keyboard.y
        data[CommonConstants.Z                    ] = keyboard.z + 1
        data[CommonConstants.Width                ] = keyboard.width
        data[CommonConstants.Height               ] = keyboard.height
        if (defaultKeyboard)
            data[CommonConstants.Pressed          ] = CustomKeyboard.hangleKeybd
        sendEvent(TextSearchViewEvent.ShowKeyboard, data)
    }

    function visibleKeyboard(visible, defaultKeyboard, text) {
        if (visible) {
            showKeyboard(defaultKeyboard)
        } else {
            setKeyboardHeight(0)
            var data = ({})
            data[CommonConstants.Owner         ] = CommonConstants.objectName(ViewRegistry.TextSearchView)
            if (!CommonUtils.isNull(text))
                data[CommonConstants.Text      ] = text
            data[CommonConstants.DontUseEditBox] = true
            sendEvent(TextSearchViewEvent.HideKeyboard, data)
        }
    }

    function back() {
        var closeSearchTabView = true

        goBack(closeSearchTabView)

        sendEvent(TextSearchViewEvent.BackClicked)
    }
    function backToSearchTabView() {
        var closeSearchTabView = false

        goBack(closeSearchTabView)
    }

    function goBack(closeSearchTabView) {
        sendEvent(TextSearchViewEvent.RemoveSearchQueue)
        var data = null
        data = ({})
        data[CommonConstants.ObjectName ] = CommonConstants.objectName(ViewRegistry.SearchResultView)
        sendEvent(TextSearchViewEvent.CloseView, data)
        if (isFavorite) {
            if (CommonUtils.isIndexOf(caller, [CommonConstants.objectName(ViewRegistry.NaviMenuView)]))
                sendEvent(TextSearchViewEvent.ReturnToNaviMenu)
            else
                sendEvent(TextSearchViewEvent.ReturnToFavorite)
        } else if (isViaSetting) {
            data = ({})
            data[CommonConstants.ObjectName ] = CommonConstants.objectName(ViewRegistry.RouteSettingView)
            data[CommonConstants.Visible    ] = true
            sendEvent(TextSearchViewEvent.RouteSettingUpdateView, data)

            data = ({})
            data[CommonConstants.ObjectName ] = CommonConstants.objectName(ViewRegistry.SearchTabView)
            sendEvent(TextSearchViewEvent.CloseView, data)
        } else {
            if (closeSearchTabView) {
                data = ({})
                data[CommonConstants.ObjectName ] = CommonConstants.objectName(ViewRegistry.SearchTabView)

                sendEvent(TextSearchViewEvent.CloseView, data)
            } else {
                sendEvent(TextSearchViewEvent.ChangeSearchTabViewPage, SearchConstants.RecentSearchPageIndex)
            }
        }

        closeView()
    }

    function sendUpdateSearchResultCount(search, totalCount) {
        if (CommonUtils.isNull(search) ||
            CommonUtils.isNull(totalCount) ||
            keyboardDown) {
            return
        }
        var data = ({})
        data[CommonConstants.Text] = search
        data[CommonConstants.Count] = updateCount = totalCount
        sendEvent(TextSearchViewEvent.UpdateSearchResultCount, data)
    }

    function createModel(result, startIndex, totalCount, addIndex){
        if (startIndex <= 0) {
            listModel.clear()

            listView.requestPageIndex = 0
            listView.contentHeight = 0
            listView.contentY = 0
            listView.currentIndex = searchRoot.defaultCurrentIndex
            listView.totalCount = 0
            listView.totalPage = 0

            searchRoot.result = []
        }

        if (startIndex <= 0 || !CommonUtils.isEqual(listView.totalCount , totalCount)) {
            listView.totalCount = totalCount
            listView.totalPage = totalCount / listView.maxCount
            listView.totalPage += (totalCount % listView.maxCount) ? 1 : 0
        }


        var len = (result) ? result.length : 0
        for (var i = 0; i< len; i++) {
            var index =  startIndex   +  addIndex + i
            var name =  getName(result[i])
            name = CommonUtils.trim(name)
            if (CommonUtils.isNull(name) ||
                CommonUtils.isEmpty(name)) {
               searchRoot.result[index] = result[i]
               continue
            }

            var kind = result[i][SearchConstants.ResultType]
            if ( listModel.count >  index) {
                listModel.set (index, {text: name, type: kind})
            } else {
                listModel.append( { text: name, type: kind})
            }
            searchRoot.result[index] = result[i]
        }

        if (!keyboardDown) {
            visibleKeyboard(visible, false, undefined)
        }

        sendUpdateSearchResultCount(query, totalCount)
        //검색이 되었을 경우만 검색 버튼 활성화
        //showKeyboard(listModel.count ? true : false)

        setKeyboardHeight(keyboard.height)
    }

    function showToast(message)   {
        var data = ({})
        data[CommonConstants.Message] = message
        data[CommonConstants.Position] = CommonConstants.ToastUp
        data[CommonConstants.Duration] = 3 * 1000
        sendEvent(TextSearchViewEvent.ShowToast ,data)
    }

    function destruct() {
        timerProg = CommonUtils.destroy(timerProg)
        timerClick = CommonUtils.destroy(timerClick)
        hideProgressView()
    }

    function closeAllView() {
        var data = ({})
        if (!CommonUtils.isNull(owner)) {
          data[CommonConstants.ObjectName] = owner
          sendEvent(TextSearchViewEvent.CloseView, data)
        }
        if (!CommonUtils.isNull(relay)) {
          data[CommonConstants.ObjectName] = relay
          sendEvent(TextSearchViewEvent.CloseView, data)
        }
        if (!CommonUtils.isNull(caller)) {
          data[CommonConstants.ObjectName] = caller
          sendEvent(TextSearchViewEvent.CloseView, data)
        }
        closeView()
    }

    function close() {
        if (searchRoot.visible) {
            viewDeactivated()
        }
        destruct()
        searchRoot.visible = false

        //1.호출뷰에 focus 요청
        if (!CommonUtils.isEmpty(owner)) {
            var data = ({})
            data[CommonConstants.ReFocus   ] = true
            data[CommonConstants.ObjectName] = owner
            sendEvent(TextSearchViewEvent.UpdateView, data)
        }
        //2.변수초기화 (caller,owner,relay... 포함)
        allocateVariant()
        isClosed = true
    }

    function closeView() {
        var data = ({})
        data[CommonConstants.ObjectName] = CommonConstants.objectName(ViewRegistry.TextSearchView)
        sendEvent(TextSearchViewEvent.CloseView, data)
    }

    function __processKeyboard(key, text) {
        if (isSyncLock()) {
            return
        }
        console.log("[Perf][TextSearchView][__processKeyboard]key: " + key + " text: " + text)
        var normalizeText = null
        switch(key) {
        case CustomKeyboard.CodeCancel:
            visibleKeyboard(false, false, CommonConstants.Empty)
            back()
            break
        case CustomKeyboard.CodeSymbol:     break
        case CustomKeyboard.CodeKorean:     break
        case CustomKeyboard.CodeEnglish:    break
        case CustomKeyboard.CodeShift:      break
        case CustomKeyboard.CodeClear:      break
        case CustomKeyboard.CodeSetting:    break
        case CustomKeyboard.CodeSearch:     break
        case CustomKeyboard.CodeConfirm:
            if (!reqSearch) {
                console.log("[Perf][TextSearchView][SearchConstants.normalize]")
                normalizeText = SearchConstants.normalized(text)
                textlimitToastShow = false
                showSearchResultView(normalizeText, true)
            }
            break
        case CustomKeyboard.CodeSpace:
        case CustomKeyboard.CodeDelete:
        default:
            console.log("[Perf][TextSearchView][__processKeyboard]SearchConstants.normalize")
            normalizeText = SearchConstants.normalized(text)
            if (normalizeText.length > searchMaxLen) {
                if (!textlimitToastShow &&
                    !CommonUtils.isEqual(CustomKeyboard.CodeDelete, key)) {
                    textlimitToastShow = true
                    showToast(textArr[textEnum.Text20Length])
                }
            } else {
                console.log("[Perf][TextSearchView][__processKeyboard]requestSearch")
                lastQuery = text
                requestSearch(text, 0, listView.maxCount, online)
            }
            break
        }
    }

    function unloadMainComponent() {
        loader.sourceComponent = undefined
    }

    function loadMainComponent() {
        if (CommonUtils.isNull(loader.sourceComponent)) {
          loader.sourceComponent = main
        }
    }

    function initEnum() {
        allocateTextEnum()
        allocateSoftKeyEnum()
        allocateResultTypeEnum()
    }

    function initSearch() {
        query = null
        requestPageIndex = null
    }

    function init(){
        initEnum()
        unloadMainComponent()
    }

    function requestNrefresh() {
        isClosed = false
        loadMainComponent()
        if (!isActivated) {
            isActivated = true
            if(CommonUtils.isEmpty(query)) {
                if (isViaSetting || isFavorite)  {
                    requestRecentSearch()
                }
            } else {
                console.log("[Perf][TextSearchView][onActivated]requestSearch")
                lastQuery = query
                requestSearch(query, 0, listView.maxCount, online)
            }
        }

        var data = ({})
        if (searchRoot.visible) {
            sendEvent(TextSearchViewEvent.ResetCsw)
            sendEvent(TextSearchViewEvent.RegistCsw, [ csw ])
            sendEvent(TextSearchViewEvent.Activated)
            if(CommonUtils.isEmpty(query)) {
                data[CommonConstants.Text] = CommonConstants.Empty
                sendEvent(TextSearchViewEvent.SelectZone1TextInput, data)
            } else {
                sendEvent(TextSearchViewEvent.SelectZone1TextInput)
                if (listModel.count > 0) {
                    sendUpdateSearchResultCount(query, updateCount)
                }
            }
            if (!keyboardDown) {
                var buttonEnable = (!CommonUtils.isEmpty(query) && listModel.count) ? false : true
                visibleKeyboard(visible, buttonEnable, undefined)
            }
            setKeyboardHeight(keyboard.height)
        }
    }

    onReFocusChanged: {
        if (!reFocus)
            return
        reFocus = false
        if (!isMediated)
            return
        if (searchRoot.visible) {
            if (!active) {
              //viewActivated()
            }
        } else {
           onInit()
        }
    }

    function resetHideKeyboard() {
        visibleKeyboard(false, false, undefined)
    }

    onVisibleChanged: {
        if (visible) {
            return
        }
        resetHideKeyboard()
    }

    onMediated: {
        onInit()
    }

    onActivated: {
        setSyncLock(false)
        if (!visible) {
            return
        }
        sendEvent(TextSearchViewEvent.RegistKeyboardInput)
        requestNrefresh()
    }

    onDeactivated: {
        sendEvent(TextSearchViewEvent.Deactivated)
        csw.forceLostFocus()
    }

    Component.onDestruction: {
        destruct()
    }

    function onInit() {
        console.log("[Perf][TextSearchView][onInit]")
        isClosed = false
        if (!isMediated) {
            isMediated = true
            sendEvent(TextSearchViewEvent.RequestLang)
            sendEvent(TextSearchViewEvent.QueryOnlineStatus)
            sendEvent(TextSearchViewEvent.RequestAddressType)
            sendEvent(TextSearchViewEvent.RequestRouteGuidanceState)
            init()
        }
        if (!searchRoot.visible) {
            if (CommonUtils.isNull(searchRoot.query)) {
                closeView()
            } else {
               requestNrefresh()
            }
        }
    }

    //검색 요청을 갯수를 얻음
    function __queueChanged(count) {
        queueChanged(count)
    }

    function queueChanged(count) {
          queueCount = count
          timerProg = CommonUtils.destroy(timerProg)
          if (!reqSearch)
              return
          if (count<=0) {
              timerProg = CommonUtils.delay(searchRoot, SearchConstants.NetworkTimeoutAPP, function() {
                stopProgressView()
              }, false)
          }
    }

    function __updatedUserProfile() {
        sendEvent(TextSearchViewEvent.RequestAddressType)
    }

    function __changedUserProfile() {
        sendEvent(TextSearchViewEvent.RequestAddressType)
    }

    function __noResult() {
        var data = ({})
        if (CommonUtils.isIndexOf(caller, [CommonConstants.objectName(ViewRegistry.SearchTabView)])) {
            sendEvent(TextSearchViewEvent.ChangeSearchTabViewPage, SearchConstants.NoResultPageIndex)
            closeView()
        } else {
            messageText = textArr[textEnum.NoData]
            messageVisible = true
        }
    }

    //캐시 검색 결과
    function __onResultCache(search, result, totalCount, startIndex) {
        startIndex = Math.max(0, startIndex)
        if(startIndex <= 0)
            resultCache = []
        var resultLength = Math.min(requestCacheMaxCount, totalCount)
        for(var i = 0; i<resultLength; i++) {
            result[i][SearchConstants.ResultType] = resultTypeEnum.Cache
            var index = startIndex + i
            resultCache[index] = result[i]
        }
    }

    //즐겨찾기 검색 결과
    function __onResultFavor(search, result, totalCount, startIndex) {
        startIndex = Math.max(0, startIndex)
        if (startIndex <= 0)
            resultFavor = []
        var resultLength = Math.min(requestFavorMaxCount, totalCount)
        for(var i = 0; i<resultLength; i++) {
            result[i][SearchConstants.ResultType] = resultTypeEnum.Favor
            var index = startIndex + i
            resultFavor[index] = result[i]
        }
    }

    //최근목적지 검색 결과
    function __onResultRcntDest(search, result, totalCount, startIndex) {
        startIndex = Math.max(0, startIndex)
        if(startIndex <= 0)
            resultRcntDest = []
        var resultLength = Math.min(requestRdestMaxCount, totalCount)
        for(var i = 0; i<resultLength; i++) {
            result[i][SearchConstants.ResultType] = resultTypeEnum.RcntDest
            var index = startIndex + i
            resultRcntDest[index] = result[i]
        }
    }

    //오프라인 자동완성 검색 결과
    function __onResultOffline(search, result, totalCount, startIndex) {
        if (isSyncLock()) {
            return
        }
        console.log('[Perf][TextSearchView][__onResultOffline]before'+search+')')
        processResult(search, result, totalCount, startIndex)
        console.log('[Perf][TextSearchView][__onResultOffline]after')
    }

    //온라인 자동완성 검색 결과
    function __onResultOnline(search, result, totalCount, startIndex, errcode) {
        if (isSyncLock()) {
            return
        }
        console.log('[Perf][TextSearchView][__onResultOnline]before('+search+','+errcode+')')
        if (errcode) {
            console.log('[Perf][TextSearchView][__onResultOnline]after')
            requestLocalSearch(search, result)
            return
        }
        processResult(search, result, totalCount, startIndex)
        console.log('[Perf][TextSearchView][__onResultOnline]after')
    }

    function processResult(search, result, totalCount, startIndex) {
         if (CommonUtils.isEqual(searchRoot.query, search))
            stopProgressView()

        messageText = CommonConstants.Empty
        messageVisible = false

        startIndex = Math.max(0, startIndex)
        var i = null
        var resultBuff = []
        if (startIndex <= 0) {
            searchRoot.search = SearchConstants.normalized (search)

            for(i = 0; i<resultFavor.length; i++  ) {
                resultBuff[resultBuff.length] = resultFavor[i]
            }

            for(i = 0; i < resultRcntDest.length; i++ ) {
                resultBuff[resultBuff.length] = resultRcntDest[i]
            }

            if(totalCount <= 0  &&
               resultBuff.length <= 0) {
                for(i = 0; i<resultCache.length; i++  ) {
                    resultCache[i] = SearchConstants.resultInit()
                    resultCache[i][SearchConstants.ResultName   ] = searchRoot.search
                    resultCache[i][SearchConstants.ResultStdName] = searchRoot.search
                    resultCache[i][SearchConstants.ResultType   ] = resultTypeEnum.Cache
                    resultBuff[resultBuff.length] = resultCache[i]
                }
            }

            startCount = resultBuff.length
        }

        for(i = 0; i< result.length; i++) {
            result[i][SearchConstants.ResultType] = resultTypeEnum.Normal
            resultBuff[resultBuff.length] = result[i]
        }

        var addIndex = (startIndex > 0) ? startCount : 0

        totalCount = (totalCount + startCount)
        createModel(resultBuff, startIndex, totalCount, addIndex)

        if (!searchRoot.visible) {
            searchRoot.visible = true
        }
    }

    function __onResultRecentSearch(result, cnt) {
        resultRcntSearch = []
        var i = 0
        for (var p in result) {
            var dest = result[p]
            resultRcntSearch[i] = SearchConstants.resultInit()
            resultRcntSearch[i][SearchConstants.ResultName] = dest[1]
            resultRcntSearch[i][SearchConstants.ResultType] = resultTypeEnum.Normal
            i++
        }
        processResult(CommonConstants.Empty,resultRcntSearch,cnt, 0)
    }

    //언어가 변경되었을 때
    function __retranslateUi(lang) {
        searchRoot.__lang = lang
        init()
        if(searchRoot.visible) {
           //viewActivated()
        }
    }

    function __changedOnlineStatus(online) {
        searchRoot.online = online
    }

    function __changedRgState(isRg) {
        searchRoot.isRg = isRg
    }

    function __changedSyncLock(lock) {
        setSyncLock(lock)
    }

    function isSyncLock() {
        return syncLock
    }

    function setSyncLock(lock) {
        syncLock = lock
    }

    function __doSoftKeyFunction(button, menuName, menuIndex) {
        switch(button) {
          case softKeyMenuArr[softKeyEnum.Back]:
              back()
              break
          case softKeyMenuArr[softKeyEnum.Option]:
          case softKeyMenuArr[softKeyEnum.Navi]:
          case softKeyMenuArr[softKeyEnum.Menu]:
          default:
              break
        }
    }

    function pullDownKeyboard() {
        if (!visible)
            return
        keyboardDown = true
        visibleKeyboard(false, false, undefined)
    }

    function pullUpKeyboard() {
        if (!visible)
            return
        keyboardDown = false
        setKeyboardHeight(keyboardHeight)
        visibleKeyboard(true, false, undefined)
    }

    function __pullDownKeyboard() {
        pullDownKeyboard()
    }

    function __pullUpKeyboard() {
        pullUpKeyboard()
    }

    //main
    Loader {
         id: loader
         width: parent.width
         height: parent.height + GlobalConstants.kScreenZone3Height
    }

    Component {
        id: main
        Rectangle{
            onParentChanged: {
                if (CommonUtils.isNull(parent))
                    return
                width = parent.width
                height = parent.height
            }
            color: GlobalConstants.kBasicBackgoundColor
            Column {
                width: parent.width
                height: parent.height
                ListView {
                    Component.onCompleted: searchRoot.listView = this
                    width: parent.width
                    height: messageVisible? 0 : frameHeight - (keyboardDown ? GlobalConstants.kScreenZone3Height : 0)
                    property int rowHeight: searchRoot.layout.rowHeight
                    property int maxCount: SearchConstants.RequestCount
                    property int totalPage: 0
                    property int totalCount: 0
                    property int requestPageIndex: 0

                    //cacheBuffer: rowHeight * 3
                    clip: false
                    model: listModel
                    delegate: listDelegate
                    currentIndex: searchRoot.defaultCurrentIndex
                    visible: !messageVisible

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
                        color: highlightColor
                    }
                    highlightFollowsCurrentItem: false

                    Image {
                          id: imgMask
                          width: parent.width
                          height: 30
                          y: parent.y + (parent.height - height)
                          visible:  scrollbar.heightRatio < 1.0 && !listView.dragging
                          sourceSize.width: width
                          sourceSize.height: height
                          source: Resource.imageUrl("zone/zone2_img_mask.png")
                    }

                    TScrollBar {
                        id: scrollbar
                        flickable: parent
                        width: 2
                        dynamicPage: listView.requestPageIndex
                        color: MultiSenseCoordinator.kAmbientColor
                        grooveColor: GlobalConstants.kScrollBarBackColor
                        rightMargin: GlobalConstants.kScrollBarRightMargin
                    }

                    onAtYEndChanged: {
                        if (!scrollbar.visible) {
                            return
                        }
                        if (!atYEnd) {
                             return
                        }
                        cancelFlick()
                        if (CommonUtils.isNull(listView)                ||
                            CommonUtils.isNull(listView.totalPage)      ||
                            CommonUtils.isNull(listView.requestPageIndex) ) {
                            return
                        }
                        if ( (listView.totalPage - 1) <= listView.requestPageIndex) {
                            return
                        }
                        if (CommonUtils.isEmpty(searchRoot.search)) {
                             return
                        }
                        console.log("[Perf][TextSearchView][onAtYEndChanged]requestSearch")
                        requestSearch(searchRoot.query, ++listView.requestPageIndex, listView.maxCount, getResultOnline())
                    }
                    onMovementStarted: {
                        sendEvent(TextSearchViewEvent.PulldownKeyboard)
                    }
                }
                Item { width:parent.width;
                       height: messageVisible ? frameHeight - (keyboardDown ? GlobalConstants.kScreenZone3Height : 0) : 0
                    Text {
                        Component.onCompleted: searchRoot.message = this
                        text: messageText
                        visible: messageVisible
                        anchors.centerIn:  parent
                        font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(GlobalConstants.kGeneralMessageFontSize) }
                        color: GlobalConstants.kListTextColor
                    }
                }
                Item {
                    Component.onCompleted: searchRoot.keyboard = this
                    width: parent.width
                    height: keyboardHeight
                }
            }
        }
    }

    ListModel { id: listModel   }

    Component {
        id: listDelegate
        Column{
            width: listView.width
            height: listView.rowHeight
            property bool isFocused: CommonUtils.isEqual(listView.currentIndex , model.index) && searchRoot.focused
            function clickItem() {
                if (!isFocused) {
                    highlightColor = Qt.binding(function(){ return MultiSenseCoordinator.kAmbientColor})
                }
                listView.currentIndex = model.index
                if (CommonUtils.isEqual(resultImgArr[model.type] , CommonConstants.Empty) || isViaSetting || isFavorite) {
                    showSearchResultView(model.text, false)
                } else {
                    queryInDriveMode()
                    if (!isRg) {
                        closeAllView()
                    }
                }
            }

            Item {
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

                    onClicked: clickItem()
                }
                Row {
                    width: parent.width
                    height: parent.height
                    Item {width: 20; height:parent.height}
                    Item {width:parent.width -(20+40); height:parent.height
                        Loader {
                           width: parent.width
                           height:parent.height
                           sourceComponent:  CommonUtils.isEqual(resultImgArr[model.type] , CommonConstants.Empty) ? normalStyle : imgStyle
                        }
                    }
                    Item {width: 40; height:parent.height}
                }
            }
            Rectangle {
               id: splitBar
               x: GlobalConstants.kListLineLeftMargin
               width: parent.width - (GlobalConstants.kListLineLeftMargin  + GlobalConstants.kListLineRightMargin)
               height: 1
               color: GlobalConstants.kListLineColor
            }

            //1.일반
            Component {
                id: normalStyle
                Row {
                    width: parent.width
                    height: parent.height
                    Text {
                        height: parent.height
                        verticalAlignment: Qt.AlignVCenter
                        font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(GlobalConstants.kOneLineListTextFontSize) }
                        color: (isFocused) ? GlobalConstants.kPressedColor :  CommonUtils.isEqual(listView.currentIndex , model.index) ? "#1781c5" : MultiSenseCoordinator.kAmbientColor
                        text: {
                            if (CommonUtils.isNull(model) || CommonUtils.isNull(model.text))
                                return CommonConstants.Empty
                            return changeColor(model.text, search)[resultTextEnum.matchText]
                        }
                    }
                    Text {
                        height: parent.height
                        verticalAlignment: Qt.AlignVCenter
                        clip: true
                        font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(GlobalConstants.kOneLineListTextFontSize) }
                        color: (isFocused) ? GlobalConstants.kBasicBackgoundColor : GlobalConstants.kListTextColor
                        text: {
                            if (CommonUtils.isNull(model) || CommonUtils.isNull(model.text))
                                return CommonConstants.Empty
                            return changeColor(model.text, search)[resultTextEnum.remainText]
                        }
                    }
                }
            }//Component


            //2.즐겨찾기 및 최근목적지가 포함된 자동완성
            Component {
                id: imgStyle
                Row {
                    width: parent.width
                    height: parent.height
                    Item {
                        width: 63
                        height: 63
                        anchors.verticalCenter: parent.verticalCenter
                        Image {
                            clip: true
                            width: parent.width
                            height: parent.height
                            sourceSize.width: width
                            sourceSize.height: height
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.horizontalCenter: parent.horizontalCenter
                            source: {
                                var imgUrl = null
                                if (isFocused) {
                                    imgUrl = resultImgArr[model.type]
                                            + textArr[textEnum.Underbar]
                                            + textArr[textEnum.Focus]
                                } else {
                                    imgUrl = resultImgArr[model.type]
                                }
                                imgUrl += textArr[textEnum.Dot]
                                imgUrl += textArr[textEnum.Png]
                                return Resource.imageUrl(imgUrl)
                            }
                        }
                    }
                    Item {width: 20; height:parent.height}
                    Loader {
                       width: parent.width -(63+20)
                       height:parent.height
                       sourceComponent: normalStyle
                    }
                }
            }//Component
        }//Column
    }//Component


    Csw {
            id: csw

            section: Csw.Section_List
            sequence: 0
            enable: searchRoot.visible


            onFocused: {
                searchRoot.focused = true
                highlightColor = GlobalConstants.kListFocusColor
                listView.currentIndex = -1
                listView.currentIndex = 0
            }
            onLostFocusd: {
                highlightColor = CommonConstants.Transparent
                searchRoot.focused = false
            }
            onLeftPressed: selected()
            onRightPressed: selected()
            onSpinCwPressed: {
                if (listView.currentIndex < listView.count - 1) {
                    ++listView.currentIndex
                    var item = listView.currentItem
                    highlightColor = GlobalConstants.kListFocusColor
                }
            }
            onSpinCcwPressed: {
                if (0 < listView.currentIndex) {
                    --listView.currentIndex
                    var item = listView.currentItem
                    highlightColor = GlobalConstants.kListFocusColor
                }
            }
            onSelected: {
                var item = listView.currentItem
                item.clickItem()
            }
        }
}
