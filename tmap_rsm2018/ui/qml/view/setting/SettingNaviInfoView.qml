import QtQuick 2.0
import ViewRegistry 1.0
import CommonConstants 1.0
import ZoneConstants 1.0
import SettingConstants 1.0
import SettingNaviInfoViewEvent 1.0
import Csw 1.0

import "../../component"
import "../../view"
import "../../view/util"
import "../../view/setting"
import "../../GlobalConstants.js" as GlobalConstants
import "../../view/util/CommonUtils.js" as CommonUtils

SettingView {
    id: settingRoot

    property int naviInfoFontSize       : 30

    //언어 관련
    property var __lang                 : null
    property bool needInit              : true


    property var caller                 : null
    property var settings               : null
    property bool isActivated           : false

    property color itemColor            : GlobalConstants.kListSubTextColor

    property bool focused               : false
    property color highlightColor       : CommonConstants.Transparent

    //ID

    //Event
    property var textEnum               : ({})
    property var settingsEnum           : ({})

    property var textArr                : ([])
    property var settingsArr            : ([])

    // for hidden setting view
    property bool isReadyLogin          : false

    function allocateVariant() {        
    }

   function allocateTextEnum(){
       if (CommonUtils.getObjectLen(textEnum) <= 0) {
            textEnum = CommonUtils.createEnum(textEnum)
       }
       var i = 0
        CommonUtils.insertEnum(textEnum, "Company",          i++, [CommonConstants.Name], [qsTr("지도성과심업체: SK텔레콤")])
        CommonUtils.insertEnum(textEnum, "ReviewL",          i++, [CommonConstants.Name], [qsTr("공간정보산업협회 심사필: 제 2012-018호")])
        CommonUtils.insertEnum(textEnum, "ReviewR",          i++, [CommonConstants.Name], [qsTr("심사년월일: 2012.05.09")])
        CommonUtils.insertEnum(textEnum, "Debug",            i++, [CommonConstants.Name], [qsTr("디버그뷰")])
        CommonUtils.insertEnum(textEnum, "Zone1Title",       i++, [CommonConstants.Name], [qsTr("내비게이션 정보")])

       CommonUtils.fetchArr(textEnum, CommonConstants.Name, textArr)
    }

    function allocateSettingsEnum(){
        if (CommonUtils.getObjectLen(settingsEnum) <= 0) {
            settingsEnum = CommonUtils.createEnum(settingsEnum)
        }
        CommonUtils.insertEnum(settingsEnum, "ProgramVersion",      SettingNaviInfoViewEvent.ResultAppVersion,         [CommonConstants.Name], [qsTr("프로그램 버전")])
        CommonUtils.insertEnum(settingsEnum, "MapVersion",          SettingNaviInfoViewEvent.ResultMapVersion,         [CommonConstants.Name], [qsTr("서버 지도 버전")])
        CommonUtils.insertEnum(settingsEnum, "LocalMapVersion",     SettingNaviInfoViewEvent.ResultLocalMapVersion,    [CommonConstants.Name], [qsTr("내장 지도 버전")])
        CommonUtils.insertEnum(settingsEnum, "LocalNetworkVersion", SettingNaviInfoViewEvent.ResultLocalNetworkVersion,[CommonConstants.Name], [qsTr("내장 전국 도로 버전")])
        CommonUtils.insertEnum(settingsEnum, "LocalPoiVersion",     SettingNaviInfoViewEvent.ResultLocalPoiVersion,    [CommonConstants.Name], [qsTr("내장 명칭 데이타 버전")])
        CommonUtils.insertEnum(settingsEnum, "SafeDriveVersion",    SettingNaviInfoViewEvent.ResultSDIVersion,         [CommonConstants.Name], [qsTr("안전 운전 버전")])

        CommonUtils.fetchArr(settingsEnum, CommonConstants.Name, settingsArr)
    }

    function init(){
        if (!needInit)
            return
        needInit = false

        allocateVariant()

        allocateSoftKeyEnum()
        allocateTextEnum()
        allocateSettingsEnum()

        if (active) {
            sendEvent(SettingNaviInfoViewEvent.SelectZone1Title, displayText(textEnum.Zone1Title))
            requestSetting()
        }
    }

    onMediated: {
        sendEvent(SettingNaviInfoViewEvent.RequestLang)
        init()
    }

    onActivated: {
        init()
        sendEvent(SettingNaviInfoViewEvent.ResetCsw)
        sendEvent(SettingNaviInfoViewEvent.SelectZone1Title, displayText(textEnum.Zone1Title))
        sendEvent(SettingNaviInfoViewEvent.Activated, [CommonConstants.NoOption])

        if (isActivated)
            return
        isActivated = true

        requestSetting()
    }

    onDeactivated: {
        sendEvent(SettingNaviInfoViewEvent.Deactivated)
    }

    function requestSetting() {
        sendEvent(SettingNaviInfoViewEvent.RequestSetting)
    }

    function back() {
        var data = ({})
        data[CommonConstants.ObjectName] = CommonConstants.objectName(ViewRegistry.SettingNaviInfoView)
        sendEvent(SettingNaviInfoViewEvent.CloseView, data)
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
        switch(button) {
          case softKeyMenu[softKeyEnum.Back]:
              back()
              break
          case softKeyMenu[softKeyEnum.Option]:
          case softKeyMenu[softKeyEnum.Navi]:
          case softKeyMenu[softKeyEnum.Menu]:
          default:
              break
        }
    }

    onSettingsChanged: {
        if (CommonUtils.isNull(settings))
            return
        loader.sourceComponent = undefined
        loader.sourceComponent = main
    }

    function displayText(seq) {
        if (CommonUtils.isNull(textArr) || textArr.length <= seq)
            return CommonConstants.Empty
        return textArr[seq]
    }

    function displaySettingsText(seq) {
        if (CommonUtils.isNull(settingsArr) || settingsArr.length <= seq)
            return CommonConstants.Empty
        return settingsArr[seq]
    }

    function displaySettingsValue(seq) {
        if (CommonUtils.isNull(settings) || settings.length <= seq)
            return CommonConstants.Empty
        return settings[seq]
    }

    function openHiddenMenuLoginView() {
        var data = ({})
        data[CommonConstants.Url] = CommonConstants.url(ViewRegistry.HiddenMenuLoginView)
        sendEvent(SettingNaviInfoViewEvent.CreateView, data )
        back()
    }

    function openHiddenBoschSettingView() {
        var data = {}
        data[CommonConstants.Url] = CommonConstants.url(ViewRegistry.HiddenBoschSettingView)
        sendEvent(SettingNaviInfoViewEvent.CreateView, data )
        back()
    }


    //main
    Loader {
         id: loader
         width: parent.width
         height:parent.height
    }

    Component {
        id: main
        Rectangle {
            onParentChanged: {
                if (CommonUtils.isNull(parent))
                    return
                width = parent.width
                height = parent.height
            }
            color: GlobalConstants.kBasicBackgoundColor
            Row {
                width: parent.width
                height: parent.height
                Item {width:82;height:parent.height}
                Item {width:parent.width;height:parent.height
                    Component.onCompleted: CommonUtils.applyRemainWidth(this,parent)
                    Column {
                        width: parent.width
                        height: parent.height
                        Item {width:parent.width;height:82}
                        Item {width:parent.width;height:318
                            Column {
                                width: 660
                                height: parent.height
                                anchors.centerIn: parent
                                Repeater {
                                    model: settingsArr.length - 1//서버 지도 버전
                                    delegate:
                                        Column {width:parent.width;height:53
                                            Row {
                                                width: parent.width
                                                height: 28
                                                Text {
                                                    width: parent.width/2
                                                    height: parent.height
                                                    horizontalAlignment: Qt.AlignLeft
                                                    verticalAlignment: Qt.AlignVCenter
                                                    font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(naviInfoFontSize)}
                                                    color: itemColor
                                                    text : settingsArr[(index) ? index + 1 : index]
                                                }
                                                Text {
                                                    width: parent.width/2
                                                    height: parent.height
                                                    horizontalAlignment: Qt.AlignRight
                                                    verticalAlignment: Qt.AlignVCenter
                                                    font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(naviInfoFontSize)}
                                                    color: GlobalConstants.kListTextColor
                                                    text : settings[(index) ? index + 1 : index]
                                                }
                                            }
                                            Item {width:parent.width;height:10}
                                            Rectangle {width: parent.width;height: 1;color: GlobalConstants.kListLineColor;visible:!CommonUtils.isEqual(index , (settingsArr.length-(1+1/*서버 지도 버전*/)))}
                                            Item {width:parent.width;height:14}
                                        }//Item
                                }//Repeater
                            }//Column
                        }//Item
                        Item {width:parent.width;height:parent.height
                            Component.onCompleted: CommonUtils.applyRemainHeight(this,parent)
                        }
                        Item {width:parent.width;height:58
                            Column {
                                width: 660
                                height: parent.height
                                anchors.centerIn: parent
                                Item {width:parent.width;height:24
                                    Row {width:parent.width;height:parent.height
                                        Image {width:13;height:parent.height;source:Resource.imageUrl("setting", "info_dot.png")}
                                        Text {
                                            width: parent.width
                                            height: 24
                                            Component.onCompleted: CommonUtils.applyRemainWidth(this,parent)
                                            horizontalAlignment: Qt.AlignLeft
                                            verticalAlignment: Qt.AlignVCenter
                                            font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(25)}
                                            color: itemColor
                                            text : displayText(textEnum.Company)
                                        }
                                    }
                                }
                                Item {width:parent.width;height:10}
                                Item {width:parent.width;height:24
                                    Row {width:parent.width;height:parent.height
                                        Image {width:13;height:24;source:Resource.imageUrl("setting", "info_dot.png")}
                                        Text {
                                            height: parent.height
                                            horizontalAlignment: Qt.AlignLeft
                                            verticalAlignment: Qt.AlignVCenter
                                            font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(25)}
                                            color: itemColor
                                            text : displayText(textEnum.ReviewL)
                                        }
                                        Item {width:30;height:parent.height}
                                        Text {
                                            height: parent.height
                                            horizontalAlignment: Qt.AlignLeft
                                            verticalAlignment: Qt.AlignVCenter
                                            font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(25)}
                                            color: itemColor
                                            text : displayText(textEnum.ReviewR)
                                        }
                                    }
                                }
                            }
                        }
                        Item {width:parent.width;height:58}
                    }
                }
                Item {width:80;height:parent.height}
            }

            // enter hidden diagnostic menu
            MouseArea {
                id: hiddenShuffleBoard
                anchors.fill: parent
                property int shuffleCnt: 0
                // property int krokorCnt: 0
                property bool isShuffling: false
                property var beginTime
                property int resetTime: 2000 // reset after 3 seconds
                onPressed: {
                    if (!isReadyLogin) {
                        if (!isShuffling) {
                            isShuffling = true
                            shuffleCnt = 0
                        }
                    beginTime = CommonUtils.getTime()
                    }
                }

                onReleased: {
                    console.log(" navi info tab time:"+parseInt(CommonUtils.getTime() - beginTime))
                    if (isShuffling) {
                        if (CommonUtils.getTime() - beginTime <= resetTime) {
                            shuffleCnt++
                        }
                        else if (shuffleCnt > 3 ) { // enable login state if it has long pressed in more than 3
                            isReadyLogin = true
                            isShuffling = false
                            shuffleCnt = 0
                        } else {
                            isReadyLogin = false
                            isShuffling  = false
                            shuffleCnt   = 0
                            krokorCnt    = 0
                        }

                        
                    }
                    else if (isReadyLogin)
                    {
                        isReadyLogin = false
                        openHiddenMenuLoginView();
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



