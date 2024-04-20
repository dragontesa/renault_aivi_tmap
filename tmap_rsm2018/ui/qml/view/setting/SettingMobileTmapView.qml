import QtQuick 2.0
import ViewRegistry 1.0
import CommonConstants 1.0
import ZoneConstants 1.0
import SettingConstants 1.0
import SettingMobileTmapViewEvent 1.0
import Csw 1.0

import "../../singleton"
import "../../component"
import "../../view"
import "../../view/setting"
import "../../GlobalConstants.js" as GlobalConstants
import "../../component/internal/Keyboard.js" as CustomKeyboard
import "../../view/util/CommonUtils.js" as CommonUtils


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                              인증 테스트
//-----------------------------------------------------------------------------------------------------------------------------------
//1.스마트폰에 tmap_android-phone-debug_send2carTest_180111.apk설치
//2.전화번호인증에서 뒤로가기 하여 전화번호를 010-1111-1112로 변경
//  ->인증번호를 86274로 인증
//3.환경설정 > 회원정보 > 일회용 인증번호(OTP)에서 인증번호를 받아 T map 인증 가능
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SettingView {
    id: settingRoot


    //onMediated에서 요청 및 응답
    property var __lang                         : null
    property bool online                        : false

    property bool needInit                      : true


    property var caller                         : null

    property int keyboardHeight                 : 419

    property bool focused                       : false
    property color highlightColor               : CommonConstants.Transparent

    property string interoperateInputFieldText  : CommonConstants.Empty

    //연동중인 profile id가 있는 경우 팝업 및 Session key 삭제 여부
    property bool checkProfileId                : true

    //연동중인 id 얻기
    property int syncedProfileId                : -1
    property string syncedProfileName           : CommonConstants.Empty

    property int currentProfileId                : -1

    //ID
    property var interoperateInputField         : null
    property var interoperateButton             : null

    //Event
    property var textEnum                       : ({})
    property var textArr                        : ([])

    function allocateVariant() {
    }

    function allocateTextEnum(){
        if (CommonUtils.getObjectLen(textEnum) <= 0) {
            textEnum = CommonUtils.createEnum(textEnum)
        }
        var i = 0
        CommonUtils.insertEnum(textEnum, "Zone1Title",              i++, [CommonConstants.Name], [qsTr("모바일 T map 연동")])
        CommonUtils.insertEnum(textEnum, "EnterAuthNumber",         i++, [CommonConstants.Name], [qsTr("일회용 인증번호를 입력해주세요")])
        CommonUtils.insertEnum(textEnum, "ClickCancellation",       i++, [CommonConstants.Name], [qsTr("연동을 해제 하시려면, 아래의 해제하기 버튼을 선택해주세요")])
        CommonUtils.insertEnum(textEnum, "Interoperate",            i++, [CommonConstants.Name], [qsTr("연동하기")])
        CommonUtils.insertEnum(textEnum, "Cancellation",            i++, [CommonConstants.Name], [qsTr("해제하기")])
        CommonUtils.insertEnum(textEnum, "NotiTitle",               i++, [CommonConstants.Name], [qsTr("알림")])
        CommonUtils.insertEnum(textEnum, "FailInteroperText",       i++, [CommonConstants.Name], [qsTr("연동에 실패하였습니다.<br>인증번호를 다시 입력해주세요")])
        CommonUtils.insertEnum(textEnum, "ReleaseInteroperTitle",   i++, [CommonConstants.Name], [qsTr("연동해제")])
        CommonUtils.insertEnum(textEnum, "ReleaseInteroperText",    i++, [CommonConstants.Name], [qsTr("모바일 T map과의 연동을 <br>해제하시겠습니까?")])
        CommonUtils.insertEnum(textEnum, "NotValid",                i++, [CommonConstants.Name], [qsTr("인증번호가 유효하지 않습니다.<br>확인 후 다시 시도해 주세요.")])
        CommonUtils.insertEnum(textEnum, "FailConnect",             i++, [CommonConstants.Name], [qsTr("네트워크 연결 상태가 원활하지 않습니다.<br>잠시 후 다시 시도해 주세요.")])
        CommonUtils.insertEnum(textEnum, "ExceedDigits",            i++, [CommonConstants.Name], [qsTr("인증번호 6자리가 초과되었습니다.<br>확인 후 다시 시도해 주세요.")])
        CommonUtils.insertEnum(textEnum, "ExceedCertification",     i++, [CommonConstants.Name], [qsTr("유효하지 않은 인증번호가 3회 입력되어<br>1분 30초간 인증번호 입력이 제한됩니다.")])

        //연동전
        CommonUtils.insertEnum(textEnum, "Befor1stText",            i++, [CommonConstants.Name], [qsTr("모바일 T map과 목적지 정보를 연동하시려면,<br>일회용 인증번호(OTP)를 입력해주세요.")])
        CommonUtils.insertEnum(textEnum, "Befor2ndText",            i++, [CommonConstants.Name], [qsTr("일회용 인증번호는 다음과 같이 모바일 T map에서 확인하실 수 있습니다.<br>환경설정 > 회원정보 > 일회용 인증번호")])
        CommonUtils.insertEnum(textEnum, "Befor3rdText",            i++, [CommonConstants.Name], [qsTr("모바일 T map과 연동하시면, 즐겨찾기/최근목적지<br>정보가 동기화되어 편리하게 사용하실 수 있습니다.")])
        CommonUtils.insertEnum(textEnum, "InputOTPnumber",          i++, [CommonConstants.Name], [qsTr("일회용 인증번호 입력")])

        //연동후
        CommonUtils.insertEnum(textEnum, "After1stText",            i++, [CommonConstants.Name], [qsTr("모바일 T map과 연동이 완료 되었습니다.")])
        CommonUtils.insertEnum(textEnum, "After2ndText",            i++, [CommonConstants.Name], [qsTr("모바일 T map과 즐겨찾기/최근목적지<br>정보를 동기화 하여 사용 중이십니다.")])


        //한글
        CommonUtils.insertEnum(textEnum, "NotiTitleKo",             i++, [CommonConstants.Name], ["알림"])
        CommonUtils.insertEnum(textEnum, "FailInteroperTextKo",     i++, [CommonConstants.Name], ["연동에 실패하였습니다.<br>인증번호를 다시 입력해주세요"])
        CommonUtils.insertEnum(textEnum, "ReleaseInteroperTitleKo", i++, [CommonConstants.Name], ["연동해제"])
        CommonUtils.insertEnum(textEnum, "ReleaseInteroperTextKo",  i++, [CommonConstants.Name], ["모바일 T map과의 연동을 <br>해제하시겠습니까?"])
        CommonUtils.insertEnum(textEnum, "NotValidKo",              i++, [CommonConstants.Name], ["인증번호가 유효하지 않습니다.<br>확인 후 다시 시도해 주세요."])
        CommonUtils.insertEnum(textEnum, "FailConnectKo",           i++, [CommonConstants.Name], ["네트워크 연결 상태가 원활하지 않습니다.<br>잠시 후 다시 시도해 주세요."])
        CommonUtils.insertEnum(textEnum, "ExceedDigitsKo",          i++, [CommonConstants.Name], ["인증번호 6자리가 초과되었습니다.<br>확인 후 다시 시도해 주세요."])
        CommonUtils.insertEnum(textEnum, "ExceedCertificationKo",   i++, [CommonConstants.Name], ["유효하지 않은 인증번호가 3회 입력되어<br>1분 30초간 인증번호 입력이 제한됩니다."])
        CommonUtils.insertEnum(textEnum, "CancelPreviousSyncKo",    i++, [CommonConstants.Name], ["프로필과의<br>연동이 해제됩니다.<br>계속하시겠습니까?"])
        CommonUtils.insertEnum(textEnum, "NotValidUserKo",          i++, [CommonConstants.Name], ["모바일 T map 연동은<br>Guest 프로필에서는 지원되지 않습니다"])
        CommonUtils.insertEnum(textEnum, "NoNetworkConn",           i++, [CommonConstants.Name], ["네트워크가 연결되어 있지 않습니다.<br>네트워크 연결 후 다시 시도해 주세요."])


        CommonUtils.insertEnum(textEnum ,"Space",                   i++, [CommonConstants.Name] , [String.fromCharCode(0x20)])

        CommonUtils.fetchArr(textEnum, CommonConstants.Name, textArr)
    }


    function init(){
        if (!needInit)
            return
        needInit = false

        allocateVariant()

        allocateSoftKeyEnum()
        allocateTextEnum()

       //동적 언어 변경시에 현재 로딩된 컴포넌트 유지 및 언어 변경 가능하도록 ...
       var sourceComponent = loader.sourceComponent
        if (!CommonUtils.isEqual(loader.sourceComponent , component1stSub)) {
            loader.sourceComponent = undefined
        }
       loader.sourceComponent = sourceComponent

        if (active) {
            if (CommonUtils.isEqual(loader.sourceComponent , component1stSub)) {
                sendEvent(SettingMobileTmapViewEvent.SelectZone1Title, textArr[textEnum.InputOTPnumber])
            } else {
                sendEvent(SettingMobileTmapViewEvent.SelectZone1Title, textArr[textEnum.Zone1Title])
            }
        }
    }

    onMediated: {
        sendEvent(SettingMobileTmapViewEvent.RequestLang)
        sendEvent(SettingMobileTmapViewEvent.QueryOnlineStatus)
        init()

        sendEvent(SettingMobileTmapViewEvent.RequestAuthOTPstatus)
        sendEvent(SettingMobileTmapViewEvent.RequestDeviceId)
    }

    onActivated: {
        init()
        sendEvent(SettingMobileTmapViewEvent.ResetCsw)
        if (CommonUtils.isEqual(loader.sourceComponent , component1stSub)) {
            sendEvent(SettingMobileTmapViewEvent.SelectZone1Title, textArr[textEnum.InputOTPnumber])
        } else {
            sendEvent(SettingMobileTmapViewEvent.SelectZone1Title, textArr[textEnum.Zone1Title])
        }
        sendEvent(SettingMobileTmapViewEvent.Activated, [CommonConstants.NoOption])
    }

    onDeactivated: {
        sendEvent(SettingMobileTmapViewEvent.Deactivated)
    }

    function loadcomponent1stMain(){
        loader.sourceComponent = component1stMain
    }

    function requestCreateSessionByOTP() {        
        var data = ({})
        data[CommonConstants.Value] = CommonUtils.replace(interoperateInputField.text, String.fromCharCode(0x20), CommonConstants.Empty)
        sendEvent(SettingMobileTmapViewEvent.CreateSessionByOTP, data)
    }

    function __deviceId(deviceId) {
    }

    function __sessionCreated(success, validateCode, errCode) {
        if( success) {
            loadComponent2nd()
        }else {
            if (validateCode > 0){
             switch(validateCode) {
                 case SettingMobileTmapViewEvent.NoUserInfo:        //사용자 정보 없음
                 case SettingMobileTmapViewEvent.ChangeUserInfo:    //사용자 정보 변경
                 case SettingMobileTmapViewEvent.FailAuthCodeCheck: //authCode 확인 실패
                 case SettingMobileTmapViewEvent.BlackList:         //black list model no사용자
                     showDialog(textEnum.NotValidKo, textArr[textEnum.NotiTitleKo], textArr[textEnum.NotValidKo], null, false)
                     break
                 default:
                     showDialog(textEnum.FailInteroperTextKo, textArr[textEnum.NotiTitleKo], textArr[textEnum.FailInteroperTextKo], null, false)
                     break
             }
            } else {
                switch(errCode) {
                    case SettingMobileTmapViewEvent.ExceedSignifDigits: //(6)자릿수 초과
                        showDialog(textEnum.ExceedDigitsKo, textArr[textEnum.NotiTitleKo], textArr[textEnum.ExceedDigitsKo], null, false)
                        break
                    default:
                        showDialog(textEnum.FailConnectKo, textArr[textEnum.NotiTitleKo], textArr[textEnum.FailConnectKo], null, false)
                        break
                }
            }
        }
    }

    //SettingMobileTmapViewEvent.GetCurrentProfileId 요청시 현재 사용자 아이디를 얻어옴
    function __currentProfileId(id) {
        currentProfileId = id
    }

    //SettingMobileTmapViewEvent.GetSyncedProfileId 요청시 동기화된 아이디를 얻어옴
    function __syncProfileId(id) {
         syncedProfileId = id
    }

    //SettingMobileTmapViewEvent.GetSyncedProfileName 요청시 동기화된 프로파일 이름을 얻어옴
    function __syncProfileName(name) {
        syncedProfileName = name
    }

    //인증 여부
    function __authOTPstatus(auth) {
        if (auth) {
            loadComponent2nd()
        } else {
            loadcomponent1stMain()
        }
    }

    //인증시도 가능 여부
    property bool authOTPtry: true
    function __authOTPTryAvailable(authTry) {
        authOTPtry = authTry
    }

    function loadComponent2nd(){
        interoperateInputFieldText = CommonConstants.Empty
        loader.sourceComponent = component2nd
    }

    function visibleKeyboard(visible, text) {
        var data = ({})
        data[CommonConstants.DontUseEditBox] = true
        if (visible) {
            keyboard.height = keyboardHeight
            data[CommonConstants.Owner] = CommonConstants.objectName(ViewRegistry.SettingMobileTmapView)
            data[CommonConstants.X] = keyboard.x
            data[CommonConstants.Y] = keyboard.y
            data[CommonConstants.Z] = keyboard.z + 1
            data[CommonConstants.Width] = keyboard.width
            data[CommonConstants.Height] = keyboard.height
            data[CommonConstants.Pressed] = CustomKeyboard.symbolKeybd
            var row = CustomKeyboard.searchKeybd[0]
            var col = CustomKeyboard.searchKeybd[1]
            data[CommonConstants.ButtonText] = [row, col, "OK"]
            data['numberKeyboard'] = true
            sendEvent(SettingMobileTmapViewEvent.ShowKeyboard, data)
        } else {
            keyboard.height = 0
            if (!CommonUtils.isNull(text))
                data[CommonConstants.Text] = text
            sendEvent(SettingMobileTmapViewEvent.HideKeyboard, data)
        }
    }

    function __changedOnlineStatus(online) {
        settingRoot.online = online
    }

    function __processKeyboard(key, text) {
        switch(key) {
        case CustomKeyboard.CodeCancel:
            if (CommonUtils.isEqual(loader.sourceComponent , component1stSub)) {
                loader.sourceComponent = component1stMain
                sendEvent(SettingMobileTmapViewEvent.ClearText)
            }
            visibleKeyboard(false, CommonConstants.Empty)
            break
        case CustomKeyboard.CodeSymbol:     break
        case CustomKeyboard.CodeKorean:     break
        case CustomKeyboard.CodeEnglish:    break
        case CustomKeyboard.CodeShift:      break
        case CustomKeyboard.CodeClear:      break
        case CustomKeyboard.CodeSetting:    break
        case CustomKeyboard.CodeSearch:     break
        case CustomKeyboard.CodeConfirm:
            interoperateInputFieldText = interoperateInputField.text
            loader.sourceComponent = component1stMain
            visibleKeyboard(false, CommonConstants.Empty)
            break
        case CustomKeyboard.CodeSpace:
        case CustomKeyboard.CodeDelete:
        default:
            //interoperateInputFieldText = text
            interoperateInputField.text = text
            break
        }
    }

    //언어가 변경되었을 때
    function __retranslateUi(lang) {
        settingRoot.__lang = lang
        needInit = true
        if (active) {
            init()
        }
    }

    function __doSoftKeyFunction(button, menuName, menuIndex) {
        var data = ({})
        switch(button) {
          case softKeyMenu[softKeyEnum.Back]:
              data[CommonConstants.ObjectName] = CommonConstants.objectName(ViewRegistry.SettingMobileTmapView)
              sendEvent(SettingMobileTmapViewEvent.CloseView, data)
              break
          case softKeyMenu[softKeyEnum.Option]:
          case softKeyMenu[softKeyEnum.Navi]:
          case softKeyMenu[softKeyEnum.Menu]:
          default:
              break
        }
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


    //////////////////////////////////////
    //{Dialog사용
        property int dlgUniqueNum: -1
        function isShowDialog() {
            if (dlgUniqueNum >= 0)
                return true
            return false
        }

        function showDialog(uniqueNum, title,context1, context2, cancel) {
            if (CommonUtils.isEqual(dlgUniqueNum,uniqueNum)) {
                return
            }
            dlgUniqueNum = uniqueNum

            var data = ({})
            data[CommonConstants.Url    ] = CommonConstants.url(ViewRegistry.CommonDialogView)
            data[CommonConstants.ParentObjectName] = CommonConstants.ContentRootForCommonDialog
            data[CommonConstants.Title  ] = title
            data[CommonConstants.Message] = context1
            var type = CommonConstants.Empty
            var s = CommonConstants.Empty
            if (!CommonUtils.isEmpty(context2)) {
                s = CommonUtils.split(context1, "<br>")
                switch(s.length) {
                 default:
                     type = CommonConstants.TypeC
                     break
                }
                data[CommonConstants.Message2] = context2
            } else {
                s = CommonUtils.split(context1, "<br>")
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
            }
            data[CommonConstants.Menus  ] = cancel ? [CommonConstants.Cancel, CommonConstants.Ok]
                                                   : [CommonConstants.Ok]
            var arg = ({})
            arg [CommonConstants.Caller ] = CommonConstants.objectName(ViewRegistry.SettingMobileTmapView)
            data[CommonConstants.Arg    ] = arg
            sendEvent(SettingMobileTmapViewEvent.CreateView, data)
        }

        function __closeDialog(yes) {
            var uniqNum = dlgUniqueNum
            closeDialog(uniqNum, yes)
        }

        function closeDialog(uniqNum, yes) {
            if (yes){
                switch(uniqNum) {
                case textEnum.ReleaseInteroperText:
                case textEnum.ReleaseInteroperTextKo:
                   sendEvent(SettingMobileTmapViewEvent.ClearSession)
                   sendEvent(SettingMobileTmapViewEvent.RequestAuthOTPstatus)
                   break
                 case textEnum.FailInteroperText:
                 case textEnum.FailInteroperTextKo:
                     break
                 case textEnum.CancelPreviousSyncKo:
                     requestCreateSessionByOTP()
                     break
                }
            } else {
                switch(uniqNum) {
                case textEnum.ReleaseInteroperText:
                case textEnum.ReleaseInteroperTextKo:
                case textEnum.FailInteroperText:
                case textEnum.FailInteroperTextKo:
                case textEnum.CancelPreviousSyncKo:
                    break
                }
            }
            if (CommonUtils.isEqual(uniqNum, dlgUniqueNum)) {
                dlgUniqueNum = -1
            }
        }
    //}Dialog사용
    //////////////////////////////////////


    //main
    Rectangle {
        width: parent.width
        height: parent.height
        color: GlobalConstants.kBasicBackgoundColor
        Loader {
             id: loader
             width: parent.width
             height:parent.height - keyboard.height
             onSourceComponentChanged: {
                 settingRoot.visible = true
                 switch(sourceComponent) {
                    case component1stSub:
                        sendEvent(SettingMobileTmapViewEvent.SelectZone1Title, textArr[textEnum.InputOTPnumber])
                        break
                    default:
                        sendEvent(SettingMobileTmapViewEvent.SelectZone1Title, textArr[textEnum.Zone1Title])
                        break
                 }
             }
        }
        Item {
            id: keyboard
            width: parent.width
            height: 0
        }
    }

    //연동전
    Component {
        id: component1stMain
        Row {width:settingRoot.width;height:settingRoot.height
            Item {width:40;height:parent.height}
            Item {width:parent.width;height:parent.height;Component.onCompleted: CommonUtils.applyRemainWidth(this,parent)
                Column {width:parent.width;height:parent.height;Component.onCompleted: CommonUtils.applyHeight(this)
                    anchors.centerIn: parent
                    Item {width:parent.width;height:84
                        Text {
                            anchors.fill: parent
                            horizontalAlignment: Qt.AlignHCenter
                            verticalAlignment: Qt.AlignVCenter
                            font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(GlobalConstants.kOneLineListTextFontSize)}
                            color: GlobalConstants.kListTextColor
                            text:  textArr[textEnum.Befor1stText]
                            lineHeightMode: Text.FixedHeight
                            lineHeight    : 42
                        }
                    }
                    Item {width:parent.width;height:36}
                    Item {width:parent.width;height:62
                        Text {
                            anchors.fill: parent
                            horizontalAlignment: Qt.AlignHCenter
                            verticalAlignment: Qt.AlignVCenter
                            font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(30)}
                            color: GlobalConstants.kListSubTextColor
                            text:  textArr[textEnum.Befor2ndText]
                            lineHeightMode: Text.FixedHeight
                            lineHeight    : 34
                        }
                    }
                    Item {width:parent.width;height:40}
                    Item {width:parent.width;height:62
                        Text {
                            anchors.fill: parent
                            horizontalAlignment: Qt.AlignHCenter
                            verticalAlignment: Qt.AlignVCenter
                            font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(30)}
                            color: GlobalConstants.kListSubTextColor
                            text:  textArr[textEnum.Befor3rdText]
                            lineHeightMode: Text.FixedHeight
                            lineHeight    : 34
                        }
                    }
                    Item {width:parent.width;height:40}
                    Item {width:parent.width;height:28
                        Text {
                            anchors.fill: parent
                            horizontalAlignment: Qt.AlignHCenter
                            verticalAlignment: Qt.AlignVCenter
                            font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(30)}
                            color: GlobalConstants.kListSubTextColor
                            text:  textArr[textEnum.EnterAuthNumber]
                        }
                    }
                    Item {width:parent.width;height:50}
                    Item {width:parent.width;height:58
                        Row {width:parent.width;height:parent.height
                            Loader {width: 450; height:parent.height;
                                 Component.onCompleted: CommonUtils.applyRemainWidth(this,parent)
                                 sourceComponent: inputComponent
                            }
                            Item {width:30;height:parent.height}
                            Item {width:260;height:parent.height
                                TPushButton {
                                    width: parent.width;height: parent.height
                                    anchors.verticalCenter: parent.verticalCenter
                                    label: textArr[textEnum.Interoperate]
                                    mutisense: true
                                    enabled: {
                                        if (CommonUtils.isNull(interoperateInputField))
                                              return false
                                        return interoperateInputField.text.length
                                    }
                                    Component.onCompleted: {
                                        settingRoot.interoperateButton = this
                                        box.colorN            = Qt.binding(function(){ return CommonConstants.Transparent})
                                        box.colorP            = Qt.binding(function(){ return CommonConstants.Transparent})
                                        img.sourceN           = Qt.binding(function(){ return Resource.imageUrl("setting","btn_bg2_n.png")})
                                        img.sourceP           = Qt.binding(function(){ return Resource.imageUrl("setting","btn_bg2_p.png")})
                                        img.width             = Qt.binding(function(){ return width})
                                        img.height            = Qt.binding(function(){ return height})
                                        img.sourceSize.width  = Qt.binding(function(){ return width})
                                        img.sourceSize.height = Qt.binding(function(){ return height})
                                        txt.colorN            = Qt.binding(function(){ return GlobalConstants.kListTextColor})
                                        txt.colorP            = Qt.binding(function(){ return GlobalConstants.kListTextColor})
                                        txt.font.family       = Qt.binding(function(){ return GlobalConstants.kFontFamily})
                                        txt.font.pixelSize    = Qt.binding(function(){ return Resource.pixelToDp(30)})
                                    }
                                    Component.onDestruction: {
                                        settingRoot.interoperateButton = null
                                    }
                                    onButtonClicked: {
                                        interoperateInputField.focus = false
                                        sendEvent(SettingMobileTmapViewEvent.GetCurrentProfileId)
                                        if (currentProfileId <= 0)  {
                                            interoperateInputField.text = CommonConstants.Empty
                                            interoperateInputFieldText = CommonConstants.Empty
                                            sendEvent(SettingMobileTmapViewEvent.ClearText)
                                            showDialog(textEnum.NotValidUserKo,
                                                       textArr[textEnum.NotiTitleKo],
                                                       textArr[textEnum.NotValidUserKo],
                                                       null,
                                                       false)
                                            return
                                        }
                                        if (!online) {
                                            showDialog(textEnum.NoNetworkConn,
                                                       textArr[textEnum.NotiTitleKo],
                                                       textArr[textEnum.NoNetworkConn],
                                                       null,
                                                       false)
                                            return
                                        }
                                        sendEvent(SettingMobileTmapViewEvent.RequestAuthOTPTryAvailable)
                                        if (!settingRoot.authOTPtry) {
                                            showDialog(textEnum.ExceedCertificationKo, textArr[textEnum.NotiTitleKo], textArr[textEnum.ExceedCertificationKo], null, false)
                                        } else {
                                            requestCreateSessionByOTP()
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            Item {width:40;height:parent.height}
        }
    }

    // 인증번호 입력
    Component {
        id: component1stSub
        Row {width:settingRoot.width;height:settingRoot.height
            Item {width:40;height:parent.height}
            Item{width:parent.width;height:parent.height;Component.onCompleted: CommonUtils.applyRemainWidth(this,parent)
                Column {width:parent.width;height:parent.height
                    Item {width:parent.width;height:120}
                    Item {width:parent.width;height:58
                        Loader {width: 450; height:parent.height;
                             sourceComponent: inputComponent
                             anchors.centerIn: parent
                        }
                    }
                    Item {width:parent.width;height:parent.height;Component.onCompleted: CommonUtils.applyRemainHeight(this,parent)}
                }
            }
            Item {width:40;height:parent.height}
        }
    }

    //연동후
    Component {
        id: component2nd
        Row {width:settingRoot.width;height:settingRoot.height
            Item {width:40;height:parent.height}
            Item {width:parent.width;height:parent.height;Component.onCompleted: CommonUtils.applyRemainWidth(this,parent)
                Column {width:parent.width;height:parent.height;Component.onCompleted: CommonUtils.applyHeight(this)
                    anchors.centerIn: parent
                    Item {width:parent.width;height:32
                        Text {
                            anchors.fill: parent
                            horizontalAlignment: Qt.AlignHCenter
                            verticalAlignment: Qt.AlignVCenter
                            font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(GlobalConstants.kOneLineListTextFontSize)}
                            color: GlobalConstants.kListTextColor
                            text:  textArr[textEnum.After1stText]
                        }
                    }
                    Item {width:parent.width;height:36}
                    Item {width:parent.width;height:62
                        Text {
                            anchors.fill: parent
                            horizontalAlignment: Qt.AlignHCenter
                            verticalAlignment: Qt.AlignVCenter
                            font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(30)}
                            color: GlobalConstants.kListSubTextColor
                            text:  textArr[textEnum.After2ndText]
                            lineHeightMode: Text.FixedHeight
                            lineHeight    : 34

                        }
                    }
                    Item {width:parent.width;height:40}
                    Item {width:parent.width;height:28
                        Text {
                            anchors.fill: parent
                            horizontalAlignment: Qt.AlignHCenter
                            verticalAlignment: Qt.AlignVCenter
                            font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(30)}
                            color: GlobalConstants.kListSubTextColor
                            text:  textArr[textEnum.ClickCancellation]
                        }
                    }
                    Item {width:parent.width;height:50}
                    Item {width:parent.width;height:58
                        TPushButton {
                            width: 260;height: parent.height
                            anchors.centerIn: parent
                            label: textArr[textEnum.Cancellation]
                            mutisense: true
                            Component.onCompleted: {
                                box.colorN            = Qt.binding(function(){ return CommonConstants.Transparent})
                                box.colorP            = Qt.binding(function(){ return CommonConstants.Transparent})
                                img.sourceN           = Qt.binding(function(){ return Resource.imageUrl("setting","btn_bg2_n.png")})
                                img.sourceP           = Qt.binding(function(){ return Resource.imageUrl("setting","btn_bg2_p.png")})
                                img.width             = Qt.binding(function(){ return width})
                                img.height            = Qt.binding(function(){ return height})
                                img.sourceSize.width  = Qt.binding(function(){ return width})
                                img.sourceSize.height = Qt.binding(function(){ return height})
                                txt.colorN            = Qt.binding(function(){ return GlobalConstants.kListTextColor})
                                txt.colorP            = Qt.binding(function(){ return GlobalConstants.kListTextColor})
                                txt.font.family       = Qt.binding(function(){ return GlobalConstants.kFontFamily})
                                txt.font.pixelSize    = Qt.binding(function(){ return Resource.pixelToDp(30)})
                            }
                            onButtonClicked: {
                                showDialog(textEnum.ReleaseInteroperTextKo, textArr[textEnum.ReleaseInteroperTitleKo], textArr[textEnum.ReleaseInteroperTextKo], null, true)
                            }
                        }
                    }
                }
            }
            Item {width:40;height:parent.height}
        }
    }

    Component {
        id: inputComponent
        Image {width:parent.width;height:parent.height
            sourceSize.width: width
            sourceSize.height: height
            source: Resource.imageUrl("setting", "img_inputbox_bg.png")
            TextInput {
                Component.onCompleted: {
                    settingRoot.interoperateInputField = this
                    text = interoperateInputFieldText
                    if (CommonUtils.isEqual(loader.sourceComponent , component1stSub)) {
                        sendEvent(SettingMobileTmapViewEvent.HideKeyboard)
                        sendEvent(SettingMobileTmapViewEvent.RegistKeyboardInput, {keyboardInput: this})
                        visibleKeyboard(true)
                    }
                }
                anchors.fill: parent
                verticalAlignment: Qt.AlignVCenter
                horizontalAlignment: Qt.AlignHCenter
                font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(GlobalConstants.kOneLineListTextFontSize)}
                color: MultiSenseCoordinator.kAmbientColor
                cursorVisible: CommonUtils.isEqual(loader.sourceComponent , component1stSub) ? true : false
                maximumLength: 7 //유효자릿수 그 이상  입력되지 않도록 수정
                onTextChanged: {
                    text = text.replace(/([0-9]{3})([0-9]{3})/,'$1 $2')
                    if (CommonUtils.isEqual(text.length , maximumLength)) {
                        sendEvent(SettingMobileTmapViewEvent.SetKeyboardText, text)
                    }
                }
                onCursorPositionChanged: {
                    cursorPosition = text.length
                }
                focus: false
                clip: true
                onActiveFocusChanged: {
                    if ( activeFocus && visible ) {
                        sendEvent(SettingMobileTmapViewEvent.RequestAuthOTPTryAvailable)
                        if (!settingRoot.authOTPtry) {
                            interoperateInputField.focus = false
                            showDialog(textEnum.ExceedCertificationKo, textArr[textEnum.NotiTitleKo], textArr[textEnum.ExceedCertificationKo], null, false)
                        } else {
                            loader.sourceComponent = component1stSub
                        }
                    }
                }
            }
        }
    }


    Csw {
            id: csw

            section: Csw.Section_Setting
            sequence: 0
            enable: settingRoot.visible
        }
}
