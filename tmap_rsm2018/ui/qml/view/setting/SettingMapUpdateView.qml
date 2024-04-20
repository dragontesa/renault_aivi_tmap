import QtQuick 2.0
import ViewRegistry 1.0
import CommonConstants 1.0
import ZoneConstants 1.0
import SettingConstants 1.0
import SettingMapUpdateViewEvent 1.0
import Csw 1.0

import "../../component"
import "../../view"
import "../../view/util"
import "../../view/setting"
import "../../GlobalConstants.js" as GlobalConstants
import "../../view/util/CommonUtils.js" as CommonUtils


//    1.지도 업데이트 구현
//    *common/updatemanagerinterface.h* 참고하시면 됩니다. UI 초기화 시 구현체에 대한 포인터가 넘어갑니다.
//    1)USB가 mount되면(PC에서는 U키를 누릅니다.)
//    2)확인 후 맵데이터가 있을 때 `updatePackageMounted` 시그널을 발생 시킵니다.
//    3)이 때, UI에서는 `packageStatus()` 메서드를 통해 USB 맵데이터의 상태(Newer/Older/Same/…)를 확인할 수 있습니다.
//    4)UI에서는 `startUpdate()` 메서드를 통해 업데이트(파일 복사)를 시작할 수 있습니다.
//    5)UI에서는 `stopUpdate()` 메서드를 통해 업데이트(파일 복사)를 중지할 수 있습니다.
//    6)업데이트(파일 복사) 진행 사항은 `updatePackageProgress()` 시그널을 통해 리포트 됩니다.
//    7)업데이트(파일 복사)가 완료되면 `updatePackageStored()` 시그널이 발생됩니다.
//    8)UI에서는 `applyUpdate()` 메서드를 통해 최종적으로 업데이트를 적용 할 수 있습니다.



