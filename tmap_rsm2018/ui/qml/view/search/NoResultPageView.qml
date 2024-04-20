import QtQuick 2.0
import ViewRegistry 1.0
import CommonConstants 1.0
import ZoneConstants 1.0
import SearchConstants 1.0
import NoResultPageViewEvent 1.0
import SearchTabViewEvent 1.0
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

    property var caller             : null
    property int index              : 0
    property int keyboardHeight     : 419
    property bool isCompleted       : false
    property string query
    property bool dontUseEditBox    : false
    property bool keyboardVisible   : false

    //언어 관련
    property var __lang             : null
    property bool needInit          : true

    //ID
    property var keyboard           : null

    //Enum
    property var textEnum           : ({})

    property var textArr            : ([])

    function allocateTextEnum(){
        if (CommonUtils.getObjectLen(textEnum) <= 0) {
            textEnum = CommonUtils.createEnum(textEnum)
        }
        var i = 0
        CommonUtils.insertEnum(textEnum , "NoData" , i++, [CommonConstants.Name] , [qsTr("검색 결과가 없습니다.")])

        CommonUtils.fetchArr(textEnum ,  CommonConstants.Name , textArr)
    }


    function showView( args )   {
        sendEvent(NoResultPageViewEvent.CreateView, args)
    }

    function closeView(index) {

        isCompleted = false
        hideKeyboard()

        sendEvent(NoResultPageViewEvent.ActivatedForNavi)
        var data = ({})
        data[CommonConstants.ObjectName] = CommonConstants.objectName(ViewRegistry.SearchTabView)
        sendEvent(NoResultPageViewEvent.CloseView, data)
    }

    function __processKeyboard(key, text) {
        switch(key) {
        case CustomKeyboard.CodeCancel:
            closeView(searchRoot.index)
            break
        case CustomKeyboard.CodeSymbol:     break
        case CustomKeyboard.CodeKorean:     break
        case CustomKeyboard.CodeEnglish:    break
        case CustomKeyboard.CodeShift:      break
        case CustomKeyboard.CodeClear:      break
        case CustomKeyboard.CodeSetting:    break
        case CustomKeyboard.CodeSearch:     break
        case CustomKeyboard.CodeConfirm:    break
        case CustomKeyboard.CodeSpace:
        case CustomKeyboard.CodeDelete:
        default:
            if(text.length > 0) {
                showTextSearchView(text)
            }  else {
                sendEvent(NoResultPageViewEvent.ChangeSearchTabViewPage, SearchConstants.RecentSearchPageIndex)
            }
            break
        }
    }

    function showTextSearchView(text){
        var data = ({})
        data[CommonConstants.Url        ] = CommonConstants.url(ViewRegistry.TextSearchView)
        data[CommonConstants.Caller     ] = CommonConstants.objectName(ViewRegistry.SearchTabView)
        data[CommonConstants.Owner      ] = CommonConstants.objectName(ViewRegistry.NoResultPageView)
        data[SearchConstants.Query      ] = text
        data[CommonConstants.Visible    ] = SearchConstants.AlwaysShow

        //data[CommonConstants.Z          ] = CommonConstants.MAX_INT_VALUE
        data[CommonConstants.ReFocus    ] = true
        data[CommonConstants.ObjectName ] = CommonConstants.objectName(ViewRegistry.TextSearchView)
        sendEvent(NoResultPageViewEvent.UpdateView, data)

        showView(data)
    }

    function showKeyboard(keyboard){
        if (!isCompleted) return
        var data = ({})
        data[CommonConstants.Owner] = CommonConstants.objectName(ViewRegistry.NoResultPageView)
        data[CommonConstants.X] = keyboard.x
        data[CommonConstants.Y] = keyboard.y
        data[CommonConstants.Z] = keyboard.z + 1
        data[CommonConstants.Width] = keyboard.width
        data[CommonConstants.Height] = keyboard.height
        if (dontUseEditBox) {
            data[CommonConstants.DontUseEditBox] = true
        }
        sendEvent(NoResultPageViewEvent.ShowKeyboard, data)
    }

    function visibleKeyboard(visible) {
        keyboardVisible = visible
        if (visible) {
            showKeyboard(keyboard)
        } else {
            hideKeyboard()
        }
    }

    function hideKeyboard(){
        var data = ({})
        data[CommonConstants.Owner] = CommonConstants.objectName(ViewRegistry.NoResultPageView)
        sendEvent(NoResultPageViewEvent.HideKeyboard, data)
    }

    Component.onCompleted: {
        isCompleted = true
    }

    Component.onDestruction: {
    }

    onVisibleChanged: {
        visibleKeyboard(visible)
    }

    function init() {
        if (!needInit)
            return
        needInit = false

        allocateTextEnum()
        loader.sourceComponent = undefined
        loader.sourceComponent = main
    }

    onMediated: {
        sendEvent(NoResultPageViewEvent.RequestLang)
        init()
    }

    onActivated: {
        if (!visible) {
            return
        }
        init()
        sendEvent(NoResultPageViewEvent.Activated)
        sendEvent(NoResultPageViewEvent.SelectZone1TextInput, {text:query})
        //dontUseEditBox = true
        visibleKeyboard(true)
        dontUseEditBox = false
        //sendEvent(NoResultPageViewEvent.UpdateSearchResultCount)
    }

    onDeactivated: {
        sendEvent(NoResultPageViewEvent.Deactivated)
        query = ""
    }

    function __doSoftKeyFunction(button, menuName, menuIndex) {
    }

    property bool keyboardDown: false
    function __pullDownKeyboard() {
        if (!active)
            return
        keyboardDown = true
        visibleKeyboard(false)
    }

    function __pullUpKeyboard() {
        if (!active)
            return
        keyboardDown = false
        visibleKeyboard(true)
    }

    //언어가 변경되었을 때
    function __retranslateUi(lang) {
      searchRoot.__lang = lang
        needInit = true
      if (active) {
        init()
      }
    }


    //main
    Loader {
         id: loader
         width: parent.width
         height: parent.height + GlobalConstants.kScreenZone3Height
    }

    Component {
        id: main
        Column {
            onParentChanged: {
                if (CommonUtils.isNull(parent)) {
                    return
                }
                width = parent.width
                height = parent.height
            }
            Rectangle{
                width: parent.width
                height: parent.height
                color: GlobalConstants.kBasicBackgoundColor
                Text {
                    width: parent.width
                    height: parent.height - keyboard.height
                    horizontalAlignment: Qt.AlignHCenter
                    verticalAlignment: Qt.AlignVCenter
                    font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(GlobalConstants.kGeneralMessageFontSize) }
                    color: GlobalConstants.kListTextColor
                    text: textArr[textEnum.NoData]
                }
            }
            Item {
                Component.onCompleted: searchRoot.keyboard = this
                width: parent.width
                height: keyboardVisible? keyboardHeight : 0
            }
        }
    }
}

