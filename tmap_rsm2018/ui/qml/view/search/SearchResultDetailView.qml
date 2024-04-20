import QtQuick 2.0
import ViewRegistry 1.0
import CommonConstants 1.0
import ZoneConstants 1.0
import SettingConstants 1.0
import SearchConstants 1.0
import SearchResultDetailViewEvent 1.0
import SearchResultViewEvent 1.0
import SceneGraphRendering 1.0
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

    background.visible                      : false
    mouseArea.enabled                       : false
    fader.fadingOpacity                     : 0


    property bool reFocus                   : false
    property bool isClosed                  : false


    //onMediated에서 요청 및 응답
    property var __lang                     : null
    property bool online                    : false
    property bool isRg                      : false



    //뷰호출객체이름
    property var caller                     : null                    //first  depth(SearchResultDetailView를 호출한 뷰의 호출뷰(필수O))
    property var relay                      : null                    //middle depth(필수X)
    property var owner                      : null                    //last   depth(SearchResultDetailView를 호출한 뷰(필수O))

    //검색을 위한 기본 정보
    property var result                     : null
    property var callerResult               : null

    //검색을 위한 부가 정보
    property var kind                       : null                   //categoryEnum 값: 업종검색 주유소,충전소 및 주변검색의 (주유소,충전소,주차장,은행,쇼핑,음식점,관광,병원)메뉴의 구분 코드값
    property string searchType              : CommonConstants.Empty  //텍스트검색(CommonConstants.Name), 주소검색(CommonConstants.Address),업종검색(CommonConstants.Industry) 구분값
    property int destType                   : SearchConstants.DestTypeNone
    property bool isFavorite                : false
    property string addressType             : SearchConstants.None   //[주의] Mediator에서 값을 넘김 - 구주소/신주소 검색을 위하여 사용: .RequestAddressType Event요청할 때 결과 값


    //경로상 검색인 경우의 상세 정보를 위하여 ...
    property int oilSearchType              : -1


    //디폴트 값
    readonly property int markerBaseIndex   : 100
    readonly property int frameHeight       : 403


    //뷰 관련
    property bool messageVisible            : false
    property bool isMediated                : false
    property bool isActivated               : false
    property string messageText             : CommonConstants.Empty

    //포커스/셀렉트 관련
    property bool focused                   : false
    property color highlightColor           : CommonConstants.Transparent
    property color selectcolor              : MultiSenseCoordinator.kAmbientColor

    //전화 관련
    property bool bluetoothStatus           : true
    property int  callStatus                : 0

    //큐관련
    property bool reqSearch                 : false

    //마커 표출
    property bool haveMarker                : false

    //Timer
    property var timerClick                 : null


    //ID
    property var frame                      : null
    property var titleRange                 : null
    property var listView                   : null
    property var message                    : null
    property var favoriteButton             : null
    property var styleLoader                : null
    property var loaderOilTypeId            : null

    //Enum
    property var textEnum                   : ({})
    property var imgEnum                    : ({})
    property var softKeyEnum                : ({})
    property var categoryEnum               : ({})
    property var oilTypeEnum                : ({})
    property var poleCateEnum               : ({})
    property var poleCateOnPathEnum         : ({})
    property var normalTitleEnum            : ({})
    property var evTitleEnum                : ({})
    property var evBrandEnum                : ({})
    property var evKindEnum                 : ({})
    property var searchTypeEnum             : ({})
    property var evStatusEnum               : ({})

    //-Enum에 대응되는 배열
    property var textArr                    : ([])
    property var imgArr                     : ([])
    property var softKeyMenuArr             : ([])
    property var oilTypeImgArr              : ([])
    property var normalTitleIndexArr        : ([])
    property var normalTitleNameArr         : ([])
    property var evTitleIndexArr            : ([])
    property var evTitleNameArr             : ([])
    property var evBrandNameArr             : ([])
    property var evBrandKeyArr              : ([])
    property var evTypeArr                  : ([])
    property var searchTypeArr              : ([])      //상세에서는 사용하지 않고 있음
    property var evStatusNameArr            : ([])


    // 네비메뉴 -> 집,회사 -> 주변검색, 업종검색, 주소검색 -> 상세로 들어왔을 때
    // 네비메뉴 -> 주변검색, 업종검색, 주소검색 -> 상세의 경우와 구별을 위해
    // 즐겨 찾기 flag로 구별한다.
    // 17.12.04 ymhong
    property bool favorite                  : false
    property bool isHome                    : false
    property bool isOffice                  : false
    property string viaMenuText             : CommonConstants.Empty


    property int gasolinePrice              : 0
    property int dieselPrice                : 0
    property int lpgPrice                   : 0
    property int highGasolinePrice          : 0

    property string stationIconSource       : CommonConstants.Empty
    property int stationIconWidth           : 0
    property int stationIconHeight          : 0

    property int dlgUniqueNum               : -1
    //검색 요청을 갯수를 얻음
    property var timerProg                  : null
    property int queueCount                 : 0

    property var fuel: [[oilTypeImgArr[oilTypeEnum.Gasoline]        , gasolinePrice    ],
                        [oilTypeImgArr[oilTypeEnum.Diesel]          , dieselPrice      ],
                        [oilTypeImgArr[oilTypeEnum.LPG]             , lpgPrice         ],
                        [oilTypeImgArr[oilTypeEnum.HighGasoline]    , highGasolinePrice]
                       ]

    onDestTypeChanged: {
        isHome = destType === 2
        isOffice = destType === 1
    }

    function allocateVariant(){
         caller             = null
         relay              = null
         owner              = null
         result             = null
         callerResult       = null

         kind               = null
         searchType         = CommonConstants.Empty
         destType           = SearchConstants.DestTypeNone
         isFavorite         = false
         oilSearchType      = -1

         messageVisible     = false
         messageText        = CommonConstants.Empty
         isActivated        = false

         focused            = false
         highlightColor     = CommonConstants.Transparent
         selectcolor        = Qt.binding(function(){ return MultiSenseCoordinator.kAmbientColor})

         bluetoothStatus    = true
         callStatus         = 0

         reqSearch          = false

         haveMarker         = false

         favorite           = false
         isHome             = false
         isOffice           = false
         viaMenuText        = CommonConstants.Empty

         gasolinePrice      = 0
         dieselPrice        = 0
         lpgPrice           = 0
         highGasolinePrice  = 0

         stationIconSource  = CommonConstants.Empty
         stationIconWidth   = 0
         stationIconHeight  = 0

         dlgUniqueNum       = -1
         queueCount         = 0

        listModel.clear()
        unloadMainComponent()
    }

    function allocateTextEnum(){
        if (CommonUtils.getObjectLen(textEnum) <= 0) {
            textEnum = CommonUtils.createEnum(textEnum)
        }
        var i = 0
        CommonUtils.insertEnum(textEnum , "Zone1Title"              , i++, [CommonConstants.Name] , [qsTr("상세 정보")])
        CommonUtils.insertEnum(textEnum , "NoneData"                , i++, [CommonConstants.Name] , [qsTr("정보없음")])
        CommonUtils.insertEnum(textEnum , "NoData"                  , i++, [CommonConstants.Name] , [qsTr("검색 데이타가 없습니다.")])
        CommonUtils.insertEnum(textEnum , "Possible"                , i++, [CommonConstants.Name] , [qsTr("가능")])
        CommonUtils.insertEnum(textEnum , "Impossible"              , i++, [CommonConstants.Name] , [qsTr("불가능")])
        CommonUtils.insertEnum(textEnum , "NoHollidays"             , i++, [CommonConstants.Name] , [qsTr("연중무휴")])
        CommonUtils.insertEnum(textEnum , "Won"                     , i++, [CommonConstants.Name] , [qsTr("원")])
        CommonUtils.insertEnum(textEnum , "Km"                      , i++, [CommonConstants.Name] , [qsTr("km")])
        CommonUtils.insertEnum(textEnum , "Free"                    , i++, [CommonConstants.Name] , [qsTr("무료")])
        CommonUtils.insertEnum(textEnum , "Pay"                     , i++, [CommonConstants.Name] , [qsTr("유료")])
        //전기차충전소 운영기관 - 영문변환 하지 않기로 결정
        CommonUtils.insertEnum(textEnum , "ME"                      , i++, [CommonConstants.Name] , ["환경부"])
        CommonUtils.insertEnum(textEnum , "KP"                      , i++, [CommonConstants.Name] , ["한국전력"])
        CommonUtils.insertEnum(textEnum , "HM"                      , i++, [CommonConstants.Name] , ["현대자동차네비게이션"])
        CommonUtils.insertEnum(textEnum , "KM"                      , i++, [CommonConstants.Name] , ["기아자동차네비게이션"])
        CommonUtils.insertEnum(textEnum , "SE"                      , i++, [CommonConstants.Name] , ["서울시"])
        CommonUtils.insertEnum(textEnum , "PI"                      , i++, [CommonConstants.Name] , ["포스코ICT"])
        CommonUtils.insertEnum(textEnum , "HE"                      , i++, [CommonConstants.Name] , ["한국전기차충전서비스"])
        CommonUtils.insertEnum(textEnum , "CT"                      , i++, [CommonConstants.Name] , ["씨티카"])
        CommonUtils.insertEnum(textEnum , "DG"                      , i++, [CommonConstants.Name] , ["대구시"])
        CommonUtils.insertEnum(textEnum , "JD"                      , i++, [CommonConstants.Name] , ["제주특별자치도청"])
        CommonUtils.insertEnum(textEnum , "JE"                      , i++, [CommonConstants.Name] , ["제주전기자동차서비스"])
        CommonUtils.insertEnum(textEnum , "TM"                      , i++, [CommonConstants.Name] , ["티맵"])
        CommonUtils.insertEnum(textEnum , "BG"                      , i++, [CommonConstants.Name] , ["비긴스"])
        CommonUtils.insertEnum(textEnum , "PW"                      , i++, [CommonConstants.Name] , ["파워큐브"])
        //상세 공통
        CommonUtils.insertEnum(textEnum , "TelNum"                  , i++, [CommonConstants.Name] , [qsTr("전화번호")  ])
        //주유소,LPG충전소-상세
        CommonUtils.insertEnum(textEnum , "OPIPrice"                , i++, [CommonConstants.Name] , [qsTr("유가정보")  ])
        CommonUtils.insertEnum(textEnum , "WebSite"                 , i++, [CommonConstants.Name] , [qsTr("웹사이트")  ])
        CommonUtils.insertEnum(textEnum , "Holliday"                , i++, [CommonConstants.Name] , [qsTr("휴무일")    ])
        CommonUtils.insertEnum(textEnum , "WorkTime"                , i++, [CommonConstants.Name] , [qsTr("영업시간")  ])
        CommonUtils.insertEnum(textEnum , "Industry"                , i++, [CommonConstants.Name] , [qsTr("업종")      ])
        CommonUtils.insertEnum(textEnum , "Parking"                 , i++, [CommonConstants.Name] , [qsTr("주차")      ])
        CommonUtils.insertEnum(textEnum , "Introduction"            , i++, [CommonConstants.Name] , [qsTr("소개글")    ])
        //-[주의] 상세결과 데이타의 한글문자열 검색에 사용
        CommonUtils.insertEnum(textEnum , "HollidayKo"              , i++, [CommonConstants.Name] , [SearchConstants.HollidayKo ])
        CommonUtils.insertEnum(textEnum , "WorkTimeKo"              , i++, [CommonConstants.Name] , [SearchConstants.WorkTimeKo ])
        CommonUtils.insertEnum(textEnum , "ParkingKo"               , i++, [CommonConstants.Name] , [SearchConstants.ParkingKo  ])
        //전기차충전소-상세
        CommonUtils.insertEnum(textEnum , "DetailLocation"          , i++, [CommonConstants.Name] , [qsTr("상세위치")   ])
        CommonUtils.insertEnum(textEnum , "ChargingFee"             , i++, [CommonConstants.Name] , [qsTr("충전요금")   ])
        CommonUtils.insertEnum(textEnum , "OperatingTime"           , i++, [CommonConstants.Name] , [qsTr("운영시간")   ])
        CommonUtils.insertEnum(textEnum , "ParkingFree"             , i++, [CommonConstants.Name] , [qsTr("주차요금")   ])
        CommonUtils.insertEnum(textEnum , "Charger"                 , i++, [CommonConstants.Name] , [qsTr("충전기")     ])
        CommonUtils.insertEnum(textEnum , "OperatingOrgan"          , i++, [CommonConstants.Name] , [qsTr("운영기관")   ])
        CommonUtils.insertEnum(textEnum , "OperatingAgencyID"       , i++, [CommonConstants.Name] , [qsTr("운영기관ID") ])
        CommonUtils.insertEnum(textEnum , "AgencyManagementID"      , i++, [CommonConstants.Name] , [qsTr("기관관리ID") ])
        CommonUtils.insertEnum(textEnum , "ChargingStationID"       , i++, [CommonConstants.Name] , [qsTr("충전소ID")   ])
        CommonUtils.insertEnum(textEnum , "UsageTime"               , i++, [CommonConstants.Name] , [qsTr("이용시간")   ])
        //전기차충전소-타입
        CommonUtils.insertEnum(textEnum , "DCdemo"                  , i++, [CommonConstants.Name] , [qsTr("DC차데모")    ])
        CommonUtils.insertEnum(textEnum , "DCcombo"                 , i++, [CommonConstants.Name] , [qsTr("DC콤보")      ])
        CommonUtils.insertEnum(textEnum , "ACslow"                  , i++, [CommonConstants.Name] , [qsTr("완속")        ]) //2019.04.12 승용차AC완속 -> 완속
        CommonUtils.insertEnum(textEnum , "ACfast"                  , i++, [CommonConstants.Name] , [qsTr("AC3상")       ]) //2019.04.12 AC급속3상 -> AC3상
        CommonUtils.insertEnum(textEnum , "AC3"                     , i++, [CommonConstants.Name] , [qsTr("AC3상")       ])
        //전기차충전소-상태
        CommonUtils.insertEnum(textEnum , "StatusUnknow"            , i++, [CommonConstants.Name] , [qsTr("정보없음")     ])
        CommonUtils.insertEnum(textEnum , "StatusError"             , i++, [CommonConstants.Name] , [qsTr("통신이상")     ])
        CommonUtils.insertEnum(textEnum , "StatusReady"             , i++, [CommonConstants.Name] , [qsTr("충전가능")     ])
        CommonUtils.insertEnum(textEnum , "StatusProceeding"        , i++, [CommonConstants.Name] , [qsTr("충전 중")      ])
        CommonUtils.insertEnum(textEnum , "StatusInactive"          , i++, [CommonConstants.Name] , [qsTr("운영중지")     ])
        CommonUtils.insertEnum(textEnum , "StatusChecking"          , i++, [CommonConstants.Name] , [qsTr("점검 중")      ])
        CommonUtils.insertEnum(textEnum , "StatusReservation"       , i++, [CommonConstants.Name] , [qsTr("예약 중")      ])
        //급속,완속
        CommonUtils.insertEnum(textEnum , "FastCharge"              , i++, [CommonConstants.Name] , [qsTr("급속")         ])
        CommonUtils.insertEnum(textEnum , "SlowCharge"              , i++, [CommonConstants.Name] , [qsTr("완속")         ])
        CommonUtils.insertEnum(textEnum , "ChargeNumber"            , i++, [CommonConstants.Name] , [qsTr("대")           ])
        CommonUtils.insertEnum(textEnum , "ChargeNumberOf"          , i++, [CommonConstants.Name] , [qsTr("중")           ])
        CommonUtils.insertEnum(textEnum , "Chargeable"              , i++, [CommonConstants.Name] , [qsTr("충전가능")     ])
        //기호
        CommonUtils.insertEnum(textEnum , "Space"                   , i++, [CommonConstants.Name] , [String.fromCharCode(0x20) ])
        CommonUtils.insertEnum(textEnum , "EvDelim"                 , i++, [CommonConstants.Name] , ["/"                       ])
        CommonUtils.insertEnum(textEnum , "EvSepar"                 , i++, [CommonConstants.Name] , [","                       ])
        CommonUtils.insertEnum(textEnum , "EvJoin"                  , i++, [CommonConstants.Name] , ["+"                       ])

        //전화 관련 팝업 다이얼로그
        CommonUtils.insertEnum(textEnum , "NotificationKo"          , i++, [CommonConstants.Name] , ["알림"])
        CommonUtils.insertEnum(textEnum , "CallKo"                  , i++, [CommonConstants.Name] , ["전화연결"])
        CommonUtils.insertEnum(textEnum,  "CallMessageKo"           , i++, [CommonConstants.Name] , ["에<br>전화 연결하시겠습니까?"])
        CommonUtils.insertEnum(textEnum,  "NeedBTKo"                , i++, [CommonConstants.Name] , ["전화 통화는 스마트폰과의<br>블루투스 연결이 필요합니다."])
        CommonUtils.insertEnum(textEnum,  "FailCalKol"              , i++, [CommonConstants.Name] , ["전화 연결이 실패 하였습니다."])
        CommonUtils.insertEnum(textEnum,  "SuccessCallKo"           , i++, [CommonConstants.Name] , ["전화 연결 대기중 입니다."])
        CommonUtils.insertEnum(textEnum,  "IdleCallKo"              , i++, [CommonConstants.Name] , ["전화 연결 대기중 입니다.."])
        CommonUtils.insertEnum(textEnum,  "DialingCallKo"           , i++, [CommonConstants.Name] , ["전화 연결중 입니다."])
        CommonUtils.insertEnum(textEnum,  "DisconnCalKol"           , i++, [CommonConstants.Name] , ["통화가 종료되었습니다."])
        CommonUtils.insertEnum(textEnum,  "BusyCallKo"              , i++, [CommonConstants.Name] , ["상대방이 통화중입니다."])
        CommonUtils.insertEnum(textEnum , "AlreadyRegKo"            , i++, [CommonConstants.Name] , ["이미 등록되어 있습니다."])

        //전기차 상세정보 - "기타" -> "정보없음"으로 표출을 위하여
        CommonUtils.insertEnum(textEnum , "EtcKo"                   , i++, [CommonConstants.Name] , ["기타"])

        CommonUtils.fetchArr(textEnum , CommonConstants.Name , textArr)
    }

    function allocateImgEnum() {
        if (CommonUtils.getObjectLen(imgEnum) <= 0) {
            imgEnum = CommonUtils.createEnum(imgEnum)
        }
        var i = 0
        CommonUtils.insertEnum(imgEnum , "BookmarkOn"                , i++, [CommonConstants.Name] , ["navi/bookmark_star_on.png"])
        CommonUtils.insertEnum(imgEnum , "BookmarkOff"               , i++, [CommonConstants.Name] , ["navi/bookmark_star_off.png"])
        CommonUtils.insertEnum(imgEnum , "Zone2Mask"                 , i++, [CommonConstants.Name] , ["zone/zone2_img_mask.png"])
        if (CommonUtils.isEqual(searchRoot.__lang, CommonConstants.En)) {
            CommonUtils.insertEnum(imgEnum , "FastOn"                , i++, [CommonConstants.Name] , ["search/list_tag_charge_fast_eng.png"])
            CommonUtils.insertEnum(imgEnum , "FastOff"               , i++, [CommonConstants.Name] , ["search/list_tag_charge_fast_g.png"])
            CommonUtils.insertEnum(imgEnum , "SlowOn"                , i++, [CommonConstants.Name] , ["search/list_tag_charge_slow_eng.png"])
            CommonUtils.insertEnum(imgEnum , "SlowOff"               , i++, [CommonConstants.Name] , ["search/list_tag_charge_slow_g.png"])
            CommonUtils.insertEnum(imgEnum , "Gasoline"              , i++, [CommonConstants.Name] , ["search/bookmark_ic_gasoline_eng.png"])
            CommonUtils.insertEnum(imgEnum , "Diesel"                , i++, [CommonConstants.Name] , ["search/bookmark_ic_disel_eng.png"])
            CommonUtils.insertEnum(imgEnum , "LPG"                   , i++, [CommonConstants.Name] , ["search/bookmark_ic_lpg.png"])
            CommonUtils.insertEnum(imgEnum , "HighGasoline"          , i++, [CommonConstants.Name] , ["search/bookmark_ic_premium_eng.png"])
        } else {
            CommonUtils.insertEnum(imgEnum , "FastOn"                , i++, [CommonConstants.Name] , ["search/list_tag_charge_fast.png"])
            CommonUtils.insertEnum(imgEnum , "FastOff"               , i++, [CommonConstants.Name] , ["search/list_tag_charge_fast_g.png"])
            CommonUtils.insertEnum(imgEnum , "SlowOn"                , i++, [CommonConstants.Name] , ["search/list_tag_charge_slow.png"])
            CommonUtils.insertEnum(imgEnum , "SlowOff"               , i++, [CommonConstants.Name] , ["search/list_tag_charge_slow_g.png"])
            CommonUtils.insertEnum(imgEnum , "Gasoline"              , i++, [CommonConstants.Name] , ["search/bookmark_ic_gasoline.png"])
            CommonUtils.insertEnum(imgEnum , "Diesel"                , i++, [CommonConstants.Name] , ["search/bookmark_ic_disel.png"])
            CommonUtils.insertEnum(imgEnum , "LPG"                   , i++, [CommonConstants.Name] , ["search/bookmark_ic_lpg.png"])
            CommonUtils.insertEnum(imgEnum , "HighGasoline"          , i++, [CommonConstants.Name] , ["search/bookmark_ic_premium.png"])
        }
        CommonUtils.insertEnum(imgEnum , "BrandNotEv"                , i++, [CommonConstants.Name] , ["search/hud_highway_icon"])
        CommonUtils.insertEnum(imgEnum , "BrandEv"                   , i++, [CommonConstants.Name] , ["search/hud_way_icon"])
        CommonUtils.insertEnum(imgEnum , "Underbar"                  , i++, [CommonConstants.Name] , ["_"])
        CommonUtils.insertEnum(imgEnum , "Normal"                    , i++, [CommonConstants.Name] , ["n"])
        CommonUtils.insertEnum(imgEnum , "Focus"                     , i++, [CommonConstants.Name] , ["f"])
        CommonUtils.insertEnum(imgEnum , "Dot"                       , i++, [CommonConstants.Name] , ["."])
        CommonUtils.insertEnum(imgEnum , "Png"                       , i++, [CommonConstants.Name] , ["png"])

        CommonUtils.fetchArr(imgEnum , CommonConstants.Name , imgArr)
    }

    function allocateEvBrandEnum() {
        if (CommonUtils.getObjectLen(evBrandEnum) <= 0) {
            evBrandEnum = CommonUtils.createEnum(evBrandEnum)
        }
        var i = 0
        CommonUtils.insertEnum(evBrandEnum , "NONE"      , i++, [CommonConstants.Key, CommonConstants.Name] , ["00", CommonConstants.Empty])
        CommonUtils.insertEnum(evBrandEnum , "ME"        , i++, [CommonConstants.Key, CommonConstants.Name] , ["01", textArr[textEnum.ME]])
        CommonUtils.insertEnum(evBrandEnum , "KP"        , i++, [CommonConstants.Key, CommonConstants.Name] , ["02", textArr[textEnum.KP]])
        CommonUtils.insertEnum(evBrandEnum , "HM"        , i++, [CommonConstants.Key, CommonConstants.Name] , ["03", textArr[textEnum.HM]])
        CommonUtils.insertEnum(evBrandEnum , "KM"        , i++, [CommonConstants.Key, CommonConstants.Name] , ["04", textArr[textEnum.KM]])
        CommonUtils.insertEnum(evBrandEnum , "SE"        , i++, [CommonConstants.Key, CommonConstants.Name] , ["05", textArr[textEnum.SE]])
        CommonUtils.insertEnum(evBrandEnum , "PI"        , i++, [CommonConstants.Key, CommonConstants.Name] , ["06", textArr[textEnum.PI]])
        CommonUtils.insertEnum(evBrandEnum , "HE"        , i++, [CommonConstants.Key, CommonConstants.Name] , ["07", textArr[textEnum.HE]])
        CommonUtils.insertEnum(evBrandEnum , "CT"        , i++, [CommonConstants.Key, CommonConstants.Name] , ["08", textArr[textEnum.CT]])
        CommonUtils.insertEnum(evBrandEnum , "DG"        , i++, [CommonConstants.Key, CommonConstants.Name] , ["09", textArr[textEnum.DG]])
        CommonUtils.insertEnum(evBrandEnum , "JD"        , i++, [CommonConstants.Key, CommonConstants.Name] , ["10", textArr[textEnum.JD]])
        CommonUtils.insertEnum(evBrandEnum , "JE"        , i++, [CommonConstants.Key, CommonConstants.Name] , ["11", textArr[textEnum.JE]])
        CommonUtils.insertEnum(evBrandEnum , "TM"        , i++, [CommonConstants.Key, CommonConstants.Name] , ["00", textArr[textEnum.TM]])
        CommonUtils.insertEnum(evBrandEnum , "BG"        , i++, [CommonConstants.Key, CommonConstants.Name] , ["00", textArr[textEnum.BG]])
        CommonUtils.insertEnum(evBrandEnum , "PW"        , i++, [CommonConstants.Key, CommonConstants.Name] , ["00", textArr[textEnum.PW]])
        CommonUtils.insertEnum(evBrandEnum , "UNKNOWN"   , i++, [CommonConstants.Key, CommonConstants.Name] , ["00", CommonConstants.Empty])

        CommonUtils.fetchArr(evBrandEnum , CommonConstants.Name , evBrandNameArr)
        CommonUtils.fetchArr(evBrandEnum , CommonConstants.Key  , evBrandKeyArr)
    }

    function allocateEvKindEnum() {
        if (CommonUtils.getObjectLen(evKindEnum) <= 0) {
            evKindEnum = CommonUtils.createEnum(evKindEnum)
        }
        //[주의] 로컬 전기차 검색 시에 넘어오는 값으로 순서 변경하지 말것
        var i = 0
        CommonUtils.insertEnum(evKindEnum , "None"              , SearchConstants.EvKindNone,           [CommonConstants.Name] , [CommonConstants.Empty])
        CommonUtils.insertEnum(evKindEnum , "DCdemo"            , SearchConstants.EvKindDCdemo,         [CommonConstants.Name] , [textArr[textEnum.DCdemo]])
        CommonUtils.insertEnum(evKindEnum , "ACslow"            , SearchConstants.EvKindACslow,         [CommonConstants.Name] , [textArr[textEnum.ACslow]])
        CommonUtils.insertEnum(evKindEnum , "DCdemoAC3"         , SearchConstants.EvKindDCdemoAC3,      [CommonConstants.Name] , [textArr[textEnum.AC3]
                                                                                                                                +textArr[textEnum.EvSepar]
                                                                                                                                +textArr[textEnum.DCdemo]])
        CommonUtils.insertEnum(evKindEnum , "DCcombo"           , SearchConstants.EvKindDCcombo,        [CommonConstants.Name] , [textArr[textEnum.DCcombo]])
        CommonUtils.insertEnum(evKindEnum , "DCdemoCombo"       , SearchConstants.EvKindDCdemoCombo,    [CommonConstants.Name] , [textArr[textEnum.DCdemo]
                                                                                                                                +textArr[textEnum.EvSepar]
                                                                                                                                +textArr[textEnum.DCcombo]])
        CommonUtils.insertEnum(evKindEnum , "DCdemoAC3Combo"    , SearchConstants.EvKindDCdemoAC3Combo, [CommonConstants.Name] , [textArr[textEnum.AC3]
                                                                                                                                +textArr[textEnum.EvSepar]
                                                                                                                                +textArr[textEnum.DCdemo]
                                                                                                                                +textArr[textEnum.EvSepar]
                                                                                                                                +textArr[textEnum.DCcombo]])
        CommonUtils.insertEnum(evKindEnum , "AC3"               , SearchConstants.EvKindAC3,            [CommonConstants.Name] , [textArr[textEnum.ACfast]])
        CommonUtils.insertEnum(evKindEnum , "End"               , SearchConstants.EvKindEnd,            [CommonConstants.Name] , [CommonConstants.Empty])

        CommonUtils.fetchArr(evKindEnum , CommonConstants.Name , evTypeArr)
    }


    function allocateSoftKeyEnum() {
        if (CommonUtils.getObjectLen(softKeyEnum) <= 0) {
            softKeyEnum = CommonUtils.createEnum(softKeyEnum)
        }
        var i = 0
        CommonUtils.insertEnum(softKeyEnum , "Back"                 , i++, [CommonConstants.Name] , [ZoneConstants.Back              ])
        CommonUtils.insertEnum(softKeyEnum , "Option"               , i++, [CommonConstants.Name] , [ZoneConstants.Option            ])
        CommonUtils.insertEnum(softKeyEnum , "Navi"                 , i++, [CommonConstants.Name] , [ZoneConstants.Navi              ])
        CommonUtils.insertEnum(softKeyEnum , "Menu"                 , i++, [CommonConstants.Name] , [ZoneConstants.Menu              ])
        CommonUtils.insertEnum(softKeyEnum , "InTheWaypoint"        , i++, [CommonConstants.Name] , [qsTr("경유지로")                ])
        CommonUtils.insertEnum(softKeyEnum , "AtTheEestination"     , i++, [CommonConstants.Name] , [qsTr("목적지로")                ])
        CommonUtils.insertEnum(softKeyEnum , "AddToFavourites"      , i++, [CommonConstants.Name] , [qsTr("즐겨찾기 추가")           ])

        CommonUtils.fetchArr(softKeyEnum , CommonConstants.Name , softKeyMenuArr)
    }

    //전화번호<->유가정보
    function allocateNormalTitleEnum() {
       if (CommonUtils.getObjectLen(normalTitleEnum) <= 0) {
           normalTitleEnum = CommonUtils.createEnum(normalTitleEnum)
       }
       var i = 0
       CommonUtils.insertEnum(normalTitleEnum , "ResultOPIPrice"    , i++, [CommonConstants.Index, CommonConstants.Name] ,[ SearchConstants.ResultOPIPrice    , textArr[textEnum.OPIPrice      ]])
       CommonUtils.insertEnum(normalTitleEnum , "ResultTelNum"      , i++, [CommonConstants.Index, CommonConstants.Name] ,[ SearchConstants.ResultTelNum      , textArr[textEnum.TelNum        ]])
       CommonUtils.insertEnum(normalTitleEnum , "ResultWebSite"     , i++, [CommonConstants.Index, CommonConstants.Name] ,[ SearchConstants.ResultWebSite     , textArr[textEnum.WebSite       ]])
       CommonUtils.insertEnum(normalTitleEnum , "ResultHolliday"    , i++, [CommonConstants.Index, CommonConstants.Name] ,[ SearchConstants.ResultHolliday    , textArr[textEnum.Holliday      ]])
       CommonUtils.insertEnum(normalTitleEnum , "ResultWorkTime"    , i++, [CommonConstants.Index, CommonConstants.Name] ,[ SearchConstants.ResultWorkTime    , textArr[textEnum.WorkTime      ]])
       CommonUtils.insertEnum(normalTitleEnum , "ResultIndustry"    , i++, [CommonConstants.Index, CommonConstants.Name] ,[ SearchConstants.ResultIndustry    , textArr[textEnum.Industry      ]])
       CommonUtils.insertEnum(normalTitleEnum , "ResultParking"     , i++, [CommonConstants.Index, CommonConstants.Name] ,[ SearchConstants.ResultParking     , textArr[textEnum.Parking       ]])
       CommonUtils.insertEnum(normalTitleEnum , "ResultIntroduction", i++, [CommonConstants.Index, CommonConstants.Name] ,[ SearchConstants.ResultIntroduction, textArr[textEnum.Introduction  ]])

       CommonUtils.fetchArr(normalTitleEnum , CommonConstants.Index , normalTitleIndexArr)
       CommonUtils.fetchArr(normalTitleEnum , CommonConstants.Name  , normalTitleNameArr)
    }


    //전기차 항목
    function allocateEvTitleEnum() {
        if (CommonUtils.getObjectLen(evTitleEnum) <= 0) {
            evTitleEnum = CommonUtils.createEnum(evTitleEnum)
        }
        var i = 0

        CommonUtils.insertEnum(evTitleEnum , "ResultFastCharge"         , i++, [CommonConstants.Index, CommonConstants.Name] ,[ SearchConstants.ResultFastCharge            , CommonConstants.Empty])
        CommonUtils.insertEnum(evTitleEnum , "ResultSlowCharge"         , i++, [CommonConstants.Index, CommonConstants.Name] ,[ SearchConstants.ResultSlowCharge            , CommonConstants.Empty])
        CommonUtils.insertEnum(evTitleEnum , "ResultDetailLocation"     , i++, [CommonConstants.Index, CommonConstants.Name] ,[ SearchConstants.ResultDetailLocation        , textArr[textEnum.DetailLocation]])
        CommonUtils.insertEnum(evTitleEnum , "ResultChargingFee"        , i++, [CommonConstants.Index, CommonConstants.Name] ,[ SearchConstants.ResultChargingFee           , textArr[textEnum.ChargingFee]])
        CommonUtils.insertEnum(evTitleEnum , "ResultOperatingTime"      , i++, [CommonConstants.Index, CommonConstants.Name] ,[ SearchConstants.ResultOperatingTime         , textArr[textEnum.OperatingTime]])
        CommonUtils.insertEnum(evTitleEnum , "ResultParkingFree"        , i++, [CommonConstants.Index, CommonConstants.Name] ,[ SearchConstants.ResultParkingFree           , textArr[textEnum.ParkingFree]])
        CommonUtils.insertEnum(evTitleEnum , "ResultCharger"            , i++, [CommonConstants.Index, CommonConstants.Name] ,[ SearchConstants.ResultCharger               , textArr[textEnum.Charger]])
        CommonUtils.insertEnum(evTitleEnum , "ResultOperatingOrgan"     , i++, [CommonConstants.Index, CommonConstants.Name] ,[ SearchConstants.ResultOperatingOrgan        , textArr[textEnum.OperatingOrgan]])
        CommonUtils.insertEnum(evTitleEnum , "ResultTelNum"             , i++, [CommonConstants.Index, CommonConstants.Name] ,[ SearchConstants.ResultTelNum                , textArr[textEnum.TelNum]])
        CommonUtils.insertEnum(evTitleEnum , "ResultOperatingAgencyID"  , i++, [CommonConstants.Index, CommonConstants.Name] ,[ SearchConstants.ResultOperatingAgencyID     , textArr[textEnum.OperatingAgencyID]])
        CommonUtils.insertEnum(evTitleEnum , "ResultAgencyManagementID" , i++, [CommonConstants.Index, CommonConstants.Name] ,[ SearchConstants.ResultAgencyManagementID    , textArr[textEnum.AgencyManagementID]])
        CommonUtils.insertEnum(evTitleEnum , "ResultChargingStationID"  , i++, [CommonConstants.Index, CommonConstants.Name] ,[ SearchConstants.ResultChargingStationID     , textArr[textEnum.ChargingStationID]])

        CommonUtils.fetchArr(evTitleEnum , CommonConstants.Index , evTitleIndexArr)
        CommonUtils.fetchArr(evTitleEnum , CommonConstants.Name  , evTitleNameArr)
    }

    function allocateEvStatusEnum() {
        if (CommonUtils.getObjectLen(evStatusEnum) <= 0) {
            evStatusEnum = CommonUtils.createEnum(evStatusEnum)
        }

        var i = 0
        CommonUtils.insertEnum(evStatusEnum , "ResultUnknown"         , i++, [CommonConstants.Name] ,[textArr[textEnum.StatusUnknow]])
        CommonUtils.insertEnum(evStatusEnum , "ResultError"           , i++, [CommonConstants.Name] ,[textArr[textEnum.StatusError]])
        CommonUtils.insertEnum(evStatusEnum , "ResultReday"           , i++, [CommonConstants.Name] ,[textArr[textEnum.StatusReady]])
        CommonUtils.insertEnum(evStatusEnum , "ResultProceeding"      , i++, [CommonConstants.Name] ,[textArr[textEnum.StatusProceeding]])
        CommonUtils.insertEnum(evStatusEnum , "ResultInactive"        , i++, [CommonConstants.Name] ,[textArr[textEnum.StatusInactive]])
        CommonUtils.insertEnum(evStatusEnum , "ResultChecking"        , i++, [CommonConstants.Name] ,[textArr[textEnum.StatusChecking]])
        CommonUtils.insertEnum(evStatusEnum , "ResultReservation"     , i++, [CommonConstants.Name] ,[textArr[textEnum.StatusReservation]])

        CommonUtils.fetchArr(evStatusEnum , CommonConstants.Name  , evStatusNameArr)
    }

