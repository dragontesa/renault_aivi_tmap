import QtQuick 2.0
import ViewRegistry 1.0
import CommonConstants 1.0
import ZoneConstants 1.0
import SettingConstants 1.0
import SearchConstants 1.0
import SettingAQIdetailViewEvent 1.0
import Csw 1.0

import "../../component"
import "../../view"
import "../../view/util"
import "../../view/setting"
import "../../GlobalConstants.js" as GlobalConstants
import "../../view/util/CommonUtils.js" as CommonUtils


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//참고: https://tde.sktelecom.com/wiki/pages/viewpage.action?spaceKey=TMCRSM&title=RSM2018%20AQI&&
//-----------------------------------------------------------------------------------------------------------------------------------
//요청: https://{DomainName}/tmapv20/getAirQuality?appKey={appKey}&lat={lat}&lon={lon}
//     개발 DomainName: tmcdev.tmap.co.kr
//     상용 DomainName: tmc.tmap.co.kr
//     예: https://tmc.tmap.co.kr/aqi/getAirQuality?appKey=TMAP_RSM_AIVI&lat=35.71221482&lon=126.9885729
//결과: getAirQuality.json
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SettingView {
    id: settingRoot

    property string caller          : CommonConstants.Empty//[주의]Mediator에서 사용 및 qml 호출시 사용하고 있음

    //onMediated에서 요청 및 응답
    property var __lang             : null
    property bool online            : false

    property bool needInit          : true


    //시간
    property date gpsTime           : null


    //검색 결과
    property var result             : ({})
    property var resultCurrentPos   : ({})
    property var resultDestPos      : ({})
    property var resultForecast     : ({})
    property var resultInit         : ({ pm10Value   : null
                                       , pm10Grade1h : 0
                                       , pm10Grade   : 0
                                       , pm10Value24 : null
                                       , pm25Value   : null
                                       , pm25Grade1h : 0
                                       , pm25Grade   : 0
                                       , pm25Value24 : null
                                       , pm10List    : [{pm10Value:null,dataTime:"0000-00-00 00:00"},]
                                       , no2Grade    : 0
                                       , no2Value    : null
                                       , o3Grade     : 0
                                       , o3Value     : null
                                       , khaiGrade   : 0
                                       , khaiValue   : null
                                       , so2Grade    : 0
                                       , so2Value    : null
                                       , coGrade     : 0
                                       , coValue     : null
                                       , dataTime    : "0000-00-00 00:00"
                                       , mangName    : CommonConstants.Empty
                                       , stationName : CommonConstants.Empty
                                       })
    property var resultForecastInit : ({ dataTime    : "0000-00-00 00:00"
                                       , addr        : CommonConstants.Empty
                                       , items       : [ {item : { dataTime : "0000-00-00 00:00", grade : "예보없음" }}
                                                       , {item : { dataTime : "0000-00-00 00:00", grade : "예보없음" }}
                                                       ]
                                       })


    //주기적으로 AQI조회
    property var timer              : null
    property var beforHoure         : null
    property var resultSuccess      : null
    property int requestKind        : SearchConstants.CurrentPos
    property int tryCount           : 0

    property bool reqSearch         : false
    property bool isShowProgressIndicator: false
    property int  availableDiffHour : 12

    property var timerProg          : null

    //ID

    //Event
    property var textEnum           : ({})
    property var resultCodeEnum     : ({})
    property var imgEnum            : ({})
    property var levelEnum          : ({})
    property var pollutantEnum      : ({})

    property var textArr            : ([])
    property var imgArr             : ([])
    property var levelArr           : ([])
    property var levelEngArr        : ([])
    property var levelRgbArr        : ([])
    property var pollutantArr       : ([])
    property var pollutantImgArr    : ([])
    property var pollutantStatusArr : ([])
    property var pollutantValueArr  : ([])

    function allocateVariant() {
        result             = ({})
        resultCurrentPos   = ({})
        resultDestPos      = ({})
        processResultInit()
        processForecastInit()
    }

    function allocateTextEnum(){
       if (CommonUtils.getObjectLen(textEnum) <= 0) {
            textEnum = CommonUtils.createEnum(textEnum)
       }
       var i = 0
        CommonUtils.insertEnum(textEnum, "Zone1Title"           , i++, [CommonConstants.Name], [qsTr("미세먼지")])
        CommonUtils.insertEnum(textEnum ,"Notice"               , i++, [CommonConstants.Name], [qsTr("실시간 관측된 자료이며 현지 사정이나 수신 상태에 의해 차이가 발생할 수 있습니다.")])
        CommonUtils.insertEnum(textEnum, "Current"              , i++, [CommonConstants.Name], [qsTr("현위치")])
        CommonUtils.insertEnum(textEnum, "Destination"          , i++, [CommonConstants.Name], [qsTr("목적지")])
        CommonUtils.insertEnum(textEnum, "DustConcentration"    , i++, [CommonConstants.Name], [qsTr("미세먼지 농도")])
        CommonUtils.insertEnum(textEnum, "Basis"                , i++, [CommonConstants.Name], [qsTr("기준")])
        CommonUtils.insertEnum(textEnum, "Apply"                , i++, [CommonConstants.Name], [qsTr("한국환경공단 제공")])
        CommonUtils.insertEnum(textEnum, "AM"                   , i++, [CommonConstants.Name], [qsTr("오전")])
        CommonUtils.insertEnum(textEnum, "PM"                   , i++, [CommonConstants.Name], [qsTr("오후")])
        CommonUtils.insertEnum(textEnum, "HourKo"               , i++, [CommonConstants.Name], ["시"])
        CommonUtils.insertEnum(textEnum, "AnnounceKo"           , i++, [CommonConstants.Name], ["발표"])
        CommonUtils.insertEnum(textEnum, "AnnounceEn"           , i++, [CommonConstants.Name], ["Announced at"])
        CommonUtils.insertEnum(textEnum, "Colon"                , i++, [CommonConstants.Name], [":"])
        CommonUtils.insertEnum(textEnum, "Slash"                , i++, [CommonConstants.Name], ["/"])
        CommonUtils.insertEnum(textEnum, "Space"                , i++, [CommonConstants.Name], [String.fromCharCode(0x20)])
        CommonUtils.insertEnum(textEnum, "Dot"                  , i++, [CommonConstants.Name], ["."])
        CommonUtils.insertEnum(textEnum, "Eng"                  , i++, [CommonConstants.Name], ["_eng"])

        //레벨
        CommonUtils.insertEnum(textEnum, "Error"                , i++, [CommonConstants.Name], ["-"])
        CommonUtils.insertEnum(textEnum, "Good"                 , i++, [CommonConstants.Name], [qsTr("좋음")])
        CommonUtils.insertEnum(textEnum, "Normal"               , i++, [CommonConstants.Name], [qsTr("보통")])
        CommonUtils.insertEnum(textEnum, "Bad"                  , i++, [CommonConstants.Name], [qsTr("나쁨")])
        CommonUtils.insertEnum(textEnum, "VeryBad"              , i++, [CommonConstants.Name], [qsTr("매우 나쁨")])
        //[주의]: 검색결과의 forecast의 문자열 비교에 사용함
        CommonUtils.insertEnum(textEnum, "ErrorKo"              , i++, [CommonConstants.Name], ["예보없음"])
        CommonUtils.insertEnum(textEnum, "GoodKo"               , i++, [CommonConstants.Name], ["좋음"])
        CommonUtils.insertEnum(textEnum, "NormalKo"             , i++, [CommonConstants.Name], ["보통"])
        CommonUtils.insertEnum(textEnum, "BadKo"                , i++, [CommonConstants.Name], ["나쁨"])
        CommonUtils.insertEnum(textEnum, "VeryBadKo"            , i++, [CommonConstants.Name], ["매우 나쁨"])
        //오염물질
        CommonUtils.insertEnum(textEnum, "Pollutant"            , i++, [CommonConstants.Name], [qsTr("오염물질 지수")])
        CommonUtils.insertEnum(textEnum, "Dust"                 , i++, [CommonConstants.Name], [qsTr("미세먼지")])
        CommonUtils.insertEnum(textEnum, "FineDust"             , i++, [CommonConstants.Name], [qsTr("초미세먼지")])
        CommonUtils.insertEnum(textEnum, "Ozone"                , i++, [CommonConstants.Name], [qsTr("오존")])
        CommonUtils.insertEnum(textEnum, "CarbonMonoxide"       , i++, [CommonConstants.Name], [qsTr("일산화탄소")])
        CommonUtils.insertEnum(textEnum, "SulfurDioxideGas"     , i++, [CommonConstants.Name], [qsTr("아황산가스")])
        CommonUtils.insertEnum(textEnum, "NitrogenDioxide"      , i++, [CommonConstants.Name], [qsTr("이산화질소")])
        CommonUtils.insertEnum(textEnum, "IntegratedWaiting"    , i++, [CommonConstants.Name], [qsTr("통합대기")])
        //미세먼지 예보
        CommonUtils.insertEnum(textEnum, "Today"                , i++, [CommonConstants.Name], [qsTr("오늘")])
        CommonUtils.insertEnum(textEnum, "Tomorrow"             , i++, [CommonConstants.Name], [qsTr("내일")])
        CommonUtils.insertEnum(textEnum, "LocalDustForecast"    , i++, [CommonConstants.Name], [qsTr("지역 미세먼지 예보")])

        CommonUtils.fetchArr(textEnum, CommonConstants.Name, textArr)
   }

   function allocateResultCodeEnum() {
       if (CommonUtils.getObjectLen(resultCodeEnum) <= 0) {
            resultCodeEnum = CommonUtils.createEnum(resultCodeEnum)
       }
       var i = 0
       CommonUtils.insertEnum(resultCodeEnum, "InvalidData" , i++)
       CommonUtils.insertEnum(resultCodeEnum, "CurrentData" , i++)
       CommonUtils.insertEnum(resultCodeEnum, "OldData"     , i++)
       CommonUtils.insertEnum(resultCodeEnum, "NoneData"    , i++)
   }

   function allocateImgEnum() {
       if (CommonUtils.getObjectLen(imgEnum) <= 0) {
            imgEnum = CommonUtils.createEnum(imgEnum)
       }
       var i = 0
       CommonUtils.insertEnum(imgEnum , "Background"            , i++, [CommonConstants.Name] , ["setting/aqi_img_bg.png"])
       CommonUtils.insertEnum(imgEnum , "PositionBG"            , i++, [CommonConstants.Name] , ["setting/aqi_img_position_bg.9.png"])
       CommonUtils.insertEnum(imgEnum , "PositionIC"            , i++, [CommonConstants.Name] , ["setting/aqi_ic_position.png"])
       CommonUtils.insertEnum(imgEnum , "Unit"                  , i++, [CommonConstants.Name] , ["setting/aqi_img_unit.png"])
       CommonUtils.insertEnum(imgEnum , "WidUnit1"              , i++, [CommonConstants.Name] , ["setting/wid_aqi_img_unit1_g.png"])
       CommonUtils.insertEnum(imgEnum , "WidUnit2"              , i++, [CommonConstants.Name] , ["setting/wid_aqi_img_unit2_g.png"])
       CommonUtils.insertEnum(imgEnum , "Graph"                 , i++, [CommonConstants.Name] , ["setting/aqi_graph_"])
       CommonUtils.insertEnum(imgEnum , "Level"                 , i++, [CommonConstants.Name] , ["setting/aqi_img_"])
       CommonUtils.insertEnum(imgEnum , "WnCondition"           , i++, [CommonConstants.Name] , ["setting/wid_aqi_condition_"])
       CommonUtils.insertEnum(imgEnum , "Dot"                   , i++, [CommonConstants.Name] , ["."])
       CommonUtils.insertEnum(imgEnum , "Png"                   , i++, [CommonConstants.Name] , ["png"])

       CommonUtils.fetchArr(imgEnum , CommonConstants.Name , imgArr)
   }

   function allocateLevelEnum() {
       if (CommonUtils.getObjectLen(levelEnum) <= 0) {
            levelEnum = CommonUtils.createEnum(levelEnum)
       }
       var i = 0
       CommonUtils.insertEnum(levelEnum, "None"                 , i++, [CommonConstants.Name,CommonConstants.Image, CommonConstants.Code], [textArr[textEnum.Error]  , "error"  ,GlobalConstants.kListTextColor])
       CommonUtils.insertEnum(levelEnum, "Good"                 , i++, [CommonConstants.Name,CommonConstants.Image, CommonConstants.Code], [textArr[textEnum.Good]   , "good"   ,"#2dbcff"])
       CommonUtils.insertEnum(levelEnum, "Normal"               , i++, [CommonConstants.Name,CommonConstants.Image, CommonConstants.Code], [textArr[textEnum.Normal] , "normal" ,"#1fdc78"])
       CommonUtils.insertEnum(levelEnum, "Bad"                  , i++, [CommonConstants.Name,CommonConstants.Image, CommonConstants.Code], [textArr[textEnum.Bad]    , "bad"    ,"#ff7c25"])
       CommonUtils.insertEnum(levelEnum, "VeryBad"              , i++, [CommonConstants.Name,CommonConstants.Image, CommonConstants.Code], [textArr[textEnum.VeryBad], "verybad","#f0491f"])

       CommonUtils.fetchArr(levelEnum, CommonConstants.Name     , levelArr)
       CommonUtils.fetchArr(levelEnum, CommonConstants.Image    , levelEngArr)
       CommonUtils.fetchArr(levelEnum, CommonConstants.Code     , levelRgbArr)
   }

   function allocatePollutantEnum() {
       if (CommonUtils.getObjectLen(pollutantEnum) <= 0) {
            pollutantEnum = CommonUtils.createEnum(pollutantEnum)
       }
       var i = 0
       CommonUtils.insertEnum(pollutantEnum, "FineDust"         , i++, [CommonConstants.Name, CommonConstants.Image, CommonConstants.Level, CommonConstants.Value], [textArr[textEnum.FineDust         ] ,"setting/aqi_pollutantIndex_dust_eng.png"     ,0, null])
       CommonUtils.insertEnum(pollutantEnum, "Ozone"            , i++, [CommonConstants.Name, CommonConstants.Image, CommonConstants.Level, CommonConstants.Value], [textArr[textEnum.Ozone            ] ,"setting/aqi_pollutantIndex_ozone_eng.png"    ,0, null])
       CommonUtils.insertEnum(pollutantEnum, "CarbonMonoxide"   , i++, [CommonConstants.Name, CommonConstants.Image, CommonConstants.Level, CommonConstants.Value], [textArr[textEnum.CarbonMonoxide   ] ,"setting/aqi_pollutantIndex_carbon_eng.png"   ,0, null])
       CommonUtils.insertEnum(pollutantEnum, "SulfurDioxideGas" , i++, [CommonConstants.Name, CommonConstants.Image, CommonConstants.Level, CommonConstants.Value], [textArr[textEnum.SulfurDioxideGas ] ,"setting/aqi_pollutantIndex_Sulfur_eng.png"   ,0, null])
       CommonUtils.insertEnum(pollutantEnum, "NitrogenDioxide"  , i++, [CommonConstants.Name, CommonConstants.Image, CommonConstants.Level, CommonConstants.Value], [textArr[textEnum.NitrogenDioxide  ] ,"setting/aqi_pollutantIndex_Nitrogen_eng.png" ,0, null])
       CommonUtils.insertEnum(pollutantEnum, "IntegratedWaiting", i++, [CommonConstants.Name, CommonConstants.Image, CommonConstants.Level, CommonConstants.Value], [textArr[textEnum.IntegratedWaiting] ,"setting/aqi_pollutantIndex_aqi_eng.png"      ,0, null])

       pollutantArrSync()
   }

   function insertPollutantEnum(result) {
       CommonUtils.insertEnum(pollutantEnum, "FineDust"         ,pollutantEnum.FineDust         ,[CommonConstants.Level, CommonConstants.Value], [result.pm25Grade1h , result.pm25Value])
       CommonUtils.insertEnum(pollutantEnum, "Ozone"            ,pollutantEnum.Ozone            ,[CommonConstants.Level, CommonConstants.Value], [result.o3Grade     , result.o3Value  ])
       CommonUtils.insertEnum(pollutantEnum, "CarbonMonoxide"   ,pollutantEnum.CarbonMonoxide   ,[CommonConstants.Level, CommonConstants.Value], [result.coGrade     , result.coValue  ])
       CommonUtils.insertEnum(pollutantEnum, "SulfurDioxideGas" ,pollutantEnum.SulfurDioxideGas ,[CommonConstants.Level, CommonConstants.Value], [result.so2Grade    , result.so2Value ])
       CommonUtils.insertEnum(pollutantEnum, "NitrogenDioxide"  ,pollutantEnum.NitrogenDioxide  ,[CommonConstants.Level, CommonConstants.Value], [result.no2Grade    , result.no2Value ])
       CommonUtils.insertEnum(pollutantEnum, "IntegratedWaiting",pollutantEnum.IntegratedWaiting,[CommonConstants.Level, CommonConstants.Value], [result.khaiGrade   , result.khaiValue])

       pollutantArrSync()
   }


   function pollutantArrSync() {
       CommonUtils.fetchArr(pollutantEnum, CommonConstants.Name,  pollutantArr)
       CommonUtils.fetchArr(pollutantEnum, CommonConstants.Image, pollutantImgArr)
       CommonUtils.fetchArr(pollutantEnum, CommonConstants.Level, pollutantStatusArr)
       CommonUtils.fetchArr(pollutantEnum, CommonConstants.Value, pollutantValueArr)
   }

   function processResultInit() {
       for ( var key in resultInit ) {
            var value = resultInit[key]
           if (CommonUtils.isObject(value)) {
               if (CommonUtils.isEqual("pm10List", key)) {
                result[CommonUtils.string(key)] = [{pm10Value:null,dataTime:"0000-00-00 00:00"},]
               }
           }
           result[CommonUtils.string(key)] = value
       }
   }

   function processForecastInit() {
       for ( var key in resultForecastInit ) {
           var value = resultForecastInit[key]
           if (CommonUtils.isObject(value)) {
               if (CommonUtils.isEqual("items", key)) {
                    resultForecast[CommonUtils.string(key)] = [ {item : { dataTime : "0000-00-00 00:00", grade : "예보없음" }}
                                                   ,{item : { dataTime : "0000-00-00 00:00", grade : "예보없음" }}]
               }
           }
           resultForecast[CommonUtils.string(key)] = value
       }
   }

   //forecast의 grade를 level로 변경
   function getLevelEnum(s) {
       var ret = s
       s = CommonUtils.trim(s)
       switch(s) {
           case textArr[textEnum.ErrorKo  ]: ret = levelEnum.None   ; break;
           case textArr[textEnum.GoodKo   ]: ret = levelEnum.Good   ; break;
           case textArr[textEnum.NormalKo ]: ret = levelEnum.Normal ; break;
           case textArr[textEnum.BadKo    ]: ret = levelEnum.Bad    ; break;
           case textArr[textEnum.VeryBadKo]: ret = levelEnum.VeryBad; break;
           default:                          ret = levelEnum.None   ; break;
       }
       return ret
   }

   function init(){
       if (!needInit)
           return
       needInit = false

        //데이타 초기화
        allocateVariant()
        //----------------------
        allocateSoftKeyEnum()
        allocateTextEnum()
        //----------------------
        allocateResultCodeEnum()
        allocateImgEnum()
        allocateLevelEnum()
        allocatePollutantEnum()

       if (active) {
        activate()
       }
   }

   function showProgressView() {
       sendEvent(SettingAQIdetailViewEvent.ShowProgressIndicator)
       reqSearch = true
       isShowProgressIndicator = true
   }

   function hideProgressView() {
       sendEvent(SettingAQIdetailViewEvent.HideProgressIndicator)
       isShowProgressIndicator = false
   }

   function stopProgressView() {
       reqSearch = false
       timerProg = CommonUtils.destroy(timerProg)
       hideProgressView()
   }


   function requestQuery(kind) {
       var result = ({})
       if (!CommonUtils.isEqual(kind, SearchConstants.CurrentPos)) {
           result = resultDestPos
       }
       if (isValidResultInfo(result)) {
           processResult(result)
           return
       }
       if (!online) {
           result = ({})
           processResult(result)
           return
       }
       showProgressView()

       var data = ({})
       data[SearchConstants.Kind] = settingRoot.requestKind = kind
       sendEvent(SettingAQIdetailViewEvent.RequestSearch, data)
       queueChanged(0)
   }

   function showToast(message)   {
       var data = ({})
       data[CommonConstants.Message] = message
       data[CommonConstants.Position] = CommonConstants.ToastUp
       data[CommonConstants.Duration] = 3 * 1000
       sendEvent(SettingAQIdetailViewEvent.ShowToast ,data)
   }

   function activate() {
       var data = ({})
       sendEvent(SettingAQIdetailViewEvent.HideKeyboard)
       sendEvent(SettingAQIdetailViewEvent.SelectZone1Title, textArr[textEnum.Zone1Title])
       sendEvent(SettingAQIdetailViewEvent.RequestRouteGuidanceState)

       if (isRg) {
           data[ZoneConstants.Menu] = [  textArr[textEnum.Current]
                                       , textArr[textEnum.Destination]
                                       , CommonConstants.NoOption]
       } else {
           data[ZoneConstants.Menu] = [  textArr[textEnum.Current]
                                       , textArr[textEnum.Destination] + ":disabled"
                                       , CommonConstants.NoOption]
       }
       data[CommonConstants.Select] = CommonUtils.isEqual(settingRoot.requestKind,SearchConstants.CurrentPos)
                                    ? textArr[textEnum.Current] : textArr[textEnum.Destination]
       sendEvent(SettingAQIdetailViewEvent.Activated, data)

       requestQuery(requestKind)
   }

    onMediated: {
        //현재 설정된 언어및 네트워크 상태 요청
        sendEvent(SettingAQIdetailViewEvent.RequestLang)
        sendEvent(SettingAQIdetailViewEvent.QueryOnlineStatus)
        //데이타 초기화
        init()
        //초기화면을 호출
        var result = {}
        processResult(result)
    }
    onActivated: {
       sendEvent(SettingAQIdetailViewEvent.ResetCsw)
       init()
       activate()
    }
    onDeactivated: sendEvent(SettingAQIdetailViewEvent.Deactivated)
    Component.onDestruction: {
        timer = CommonUtils.destroy(timer)
        timerProg = CommonUtils.destroy(timerProg)
    }

    //검색 요청을 갯수를 얻음
    property int queueCount: 0
    function __queueChanged(count) {
        queueChanged(count)
    }

    function queueChanged(count) {
          queueCount = count
          timerProg = CommonUtils.destroy(timerProg)
          if (!reqSearch)
              return
          if (count<=0) {
              timerProg = CommonUtils.delay(settingRoot, SearchConstants.NetworkTimeoutAPP, function() {
                  stopProgressView()
              }, false)
          }
    }

    function back() {
        sendEvent(SettingAQIdetailViewEvent.BackPressed)

        var data = ({})
        data[CommonConstants.ObjectName] = CommonConstants.objectName(ViewRegistry.SettingAQIdetailView)
        sendEvent(SettingAQIdetailViewEvent.CloseView, data)
    }

    //언어가 변경되었을 때
    function __retranslateUi(lang) {
        if (CommonUtils.isEqual(settingRoot.__lang, lang))
            return
        settingRoot.__lang = lang
        needInit = true
        if (active) {
            init()
        }
    }

    function __changedOnlineStatus(online) {
        settingRoot.online = online
    }


    function __doSoftKeyFunction(button, menuName, menuIndex) {
        var data = ({})
        switch(button) {
          case softKeyMenu[softKeyEnum.Back]:
              back()
              break
          case softKeyMenu[softKeyEnum.Option]:
              break
          case softKeyMenu[softKeyEnum.Navi]:
              break
          case softKeyMenu[softKeyEnum.Menu]:
              switch (menuName) {
                case textArr[textEnum.Current]:
                    requestQuery(SearchConstants.CurrentPos)
                    break
                case textArr[textEnum.Destination]:
                    requestQuery(SearchConstants.DestinationPos)
                    break
              }
              break
          default:
              break
        }
    }

    property bool isRg: false
    function __changedRgState(isRg) {
        settingRoot.isRg = isRg
    }

    function displayText(seq) {
        if (CommonUtils.isNull(textArr) || textArr.length <= seq)
            return CommonConstants.Empty
        return textArr[seq]
    }

    function isValidResultInfo(result) {
        if(CommonUtils.isUndefined(typeof result.resultInfo)               ||
           CommonUtils.isUndefined(typeof result.resultInfo.resultCode)    ||
           CommonUtils.isUndefined(typeof result.resultInfo.curInfo)       ||
           CommonUtils.isUndefined(typeof result.resultInfo.curInfo.counts)||
           result.resultInfo.curInfo.counts <= 0) {
            return false
        }
        return true
    }

    function getStationName(text) {
        if (CommonUtils.isNull(text))
            return CommonConstants.Empty
        return text
    }

    //측정시간을 오전(오후) H:MM
    function getMeasurementTime(dt) {
        if (CommonUtils.isNull(dt))
            return CommonConstants.Empty
        var time =  new Date(dt)
        var dataTime = CommonConstants.Empty
        var hour = time.getHours()
        var min = time.getMinutes()
        var ampm = CommonConstants.Empty
        var en = CommonUtils.isEqual(settingRoot.__lang, CommonConstants.En)
        if (hour <= 12) {
            ampm = textArr[textEnum.AM]
        } else {
            ampm = textArr[textEnum.PM]
            hour =  hour % 12
        }
        if (en) {
            dataTime = hour + textArr[textEnum.Colon] + CommonUtils.zeroPadding(min,2) + textArr[textEnum.Space]  + ampm
        } else {
            dataTime = ampm + textArr[textEnum.Space] + hour + textArr[textEnum.Colon] + CommonUtils.zeroPadding(min,2)
        }
        return dataTime
    }

    function makeResult(curInfo) {
        var data = null
        var items = curInfo.items
        for (var i = 0; i< curInfo.counts; i++ ) {
            if (CommonUtils.isUndefined(typeof items[i]) ||
                CommonUtils.isUndefined(typeof items[i].item)) {
                continue
            }
            data = items[i].item
            settingRoot.result.pm10Value        = data.pm10Value
            settingRoot.result.pm10Grade1h      = data.pm10Grade1h
            settingRoot.result.pm10Grade        = data.pm10Grade
            settingRoot.result.pm10Value24      = data.pm10Value24
            settingRoot.result.pm25Value        = data.pm25Value
            settingRoot.result.pm25Grade1h      = data.pm25Grade1h
            settingRoot.result.pm25Grade        = data.pm25Grade
            settingRoot.result.pm25Value24      = data.pm25Value24
            settingRoot.result.no2Grade         = data.no2Grade
            settingRoot.result.no2Value         = data.no2Value
            settingRoot.result.o3Grade          = data.o3Grade
            settingRoot.result.o3Value          = data.o3Value
            settingRoot.result.khaiGrade        = data.khaiGrade
            settingRoot.result.khaiValue        = data.khaiValue
            settingRoot.result.so2Grade         = data.so2Grade
            settingRoot.result.so2Value         = data.so2Value
            settingRoot.result.coGrade          = data.coGrade
            settingRoot.result.coValue          = data.coValue
            settingRoot.result.dataTime         = data.dataTime
            settingRoot.result.mangName         = data.mangName
            settingRoot.result.stationName      = curInfo.stationName

            //availableDiffHour시간 이내의 데이타가 아닌 경우
            if (!isValidDiffDate(data.dataTime))
                return false

            if (CommonUtils.number(data.pm10Value) >= 0 &&
                CommonUtils.number(data.pm10Grade1h) > 0) {
                return true
            }
        }
        return false
    }

    function processValidData(result) {
        var ret = makeResult(result.resultInfo.curInfo)
        settingRoot.resultForecast = result.resultInfo.forecast
        resultSuccess = true
        if (ret) {
            if (CommonUtils.isEqual(settingRoot.requestKind,SearchConstants.CurrentPos)) {
                if (!isValidResultInfo(resultCurrentPos)) {
                   resultCurrentPos = result
                }
            } else {
                if (!isValidResultInfo(resultDestPos)) {
                   resultDestPos = result
                }
            }
        }
    }

    function isValidDiffDate(date) {
        sendEvent(SettingAQIdetailViewEvent.RequestGpsTime)
        var currDate = null
        var oldDate = new Date(date)
        if (CommonUtils.isNull(gpsTime) ||
            CommonUtils.isEmpty(gpsTime.toTimeString())) {
            //gps안테나가 없는 경우, 데스크탑 사용 시
            currDate = new Date()
        } else {
            currDate = gpsTime
        }
        var oldTime = oldDate.getTime()
        var currTime = currDate.getTime()
        var diffMs = Math.abs(currTime - oldTime)
        var diffDate = new Date(0,0,0,0,0,0, diffMs)
        var diffDays = Math.floor(diffMs / 1000 / 60 / (60 * 24))
        var diffHours = diffDate.getHours()
        var diffMinutes = diffDate.getMinutes()
        if (diffDays <= 0 && diffHours <= availableDiffHour) {
            if (CommonUtils.isEqual(diffHours, availableDiffHour) ) {
                if (diffMinutes > 0)
                    return false
            }
            return true
        }
        return false
    }

    function processInvalidData() {
        processResultInit()
        resultSuccess = false
    }

    function __onResult(result) {
        stopProgressView()
        processResult(result)
    }

    function getCacheResultData() {
        var result = ({})
        if (CommonUtils.isEqual(settingRoot.requestKind,SearchConstants.CurrentPos)) {
           if (isValidResultInfo(resultCurrentPos)) {
              result = resultCurrentPos
           }
        } else {
           if (isValidResultInfo(resultDestPos)) {
              result = resultDestPos
           }
        }
        return result
    }

    function processResultData(result) {
        switch(result.resultInfo.resultCode) {
            case resultCodeEnum.CurrentData:
                processValidData(result)
                break
            case resultCodeEnum.OldData:
                var items = result.resultInfo.curInfo.items
                if (!CommonUtils.isUndefined(typeof items[0]) &&
                    !CommonUtils.isUndefined(typeof items[0].item)) {
                    if (isValidDiffDate(items[0].item.dataTime)) {
                       processValidData(result)
                       break
                    }
                }
                processInvalidData()
                break
            case resultCodeEnum.NoneData:
            default:
                processInvalidData()
                break
        }
    }

    function processResult(result) {
        loader.sourceComponent = undefined
         if(!isValidResultInfo(result)) {
             result = getCacheResultData()
             if (online && isValidResultInfo(result)) {
                 processResultData(result)
             } else {
                 allocateVariant()
                 resultSuccess = false
             }
         } else {
             processResultData(result)
         }
        insertPollutantEnum(settingRoot.result)
        loader.sourceComponent = main
    }

    MultiPointTouchArea {
        anchors.fill: loader
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
            Image {
                width:parent.width
                height:parent.height
                source:Resource.imageUrl(imgArr[imgEnum.Background])
                Row {
                    width:parent.width
                    height:parent.height
                    Item {width:30;height:parent.height}
                    Item {
                        width:parent.width - (30*2)
                        height:parent.height
                        Column {
                            width:parent.width
                            height:parent.height
                            anchors.centerIn: parent
                            Row {
                                width: parent.width
                                height: 351
                                Item {width:250;height:parent.height
                                    Column {
                                        width:parent.width
                                        height:parent.height
                                        Item {width: parent.width;height: 28}
                                        Item {width: parent.width;height: 28
                                            Text {
                                                text:textArr[textEnum.DustConcentration]
                                                color: GlobalConstants.kListTextColor
                                                opacity: 0.5
                                                font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(30)}
                                                anchors.left: parent.left
                                                anchors.verticalCenter: parent.verticalCenter
                                            }
                                        }
                                        Item {width: parent.width;height: 45}
                                        CircleGraphDustDensity {
                                            width: parent.width
                                            height:parent.height - ((28*2) + 45)
                                            layout:{
                                                    var pm10Grade1h = settingRoot.result.pm10Grade1h
                                                    //미세먼지 레벨이 오류 처리
                                                    if (levelEnum.None > pm10Grade1h ||
                                                                         pm10Grade1h > levelEnum.VeryBad) {
                                                        pm10Grade1h = levelEnum.None
                                                    }
                                                    var topImg = CommonConstants.Empty
                                                    topImg += imgArr[imgEnum.Graph]
                                                    topImg += levelEngArr[pm10Grade1h]
                                                    topImg += imgArr[imgEnum.Dot]
                                                    topImg += imgArr[imgEnum.Png]

                                                    var middleImg = CommonConstants.Empty
                                                    middleImg += imgArr[imgEnum.Level]
                                                    middleImg += levelEngArr[pm10Grade1h]
                                                    if (CommonUtils.isEqual(settingRoot.__lang, CommonConstants.En)) {
                                                        middleImg += textArr[textEnum.Eng]
                                                    }
                                                    middleImg += imgArr[imgEnum.Dot]
                                                    middleImg += imgArr[imgEnum.Png]

                                                    var pm10Value = settingRoot.result.pm10Value
                                                    //미세먼지 농도 에러 처리
                                                    if (pm10Value < 0) {
                                                        pm10Value = null
                                                    }
                                                    var  bottomText = CommonUtils.isNull(pm10Value)
                                                                     ? pm10Value
                                                                     : CommonUtils.string(pm10Value)

                                                    return ({  t: { margin : 0
                                                                  , height : 76
                                                                  , image  : topImg
                                                                  , text   : CommonConstants.Empty }
                                                             , m: { margin : 0
                                                                  , height : 70
                                                                  , image  : middleImg
                                                                  , text   : CommonConstants.Empty }
                                                             , b: { margin : 12
                                                                  , height : 32
                                                                  , image  : imgArr[imgEnum.Unit]
                                                                  , text   : bottomText }
                                                             })
                                                    }
                                        }
                                    }
                                }
                                Item {width:50;height:parent.height}
                                Item {
                                    width:96+20+344
                                    height:parent.height
                                    Column {
                                        width:parent.width
                                        height:parent.height
                                        Item {width:parent.width;height:24}
                                        Item {width:parent.width;height:36; visible: !CommonUtils.isNull(resultSuccess) ? resultSuccess : false
                                            BorderImage {
                                                id:borderImage
                                                width:parent.width
                                                height: parent.height
                                                anchors.right: parent.right
                                                source: Resource.imageUrl(imgArr[imgEnum.PositionBG])
                                                border{left:16;top:16;right:16;bottom:16}
                                                Row {
                                                    id: borderRow
                                                    width: parent.width
                                                    height: parent.height
                                                    Item {width:16;height:parent.height}
                                                    Item {width:12;height:parent.height
                                                        Image {
                                                            width:12
                                                            height:18
                                                            source: Resource.imageUrl(imgArr[imgEnum.PositionIC])
                                                            anchors.centerIn: parent
                                                        }
                                                    }
                                                    Item {width:8;height:parent.height}
                                                    Item {width:292;height:parent.height
                                                        Text {
                                                            text:{
                                                                if (CommonUtils.isNull(resultSuccess) || !resultSuccess )
                                                                    return CommonConstants.Empty
                                                                var ampmTime = getMeasurementTime(settingRoot.result.dataTime)
                                                                return getStationName(settingRoot.result.stationName)
                                                                     + textArr[textEnum.Space]
                                                                     + textArr[textEnum.Slash]
                                                                     + textArr[textEnum.Space]
                                                                     + ampmTime
                                                                     + textArr[textEnum.Space]
                                                                     + textArr[textEnum.Basis]
                                                            }
                                                            color: GlobalConstants.kListTextColor
                                                            font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(24)}
                                                            anchors.right: parent.right
                                                            anchors.verticalCenter: parent.verticalCenter
                                                            onContentWidthChanged:{
                                                                if (contentWidth <= 0)
                                                                    return
                                                                parent.width = contentWidth
                                                                borderRow.width = CommonUtils.applyWidth(borderRow)
                                                                borderImage.width = CommonUtils.applyWidth(borderImage)
                                                            }
                                                        }
                                                    }
                                                    Item {width:16;height:parent.height}
                                                }
                                            }
                                        }
                                        Item {width:parent.width;height:10}
                                        Item {width:parent.width;height:18
                                            Row {
                                                id:apply
                                                width: parent.width
                                                height: parent.height
                                                anchors.right: parent.right
                                                Item {width: parent.width-16;height:parent.height
                                                    Text {
                                                        text:textArr[textEnum.Apply]
                                                        color: GlobalConstants.kListTextColor
                                                        font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(20)}
                                                        anchors.right: parent.right
                                                        anchors.verticalCenter: parent.verticalCenter
                                                        opacity: 0.3
                                                        onContentWidthChanged:{
                                                            if (contentWidth <= 0)
                                                                return
                                                            parent.width = contentWidth
                                                            apply.width = CommonUtils.applyWidth(apply)
                                                        }
                                                    }
                                                }
                                                Item {width:16;height:parent.height}
                                            }
                                        }
                                        Item {width:parent.width;height:32}
                                        Loader {
                                            width:parent.width
                                            height:251
                                            sourceComponent: forecast
                                        }
                                   }
                                }
                            }
                            Item {width:parent.width;height:20}
                            Column {
                                width:parent.width
                                height:342
                                Item {width:parent.width;height:28
                                    Text {text:textArr[textEnum.Pollutant]
                                        color: GlobalConstants.kListTextColor
                                        opacity: 0.5
                                        anchors.left: parent.left
                                        font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(30)}
                                    }
                                }
                                Item {width:parent.width;height:30}
                                Row {
                                    width:parent.width
                                    height:parent.height
                                    Component.onCompleted: {CommonUtils.applyRemainHeight(this,parent)}
                                    Repeater {
                                        model: pollutantArr.length
                                        delegate:Row {
                                                    width:128
                                                    height:parent.height
                                                    Item{width:120;height:parent.height
                                                        Loader {
                                                            property int idx: index
                                                            property int status: pollutantStatusArr[index]
                                                            property var value: pollutantValueArr[index]
                                                            width: parent.width
                                                            height:parent.height
                                                            sourceComponent: pollutant
                                                            anchors.centerIn: parent
                                                            property var layout : {
                                                                var middelImg = CommonConstants.Empty
                                                                middelImg += imgArr[imgEnum.WnCondition]
                                                                middelImg += levelEngArr[pollutantStatusArr[index]]
                                                                if (CommonUtils.isEqual(settingRoot.__lang, CommonConstants.En)) {
                                                                    middelImg += textArr[textEnum.Eng]
                                                                }
                                                                middelImg += imgArr[imgEnum.Dot]
                                                                middelImg += imgArr[imgEnum.Png]

                                                                var bottomImg = CommonConstants.Empty
                                                                if (CommonUtils.isEqual(index, 5)) {
                                                                    bottomImg = CommonConstants.Empty
                                                                } else {
                                                                     var idx = (!index) ? imgEnum.WidUnit1 : imgEnum.WidUnit2
                                                                     bottomImg = imgArr[idx]
                                                                }

                                                                var bottomTxt = CommonConstants.Empty
                                                                if (CommonUtils.isNull(pollutantValueArr[index])) {
                                                                    bottomTxt = pollutantValueArr[index]
                                                                } else {
                                                                    bottomTxt = CommonUtils.string(pollutantValueArr[index])
                                                                }

                                                                return ({ t: { margin: 0
                                                                            , height: 22
                                                                            , width : 0
                                                                            , image : pollutantImgArr[index]
                                                                            , text  : pollutantArr[index]
                                                                            }
                                                                       , m: { margin: CommonUtils.isEqual(settingRoot.__lang, CommonConstants.En) ? 6  : 18
                                                                            , height: CommonUtils.isEqual(settingRoot.__lang, CommonConstants.En) ? 50 : 34
                                                                            , width : 98
                                                                            , image : middelImg
                                                                            }
                                                                       , b: { margin: CommonUtils.isEqual(settingRoot.__lang, CommonConstants.En) ? 4 : 8
                                                                            , height: 22
                                                                            , width : 0
                                                                            , image : bottomImg
                                                                            , text  : bottomTxt
                                                                            }
                                                                        })
                                                            }
                                                        }
                                                    }
                                                    Item{width:8;height:parent.height}
                                                }
                                    }
                                }
                                Item {width:parent.width;height:12}
                                Item {width:parent.width;height:18
                                    Text {text:textArr[textEnum.Notice]
                                        color: GlobalConstants.kListTextColor
                                        font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(20)}
                                        anchors.centerIn: parent
                                        opacity: 0.3
                                    }
                                }
                                Item {width:parent.width;height:18}
                            }
                        }
                    }
                    Item { width:30; height:parent.height}
                }
            }
        }
    }

    Component {
        id: forecast
        Column {
            width: parent.width
            height: parent.height
            Item { width: parent.width; height: 22
                Text {
                    color: GlobalConstants.kListTextColor
                    font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(24)}
                    anchors.verticalCenter: parent.verticalCenter
                    text: {
                            var dateTime = settingRoot.resultForecast.dataTime
                            var data = CommonUtils.split(dateTime,textArr[textEnum.HourKo])                             
                            var ampmTime = dateTime
                            if (CommonUtils.isObject(data) && data.length > 0) {
                                ampmTime = getMeasurementTime(data[0]+":00")
                            }
                            dateTime = ampmTime
                            if (CommonUtils.isObject(data) && data.length > 1 &&
                                CommonUtils.indexOf(data[1], textArr[textEnum.AnnounceKo])) {
                                if (CommonUtils.isEqual(settingRoot.__lang, CommonConstants.En)) {
                                    dateTime = textArr[textEnum.AnnounceEn] + textArr[textEnum.Space] + ampmTime
                                } else {
                                    dateTime = ampmTime + textArr[textEnum.Space] + textArr[textEnum.AnnounceKo]
                                }
                            }
                            return dateTime
                    }
                    visible: resultSuccess
                }
            }
            Item { width: parent.width; height: 6}
            Item { width: parent.width; height: 22
                Text {
                    color: GlobalConstants.kListTextColor
                    font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(24)}
                    anchors.verticalCenter: parent.verticalCenter
                    text: {
                        if (CommonUtils.isEqual(settingRoot.__lang, CommonConstants.En)) {
                            return textArr[textEnum.LocalDustForecast] + settingRoot.resultForecast.addr
                        }
                        return settingRoot.resultForecast.addr + textArr[textEnum.LocalDustForecast]
                    }
                    visible: resultSuccess
                }
            }
            Item { width: parent.width; height: 6}
            Column {
                width: parent.width
                height: parent.height - (22*2 + 6*2)
                Rectangle { width: parent.width; height: 1; color: GlobalConstants.kListTextColor; opacity: 0.3}
                Row {
                    width: parent.width
                    height: 47
                    Rectangle { width: 1; height: parent.height; color: GlobalConstants.kListTextColor; opacity: 0.3}

                    Item { width: (parent.width-3)/2; height: parent.height
                        Text {
                            color: GlobalConstants.kListTextColor
                            font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(28)}
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: textArr[textEnum.Today]
                        }
                    }
                    Rectangle { width: 1; height: parent.height; color: GlobalConstants.kListTextColor; opacity: 0.3}
                    Item { width: (parent.width-3)/2; height: parent.height
                        Text {
                            color: GlobalConstants.kListTextColor
                            font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(28)}
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: textArr[textEnum.Tomorrow]
                        }
                    }
                    Rectangle { width: 1; height: parent.height; color: GlobalConstants.kListTextColor; opacity: 0.3}
                }
                Rectangle { width: parent.width; height: 1; color: GlobalConstants.kListTextColor; opacity: 0.3}
                Row {
                    width: parent.width
                    height: 90
                    Rectangle { width: 1; height: parent.height; color: GlobalConstants.kListTextColor; opacity: 0.3}
                    Item { width: (parent.width-3)/2; height: parent.height
                        Image {
                            source : {
                                var level = getLevelEnum(settingRoot.resultForecast.items[0].item.grade)
                                var  imagePath = CommonConstants.Empty
                                imagePath += "setting"
                                imagePath += textArr[textEnum.Slash]
                                if (!resultSuccess) {
                                    level = levelEnum.None
                                }
                                //UID문서에 다른 곳은 error로 사용하고 있으나 이곳은 nodata로 유지보수를 위하여 nodata로 그대로 사용함
                                if (CommonUtils.isEqual(level, levelEnum.None)) {
                                    imagePath += "nodata"
                                } else {
                                    imagePath += levelEngArr[level]
                                }
                                if (!CommonUtils.isEqual(level, levelEnum.None)) {
                                    if (CommonUtils.isEqual(settingRoot.__lang, CommonConstants.En)) {
                                        imagePath += textArr[textEnum.Eng]
                                    }
                                }
                                imagePath += imgArr[imgEnum.Dot]
                                imagePath += imgArr[imgEnum.Png]
                                return Resource.imageUrl(imagePath)
                            }
                            anchors.centerIn: parent
                        }
                    }
                    Rectangle { width: 1; height: parent.height; color: GlobalConstants.kListTextColor; opacity: 0.3}
                    Item { width: (parent.width-3)/2; height: parent.height
                        Image {
                            source : {
                                var level = getLevelEnum(settingRoot.resultForecast.items[1].item.grade)
                                visible = true
                                var  imagePath = CommonConstants.Empty
                                imagePath += "setting"
                                imagePath += textArr[textEnum.Slash]
                                if (!resultSuccess) {
                                    level = levelEnum.None
                                }
                                //UID문서에 다른 곳은 error로 사용하고 있으나 이곳은 nodata로 유지보수를 위하여 nodata로 그대로 사용함
                                if (CommonUtils.isEqual(level, levelEnum.None)) {
                                    imagePath += "nodata"
                                } else {
                                    imagePath += levelEngArr[level]
                                }
                                if (!CommonUtils.isEqual(level, levelEnum.None)) {
                                    if (CommonUtils.isEqual(settingRoot.__lang, CommonConstants.En)) {
                                        imagePath += textArr[textEnum.Eng]
                                    }
                                }
                                imagePath += imgArr[imgEnum.Dot]
                                imagePath += imgArr[imgEnum.Png]
                                return Resource.imageUrl(imagePath)
                            }
                            anchors.centerIn: parent
                        }
                    }
                    Rectangle { width: 1; height: parent.height; color: GlobalConstants.kListTextColor; opacity: 0.3}

                }
                Rectangle { width: parent.width; height: 1; color: GlobalConstants.kListTextColor; opacity: 0.3}
            }
        }
    }

    //오염물질 지수
    Component {
        id: pollutant
        Column {
            width:parent.width
            height:parent.height
            property int index: 0
            property int status: 0
            property var value: null
            property var layout: ({
                t:{margin:0,height:0,width:0,image:CommonConstants.Empty,text:CommonConstants.Empty},
                m:{margin:0,height:0,width:0,image:CommonConstants.Empty,text:CommonConstants.Empty},
                b:{margin:0,height:0,width:0,image:CommonConstants.Empty,text:CommonConstants.Empty},
            })

            onParentChanged: {
                if (CommonUtils.isNull(parent))
                    return
                //스크립트 warning 수정
                layout = Qt.binding(function(){
                     if (CommonUtils.isUndefined(typeof parent.layout) ||
                         CommonUtils.isNull(parent.layout)) {
                         return  ({
                                      t:{margin:0,height:0,width:0,image:CommonConstants.Empty,text:CommonConstants.Empty},
                                      m:{margin:0,height:0,width:0,image:CommonConstants.Empty,text:CommonConstants.Empty},
                                      b:{margin:0,height:0,width:0,image:CommonConstants.Empty,text:CommonConstants.Empty},
                                  })
                     }
                     return parent.layout
                })
                index = Qt.binding(function(){
                    if (CommonUtils.isUndefined(typeof parent.idx) ||
                        CommonUtils.isNull(parent.idx))
                        return 0
                    return parent.idx
                })
                status = Qt.binding(function(){
                    if (CommonUtils.isUndefined(typeof parent.status) ||
                        CommonUtils.isNull(parent.status))
                        return 0
                    return parent.status
                })
                value = Qt.binding(function(){
                    if (CommonUtils.isUndefined(typeof parent.value) ||
                        CommonUtils.isNull(parent.value))
                        return 0
                    return parent.value
                })
            }

            onStatusChanged:  {
                if (status < 0) {
                    status = 0
                }
            }

            Item {width:parent.width;height:layout.t.height;
                Image {
                    source:Resource.imageUrl(layout.t.image)
                    anchors.centerIn: parent
                    visible: CommonUtils.isEqual(settingRoot.__lang, CommonConstants.En)
                }
                Text {
                    text:layout.t.text
                    color: GlobalConstants.kListTextColor
                    font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(24)}
                    anchors.centerIn: parent
                    visible: !CommonUtils.isEqual(settingRoot.__lang, CommonConstants.En)
                }
            }
            Repeater {
                model: 4
                delegate:
                    Column {
                        width: parent.width
                        height: 33
                        Item {width:parent.width;height:18}
                        Item {width:parent.width;height:15
                            Rectangle {
                                width:15
                                height:15
                                radius:15
                                color: ((4 - status ) <= index) ?  levelRgbArr[status] : GlobalConstants.kListTextColor
                                opacity: ((4 - status ) <= index) ?  1.0 : 0.1
                                anchors.centerIn: parent
                            }
                        }
                    }
            }
            Item {width:parent.width;height:layout.m.margin}
            Item {width:parent.width;height:layout.m.height
                Column {
                    width: parent.width
                    height: CommonUtils.getCalcHeight(this)
                    anchors.centerIn: parent
                    Repeater {
                        id: rpter
                        model: {
                            if (status >= 0) {
                                var s = CommonUtils.split(levelArr[status], "<br>")
                                return s.length
                            } else {
                                return 1
                            }
                        }
                        delegate:
                            Item {width:parent.width;height:CommonUtils.getCalcHeight(this)
                                Item {width: parent.width; height: index ? 2 : 0}
                                Text {
                                    width: parent.width
                                    height: contentHeight
                                    text: {
                                        if (status >= 0) {
                                            var s = CommonUtils.split(levelArr[status], "<br>")
                                            return  s[index]
                                        } else {
                                            return CommonConstants.Empty
                                        }
                                    }
                                    color: (status >= 0) ? levelRgbArr[status] : CommonConstants.Transparent
                                    opacity: (status > 0)? 1.0 : 0.3
                                    horizontalAlignment: Qt.AlignHCenter
                                    verticalAlignment: Qt.AlignVCenter
                                    font { family: GlobalConstants.kFontFamily
                                           pixelSize: {
                                               var size
                                               if (CommonUtils.isEqual(settingRoot.__lang, CommonConstants.En)) {
                                                   var s = [23,25]
                                                   size = rpter.model > 1 ? s[index] : s[1]
                                               } else {
                                                   size = 36
                                               }
                                               return Resource.pixelToDp(size)
                                           }
                                    }
                                }
                        }
                    }
                }
            }
            Item {width:parent.width;height:layout.b.margin}
            Item {width:parent.width;height:layout.b.height
                Row {
                    height:parent.height
                    anchors.horizontalCenter: parent.horizontalCenter
                    Text {
                        text: (CommonUtils.isNull(layout.b.text) || CommonUtils.number(layout.b.text) < 0) ? textArr[textEnum.Error] : layout.b.text
                        color: "#666666"
                        font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(20)}
                        anchors.verticalCenter: parent.verticalCenter
                    }
                    Item {width:(CommonUtils.isNull(layout.b.text) || CommonUtils.number(layout.b.text) < 0) ? 0 : 4;height:parent.height}
                    Image {
                        source: CommonUtils.isEmpty(layout.b.image) ? layout.b.image :  Resource.imageUrl(layout.b.image)
                        anchors.verticalCenter: parent.verticalCenter
                    }
                }
            }
        }
    }
}
