import QtQuick 2.0
import ViewRegistry 1.0
import CommonConstants 1.0
import ZoneConstants 1.0
import SettingConstants 1.0
import SettingGpsInfoViewEvent 1.0
import Csw 1.0

import "../../component"
import "../../view"
import "../../view/util"
import "../../view/setting"
import "../../GlobalConstants.js" as GlobalConstants
import "../../view/util/CommonUtils.js" as CommonUtils

SettingView {
    id: settingRoot


    //언어 관련
    property var __lang             : null
    property bool needInit          : true

    property var caller             : null
    property var settings           : null

    property var objSatellite       : []

    property string dateText        : CommonConstants.Empty
    property string timeText        : CommonConstants.Empty
    property string sateliteText    : CommonConstants.Empty
    property string longitudeText   : CommonConstants.Empty
    property string latitudeText    : CommonConstants.Empty
    property string modeText        : CommonConstants.Empty
    property string speedText       : CommonConstants.Empty
    property string altitudeText    : CommonConstants.Empty
    property string hoodText        : CommonConstants.Empty
    property color itemColor        : GlobalConstants.kListSubTextColor


    property bool focused           : false
    property color highlightColor   : CommonConstants.Transparent

    property var timer              : null

    //ID
    property var rader              : null
    property var loaderSatellite    : null

    //Enum
    property var textEnum           : ({})
    property var textArr            : ([])

    function allocateVariant() {
    }

    function allocateTextEnum(){
        if (CommonUtils.getObjectLen(textEnum) <= 0) {
            textEnum = CommonUtils.createEnum(textEnum)
        }
        var i = 0
        CommonUtils.insertEnum(textEnum, "Zone1Title",              i++, [CommonConstants.Name], [qsTr("GPS 정보")])
        CommonUtils.insertEnum(textEnum, "Year",                    i++, [CommonConstants.Name], [qsTr("년")])
        CommonUtils.insertEnum(textEnum, "Month",                   i++, [CommonConstants.Name], [qsTr("월")])
        CommonUtils.insertEnum(textEnum, "Day",                     i++, [CommonConstants.Name], [qsTr("일")])
        CommonUtils.insertEnum(textEnum, "Hour",                    i++, [CommonConstants.Name], [qsTr("시")])
        CommonUtils.insertEnum(textEnum, "Minute",                  i++, [CommonConstants.Name], [qsTr("분")])
        CommonUtils.insertEnum(textEnum, "Second",                  i++, [CommonConstants.Name], [qsTr("초")])
        CommonUtils.insertEnum(textEnum, "Count",                   i++, [CommonConstants.Name], [qsTr("개")])
        CommonUtils.insertEnum(textEnum, "Speed",                   i++, [CommonConstants.Name], [qsTr("Km/h")])
        CommonUtils.insertEnum(textEnum, "Meter",                   i++, [CommonConstants.Name], [qsTr("m")])
        CommonUtils.insertEnum(textEnum, "ItemDate",                i++, [CommonConstants.Name], [qsTr("날짜")])
        CommonUtils.insertEnum(textEnum, "ItemHour",                i++, [CommonConstants.Name], [qsTr("시간")])
        CommonUtils.insertEnum(textEnum, "ItemSatelliteCount",      i++, [CommonConstants.Name], [qsTr("위성수")])
        CommonUtils.insertEnum(textEnum, "ItemLongitude",           i++, [CommonConstants.Name], [qsTr("경도")])
        CommonUtils.insertEnum(textEnum, "ItemLatitude",            i++, [CommonConstants.Name], [qsTr("위도")])
        CommonUtils.insertEnum(textEnum, "ItemMode",                i++, [CommonConstants.Name], [qsTr("측위방식")])
        CommonUtils.insertEnum(textEnum, "ItemSpeed",               i++, [CommonConstants.Name], [qsTr("속도")])
        CommonUtils.insertEnum(textEnum, "ItemAltitude",            i++, [CommonConstants.Name], [qsTr("고도")])
        CommonUtils.insertEnum(textEnum, "ItemHdop",                i++, [CommonConstants.Name], [qsTr("HDOP")])

        CommonUtils.fetchArr(textEnum, CommonConstants.Name, textArr)
    }

    function init(){
        if (!needInit)
            return
        needInit = false

        allocateVariant()
        allocateSoftKeyEnum()
        allocateTextEnum()

        loader.sourceComponent = undefined
        loader.sourceComponent = main
        if (active) {
            sendEvent(SettingGpsInfoViewEvent.SelectZone1Title, textArr[textEnum.Zone1Title])
            requestSetting()
        }
    }


    onMediated: {
        sendEvent(SettingGpsInfoViewEvent.RequestLang)
        init()
    }

    onActivated: {
        init()
        sendEvent(SettingGpsInfoViewEvent.ResetCsw)
        sendEvent(SettingGpsInfoViewEvent.SelectZone1Title, textArr[textEnum.Zone1Title])
        sendEvent(SettingGpsInfoViewEvent.Activated, [CommonConstants.NoOption])
        requestSetting()
    }

    onDeactivated: {
        sendEvent(SettingGpsInfoViewEvent.Deactivated)
    }

    Component.onDestruction: {
        timer = CommonUtils.destroy(timer)
    }

    function  getGPSPosInSky(width,  elevation, azimuth) {
        var iCenterx
        var iCentery
        var ret = []
        iCenterx = iCentery = width/2
        var h = parseFloat((Math.cos(elevation*(Math.PI/180)) * iCentery) * 1.0)
        ret[0] = parseInt(iCenterx + (h * Math.sin(azimuth*(Math.PI/180))))
        ret[1] = parseInt(iCentery - (h * Math.cos(azimuth*(Math.PI/180))))
        return ret
    }

    function loadSatelliteComponent(count) {
        loaderSatellite.satellitesInUse = count
        loaderSatellite.sourceComponent = satelliteRepeater
    }

    function requestSetting() {
        timer = CommonUtils.destroy(timer)
        timer = CommonUtils.delay(settingRoot, 0, function() {
            try {
                settings = null
                sendEvent(SettingGpsInfoViewEvent.RequestSetting)
                timer.interval = 1*1000
                timer.start()
            } catch(e) {
            }
        }, false)
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
              data[CommonConstants.ObjectName] = CommonConstants.objectName(ViewRegistry.SettingGpsInfoView)
              sendEvent(SettingGpsInfoViewEvent.CloseView, data)
              break
          case softKeyMenu[softKeyEnum.Option]:
          case softKeyMenu[softKeyEnum.Navi]:
          case softKeyMenu[softKeyEnum.Menu]:
              break
        }
    }

    onSettingsChanged: {
        if (CommonUtils.isNull(settings))
            return
        var dateY = settings[SettingConstants.GPSInfoDateY]
        var dateM = settings[SettingConstants.GPSInfoDateM]
        var dateD = settings[SettingConstants.GPSInfoDateD]
        var timeH = settings[SettingConstants.GPSInfoTimeH]
        var timeM = settings[SettingConstants.GPSInfoTimeM]
        var timeS = settings[SettingConstants.GPSInfoTimeS]
        if (CommonUtils.number(dateY) <= 0 &&
            CommonUtils.number(dateM) <= 0 &&
            CommonUtils.number(dateD) <= 0 ) {
            dateText = "----" + textArr[textEnum.Year]
                     + "--"   + textArr[textEnum.Month]
                     + "--"   + textArr[textEnum.Day]
        } else {
            dateText = dateY + textArr[textEnum.Year]
                     + dateM + textArr[textEnum.Month]
                     + dateD + textArr[textEnum.Day]
        }

        if (CommonUtils.number(timeH) < 0 &&
            CommonUtils.number(timeM) < 0 &&
            CommonUtils.number(timeS) < 0 ) {
            timeText = "--" + textArr[textEnum.Hour]
                     + "--" + textArr[textEnum.Minute]
                     + "--" + textArr[textEnum.Second]
        } else {
            timeText = timeH + textArr[textEnum.Hour]
                     + timeM + textArr[textEnum.Minute]
                     + timeS + textArr[textEnum.Second]
        }
        sateliteText = settings[SettingConstants.GPSInfoSVCnt] + textArr[textEnum.Count]

        longitudeText = settings[SettingConstants.GPSInfoCpxD] + ":" + settings[SettingConstants.GPSInfoCpxM] + ":" + settings[SettingConstants.GPSInfoCpxS].toFixed(2)
        latitudeText  = settings[SettingConstants.GPSInfoCpyD] + ":" + settings[SettingConstants.GPSInfoCpyM] + ":" + settings[SettingConstants.GPSInfoCpyS].toFixed(2)

        modeText = settings[SettingConstants.GPSInfoMode]
        speedText = settings[SettingConstants.GPSInfoSpeed] + textArr[textEnum.Speed]
        altitudeText = settings[SettingConstants.GPSInfAltitude] + textArr[textEnum.Meter]
        hoodText = settings[SettingConstants.GPSInfodHdop]
        hoodText = CommonUtils.string(hoodText)
        var s = CommonUtils.split(hoodText, ".")
        if (s.length > 1)  {
            if (s[s.length - 1].length>2) {
                hoodText = settings[SettingConstants.GPSInfodHdop].toFixed(2)
            }
        }

        //위성 갯수만큼 컴포넌트 로딩
        loadSatelliteComponent(settings[SettingConstants.GPSSVInfo].length)


        var agoGpsL = 0
        var agoGpsT = 0
        var notGpsRadar = 0
        var infoList = settings[SettingConstants.GPSSVInfo]
        var count = infoList.length
        for(var i = 0; i<count; i++) {
            var info = infoList[i]
            var id = info[SettingConstants.GPSSVinfoId]
            var db = info[SettingConstants.GPSSVinfoDb]
            var elevation = info[SettingConstants.GPSSVinfoElevation]
            var azimuth = info[SettingConstants.GPSSVinfoAzimuth]

            // 해당 X, Y 좌표 가져오기
            var coord = getGPSPosInSky(rader.width - (rader.width / 10),    elevation,    azimuth)
            if(db > 30)   {
                //녹색계통
                if((CommonUtils.isEqual(coord[0] , 0) && CommonUtils.isEqual(coord[1] , 0) )||( CommonUtils.isEqual(coord[0] , agoGpsL) && CommonUtils.isEqual(coord[1] , agoGpsT))){
                    notGpsRadar++
                } else{
                    agoGpsL=coord[0]
                    agoGpsT=coord[1]
                    if (!CommonUtils.isNull(objSatellite[i] )) {
                        objSatellite[i].source = Resource.imageUrl("setting", "gps_on_g.png")
                        objSatellite[i].visible = true
                        objSatellite[i].x =  coord[0]
                        objSatellite[i].y = coord[1]
                    }
                }
            } else {
                //붉은색개통
                if(( CommonUtils.isEqual(coord[0] , 0) && CommonUtils.isEqual(coord[1] ,  0))|| ( CommonUtils.isEqual(coord[0] , agoGpsL) && CommonUtils.isEqual(coord[1] , agoGpsT) )){
                    notGpsRadar++
                } else {
                    agoGpsL=coord[0]
                    agoGpsT=coord[1]
                    if (!CommonUtils.isNull(objSatellite[i])) {
                        objSatellite[i].source = Resource.imageUrl("setting", "gps_on_r.png")
                        objSatellite[i].visible = true
                        objSatellite[i].x =  coord[0]
                        objSatellite[i].y =  coord[1]
                    }
                }
            }
        }
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
            Column {
                width: parent.width
                height: parent.height
                Item {width:parent.width;height: 108}
                Item {width:parent.width;height: 267
                    Row {width:parent.width;height:parent.height
                        Item {width:90;height:parent.height}
                        Item {width:267;height:parent.height
                            Image {
                                Component.onCompleted: settingRoot.rader = this
                                anchors.centerIn: parent
                                source : Resource.imageUrl("setting", "gps_bg.png")
                                sourceSize.width :  settingRoot.layout.gpsImgSize
                                sourceSize.height: settingRoot.layout.gpsImgSize
                                width: sourceSize.width
                                height: sourceSize.height
                                Loader {
                                     Component.onCompleted: settingRoot.loaderSatellite = this
                                     width: parent.width
                                     height: parent.height
                                     property int satellitesInUse : 14
                                }
                            }
                        }
                        Item {width:parent.width;height:parent.height
                            Component.onCompleted: CommonUtils.applyRemainWidth(this,parent)
                        }
                        Item {width:293;height:parent.height
                            Column {width:parent.width;height:parent.height
                                Item {width:parent.width;height: 57}
                                Item {width:parent.width;height: 34
                                    Row {width:parent.width;height:parent.height
                                        Item {width:80;height:parent.height
                                            Text {
                                                width: parent.width
                                                height: parent.height
                                                horizontalAlignment: Qt.AlignLeft
                                                verticalAlignment: Qt.AlignVCenter
                                                font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(GlobalConstants.kOneLineListTextFontSize)}
                                                color: itemColor
                                                text : textArr[textEnum.ItemDate]
                                            }
                                        }
                                        Item {width:13;height:parent.height}
                                        Item {width:200;height:parent.height
                                            Text {
                                                width: parent.width
                                                height: parent.height
                                                horizontalAlignment: Qt.AlignRight
                                                verticalAlignment: Qt.AlignVCenter
                                                font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(GlobalConstants.kOneLineListTextFontSize)}
                                                color: GlobalConstants.kListTextColor
                                                text :  dateText
                                            }
                                        }
                                    }
                                }
                                Item {width:parent.width;height:26}
                                Item {width:parent.width;height:34
                                    Row {width:parent.width;height:parent.height
                                        Item {width:80;height:parent.height
                                            Text {
                                                width: parent.width
                                                height: parent.height
                                                horizontalAlignment: Qt.AlignLeft
                                                verticalAlignment: Qt.AlignVCenter
                                                font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(GlobalConstants.kOneLineListTextFontSize)}
                                                color: itemColor
                                                text : textArr[textEnum.ItemHour]
                                            }
                                        }
                                        Item {width:13;height:parent.height}
                                        Item {width:200;height:parent.height
                                            Text {
                                                width: parent.width
                                                height: parent.height
                                                horizontalAlignment: Qt.AlignRight
                                                verticalAlignment: Qt.AlignVCenter
                                                font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(GlobalConstants.kOneLineListTextFontSize)}
                                                color: GlobalConstants.kListTextColor
                                                text : timeText
                                            }
                                        }
                                    }
                                }
                                Item {width:parent.width;height:26}
                                Item {width:parent.width;height:34
                                    Row {width:parent.width;height:parent.height
                                        Item {width:80;height:parent.height
                                            Text {
                                                width: parent.width
                                                height: parent.height
                                                horizontalAlignment: Qt.AlignLeft
                                                verticalAlignment: Qt.AlignVCenter
                                                font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(35)}
                                                color: itemColor
                                                text : textArr[textEnum.ItemSatelliteCount]
                                            }
                                        }
                                        Item {width:13;height:parent.height}
                                        Item {width:200;height:parent.height
                                            Text {
                                                width: parent.width
                                                height: parent.height
                                                horizontalAlignment: Qt.AlignRight
                                                verticalAlignment: Qt.AlignVCenter
                                                font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(35)}
                                                color: GlobalConstants.kListTextColor
                                                text : sateliteText
                                            }
                                        }
                                    }
                                }
                                Item {width:parent.width;height:parent.height
                                    Component.onCompleted: CommonUtils.applyRemainHeight(this,parent)
                                }
                            }
                        }
                        Item {width:80;height:parent.height}
                    }
                }
                Item {width:parent.width;height:108
                    Component.onCompleted: CommonUtils.applyRemainHeight(this,parent)
                }
                Item {width:parent.width;height:190
                    Row {width:parent.width;height:parent.height
                        Item {width:50;height:parent.height}
                        Item {width:parent.width;height:parent.height
                            Component.onCompleted: CommonUtils.applyRemainWidth(this,parent)
                            Image {
                                width: parent.width
                                height: parent.height
                                sourceSize.width: width
                                sourceSize.height: height
                                source : Resource.imageUrl("setting", "etc_box_bg2.png")
                                Row {width:parent.width;height:parent.height
                                    Item {width:40;height:parent.height}
                                    Item {width:94;height:parent.height
                                        Column {width:parent.width;height:parent.height
                                            Item {width:parent.width;height:30}
                                            Item {width:parent.width;height:28

                                                Text {
                                                    width: parent.width
                                                    height: parent.height
                                                    horizontalAlignment: Qt.AlignLeft
                                                    verticalAlignment: Qt.AlignVCenter
                                                    font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(30)}
                                                    color: itemColor
                                                    text : textArr[textEnum.ItemLongitude]
                                                }
                                            }
                                            Item {width:parent.width;height:24}
                                            Item {width:parent.width;height:28
                                                Text {
                                                    width: parent.width
                                                    height: parent.height
                                                    horizontalAlignment: Qt.AlignLeft
                                                    verticalAlignment: Qt.AlignVCenter
                                                    font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(30)}
                                                    color: itemColor
                                                    text : textArr[textEnum.ItemLatitude]
                                                }
                                            }
                                            Item {width:parent.width;height:24}
                                            Item {width:parent.width;height:28
                                                Text {
                                                    width: parent.width
                                                    height: parent.height
                                                    horizontalAlignment: Qt.AlignLeft
                                                    verticalAlignment: Qt.AlignVCenter
                                                    font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(30)}
                                                    color: itemColor
                                                    text : textArr[textEnum.ItemMode]
                                                }
                                            }
                                            Item {width:parent.width;height:28}
                                        }
                                    }
                                    Item {width:32;height:parent.height}
                                    Item {width:144;height:parent.height
                                        Column {width:parent.width;height:parent.height
                                            Item {width:parent.width;height: 30}
                                            Item {width:parent.width;height: 28
                                                Text {
                                                    width: parent.width
                                                    height: parent.height
                                                    horizontalAlignment: Qt.AlignRight
                                                    verticalAlignment: Qt.AlignVCenter
                                                    font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(30)}
                                                    color: GlobalConstants.kListTextColor
                                                    text : longitudeText
                                                }
                                            }
                                            Item {width:parent.width;height:24}
                                            Item {width:parent.width;height:28
                                                Text {
                                                    width: parent.width
                                                    height: parent.height
                                                    horizontalAlignment: Qt.AlignRight
                                                    verticalAlignment: Qt.AlignVCenter
                                                    font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(30)}
                                                    color: GlobalConstants.kListTextColor
                                                    text : latitudeText
                                                }
                                            }
                                            Item {width:parent.width;height:24}
                                            Item {width:parent.width;height:28
                                                Text {
                                                    width: parent.width
                                                    height: parent.height
                                                    horizontalAlignment: Qt.AlignRight
                                                    verticalAlignment: Qt.AlignVCenter
                                                    font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(30)}
                                                    color: GlobalConstants.kListTextColor
                                                    text : modeText
                                                }
                                            }
                                            Item {width:parent.width;height:28}
                                        }
                                    }
                                    Item {width:50;height:parent.height}
                                    Item {width:1;height:parent.height
                                        Rectangle {width:1;height:126;color: GlobalConstants.kListTextColor; anchors.centerIn:parent;opacity:0.12}
                                    }
                                    Item {width:49;height:parent.height}
                                    Item {width:90;height:parent.height
                                        Column {width:parent.width;height:parent.height
                                            Item {width:parent.width;height:30}
                                            Item {width:parent.width;height:28
                                                Text {
                                                    width: parent.width
                                                    height: parent.height
                                                    horizontalAlignment: Qt.AlignLeft
                                                    verticalAlignment: Qt.AlignVCenter
                                                    font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(30)}
                                                    color: itemColor
                                                    text : textArr[textEnum.ItemSpeed]
                                                }
                                            }
                                            Item {width:parent.width;height:24}
                                            Item {width:parent.width;height:28
                                                Text {
                                                    width: parent.width
                                                    height: parent.height
                                                    horizontalAlignment: Qt.AlignLeft
                                                    verticalAlignment: Qt.AlignVCenter
                                                    font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(30)}
                                                    color: itemColor
                                                    text : textArr[textEnum.ItemAltitude]
                                                }
                                            }
                                            Item {width:parent.width;height:24}
                                            Item {width:parent.width;height:28
                                                Text {
                                                    width: parent.width
                                                    height: parent.height
                                                    horizontalAlignment: Qt.AlignLeft
                                                    verticalAlignment: Qt.AlignVCenter
                                                    font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(30)}
                                                    color: itemColor
                                                    text : textArr[textEnum.ItemHdop]
                                                }
                                            }
                                            Item {width:parent.width;height:28}
                                        }
                                    }
                                    Item {width:32;height:parent.height}
                                    Item {width:144;height:parent.height
                                        Column {width:parent.width;height:parent.height
                                            Item {width:parent.width;height:30}
                                            Item {width:parent.width;height:28
                                                Text {
                                                    width: parent.width
                                                    height: parent.height
                                                    horizontalAlignment: Qt.AlignRight
                                                    verticalAlignment: Qt.AlignVCenter
                                                    font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(30)}
                                                    color: GlobalConstants.kListTextColor
                                                    text : speedText
                                                }
                                            }
                                            Item {width:parent.width;height:24}
                                            Item {width:parent.width;height:28
                                                Text {
                                                    width: parent.width
                                                    height: parent.height
                                                    horizontalAlignment: Qt.AlignRight
                                                    verticalAlignment: Qt.AlignVCenter
                                                    font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(30)}
                                                    color: GlobalConstants.kListTextColor
                                                    text : altitudeText
                                                }
                                            }
                                            Item {width:parent.width;height:24}
                                            Item {width:parent.width;height:28
                                                Text {
                                                    width: parent.width
                                                    height: parent.height
                                                    horizontalAlignment: Qt.AlignRight
                                                    verticalAlignment: Qt.AlignVCenter
                                                    font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(30)}
                                                    color: GlobalConstants.kListTextColor
                                                    text : hoodText
                                                }
                                            }
                                            Item {width:parent.width;height:28}
                                        }
                                    }
                                    Item {width:40;height:parent.height}
                                }
                            }
                        }
                        Item {width:50;height:parent.height}
                    }
                }
                Item {width:parent.width;height:40}
            }
        }
    }


    //위성을 표시하기 위하여..
    Component {
        id: satelliteRepeater
        Repeater {
            model: loaderSatellite.satellitesInUse
            delegate:
                Image {
                    width: 18
                    height: 18
                    source: Resource.imageUrl("setting", "gps_off.png")
                    visible: false
                    Component.onCompleted: settingRoot.objSatellite[index] = this
                }
        }
    }
}