/////////////////////////////////////////////
//주유소 처리를 위해
    function allocateCategoryEnum() {
        if (CommonUtils.getObjectLen(categoryEnum) <= 0) {
            categoryEnum = CommonUtils.createEnum(categoryEnum)
        }
       CommonUtils.insertEnum(categoryEnum , "GasSt"             , SearchConstants.CategoryGasSt             )
       CommonUtils.insertEnum(categoryEnum , "ChargingSt"        , SearchConstants.CategoryChargingSt        )
       CommonUtils.insertEnum(categoryEnum , "ParkingSp"         , SearchConstants.CategoryParkingSp         )
       CommonUtils.insertEnum(categoryEnum , "Bank"              , SearchConstants.CategoryBank              )
       CommonUtils.insertEnum(categoryEnum , "Shopping"          , SearchConstants.CategoryShopping          )
       CommonUtils.insertEnum(categoryEnum , "Restaurant"        , SearchConstants.CategoryRestaurant        )
       CommonUtils.insertEnum(categoryEnum , "Tourism"           , SearchConstants.CategoryTourism           )
       CommonUtils.insertEnum(categoryEnum , "Medical"           , SearchConstants.CategoryMedical           )
       CommonUtils.insertEnum(categoryEnum , "PublicInstitution" , SearchConstants.CategoryPublicInstitution )
       CommonUtils.insertEnum(categoryEnum , "Finance"           , SearchConstants.CategoryFinance           )
       CommonUtils.insertEnum(categoryEnum , "ChargingEvSt"      , SearchConstants.CategoryChargingEvSt      )
    }

    function allocateOilTypeEnum(){
        if (CommonUtils.getObjectLen(oilTypeEnum) <= 0) {
            oilTypeEnum = CommonUtils.createEnum(oilTypeEnum)
        }
        var i = 0
        CommonUtils.insertEnum(oilTypeEnum , "Gasoline"          , i++, [CommonConstants.Image] , [imgArr[imgEnum.Gasoline]])
        CommonUtils.insertEnum(oilTypeEnum , "Diesel"            , i++, [CommonConstants.Image] , [imgArr[imgEnum.Diesel]])
        CommonUtils.insertEnum(oilTypeEnum , "LPG"               , i++, [CommonConstants.Image] , [imgArr[imgEnum.LPG]])
        CommonUtils.insertEnum(oilTypeEnum , "HighGasoline"      , i++, [CommonConstants.Image] , [imgArr[imgEnum.HighGasoline]])

        CommonUtils.fetchArr(oilTypeEnum , CommonConstants.Image  , oilTypeImgArr)

        fuel = [[oilTypeImgArr[oilTypeEnum.Gasoline]        , gasolinePrice    ],
                [oilTypeImgArr[oilTypeEnum.Diesel]          , dieselPrice      ],
                [oilTypeImgArr[oilTypeEnum.LPG]             , lpgPrice         ],
                [oilTypeImgArr[oilTypeEnum.HighGasoline]    , highGasolinePrice]
               ]
    }

    function allocatePoleCateEnum(){
        if (CommonUtils.getObjectLen(poleCateEnum) <= 0) {
            poleCateEnum = CommonUtils.createEnum(poleCateEnum)
        }
        CommonUtils.insertEnum(poleCateEnum , "None"                      , 0                                                              , [CommonConstants.Key] , ["00"])
        CommonUtils.insertEnum(poleCateEnum , "GasStationSK"              , SearchConstants.SK           + SearchConstants.OILBRANDBASE    , [CommonConstants.Key] , ["01"])
        CommonUtils.insertEnum(poleCateEnum , "GasStationGS"              , SearchConstants.GS           + SearchConstants.OILBRANDBASE    , [CommonConstants.Key] , ["03"])
        CommonUtils.insertEnum(poleCateEnum , "GasStationSOIL"            , SearchConstants.SOIL         + SearchConstants.OILBRANDBASE    , [CommonConstants.Key] , ["06"])
        CommonUtils.insertEnum(poleCateEnum , "GasStationOILBANK"         , SearchConstants.OILBANK      + SearchConstants.OILBRANDBASE    , [CommonConstants.Key] , ["02"])
        CommonUtils.insertEnum(poleCateEnum , "GasStationUNKNOWN"         , SearchConstants.UNKNOWNBRAND + SearchConstants.OILBRANDBASE    , [CommonConstants.Key] , ["07"])
        CommonUtils.insertEnum(poleCateEnum , "ChargingStSK"              , SearchConstants.SK           + SearchConstants.CHARGINGBASE    , [CommonConstants.Key] , ["08"])
        CommonUtils.insertEnum(poleCateEnum , "ChargingStGS"              , SearchConstants.GS           + SearchConstants.CHARGINGBASE    , [CommonConstants.Key] , ["10"])
        CommonUtils.insertEnum(poleCateEnum , "ChargingStSOIL"            , SearchConstants.SOIL         + SearchConstants.CHARGINGBASE    , [CommonConstants.Key] , ["13"])
        CommonUtils.insertEnum(poleCateEnum , "ChargingStOILBANK"         , SearchConstants.OILBANK      + SearchConstants.CHARGINGBASE    , [CommonConstants.Key] , ["09"])
        CommonUtils.insertEnum(poleCateEnum , "ChargingStUNKNOWN"         , SearchConstants.UNKNOWNBRAND + SearchConstants.CHARGINGBASE    , [CommonConstants.Key] , ["14"])


        allocateEvBrandEnum()
        CommonUtils.insertEnum(poleCateEnum , "ChargingEvStNONE"           , evBrandEnum.NONE            + SearchConstants.EVCHARGINGBASE  , [CommonConstants.Key] , [evBrandKeyArr[evBrandEnum.NONE    ]])
        CommonUtils.insertEnum(poleCateEnum , "ChargingEvStME"             , evBrandEnum.ME              + SearchConstants.EVCHARGINGBASE  , [CommonConstants.Key] , [evBrandKeyArr[evBrandEnum.ME      ]])
        CommonUtils.insertEnum(poleCateEnum , "ChargingEvStKP"             , evBrandEnum.KP              + SearchConstants.EVCHARGINGBASE  , [CommonConstants.Key] , [evBrandKeyArr[evBrandEnum.KP      ]])
        CommonUtils.insertEnum(poleCateEnum , "ChargingEvStHM"             , evBrandEnum.HM              + SearchConstants.EVCHARGINGBASE  , [CommonConstants.Key] , [evBrandKeyArr[evBrandEnum.HM      ]])
        CommonUtils.insertEnum(poleCateEnum , "ChargingEvStKM"             , evBrandEnum.KM              + SearchConstants.EVCHARGINGBASE  , [CommonConstants.Key] , [evBrandKeyArr[evBrandEnum.KM      ]])
        CommonUtils.insertEnum(poleCateEnum , "ChargingEvStSE"             , evBrandEnum.SE              + SearchConstants.EVCHARGINGBASE  , [CommonConstants.Key] , [evBrandKeyArr[evBrandEnum.SE      ]])
        CommonUtils.insertEnum(poleCateEnum , "ChargingEvStPI"             , evBrandEnum.PI              + SearchConstants.EVCHARGINGBASE  , [CommonConstants.Key] , [evBrandKeyArr[evBrandEnum.PI      ]])
        CommonUtils.insertEnum(poleCateEnum , "ChargingEvStHE"             , evBrandEnum.HE              + SearchConstants.EVCHARGINGBASE  , [CommonConstants.Key] , [evBrandKeyArr[evBrandEnum.HE      ]])
        CommonUtils.insertEnum(poleCateEnum , "ChargingEvStCT"             , evBrandEnum.CT              + SearchConstants.EVCHARGINGBASE  , [CommonConstants.Key] , [evBrandKeyArr[evBrandEnum.CT      ]])
        CommonUtils.insertEnum(poleCateEnum , "ChargingEvStDG"             , evBrandEnum.DG              + SearchConstants.EVCHARGINGBASE  , [CommonConstants.Key] , [evBrandKeyArr[evBrandEnum.DG      ]])
        CommonUtils.insertEnum(poleCateEnum , "ChargingEvStJD"             , evBrandEnum.JD              + SearchConstants.EVCHARGINGBASE  , [CommonConstants.Key] , [evBrandKeyArr[evBrandEnum.JD      ]])
        CommonUtils.insertEnum(poleCateEnum , "ChargingEvStJE"             , evBrandEnum.JE              + SearchConstants.EVCHARGINGBASE  , [CommonConstants.Key] , [evBrandKeyArr[evBrandEnum.JE      ]])
        CommonUtils.insertEnum(poleCateEnum , "ChargingEvStTM"             , evBrandEnum.TM              + SearchConstants.EVCHARGINGBASE  , [CommonConstants.Key] , [evBrandKeyArr[evBrandEnum.TM      ]])
        CommonUtils.insertEnum(poleCateEnum , "ChargingEvStBG"             , evBrandEnum.BG              + SearchConstants.EVCHARGINGBASE  , [CommonConstants.Key] , [evBrandKeyArr[evBrandEnum.BG      ]])
        CommonUtils.insertEnum(poleCateEnum , "ChargingEvStPW"             , evBrandEnum.PW              + SearchConstants.EVCHARGINGBASE  , [CommonConstants.Key] , [evBrandKeyArr[evBrandEnum.PW      ]])
        CommonUtils.insertEnum(poleCateEnum , "ChargingEvStUNKNOWN"        , evBrandEnum.UNKNOWN         + SearchConstants.EVCHARGINGBASE  , [CommonConstants.Key] , [evBrandKeyArr[evBrandEnum.UNKNOWN ]])
    }

    //경로상 검색 브랜드
    function allocatePoleCateOnPathEnum(){
        if (CommonUtils.getObjectLen(poleCateOnPathEnum) <= 0) {
            poleCateOnPathEnum = CommonUtils.createEnum(poleCateOnPathEnum)
        }
        var i = 0
        CommonUtils.insertEnum(poleCateOnPathEnum , "None"                     , i++, [CommonConstants.Key] , ["00"])//00.없음
        CommonUtils.insertEnum(poleCateOnPathEnum , "GasStationSK"             , i++, [CommonConstants.Key] , ["01"])//01.SK
        CommonUtils.insertEnum(poleCateOnPathEnum , "GasStationGS"             , i++, [CommonConstants.Key] , ["03"])//02.GS
        CommonUtils.insertEnum(poleCateOnPathEnum , "GasStationOILBANK"        , i++, [CommonConstants.Key] , ["02"])//03.H-OIL
        CommonUtils.insertEnum(poleCateOnPathEnum , "GasStationSOIL"           , i++, [CommonConstants.Key] , ["06"])//04.S-OIL
        CommonUtils.insertEnum(poleCateOnPathEnum , "GasStationTOIL"           , i++, [CommonConstants.Key] , ["00"])//05.T-OIL
        CommonUtils.insertEnum(poleCateOnPathEnum , "GasStationUNKNOWN"        , i++, [CommonConstants.Key] , ["07"])//06.무폴
        CommonUtils.insertEnum(poleCateOnPathEnum , "ChargingStSK"             , i++, [CommonConstants.Key] , ["08"])//07.SK-GAS
        CommonUtils.insertEnum(poleCateOnPathEnum , "ChargingStGS"             , i++, [CommonConstants.Key] , ["10"])//08.GS-GAS
        CommonUtils.insertEnum(poleCateOnPathEnum , "ChargingStOILBANK"        , i++, [CommonConstants.Key] , ["09"])//09.H-GAS
        CommonUtils.insertEnum(poleCateOnPathEnum , "ChargingStSOIL"           , i++, [CommonConstants.Key] , ["13"])//10.S-OIL GAS
        CommonUtils.insertEnum(poleCateOnPathEnum , "ChargingStUNKNOWN"        , i++, [CommonConstants.Key] , ["14"])//11.LPG무폴
        CommonUtils.insertEnum(poleCateOnPathEnum , "UNKNOWN"                  , i++, [CommonConstants.Key] , ["00"])//12.미확인
        CommonUtils.insertEnum(poleCateOnPathEnum , "E1"                       , i++, [CommonConstants.Key] , ["00"])//13.E1
        CommonUtils.insertEnum(poleCateOnPathEnum , "GasStationSKINCHUN"       , i++, [CommonConstants.Key] , ["00"])//14.SK인천정유
        CommonUtils.insertEnum(poleCateOnPathEnum , "ChargingStSKINCHUN"       , i++, [CommonConstants.Key] , ["00"])//15.SK인천정유GAS
        CommonUtils.insertEnum(poleCateOnPathEnum , "GasStationNCOIL"          , i++, [CommonConstants.Key] , ["00"])//16.NC-OIL
        CommonUtils.insertEnum(poleCateOnPathEnum , "ChargingStNCOIL"          , i++, [CommonConstants.Key] , ["00"])//17.NC-OIL GAS
        CommonUtils.insertEnum(poleCateOnPathEnum , "GasStationNHOIL"          , i++, [CommonConstants.Key] , ["05"])//18.NH-OIL
        CommonUtils.insertEnum(poleCateOnPathEnum , "ChargingStNHOIL"          , i++, [CommonConstants.Key] , ["12"])//19.NH-OIL GAS
        CommonUtils.insertEnum(poleCateOnPathEnum , "GasStationSevenDHOIL"     , i++, [CommonConstants.Key] , ["00"])//20.sevenD-OIL
        CommonUtils.insertEnum(poleCateOnPathEnum , "ChargingStSevenDHOIL"     , i++, [CommonConstants.Key] , ["00"])//21.sevenD-OIL GAS
        CommonUtils.insertEnum(poleCateOnPathEnum , "Alttle"                   , i++, [CommonConstants.Key] , ["04"])//22.알뜰주유소
        CommonUtils.insertEnum(poleCateOnPathEnum , "ChargingStEX"             , i++, [CommonConstants.Key] , ["00"])//23.EX충전소
        CommonUtils.insertEnum(poleCateOnPathEnum , "GasStationEX"             , i++, [CommonConstants.Key] , ["00"])//24.EX주유소
    }

    function allocateSearchTypeEnum(){
        if (CommonUtils.getObjectLen(searchTypeEnum) <= 0) {
            searchTypeEnum = CommonUtils.createEnum(searchTypeEnum)
        }
        CommonUtils.insertEnum(searchTypeEnum , "WithinRadius"  , SettingConstants.SearchTypeWithinRadius  , [CommonConstants.Name] , [qsTr("반경내") ])
        CommonUtils.insertEnum(searchTypeEnum , "AlongRoute"    , SettingConstants.SearchTypeAlongRoute    , [CommonConstants.Name] , [qsTr("경로상") ])

        CommonUtils.fetchArr(searchTypeEnum , CommonConstants.Name , searchTypeArr)
    }

    function getPoleCateImgIdx(enumValue) {
        //경로상의 브랜드 코드가 각기 다름
        if (CommonUtils.isEqual(oilSearchType , searchTypeEnum.AlongRoute)){
            switch(category()) {
                case categoryEnum.GasSt:
                    enumValue-= SearchConstants.OILBRANDBASE
                    if (CommonUtils.isUndefined(typeof poleCateOnPathEnum.properties[enumValue]))
                        return poleCateOnPathEnum.properties[poleCateOnPathEnum.None][CommonConstants.Key]
                    return poleCateOnPathEnum.properties[enumValue][CommonConstants.Key]
                case categoryEnum.ChargingSt:
                    enumValue-= SearchConstants.CHARGINGBASE
                    if (CommonUtils.isUndefined(typeof poleCateOnPathEnum.properties[enumValue]))
                        return poleCateOnPathEnum.properties[poleCateOnPathEnum.None][CommonConstants.Key]
                    return poleCateOnPathEnum.properties[enumValue][CommonConstants.Key]
                case categoryEnum.ChargingEvSt:
                    if (CommonUtils.isUndefined(typeof poleCateEnum.properties[enumValue]))
                        return poleCateEnum.properties[poleCateEnum.None][CommonConstants.Key]
                    return poleCateEnum.properties[enumValue][CommonConstants.Key]
            }
        } else {
            if((poleCateEnum.GasStationSK <= enumValue && enumValue <= poleCateEnum.GasStationUNKNOWN) ||
               (poleCateEnum.GasStationSK + SearchConstants.CATEGORYGAP <= enumValue &&
                enumValue <= poleCateEnum.GasStationUNKNOWN + SearchConstants.CATEGORYGAP)            ||
               (poleCateEnum.ChargingEvStNONE <= enumValue && enumValue <= poleCateEnum.ChargingEvStUNKNOWN)){
                return poleCateEnum.properties[enumValue][CommonConstants.Key]
            }
            return poleCateEnum.properties[poleCateEnum.None][CommonConstants.Key]
       }
    }

    function applyPrice() {
        //전기차충전소
        if (isChargingEvSt()) {
            return
        }
        gasolinePrice     = result[SearchConstants.ResultOPIGasolinePrice]
        dieselPrice       = result[SearchConstants.ResultOPIDieselPrice]
        lpgPrice          = result[SearchConstants.ResultOPILPGPrice]
        highGasolinePrice = result[SearchConstants.ResultOPIHiGasolinePrice]
    }

    function isValidPrice(){
        if( (gasolinePrice + dieselPrice + lpgPrice  + highGasolinePrice) > 0 )
            return true
        return false
    }

    function isValidBrand() {
        var value = result[SearchConstants.ResultOPIPoleCate]
        if ( value >= poleCateEnum.GasStationSK &&  poleCateEnum.GasStationUNKNOWN >= value)
            return true
        if ( value >= poleCateEnum.ChargingStSK && poleCateEnum.ChargingStUNKNOWN >= value)
            return  true
        if ( value >= poleCateEnum.ChargingEvStNONE && poleCateEnum.ChargingEvStUNKNOWN >= value)
            return true
        return false
    }

    function category(){
        if (CommonUtils.isNull(result))
            return -1
        //(전기차충전소제외)주변검색-로컬/서버
        var value = result[SearchConstants.ResultOPIPoleCate]
        if (value >= SearchConstants.OILBRANDBASE && value < SearchConstants.OILBRANDBASE +  SearchConstants.CATEGORYGAP)
            return categoryEnum.GasSt
        else if (value >= SearchConstants.CHARGINGBASE && value < SearchConstants.CHARGINGBASE +  SearchConstants.CATEGORYGAP)
            return  categoryEnum.ChargingSt
        else if (value >= SearchConstants.EVCHARGINGBASE && value < SearchConstants.EVCHARGINGBASE +  SearchConstants.CATEGORYGAP)
            return  categoryEnum.ChargingEvSt

        //전기차충전소-로컬/서버검색
        value = result[SearchConstants.ResultKind]
        if (value >= SearchConstants.OILBRANDBASE && value < SearchConstants.OILBRANDBASE +  SearchConstants.CATEGORYGAP)
            return categoryEnum.GasSt
        else if (value >= SearchConstants.CHARGINGBASE && value < SearchConstants.CHARGINGBASE +  SearchConstants.CATEGORYGAP)
            return  categoryEnum.ChargingSt
        else if (value >= SearchConstants.EVCHARGINGBASE && value < SearchConstants.EVCHARGINGBASE +  SearchConstants.CATEGORYGAP)
            return  categoryEnum.ChargingEvSt

        //(통합검색)서버검색
        if (CommonUtils.isEqual(value , SearchConstants.OilStKindJuyuso))
            return categoryEnum.GasSt
        if (CommonUtils.isEqual(value , SearchConstants.OilStKindChungjeonso))
            return  categoryEnum.ChargingSt
        if (CommonUtils.isEqual(value , SearchConstants.OilStKindEVchungjeonso))
            return  categoryEnum.ChargingEvSt

        return  -1
    }

    function applyBrandIcon() {
        applyPrice()
        stationIconHeight = searchRoot.layout.stationIconHeight
        if (!CommonUtils.isEqual(category() , categoryEnum.GasSt) &&
            !CommonUtils.isEqual(category() , categoryEnum.ChargingSt) &&
            !CommonUtils.isEqual(category() , categoryEnum.ChargingEvSt)) {
            stationIconWidth = 0
            return
       }
       var value = result[SearchConstants.ResultOPIPoleCate]
       if (CommonUtils.isEqual(oilSearchType , searchTypeEnum.AlongRoute)){
           if (CommonUtils.isEmpty(result[SearchConstants.ResultBrand])) {
               result[SearchConstants.ResultBrand]  = result[SearchConstants.ResultOPIPoleCate]
            }
            value = result[SearchConstants.ResultBrand]
       }
       var imgNum = getPoleCateImgIdx(value)
       var index = CommonUtils.number(imgNum)
       if (CommonUtils.isNull(imgNum) || index <= 0 ) {
           stationIconWidth = 0
           return
       }
       stationIconWidth  = searchRoot.layout.stationIconWidth
       if (CommonUtils.isEqual(category() , categoryEnum.ChargingEvSt))  {
           stationIconSource = Resource.imageUrl( imgArr[imgEnum.BrandEv]
                                                + imgArr[imgEnum.Underbar]
                                                + imgNum
                                                + imgArr[imgEnum.Dot]
                                                + imgArr[imgEnum.Png] )
       } else {
           stationIconSource = Resource.imageUrl( imgArr[imgEnum.BrandNotEv]
                                                + imgArr[imgEnum.Underbar]
                                                + imgNum
                                                + imgArr[imgEnum.Dot]
                                                + imgArr[imgEnum.Png] )
       }
    }