SettingView {
    id: settingRoot

    property int mainMessageFontSize        : 35
    property int subMessageFontSize         : 30
    property int lineHeight                 : 34

    //언어 관련
    property var __lang                     : null


    property var caller                     : null
    property var settings                   : null
    property int updataStatus               : 0

    property color itemColor                : GlobalConstants.kListSubTextColor

    property bool isActivated               : false

    //ID

    //Event
    property var textEnum                   : ({})
    property var settingsEnum               : ({})
    property var updateStatusEnum           : ({})

    property var textArr                    : ([])
    property var settingsArr                : ([])
    property var updateStatusArr            : ([])

    function allocateVariant() {
    }

    function allocateTextEnum(){
        if (CommonUtils.getObjectLen(textEnum) <= 0) {
            textEnum = CommonUtils.createEnum(textEnum)
        }
        var i = 0
        CommonUtils.insertEnum(textEnum, "Zone1Title",            i++, [CommonConstants.Name], [qsTr("지도 업데이트")])
        CommonUtils.insertEnum(textEnum, "UpdateButton",          i++, [CommonConstants.Name], [qsTr("업데이트")])
        CommonUtils.insertEnum(textEnum, "MapUpdateButton",       i++, [CommonConstants.Name], [qsTr("지도 업데이트")])
        CommonUtils.insertEnum(textEnum, "NotiTitle",             i++, [CommonConstants.Name], [qsTr("알림")])
        CommonUtils.insertEnum(textEnum, "NoMapData",             i++, [CommonConstants.Name], [SettingConstants.NoMapData])
        CommonUtils.insertEnum(textEnum, "MapDataError",          i++, [CommonConstants.Name], [SettingConstants.MapDataError])
        CommonUtils.insertEnum(textEnum, "IncompleteMapData",     i++, [CommonConstants.Name], [SettingConstants.IncompleteMapData])
        CommonUtils.insertEnum(textEnum, "IncompatibleMapData",   i++, [CommonConstants.Name], [SettingConstants.IncompatibleMapData])
        CommonUtils.insertEnum(textEnum, "ErrorMapData",          i++, [CommonConstants.Name], [SettingConstants.ErrorMapData])
        //-----------------------------------------------------------------------------------------------------------------------------------------------------------------
        CommonUtils.insertEnum(textEnum, "NoMapInfo1st",          i++, [CommonConstants.Name], [qsTr("지도 데이터가 없습니다.")])
        CommonUtils.insertEnum(textEnum, "NoMapInfo2nd",          i++, [CommonConstants.Name], [qsTr("지도 데이터가 없어 내비게이션을 사용할 수 없습니다.<br><br>"
                                                                                                    +"USB 지도 업데이트를 통해 지도를 설치하거나<br>"
                                                                                                    +"통신 환경에서 재실행 해주세요.")])
        //-----------------------------------------------------------------------------------------------------------------------------------------------------------------
        CommonUtils.insertEnum(textEnum, "IncompatibleMapInfo1st",i++, [CommonConstants.Name], [qsTr("지도 데이터를 사용할 수 없습니다.")])
        CommonUtils.insertEnum(textEnum, "IncompatibleMapInfo2nd",i++, [CommonConstants.Name], [qsTr("지도의 버전이 낮거나 지도 데이터에 문제가 있어<br>"
                                                                                                    +"내비게이션을 실행할 수 없습니다.<br><br>"
                                                                                                    +"USB 지도 업데이트를 통해 지도를 설치하거나<br>"
                                                                                                    +"통신 환경에서 재실행 해주세요.")])
        //-----------------------------------------------------------------------------------------------------------------------------------------------------------------
        CommonUtils.insertEnum(textEnum, "VersionInfo1st",        i++, [CommonConstants.Name], [qsTr("업데이트 정보가 없습니다.")])
        CommonUtils.insertEnum(textEnum, "VersionInfo2nd",        i++, [CommonConstants.Name], [qsTr("현재 맵이 최신버전이거나<br>USB로부터 전송받은 데이터가 없습니다.")])
        //-----------------------------------------------------------------------------------------------------------------------------------------------------------------
        CommonUtils.insertEnum(textEnum, "CopyInfo1st",           i++, [CommonConstants.Name], [qsTr("지도 데이터 복사중 입니다.")])
        CommonUtils.insertEnum(textEnum, "CopyInfo2nd",           i++, [CommonConstants.Name], [qsTr("지도 복사중에는 차량의 시동을 끄거나 USB를 제거하지 마십시오.<br>"
                                                                                                    +"완료까지 약간의 시간이 소요될 수 있습니다.<br>"
                                                                                                    +"지도 복사중에도 Home 버튼 또는 Back 버튼을 이용하여 <br>"
                                                                                                    +"다른 작업을 하실 수 있습니다.")])
        CommonUtils.insertEnum(textEnum, "CopyInfo2ndNoMapData",  i++, [CommonConstants.Name], [qsTr("지도 복사중에는 차량의 시동을 끄거나 USB를 제거하지 마십시오.<br>"
                                                                                                    +"완료까지 약간의 시간이 소요될 수 있습니다.")])
        //-----------------------------------------------------------------------------------------------------------------------------------------------------------------
        CommonUtils.insertEnum(textEnum, "CompleteInfo1st",       i++, [CommonConstants.Name], [qsTr("복사된 지도 데이타가 있습니다.")])
        CommonUtils.insertEnum(textEnum, "CompleteInfo2nd",       i++, [CommonConstants.Name], [qsTr("시스템 재시동 시 지도가 업데이트 됩니다.")])



        //-----------------------------------------------------------------------------------------------------------------------------------------------------------------
        //한글
        CommonUtils.insertEnum(textEnum, "Zone1TitleKo",          i++, [CommonConstants.Name], ["지도 업데이트"])
        CommonUtils.insertEnum(textEnum, "NotiTitleKo",           i++, [CommonConstants.Name], ["알림"])
        CommonUtils.insertEnum(textEnum, "CopyMapData1Ko",        i++, [CommonConstants.Name], ["지도 업데이트를 설치할 수 있습니다.<br>"
                                                                                               +"(USB의 맵 버전으로 모두 대체됩니다)"])
        CommonUtils.insertEnum(textEnum, "CopyMapData2Ko",        i++, [CommonConstants.Name], ["지도 데이타를 복사 하시겠습니까?"])
        CommonUtils.insertEnum(textEnum, "SameVersion1Ko",        i++, [CommonConstants.Name], ["동일 버전입니다."])
        CommonUtils.insertEnum(textEnum, "SameVersion2Ko",        i++, [CommonConstants.Name], ["업데이트를 진행하시겠습니까?"])
        CommonUtils.insertEnum(textEnum, "OldVersion1Ko",         i++, [CommonConstants.Name], ["구버전 입니다."])
        CommonUtils.insertEnum(textEnum, "OldVersion2Ko",         i++, [CommonConstants.Name], ["지도 업데이트가 취소됩니다."])
        CommonUtils.insertEnum(textEnum, "CopyError1Ko",          i++, [CommonConstants.Name], ["지도 데이터 파일에 문제가 있어<br>"
                                                                                               +"설치를 진행할 수 없습니다."])
        CommonUtils.insertEnum(textEnum, "CopyError2Ko",          i++, [CommonConstants.Name], ["지도 데이터를 다시 다운받아 주세요."])
        CommonUtils.insertEnum(textEnum, "Unmounted1Ko",          i++, [CommonConstants.Name], ["USB가 제거되어 설치가 중단되었습니다."])
        CommonUtils.insertEnum(textEnum, "Unmounted2Ko",          i++, [CommonConstants.Name], ["USB를 다시 넣어 주십시오."])
        CommonUtils.insertEnum(textEnum, "InCompleteMapDataKo",   i++, [CommonConstants.Name], ["지도가 온전히 복사되지 않았습니다.<br>"
                                                                                               +"지도를 업데이트 하려면<br>"
                                                                                               +"USB를 다시 넣어주세요."])
        CommonUtils.insertEnum(textEnum, "MapDataCompleteKo",     i++, [CommonConstants.Name], ["지도 데이터 복사가 완료되었습니다.<br>"
                                                                                               +"시스템 재시동 시 지도 업데이트가 계속됩니다."])
        CommonUtils.insertEnum(textEnum, "ErrorInfoKo",           i++, [CommonConstants.Name], ["문제가 발생하여<br>"
                                                                                               +"지도 데이터 복사가 취소되었습니다.<br>"
                                                                                               +"지도를 업데이트 하려면<br>"
                                                                                               +"USB장치를 다시 연결해주세요."])
        CommonUtils.insertEnum(textEnum, "CentralLocked1Ko",      i++, [CommonConstants.Name], ["시스템 동작으로 인해 지금은<br>"
                                                                                               +"지도 업데이트를 실행할 수 없습니다."])
        CommonUtils.insertEnum(textEnum, "CentralLocked2Ko",      i++, [CommonConstants.Name], ["나중에 다시 USB를 넣어 주세요."])



        //-----------------------------------------------------------------------------------------------------------------------------------------------------------------
        CommonUtils.insertEnum(textEnum ,"Space",                 i++, [CommonConstants.Name], [String.fromCharCode(0x20)])
        CommonUtils.fetchArr(textEnum, CommonConstants.Name, textArr)
    }

    function allocateSettingsEnum(){
        if (CommonUtils.getObjectLen(settingsEnum) <= 0) {
            settingsEnum = CommonUtils.createEnum(settingsEnum)
        }
        CommonUtils.insertEnum(settingsEnum, "ProgramVersion",        SettingMapUpdateViewEvent.ResultAppVersion,          [CommonConstants.Name], [qsTr("프로그램 버전")])
        CommonUtils.insertEnum(settingsEnum, "MapVersion",            SettingMapUpdateViewEvent.ResultMapVersion,          [CommonConstants.Name], [qsTr("서버 지도 버전")])
        CommonUtils.insertEnum(settingsEnum, "LocalMapVersion",       SettingMapUpdateViewEvent.ResultLocalMapVersion,     [CommonConstants.Name], [qsTr("내장 지도 버전")])
        CommonUtils.insertEnum(settingsEnum, "LocalNetworkVersion",   SettingMapUpdateViewEvent.ResultLocalNetworkVersion, [CommonConstants.Name], [qsTr("내장 전국 도로 버전")])
        CommonUtils.insertEnum(settingsEnum, "LocalPoiVersion",       SettingMapUpdateViewEvent.ResultLocalPoiVersion,     [CommonConstants.Name], [qsTr("내장 명칭 데이타 버전")])
        CommonUtils.insertEnum(settingsEnum, "SafeDriveVersion",      SettingMapUpdateViewEvent.ResultSDIVersion,          [CommonConstants.Name], [qsTr("안전 운전 버전")])

        CommonUtils.fetchArr(settingsEnum, CommonConstants.Name, settingsArr)
    }

    //순서를 바꾸지 말것: 진행순서
    function allocateUpdateStatusEnum(){
        if (CommonUtils.getObjectLen(updateStatusEnum) <= 0) {
            updateStatusEnum = CommonUtils.createEnum(updateStatusEnum)
        }
        var i = 0
        CommonUtils.insertEnum(updateStatusEnum, "Init"                 , i++)
        CommonUtils.insertEnum(updateStatusEnum, "Mounted"              , i++)
        CommonUtils.insertEnum(updateStatusEnum, "RequestStatus"        , i++)
        CommonUtils.insertEnum(updateStatusEnum, "ResponseStatus"       , i++)
        CommonUtils.insertEnum(updateStatusEnum, "UpdateStart"          , i++)
        CommonUtils.insertEnum(updateStatusEnum, "UpdateProgress"       , i++)
        CommonUtils.insertEnum(updateStatusEnum, "UpdateEnd"            , i++)
    }

    function requestSetting() {
        sendEvent(SettingMapUpdateViewEvent.RequestSetting)
    }

    function requestNeedRestart() {
        sendEvent(SettingMapUpdateViewEvent.RequestNeedRestart)
    }

    function __showZone1Title() {
        showZone1Title()
    }

    function showZone1Title() {
        sendEvent(SettingMapUpdateViewEvent.SelectZone1Title, textArr[textEnum.Zone1Title])
        sendEvent(SettingMapUpdateViewEvent.Activated, [CommonConstants.NoOption])

    }

    function init(){
        allocateVariant()

        allocateSoftKeyEnum()
        allocateTextEnum()
        allocateSettingsEnum()
        allocateUpdateStatusEnum()

        var sourceComponent = loader.sourceComponent
        loader.sourceComponent = undefined
        loader.sourceComponent = sourceComponent
    }

    onMediated: {
        sendEvent(SettingMapUpdateViewEvent.RequestLang)
        sendEvent(SettingMapUpdateViewEvent.Mediated)
        init()
        showZone1Title()
        switch(caller) {
            //USB 맵데이타가 정상적이 아닌 경우...(공개키가 바뀌었거나
            //package.json 파일을 읽다가 에러가 난경우:
            // 1)파일을 읽지 못한경우,
            // 2)파일이 비어 있는 경우
            // 3)json파일이 잘못된 경우 MessageDigestSHA256
            case textArr[textEnum.ErrorMapData]:
               showDialog(textEnum.CopyError1Ko, textArr[textEnum.NotiTitleKo], textArr[textEnum.CopyError1Ko], textArr[textEnum.CopyError2Ko], false)
               break
            //USB 맵업데이트가 정상적으로 종료되지 않은 경우
            case textArr[textEnum.IncompleteMapData]:
               sendEvent(SettingMapUpdateViewEvent.RequestPopupInCompleteMapDataCopy)
               break
        }
    }

    onActivated: {
        sendEvent(SettingMapUpdateViewEvent.ResetCsw)
        if (isActivated) {
            return
        }
        isActivated = true
        switch(caller) {
         //환경설정>지도맵데이터 로 들어온경우
         case CommonConstants.objectName(ViewRegistry.SettingMainView):
            requestSetting()
            break
         //맵데이타가 없는 경우
         case textArr[textEnum.NoMapData]:
         case textArr[textEnum.IncompatibleMapData]:
            loader.sourceComponent = nomapdata
            break
        }
        sendEvent(SettingMapUpdateViewEvent.HideKeyboard)
        showZone1Title()
    }

    onDeactivated: {
        isActivated = false
        sendEvent(SettingMapUpdateViewEvent.Deactivated)
    }

    Component.onDestruction: {
        timer = CommonUtils.destroy(timer)
    }

    onVisibleChanged: {
        if (!visible) return
        init()
    }

    onSettingsChanged: {
        if (CommonUtils.isNull(settings))
            return
        loader.sourceComponent = version
    }

    //언어가 변경되었을 때
    function __retranslateUi(lang) {
        settingRoot.__lang = lang
        init()
    }

    function __doSoftKeyFunction(button, menuName, menuIndex) {
        switch(button) {
          case softKeyMenu[softKeyEnum.Back]:
             if (CommonUtils.isIndexOf(caller, [textArr[textEnum.NoMapData], textArr[textEnum.IncompatibleMapData]])) {
                 sendEvent(SettingMapUpdateViewEvent.Back)
                 break
             }
             closeView(true)
             break
          case softKeyMenu[softKeyEnum.Option]:
          case softKeyMenu[softKeyEnum.Navi]:
              break
          case softKeyMenu[softKeyEnum.Menu]:
              break
        }
    }

    function hideNcloseByCondition() {
        closeView(!settingRoot.visible)
    }

    function closeView(doClose) {
        if (!CommonUtils.isIndexOf(caller, [textArr[textEnum.NoMapData], textArr[textEnum.IncompatibleMapData]])) {
            switch(updataStatus) {
            case updateStatusEnum.UpdateStart:
            case updateStatusEnum.UpdateProgress:
            case updateStatusEnum.UpdateEnd:
                settingRoot.visible = false
                break
            default:
                if (doClose) {
                    var data = ({})
                    data[CommonConstants.ObjectName] = CommonConstants.objectName(ViewRegistry.SettingMapUpdateView)
                    sendEvent(SettingMapUpdateViewEvent.CloseView, data)
                    updataStatus = updateStatusEnum.Init
                }
                break
            }
        }
    }

    property var visibleCommonDialogView : null
    function __onResultViewList(result) {
        for(var i= 0; i<result.length; i++) {
            if (CommonUtils.isEqual(result[i][CommonConstants.ResultViewId], ViewRegistry.CommonDialogView)) {
                visibleCommonDialogView = true
                return
            }
        }
        visibleCommonDialogView = false
    }


    //0.맵업데이트가 완료되지 않은 경우
    function __popupInCompleteMapDataCopy(yes) {
        if (yes) {
            showDialog(textEnum.InCompleteMapDataKo, textArr[textEnum.NotiTitleKo], textArr[textEnum.InCompleteMapDataKo], null, false)
        } else {
            hideNcloseByCondition()
        }
    }

    //1.USB마운트/언마운트
    function __updatePackageMounted(locked) {
        gErrno = SettingMapUpdateViewEvent.ErrorTypeNone
        updataStatus = updateStatusEnum.Mounted
        if (CommonUtils.isEqual(dlgUniqueNum,  textEnum.InCompleteMapDataKo)) {
            sendEvent(SettingMapUpdateViewEvent.RequestIgnoreMapUpdate)
            var data = ({})
            data[CommonConstants.ObjectName] = CommonConstants.objectName(ViewRegistry.CommonDialogView)
            sendEvent(SettingMapUpdateViewEvent.CloseView, data)
            dlgUniqueNum = -1
        }
        if (locked) {
            showDialog(textEnum.CopyMapData1Ko, textArr[textEnum.Zone1TitleKo], textArr[textEnum.CopyMapData1Ko], textArr[textEnum.CopyMapData2Ko], true)
        } else {
            showDialog(textEnum.CentralLocked1Ko, textArr[textEnum.Zone1TitleKo], textArr[textEnum.CentralLocked1Ko], textArr[textEnum.CentralLocked2Ko], false)
        }
    }

    function __updatePackageUnmounted() {
        gErrno = SettingMapUpdateViewEvent.ErrorTypeMediaUnmounted
        //맵업데이트 중이면...
        if ((updateStatusEnum.UpdateStart <= updataStatus && updataStatus < updateStatusEnum.UpdateEnd)) {
            return
        }
        sendEvent(SettingMapUpdateViewEvent.RequestViewList)
        if (!visibleCommonDialogView) {
            return
        }
        var data = ({})
        switch(dlgUniqueNum) {
            case textEnum.CopyMapData1Ko:
            case textEnum.CopyMapData2Ko:
            case textEnum.SameVersion1Ko:
            case textEnum.SameVersion2Ko:
                data[CommonConstants.Caller ] = CommonConstants.objectName(ViewRegistry.SettingMapUpdateView)
                sendEvent(SettingMapUpdateViewEvent.RequestDlgCanceled, data);
                data = ({})
                data[CommonConstants.ObjectName] = CommonConstants.objectName(ViewRegistry.CommonDialogView)
                sendEvent(SettingMapUpdateViewEvent.CloseView, data)
                break
            case textEnum.OldVersion1Ko:
            case textEnum.OldVersion2Ko:
            case textEnum.CentralLocked1Ko:
            case textEnum.CentralLocked2Ko:
                data[CommonConstants.Caller ] = CommonConstants.objectName(ViewRegistry.SettingMapUpdateView)                
                sendEvent(SettingMapUpdateViewEvent.RequestDlgAccepted, data);
                data = ({})
                data[CommonConstants.ObjectName] = CommonConstants.objectName(ViewRegistry.CommonDialogView)
                sendEvent(SettingMapUpdateViewEvent.CloseView, data)
                break
        }
    }


    //2.상태
    function requestPackageStatus() {
        updataStatus = updateStatusEnum.RequestStatus
        sendEvent(SettingMapUpdateViewEvent.RequestPackageStatus)
    }
    function __packageStatus(status) {
        updataStatus = updateStatusEnum.ResponseStatus
        switch(status) {
        case SettingMapUpdateViewEvent.PackageStatusNewer:
            updateStart()
            break
        case SettingMapUpdateViewEvent.PackageStatusSame:
            showDialog(textEnum.SameVersion1Ko, textArr[textEnum.NotiTitleKo], textArr[textEnum.SameVersion1Ko], textArr[textEnum.SameVersion2Ko], true)
            break
        case SettingMapUpdateViewEvent.PackageStatusOlder:
            showDialog(textEnum.OldVersion1Ko, textArr[textEnum.NotiTitleKo], textArr[textEnum.OldVersion1Ko], textArr[textEnum.OldVersion2Ko], false)
            break
        case SettingMapUpdateViewEvent.PackageStatusError:
        default:
            showDialog(textEnum.CopyError1Ko, textArr[textEnum.NotiTitleKo], textArr[textEnum.CopyError1Ko], textArr[textEnum.CopyError2Ko], false)
            break
        }
    }

    //3.복사 시작
    function updateStart() {
        updataStatus = updateStatusEnum.UpdateStart
        if (CommonUtils.isEqual(gErrno,  SettingMapUpdateViewEvent.ErrorTypeMediaUnmounted)) {
            __updatePackageError(SettingMapUpdateViewEvent.ErrorTypeMediaUnmounted)
            return
        }
        sendEvent(SettingMapUpdateViewEvent.RequestStartUpdate)
    }

    //4.복사 진행
    property real currProgressValue: 0.0
    property real progressLeftWidth: 0.0
    property real progressCenterWidth: 0.0
    property real progressRightWidth: 0.0


    property int  updateTrackWidth: 620
    property int  updateProgressLeftWidth: 1
    property int  updateProgressRightWidth: 5
    function __updatePackageProgress(storedSize, totalSize, count, totalCount){
        updataStatus = updateStatusEnum.UpdateProgress
        if (!CommonUtils.isEqual(loader.sourceComponent , undefined) &&
            !CommonUtils.isEqual(loader.sourceComponent , copy)) {
            settingRoot.visible = true
            loader.sourceComponent = copy
        }
        currProgressValue =  parseFloat(storedSize*1.0) / parseFloat(totalSize*1.0)
        progressLeftWidth = (currProgressValue >= 2) ? updateProgressLeftWidth : 0
        progressCenterWidth = Math.max(0,(currProgressValue * updateTrackWidth)-(updateProgressLeftWidth + updateProgressRightWidth))
        progressRightWidth = currProgressValue ? ((currProgressValue >= (updateProgressLeftWidth + updateProgressRightWidth)) ?
                                                      updateProgressRightWidth : Math.max(1,currProgressValue - updateProgressLeftWidth)) : 0
    }


    //5.복사 종료
    function __updatePackageStored() {
        updataStatus = updateStatusEnum.UpdateEnd
        showToast(textArr[textEnum.MapDataCompleteKo])
        completeCopy()
    }

    function showToast(message)  {
        var data  = ({})
        data[CommonConstants.Message] = message
        data[CommonConstants.Position] = CommonConstants.ToastUp
        data[CommonConstants.Duration] = 3 * 1000
        sendEvent(SettingMapUpdateViewEvent.ShowToast ,data)
    }

    function completeCopy() {
        updataStatus = updateStatusEnum.UpdateEnd
        loader.sourceComponent = complete
        settingRoot.visible = true
    }

    //*.에러
    property int gErrno : SettingMapUpdateViewEvent.ErrorTypeNone
    function __updatePackageError(errno) {
        gErrno = errno
        //맵업데이트 중이 아니면...
        if (!(updateStatusEnum.UpdateStart <= updataStatus && updataStatus < updateStatusEnum.UpdateEnd)) {
            return
        }
        switch(errno) {
            case SettingMapUpdateViewEvent.ErrorTypeMediaUnmounted:
               showDialog(textEnum.Unmounted1Ko, textArr[textEnum.NotiTitleKo], textArr[textEnum.Unmounted1Ko], textArr[textEnum.Unmounted2Ko], false)
               break
            case SettingMapUpdateViewEvent.ErrorTypeFileRead:
            case SettingMapUpdateViewEvent.ErrorTypeFileWrite:
            case SettingMapUpdateViewEvent.ErrorTypeFileIntegrity:
            case SettingMapUpdateViewEvent.ErrorTypeUnknown:
               showDialog(textEnum.ErrorInfoKo, textArr[textEnum.NotiTitleKo], textArr[textEnum.ErrorInfoKo], null, false)
               break
        }
    }


    //////////////////////////////////////
    //{Dialog사용
           property int dlgUniqueNum: -1
            function isShowDialog() {
                if (dlgUniqueNum >= 0)
                    return true
                return false
            }

           property var timer : null
           function showDialog(uniqueNum, title,context1, context2, cancel) {
               if (CommonUtils.isEqual(dlgUniqueNum,uniqueNum)) {
                   return
               }

               timer = CommonUtils.destroy(timer)
               timer = CommonUtils.delay(settingRoot, 10, function() {
                   try {
                       sendEvent(SettingMapUpdateViewEvent.RequestViewList)
                       var data = ({})
                       if (visibleCommonDialogView) {
                           data[CommonConstants.ObjectName] = CommonConstants.objectName(ViewRegistry.CommonDialogView)
                           sendEvent(SettingMapUpdateViewEvent.CloseView, data)

                           timer.interval = 100
                           timer.start()
                           return
                       }
                       timer = CommonUtils.destroy(timer)

                       dlgUniqueNum = uniqueNum

                       data = ({})
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
                                type =  CommonConstants.TypeC
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
                       arg [CommonConstants.Caller ] = CommonConstants.objectName(ViewRegistry.SettingMapUpdateView)
                       data[CommonConstants.Arg    ] = arg
                       data[CommonConstants.MessageType] = uniqueNum
                       sendEvent(SettingMapUpdateViewEvent.CreateView, data)
                   } catch(e) {
                   }
               }, false)
           }

           function __closeDialog(yes) {
               var uniqNum = dlgUniqueNum
               closeDialog(uniqNum, yes)
           }

           function closeDialog(uniqNum, yes) {
                if (yes){
                    switch(uniqNum) {
                    case textEnum.CopyMapData1Ko:
                    case textEnum.CopyMapData2Ko:
                        requestPackageStatus()
                        break
                    case textEnum.SameVersion1Ko:
                    case textEnum.SameVersion2Ko:
                        updateStart()
                        break
                    //예가 아닌 확인 버튼임
                    case textEnum.OldVersion1Ko:
                    case textEnum.OldVersion2Ko:
                        hideNcloseByCondition()
                        break
                    case textEnum.CopyError1Ko:
                    case textEnum.CopyError2Ko:
                    case textEnum.Unmounted1Ko:
                    case textEnum.Unmounted2Ko:
                    case textEnum.ErrorInfoKo: //[*주의*]해당하는 팝업의 type이 없어 3라인만 표출되고 있음
                    case textEnum.CentralLocked1Ko:
                    case textEnum.CentralLocked2Ko:
                        updataStatus = updateStatusEnum.Init
                        sendEvent(SettingMapUpdateViewEvent.RequestStopUpdate)
                        hideNcloseByCondition()
                        break
                    case textEnum.InCompleteMapDataKo:
                        sendEvent(SettingMapUpdateViewEvent.RequestIgnoreMapUpdate)
                        hideNcloseByCondition()
                        break
                    }
                } else {
                    switch(uniqNum) {
                    case textEnum.CopyMapData1Ko:
                    case textEnum.CopyMapData2Ko:
                    case textEnum.SameVersion1Ko:
                    case textEnum.SameVersion2Ko:
                        sendEvent(SettingMapUpdateViewEvent.RequestCancelUpdate)
                        hideNcloseByCondition()
                        break
                    }
                }
                if (CommonUtils.isEqual(uniqNum, dlgUniqueNum)) {
                    dlgUniqueNum = -1
                }
            }
    //}Dialog사용
    //////////////////////////////////////


    //main: activate() 또는 마운트 시그널(__updatePackageMounted())
    Rectangle {
        width: parent.width
        height: parent.height
        color: GlobalConstants.kBasicBackgoundColor
        Loader {
             id: loader
             width: parent.width
             height:parent.height
        }
    }

    //업데이트가 있음을 표시
    Component {
           id: nomapdata
           Item {
               Column {width:parent.width;height:parent.height
                   Item {width:parent.width;height:160}
                   Item {width:parent.width;height:102
                       Image {
                           width: 102
                           height: 102
                           source : Resource.imageUrl("etc_ic_info.png")
                           sourceSize.width: width
                           sourceSize.height: height
                           anchors.centerIn: parent
                       }
                   }
                   Item {width:parent.width;height:68}
                   Item {width:parent.width;height:32
                       Text {
                           width: parent.width
                           height: parent.height
                           horizontalAlignment: Qt.AlignHCenter
                           verticalAlignment: Qt.AlignVCenter
                           font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(mainMessageFontSize)}
                           color: GlobalConstants.kListTextColor
                           text : (CommonUtils.isIndexOf(caller, [textArr[textEnum.NoMapData]])) ? textArr[textEnum.NoMapInfo1st] : textArr[textEnum.IncompatibleMapInfo1st]
                       }
                   }
                   Item {width:parent.width;height:36}
                   Item {width:parent.width;height:(5*34)
                       Text {
                           width: parent.width
                           height: parent.height
                           horizontalAlignment: Qt.AlignHCenter
                           verticalAlignment: Qt.AlignVCenter
                           font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(subMessageFontSize)}
                           color: itemColor
                           text : (CommonUtils.isIndexOf(caller, [textArr[textEnum.NoMapData]])) ? textArr[textEnum.NoMapInfo2nd] : textArr[textEnum.IncompatibleMapInfo2nd]
                           lineHeightMode: Text.FixedHeight
                           lineHeight    : settingRoot.lineHeight
                       }
                   }
                   Item {width:parent.width;height:parent.height;Component.onCompleted:CommonUtils.applyRemainHeight(this,parent)}
               }
           }
    }


    //버전 표시
    Component {
           id: version
           Item {
               property int minusCount: settingsEnum.LocalMapVersion
               property int startIndex: settingsEnum.LocalMapVersion
               Column {
                   width: parent.width
                   height: parent.height
                   Column {width:parent.width;height:parent.height
                       Item {width:parent.width;height:parent.height;Component.onCompleted: CommonUtils.applyRemainHeight(this,parent)}
                       Item {width:parent.width;height:102
                           Image {
                               width: 102
                               height: 102
                               source : Resource.imageUrl("etc_ic_info.png")
                               sourceSize.width: width
                               sourceSize.height: height
                               anchors.centerIn: parent
                           }
                       }
                       Item {width:parent.width;height:48}
                       Item {width:parent.width;height:32
                           Text {
                               width: parent.width
                               height: parent.height
                               horizontalAlignment: Qt.AlignHCenter
                               verticalAlignment: Qt.AlignVCenter
                               font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(mainMessageFontSize)}
                               color: GlobalConstants.kListTextColor
                               text : textArr[textEnum.VersionInfo1st]
                           }
                       }
                       Item {width:parent.width;height:26}
                       Item {width:parent.width;height:62
                           Text {
                               width: parent.width
                               height: parent.height
                               horizontalAlignment: Qt.AlignHCenter
                               verticalAlignment: Qt.AlignVCenter
                               font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(subMessageFontSize)}
                               color: itemColor
                               text : textArr[textEnum.VersionInfo2nd]
                               lineHeightMode: Text.FixedHeight
                               lineHeight: settingRoot.lineHeight
                           }
                       }
                       Item {width:parent.width;height:81}
                       Item {width:parent.width;height:242
                           Image {
                               width: 720
                               height: parent.height
                               source : Resource.imageUrl("setting", "etc_box_bg.png")
                               sourceSize.width: width
                               sourceSize.height: height
                               anchors.centerIn:  parent
                               Column {width:660;height:parent.height;anchors.centerIn: parent
                                   Item {width:parent.width;height:28}
                                   Repeater {
                                       model: (settingsArr.length - startIndex)
                                       delegate:
                                           Column {width:parent.width;height:53
                                               Item {width:parent.width;height:28
                                                   Row {
                                                       width: parent.width
                                                       height: parent.height
                                                       Text {
                                                           width: parent.width/2
                                                           height: parent.height
                                                           horizontalAlignment: Qt.AlignLeft
                                                           verticalAlignment: Qt.AlignVCenter
                                                           font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(subMessageFontSize)}
                                                           color: itemColor
                                                           text : settingsArr[index + startIndex]
                                                       }
                                                       Text {
                                                           width: parent.width/2
                                                           height: parent.height
                                                           horizontalAlignment: Qt.AlignRight
                                                           verticalAlignment: Qt.AlignVCenter
                                                           font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(subMessageFontSize)}
                                                           color: GlobalConstants.kListTextColor
                                                           text : settings[index + startIndex]
                                                       }
                                                   }
                                               }
                                               Item {width:parent.width;height:10}
                                               Rectangle {width:parent.width;height:(!CommonUtils.isEqual(index , (settingsArr.length-(minusCount+1)))) ? 1: 0
                                                        color:GlobalConstants.kBasicLineColor;anchors.horizontalCenter:parent.horizontalCenter
                                                        visible:!CommonUtils.isEqual(index , (settingsArr.length-(minusCount+1)))
                                               }
                                               Item {width:parent.width;height:14}
                                       }//Column
                                   }
                               }
                           }
                       }
                       Item {width:parent.width;height:40}
                   }
               }//Column
           }
    }


    //업데이트(복사) 진행
    Component {
           id: copy
           Item {
               property color progressColor: Qt.rgba(153/255.0, 217/255.0, 234/255.0, 1.0)
               property color trackColor: CommonConstants.Gray

               Column {width:parent.width;height:parent.height
                   Item {width:parent.width;height:160}
                   Item {width:parent.width;height:102
                       Image {
                           width: 102
                           height: 102
                           source : Resource.imageUrl("etc_ic_info.png")
                           sourceSize.width: width
                           sourceSize.height: height
                           anchors.centerIn: parent
                       }
                   }
                   Item {width:parent.width;height:68}
                   Item {width:parent.width;height:32
                       Text {
                           width: parent.width
                           height: parent.height
                           horizontalAlignment: Qt.AlignHCenter
                           verticalAlignment: Qt.AlignVCenter
                           font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(mainMessageFontSize)}
                           color: GlobalConstants.kListTextColor
                           text : textArr[textEnum.CopyInfo1st]
                       }
                   }
                   Item {width:parent.width;height:36}
                   Item {width:parent.width;height:parent.height
                       Component.onCompleted: CommonUtils.applyRemainHeight(this,parent)
                       Text {
                           width: parent.width
                           height: parent.height
                           horizontalAlignment: Qt.AlignHCenter
                           verticalAlignment: Qt.AlignVCenter
                           font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(subMessageFontSize)}
                           color: itemColor
                           text : (CommonUtils.isIndexOf(caller, [textArr[textEnum.NoMapData], textArr[textEnum.IncompatibleMapData]])) ? textArr[textEnum.CopyInfo2ndNoMapData] : textArr[textEnum.CopyInfo2nd]
                           lineHeightMode: Text.FixedHeight
                           lineHeight    : settingRoot.lineHeight
                       }
                   }
                   Item {width:parent.width;height:58}
                   Item {width:parent.width;height:6
                       Image {width:updateTrackWidth;height:parent.height;clip:true;anchors.centerIn: parent; source: Resource.imageUrl("setting","etc_progress_bg.png")
                           Row {width:parent.width;height:parent.height
                               Image {width:progressLeftWidth;height:parent.height;source:Resource.imageUrl("setting","etc_progress_left.png")}
                               Image {width:progressCenterWidth;height:parent.height;source:Resource.imageUrl("setting","etc_progress_center.png")}
                               Image {width:progressRightWidth;height:parent.height;source:Resource.imageUrl("setting","etc_progress_right.png")}
                           }
                       }
                   }
                   Item {width:parent.width;height:110}
               }
           }
    }

    //업데이트(복사) 완료
    Component {
           id: complete
           Item {
               Column {width:parent.width;height:parent.height
                   Item {width:parent.width;height:160}
                   Item {width:parent.width;height:102
                       Image {
                           width: 102
                           height: 102
                           source : Resource.imageUrl("etc_ic_info.png")
                           sourceSize.width: width
                           sourceSize.height: height
                           anchors.centerIn: parent
                       }
                   }
                   Item {width:parent.width;height:68}
                   Item {width:parent.width;height:32
                       Text {
                           width: parent.width
                           height: parent.height
                           horizontalAlignment: Qt.AlignHCenter
                           verticalAlignment: Qt.AlignVCenter
                           font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(mainMessageFontSize)}
                           color: GlobalConstants.kListTextColor
                           text : textArr[textEnum.CompleteInfo1st]
                       }
                   }
                   Item {width:parent.width;height:36}
                   Item {width:parent.width;height:(2*34)
                       Text {
                           width: parent.width
                           height: parent.height
                           horizontalAlignment: Qt.AlignHCenter
                           verticalAlignment: Qt.AlignVCenter
                           font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(subMessageFontSize)}
                           color: itemColor
                           text : textArr[textEnum.CompleteInfo2nd]
                       }
                   }
                   Item {width:parent.width;height:parent.height;Component.onCompleted:CommonUtils.applyRemainHeight(this,parent)}
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