//주유소 처리를 위해
/////////////////////////////////////////////

    function focusList(result) {
        requestAtThePOI(result)
        requestAddMarker(result)
    }

    function requestAtThePOI(result) {
      var data = ({})
      // 17.12.8 hskim apply multi map view
      data[SearchConstants.MapView  ] = mapView
      data[CommonConstants.X        ] = result[SearchConstants.ResultX]
      data[CommonConstants.Y        ] = result[SearchConstants.ResultY]
      sendEvent(SearchResultDetailViewEvent.RequestAtThePOI, data )
    }

    function requestAddMarker(result) {
      var data = ({})
      data[CommonConstants.X         ] = result[SearchConstants.ResultX]
      data[CommonConstants.Y         ] = result[SearchConstants.ResultY]
      data[CommonConstants.Index     ] = markerBaseIndex
      data[CommonConstants.FocusIndex] = markerBaseIndex
      data[CommonConstants.Text      ] = CommonConstants.Empty
      sendEvent(SearchResultDetailViewEvent.RequestAddMarker, data )
      searchRoot.haveMarker = true
    }

    function requestRemoveMarker() {
      if (CommonUtils.isNull(result))
          return
      var data = ({})
     data[CommonConstants.X         ] = result[SearchConstants.ResultX]
     data[CommonConstants.Y         ] = result[SearchConstants.ResultY]
     data[CommonConstants.Index     ] = markerBaseIndex
     sendEvent(SearchResultDetailViewEvent.RequestRemoveMarker, data )
     searchRoot.haveMarker = false
    }

    function requestPositionInScreenCenter() {
      sendEvent(SearchResultDetailViewEvent.requestPositionInScreenCenter)
    }

    function getStdName() {
        if (CommonUtils.isNull(result))
            return CommonConstants.Empty
        var titleName = result[SearchConstants.ResultStdName]
        if (CommonUtils.isEqual(titleName , CommonConstants.Empty))
            titleName = result[SearchConstants.ResultName]
        return titleName
    }

    function getAddress() {
        if (CommonUtils.isNull(result))
            return CommonConstants.Empty
        return   CommonUtils.string(result[SearchConstants.ResultAddress])
    }

    function requestInTheWaypoint() {
        var data = ({})
        data[CommonConstants.X          ] = result[SearchConstants.ResultX]
        data[CommonConstants.Y          ] = result[SearchConstants.ResultY]
        data[CommonConstants.Name       ] = getStdName()
        data[CommonConstants.Address    ] = result[SearchConstants.ResultAddress]
        data[SearchConstants.PoiID      ] = result[SearchConstants.ResultPOIID]
        data[SearchConstants.NavSeq     ] = result[SearchConstants.ResultNavseq]
        data[SearchConstants.RpFlag     ] = result[SearchConstants.ResultRpFlag]
        data[SearchConstants.OpiPoleCate] = result[SearchConstants.ResultOPIPoleCate]
        sendEvent(SearchResultDetailViewEvent.RequestInTheWaypoint, data)
    }

    function queryInDriveMode() {
        var data = ({})
        data[CommonConstants.X          ] = result[SearchConstants.ResultX]
        data[CommonConstants.Y          ] = result[SearchConstants.ResultY]
        data[CommonConstants.Name       ] = getStdName()
        data[CommonConstants.Address    ] = result[SearchConstants.ResultAddress]
        data[SearchConstants.PoiID      ] = result[SearchConstants.ResultPOIID]
        data[SearchConstants.NavSeq     ] = result[SearchConstants.ResultNavseq]
        data[SearchConstants.RpFlag     ] = result[SearchConstants.ResultRpFlag]
        data[SearchConstants.OpiPoleCate] = result[SearchConstants.ResultOPIPoleCate]
        sendEvent(SearchResultDetailViewEvent.QueryInDriveMode, data)
    }

    function viewSizeAndModeChanged(height){
        var data = ({})
        data[CommonConstants.B] = height
        sendEvent(SearchResultDetailViewEvent.ViewSizeChanged,  data)

        mapView.viewMode = Renderer.NorthBound
        mapView.moveMode = Renderer.Normal
    }

    function addToFavourites(destType) {
        // 즐겨 찾기에 추가.
        var data = ({})
        data[CommonConstants.X          ] = result[SearchConstants.ResultX]
        data[CommonConstants.Y          ] = result[SearchConstants.ResultY]
        data[CommonConstants.Name       ] = getStdName()
        data[CommonConstants.Address    ] = result[SearchConstants.ResultAddress]
        data[SearchConstants.PoiID      ] = result[SearchConstants.ResultPOIID]
        data[SearchConstants.DestType   ] = destType
        data[SearchConstants.Favorite   ] = true
        data[SearchConstants.IsHome     ] = isHome
        data[SearchConstants.IsOffice   ] = isOffice
        data[SearchConstants.OpiPoleCate] = result[SearchConstants.ResultOPIPoleCate]
        //입구점
        data[SearchConstants.NavSeq     ] = result[SearchConstants.ResultNavseq]
        //RpFlag
        data[SearchConstants.RpFlag     ] = result[SearchConstants.ResultRpFlag]
        data[CommonConstants.Caller   ] = CommonConstants.objectName(ViewRegistry.SearchResultDetailView)
        sendEvent(SearchResultDetailViewEvent.AddFavorite, data)
    }

    function removeFromFavorites() {
        var data = ({})
        data[CommonConstants.X        ] = result[SearchConstants.ResultX]
        data[CommonConstants.Y        ] = result[SearchConstants.ResultY]
        data[SearchConstants.DestType ] = 0
        data[CommonConstants.Caller   ] = CommonConstants.objectName(ViewRegistry.SearchResultDetailView)
        sendEvent(SearchResultDetailViewEvent.RemoveFromFavorite, data )
    }

    function __setFavoriteOn(isOn) {
      // 즐겨찾기 화면에서 추가하기로 상세 메뉴로 온 경우에는 (즐겨찾기 모드)
      // 별도의 버튼을 노출하지 않고 소프트키 '확인' 또는 '즐겨찾기로 추가'만 표시
      // 17.12.04 ymhong
      // DB ID가 0이면 즐겨찾기에 없음.
      isFavorite = isOn
    }

    function showToast(message)   {
        var data = ({})
        data[CommonConstants.Message] = message
        data[CommonConstants.Position] = CommonConstants.ToastUp
        data[CommonConstants.Duration] = 3 * 1000
        sendEvent(SearchResultDetailViewEvent.ShowToast ,data)
    }

   //로컬 전기차 리스트 모델의 value 값이 바뀌는 현상이 있어 함수를 사용하도록 수정: 추후 원인 파악예정
   function getEvValue(index) {
       if (CommonUtils.isNull(result))
           return textArr[textEnum.NoneData]
       var value = result[evTitleIndexArr[index]]
       value = CommonUtils.trim(value)
       return CommonUtils.isEmpty(value) ? textArr[textEnum.NoneData] : value
    }


    function createModel(){
        initEnum()
        var index = 0
        var value = CommonConstants.Empty
        var data = ({})
        //전기차충전소
        if (isChargingEvSt()) {
            for (index = 0; index< evTitleNameArr.length; index++) {
                  value = result[evTitleIndexArr[index]]
                  value = CommonUtils.trim(value)
                  data =  {
                      idx :  index,
                      name:  evTitleNameArr[index],
                      value: CommonUtils.isEmpty(value) ? textArr[textEnum.NoneData] : CommonUtils.string(value)
                  }
                  listModel.set (index, data)
            }
        } else {
            for (index = 0; index< normalTitleNameArr.length; index++) {
                  value = result[normalTitleIndexArr[index]]
                  value = CommonUtils.trim(value)
                  data =  {
                      idx :  index,
                      name:  normalTitleNameArr[index],
                      value: CommonUtils.isEmpty(value) ? textArr[textEnum.NoneData] : CommonUtils.string(value)
                  }
                  listModel.set (index, data)
            }
        }

        messageText =  textArr[textEnum.NoData]
        messageVisible = listModel.count ? false : true
    }

    function closeOwnerRelayView() {
        var data = ({})
        if (!CommonUtils.isNull(owner)) {
          data[CommonConstants.ObjectName] = owner
          sendEvent(SearchResultDetailViewEvent.CloseView, data)
        }
        if (!CommonUtils.isNull(relay)) {
          data[CommonConstants.ObjectName] = relay
          sendEvent(SearchResultDetailViewEvent.CloseView, data)
        }
        if (!CommonUtils.isNull(caller)) {
          data[CommonConstants.ObjectName] = caller
          sendEvent(SearchResultDetailViewEvent.ShowView, data)
        }
        closeView()
    }

    function closeAllView() {
        var data = ({})
        if (!CommonUtils.isNull(owner)) {
          data[CommonConstants.ObjectName] = owner
          sendEvent(SearchResultDetailViewEvent.CloseView, data)
        }
        if (!CommonUtils.isNull(relay)) {
          data[CommonConstants.ObjectName] = relay
          sendEvent(SearchResultDetailViewEvent.CloseView, data)
        }
        if (!CommonUtils.isNull(caller)) {
          data[CommonConstants.ObjectName] = caller
          sendEvent(SearchResultDetailViewEvent.CloseView, data)
        }
        closeView()
    }

    function close() {
        if(searchRoot.visible) {
            viewDeactivated()
        }
        destruct()
        searchRoot.visible = false
        //1.호출뷰에 focus 요청
        if (!CommonUtils.isEmpty(owner)) {
            var data = ({})
            data[CommonConstants.ReFocus   ] = true
            data[CommonConstants.ObjectName] = owner
            sendEvent(SearchResultDetailViewEvent.UpdateView, data)
        }
        //2.변수초기화 (caller,owner,relay... 포함)
        allocateVariant()
        isClosed = true
    }

    function closeView() {
        var data = ({})
        data[CommonConstants.ObjectName] = CommonConstants.objectName(ViewRegistry.SearchResultDetailView)
        sendEvent(SearchResultDetailViewEvent.CloseView, data)
    }

    //"전화연결 여부 팝업" 추가
    function callPhoneNumber(name, phoneNumber) {
        var cancel = null
        var uniqNum = null
        var title   = null
        var context = null
        if (!bluetoothStatus) {
            cancel = false
            uniqNum = textEnum.NotificationKo
            title = textArr[textEnum.NotificationKo]
            context = textArr[textEnum.NeedBTKo]
        } else {
            cancel = true
            uniqNum = textEnum.CallKo
            title = textArr[textEnum.CallKo]
            context = name
                    + "("
                    + CommonUtils.phoneFomatter(phoneNumber)
                    + ")"
                    + textArr[textEnum.CallMessageKo]
        }
        showDialog(uniqNum, title, context, cancel)
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
             arg [CommonConstants.Caller ] = CommonConstants.objectName(ViewRegistry.SearchResultDetailView)
             data[CommonConstants.Arg    ] = arg
             sendEvent(SearchResultDetailViewEvent.CreateView, data)

        }

        function __closeDialog(yes) {
            var uniqNum = dlgUniqueNum
            closeDialog(uniqNum, yes)
        }

        function closeDialog(uniqNum, yes) {
            if (yes){
                switch(uniqNum) {
                   case textEnum.CallKo:
                        var data = ({})
                        var phoneNumber = result[SearchConstants.ResultTelNum]
                        data[SearchConstants.PhoneNumber] =  phoneNumber
                        sendEvent(SearchResultDetailViewEvent.CallPhoneNumber, data)
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

    function getInfoChargingEvSt(result) {
        //"목적지설정"선택 했을때, 경로설정 정상동작 안되는 현상
        //[주의] 로컬 전기차충전소 상세에는 해당값이 넘어오지 않고 있어 검색결과에서 넘어오는 값을 사용
        result[SearchConstants.ResultX]           = searchRoot.result[SearchConstants.ResultX]
        result[SearchConstants.ResultY]           = searchRoot.result[SearchConstants.ResultY]

        //[주의] 개발버전에서만 검색이 되고 있어 상용버전에 반영되기 전까지는 검색결과의 값을 사용하도록 함
        result[SearchConstants.ResultPOIID]       = searchRoot.result[SearchConstants.ResultPOIID]

        result[SearchConstants.ResultNavseq]      = searchRoot.result[SearchConstants.ResultNavseq]

        var info = result[SearchConstants.ResultChargerInfo  ]
        var oper = CommonConstants.Empty
        var type = CommonConstants.Empty
        var lineType = CommonConstants.Empty
        var abbreviation = CommonConstants.Empty
        var data = CommonUtils.split(info, ";")

        var isFastCharger = false
        var status = [0,0]

        //로컬 검색인 경우
        if (data.length <= 0) {
            if (CommonUtils.isEmpty(type)) {
                if (result[SearchConstants.ResultAC3    ]) {
                    if (!CommonUtils.isEmpty(type)) {
                        type += textArr[textEnum.EvSepar]
                              + textArr[textEnum.Space]
                    }
                    type += textArr[textEnum.AC3]
                }
                if (result[SearchConstants.ResultDCDemo ]) {
                    if (!CommonUtils.isEmpty(type)) {
                        type += textArr[textEnum.EvSepar]
                              + textArr[textEnum.Space]
                    }
                    type += textArr[textEnum.DCdemo]
                }
                if (result[SearchConstants.ResultDCCombo]) {
                    if (!CommonUtils.isEmpty(type)) {
                        type += textArr[textEnum.EvSepar]
                              + textArr[textEnum.Space]
                    }
                    type += textArr[textEnum.DCcombo]
                }
            }
            if (result[SearchConstants.ResultFastCharge])
                result[SearchConstants.ResultFastCharge] = type
            if (result[SearchConstants.ResultSlowCharge])
                result[SearchConstants.ResultSlowCharge] = textArr[textEnum.NoneData]
            result[SearchConstants.ResultChargingType  ] = getDeduplicationEvType(type)
            return
        }
        result[SearchConstants.ResultSlowCharge] = 0
        result[SearchConstants.ResultFastCharge] = 0

        var tid = null
        for(var i= 0; i<data.length; i++) {
            var d = data[i]
            var s = CommonUtils.indexOf(d, "[")
            var e = CommonUtils.indexOf(d, "]")
            if(s < 0 && e < 0)
                continue
            var t = CommonUtils.substring(d, s+1, e-s)
            var v = CommonUtils.substring(d, e+1)
            t = CommonUtils.trim(t)
            v = CommonUtils.trim(v)
            if (CommonUtils.isEmpty(t) || CommonUtils.isEmpty(v))
                continue
            switch(t) {
                case SearchConstants.Bid:
                    if (CommonUtils.isAlpaNum(v)) {
                        v = CommonUtils.number(v)
                    }else {
                        if (CommonUtils.isNull(evBrandEnum[v]))
                            continue
                        v = evBrandEnum[v]
                    }
                    if (CommonUtils.isUndefined(typeof evBrandNameArr[v]))
                        continue
                    if (CommonUtils.isNull(evBrandNameArr[v]))
                        continue
                    abbreviation = evBrandNameArr[v]
                    if (!CommonUtils.isIndexOf(oper, abbreviation)) {
                        if (!CommonUtils.isEmpty(oper)) {
                            oper += textArr[textEnum.EvSepar] + textArr[textEnum.Space]
                        } else {
                            //브랜드 코드를 주유소/충전소와 겹치지 않도록 만듦
                             result[SearchConstants.ResultOPIPoleCate] = v + SearchConstants.EVCHARGINGBASE
                        }
                        oper += evBrandNameArr[v]
                    }
                    break;
                case SearchConstants.Tid:
                    if (!CommonUtils.isAlpaNum(v))
                        continue
                    if (evKindEnum.None >  CommonUtils.number(v) ||
                        evKindEnum.End  <= CommonUtils.number(v)) {
                        continue
                    }
                    tid = CommonUtils.number(v)
                    switch(tid) {
                        case evKindEnum.ACslow:
                            isFastCharger = false
                            result[SearchConstants.ResultSlowCharge] += 1
                            if (!CommonUtils.isEmpty(result[SearchConstants.ResultCharger]))
                                result[SearchConstants.ResultCharger] += "<br>"
                            result[SearchConstants.ResultCharger] += CommonUtils.zeroPadding(parseInt(i/5)+1,2)
                                                                   + "."
                                                                   + textArr[textEnum.SlowCharge]
                                                                   + textArr[textEnum.Space]
                                                                   + "-"
                                                                   + textArr[textEnum.Space]
                            break
                        case evKindEnum.DCdemo:
                        case evKindEnum.DCdemoAC3:
                        case evKindEnum.DCcombo:
                        case evKindEnum.DCdemoCombo:
                        case evKindEnum.DCdemoAC3Combo:
                        case evKindEnum.AC3:
                          isFastCharger = true
                          result[SearchConstants.ResultFastCharge] += 1
                          if (CommonUtils.isEmpty(result[SearchConstants.ResultCharger]))
                                lineType = CommonConstants.Empty
                          else
                                result[SearchConstants.ResultCharger] += "<br>"
                          result[SearchConstants.ResultCharger] += CommonUtils.zeroPadding(parseInt(i/5)+1,2)
                                                                 + "."
                                                                 + textArr[textEnum.FastCharge]
                                                                 + textArr[textEnum.Space]
                                                                 + "-"
                                                                 + textArr[textEnum.Space]
                          break
                    }
                    if (isFastCharger) {
                        //TODO: v1.79 143p 충전타입 표시 순서를 AC상 + DC차데모 + DC콤보로 함
                        abbreviation = evTypeArr[CommonUtils.number(v)]
                        if (!CommonUtils.isIndexOf(type, abbreviation)) {
                            if (!CommonUtils.isEmpty(type))
                                type += textArr[textEnum.EvSepar] + textArr[textEnum.Space]
                            type += evTypeArr[CommonUtils.number(v)]
                        }

                       lineType = CommonUtils.replaceAll(evTypeArr[CommonUtils.number(v)]
                                                         , textArr[textEnum.EvSepar]
                                                         , textArr[textEnum.Space]
                                                         + textArr[textEnum.EvJoin]
                                                         + textArr[textEnum.Space])
                    }
                    break;
                 case SearchConstants.StId:
                        switch(tid) {
                         case evKindEnum.DCdemo:
                         case evKindEnum.ACslow:
                         case evKindEnum.DCdemoAC3:
                         case evKindEnum.DCcombo:
                         case evKindEnum.DCdemoCombo:
                         case evKindEnum.DCdemoAC3Combo:
                         case evKindEnum.AC3:
                              if (CommonUtils.isUndefined(typeof evStatusNameArr[CommonUtils.number(v)]))
                                 result[SearchConstants.ResultCharger] += textArr[textEnum.NoneData]
                              else
                                result[SearchConstants.ResultCharger] += evStatusNameArr[CommonUtils.number(v)]
                              switch(CommonUtils.number(v)) {
                                 case evStatusEnum.ResultReday:      //충전대기
                                     status[CommonUtils.number(isFastCharger)] += 1
                                     break
                              }
                              if (isFastCharger) {
                                  if (!CommonUtils.isEmpty(result[SearchConstants.ResultCharger])) {
                                      result[SearchConstants.ResultCharger] += textArr[textEnum.Space]
                                                                             + textArr[textEnum.EvDelim]
                                                                             + textArr[textEnum.Space]
                                                                             + "<font color='#bababa'>"
                                                                             + lineType
                                                                             + "</font>"
                                  }
                              }
                        }//switch
                }//switch
            }//for


        result[SearchConstants.ResultOperatingOrgan] = oper
        result[SearchConstants.ResultChargingType  ] = getDeduplicationEvType(type)


        var color = null
        var n1 = null
        var n2 = null
        if (CommonUtils.number(result[SearchConstants.ResultSlowCharge])) {
            if (result[SearchConstants.ResultOnline]) {
                color = status[0]? "#99d000" : GlobalConstants.kListTextColor
                if (CommonUtils.isEqual(searchRoot.__lang, CommonConstants.En)) {
                    n1 = result[SearchConstants.ResultSlowCharge]
                    n2 = "<font color='"+color+"'>" + status[0] + "</font>"
                    result[SearchConstants.ResultSlowCharge] = n2
                                                             + textArr[textEnum.Space]
                                                             + CommonUtils.string(textArr[textEnum.Chargeable]).toLowerCase()
                                                             + textArr[textEnum.Space]
                                                             + textArr[textEnum.ChargeNumberOf]
                                                             + textArr[textEnum.Space]
                                                             + n1
                } else {
                    result[SearchConstants.ResultSlowCharge] += textArr[textEnum.ChargeNumber]
                    result[SearchConstants.ResultSlowCharge] += textArr[textEnum.Space]
                                                              + textArr[textEnum.ChargeNumberOf]
                                                              + textArr[textEnum.Space]
                                                              + "<font color='"+color+"'>"
                                                              + status[0]
                                                              + textArr[textEnum.ChargeNumber]
                                                              + "</font>"
                                                              + textArr[textEnum.Space]
                                                              + textArr[textEnum.Chargeable]
                }
            } else {
                 result[SearchConstants.ResultSlowCharge] += textArr[textEnum.Space]
                                                           + textArr[textEnum.ChargeNumber]
            }
        }


        if (CommonUtils.number(result[SearchConstants.ResultFastCharge])) {
            if (result[SearchConstants.ResultOnline]) {
                color = status[1]? "#4ca6ff" : GlobalConstants.kListTextColor
                if (CommonUtils.isEqual(searchRoot.__lang, CommonConstants.En )) {
                    n1 = result[SearchConstants.ResultFastCharge]
                    n2 = "<font color='"+color+"'>" + status[1] + "</font>"
                    result[SearchConstants.ResultFastCharge] = n2
                                                             + textArr[textEnum.Space]
                                                             + CommonUtils.string(textArr[textEnum.Chargeable]).toLowerCase()
                                                             + textArr[textEnum.Space]
                                                             + textArr[textEnum.ChargeNumberOf]
                                                             + textArr[textEnum.Space]
                                                             + n1
                } else {
                    result[SearchConstants.ResultFastCharge] += textArr[textEnum.ChargeNumber]
                    result[SearchConstants.ResultFastCharge] += textArr[textEnum.Space]
                                                              + textArr[textEnum.ChargeNumberOf]
                                                              + textArr[textEnum.Space]
                                                              + "<font color='"+color+"'>"
                                                              + status[1]
                                                              + textArr[textEnum.ChargeNumber]
                                                              + "</font>"
                                                              + textArr[textEnum.Space]
                                                              + textArr[textEnum.Chargeable]
                }
            } else {
                result[SearchConstants.ResultFastCharge] += textArr[textEnum.Space]
                                                          + textArr[textEnum.ChargeNumber]
            }
        }

        if (!CommonUtils.isEmpty(result[SearchConstants.ResultFastCharge])) {
            result[SearchConstants.ResultFastCharge] += textArr[textEnum.Space]
                                                      + textArr[textEnum.EvDelim]
                                                      + textArr[textEnum.Space]
                                                      + "<font color='#bababa'>"
                                                      + getDeduplicationEvType(type)
                                                      + "</font>"
        }
    }

    //전기차 충전타입 중복 제거
    function getDeduplicationEvType(type) {
        var i = 0
        var ret = CommonConstants.Empty
        var data = CommonUtils.split(type, textArr[textEnum.EvSepar])
        if (data.length <= 0) {
            return CommonConstants.Empty
        }
        for( i=0; i<data.length; i++) {
            data[i] = CommonUtils.trim(data[i])
        }
        data = data.reduce(function(a,b){
            if(a.indexOf(b)<0)
                a.push(b)
            return a
        },[])
        data = data.sort(function(a,b){
                if (CommonUtils.isEqual(a, b))
                    return 0
                return (a<b) ? -1 : 1
            }
        )
        for(i=0; i<data.length; i++) {
            if (!CommonUtils.isEmpty(ret)) {
                ret += textArr[textEnum.EvSepar]
                     + textArr[textEnum.Space]
            }
            ret += data[i]
        }
        return ret
    }


    function getInfoStation(result) {
        //serversearch.cpp의 toServer()함수 수정으로 문제 해결 - 추후 상세 검증시 주석처리 필요함
        //- 검색결과에서 유종중 유가가 하나라도 있는 경우만 상세를 사용하지 않도록 수정
        applyPrice()
        if (isValidPrice()) {
            if (CommonUtils.isEqual(category() , categoryEnum.GasSt) || CommonUtils.isEqual(category() , categoryEnum.ChargingSt)) {
                result[SearchConstants.ResultOPIGasolinePrice    ] = searchRoot.result[SearchConstants.ResultOPIGasolinePrice    ]
                result[SearchConstants.ResultOPIDieselPrice      ] = searchRoot.result[SearchConstants.ResultOPIDieselPrice      ]
                result[SearchConstants.ResultOPILPGPrice         ] = searchRoot.result[SearchConstants.ResultOPILPGPrice         ]
                result[SearchConstants.ResultOPIHiGasolinePrice  ] = searchRoot.result[SearchConstants.ResultOPIHiGasolinePrice  ]
                result[SearchConstants.ResultOPIHiLPGPrice       ] = searchRoot.result[SearchConstants.ResultOPIHiLPGPrice       ]
            }
        }

        var text = CommonConstants.Empty
        var value = result[SearchConstants.ResultParking]
        var index = CommonUtils.number(value)
        switch(index) {
            case 0: text = textArr[textEnum.ImPossible]; break;
            case 1: text = textArr[textEnum.Possible];   break;
            default: text = CommonConstants.Empty; break;
        }
        result[SearchConstants.ResultParking] = text


        if (result[SearchConstants.ResultHolliday])
            text = textArr[textEnum.NoHollidays]
        else
            text = CommonConstants.Empty
        result[SearchConstants.ResultHolliday] = text

         var addInfo = result[SearchConstants.ResultWorkTime]
         var data = CommonUtils.split(addInfo, ";")
         result[SearchConstants.ResultWorkTime]  = CommonConstants.Empty

         for(var i= 0; i<data.length; i++) {
             var d = data[i]
             var s = CommonUtils.indexOf(d, "[")
             var e = CommonUtils.indexOf(d, "]")
             if(s >= 0 && e >= 0) {
                 d = CommonUtils.substring(d, s+1, e-s)
                 var f = CommonUtils.substring(data[i], e+1)
                 if (CommonUtils.indexOf(textArr[textEnum.HollidayKo], d) >= 0 ){
                     if (!CommonUtils.isEmpty(f)) {
                        result[SearchConstants.ResultHolliday] = f
                     }
                 } else if (CommonUtils.indexOf(textArr[textEnum.WorkTimeKo], d) >= 0){
                     if (!CommonUtils.isEmpty(f)) {
                        result[SearchConstants.ResultWorkTime] = f
                     }
                 } else if(CommonUtils.indexOf(textArr[textEnum.ParkingKo], d) >=0){
                     if (!CommonUtils.isEmpty(f)) {
                        result[SearchConstants.ResultParking] = f
                     }
                 }
             }
        }

        if (isValidBrand()) {
            result[SearchConstants.ResultOPIPoleCate] = searchRoot.result[SearchConstants.ResultOPIPoleCate]
        }
        result[SearchConstants.ResultRpFlag  ] = searchRoot.result[SearchConstants.ResultRpFlag]
        result[SearchConstants.ResultOPIPrice] = searchRoot.result[SearchConstants.ResultOPIPrice]
    }

    function initEnum() {
        allocateTextEnum()
        allocateImgEnum()

        allocateSoftKeyEnum()
        allocateCategoryEnum()
        allocateOilTypeEnum()
        allocatePoleCateEnum()
        allocatePoleCateOnPathEnum()
        allocateNormalTitleEnum()
        allocateEvTitleEnum()
        allocateEvStatusEnum()
        allocateEvBrandEnum()
        allocateEvKindEnum()
        allocateSearchTypeEnum()
    }

    function init(){
        initEnum()
        unloadMainComponent()
    }

    property bool isShowProgressIndicator: false
    function showProgressView() {
        reqSearch = true
        sendEvent(SearchResultDetailViewEvent.ShowProgressIndicator)
        isShowProgressIndicator = true
    }

    function hideProgressView() {
        sendEvent(SearchResultDetailViewEvent.HideProgressIndicator)
        isShowProgressIndicator = false
    }

    function stopProgressView() {
        reqSearch = false
        timerProg = CommonUtils.destroy(timerProg)
        hideProgressView()
    }

    //맵모드의 설정값 요청: __resultSettingDirectionGuid함수로 응답
    function requestDirectionGuid() {
        var data = ({})
        data[CommonConstants.Section] = SettingConstants.DirectionGuid
        data[CommonConstants.Slot] = "__resultSettingDirectionGuid"
        sendEvent(SearchResultDetailViewEvent.RequestSetting, data )
    }

    //맵모드의 설정값 (requestDirectionGuid)요청에 대한 응답
    function __resultSettingDirectionGuid(settings) {
        if (CommonUtils.isNull(settings))
            return
        if (!settings.hasOwnProperty(SettingConstants.MapMode))
            return
        var value = settings[SettingConstants.MapMode][CommonConstants.Value]
        var index = CommonUtils.number(value)
        changeMapViewMode(index)
    }

  function changeMapViewMode(index) {
      switch (index) {
      case SettingConstants.MapModeBirdView:
          mapView.viewMode = Renderer.BirdView
          break
      case SettingConstants.MapModeDriveDirection:
          mapView.viewMode = Renderer.HeadUp
          break
      case SettingConstants.MapModeNorthDirection:
          mapView.viewMode = Renderer.NorthBound
          break
      }
  }

    function requestQuery() {
        //VR요청: 상세 화면을 보여주기 위하여 ...
        if (CommonUtils.isUndefined(typeof result[SearchConstants.ResultPOIID]) ||
            !CommonUtils.isNumber(result[SearchConstants.ResultPOIID])   ||
            result[SearchConstants.ResultPOIID] <= 0                      ){
            processResult(CommonConstants.Empty, result, 1, 1)
            return
        }

        showProgressView()
        var params = ({})
        params[SearchConstants.PoiID] = result[SearchConstants.ResultPOIID]
        params[SearchConstants.NavSeq] = result[SearchConstants.ResultNavseq]
        params[SearchConstants.Kind] =  kind
        params[SearchConstants.StartIndex] = 0
        params[SearchConstants.Count]      = 1

        console.log("[Perf][SearchResultDetailView][requestQuery]addressType="+addressType)
        var data =({})
        data[SearchConstants.Query       ] = getStdName()
        data[SearchConstants.Params      ] = params
        data[SearchConstants.SearchType  ] = isChargingEvSt() ? SearchConstants.EvDetail : SearchConstants.Detail
        sendEvent(SearchResultDetailViewEvent.RequestSearch, data )

        //[주의] 검색 요청이 블럭되는 경우 처리를 위하여 ...
        queueChanged(0)
    }

    function displaySystemZone() {
        sendEvent(SearchResultDetailViewEvent.SelectZone1Title, textArr[textEnum.Zone1Title])

        var menus = null
        if (favorite) {
            menus = [softKeyMenuArr[softKeyEnum.AddToFavourites],
                     CommonConstants.NoOption]
        } else {
            if (CommonUtils.isIndexOf(caller, [CommonConstants.objectName(ViewRegistry.RouteSettingView)])) {
                sendEvent(SearchResultDetailViewEvent.RequestViaMenuText)
                menus = [viaMenuText, CommonConstants.NoOption]
            } else {
                if (isRg) {
                    menus = [softKeyMenuArr[softKeyEnum.InTheWaypoint],
                             softKeyMenuArr[softKeyEnum.AtTheEestination],
                             CommonConstants.NoOption]
                } else {
                    menus = [softKeyMenuArr[softKeyEnum.AtTheEestination],
                             CommonConstants.NoOption]
                }
            }
        }
        sendEvent(SearchResultDetailViewEvent.Activated, menus)
    }

    function destruct() {
        timerProg = CommonUtils.destroy(timerProg)
        timerClick = CommonUtils.destroy(timerClick)
        sendEvent(SearchResultDetailViewEvent.RemoveSearchQueue)
        requestRemoveMarker()
        hideProgressView()
    }

    function unloadMainComponent() {
        loader.sourceComponent = undefined
    }

    function loadMainComponent() {
        if (CommonUtils.isNull(loader.sourceComponent)) {
          loader.sourceComponent = main
        }
    }

    function requestNrefresh() {
        isClosed = false
        loadMainComponent()
        console.log("caller = " + caller)
        console.log("owner = " + owner)
        console.log("relay = " + relay)

        if (!isActivated) {
            isActivated = true
            if (CommonUtils.isNull(callerResult)) {
                callerResult = result
            }
            stationIconWidth  = searchRoot.layout.stationIconWidth
            requestQuery()
        }

        if (searchRoot.visible) {
            sendEvent(SearchResultDetailViewEvent.ResetCsw)
            sendEvent(SearchResultDetailViewEvent.RegistCsw, [ csw ])

            displaySystemZone()

            if (CommonUtils.isUndefined(typeof callerResult[SearchConstants.ResultX]) ||
                CommonUtils.isUndefined(typeof callerResult[SearchConstants.ResultY]) ||
                callerResult[SearchConstants.ResultX] < 0                             ||
                callerResult[SearchConstants.ResultY] < 0                           ) {
            } else {
                //즐겨찾기에 존재 유무 요청->응답: __setFavoriteOn(bool)
                var data = {}
                data[CommonConstants.X] = callerResult[SearchConstants.ResultX]
                data[CommonConstants.Y] = callerResult[SearchConstants.ResultY]
                data[SearchConstants.PoiID] = callerResult[SearchConstants.ResultPOIID]
                sendEvent(SearchResultDetailViewEvent.QueryIsFavorite, data)

               //상세검색 결과의 x,y좌표를 사용하지 않고 검색결과의 x,y좌표를 사용
               if (CommonUtils.isEqual(owner, CommonConstants.objectName(ViewRegistry.SearchResultView))) {
                   focusList(searchRoot.callerResult)
               }
            }

            //검색결과를 보관하고 있던 최근 목적지 또는 즐겨찾기로 부터 온 경우는 x,y좌표가 바뀌었을 수 있으므로
            //POI검색으로 얻은 x,y좌표를 사용하도록 한다.
            if (!searchRoot.haveMarker ||
                !CommonUtils.isEqual(owner, CommonConstants.objectName(ViewRegistry.SearchResultView))) {
                focusList(searchRoot.result)
            }
        }
    }

    function activate() {
        if (!visible) {
            return
        }
        viewSizeAndModeChanged(frameHeight)
        isActivated = false
        requestNrefresh()
    }

    onReFocusChanged: {
        if (!reFocus)
            return
        reFocus = false
        if (!isMediated)
            return
        if (!searchRoot.visible) {
           onInit()
        }
    }

    onMediated: {
        onInit()
    }

    onActivated: {
        activate()
    }

    onDeactivated: {
        sendEvent(SearchResultDetailViewEvent.Deactivated)
    }

    Component.onDestruction: {
       destruct()
    }

    function onInit() {
        console.log("[Perf][SearchResultDetailView]onInit")
        isClosed = false
        if (!isMediated) {
            isMediated = true
            sendEvent(SearchResultDetailViewEvent.RequestLang)
            sendEvent(SearchResultDetailViewEvent.QueryOnlineStatus)
            sendEvent(SearchResultDetailViewEvent.RequestAddressType)
            sendEvent(SearchResultDetailViewEvent.RequestRouteGuidanceState)
            init()
        }
        if(!searchRoot.visible) {
           if (CommonUtils.isNull(result)) {
               closeView()
           } else {
               requestNrefresh()
           }
        }
    }

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
        sendEvent(SearchResultDetailViewEvent.RequestAddressType)
    }

    function __changedUserProfile() {
        sendEvent(SearchResultDetailViewEvent.RequestAddressType)
    }

    function __onResultFavor(search, result, totalCount, startIndex) {
        console.log("[Perf][SearchResultDetailView][__onResultFavor]before")
        console.log("[Perf][SearchResultDetailView][__onResultFavor]after")
    }

    function __onResult(search, result, totalCount, startIndex)  {
        console.log("[Perf][SearchResultDetailView][__onResult]before")
        processResult(search, result, totalCount, startIndex)
        console.log("[Perf][SearchResultDetailView][__onResult]after")
    }

    function processResult(search, result, totalCount, startIndex) {
        console.log("[Perf][SearchResultDetailView][processResult]before")
        stopProgressView()
        if(totalCount > 0) {
            //상세검색결과에는 넘어오지 않는 값을 검색결과에서 넘어오는 값으로 할당
            result[SearchConstants.ResultAddrCode   ] = searchRoot.result[SearchConstants.ResultAddrCode   ]
            result[SearchConstants.ResultRpFlag     ] = searchRoot.result[SearchConstants.ResultRpFlag     ]
            result[SearchConstants.ResultDeg        ] = searchRoot.result[SearchConstants.ResultDeg        ]
            result[SearchConstants.ResultOPIPoleCate] = searchRoot.result[SearchConstants.ResultOPIPoleCate]
            result[SearchConstants.ResultOPIPrice   ] = searchRoot.result[SearchConstants.ResultOPIPrice   ]
            result[SearchConstants.ResultOilCompany ] = searchRoot.result[SearchConstants.ResultOilCompany ]
            result[SearchConstants.ResultLowestOilSt] = searchRoot.result[SearchConstants.ResultLowestOilSt]
            result[SearchConstants.ResultBrand      ] = searchRoot.result[SearchConstants.ResultBrand      ]
            if (result[SearchConstants.ResultKind] <= 0) {
                result[SearchConstants.ResultKind   ] = searchRoot.result[SearchConstants.ResultKind       ]
            }

            //[주의]경로상검색의 검색결과의 이름과 상세정보의 이름이 다른 경우가 있어 검색결과의 이름을 사용하도록 수정함
            var title = getStdName()
            if (!CommonUtils.isEmpty(title)) {
                result[SearchConstants.ResultName] = title
                result[SearchConstants.ResultStdName] = title
            } else {
                //serversearch.cpp의 toServer()함수 수정으로 문제 해결되어 됨
                if (CommonUtils.isEmpty(result[SearchConstants.ResultStdName])) {
                    if (!CommonUtils.isEmpty(searchRoot.result[SearchConstants.ResultStdName])){
                        result[SearchConstants.ResultName] = searchRoot.result[SearchConstants.ResultStdName]
                        result[SearchConstants.ResultStdName] = searchRoot.result[SearchConstants.ResultStdName]
                    } else if(!CommonUtils.isEmpty(searchRoot.result[SearchConstants.ResultName])){
                        result[SearchConstants.ResultName] = searchRoot.result[SearchConstants.ResultName]
                        result[SearchConstants.ResultStdName] = searchRoot.result[SearchConstants.ResultName]
                    }
                }
            }

            //구주소가 넘어오고 있어 사용하지 않고 상세결과를 사용하지 않음
            if (!CommonUtils.isEmpty(searchRoot.result[SearchConstants.ResultAddress])){
                result[SearchConstants.ResultAddress] = searchRoot.result[SearchConstants.ResultAddress]
            }

            //전기차충전소 - 로컬검색일때 전화번호를 얻을수 없어 POI상세검색을 한 결과에서 얻어온 전화번호
            result[SearchConstants.ResultTelNum] = CommonUtils.trim(result[SearchConstants.ResultTelNum])
            if (CommonUtils.isEmpty(result[SearchConstants.ResultTelNum])) {
                result[SearchConstants.ResultTelNum] = searchRoot.result[SearchConstants.ResultTelNum]
            }

            //[*주의*]경로상 전기차 상세 정보가 넘어오고 있지 않아서...
            if (!searchRoot.online &&
                CommonUtils.isEqual(oilSearchType , searchTypeEnum.AlongRoute)) {
                result[SearchConstants.ResultAC3         ] = searchRoot.result[SearchConstants.ResultAC3         ]
                result[SearchConstants.ResultDCDemo      ] = searchRoot.result[SearchConstants.ResultDCDemo      ]
                result[SearchConstants.ResultDCCombo     ] = searchRoot.result[SearchConstants.ResultDCCombo     ]
                result[SearchConstants.ResultFastCharge  ] = searchRoot.result[SearchConstants.ResultFastCharge  ]
                result[SearchConstants.ResultSlowCharge  ] = searchRoot.result[SearchConstants.ResultSlowCharge  ]
            }

            //전기차충전소
            if (isChargingEvSt()) {
                getInfoChargingEvSt(result)
            } else {
                getInfoStation(result)
            }
            searchRoot.result = result
        } else {
            if (isChargingEvSt()) {
                searchRoot.result[SearchConstants.ResultChargerInfo]  = CommonConstants.Empty
                getInfoChargingEvSt(searchRoot.result)
            }
        }

        applyBrandIcon()
        createModel()

        //전기차충전소
        if (isChargingEvSt()) {
            styleLoader.sourceComponent =  newEvChargType
        } else {
            styleLoader.sourceComponent = stationIconWidth ? gasChargType : normalType
        }
        if (!searchRoot.visible) {
            searchRoot.visible = true
        }
        console.log("[Perf][SearchResultDetailView][processResult]after")
    }

    function isChargingEvSt() {
        //[주의]전기차충전소 서버검색인 경우
        //      result[SearchConstants.ResultKind]나 result[SearchConstants.ResultOPIPoleCate]로도 범주를 알수 없음
        if (CommonUtils.isEqual(kind, categoryEnum.ChargingEvSt)) {
            return true
        }
        if (CommonUtils.isEqual(category(), categoryEnum.ChargingEvSt)) {
            return true
        }
        return false
    }

    function isFromRecentDestination() {
      return CommonUtils.isIndexOf(caller, [CommonConstants.objectName(ViewRegistry.SearchTabView)]) &&
             CommonUtils.isIndexOf(owner, [CommonConstants.objectName(ViewRegistry.RecentDestinationPageView)])
    }

    function isFromFavorite() {
      return CommonUtils.isIndexOf(caller, [CommonConstants.objectName(ViewRegistry.SearchTabView)]) &&
             CommonUtils.isIndexOf(owner, [CommonConstants.objectName(ViewRegistry.BookmarkPageView)])
    }

    //언어가 변경되었을 때
    function __retranslateUi(lang) {
        searchRoot.__lang = lang
        init()
        if(searchRoot.visible) {
           activate()
        }
    }

    function __changedOnlineStatus(online) {
        searchRoot.online = online
    }

    function __changedRgState(isRg) {
        searchRoot.isRg = isRg
    }

    function __doSoftKeyFunction(button, menuName, menuIndex) {
        var data = ({})
        switch (button) {
            case softKeyMenuArr[softKeyEnum.Back]:
                data[CommonConstants.Owner] = owner
                data[CommonConstants.Relay] = relay
                data[CommonConstants.Caller] = caller

                if (isFromFavorite()) {
                    // 즐겨찾기 Tab에서 바로 온 경우에는 SearchTabView 복구.
                    sendEvent(SearchResultDetailViewEvent.ReturnToFavoriteView, data)
                } else if (isFromRecentDestination()) {
                    sendEvent(SearchResultDetailViewEvent.ReturnToRecentDestinationView, data)
                } else if (favorite) {
                    // 1. 즐겨 찾기가 아닌 네비메뉴에서 집,회사로를 선택해서 온 경우
                    // 2. 즐겨 찾기 -> 최근목적지 -> 상세로 들어온 경
                    sendEvent(SearchResultDetailViewEvent.ReturnToRecentDestinationView, data)
                } else {
                    data = ({})
                    data[CommonConstants.ObjectName] = owner
                    sendEvent(SearchResultDetailViewEvent.ShowView, data)
                }
                closeView()
                break
            case softKeyMenuArr[softKeyEnum.Menu]:
                if (CommonUtils.isIndexOf(caller, [CommonConstants.objectName(ViewRegistry.RouteSettingView)])) {
                    if (CommonUtils.isEqual(menuName, viaMenuText)) {
                        requestInTheWaypoint()
                        closeOwnerRelayView()
                        sendEvent(SearchResultDetailViewEvent.UpdateMapView)
                    }
                } else {
                    switch (menuName) {
                        case softKeyMenuArr[softKeyEnum.InTheWaypoint]:
                            requestInTheWaypoint()
                            closeAllView()
                            sendEvent(SearchResultDetailViewEvent.UpdateMapView)
                            break
                        case softKeyMenuArr[softKeyEnum.AtTheEestination]:
                            queryInDriveMode()
                            if (!isRg) {
                                closeAllView()
                            }
                            break
                        case softKeyMenuArr[softKeyEnum.AddToFavourites]:
                            if (favorite) { //isFavorite) {
                                var destType = 0
                                if (isHome)
                                    destType = 2
                                else if (isOffice)
                                    destType = 1

                                addToFavourites(destType)
                                data[CommonConstants.ObjectName] = relay
                                sendEvent(SearchResultDetailViewEvent.CloseView, data)
                                data[CommonConstants.ObjectName] = owner
                                sendEvent(SearchResultDetailViewEvent.CloseView, data)
                                sendEvent(SearchResultDetailViewEvent.ReturnToFavoriteView)
                                closeView()
                            } else {
                                if (isFavorite) {
                                    showToast(textArr[textEnum.AlreadyRegKo])
                                }
                            }
                            break
                    }//switch (menuName)
                }
                break
        }//switch
    }

    function __onBluetoothStatus(status) {
        bluetoothStatus = status
    }

    function __onCallStatus(status) {
        callStatus = status
        var text = CommonConstants.Empty
        switch(callStatus) {
           case SearchConstants.CallStatusFail:             text = textArr[textEnum.FailCallKo]      ;    break;
           case SearchConstants.CallStatusSuccess:          text = textArr[textEnum.SuccessCallKo]   ;    break;
           case SearchConstants.CallStatusIDLE:             text = textArr[textEnum.IdleCallKo]      ;    break;
           case SearchConstants.CallStatusDIALING:          text = textArr[textEnum.DialingCallKo]   ;    break;
           case SearchConstants.CallStatusACTIVE:           text = CommonConstants.Empty             ;    break;
           case SearchConstants.CallStatusON_HOLD:          text = CommonConstants.Empty             ;    break;
           case SearchConstants.CallStatusDISCONNECTING:    text = textArr[textEnum.DisconnCallKo]   ;    break;
           case SearchConstants.CallStatusBUSY:             text = textArr[textEnum.BusyCallKo]      ;    break;
           case SearchConstants.CallStatusCONFERENCE:       text = CommonConstants.Empty             ;    break;
           case SearchConstants.CallStatusIN_VOICEMAIL:     text = CommonConstants.Empty             ;    break;
           case SearchConstants.CallStatusRINGTONE:         text = CommonConstants.Empty             ;    break;
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
        Column {
            onParentChanged: {
                if (CommonUtils.isNull(parent))
                    return
                width = parent.width
                height = parent.height
            }
            Item { width: parent.width; height:  parent.height - frameHeight }
            Item {
                width: parent.width
                height: frameHeight
                Column { width: parent.width; height: parent.height
                    Rectangle{
                        Component.onCompleted: searchRoot.titleRange = this
                        width: parent.width
                        height: searchRoot.layout.titleRange
                        color: GlobalConstants.kBasicBackgoundColor
                        //백그라운드에 있는 검색결과 뷰의 리스트 목록이 클릭되지 않도록
                        MouseArea { anchors.fill: parent}
                        Column {width: parent.width;height:parent.height
                            Row {
                                width: parent.width
                                height: parent.height - 1
                                Item {width:GlobalConstants.kListTextLeftMargin;height:parent.height}
                                Item {width: parent.width -(GlobalConstants.kListTextLeftMargin+GlobalConstants.kListTextRightMargin+63+30)
                                    height: parent.height
                                    Loader {
                                       width: parent.width
                                       height:parent.height
                                       Component.onCompleted: searchRoot.styleLoader = this
                                    }
                                }
                                Item {width:GlobalConstants.kListTextRightMargin;height:parent.height}
                                TImageButton {
                                    // 즐겨찾기 버튼
                                    Component.onCompleted: searchRoot.favoriteButton = this

                                    // 즐겨찾기 추가메뉴에 의한 결과 상세인경우 버튼 표시하지 않음
                                    // 17.12.04 ymhong
                                    visible: !searchRoot.favorite
                                    width: 63
                                    height: 63
                                    anchors.verticalCenter: parent.verticalCenter
                                    bgSources: function (state) {
                                        return isFavorite ? imgArr[imgEnum.BookmarkOn] : imgArr[imgEnum.BookmarkOff]
                                    }
                                    onClicked: {
                                        if (isFavorite) {
                                            isFavorite = false
                                            removeFromFavorites()
                                        } else {
                                            isFavorite = true
                                            addToFavourites(0)
                                        }
                                    }
                                }
                                Item {width: 30;height: parent.height}
                            }
                            Row {width: parent.width;height: 1
                                Item {width:10;height:parent.height}
                                Rectangle {width:parent.width - (10+10); height:parent.height;color:GlobalConstants.kBasicLineColor}
                                Item {width:10;height:parent.height}
                            }
                        }
                    }
                    Rectangle {
                        Component.onCompleted: searchRoot.frame = this
                        width: parent.width
                        height: parent.height - searchRoot.layout.titleRange
                        color: GlobalConstants.kBasicBackgoundColor
                        ListView {
                            Component.onCompleted: searchRoot.listView = this
                            width: parent.width
                            height: parent.height
                            property int rowHeight: searchRoot.layout.rowHeight
                            property int marginHeight: 22
                            property int textHeight: rowHeight - marginHeight

                            property var pressedItem: null
                            PressBar {
                                currentItem: listView.pressedItem
                                leftMargin: GlobalConstants.kListBackgroundLeftMargin
                                rightMargin: GlobalConstants.kListBackgroundRightMargin
                                visible: false
                            }
                            highlight: HighlightBar {
                                currentItem: listView.currentItem
                                leftMargin: GlobalConstants.kListBackgroundLeftMargin
                                rightMargin: GlobalConstants.kListBackgroundRightMargin
                                color: highlightColor
                                visible: false
                            }
                            highlightFollowsCurrentItem: false
                            cacheBuffer: rowHeight * 3
                            clip: true
                            model: listModel
                            delegate: (isChargingEvSt()) ? listEvDelegate  : listNormalDelegate
                            currentIndex: -1

                            Image {
                                  id: imgMask
                                  width: parent.width
                                  height: 30
                                  y: parent.y + (parent.height - height)
                                  visible:  scrollbar.heightRatio < 1.0 && !listView.dragging
                                  sourceSize.width: width
                                  sourceSize.height: height
                                  source: Resource.imageUrl(imgArr[imgEnum.Zone2Mask])
                            }

                            TScrollBar {
                                id: scrollbar
                                flickable: parent
                                width: 2
                                color: MultiSenseCoordinator.kAmbientColor
                                grooveColor: GlobalConstants.kScrollBarBackColor
                                rightMargin: GlobalConstants.kScrollBarRightMargin
                                dynamicPage: true
                            }
                        }
                        Item { width:parent.width;height:parent.height
                            Text {
                                Component.onCompleted: searchRoot.message = this
                                text: messageText
                                visible: messageVisible
                                anchors.centerIn:  parent
                                font { family: GlobalConstants.kFontFamily;
                                    pixelSize: Resource.pixelToDp(GlobalConstants.kGeneralMessageFontSize) }
                                color: GlobalConstants.kListTextColor
                            }
                        }
                    }
                }//Column
            }//Item
        }//Column
    }//Component main

    //일반
    Component {
         id: normalType
          Row {
              width: parent? parent.width : 0
              height: parent? parent.height : 0
              //타이틀,주소
              Item { width:parent.width;height:parent.height
                  Column { width:parent.width
                      height: (32+12+28)
                      anchors.verticalCenter: parent.verticalCenter
                      Item {width: parent.width; height: 32
                          Text {
                              anchors.fill: parent
                              verticalAlignment: Qt.AlignVCenter
                              font { family: GlobalConstants.kFontFamily;
                                  pixelSize: Resource.pixelToDp(GlobalConstants.kOneLineListTextFontSize); bold: false }
                              color: GlobalConstants.kListTextColor
                              text:  getStdName()
                              elide: Text.ElideRight
                          }
                      }
                      Item {width: parent.width; height: 12}
                      Item {width: parent.width; height: 28
                          Text {
                              anchors.fill: parent
                              verticalAlignment: Qt.AlignVCenter
                              font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(GlobalConstants.kSecondLineListTextFontSize) }
                              color: GlobalConstants.kListSubTextColor
                              text: getAddress()
                              elide: Text.ElideRight
                          }
                      }
                  }//Column
              }//Item
          }//Row
    }//Component normalType

    //주유소 및 충전소
    Component {
         id: gasChargType
          Row {
              width: parent ? parent.width : 0
              height: parent ? parent.height : 0
              //주유소,충전소 브랜드 icon
              Item { width:  stationIconWidth; height: parent.height
                  Image { width: parent.width; height: stationIconHeight
                      sourceSize.width: width
                      sourceSize.height: height
                      anchors.verticalCenter: parent.verticalCenter
                      anchors.horizontalCenter: parent.horizontalCenter
                      source: stationIconSource
                      Component.onCompleted: applyBrandIcon()
                  }
              }
              Item {width:stationIconWidth ? GlobalConstants.kListTextLeftMargin: 0;height:parent.height}
              //타이틀,주소
              Item {width:parent.width - (stationIconWidth + CommonUtils.getItemFromParent(this,-1).width);height:parent.height
                  Column {width:parent.width
                      height: (32+12+28)
                      anchors.verticalCenter: parent.verticalCenter
                      Item {width: parent.width; height: 32
                          Text {
                              anchors.fill: parent
                              verticalAlignment: Qt.AlignVCenter
                              font { family: GlobalConstants.kFontFamily;
                                  pixelSize: Resource.pixelToDp(GlobalConstants.kOneLineListTextFontSize); bold: false }
                              color: GlobalConstants.kListTextColor
                              text:  getStdName()
                              elide: Text.ElideRight
                          }
                      }
                      Item {width: parent.width; height: 12}
                      Item {width: parent.width; height: 28
                          Text {
                              anchors.fill: parent
                              verticalAlignment: Qt.AlignVCenter
                              font { family: GlobalConstants.kFontFamily;
                                  pixelSize: Resource.pixelToDp(GlobalConstants.kSecondLineListTextFontSize) }
                              color: GlobalConstants.kListSubTextColor
                              text: getAddress()
                              elide: Text.ElideRight
                          }
                      }
                  }
              }
          }
    }//Componet gasChargType


    //전기차충전소
      Component {
           id: evChargType
            Row {
                width: parent ? parent.width : 0
                height: parent ? parent.height : 0
                //전기차 브랜드 icon
                Item { width:  stationIconWidth; height: parent.height
                    Image { width: parent.width; height: stationIconHeight
                        sourceSize.width: width
                        sourceSize.height: height
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.horizontalCenter: parent.horizontalCenter
                        source: stationIconSource
                        Component.onCompleted: applyBrandIcon()
                    }
                }
                Item {width:stationIconWidth ? GlobalConstants.kListTextLeftMargin : 0;height:parent.height}
                //타이틀,주소
                Item {width:parent.width - (stationIconWidth + CommonUtils.getItemFromParent(this,-1).width);height:parent.height
                    Column {width:parent.width
                        height: (32+12+28)
                        anchors.verticalCenter: parent.verticalCenter
                        Item {width: parent.width; height: 32
                            Row {width: parent.width;height:parent.height
                                Item { width: parent.width; height:parent.height
                                    Text {
                                        anchors.fill: parent
                                        verticalAlignment: Qt.AlignVCenter
                                        font { family: GlobalConstants.kFontFamily;
                                            pixelSize: Resource.pixelToDp(GlobalConstants.kOneLineListTextFontSize); bold: false }
                                        color: GlobalConstants.kListTextColor
                                        text:  getStdName()
                                        elide: Text.ElideRight
                                        onContentWidthChanged: {
                                          width = contentWidth
                                          parent.width = width
                                        }
                                    }
                                }
                                Item { width: 30; height:parent.height}
                                Item {width: 70; height: parent.height
                                    Image {
                                        anchors.centerIn: parent
                                        width: parent.width
                                        height: 28
                                        sourceSize.width: width
                                        sourceSize.height: height
                                        property var fastCharge: null
                                        Component.onCompleted: {
                                            fastCharge = result[SearchConstants.ResultFastCharge]
                                            if ( CommonUtils.isNull(fastCharge) ) {
                                                return
                                            }
                                            source = CommonUtils.isEmpty(fastCharge)  ? Resource.imageUrl(imgArr[imgEnum.FastOff]) :
                                                     CommonUtils.isBoolean(fastCharge)? (fastCharge ? Resource.imageUrl(imgArr[imgEnum.FastOn]) :
                                                                                                      Resource.imageUrl(imgArr[imgEnum.FastOff])) :
                                                                                        Resource.imageUrl(imgArr[imgEnum.FastOn])
                                        }
                                        visible: (CommonUtils.isNull(fastCharge)) ? false : true
                                    }
                                }
                                Item { width: 6; height:parent.height}
                                Item {width: 70; height: parent.height
                                    Image {
                                        anchors.centerIn: parent
                                        width: parent.width
                                        height: 28
                                        sourceSize.width: width
                                        sourceSize.height: height
                                        property var slowCharge: null
                                        Component.onCompleted: {
                                            slowCharge = result[SearchConstants.ResultSlowCharge]
                                            if ( CommonUtils.isNull(slowCharge)) {
                                                return
                                            }
                                            source = CommonUtils.isEmpty(slowCharge)  ? Resource.imageUrl(imgArr[imgEnum.SlowOff]) :
                                                     CommonUtils.isBoolean(slowCharge)? (slowCharge ? Resource.imageUrl(imgArr[imgEnum.SlowOn]) :
                                                                                                      Resource.imageUrl(imgArr[imgEnum.SlowOff])) :
                                                                                        Resource.imageUrl(imgArr[imgEnum.SlowOn])

                                        }
                                        visible: (CommonUtils.isNull(slowCharge) ) ? false : true
                                    }
                                }
                            }
                        }
                        Item {width: parent.width; height: 12}
                        Item {width: parent.width; height: 28
                            Text {
                                anchors.fill: parent
                                verticalAlignment: Qt.AlignVCenter
                                font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(GlobalConstants.kSecondLineListTextFontSize) }
                                color: GlobalConstants.kListSubTextColor
                                text: getAddress()
                                elide: Text.ElideRight
                            }
                        }
                    }
                }

            }
      }//Componet evChargType

    //[신규]전기차충전소
    Component {
         id: newEvChargType
          Row {
              width: parent ? parent.width : 0
              height: parent ? parent.height : 0
              //전기차 브랜드 icon
              Item { width:  stationIconWidth; height: parent.height
                  Image { width: parent.width; height: stationIconHeight
                      sourceSize.width: width
                      sourceSize.height: height
                      anchors.verticalCenter: parent.verticalCenter
                      anchors.horizontalCenter: parent.horizontalCenter
                      source: stationIconSource
                      Component.onCompleted: applyBrandIcon()
                  }
              }
              Item {width:stationIconWidth ? GlobalConstants.kListTextLeftMargin : 0;height:parent.height}
              //타이틀,주소
              Item {width:parent.width - (stationIconWidth + CommonUtils.getItemFromParent(this,-1).width);height:parent.height
                  Column {width:parent.width
                      height: (32+12+28)
                      anchors.verticalCenter: parent.verticalCenter
                      Item {width: parent.width; height: 32
                          Text {
                              anchors.fill: parent
                              verticalAlignment: Qt.AlignVCenter
                              font { family: GlobalConstants.kFontFamily;
                                  pixelSize: Resource.pixelToDp(GlobalConstants.kOneLineListTextFontSize); bold: false }
                              color: GlobalConstants.kListTextColor
                              text: getStdName()
                              elide: Text.ElideRight
                              onContentWidthChanged: {
                                width = contentWidth
                                parent.width = width
                              }
                          }
                      }
                      Item {width: parent.width; height: 12}
                      Item {width: parent.width; height: 28
                          Text {
                              anchors.fill: parent
                              verticalAlignment: Qt.AlignVCenter
                              font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(GlobalConstants.kSecondLineListTextFontSize) }
                              color: GlobalConstants.kListSubTextColor
                              text: getAddress()
                              elide: Text.ElideRight
                          }
                      }
                  }
              }
          }
    }//Componet evChargType


    //고급휘발유#1
    function getPriceRow() {
        var ret = 0
        for(var i=0;i<fuel.length;i++) {
         if (fuel[i][1] >  0)
             ret++
        }
        return parseInt(ret/3) + ((ret % 3) ? 1 : 0)
    }

    Component {
          id: oilTypeId
          Column {
              property var delegateItemObj:null
              property var model: delegateItemObj.modelObj
              width: parent.width
              height: parent.height
              anchors.centerIn: parent
              spacing: 22
              Repeater {
                  model: getPriceRow()
                  delegate:
                          Row { width:parent.width;height:28
                              id: nodeRow
                              property int i: index
                              Repeater {
                                  model: {
                                      //고급휘발유#1
                                      switch(nodeRow.i) {
                                          case 0: return (getPriceRow() >= 2) ? fuel.length - 1 : fuel.length
                                          case 1: return 1
                                      }
                                  }
                                  delegate: Row { width:parent.width;height:parent.height;Component.onCompleted: CommonUtils.applyWidth(this)
                                         property int i: (nodeRow.i*3)+index
                                         property var fileName:fuel[i][0]
                                         property var price: (fuel[i][1]) ?
                                                                 (CommonUtils.addComma(fuel[i][1]) + textArr[textEnum.Won] ) :
                                                                 textArr[textEnum.NoneData]
                                         Image { width:(fuel[i][1]) ? 64: 0;height:parent.height
                                             sourceSize.width: width
                                             sourceSize.height: height
                                             source: Resource.imageUrl(fileName)
                                         }
                                         Item { width: (fuel[i][1]) ? 10: 0; height: parent.height}
                                         Text {
                                             height: parent.height
                                             verticalAlignment: Qt.AlignVCenter
                                             font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(30) }
                                             color: (delegateItemObj.isFocused && !delegateItemObj.isButtonFocused) ?
                                                        GlobalConstants.kBasicBackgoundColor : GlobalConstants.kListTextColor
                                             text: (fuel[i][1]) ?  price : CommonConstants.Empty
                                         }
                                         Item { width: (fuel[i][1]) ? 22: 0; height: parent.height}
                                  }//Row
                              }//Repeater
                      }//Row
              }//Repeater
          }//Column
      }//Component


      ListModel { id: listModel   }

      //(전기차충전소제외)검색상세
      Component {
          id: listNormalDelegate
          Item {
              id: item
              width: listView.width
              height: listView.rowHeight + bottomMargin

              property var  delegateItemObj: this
              property var  modelObj: model
              property bool isSelected: CommonUtils.isEqual(listView.currentIndex , model.index) && CommonUtils.isEqual(highlightColor , selectcolor)
              property bool isFocused: CommonUtils.isEqual(listView.currentIndex , model.index) && searchRoot.focused
              property bool isButtonFocused: false
              property int  bottomMargin: CommonUtils.isEqual(listView.count-1, model.index) ? listView.marginHeight : 0

              function execButton() {
                  var name = !CommonUtils.isEmpty(result[SearchConstants.ResultStdName]) ?
                              result[SearchConstants.ResultStdName] : result[SearchConstants.ResultName]
                  callPhoneNumber(name, model.value)
              }

              function clickItem(){
                  listView.currentIndex = model.index
                  if (!isFocused) {
                    highlightColor = Qt.binding(function(){ return MultiSenseCoordinator.kAmbientColor})
                  }
                  var sleep =  GlobalConstants.kPressedColorExposureTime
                  timerClick = CommonUtils.destroy(timerClick)
                  timerClick = CommonUtils.delay(searchRoot, sleep, function() {
                      try {
                          highlightColor = CommonConstants.Transparent
                          listView.currentIndex = -1
                      } catch(e) {
                      }
                  })
              }

              onIsFocusedChanged: {
                  if (isFocused) {
                  } else {
                      isButtonFocused = false
                  }
              }
              onIsButtonFocusedChanged: {
                  if (isButtonFocused) {
                  } else {
                  }
              }

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
                      clickItem()
                  }
              }

              Column {width:parent.width; height: parent.height
                  Item {width: parent.width; height:listView.marginHeight}
                  Item {width: parent.width; height: listView.textHeight
                      Row {width: parent.width; height: parent.height
                          Item {width:20; height:parent.height}
                          Item {width:90; height:parent.height
                              Text {
                                  anchors.fill: parent
                                  font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(30); bold: false }
                                  color: (isFocused && !isButtonFocused) ? GlobalConstants.kBasicBackgoundColor : GlobalConstants.kListSubTextColor
                                  text: model.name
                                  elide: Text.ElideRight
                              }
                          }
                          Item {width:30;height:parent.height}
                          Item {width:parent.width - (20+90+30+30); height:parent.height
                              Text {
                                  anchors.fill: parent
                                  wrapMode: Text.WrapAnywhere
                                  font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(30) }
                                  color: {
                                      if (isFocused && !isButtonFocused) {
                                          return GlobalConstants.kBasicBackgoundColor
                                      }  else {
                                         if ( CommonUtils.isEqual(normalTitleNameArr[normalTitleEnum.ResultTelNum] , model.name) &&
                                                 !CommonUtils.isEqual(model.value , textArr[textEnum.NoneData]) && !isSelected)
                                             return MultiSenseCoordinator.kAmbientColor
                                      }
                                      return GlobalConstants.kListTextColor
                                  }
                                  text: {
                                       if (CommonUtils.isNull(model.value))
                                           return CommonConstants.Empty
                                       if (CommonUtils.isEqual(normalTitleNameArr[normalTitleEnum.ResultTelNum] , model.name)) {
                                           if (!CommonUtils.isEqual(model.value , textArr[textEnum.NoneData])){
                                             return CommonUtils.phoneFomatter(model.value)
                                           } else {
                                               color = GlobalConstants.kListTextColor
                                           }
                                       }
                                       return model.value
                                  }
                                  visible:  {
                                        if (CommonUtils.isEqual(normalTitleNameArr[normalTitleEnum.ResultOPIPrice] , model.name)){
                                          if (CommonUtils.isEqual(category() , categoryEnum.GasSt) || CommonUtils.isEqual(category() , categoryEnum.ChargingSt)) {
                                            if (isValidPrice()) {
                                                text = CommonConstants.Empty
                                                var row = getPriceRow()
                                                if (row >= 2) {
                                                    item.height = (28 * row) +  (listView.marginHeight * row)
                                                }
                                                loaderOilTypeId.sourceComponent = oilTypeId
                                            }
                                            return true
                                          }
                                          item.height = 0
                                          item.visible = false
                                          return false
                                      }
                                      return true
                                  }
                                  onContentHeightChanged:{
                                    if (height <= 0)
                                      return
                                    if (contentHeight <= 0)
                                      return
                                    if (contentHeight < height)
                                      return
                                    if(!CommonUtils.isEqual(normalTitleNameArr[normalTitleEnum.ResultOPIPrice] , model.name)) {
                                        item.height = contentHeight +  listView.marginHeight + bottomMargin
                                    }
                                  }
                                  MouseArea {
                                      anchors.fill: parent
                                      enabled: (CommonUtils.isEqual(normalTitleNameArr[normalTitleEnum.ResultTelNum] , model.name)
                                                && !CommonUtils.isEqual(model.value , textArr[textEnum.NoneData]))
                                      onClicked: execButton()
                                  }
                              }//Text
                              Loader {
                                  id: loaderOilTypeId
                                  anchors.fill: parent
                                  onChildrenChanged: {
                                      if (children.length <= 0)
                                          return
                                      children[0].delegateItemObj = delegateItemObj
                                  }
                              }
                          }//Item
                          Item {width:30;height:parent.height}
                      }//Row
                  }//Item
                  Item {width: parent.width; height: bottomMargin}
              }//Column
          }//Item
      }

      //전기차충전소
      Component {
          id: listEvDelegate
          Item {
              id: item
              width: listView.width
              property var  delegateItemObj: this
              property var  modelObj: model
              property int telNumBottomMarginHeight: 37
              function loadStyle(){
                      if (CommonUtils.isNull(styleLoader))
                          return
                      if (CommonUtils.isNull(result))
                          return
                      var data = result[evTitleIndexArr[model.idx]]
                      switch(model.idx) {
                      case evTitleEnum.ResultFastCharge:
                      case evTitleEnum.ResultSlowCharge:
                          if ( CommonUtils.isNull(data) ||
                               CommonUtils.isEmpty(data) ||
                               (!CommonUtils.isString(data) && !data)) {
                              styleLoader.height = 0
                              break
                          }
                          styleLoader.width = listView.width
                          styleLoader.height =  listView.rowHeight
                          styleLoader.sourceComponent = chargeEvType
                          break
                      case evTitleEnum.ResultDetailLocation:
                           if ( CommonUtils.isNull(data) ||
                                CommonUtils.isEmpty(data)) {
                               styleLoader.height = 0
                               break
                           }
                           styleLoader.width = listView.width
                           styleLoader.height =  listView.rowHeight + CommonUtils.isEqual(model.idx, evTitleEnum.ResultTelNum) ? telNumBottomMarginHeight : 0
                           styleLoader.sourceComponent = normalEvType
                           break
                      case evTitleEnum.ResultChargingFee:
                      case evTitleEnum.ResultOperatingTime:
                      case evTitleEnum.ResultParkingFree:
                      case evTitleEnum.ResultCharger:
                      case evTitleEnum.ResultOperatingOrgan:
                      case evTitleEnum.ResultTelNum:
                          styleLoader.width = listView.width
                          styleLoader.height =  listView.rowHeight + CommonUtils.isEqual(model.idx, evTitleEnum.ResultTelNum) ? telNumBottomMarginHeight  : 0
                          styleLoader.sourceComponent = normalEvType
                          break
                      case evTitleEnum.ResultOperatingAgencyID:
                      case evTitleEnum.ResultAgencyManagementID:
                      case evTitleEnum.ResultChargingStationID:
                          styleLoader.width = listView.width
                          styleLoader.height =  listView.rowHeight
                          styleLoader.sourceComponent = idEvType
                          break
                      }
                }
              property var styleLoader: null
              onStyleLoaderChanged: loadStyle()

              Loader {
                 Component.onCompleted:  styleLoader = this
                 onWidthChanged:  {
                     parent.width = width
                 }
                 onHeightChanged: {
                     parent.height = height
                 }
                 onChildrenChanged: {
                      if (children.length <= 0)
                          return
                      children[0].delegateItemObj =  delegateItemObj
                 }
              }

          }//Item
      }

      //급속,완속 이미지 표시
      Component {
          id: chargeEvType
          Column {
              property var delegateItemObj:null
              property int modelIndex: -1
              property var modelName: null
              property var modelValue: null

              property int bottomMarginHeight: CommonUtils.isEqual(listView.count-1, modelIndex) ? listView.marginHeight : 0

              property bool isSelected: CommonUtils.isEqual(listView.currentIndex , modelIndex) && CommonUtils.isEqual(highlightColor , selectcolor)
              property bool isFocused: CommonUtils.isEqual(listView.currentIndex , modelIndex) && searchRoot.focused
              property bool isButtonFocused: false


              onParentChanged: {
                  if (CommonUtils.isNull(parent))
                      return
                  width = parent.width
                  height = parent.height
              }

              function clickItem(){
                  listView.currentIndex = modelIndex
                  if (!isFocused) {
                    highlightColor = Qt.binding(function(){ return MultiSenseCoordinator.kAmbientColor})
                  }
                  var sleep =  GlobalConstants.kPressedColorExposureTime
                  timerClick = CommonUtils.destroy(timerClick)
                  timerClick = CommonUtils.delay(searchRoot, sleep, function() {
                      try {
                          highlightColor = CommonConstants.Transparent
                          listView.currentIndex = -1
                      } catch(e) {
                      }
                  })
              }

              onIsFocusedChanged: {
                  if (isFocused) {
                  } else {
                      isButtonFocused = false
                  }
              }
              onIsButtonFocusedChanged: {
                  if (isButtonFocused) {
                  } else {
                  }
              }

              onDelegateItemObjChanged: {
                if (CommonUtils.isNull(delegateItemObj))
                    return
                modelIndex = Qt.binding(function(){
                    if (CommonUtils.isNull(delegateItemObj.modelObj))
                        return 0
                    if (CommonUtils.isNull(delegateItemObj.modelObj.idx))
                        return 0
                    return delegateItemObj.modelObj.idx})
                modelName = Qt.binding(function(){
                    if (CommonUtils.isNull(delegateItemObj.modelObj))
                        return 0
                    if (CommonUtils.isNull(delegateItemObj.modelObj.name))
                        return 0
                    return delegateItemObj.modelObj.name})
                modelValue = Qt.binding(function(){
                    if (CommonUtils.isNull(delegateItemObj.modelObj))
                        return 0
                    if (CommonUtils.isNull(delegateItemObj.modelObj.value))
                        return 0
                    return delegateItemObj.modelObj.value})
              }


              Item {width: parent.width; height:listView.marginHeight}
              Item {width: parent.width; height: listView.textHeight
                  Row {width: parent.width; height: parent.height
                      Item {width:40; height:parent.height}
                      Item {width:70; height:parent.height
                          Image {
                              anchors.centerIn: parent
                              width: parent.width
                              height: 28
                              sourceSize.width: width
                              sourceSize.height: height
                              Component.onCompleted: {
                                  switch(modelIndex ) {
                                  case evTitleEnum.ResultFastCharge:
                                      source = Resource.imageUrl(imgArr[imgEnum.FastOn])
                                      break
                                  case evTitleEnum.ResultSlowCharge:
                                      source = Resource.imageUrl(imgArr[imgEnum.SlowOn])
                                      break
                                  }
                              }
                          }
                      }
                      Item {width:30;height:parent.height}
                      Item {width:parent.width - (40+70+30+30); height:parent.height
                          Text {
                              anchors.fill: parent
                              wrapMode: Text.WrapAnywhere
                              lineHeightMode:  Text.FixedHeight
                              lineHeight: listView.marginHeight + parent.height
                              font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(30) }
                              color: {
                                  if (isFocused && !isButtonFocused) {
                                      return GlobalConstants.kBasicBackgoundColor
                                  }
                                  return GlobalConstants.kListTextColor
                              }
                              text: {
                                   var textValue = CommonConstants.Empty
                                   switch(modelName) {
                                        default:
                                            textValue =  getEvValue(modelIndex)
                                            if (CommonUtils.isBoolean(textValue))
                                                textValue = textArr[textEnum.NoneData]
                                            break
                                   }
                                   return textValue
                              }
                              onContentHeightChanged:{
                                if (height <= 0)
                                  return
                                if (contentHeight <= 0)
                                  return
                                if (contentHeight < height)
                                  return
                                delegateItemObj.height = contentHeight
                              }
                          }//Text
                      }//Item
                      Item {width:30;height:parent.height}
                  }//Row
              }//Item
              Item {width: parent.width; height:bottomMarginHeight}
          }//Column
      }

      //상세위치, 충전요금, 운영시간, 주차요금, 충전기, 운영기관, 전화번호
      Component {
          id: normalEvType
          Column {
              property var delegateItemObj:null
              property int modelIndex: -1
              property var modelName: null
              property var modelValue: null

              property int telNumBottomMarginHeight: !CommonUtils.isNull(delegateItemObj) ? delegateItemObj.telNumBottomMarginHeight : 0

              property bool isSelected: CommonUtils.isEqual(listView.currentIndex , modelIndex) && CommonUtils.isEqual(highlightColor , selectcolor)
              property bool isFocused: CommonUtils.isEqual(listView.currentIndex , modelIndex) && searchRoot.focused
              property bool isButtonFocused: false

              onParentChanged: {
                  if (CommonUtils.isNull(parent))
                      return
                  width = parent.width
                  height = parent.height
              }

              function execButton() {
                  var name = !CommonUtils.isEmpty(result[SearchConstants.ResultStdName]) ? result[SearchConstants.ResultStdName] : result[SearchConstants.ResultName]
                  callPhoneNumber(name, getEvValue(modelIndex))
              }

              function clickItem(){
                  listView.currentIndex = modelIndex
                  if (!isFocused) {
                    highlightColor = Qt.binding(function(){ return MultiSenseCoordinator.kAmbientColor})
                  }
                  var sleep =  GlobalConstants.kPressedColorExposureTime
                  timerClick = CommonUtils.destroy(timerClick)
                  timerClick = CommonUtils.delay(searchRoot, sleep, function() {
                      try {
                          highlightColor = CommonConstants.Transparent
                          listView.currentIndex = -1
                      } catch(e) {
                      }
                  })
              }

              onIsFocusedChanged: {
                  if (isFocused) {
                  } else {
                      isButtonFocused = false
                  }
              }
              onIsButtonFocusedChanged: {
                  if (isButtonFocused) {
                  } else {
                  }
              }

              onDelegateItemObjChanged: {
                if (CommonUtils.isNull(delegateItemObj))
                    return
                modelIndex = Qt.binding(function(){
                    if (CommonUtils.isNull(delegateItemObj.modelObj))
                        return 0
                    if (CommonUtils.isNull(delegateItemObj.modelObj.idx))
                        return 0
                    return delegateItemObj.modelObj.idx})
                modelName = Qt.binding(function(){
                    if (CommonUtils.isNull(delegateItemObj.modelObj))
                        return 0
                    if (CommonUtils.isNull(delegateItemObj.modelObj.name))
                        return 0
                    return delegateItemObj.modelObj.name})
                modelValue = Qt.binding(function(){
                    if (CommonUtils.isNull(delegateItemObj.modelObj))
                        return 0
                    if (CommonUtils.isNull(delegateItemObj.modelObj.value))
                        return 0
                    return delegateItemObj.modelObj.value})
              }

              Item {width: parent.width; height:listView.marginHeight}
              Item {width: parent.width; height: listView.textHeight
                  Row {width: parent.width; height: parent.height
                      Item {width:20; height:parent.height}
                      Item {width:90; height:parent.height
                          Text {
                              anchors.fill: parent
                              font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(30) }
                              color: (isFocused && !isButtonFocused) ? GlobalConstants.kBasicBackgoundColor : GlobalConstants.kListSubTextColor
                              text: modelName
                              elide: Text.ElideRight
                          }
                      }
                      Item {width:30;height:parent.height}
                      Item {width:parent.width - (20+90+30+30); height:parent.height
                          Column {
                              width: parent.width
                              height: parent.height
                              property var modelData: ([])
                              Component.onCompleted: {
                                 modelData = Qt.binding(function(){
                                     var textValue = CommonConstants.Empty
                                     switch(modelName) {
                                         case evTitleNameArr[evTitleEnum.ResultChargingFee]:
                                         case evTitleNameArr[evTitleEnum.ResultDetailLocation]:
                                         case evTitleNameArr[evTitleEnum.ResultOperatingTime]:
                                             textValue =  getEvValue(modelIndex)
                                             textValue = CommonUtils.trim(textValue)
                                             if (CommonUtils.isEqual(textValue, textArr[textEnum.EtcKo])) {
                                                 textValue = textArr[textEnum.NoneData]
                                             }
                                             break
                                         case evTitleNameArr[evTitleEnum.ResultParkingFree]:
                                             textValue =  getEvValue(modelIndex)
                                             if (CommonUtils.isEqual(textValue, textArr[textEnum.NoneData]))
                                                 break
                                             if (!CommonUtils.isNull(result) &&
                                                 result[SearchConstants.ResultStationCount]) {
                                                 if (textValue)
                                                     textValue = textArr[textEnum.Free]
                                                 else
                                                     textValue = textArr[textEnum.Pay]
                                             } else {
                                                 textValue = textArr[textEnum.NoneData]
                                             }
                                             break
                                          case evTitleNameArr[evTitleEnum.ResultCharger]:
                                          case evTitleNameArr[evTitleEnum.ResultOperatingOrgan]:
                                              textValue =  getEvValue(modelIndex)
                                              break
                                          case evTitleNameArr[evTitleEnum.ResultTelNum]:
                                              textValue =  getEvValue(modelIndex)
                                              if (!CommonUtils.isEqual(textValue , textArr[textEnum.NoneData])){
                                                textValue = CommonUtils.phoneFomatter(textValue)
                                              }
                                              break
                                          default:
                                              textValue =  getEvValue(modelIndex)
                                              break
                                     }
                                     var ret = []
                                     var s = CommonUtils.split(textValue, "<br>")
                                     if (CommonUtils.isObject(s)) {
                                         ret = s
                                     } else {
                                         ret[0] = s
                                     }
                                     if (CommonUtils.isEqual(modelIndex, evTitleEnum.ResultTelNum))
                                         delegateItemObj.height = (ret.length * listView.rowHeight) + telNumBottomMarginHeight
                                     else
                                         delegateItemObj.height = ret.length * listView.rowHeight
                                     return ret
                                  })
                              }
                              onModelDataChanged: {
                                  rp.model = modelData
                                  for(var i=0; i<modelData.length; i++)
                                    rp.itemAt(i).text = modelData[i]
                              }
                              Repeater {
                                  id: rp
                                  delegate:
                                  Text {
                                    width: parent.width
                                    height: listView.rowHeight
                                    font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(30) }
                                    elide: Text.ElideRight
                                    color: {
                                        if (isFocused && !isButtonFocused) {
                                          return GlobalConstants.kBasicBackgoundColor
                                        }  else {
                                             if ( CommonUtils.isEqual(evTitleNameArr[evTitleEnum.ResultTelNum] , modelName) &&
                                                  !CommonUtils.isEqual(getEvValue(modelIndex) , textArr[textEnum.NoneData]) && !isSelected) {
                                                    return MultiSenseCoordinator.kAmbientColor
                                             }
                                        }
                                        return GlobalConstants.kListTextColor
                                    }
                                    MouseArea {
                                        anchors.fill: parent
                                        enabled: (CommonUtils.isEqual(evTitleNameArr[evTitleEnum.ResultTelNum] , modelName) &&
                                                 !CommonUtils.isEqual(getEvValue(modelIndex) , textArr[textEnum.NoneData]))
                                        onClicked: {
                                            execButton()
                                        }
                                    }
                                  }
                              }
                          }//Column
                      }//Item
                      Item {width:30;height:parent.height}
                  }//Row
              }//Item
              Item {width: parent.width; height: CommonUtils.isEqual(modelIndex, evTitleEnum.ResultTelNum) ? telNumBottomMarginHeight - 1 : 0}
              Row {width: parent.width;height: CommonUtils.isEqual(modelIndex, evTitleEnum.ResultTelNum)? 1 : 0
                  Item {width:10;height:parent.height}
                  Rectangle {width:parent.width - (10+10); height:parent.height;color:GlobalConstants.kBasicLineColor}
                  Item {width:10;height:parent.height}
              }
              //Rectangle {width: parent.width;height:CommonUtils.isEqual(modelIndex, evTitleEnum.ResultTelNum)? 22 : 0; color:"red"}
          }//Column
      }

      //ID항목: 운영기관 ID,기관관리 ID,충전소 ID
      Component {
          id: idEvType
          Column {
              property var delegateItemObj:null
              property int modelIndex: -1
              property var modelName: null
              property var modelValue: null

              property int bottomMarginHeight: CommonUtils.isEqual(listView.count-1, modelIndex) ? listView.marginHeight : 0

              property bool isSelected: CommonUtils.isEqual(listView.currentIndex , modelIndex) && CommonUtils.isEqual(highlightColor , selectcolor)
              property bool isFocused: CommonUtils.isEqual(listView.currentIndex , modelIndex) && searchRoot.focused
              property bool isButtonFocused: false

              onParentChanged: {
                  if (CommonUtils.isNull(parent))
                      return
                  width = parent.width
                  height = Qt.binding(function(){
                      if (CommonUtils.isUndefined(typeof bottomMarginHeight) ||
                          CommonUtils.isNull(bottomMarginHeight))
                          return 0
                      return parent.height + bottomMarginHeight
                  })
              }

              onIsFocusedChanged: {
                  if (isFocused) {
                  } else {
                      isButtonFocused = false
                  }
              }
              onIsButtonFocusedChanged: {
                  if (isButtonFocused) {
                  } else {
                  }
              }

              onDelegateItemObjChanged: {
                if (CommonUtils.isNull(delegateItemObj))
                    return
                modelIndex = Qt.binding(function(){
                    if (CommonUtils.isNull(delegateItemObj.modelObj))
                        return 0
                    if (CommonUtils.isNull(delegateItemObj.modelObj.idx))
                        return 0
                    return delegateItemObj.modelObj.idx})
                modelName = Qt.binding(function(){
                    if (CommonUtils.isNull(delegateItemObj.modelObj))
                        return 0
                    if (CommonUtils.isNull(delegateItemObj.modelObj.name))
                        return 0
                    return delegateItemObj.modelObj.name})
                modelValue = Qt.binding(function(){
                    if (CommonUtils.isNull(delegateItemObj.modelObj))
                        return 0
                    if (CommonUtils.isNull(delegateItemObj.modelObj.value))
                        return 0
                    return delegateItemObj.modelObj.value})
              }


              Item {width: parent.width; height:listView.marginHeight}
              Item {width: parent.width; height: listView.textHeight
                  Row {width: parent.width; height: parent.height
                      Item {width:20; height:parent.height}
                      Item {width:146; height:parent.height
                          Text {
                              anchors.fill: parent
                              font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(30); bold: false }
                              color: (isFocused && !isButtonFocused) ? GlobalConstants.kBasicBackgoundColor : GlobalConstants.kListSubTextColor
                              text: modelName
                              elide: Text.ElideRight
                          }
                      }
                      Item {width:30;height:parent.height}
                      Item {width:parent.width -(20+146+30+30); height:parent.height
                          Text {
                              anchors.fill: parent
                              wrapMode: Text.WrapAnywhere
                              lineHeightMode:  Text.FixedHeight
                              lineHeight: listView.marginHeight + parent.height
                              font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(30) }
                              color: {
                                  if (isFocused && !isButtonFocused) {
                                      return GlobalConstants.kBasicBackgoundColor
                                  }
                                  return GlobalConstants.kListTextColor
                              }
                              text: {
                                   var textValue = CommonConstants.Empty
                                   switch(modelName) {
                                   case evTitleNameArr[evTitleEnum.ResultOperatingAgencyID]:
                                   case evTitleNameArr[evTitleEnum.ResultAgencyManagementID]:
                                   case evTitleNameArr[evTitleEnum.ResultChargingStationID]:
                                   default:
                                       textValue =  getEvValue(modelIndex)
                                       break
                                   }
                                   return textValue
                              }
                              onContentHeightChanged:{
                                if (height <= 0)
                                  return
                                if (contentHeight <= 0)
                                  return
                                if (contentHeight < height)
                                  return
                                delegateItemObj.height = contentHeight + bottomMarginHeight
                              }
                          }//Text
                      }//Item
                      Item {width:30;height:parent.height}
                  }//Row
              }//Item
              Item {width: parent.width; height:bottomMarginHeight}
          }//Column
      }




      Csw {
          id: csw

          section: Csw.Section_List
          sequence: 0
          enable: false//searchRoot.visible

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
          onLeftPressed: {
              var item = listView.currentItem
              if (!CommonUtils.isNull(item.isButtonFocused)) {
                  item.isButtonFocused = false
              }
              highlightColor = GlobalConstants.kListFocusColor
          }
          onRightPressed: {
              var item = listView.currentItem
              if (!CommonUtils.isNull(item.buttonVisible) && item.buttonVisible) {
                  item.isButtonFocused = true
                  highlightColor = GlobalConstants.kBasicBackgoundColor
              }
          }
          onSpinCwPressed: {
              if (listView.currentIndex < listView.count - 1) {
                  ++listView.currentIndex
                  //비어있는 행이 있어...
                  while( listView.currentIndex < listView.count - 1 && listView.currentItem.height <= 0) {
                      ++listView.currentIndex
                  }
                  var item = listView.currentItem
                  if (!CommonUtils.isNull(item.isButtonFocused) && item.isButtonFocused) {
                      item.isButtonFocused = false
                  }
                  highlightColor = GlobalConstants.kListFocusColor
              }
          }
          onSpinCcwPressed: {
              if (0 < listView.currentIndex) {
                  --listView.currentIndex
                  //비어있는 행이 있어...
                  while( 0 < listView.currentIndex && listView.currentItem.height <= 0) {
                      --listView.currentIndex
                  }
                  var item = listView.currentItem
                  item.isButtonFocused = false
                  highlightColor = GlobalConstants.kListFocusColor
              }
          }
          onSelected: {
              var item = listView.currentItem
              if (!CommonUtils.isNull(item.isButtonFocused) && item.isButtonFocused) {
                  item.execButton()
              } else {
                  item.clickItem()
              }
          }
      }
}
