import QtQuick 2.1
import ViewRegistry 1.0
import CommonConstants 1.0
import ZoneConstants 1.0
import SettingConstants 1.0
import SearchConstants 1.0
import SearchResultViewEvent 1.0
import SearchResultDetailViewEvent 1.0
import SceneGraphRendering 1.0
import Csw 1.0

import "../../singleton"
import "../../component"
import "../../view"
import "../../view/util"
import "../../view/search"
import "../../GlobalConstants.js" as GlobalConstants
import "../../view/util/CommonUtils.js" as CommonUtils
import "../../view/util/Formatter.js" as Formatter


PopupView {
    id: searchRoot

    fader.fadingOpacity                         : 0
    background.visible                          : false
    mouseArea.enabled                           : false

    visible                                     : true

    property bool reFocus                       : false
    property bool isClosed                      : true
    property bool isInitSetting                 : false
    property bool isShowTopView                 : false

    //onMediated에서 요청 및 응답
    property var __lang                         : null
    property bool online                        : false
    property bool isRg                          : false


    property var zone1Title                     : null
    property var zone1Edit                      : null

    property var applyZone1Title                : null
    property var applyZone1Edit                 : null


    //뷰호출객체이름
    property var caller                         : null                    //first  depth(SearchResultView를 호출한 뷰의 호출뷰(필수O))
    property var relay                          : null                    //middle depth(필수X)
    property var owner                          : null                    //last   depth(SearchResultView를 호출한 뷰(필수O))


    //옵션메뉴
    property var options                        : null
    property var newSettings                    : options
    property var optionMenus                    : ([])
    property bool autoClickOption               : false
    property bool manualClickOption             : false
    property int zone1SearchType                : -1                     //Zone1 타이틀 표출에 사용



    //검색을 위한 기본 정보
    property string textParam                   : CommonConstants.Empty  //요청검색어: 주변검색에서는 category를 사용함
    property string query                       : CommonConstants.Empty  //요청/응답검색어
    property string searchType                  : CommonConstants.Empty  //텍스트검색(CommonConstants.Name), 주변검색(CommonConstants.Around), 주소검색(CommonConstants.Address),업종검색(CommonConstants.Industry) 구분값
    property string response                    : CommonConstants.Empty  //응답할 시그널 명(필수X)
    property var result                         : ([])                   //결과값: 리스트와 일치
    property var resultAround                   : ([])                   //결과값: 주변 검색 전체 데이타
    property var resultCache                    : ([])
    property var oldOrder                       : null                   //정렬이 반복되지 않도록 처리하기 위한 값


    //검색을 위한 부가 정보
    property var params                         : null                   //주소검색 및 지역설정 검색에서 사용
    property var kind                           : null                   //categoryEnum 값: 업종검색 주유소,충전소 및 주변검색의 (주유소,충전소,주차장,은행,쇼핑,음식점,관광,병원)메뉴의 구분 코드값
    property var order                          : null                   //Zone3영역의 sort 종류
    property int addrCode                       : 0                      //통합/업종 검색을 위한 (지역)코드값
    property int beforAddrCode                  : 0                      //지역 검색후 결과가 없을 때 이전 데이타 유지를 위하여
    property int currAddrCode                   : 0                      //현재 지역 코드 값
    property int distance                       : SearchResultViewEvent.Distance10Km     //검색 반경 거리 범위의 값
    property string addressType                 : SearchConstants.None   //[주의] Mediator에서 값을 넘김 - 구주소/신주소 검색을 위하여 사용: .RequestAddressType Event요청할 때 결과 값


    //주소 검색시 사용
    property bool isSan                         : false
    property bool isApt                         : false
    property bool isRoad                        : false
    property bool favorite                      : false                  //내비메뉴,즐겨찾기 > 주소검색 >"즐겨찾기"로 부터 검색요청 판단
    property bool isHome                        : false                  //내비메뉴,즐겨찾기 > 주소검색 >"집으로 등록"으로 부터 검색요청 판단
    property bool isOffice                      : false                  //내비메뉴,즐겨찾기 > 주소검색 >"회사로 등록"으로 부터 검색요청 판단
    property string text                        : CommonConstants.Empty
    property string keyword                     : CommonConstants.Empty


    //경로설정으로 부터 검색요청 판단
    property var viaSetting                     : null


    //포커스/셀렉트 관련
    property bool focused                       : false


    //디폴트 값
    readonly property int markerBaseIndex       : 100
    readonly property int lowestCount           : 1
    readonly property int requestCacheMaxCount  : SearchConstants.RequestMaxCount
    readonly property int defaultStartIndex     : 0
    readonly property int reqEvDetailRetryMax   : 3
    readonly property int frameHeight           : 403
    readonly property bool useCurrAddrCode      : true                  //업종검색: 현위치를 기준으로 검색하도록...
    readonly property bool resultEvDataSync     : true                  //requestEvDetailInfo ? false : true


    //타이머
    property var timerQueue                     : null
    property var timerEvDetailInfo              : null
    property var timerCheckList                 : null
    property var timerHideProg                  : null
    property var timerSort                      : null


    //상세/부가정보 요청및 수신 체크를 위하여...
    property var verifyReqDetail                : ({})
    property var verifyResDetail                : ({})
    property var verifyReqEvDetail              : ({})
    property var verifyResEvDetail              : ({})

    //poi상세 및 전기차 상세
    property bool poiDetailSearch               : false


    //뷰 관련
    property bool messageVisible                : false
    property bool isMediated                    : false
    property bool isActivated                   : false
    property string messageText                 : CommonConstants.Empty
    property int listViewHeight                 : 0

    //큐 관련
    property bool reqSearch                     : false


    //ID
    property var listView                       : null
    property var scrollbar                      : null
    property var message                        : null
    property var listModel                      : null

    //Enum
    property var textEnum                       : ({})
    property var imgEnum                        : ({})
    property var softKeyEnum                    : ({})
    property var categoryEnum                   : ({})
    property var categoryOilStEnum              : ({})
    property var oilTypeEnum                    : ({})
    property var poleCateEnum                   : ({})
    property var poleCateOnPathEnum             : ({})
    property var oilBrandEnum                   : ({})
    property var searchTypeEnum                 : ({})
    property var sectionEnum                    : ({})
    property var keyEnum                        : ({})
    property var evBrandEnum                    : ({})
    property var evKindEnum                     : ({})
    property var evTypeEnum                     : ({})
    property var menuEnum                       : ({})


    //-Enum에 대응되는 배열
    property var textArr                        : ([])
    property var imgArr                         : ([])
    property var softKeyMenuArr                 : ([])
    property var oilTypeArr                     : ([])
    property var oilBrandArr                    : ([])
    property var searchTypeArr                  : ([])
    property var keyArr                         : ([])
    property var sectionArr                     : ([])
    property var evBrandNameArr                 : ([])
    property var evBrandKeyArr                  : ([])
    property var evKindArr                      : ([])
    property var evTypeArr                      : ([])
    property var menuArr                        : ([])


    //전기차: 리스트뷰에 보여지는 항목만 상세정보 요청을 위하여
    property int startListIndex                 : -1
    property int endListIndex                   : -1

    property bool isRegionSearch                : false
    property bool showhidebyowner               : false
    property var objectNameOfSetting            : null
    property int settingChargeTypeValue         : SettingConstants.ChargeTypeAC3

    //유종,브랜드,반경/경로
    property int oilSearchType                  : -1
    property int oilKind                        : -1
    property int oilBrand                       : -1
    property int oilArea                        : -1
    property int chargeType                     : -1

    property bool changedSearchType             : false
    property bool changedOilKind                : false
    property bool changedOilBrand               : false
    property bool changedOilArea                : false
    property bool changedChargeType             : false
    property bool isShowProgressIndicator       : false
    property int dlgUniqueNum                   : -1


    //가까운 거리 표시를 위하여...
    property int nearDistValue                  : CommonConstants.MAX_INT_VALUE
    property int lowestPriceValue               : CommonConstants.MAX_INT_VALUE
    property var modelNearIndex                 : null
    property var modelLowestIndex               : null


    property int lowestPOID                     : 0

    property int queueCount                     : 0

    property bool switchedLine                  : false
    property bool withinRadiusFromAlongRoute    : false
    property var updateCount                    : null

    //[주의] Mediator에서 사용하고 있음
    property var viewHandle                     : mapView

    function allocateVariant() {
        reFocus                         = false
        isClosed                        = true
        isShowTopView                   = false
        isInitSetting                   = false

        zone1Title                      = null
        zone1Edit                       = null
        applyZone1Title                 = null
        applyZone1Edit                  = null

        caller                          = null
        relay                           = null
        owner                           = null

        options                         = null
        newSettings                     = Qt.binding(function(){ return options})
        optionMenus                     = ([])
        autoClickOption                 = false
        manualClickOption               = false
        zone1SearchType                 = -1

        textParam                       = CommonConstants.Empty
        query                           = CommonConstants.Empty
        searchType                      = CommonConstants.Empty
        response                        = CommonConstants.Empty
        resultAround                    = ([])
        resultCache                     = ([])
        result                          = ([])
        oldOrder                        = null

        params                          = null
        kind                            = null
        order                           = null
        addrCode                        = 0
        beforAddrCode                   = 0

        isSan                           = false
        isApt                           = false
        isRoad                          = false
        favorite                        = false
        isHome                          = false
        isOffice                        = false
        text                            = CommonConstants.Empty
        keyword                         = CommonConstants.Empty

        viaSetting                      = null

        focused                         = false

        verifyReqDetail                 = ({})
        verifyResDetail                 = ({})
        verifyReqEvDetail               = ({})
        verifyResEvDetail               = ({})

        poiDetailSearch                 = false

        messageVisible                  = false
        isActivated                     = false
        messageText                     = CommonConstants.Empty
        listViewHeight                  = 0

        reqSearch                       = false

        startListIndex                  = -1
        endListIndex                    = -1

        isRegionSearch                  = false
        showhidebyowner                 = false
        objectNameOfSetting             = null
        settingChargeTypeValue          =  SettingConstants.ChargeTypeAC3

        oilSearchType                   = -1
        oilKind                         = -1
        oilBrand                        = -1
        oilArea                         = -1
        chargeType                      = -1

        changedSearchType               = false
        changedOilKind                  = false
        changedOilBrand                 = false
        changedOilArea                  = false
        changedChargeType               = false
        isShowProgressIndicator         = false
        dlgUniqueNum                    = -1

        nearDistValue                   = CommonConstants.MAX_INT_VALUE
        lowestPriceValue                = CommonConstants.MAX_INT_VALUE
        modelNearIndex                  = null
        modelLowestIndex                = null

        lowestPOID                      = 0

        queueCount                      = 0

        switchedLine                    = false
        withinRadiusFromAlongRoute      = false
        updateCount                     = null

        if (!CommonUtils.isNull(searchRoot.listView)) {
            searchRoot.listView.init()
        }

        unloadMainComponent()
        deleteListModel()
    }

    function allocateTextEnum(){
        if (CommonUtils.getObjectLen(textEnum) <= 0) {
            textEnum = CommonUtils.createEnum(textEnum)
        }
        var i = 0
        CommonUtils.insertEnum(textEnum , "Zone1Title"             , i++, [CommonConstants.Name] , [qsTr("검색 결과")])
        CommonUtils.insertEnum(textEnum , "Detail"                 , i++, [CommonConstants.Name] , [qsTr("상세")])
        CommonUtils.insertEnum(textEnum , "NoData"                 , i++, [CommonConstants.Name] , [qsTr("검색 결과가 없습니다.")])
        CommonUtils.insertEnum(textEnum , "Won"                    , i++, [CommonConstants.Name] , [qsTr("원")])
        CommonUtils.insertEnum(textEnum , "InfoNone"               , i++, [CommonConstants.Name] , [qsTr("정보없음")])

        //한글
        CommonUtils.insertEnum(textEnum , "GasKo"                  , i++, [CommonConstants.Name] , ["주유소"])
        CommonUtils.insertEnum(textEnum , "LPGKo"                  , i++, [CommonConstants.Name] , ["충전소"])
        CommonUtils.insertEnum(textEnum , "EVKo"                   , i++, [CommonConstants.Name] , ["전기차 충전소"])
        CommonUtils.insertEnum(textEnum , "NoDataKo"               , i++, [CommonConstants.Name] , ["검색 결과가 없습니다."])
        CommonUtils.insertEnum(textEnum , "NoDataNoNet"            , i++, [CommonConstants.Name] , ["검색 결과가 없습니다.<br>네트워크가 연결되면 다시 시도해 주세요."])
        CommonUtils.insertEnum(textEnum , "KindNoData"             , i++, [CommonConstants.Name] , ["선택한 유종의 %1가 없습니다."])
        CommonUtils.insertEnum(textEnum , "BrandNoData"            , i++, [CommonConstants.Name] , ["선택한 브랜드의 %1가 없습니다."])
        CommonUtils.insertEnum(textEnum , "ChargeNoData"           , i++, [CommonConstants.Name] , ["선택한 충전타입의 전기차 충전소가 없습니다."])
        CommonUtils.insertEnum(textEnum , "NoRoute"                , i++, [CommonConstants.Name] , ["경로설정 정보가 없습니다."])
        CommonUtils.insertEnum(textEnum , "Notification"           , i++, [CommonConstants.Name] , ["알림"])
        CommonUtils.insertEnum(textEnum , "UseOnlineWithinRadius"  , i++, [CommonConstants.Name] , ["경로상 %1가 없습니다<br>반경내 %2를 이용하세요."])
        CommonUtils.insertEnum(textEnum , "UseOfflineWithinRadius" , i++, [CommonConstants.Name] , ["네트워크가 연결되어야 경로상 검색이 가능합니다.<br>반경내 %1를 이용하세요."])
        CommonUtils.insertEnum(textEnum , "Over20Length"           , i++, [CommonConstants.Name] , ["입력한 검색어의 첫 20글자로 검색한 결과입니다."])
        CommonUtils.insertEnum(textEnum , "UseOnlineSortByPrice"   , i++, [CommonConstants.Name] , ["네트워크가 연결되어야 가격순으로 정렬 가능합니다."])
        CommonUtils.insertEnum(textEnum , "UseRetrySortByPrice"    , i++, [CommonConstants.Name] , ["서버에 문제가 있어 가격순 정렬이 불가합니다.<br>잠시 후 다시 시도해 주세요."])
        CommonUtils.insertEnum(textEnum , "SwtichOfflineSearch"    , i++, [CommonConstants.Name] , ["서버에 문제가 있어 로컬 검색으로 전환되었습니다.<br>반경내 검색만 가능합니다."])

        CommonUtils.insertEnum(textEnum , "Space"                  , i++, [CommonConstants.Name] , [String.fromCharCode(0x20)])


        CommonUtils.fetchArr(textEnum , CommonConstants.Name , textArr)
    }

    function allocateImgEnum() {
        if (CommonUtils.getObjectLen(imgEnum) <= 0) {
            imgEnum = CommonUtils.createEnum(imgEnum)
        }
        var i = 0
        CommonUtils.insertEnum(imgEnum , "BaseBtnNormalName"         , i++, [CommonConstants.Name] , ["btn_bg_n.png"])
        CommonUtils.insertEnum(imgEnum , "BaseBtnCswFocusName"       , i++, [CommonConstants.Name] , ["btn_bg_f.png"])
        CommonUtils.insertEnum(imgEnum , "BaseBtnPressName"          , i++, [CommonConstants.Name] , ["btn_bg_p.png"])
        CommonUtils.insertEnum(imgEnum , "BaseBtnListPressName"      , i++, [CommonConstants.Name] , ["btn_bg_lp.png"])
        CommonUtils.insertEnum(imgEnum , "PoiListG"                  , i++, [CommonConstants.Name] , ["poi_list_gray.png"])
        CommonUtils.insertEnum(imgEnum , "PoiListB"                  , i++, [CommonConstants.Name] , ["poi_list_blue.png"])
        CommonUtils.insertEnum(imgEnum , "Zone2Mask"                 , i++, [CommonConstants.Name] , ["zone/zone2_img_mask.png"])
        if (CommonUtils.isEqual(searchRoot.__lang, CommonConstants.En)) {
            CommonUtils.insertEnum(imgEnum , "FastOn"                , i++, [CommonConstants.Name] , ["search/list_tag_charge_fast_eng.png"])
            CommonUtils.insertEnum(imgEnum , "FastOff"               , i++, [CommonConstants.Name] , ["search/list_tag_charge_fast_g.png"])
            CommonUtils.insertEnum(imgEnum , "SlowOn"                , i++, [CommonConstants.Name] , ["search/list_tag_charge_slow_eng.png"])
            CommonUtils.insertEnum(imgEnum , "SlowOff"               , i++, [CommonConstants.Name] , ["search/list_tag_charge_slow_g.png"])
            CommonUtils.insertEnum(imgEnum , "NearTag"               , i++, [CommonConstants.Name] , ["search/list_tag_near_eng.png"])
            CommonUtils.insertEnum(imgEnum , "MinimumTag"            , i++, [CommonConstants.Name] , ["search/list_tag_minimum_eng.png"])
        } else {
            CommonUtils.insertEnum(imgEnum , "FastOn"                , i++, [CommonConstants.Name] , ["search/list_tag_charge_fast.png"])
            CommonUtils.insertEnum(imgEnum , "FastOff"               , i++, [CommonConstants.Name] , ["search/list_tag_charge_fast_g.png"])
            CommonUtils.insertEnum(imgEnum , "SlowOn"                , i++, [CommonConstants.Name] , ["search/list_tag_charge_slow.png"])
            CommonUtils.insertEnum(imgEnum , "SlowOff"               , i++, [CommonConstants.Name] , ["search/list_tag_charge_slow_g.png"])
            CommonUtils.insertEnum(imgEnum , "NearTag"               , i++, [CommonConstants.Name] , ["search/list_tag_near.png"])
            CommonUtils.insertEnum(imgEnum , "MinimumTag"            , i++, [CommonConstants.Name] , ["search/list_tag_minimum.png"])
        }
        CommonUtils.insertEnum(imgEnum , "ElectricStation"           , i++, [CommonConstants.Name] , ["search/list_electric_station_icon"])
        CommonUtils.insertEnum(imgEnum , "Direction"                 , i++, [CommonConstants.Name] , ["direction/list_direction_icon"])
        CommonUtils.insertEnum(imgEnum , "Underbar"                  , i++, [CommonConstants.Name] , ["_"])
        CommonUtils.insertEnum(imgEnum , "Normal"                    , i++, [CommonConstants.Name] , ["n"])
        CommonUtils.insertEnum(imgEnum , "Focus"                     , i++, [CommonConstants.Name] , ["f"])
        CommonUtils.insertEnum(imgEnum , "Dot"                       , i++, [CommonConstants.Name] , ["."])
        CommonUtils.insertEnum(imgEnum , "Png"                       , i++, [CommonConstants.Name] , ["png"])

        CommonUtils.fetchArr(imgEnum , CommonConstants.Name , imgArr)
    }

    function allocateSoftKeyEnum() {
        if (CommonUtils.getObjectLen(softKeyEnum) <= 0) {
            softKeyEnum = CommonUtils.createEnum(softKeyEnum)
        }
        var i = 0
        CommonUtils.insertEnum(softKeyEnum , "Back"                  , i++, [CommonConstants.Name] , [ZoneConstants.Back])
        CommonUtils.insertEnum(softKeyEnum , "Option"                , i++, [CommonConstants.Name] , [ZoneConstants.Option])
        CommonUtils.insertEnum(softKeyEnum , "Navi"                  , i++, [CommonConstants.Name] , [ZoneConstants.Navi])
        CommonUtils.insertEnum(softKeyEnum , "Menu"                  , i++, [CommonConstants.Name] , [ZoneConstants.Menu])
        CommonUtils.insertEnum(softKeyEnum , "Confirm"               , i++, [CommonConstants.Name] , [qsTr("확인")])
        CommonUtils.insertEnum(softKeyEnum , "DistanceOrder"         , i++, [CommonConstants.Name] , [qsTr("거리순")])
        CommonUtils.insertEnum(softKeyEnum , "PriceOrder"            , i++, [CommonConstants.Name] , [qsTr("가격순")])
        CommonUtils.insertEnum(softKeyEnum , "NameOrder"             , i++, [CommonConstants.Name] , [qsTr("이름순")])
        CommonUtils.insertEnum(softKeyEnum , "AlphabeticalOrder"     , i++, [CommonConstants.Name] , [qsTr("정확도순")])

        CommonUtils.fetchArr(softKeyEnum, CommonConstants.Name, softKeyMenuArr)
    }

    function allocateCategoryEnum() {
        if (CommonUtils.getObjectLen(categoryEnum) <= 0) {
            categoryEnum = CommonUtils.createEnum(categoryEnum)
        }
        var i = 0
       CommonUtils.insertEnum(categoryEnum , "GasSt"                , SearchConstants.CategoryGasSt             )
       CommonUtils.insertEnum(categoryEnum , "ChargingSt"           , SearchConstants.CategoryChargingSt        )
       CommonUtils.insertEnum(categoryEnum , "ParkingSp"            , SearchConstants.CategoryParkingSp         )
       CommonUtils.insertEnum(categoryEnum , "Bank"                 , SearchConstants.CategoryBank              )
       CommonUtils.insertEnum(categoryEnum , "Shopping"             , SearchConstants.CategoryShopping          )
       CommonUtils.insertEnum(categoryEnum , "Restaurant"           , SearchConstants.CategoryRestaurant        )
       CommonUtils.insertEnum(categoryEnum , "Tourism"              , SearchConstants.CategoryTourism           )
       CommonUtils.insertEnum(categoryEnum , "Medical"              , SearchConstants.CategoryMedical           )
       CommonUtils.insertEnum(categoryEnum , "PublicInstitution"    , SearchConstants.CategoryPublicInstitution )
       CommonUtils.insertEnum(categoryEnum , "Finance"              , SearchConstants.CategoryFinance           )
       CommonUtils.insertEnum(categoryEnum , "ChargingEvSt"         , SearchConstants.CategoryChargingEvSt      )
    }

    //경로상 검색인 경우의 타입(0:주유소, 1:충전소, 2:주유소&충전소)
    function allocateCategoryOilStEnum() {
        if (CommonUtils.getObjectLen(categoryOilStEnum) <= 0) {
            categoryOilStEnum = CommonUtils.createEnum(categoryOilStEnum)
        }
       CommonUtils.insertEnum(categoryOilStEnum , "GasSt"           , SearchConstants.CategoryGasSt             )
       CommonUtils.insertEnum(categoryOilStEnum , "ChargingSt"      , SearchConstants.CategoryChargingSt        )
       CommonUtils.insertEnum(categoryOilStEnum , "GasChargingSt"   , SearchConstants.CategoryChargingSt+1      )
    }

    function allocateOilTypeEnum(){
        if (CommonUtils.getObjectLen(oilTypeEnum) <= 0) {
            oilTypeEnum = CommonUtils.createEnum(oilTypeEnum)
        }
        var i = 0
        CommonUtils.insertEnum(oilTypeEnum , "Gasoline"           , SettingConstants.OilTypeGasoline        , [CommonConstants.Name] , ["휘발유"])
        CommonUtils.insertEnum(oilTypeEnum , "HighGasoline"       , SettingConstants.OilTypePremiumGasoline , [CommonConstants.Name] , ["고급휘발유"])
        CommonUtils.insertEnum(oilTypeEnum , "Diesel"             , SettingConstants.OilTypeDiesel          , [CommonConstants.Name] , ["경유"])
        CommonUtils.insertEnum(oilTypeEnum , "LPG"                , SettingConstants.OilTypeLPG             , [CommonConstants.Name] , ["LPG"])
        CommonUtils.insertEnum(oilTypeEnum , "None"               , SettingConstants.OilTypeNone            , [CommonConstants.Name] , [CommonConstants.Empty])

        CommonUtils.fetchArr(oilTypeEnum , CommonConstants.Name , oilTypeArr)
    }


    function allocateEvTypeEnum(){
        if (CommonUtils.getObjectLen(evTypeEnum) <= 0) {
            evTypeEnum = CommonUtils.createEnum(evTypeEnum)
        }
        var i = 0
        CommonUtils.insertEnum(evTypeEnum, "All"      , SettingConstants.ChargeTypeAll              , [CommonConstants.Name], ["전체"    ])
        CommonUtils.insertEnum(evTypeEnum, "AC3"      , SettingConstants.ChargeTypeAC3              , [CommonConstants.Name], ["AC3상"   ])
        CommonUtils.insertEnum(evTypeEnum, "DCDemo"   , SettingConstants.ChargeTypeDCDemo           , [CommonConstants.Name], ["DC차데모"])
        CommonUtils.insertEnum(evTypeEnum, "DCCombo"  , SettingConstants.ChargeTypeDCCombo          , [CommonConstants.Name], ["DC콤보"  ])
        CommonUtils.insertEnum(evTypeEnum, "SlowSpeed", SettingConstants.ChargeTypeSlowSpeed        , [CommonConstants.Name], ["완속"    ])

        CommonUtils.fetchArr(evTypeEnum, CommonConstants.Name, evTypeArr)
    }


    function allocateEvBrandEnum() {
        if (CommonUtils.getObjectLen(evBrandEnum) <= 0) {
            evBrandEnum = CommonUtils.createEnum(evBrandEnum)
        }
        var i = 0
        CommonUtils.insertEnum(evBrandEnum , "NONE"      , i++, [CommonConstants.Key, CommonConstants.Name] , ["00", CommonConstants.Empty])
        CommonUtils.insertEnum(evBrandEnum , "ME"        , i++, [CommonConstants.Key, CommonConstants.Name] , ["01", "환경부"])
        CommonUtils.insertEnum(evBrandEnum , "KP"        , i++, [CommonConstants.Key, CommonConstants.Name] , ["02", "한국전력"])
        CommonUtils.insertEnum(evBrandEnum , "HM"        , i++, [CommonConstants.Key, CommonConstants.Name] , ["03", "현대자동차네비게이션"])
        CommonUtils.insertEnum(evBrandEnum , "KM"        , i++, [CommonConstants.Key, CommonConstants.Name] , ["04", "기아자동차네비게이션"])
        CommonUtils.insertEnum(evBrandEnum , "SE"        , i++, [CommonConstants.Key, CommonConstants.Name] , ["05", "서울시"])
        CommonUtils.insertEnum(evBrandEnum , "PI"        , i++, [CommonConstants.Key, CommonConstants.Name] , ["06", "포스코ICT"])
        CommonUtils.insertEnum(evBrandEnum , "HE"        , i++, [CommonConstants.Key, CommonConstants.Name] , ["07", "한국전기차충전서비스"])
        CommonUtils.insertEnum(evBrandEnum , "CT"        , i++, [CommonConstants.Key, CommonConstants.Name] , ["08", "씨티카"])
        CommonUtils.insertEnum(evBrandEnum , "DG"        , i++, [CommonConstants.Key, CommonConstants.Name] , ["09", "대구시"])
        CommonUtils.insertEnum(evBrandEnum , "JD"        , i++, [CommonConstants.Key, CommonConstants.Name] , ["10", "제주특별자치도청"])
        CommonUtils.insertEnum(evBrandEnum , "JE"        , i++, [CommonConstants.Key, CommonConstants.Name] , ["11", "제주전기자동차서비스"])
        CommonUtils.insertEnum(evBrandEnum , "TM"        , i++, [CommonConstants.Key, CommonConstants.Name] , ["00", "티맵"])
        CommonUtils.insertEnum(evBrandEnum , "BG"        , i++, [CommonConstants.Key, CommonConstants.Name] , ["00", "비긴스"])
        CommonUtils.insertEnum(evBrandEnum , "PW"        , i++, [CommonConstants.Key, CommonConstants.Name] , ["00", "파워큐브"])
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
        CommonUtils.insertEnum(evKindEnum , "None"              , SearchConstants.EvKindNone)           //없음
        CommonUtils.insertEnum(evKindEnum , "DCdemo"            , SearchConstants.EvKindDCdemo)         //DC차데모
        CommonUtils.insertEnum(evKindEnum , "ACslow"            , SearchConstants.EvKindACslow)         //승용차AC완속
        CommonUtils.insertEnum(evKindEnum , "DCdemoAC3"         , SearchConstants.EvKindDCdemoAC3)      //DC차데모+AC3상
        CommonUtils.insertEnum(evKindEnum , "DCcombo"           , SearchConstants.EvKindDCcombo)        //DC콤보
        CommonUtils.insertEnum(evKindEnum , "DCdemoCombo"       , SearchConstants.EvKindDCdemoCombo)    //DC차데모+DC콤보
        CommonUtils.insertEnum(evKindEnum , "DCdemoAC3Combo"    , SearchConstants.EvKindDCdemoAC3Combo) //DC차데모+AC3상+DC콤보)
        CommonUtils.insertEnum(evKindEnum , "AC3"               , SearchConstants.EvKindAC3)            //AC급속3상
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

    function allocateOilBrandEnum(){
        if (CommonUtils.getObjectLen(oilBrandEnum) <= 0) {
            oilBrandEnum = CommonUtils.createEnum(oilBrandEnum)
        }
        var i = 0
        CommonUtils.insertEnum(oilBrandEnum , "All"       , SettingConstants.OilBrandAll             , [CommonConstants.Name] , ["(전체)"         ])
        CommonUtils.insertEnum(oilBrandEnum , "SK"        , SettingConstants.OilBrandSK              , [CommonConstants.Name] , ["(SK에너지)"     ])
        CommonUtils.insertEnum(oilBrandEnum , "S_Oil"     , SettingConstants.OilBrandS_Oil           , [CommonConstants.Name] , ["(S-OIL)"        ])
        CommonUtils.insertEnum(oilBrandEnum , "GS"        , SettingConstants.OilBrandGS              , [CommonConstants.Name] , ["(GS칼텍스)"     ])
        CommonUtils.insertEnum(oilBrandEnum , "HyunDae"   , SettingConstants.OilBrandHyunDaeOilBank  , [CommonConstants.Name] , ["(현대오일뱅크)" ])
        CommonUtils.insertEnum(oilBrandEnum , "Etc"       , SettingConstants.OilBrandEtc             , [CommonConstants.Name] , ["(기타)"         ])

        CommonUtils.fetchArr(oilBrandEnum , CommonConstants.Name , oilBrandArr)
    }

    function allocateSearchTypeEnum(){
        if (CommonUtils.getObjectLen(searchTypeEnum) <= 0) {
            searchTypeEnum = CommonUtils.createEnum(searchTypeEnum)
        }
        var i = 0
        CommonUtils.insertEnum(searchTypeEnum , "WithinRadius"  , SettingConstants.SearchTypeWithinRadius  , [CommonConstants.Name] , [qsTr("반경내") ])
        CommonUtils.insertEnum(searchTypeEnum , "AlongRoute"    , SettingConstants.SearchTypeAlongRoute    , [CommonConstants.Name] , [qsTr("경로상") ])

        //한글 메뉴
        CommonUtils.insertEnum(searchTypeEnum , "WithinRadiusKo", SettingConstants.SearchTypeEnd + (i++)   , [CommonConstants.Name] , ["반경내"       ])
        CommonUtils.insertEnum(searchTypeEnum , "AlongRouteKo"  , SettingConstants.SearchTypeEnd + (i++)   , [CommonConstants.Name] , ["경로상"       ])

        CommonUtils.fetchArr(searchTypeEnum , CommonConstants.Name , searchTypeArr)
    }

    //옵션메뉴의 디폴트 값을 얻기 위함
    function allocateSectionEnum(){
        if (CommonUtils.getObjectLen(sectionEnum) <= 0) {
            sectionEnum = CommonUtils.createEnum(sectionEnum)
        }
        var i = 0
        CommonUtils.insertEnum(sectionEnum , "SettingOilType"       , i++, [CommonConstants.Section] , [SettingConstants.MapSubOilType   ])
        CommonUtils.insertEnum(sectionEnum , "SettingChargeType"    , i++, [CommonConstants.Section] , [SettingConstants.MapSubChargeType])
        CommonUtils.insertEnum(sectionEnum , "SettingDirectionGuid" , i++, [CommonConstants.Section] , [SettingConstants.DirectionGuid   ])
        CommonUtils.insertEnum(sectionEnum , "SearchOption"         , i++, [CommonConstants.Section] , [SettingConstants.SearchOption    ])
        CommonUtils.fetchArr(sectionEnum, CommonConstants.Section, sectionArr)
    }


    //옵션메뉴
    function allocateMenuEnum(){
        if (CommonUtils.getObjectLen(menuEnum) <= 0) {
            menuEnum = CommonUtils.createEnum(menuEnum)
        }
        var i = 0
        //주변검색
        CommonUtils.insertEnum(menuEnum , "SearchType"    , i++, [CommonConstants.Name] , [CommonConstants.Empty      ])
        CommonUtils.insertEnum(menuEnum , "OilKind"       , i++, [CommonConstants.Name] , ["유종"                     ])
        CommonUtils.insertEnum(menuEnum , "OilBrand"      , i++, [CommonConstants.Name] , ["브랜드"                   ])
        CommonUtils.insertEnum(menuEnum , "OilArea"       , i++, [CommonConstants.Name] , [CommonConstants.Empty      ])
        CommonUtils.insertEnum(menuEnum , "ChargeType"    , i++, [CommonConstants.Name] , ["충전타입"                 ])
        CommonUtils.insertEnum(menuEnum , "Region"        , i++, [CommonConstants.Name] , ["지역 설정"                ])

        CommonUtils.fetchArr(menuEnum , CommonConstants.Name , menuArr)
    }

    // 옵션메뉴 대응되는 키
    function allocateKeyEnum(){
        if (CommonUtils.getObjectLen(keyEnum) <= 0) {
            keyEnum = CommonUtils.createEnum(keyEnum)
        }
        allocateMenuEnum()
        CommonUtils.insertEnum(keyEnum , "SearchType"   , menuEnum.SearchType   , [CommonConstants.Key] , [SettingConstants.SearchType  ])
        CommonUtils.insertEnum(keyEnum , "OilKind"      , menuEnum.OilKind      , [CommonConstants.Key] , [SettingConstants.OilKind     ])
        CommonUtils.insertEnum(keyEnum , "OilBrand"     , menuEnum.OilBrand     , [CommonConstants.Key] , [SettingConstants.OilBrand    ])
        CommonUtils.insertEnum(keyEnum , "OilArea"      , menuEnum.OilArea      , [CommonConstants.Key] , [SettingConstants.OilArea     ])
        CommonUtils.insertEnum(keyEnum , "ChargeType"   , menuEnum.ChargeType   , [CommonConstants.Key] , [SettingConstants.ChargeType  ])

        CommonUtils.fetchArr(keyEnum, CommonConstants.Key, keyArr)
    }

    function getChargeKindTxt() {
        var text = CommonConstants.Empty
        switch(searchRoot.kind) {
            case categoryEnum.GasSt:        text = textArr[textEnum.GasKo];    break;
            case categoryEnum.ChargingSt:   text = textArr[textEnum.LPGKo];    break;
            case categoryEnum.ChargingEvSt: text = textArr[textEnum.EVKo];     break;
        }
        return text
    }

    function createListModel() {
        if (!CommonUtils.isNull(listModel))
            return listModel
       listModel = Qt.createQmlObject('import QtQuick 2.1; ListModel {}', searchRoot)
       return listModel
    }

    function deleteListModel() {
        if (CommonUtils.isNull(listModel))
            return
        CommonUtils.destroy(listModel,0)
        listModel = null
    }

    function getPoleCateImgIdx(index, enumValue) {
        if (CommonUtils.isEqual(oilSearchType , searchTypeEnum.AlongRoute)){
            switch(getCategory(index)) {
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
            if ((poleCateEnum.GasStationSK <= enumValue && enumValue <= poleCateEnum.GasStationUNKNOWN) ||
                (poleCateEnum.GasStationSK + SearchConstants.CHARGINGBASE <= enumValue &&
                 enumValue <= poleCateEnum.GasStationUNKNOWN + SearchConstants.CHARGINGBASE)  ||
                (poleCateEnum.ChargingEvStNONE <= enumValue && enumValue <= poleCateEnum.ChargingEvStUNKNOWN)){
                return poleCateEnum.properties[enumValue][CommonConstants.Key]
            }
            return poleCateEnum.properties[poleCateEnum.None][CommonConstants.Key]
        }
    }

    function showToast(message)   {
        var data = ({})
        data[CommonConstants.Message] = message
        data[CommonConstants.Position] = CommonConstants.ToastUp
        data[CommonConstants.Duration] = 3 * 1000
        sendEvent(SearchResultViewEvent.ShowToast ,data)
    }

    function unloadMainComponent() {
        loader.sourceComponent = undefined
    }

    function loadMainComponent() {
        if (CommonUtils.isNull(loader.sourceComponent)) {
          createListModel(searchRoot)
          loader.sourceComponent = main
        }
    }

    //검색 요청 갯수
    function getReqMaxCount()  {
       loadMainComponent()
       var requestCount = null
       if (isSearchType(CommonConstants.Around)   ||
           isSearchType(CommonConstants.Industry) ||
           isSearchType(CommonConstants.Address)) {
           requestCount = SearchConstants.RequestMaxCount
       } else {
           if(isSearchType(CommonConstants.Name)) {
               if (searchRoot.online) {
                   requestCount = SearchConstants.RequestMaxCount
               } else {
                   requestCount =  SearchConstants.RequestCount
               }
           } else {
             requestCount =  SearchConstants.RequestCount
           }
       }
       if (CommonUtils.isIndexOf(owner, [CommonConstants.objectName(ViewRegistry.TextSearchView),
                                         CommonConstants.objectName(ViewRegistry.RecentSearchPageView)])) {
           if (listView.maxCount < requestCount) {
                listView.maxCount = requestCount
           }
       } else {
           listView.maxCount = requestCount
       }
       return requestCount
    }

    //첫번째 검색 요청 결과 on/offline여부
    function getResultOnline() {
        if (CommonUtils.isNull(result))
            return online
        for(var i = 0; i< result.length; i++) {
            if (CommonUtils.isEqual(result[i][SearchConstants.ResultType],  SearchConstants.ResultTypeNormal)) {
                return result[i][SearchConstants.ResultOnline]
            }
        }
        return online
    }

    //첫번째 검색 요청 결과, 경로상 여부
    function getResultAlongRoute() {
        if (CommonUtils.isNull(result))
            return false
        for(var i = 0; i< result.length; i++) {
            if (CommonUtils.isEqual(result[i][SearchConstants.ResultType],  SearchConstants.ResultTypeNormal)) {
                return result[i][SearchConstants.ResultAlongRoute]
            }
        }
        return false
    }

    //첫번째 검색 요청 결과, on/offline 변경 여부
    function getResultLineChanged() {
        if (CommonUtils.isNull(result))
            return false
        for(var i = 0; i< result.length; i++) {
            if (CommonUtils.isEqual(result[i][SearchConstants.ResultType],  SearchConstants.ResultTypeNormal)) {
                return result[i][SearchConstants.ResultLineChanged]
            }
        }
        return false
    }


    //창을 닫거나 숨기거나... - 호출한 뷰에 의해~
    function showHideByOwner() {
        if (!isClosed) {
            searchRoot.visible = true
        }

        var  data = ({})
        data[CommonConstants.ObjectName] = CommonConstants.objectName(ViewRegistry.RegionSearchView)
        sendEvent(SearchResultViewEvent.CloseView, data)

        if (showhidebyowner)
            return
        showhidebyowner = true

        if (CommonUtils.isIndexOf(owner, [CommonConstants.objectName(ViewRegistry.TextSearchView)]) ) {
            data =({})
            data[CommonConstants.ObjectName ] = owner
            if (viaSetting)
                sendEvent(SearchResultViewEvent.HideView, data)
            else
                sendEvent(SearchResultViewEvent.CloseView, data)
        } else if (CommonUtils.isIndexOf(owner, [CommonConstants.objectName(ViewRegistry.SearchAroundView),
                                         CommonConstants.objectName(ViewRegistry.IndustrySearchView),
                                         CommonConstants.objectName(ViewRegistry.AddressSearchView)]) ){
            data =({})
            data[CommonConstants.ObjectName ] = owner
            data[CommonConstants.Visible] = false
            sendEvent(SearchResultViewEvent.UpdateView, data)
        }

        if (CommonUtils.isIndexOf(owner, [CommonConstants.objectName(ViewRegistry.TextSearchView),
                     CommonConstants.objectName(ViewRegistry.RecentSearchPageView)])) {
            data =({})
            data[CommonConstants.ObjectName ] = CommonConstants.objectName(ViewRegistry.SearchTabView)
            sendEvent(SearchResultViewEvent.CloseView, data)
        }

        if (CommonUtils.isEqual(owner, CommonConstants.System) &&
            CommonUtils.isEqual(response, SearchConstants.ContactGadgetKey)) {
            if (listModel.count <= 0) {
                data =({})
                data[CommonConstants.Url] = CommonConstants.url(ViewRegistry.SearchTabView)
                data[SearchConstants.Page] = SearchConstants.NoResultPageIndex
                data[SearchConstants.Query] = query
                sendEvent(SearchResultViewEvent.CreateView, data )
                closeView()
            }
        }
    }

    function aroundPriceSort() {
        var data = ({})
        if (CommonUtils.isEqual(searchRoot.kind , categoryEnum.GasSt)       ||
            CommonUtils.isEqual(searchRoot.kind , categoryEnum.ChargingSt)  ) {
            if (!getResultOnline()) {
                if (getResultLineChanged()) {
                    showToast(textArr[textEnum.UseRetrySortByPrice])
                } else {
                    showToast(textArr[textEnum.UseOnlineSortByPrice])
                }
                searchRoot.order = SearchResultViewEvent.DistanceOrder
                data[ZoneConstants.Menu    ] = [softKeyMenuArr[softKeyEnum.DistanceOrder], softKeyMenuArr[softKeyEnum.PriceOrder]]
                data[CommonConstants.Select] = (CommonUtils.isEqual(searchRoot.order, SearchResultViewEvent.DistanceOrder) ? 0 : 1)
                sendEvent(SearchResultViewEvent.ChangeMenu, data)
                return true
            }
        }
        return sortListData()
    }

    function doSort() {
        getReqMaxCount()
        if (!CommonUtils.isEqual(listView.maxCount,
             SearchConstants.RequestMaxCount)) {
            return false
        }
        if (isSearchType(CommonConstants.Around)) {
             switch(searchRoot.order) {
                 case SearchResultViewEvent.PriceOrder:
                     return aroundPriceSort()
                 case SearchResultViewEvent.DistanceOrder:
                 case SearchResultViewEvent.NameOrder:
                    return sortListData()
             }

        } else {
              switch(searchRoot.order) {
                 case SearchResultViewEvent.DistanceOrder:
                     if(!isSearchType(CommonConstants.Industry)) {
                        return sortListData()
                     }
                     break
                 case SearchResultViewEvent.NameOrder:
                     return sortListData()
                 }
        }
        return false
    }

    ////////////////////////////////////////////
    //{전기차충전소 브랜드 표출을 위하여
        function isValidBrand(index) {
            if (index < 0 || listModel.count <= index)
                return false
            var item = listModel.get(index)
            var result = searchRoot.result[item.idx]
            var value = result[SearchConstants.ResultOPIPoleCate]
            if ( value >= poleCateEnum.GasStationSK &&  poleCateEnum.GasStationUNKNOWN >= value)
                return true
            if ( value >= poleCateEnum.ChargingStSK && poleCateEnum.ChargingStUNKNOWN >= value)
                return  true
            if ( value >= poleCateEnum.ChargingEvStNONE && poleCateEnum.ChargingEvStUNKNOWN >= value)
                return true
            return false
        }

        function getCategory(index){
            if (index < 0 || listModel.count <= index)
                return -1
            var item = listModel.get(index)

            //(전기차충전소제외)주변검색-로컬/서버
            var result = searchRoot.result[item.idx]
            if (CommonUtils.isNull(result))
                return -1
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
            if (CommonUtils.isEqual( value, SearchConstants.OilStKindJuyuso))
                return categoryEnum.GasSt
            if (CommonUtils.isEqual(value , SearchConstants.OilStKindChungjeonso))
                return  categoryEnum.ChargingSt
            if (CommonUtils.isEqual(value , SearchConstants.OilStKindEVchungjeonso))
                return  categoryEnum.ChargingEvSt
            return  -1
        }


        function applyBrandIcon(index) {
           if (index < 0 || listModel.count <= index)
               return CommonConstants.Empty
           var item = listModel.get(index)
           var result = searchRoot.result[item.idx]
           if (CommonUtils.isNull(result)) {
               return CommonConstants.Empty
           }
           var value = result[SearchConstants.ResultOPIPoleCate]
           var imgNum = getPoleCateImgIdx(index, value)
           var idx = CommonUtils.number(imgNum)
           if (CommonUtils.isNull(imgNum) || idx <= 0 ) {
               return CommonConstants.Empty
           }
           return  Resource.imageUrl( imgArr[imgEnum.ElectricStation]
                                    + imgArr[imgEnum.Underbar]
                                    + imgNum
                                    + imgArr[imgEnum.Dot]
                                    + imgArr[imgEnum.Png])
        }

        function getChargeTypeShow(index) {
            if (index < 0 || listModel.count <= index)
                return false
            var item = listModel.get(index)
            var ret = false
            var value = options[keyArr[keyEnum.ChargeType]][CommonConstants.Value]
            value = CommonUtils.number(value)
            switch(value) {
               case evTypeEnum.All:
                   ret = true
                   break
               case evTypeEnum.AC3:
                   ret = searchRoot.result[item.idx][SearchConstants.ResultAC3]
                   break
               case evTypeEnum.DCDemo:
                   ret = searchRoot.result[item.idx][SearchConstants.ResultDCDemo]
                   break
               case evTypeEnum.DCCombo:
                   ret = searchRoot.result[item.idx] [SearchConstants.ResultDCCombo]
                   break
               case evTypeEnum.SlowSpeed:
                   ret = searchRoot.result[item.idx] [SearchConstants.ResultSlowCharge]
                   break
                default:
                    ret = false
                    break
            }
            return ret
        }

    //}전기차충전소 브랜드 표출을 위하여
    ////////////////////////////////////////////


    ////////////////////////////////////////////
    //{옵션메뉴 처리
        function saveSetting(section ,settings) {
            var data = ({})
            data[CommonConstants.Section] = section
            data[CommonConstants.Settings] = settings
            sendEvent(SearchResultViewEvent.SaveSetting, data )
        }

        //반경내를 저장
        function saveWithinRadiusFromAlongRoute() {
            withinRadiusFromAlongRoute = true
            saveSearchType(searchTypeEnum.WithinRadius)
        }

        //환경설정의 유종 요청 : __resultSettingOilType함수로 응답
        function requestOptionDefault() {
            var data = ({})
            data[CommonConstants.Section] = sectionArr[sectionEnum.SettingOilType]
            data[CommonConstants.Slot] = "__resultSettingOilType"
            sendEvent(SearchResultViewEvent.RequestSetting, data )
        }

        //환경설정의 전기차 충전타입 요청: __resultSettingChargeType함수로 응답
        function requestEvOptionDefault() {
            var data = ({})
            data[CommonConstants.Section] = sectionArr[sectionEnum.SettingChargeType]
            data[CommonConstants.Slot] = "__resultSettingChargeType"
            sendEvent(SearchResultViewEvent.RequestSetting, data )
        }

        //옵션메뉴의 설정값 요청: __resultSettingOption함수로 응답
        function requestOption() {
            var data = ({})
            data[CommonConstants.Section] = sectionArr[sectionEnum.SearchOption]
            data[CommonConstants.Slot] = "__resultSettingOption"
            sendEvent(SearchResultViewEvent.RequestSetting, data )
        }

        //맵모드의 설정값 요청: __resultSettingDirectionGuid함수로 응답
        function requestDirectionGuid() {
            var data = ({})
            data[CommonConstants.Section] = sectionArr[sectionEnum.SettingDirectionGuid]
            data[CommonConstants.Slot] = "__resultSettingDirectionGuid"
            sendEvent(SearchResultViewEvent.RequestSetting, data )
        }

        //프로파일 업데이트 시
        function __updatedUserProfile() {
            sendEvent(SearchResultViewEvent.RequestAddressType)
        }

        //프로파일 변경 시
        function __changedUserProfile() {
            sendEvent(SearchResultViewEvent.RequestAddressType)
        }

        //(디폴트 값)초기화
        function initOption() {
             var settings = {}
             for (var i = 0; i<keyArr.length; i++){
                 var key = keyArr[i]
                 var value = 0
                 switch(i) {
                     case keyEnum.SearchType:
                         value = isRg ?  searchTypeEnum.AlongRoute : searchTypeEnum.WithinRadius
                         break
                     case keyEnum.OilKind:
                         value = settingOilKindValue
                         break
                     case keyEnum.ChargeType:
                         value = settingChargeTypeValue
                         break
                 }
                 settings[key] = ({})
                 settings[key][CommonConstants.Key  ] = key
                 settings[key][CommonConstants.Value] = CommonUtils.string(value)
             }
             saveSetting(sectionArr[sectionEnum.SearchOption], settings)
        }

        function requestOptionMenu() {
            var data = ({})
            data[CommonConstants.Section] = sectionArr[sectionEnum.SearchOption]
            data[CommonConstants.Slot] = "_createOptionMenu"
            sendEvent(SearchResultViewEvent.RequestSetting, data )
        }

        function saveOptionMenuSetting(settings) {
            var data = ({})
            data[CommonConstants.Section] = sectionArr[sectionEnum.SearchOption]
            data[CommonConstants.Settings] = newSettings
            sendEvent(SearchResultViewEvent.SaveSetting, data )
        }

        //옵션>반경내,경로상
        function searchTypeButtonClicked(type)  {
            withinRadiusFromAlongRoute = false
            manualClickOption   = true
            saveSearchType(type)
        }

        function saveSearchType(type) {
            if (!searchRoot.isRg) {
                if (CommonUtils.isEqual(type, searchTypeEnum.AlongRoute))
                    return
            }
            var key = keyArr[keyEnum.SearchType]
            newSettings[key][CommonConstants.Value] = CommonUtils.string(type)
            saveOptionMenuSetting()
        }


        //옵션>유종,브랜드,충전타입의 뷰가 사라질 때 설정값을 초기화하여 __resultSettingOption을 사용 하기 위하여...
        function __savedSettingFromOption(objectName) {
            objectNameOfSetting = objectName
            switch(objectName) {
            case CommonConstants.objectName(ViewRegistry.SettingOilKindView):
                oilKind = -1
                break
            case CommonConstants.objectName(ViewRegistry.SettingOilBrandView):
                oilBrand = -1
                break
            case CommonConstants.objectName(ViewRegistry.SettingEvChargeTypeView):
                chargeType = -1
                break
            }
        }

        //옵션>유종
        function oilKindButtonClicked()  {
            var data = ({})
            data[CommonConstants.Url     ] = CommonConstants.url(ViewRegistry.SettingOilKindView)
            data[CommonConstants.Caller  ] = caller
            data[CommonConstants.Owner   ] = owner
            //data[CommonConstants.Z       ] = CommonConstants.MAX_INT_VALUE
            sendEvent(SearchResultViewEvent.CreateView, data )
            manualClickOption   = true
        }

        //옵션>브랜드
        function oilBrandButtonClicked()  {
            var data = ({})
            data[CommonConstants.Url     ] = CommonConstants.url(ViewRegistry.SettingOilBrandView)
            data[CommonConstants.Caller  ] = caller
            data[CommonConstants.Owner   ] = owner
            data[SearchConstants.Kind    ] = searchRoot.kind
            //data[CommonConstants.Z       ] = CommonConstants.MAX_INT_VALUE
            sendEvent(SearchResultViewEvent.CreateView, data )
            manualClickOption   = true
        }

        //옵션>충전타입
        function chargeTypeButtonClicked()  {
            var data = ({})
            data[CommonConstants.Url     ] = CommonConstants.url(ViewRegistry.SettingEvChargeTypeView)
            data[CommonConstants.Caller  ] = caller
            data[CommonConstants.Owner   ] = owner
            data[SearchConstants.Kind    ] = searchRoot.kind
            //data[CommonConstants.Z       ] = CommonConstants.MAX_INT_VALUE
            sendEvent(SearchResultViewEvent.CreateView, data )
            manualClickOption   = true
        }

        //옵션메뉴 생성
        function _createOptionMenu(options) {
            searchRoot.options = options
            searchRoot.newSettings = options

            var searchType,oilType,oilBrand,chargeType
            var key, value, index, text
            if (CommonUtils.isNull(options))
                return
            for (var i = menuEnum.SearchType; i<keyArr.length; i++){
                key = keyArr[i]
                if (!options.hasOwnProperty(key))
                    continue
                value = options[key][CommonConstants.Value]
                index = CommonUtils.number(value)
                var typeText = null
                if (CommonUtils.isEqual(i , menuEnum.SearchType)){
                    switch(index){
                    case searchTypeEnum.WithinRadius:  text = searchTypeArr[searchTypeEnum.AlongRouteKo]; break;
                    default: text = searchTypeArr[searchTypeEnum.WithinRadiusKo];  break;
                    }
                    searchType = text
                } else if ( CommonUtils.isEqual(i , menuEnum.OilKind)) {
                    text = menuArr[menuEnum.OilKind]
                    switch(index){
                    case oilTypeEnum.Gasoline       : typeText= oilTypeArr[oilTypeEnum.Gasoline];     break;
                    case oilTypeEnum.HighGasoline   : typeText= oilTypeArr[oilTypeEnum.HighGasoline]; break;
                    case oilTypeEnum.Diesel         : typeText= oilTypeArr[oilTypeEnum.Diesel];       break;
                    case oilTypeEnum.LPG            : typeText= oilTypeArr[oilTypeEnum.LPG];          break;
                    }
                    oilType = text + "(" + typeText + ")"
                } else if (CommonUtils.isEqual(i , menuEnum.OilBrand)) {
                    text = menuArr[menuEnum.OilBrand]
                    switch(index){
                    case oilBrandEnum.All           : text += oilBrandArr[oilBrandEnum.All]    ;      break;
                    case oilBrandEnum.SK            : text += oilBrandArr[oilBrandEnum.SK]     ;      break;
                    case oilBrandEnum.S_Oil         : text += oilBrandArr[oilBrandEnum.S_Oil]  ;      break;
                    case oilBrandEnum.GS            : text += oilBrandArr[oilBrandEnum.GS]     ;      break;
                    case oilBrandEnum.HyunDae       : text += oilBrandArr[oilBrandEnum.HyunDae];      break;
                    case oilBrandEnum.Etc           : text += oilBrandArr[oilBrandEnum.Etc]    ;      break;
                    }
                    oilBrand = text
                } else if (CommonUtils.isEqual(i , menuEnum.ChargeType)) {
                    text = menuArr[menuEnum.ChargeType]
                    switch(index){
                    case evTypeEnum.All            : typeText = evTypeArr[evTypeEnum.All      ];      break;
                    case evTypeEnum.AC3            : typeText = evTypeArr[evTypeEnum.AC3      ];      break;
                    case evTypeEnum.DCDemo         : typeText = evTypeArr[evTypeEnum.DCDemo   ];      break;
                    case evTypeEnum.DCCombo        : typeText = evTypeArr[evTypeEnum.DCCombo  ];      break;
                    case evTypeEnum.SlowSpeed      : typeText = evTypeArr[evTypeEnum.SlowSpeed];      break;
                    }
                    chargeType = text + "(" + typeText + ")"
                }
            }

            optionMenus = []
            if (isRg) {
                optionMenus.push(searchType)
            }

            if (CommonUtils.isEqual(kind , categoryEnum.GasSt)) {
                optionMenus.push(oilType)
                optionMenus.push(oilBrand)
            } else if (CommonUtils.isEqual(kind , categoryEnum.ChargingSt)) {
                optionMenus.push(oilBrand)
            } else if (CommonUtils.isEqual(kind , categoryEnum.ChargingEvSt)) {
                optionMenus.push(chargeType)
            }
            applyOptionMenu()
        }

        //동적으로 언어가 변경될 때 옵션메뉴를 변경하기 위함
        function applyOptionMenu() {
            var data = ({})
            if (isSearchType(CommonConstants.Around)) {
                if (CommonUtils.isEqual(searchRoot.kind , categoryEnum.GasSt)       ||
                    CommonUtils.isEqual(searchRoot.kind , categoryEnum.ChargingSt)  ||
                    CommonUtils.isEqual(searchRoot.kind , categoryEnum.ChargingEvSt)) {
                   data[ZoneConstants.OptionMenu] = optionMenus
                   sendEvent(SearchResultViewEvent.SelectSoftkeyIndexMenuMode, data)
               }
           } else {
             if (!isSearchType(CommonConstants.Address)) {
                 data[ZoneConstants.OptionMenu] = [menuArr[menuEnum.Region]]
                 sendEvent(SearchResultViewEvent.SelectSoftkeyIndexMenuMode, data)
             }
           }
        }


        //환경설정에 설정되어 있는 유종 얻기: requestOptionDefault함수 요청에 대한 응답
        property int settingOilKindValue: SettingConstants.OilTypeGasoline
        function __resultSettingOilType(settings) {
            if (CommonUtils.isNull(settings))
                return

            var value = null
            if (CommonUtils.isEqual(sectionArr[sectionEnum.SettingOilType],
                                    settings[keyArr[keyEnum.OilKind]][CommonConstants.Section]) ) {
                value = CommonUtils.number(settings[keyArr[keyEnum.OilKind]][CommonConstants.Value])
                switch(value) {
                    case SettingConstants.OilTypeGasoline:
                    case SettingConstants.OilTypePremiumGasoline:
                    case SettingConstants.OilTypeDiesel:
                        settingOilKindValue  = value
                        break
                    default:
                        settingOilKindValue  =  SettingConstants.OilTypeGasoline
                        break
                }
                settings[keyArr[keyEnum.OilKind]][CommonConstants.Section] = sectionArr[sectionEnum.SearchOption]
                settings[keyArr[keyEnum.OilKind]][CommonConstants.Value] = CommonUtils.string(settingOilKindValue)
                saveSetting(sectionArr[sectionEnum.SearchOption],settings)

                if (CommonUtils.isEqual(searchRoot.kind, SearchConstants.CategoryPatrol)) {
                    switch (value) {
                    case SettingConstants.OilTypeGasoline:
                    case SettingConstants.OilTypePremiumGasoline:
                    case SettingConstants.OilTypeDiesel:
                        searchRoot.kind = SearchConstants.CategoryGasSt
                        break
                    case SettingConstants.OilTypeLPG:
                        searchRoot.kind = SearchConstants.CategoryChargingSt
                        break
                    case SettingConstants.OilTypeElec:
                        searchRoot.kind = SearchConstants.CategoryChargingEvSt
                        break
                    }
                }
             }
        }

        //환경 설정에 설정된 전기차 충전 타입 얻기:requestEvOptionDefault함수 요청에 대한 응답
        function __resultSettingChargeType(settings) {
            if (CommonUtils.isNull(settings))
                return

            var value = null
            if (CommonUtils.isEqual(sectionArr[sectionEnum.SettingChargeType],
                                    settings[keyArr[keyEnum.ChargeType]][CommonConstants.Section]) ) {
                value = CommonUtils.number(settings[keyArr[keyEnum.ChargeType]][CommonConstants.Value])
                switch(value) {
                    case SettingConstants.ChargeTypeAll:
                    case SettingConstants.ChargeTypeAC3:
                    case SettingConstants.ChargeTypeDCDemo:
                    case SettingConstants.ChargeTypeDCCombo:
                    case SettingConstants.ChargeTypeSlowSpeed:
                        settingChargeTypeValue  = value
                        break
                    default:
                        settingChargeTypeValue  =  SettingConstants.ChargeTypeAC3
                        break
                }
                settings[keyArr[keyEnum.ChargeType]][CommonConstants.Section] = sectionArr[sectionEnum.SearchOption]
                settings[keyArr[keyEnum.ChargeType]][CommonConstants.Value] = CommonUtils.string(settingChargeTypeValue)
                saveSetting(sectionArr[sectionEnum.SearchOption],settings)
             }
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

        //requestOption함수 호출 시에 settings(CommonConstants.Settings)변수에 응답
        function __resultSettingOption(settings) {
            if (CommonUtils.isNull(settings))
                return
            var oilSearchType   = CommonUtils.number(settings[keyArr[keyEnum.SearchType]][CommonConstants.Value])
            var oilKind         = CommonUtils.number(settings[keyArr[keyEnum.OilKind   ]][CommonConstants.Value])
            var oilBrand        = CommonUtils.number(settings[keyArr[keyEnum.OilBrand  ]][CommonConstants.Value])
            var oilArea         = CommonUtils.number(settings[keyArr[keyEnum.OilArea   ]][CommonConstants.Value])
            var chargeType      = CommonUtils.number(settings[keyArr[keyEnum.ChargeType]][CommonConstants.Value])

            //충전소인 경우 처리
            oilKind             = CommonUtils.isEqual(searchRoot.kind , categoryEnum.ChargingSt) ? oilTypeEnum.LPG : oilKind
            changedSearchType = !CommonUtils.isEqual(searchRoot.oilSearchType , oilSearchType)
            changedOilKind    = !CommonUtils.isEqual(searchRoot.oilKind  , oilKind)
            changedOilBrand   = !CommonUtils.isEqual(searchRoot.oilBrand , oilBrand)
            changedOilArea    = !CommonUtils.isEqual(searchRoot.oilArea , oilArea)
            changedChargeType = !CommonUtils.isEqual(searchRoot.chargeType, chargeType)
            if (!(changedSearchType ||
                  changedOilKind    ||
                  changedOilBrand   ||
                  changedOilArea    ||
                  changedChargeType)) {
                return
            }

            searchRoot.oilSearchType = oilSearchType
            searchRoot.oilKind = oilKind
            searchRoot.oilBrand = oilBrand
            searchRoot.oilArea = oilArea
            searchRoot.chargeType = chargeType

            //반경/경로,전기차 충전기 타입 변경시
            if ((changedSearchType || changedChargeType) &&
                (autoClickOption || manualClickOption)   ) {
                if (CommonUtils.isEqual(oilSearchType , searchTypeEnum.AlongRoute)) {
                    switch(searchRoot.kind) {
                    case categoryEnum.GasSt:
                    case categoryEnum.ChargingSt:
                        requestAlongRoute()
                        break
                    case categoryEnum.ChargingEvSt:
                        requestAlongRouteEv()
                        break
                    }
                } else {
                    requestSearch(searchRoot.query, searchRoot.order, defaultStartIndex, getReqMaxCount())
                }
            }
            //유종,브랜드 변경시
            else {
                searchRoot.result = getResultByOilBrand(oilBrand, defaultStartIndex)
                if (!doSort()) {
                    requestSearch(searchRoot.query, searchRoot.order, defaultStartIndex, getReqMaxCount())
                }
            }
        }

        function getResultByOilBrand(value, startIndex) {
            var oilBrand = 0
            switch(CommonUtils.number(value)){
                case oilBrandEnum.All:
                    oilBrand = poleCateEnum.None
                    break
                case oilBrandEnum.SK:
                    oilBrand = CommonUtils.isEqual(searchRoot.kind , categoryEnum.GasSt) ?
                                poleCateEnum.GasStationSK : poleCateEnum.ChargingStSK
                    break
                case oilBrandEnum.S_Oil:
                    oilBrand = CommonUtils.isEqual(searchRoot.kind , categoryEnum.GasSt) ?
                                poleCateEnum.GasStationSOIL : poleCateEnum.ChargingStSOIL
                    break
                case oilBrandEnum.GS:
                    oilBrand = CommonUtils.isEqual(searchRoot.kind , categoryEnum.GasSt) ?
                                poleCateEnum.GasStationGS :  poleCateEnum.ChargingStGS
                    break
                case oilBrandEnum.HyunDae:
                    oilBrand = CommonUtils.isEqual(searchRoot.kind , categoryEnum.GasSt) ?
                                poleCateEnum.GasStationOILBANK : poleCateEnum.ChargingStOILBANK
                    break
                case oilBrandEnum.Etc:
                    oilBrand = CommonUtils.isEqual(searchRoot.kind , categoryEnum.GasSt) ?
                                poleCateEnum.GasStationUNKNOWN : poleCateEnum.ChargingStUNKNOWN
                    break
            }

            var i = 0
            var j = 0
            var result = []
            var resultAround = searchRoot.resultAround
            for(i = 0; i<resultAround.length; i++) {
                if (CommonUtils.isEqual(searchRoot.kind , categoryEnum.ChargingSt)) {
                    //온라인 검색일때 충전소 코드가 주유소 코드로 들어와 추가함
                    if (resultAround[i][SearchConstants.ResultOPIPoleCate] < poleCateEnum.GasStationSK)
                        resultAround[i][SearchConstants.ResultOPIPoleCate]  += SearchConstants.CHARGINGBASE
                    if (resultAround[i][SearchConstants.ResultOPIPoleCate] < poleCateEnum.ChargingStSK)
                        resultAround[i][SearchConstants.ResultOPIPoleCate]  += SearchConstants.CATEGORYGAP
                }

                //상세에서 브랜드 아이콘 표출을 위하여
                if (CommonUtils.isEmpty(resultAround[i][SearchConstants.ResultBrand])) {
                    resultAround[i][SearchConstants.ResultBrand]  = resultAround[i][SearchConstants.ResultOPIPoleCate]
                 }

                //TVAS브랜드 코드값 -> NDDS브랜드 코드값으로 변경
                var poleCateOnPath = null
                if (CommonUtils.isEqual(oilSearchType , searchTypeEnum.AlongRoute)) {
                    if (CommonUtils.isEqual(searchRoot.kind , categoryEnum.GasSt)) {
                        poleCateOnPath = resultAround[i][SearchConstants.ResultOPIPoleCate] -  SearchConstants.OILBRANDBASE
                        switch(poleCateOnPath) {
                        case poleCateOnPathEnum.GasStationSK:
                            resultAround[i][SearchConstants.ResultOPIPoleCate] = poleCateEnum.GasStationSK
                            break
                        case poleCateOnPathEnum.GasStationGS:
                            resultAround[i][SearchConstants.ResultOPIPoleCate] = poleCateEnum.GasStationGS
                            break
                        case poleCateOnPathEnum.GasStationSOIL:
                            resultAround[i][SearchConstants.ResultOPIPoleCate] = poleCateEnum.GasStationSOIL
                            break
                        case poleCateOnPathEnum.GasStationOILBANK:
                            resultAround[i][SearchConstants.ResultOPIPoleCate] = poleCateEnum.GasStationOILBANK
                            break
                        }
                    } else if (CommonUtils.isEqual(searchRoot.kind , categoryEnum.ChargingSt)) {
                        poleCateOnPath = resultAround[i][SearchConstants.ResultOPIPoleCate] -  SearchConstants.CHARGINGBASE
                        switch(poleCateOnPath) {
                            case poleCateOnPathEnum.ChargingStSK:
                                resultAround[i][SearchConstants.ResultOPIPoleCate] = poleCateEnum.ChargingStSK
                                break
                            case poleCateOnPathEnum.ChargingStGS:
                                resultAround[i][SearchConstants.ResultOPIPoleCate] = poleCateEnum.ChargingStGS
                                break
                            case poleCateOnPathEnum.ChargingStSOIL:
                                resultAround[i][SearchConstants.ResultOPIPoleCate] = poleCateEnum.ChargingStSOIL
                                break
                            case poleCateOnPathEnum.ChargingStOILBANK:
                                resultAround[i][SearchConstants.ResultOPIPoleCate] = poleCateEnum.ChargingStOILBANK
                                break
                        }
                    }
                }

                //[주의]무풀/기타에 해당하는 코드값이 잘못들어 오는 경우
                var unknowBrand = CommonUtils.isEqual(searchRoot.kind , categoryEnum.GasSt) ? poleCateEnum.GasStationUNKNOWN : poleCateEnum.ChargingStUNKNOWN
                if (CommonUtils.isEqual(oilBrand,  unknowBrand)) {
                    var baseBrand = CommonUtils.isEqual(searchRoot.kind , categoryEnum.GasSt) ? poleCateEnum.GasStationSK : poleCateEnum.ChargingStSK
                    var lastBrand = CommonUtils.isEqual(searchRoot.kind , categoryEnum.GasSt) ? poleCateEnum.GasStationUNKNOWN : poleCateEnum.ChargingStUNKNOWN
                    if (resultAround[i][SearchConstants.ResultOPIPoleCate] < baseBrand ||
                        resultAround[i][SearchConstants.ResultOPIPoleCate] > lastBrand) {
                        resultAround[i][SearchConstants.ResultOPIPoleCate] = unknowBrand
                    }
                }

                if (CommonUtils.isEqual(oilBrand , poleCateEnum.None) ||
                    CommonUtils.isEqual(resultAround[i][SearchConstants.ResultOPIPoleCate] , oilBrand)){
                    result[j++] = resultAround[i]
                }
            }
            return result
        }
    //}옵션메뉴 처리
    ////////////////////////////////////////////

    ///////////////////////////////////////////
    //{마커 관련
        function requestPageMarker(startIndex){
            if (result.length <= 0)
                return

            for(var i=startIndex; i <listModel.count; i++) {
                requestAddMarker(i, -1)
             }
        }

        function requestAddMarker(index, focusIndex) {
            if (isClosed) {
                return
            }
            var item = null
            var text = null
            var data = null

            if (index < 0 || listModel.count <= index)
                return
            item  = listModel.get(index)
            if (CommonUtils.isEqual(searchRoot.kind, categoryEnum.ChargingEvSt)) {
                if (item.height <= 0  ||
                    item.request <= 0 ||
                    item.response <= 0) {
                    return
                 }
            }

            //주변검색,업종검색일 경우 마커에 번호 표출하지 않도록 수정
            if (isSearchType(CommonConstants.Around)   ||
                isSearchType(CommonConstants.Industry) ) {
                text = CommonConstants.Empty
            } else {
                text = index + 1
            }

            if (focusIndex >= 0) {
                listView.markerFocusedIndex = focusIndex
            }

            data = ({})
            data[CommonConstants.X            ] = item.x
            data[CommonConstants.Y            ] = item.y
            data[CommonConstants.Index        ] = index + markerBaseIndex
            data[CommonConstants.FocusIndex   ] = (focusIndex >= 0) ? focusIndex + markerBaseIndex : focusIndex
            data[CommonConstants.Text         ] = CommonUtils.string(text)
            sendEvent(SearchResultViewEvent.RequestAddMarker, data )
        }

        function requestRemoveMarker(index) {
            var item = null
            if (index < 0 || listModel.count <= index)
               return

            item  = listModel.get(index)
            var data = ({})
            data[CommonConstants.X            ] = item.x
            data[CommonConstants.Y            ] = item.y
            data[CommonConstants.Index        ] = index + markerBaseIndex
            sendEvent(SearchResultViewEvent.RequestRemoveMarker, data)
        }

        function requestRemoveAllMarker() {
            if (CommonUtils.isNull(listModel))
                return
            listView.markerFocusedIndex = -1
            for(var i= 0; i <listModel.count; i++) {
                requestRemoveMarker(i)
            }
        }

        function resetSelectList(oldIdx, index) {
            csw.forceLostFocus()
            resetCurrentList(index)
            setSelectList(oldIdx, index)
        }

        function resetCurrentList(index) {
            listView.currentIndex = -1
            setCurrentList(index)
        }

        function setCurrentList(index) {
            listView.currentIndex = index
        }

        function setSelectList(oldIdx, index) {
                if (!CommonUtils.isEqual(listView.selectedIndex , index)) {
                    var applyIndex = true
                    if (CommonUtils.isEqual(searchRoot.kind, categoryEnum.ChargingEvSt)) {
                        if (!(index < 0 || listModel.count <= index)) {
                            var item  = listModel.get(index)
                            if (!searchRoot.result[item.idx][SearchConstants.ResultOnline]) {
                                if (!(item.request && item.response && item.height)) {
                                    applyIndex = false
                                }
                            }
                        }
                    }
                    if (applyIndex) {
                        listView.selectedIndex = index
                    }
                }
                //경로상 좌표가 잘못되어 상세검색을 통해 x,y좌표 보정을 위하여...
                if (CommonUtils.isEqual(oilSearchType , searchTypeEnum.AlongRoute)) {
                    if (index >= 0 && listModel.count > index) {
                         requestDetailInfo(index)
                    }
                }

                requestAtThePOI(index)
                if (!CommonUtils.isEqual(oldIdx, index)||
                    !CommonUtils.isEqual(index, listView.markerFocusedIndex) ) {
                    requestRemoveMarker(oldIdx)
                    requestAddMarker(oldIdx, -1)
                    requestRemoveMarker(index)
                    requestAddMarker(index, index)
                }
        }
     //}마커 관련
     ///////////////////////////////////////////

        function close() {
            destruct()
            searchRoot.visible = false

            //1.호출뷰에 focus 요청
            if (!CommonUtils.isEmpty(owner)) {
                var data = ({})
                data[CommonConstants.ReFocus   ] = true
                data[CommonConstants.ObjectName] = owner
                sendEvent(SearchResultViewEvent.UpdateView, data)
            }
            //2.변수초기화 (caller,owner,relay... 포함)
            allocateVariant()
            isClosed = true
        }


        function closeView() {
            var data = ({})
            data[CommonConstants.ObjectName] = CommonConstants.objectName(ViewRegistry.SearchResultView)
            sendEvent(SearchResultViewEvent.CloseView, data)
        }


      function closeAllView() {
          var data = ({})
          if (!CommonUtils.isNull(owner)) {
              data[CommonConstants.ObjectName] = owner
              sendEvent(SearchResultViewEvent.CloseView, data)
          }
          if (!CommonUtils.isNull(relay)) {
              data[CommonConstants.ObjectName] = relay
              sendEvent(SearchResultViewEvent.CloseView, data)
          }
          if (!CommonUtils.isNull(caller)) {
              data[CommonConstants.ObjectName] = caller
              sendEvent(SearchResultViewEvent.CloseView, data)
          }
          closeView()
      }

      function showProgressView() {
          reqSearch = true
          sendEvent(SearchResultViewEvent.ShowProgressIndicator)
          isShowProgressIndicator = true
      }

      function hideProgressView() {
          reqSearch = false
          timerQueue = CommonUtils.destroy(timerQueue)
          sendEvent(SearchResultViewEvent.HideProgressIndicator)
          isShowProgressIndicator = false
      }

      function stopProgressViewByCond(oilSearchType, startIndex, totalCount, result) {
          //[주의]경로상은 requestDetailInfo를 요청하여 __onResultDetailInfo수신시에 hideProgressView()함
          if (CommonUtils.isEqual(searchRoot.kind, categoryEnum.ChargingEvSt)) {
              if (totalCount > 0) {
                  if (!(resultEvDataSync &&
                      (!CommonUtils.isNull(result) &&
                       result[SearchConstants.ResultOnline]) &&
                       !result[SearchConstants.ResultAlongRoute] )) {
                      return
                  }
              }
          }
          if (startIndex <= 0 && totalCount <= 0) {
             if (CommonUtils.isEqual(searchType ,  CommonConstants.Around) &&
                 (CommonUtils.isEqual(searchRoot.kind , categoryEnum.GasSt)       ||
                  CommonUtils.isEqual(searchRoot.kind , categoryEnum.ChargingSt)  ||
                  CommonUtils.isEqual(searchRoot.kind , categoryEnum.ChargingEvSt))) {
                 if (!CommonUtils.isEqual(oilSearchType, searchRoot.oilSearchType)) {
                     return
                 }
             }
          }
          hideProgressView()
      }

      function moveDestination(index) {
        if(CommonUtils.isIndexOf(caller, [CommonConstants.objectName(ViewRegistry.RouteSettingView),
                     CommonConstants.objectName(ViewRegistry.AddressSearchView)])){
            requestAddVia(index)

            var  data = ({})
            data[CommonConstants.ObjectName ] = owner
            sendEvent(SearchResultViewEvent.CloseView, data)
            data[CommonConstants.ObjectName ] = caller
            sendEvent(SearchResultViewEvent.ShowView, data )
            closeView()
            return
        }
        queryInDriveMode(index)
      }


      function setRequestDetailInfo(index) {
          verifyReqDetail[CommonUtils.string(index)] = CommonUtils.getTime()
      }

      function setResponseDetailInfo(index) {
          verifyResDetail[CommonUtils.string(index)] = CommonUtils.getTime()
      }


      function setRequestEvDetailInfo(index) {
          verifyReqEvDetail[CommonUtils.string(index)] = CommonUtils.getTime()
      }

      function setResponseEvDetailInfo(index) {
          verifyResEvDetail[CommonUtils.string(index)] = CommonUtils.getTime()
      }

      //경로상 검색: 주유소,충전소 -> 검색결과:__onResultAlongRoute(...)
      function requestAlongRoute() {
          getReqMaxCount()
          if (searchRoot.online) {
              showProgressView()
              sendEvent(SearchResultViewEvent.RequestGetGasStationInfo, [] )
          } else {
              var result = ([])
              processResult("경로상검색>결과없음", result, result.length, defaultStartIndex)
          }
      }

      //경로상 검색: 전기차-> 검색결과: __onResultAlongRouteEStation(...)
      function requestAlongRouteEv() {
          getReqMaxCount()
          if (searchRoot.online) {
              showProgressView()
              var data = ({})
              data[SettingConstants.ChargeType] = searchRoot.chargeType
              sendEvent(SearchResultViewEvent.RequestGetEStationInfo, data )
          } else {
              var result = ([])
              processResult("경로상검색>결과없음", result, result.length, defaultStartIndex)
          }
      }

      //상세 검색->검색결과:__onResultDetailInfo(...)
      function requestDetailInfo(index) {
          if (index < 0 || listModel.count <= index)
              return
          var item = listModel.get(index)
          if (item.responsedetail)
              return
          if (item.requestdetail >= 2)
              return
          listModel.setProperty(index, CommonConstants.Request + CommonConstants.Detail, ++item.requestdetail)

          showProgressView()

          poiDetailSearch = true

          setRequestDetailInfo(index)

          var params = ({})
          params[SearchConstants.PoiID] = item.poiID
          params[SearchConstants.NavSeq] = item.navSeq
          params[SearchConstants.StartIndex] = index
          params[SearchConstants.Count     ] = 1
          var data =({})
          data[SearchConstants.Params ] = params
          sendEvent(SearchResultViewEvent.RequestDetailInfo, data )

          //[주의] 검색 요청이 블럭되는 경우 처리를 위하여 ...
          queueChanged(0)
      }

      //(전기차)상세 검색->검색결과:__onResultEvDetailInfo(...)
      function requestEvDetailInfo(index) {
          if (index < 0 || listModel.count <= index)
            return
          var item = listModel.get(index)
          if (resultEvDataSync &&
              searchRoot.result[item.idx][SearchConstants.ResultOnline] &&
              !searchRoot.result[item.idx][SearchConstants.ResultAlongRoute])
              return

          var req = ++item.request
          var res = null
          reqSearch = true
          poiDetailSearch = true
          setRequestEvDetailInfo(index)
          listModel.setProperty(index, SearchConstants.Request, req)

          //[*주의*]경로상 전기차 상세 정보가 넘어오고 있지 않아서...
          if (CommonUtils.isEqual(oilSearchType , searchTypeEnum.AlongRoute)) {
                if (!searchRoot.online) {
                  res = ++item.response
                  listModel.setProperty(index, SearchConstants.Response, res)
                  setResponseEvDetailInfo(index)
                  item = searchRoot.result[item.idx][SearchConstants.ResultObject]
                  if (!CommonUtils.isObject(item)) {
                      return
                  }
                  var styleLoader = item.styleLoader
                  if (CommonUtils.isNull(styleLoader) || CommonUtils.isNull(styleLoader.children)) {
                      return
                  }
                  if (styleLoader.children.length <= 0) {
                      return
                  }
                  if (CommonUtils.isNull(styleLoader.children[0].refreshCharger)) {
                      return
                  }
                  styleLoader.children[0].refreshCharger()
                  return
                }
          }

          showProgressView()
          var params = ({})
          params[SearchConstants.PoiID] = item.poiID
          params[SearchConstants.NavSeq] = item.navSeq
          params[SearchConstants.StartIndex] = index
          params[SearchConstants.Count]      =  1
          //[주의]검색된 결과에 따라  (온/오프라인)상태로 검색을 요청하도록 ...
          params[SearchConstants.SearchType] = searchRoot.result[item.idx][SearchConstants.ResultOnline]
          var data =({})
          data[SearchConstants.Params ] = params
          sendEvent(SearchResultViewEvent.RequestEvDetailInfo, data )
          //[주의] 검색 요청이 블럭되는 경우 처리를 위하여 ...
          queueChanged(0)
      }

      //(통합,업종,주변,주소...)검색 -> 검색결과: __onResult(...)
      function requestSearch(search, order, requestPageIndex, maxCount) {
          if (requestPageIndex <= 0) {
            switchedLine = false
          }
          if (CommonUtils.isEmpty(searchRoot.searchType)) {
              hideProgressView()
              return
          }
          loadMainComponent()

          showProgressView()

          reqSearch = true
          poiDetailSearch = false
          searchRoot.order = order

          var data = ({})
          //공통 부분
          data[SearchConstants.Query       ] = CommonUtils.isEqual(searchType , CommonConstants.Address) ? keyword : search
          data[SearchConstants.TextParam   ] = (CommonUtils.isEqual(searchType , CommonConstants.Industry) ||
                                                CommonUtils.isEqual(searchType ,  CommonConstants.Around)) ?
                                                searchRoot.textParam : data[SearchConstants.Query]
          data[SearchConstants.Order       ] = order
          data[SearchConstants.SearchType  ] = (CommonUtils.isEqual(searchType ,  CommonConstants.Around) &&
                                                CommonUtils.isEqual(maxCount,  lowestCount)) ?  SearchConstants.Lowest:  searchType
          data[SearchConstants.AddressType ] = addressType
          data[SearchConstants.Response    ] = CommonUtils.isEmpty(response) ? CommonConstants.Empty : response


          //주소검색에 해당하는 값
          data[CommonConstants.Text        ] = text
          data[CommonConstants.Keyword     ] = keyword
          data[SearchConstants.IsSan       ] = isSan
          data[SearchConstants.IsApt       ] = isApt
          data[SearchConstants.IsRoad      ] = isRoad


          //부가 정보
          var params = ({})
          if (!CommonUtils.isNull(searchRoot.params)) {
              //[주의] 주소검색 및 지역설정 검색에서 사용
              params = searchRoot.params
          }
          params[SearchConstants.Kind      ] = searchRoot.kind
          params[SearchConstants.Distance  ] = distance
          params[SearchConstants.StartIndex] = requestPageIndex * maxCount
          params[SearchConstants.Count     ] = maxCount
          params[SearchConstants.CacheCount] = requestCacheMaxCount
          params[SearchConstants.Sync      ] = resultEvDataSync
          var addrCode = searchRoot.addrCode
          if (useCurrAddrCode &&
              (addrCode <= 0 || addrCode >= CommonConstants.MAX_INT_VALUE)) {
              if (CommonUtils.isEqual(searchType , CommonConstants.Industry)) {
                  if (addrCode <= 0) {
                      addrCode = searchRoot.currAddrCode
                  }
              }
              if (addrCode >= CommonConstants.MAX_INT_VALUE) {
                  addrCode = 0
              }
          }
          params[SearchConstants.AddrCode  ] = addrCode

          //[주의]첫번째 페이지로 검색된 (온/오프라인)상태로 검색을 요청하도록 ...
          if (requestPageIndex > 0) {
             if (!CommonUtils.isNull(searchRoot.result) && searchRoot.result.length) {
                params[SearchConstants.SearchType] =  getResultOnline()
             }
          }

          //2018.02.20-최저가 - 유종
          var kind  = SearchConstants.OilStKindJuyuso
          if (CommonUtils.isEqual(searchRoot.kind , categoryEnum.GasSt))
              kind = SearchConstants.OilStKindJuyuso
          else if (CommonUtils.isEqual(searchRoot.kind , categoryEnum.ChargingSt))
              kind = SearchConstants.OilStKindChungjeonso
          params[SearchConstants.DataKind  ] = kind
          params[SearchConstants.FuelKind  ] = searchRoot.oilKind

          //전기차 충전타입
          params[SettingConstants.ChargeType] = searchRoot.chargeType

          //부가 정보(대입)
          data[SearchConstants.Params      ] = params
          sendEvent(SearchResultViewEvent.RequestSearch, data )


          //[주의] 검색 요청이 블럭되는 경우 처리를 위하여 ...
          queueChanged(0)
       }

      //POI로~
      function requestAtThePOI(index) {
          index = (index >= 0) ? index : 0
          if (index < 0 || listModel.count <= index)
              return
          var item  = listModel.get(index)
          var data = ({})
          data[SearchConstants.MapView      ] = mapView
          data[CommonConstants.X            ] = item.x
          data[CommonConstants.Y            ] = item.y
          sendEvent(SearchResultViewEvent.RequestAtThePOI, data )
      }

      //경유지로~
      function requestAddVia(index) {
          if (index < 0 || listModel.count <= index)
              return
          var item  = listModel.get(index)
          var data = ({})
          data[CommonConstants.X            ] = item.x
          data[CommonConstants.Y            ] = item.y
          data[CommonConstants.Name         ] = item.name
          data[CommonConstants.Address      ] = item.address
          data[SearchConstants.PoiID        ] = item.poiID
          data[SearchConstants.NavSeq       ] = item.navSeq
          data[SearchConstants.OpiPoleCate  ] = item.poleCate
          data[SearchConstants.RpFlag       ] = item.rpFlag
          sendEvent(SearchResultViewEvent.RequestAddVia, data )
      }

      //목적지로~
      function queryInDriveMode(index) {
          if (index < 0 || listModel.count <= index)
              return
          var item  = listModel.get(index)
          var data = ({})
          data[CommonConstants.X            ] = item.x
          data[CommonConstants.Y            ] = item.y
          data[CommonConstants.Name         ] = item.name
          data[CommonConstants.Address      ] = item.address
          data[SearchConstants.Favorite     ] = favorite
          data[SearchConstants.IsHome       ] = isHome
          data[SearchConstants.IsOffice     ] = isOffice
          data[SearchConstants.ViaSetting   ] = viaSetting
          data[SearchConstants.PoiID        ] = item.poiID
          data[SearchConstants.NavSeq       ] = item.navSeq
          data[SearchConstants.OpiPoleCate  ] = item.poleCate
          data[SearchConstants.RpFlag       ] = item.rpFlag
          if (favorite || viaSetting)
              data[SearchConstants.Dest     ] = item.name

          sendEvent(SearchResultViewEvent.QueryInDriveMode, data )
      }

      //페이지 수
      function calcTotalPage(totalCount) {
          listView.totalCount = totalCount
          var maxCount = null
          if (CommonUtils.isEqual(listView.maxCount , SearchConstants.RequestMaxCount)) {
              maxCount = SearchConstants.RequestCount
          } else {
              maxCount = listView.maxCount
          }
          listView.totalPage = parseInt(totalCount / maxCount)
          listView.totalPage += (totalCount % maxCount) ? 1 : 0
          return listView.totalPage
      }

      function viewSizeChanged(height){
          listViewHeight = height

          var data = ({})
          data[CommonConstants.B] = height
          sendEvent(SearchResultViewEvent.ViewSizeChanged, data)
      }

      function modeChange() {
          mapView.viewMode = Renderer.NorthBound
          mapView.moveMode = Renderer.Normal
      }

      //상세뷰 표출
      function showSearchResultDetailView(index){
            if (index < 0 || listModel.count <= index)
              return

            reqSearch = false

            requestRemoveAllMarker()

            var item  = listModel.get(index)

            var data = ({})
            data[CommonConstants.ObjectName ] = CommonConstants.objectName(ViewRegistry.SearchResultDetailView)
            sendEvent(SearchResultViewEvent.CloseView, data)

            data = ({})
            data[CommonConstants.Url        ] = CommonConstants.url(ViewRegistry.SearchResultDetailView)
            data[CommonConstants.Caller     ] = caller
            data[CommonConstants.Relay      ] = owner
            data[CommonConstants.Owner      ] = CommonConstants.objectName(ViewRegistry.SearchResultView)
            data[CommonConstants.Index      ] = index
            data[SearchConstants.Kind       ] = kind

            //[주의] 원래의 값으로 변경함 - 충전소의 브랜드코드가 로컬과 서버가 달라 변경되었으며, 전기충전소를 임의로 변경하였음
            searchRoot.result[item.idx] [SearchConstants.ResultOPIPoleCate] = item.poleCate

            data[SearchConstants.Result     ] = searchRoot.result[item.idx]
            data[SearchConstants.Favorite   ] = favorite
            data[SearchConstants.IsHome     ] = isHome
            data[SearchConstants.IsOffice   ] = isOffice
            data[SearchConstants.DestType   ] = isHome ? SearchConstants.DestTypeHome :
                                                      (isOffice ? SearchConstants.DestTypeOffice : SearchConstants.DestTypeNone)
            data[SearchConstants.OilSearchType] = oilSearchType
            data[SearchConstants.SearchType ] = searchType
            data[CommonConstants.Visible    ] = SearchConstants.AlwaysShow

            //data[CommonConstants.Z          ] = CommonConstants.MAX_INT_VALUE
            data[CommonConstants.ReFocus    ] = true
            data[CommonConstants.ObjectName ] = CommonConstants.objectName(ViewRegistry.SearchResultDetailView)
            sendEvent(SearchResultViewEvent.UpdateView, data)

            sendEvent(SearchResultViewEvent.CreateView, data)
      }

      //현재 유종및 유가를 얻음
      function getOilKindWidthPrice(result) {
            var ret = []
            var selectOilKind  = oilKind
            var oilPrice = 0
            var gasolinePrice     = result[SearchConstants.ResultOPIGasolinePrice]
            var highGasolinePrice = result[SearchConstants.ResultOPIHiGasolinePrice]
            var dieselPrice       = result[SearchConstants.ResultOPIDieselPrice]
            var lpgPrice          = result[SearchConstants.ResultOPILPGPrice]
            switch (oilKind) {
              case oilTypeEnum.Gasoline:
                  selectOilKind = oilTypeEnum.Gasoline
                  oilPrice = gasolinePrice
                  break
              case oilTypeEnum.Diesel:
                  selectOilKind = oilTypeEnum.Diesel
                  oilPrice = dieselPrice
                  break
              case oilTypeEnum.LPG:
                  selectOilKind = oilTypeEnum.LPG
                  oilPrice = lpgPrice
                  break
              case oilTypeEnum.HighGasoline:
                  selectOilKind = oilTypeEnum.HighGasoline
                  oilPrice = highGasolinePrice
                  break

              default:
                  selectOilKind =  oilTypeEnum.None
                  oilPrice = 0
                  break
            }

            ret[0] = selectOilKind
            ret[1] = oilPrice
            return ret
      }

      //정렬타입에 해당하는 리스트모델의 키값 얻음
      function getListModelElementName(order) {
          var columnName = CommonConstants.Empty
          switch(order) {
              case SearchResultViewEvent.DistanceOrder:
                columnName = SearchConstants.Distance
                break
              case SearchResultViewEvent.PriceOrder:
                columnName = SearchConstants.Price
                break
              case SearchResultViewEvent.NameOrder:
                columnName = CommonConstants.Name
          }
          return columnName
      }

      //(상호,...)이름/표준 (상호,...)이름중 우선순위에 따라 얻기
      function getStdName(result) {
          var titleName = result[SearchConstants.ResultStdName]
          if (CommonUtils.isEmpty(titleName))
              titleName = result[SearchConstants.ResultName]
          return titleName
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
              arg [CommonConstants.Caller ] = CommonConstants.objectName(ViewRegistry.SearchResultView)
              data[CommonConstants.Arg    ] = arg
              sendEvent(SearchResultViewEvent.CreateView, data)
          }

          function __closeDialog(yes) {
              var uniqNum = dlgUniqueNum
              closeDialog(uniqNum, yes)
          }

          function closeDialog(uniqNum, yes) {
              if (yes){
                  switch(uniqNum) {
                     case textEnum.NoDataKo:
                     case textEnum.NoDataNoNet:
                         withinRadiusFromAlongRoute = false
                         if (!isRegionSearch) {
                            back()
                            return
                         }
                         break
                     case textEnum.BrandNoData:
                     case textEnum.KindNoData:
                     case textEnum.ChargeNoData:
                         objectNameOfSetting = CommonConstants.Empty
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

      function createModel(oilSearchType, result, startIndex, totalCount, addCount) {
          console.log("[Perf][SearchResultView][createModel]before")
          var data = ({})
          if (startIndex <= 0) {
              searchRoot.result = []
              if (CommonUtils.isEqual(listView.maxCount , SearchConstants.RequestMaxCount)) {
                  searchRoot.result = result
                  if (searchRoot.result.length > 0) {
                      if (isSearchType(CommonConstants.Around)) {
                          if (!searchRoot.result[0][SearchConstants.ResultSort]) {
                              if (doSort()) {
                                  return
                              }
                          }
                      }
                  }
              }
              nearDistValue =  CommonConstants.MAX_INT_VALUE
              lowestPriceValue = CommonConstants.MAX_INT_VALUE
              modelNearIndex = null
              modelLowestIndex = null

              requestRemoveAllMarker()

              initListView()
              if (CommonUtils.isEqual(oilSearchType, searchRoot.oilSearchType)) {
                listModel.clear()
              }
          }

          stopProgressViewByCond(oilSearchType, startIndex, totalCount, result.length ? result[0] : null)

          if (startIndex <= 0 || !CommonUtils.isEqual(listView.totalCount , totalCount)) {
              calcTotalPage(totalCount)
          }

          if (CommonUtils.isIndexOf(owner, [CommonConstants.objectName(ViewRegistry.TextSearchView),
                                            CommonConstants.objectName(ViewRegistry.RecentSearchPageView)])) {
              if (addCount > 0) {
                  if (!CommonUtils.isEqual(listView.maxCount , SearchConstants.RequestMaxCount)) {
                     listView.maxCount = SearchConstants.RequestMaxCount
                  } else {
                      addCount = 0
                  }
              }
          }


          var oilArr = null
          var oilPrice = null
          var selectOilKind = null

          var i = 0
          var len = (result) ? result.length : 0
          for (i; i< len; i++) {
              if (CommonUtils.isUndefined(typeof result[i])) {
                  continue
              }

              if (CommonUtils.isIndexOf(owner, [CommonConstants.objectName(ViewRegistry.TextSearchView),
                                                CommonConstants.objectName(ViewRegistry.RecentSearchPageView)])) {
                  if (addCount > 0) {
                    searchRoot.result[startIndex + i] = result[i]
                  }
              }

              oilArr = getOilKindWidthPrice(result[i])
              oilPrice = oilArr[1]

              //가까운
              if ( result[i][SearchConstants.ResultDistance] > 0 ) {
                  if (nearDistValue > result[i][SearchConstants.ResultDistance]) {
                      nearDistValue = result[i][SearchConstants.ResultDistance]
                  }
              }

              //최저가
              if (oilPrice > 0) {
                  if (lowestPriceValue > oilPrice) {
                      lowestPriceValue = oilPrice
                  }
              }
          }


          //인덱스오류#1 - 최대갯수 요청시는 firstIndex를 0으로 할당
          i = 0
          var firstIndex = startIndex
          if (CommonUtils.isEqual(listView.maxCount , SearchConstants.RequestMaxCount)) {
            firstIndex = 0
            if (listView.totalCount > 0) {
                i = startIndex
                if ((listView.totalPage - 1) <= listView.requestPageIndex) {
                    var remain = listView.totalCount % SearchConstants.RequestCount
                    if (remain)
                        len = i + remain
                    else
                        len = i + SearchConstants.RequestCount
                } else {
                    len = i + SearchConstants.RequestCount
                }
            }
          }

          if (CommonUtils.isIndexOf(owner, [CommonConstants.objectName(ViewRegistry.TextSearchView),
                                            CommonConstants.objectName(ViewRegistry.RecentSearchPageView)])) {
              if (addCount > 0) {
                result = searchRoot.result
              }
          }

          for (i; i< len; i++) {
              if (CommonUtils.isUndefined(typeof result[i])) {
                  continue
              }
              oilArr = getOilKindWidthPrice(result[i])
              selectOilKind = oilArr[0]
              oilPrice = oilArr[1]

              var index =  firstIndex   + i

              // 업종검색의 주유소/충전소가 주변검색의 주유소/충전소 처럼 동작할 수 있도록 브랜드 코드값을 넣어줌
              if (isSearchType(CommonConstants.Industry)) {
                  if (CommonUtils.isEqual(searchRoot.kind , categoryEnum.GasSt)       ||
                      CommonUtils.isEqual(searchRoot.kind , categoryEnum.ChargingSt)  ||
                      CommonUtils.isEqual(searchRoot.kind , categoryEnum.ChargingEvSt)) {
                      if (!CommonUtils.isEmpty(textParam))
                        result[i][SearchConstants.ResultOPIPoleCate] = CommonUtils.number(textParam)
                  }
              }

              //[타당성체크#1]
              //Q.타당하지 않은 데이타가 들어오는 경우가 있는것 같음 - 검증 필요
              //A.리스트 아이템에 넣지 않는다.
              var stdName =  getStdName(result[i])
              stdName = CommonUtils.trim(stdName)
              if (CommonUtils.isEmpty(stdName)) {
                 searchRoot.result[index] = result[i]
                 continue
              }

              var poleCate = result[i][SearchConstants.ResultOPIPoleCate]
              if (CommonUtils.isEqual(oilSearchType , searchTypeEnum.AlongRoute)){
                  if (CommonUtils.isEqual(searchRoot.kind , categoryEnum.GasSt)     ||
                      CommonUtils.isEqual(searchRoot.kind , categoryEnum.ChargingSt) ){
                   poleCate = result[i][SearchConstants.ResultBrand]
                  }
              }

              data = ({})
              data = {
                  seq:          listModel.count,
                  idx:          index,//searchRoot.result의 인덱스를 찾기 위함
                  kind:         oilTypeArr[selectOilKind],
                  x:            result[i][SearchConstants.ResultX],
                  y:            result[i][SearchConstants.ResultY],
                  name:         stdName,
                  address:      result[i][SearchConstants.ResultAddress],
                  distance:     result[i][SearchConstants.ResultDistance],
                  telnum:       result[i][SearchConstants.ResultTelNum],
                  angle:        result[i][SearchConstants.ResultDeg],
                  lowest:       result[i][SearchConstants.ResultLowestOilSt],
                  price:        oilPrice,
                  poiID:        result[i][SearchConstants.ResultPOIID],
                  navSeq:       result[i][SearchConstants.ResultNavseq],
                  poleCate:     poleCate,
                  rpFlag:       result[i][SearchConstants.ResultRpFlag],

                  visible:      false,                                                                                                                      //리스트뷰 영역 안에 visible여부
                  height:       listView.rowHeight,                                                                                                         //리스트뷰 항목 높이

                  //전기차 충전소 상세검색
                  request:      (resultEvDataSync && result[i][SearchConstants.ResultOnline] && !result[i][SearchConstants.ResultAlongRoute]) ? 1 : 0,      //전기차 상세 요청 갯수
                  response:     (resultEvDataSync && result[i][SearchConstants.ResultOnline] && !result[i][SearchConstants.ResultAlongRoute]) ? 1 : 0,      //전기차 상세 수신 갯수
                  count:        (resultEvDataSync && result[i][SearchConstants.ResultOnline] && !result[i][SearchConstants.ResultAlongRoute]) ? 1 : 0,      //전기차 상세 결과 갯수
                  key:          (resultEvDataSync && result[i][SearchConstants.ResultOnline] && !result[i][SearchConstants.ResultAlongRoute]) ? index : -1, //전기차 충전소에서 마커를 표출하기 위한 인덱스로

                  //POI 상세검색: (주유소,충전소,전기차충전소)경로상 검색 > 상세검색을 통해 x,y좌표 보정
                  requestdetail: 0,
                  resposedetail: 0,
              }

              if ( listModel.count >  index) {
                  listModel.set (index, data)
              } else {
                  listModel.append(data)
              }
              searchRoot.result[index] = result[i]
          }
          if (listModel.count > 0) {
              var item = listModel.get(listModel.count-1)
              console.log("[Perf][SearchResultView][createModel] Next page available["+item.seq+"]="+item.name)
          }
          sendUpdateSearchResultCount(query, totalCount)

          if ( (listView.totalPage - 1) <= listView.requestPageIndex) {
              sendUpdateSearchResultCount(query,listModel.count)
          }
          viewSizeChanged(frameHeight)
          messageVisible = (listModel.count ) ? false : CommonUtils.isEqual(oilSearchType , searchTypeEnum.AlongRoute) ? false : true
          messageText =  messageVisible ? textArr[textEnum.NoData] : CommonConstants.Empty
          if (listModel.count > 0) {
               if (startIndex <= 0 ) {
                   initMarker()
               } else {
                   requestPageMarker(startIndex)
               }
          }

          if (startIndex <= 0 && (searchRoot.visible ||
                                   listModel.count > 0)) {
              if( CommonUtils.isEqual(searchRoot.kind , categoryEnum.GasSt)       ||
                  CommonUtils.isEqual(searchRoot.kind , categoryEnum.ChargingSt)  ||
                  CommonUtils.isEqual(searchRoot.kind , categoryEnum.ChargingEvSt)) {
                  if (CommonUtils.isEqual(oilSearchType , searchTypeEnum.WithinRadius)){
                      if (isRg) {
                        if (!searchRoot.visible ||
                             withinRadiusFromAlongRoute) {
                            if (getResultLineChanged()) {
                                showToast(textArr[textEnum.SwtichOfflineSearch])
                            } else {
                                var text = CommonConstants.Empty
                                var kindTxt = getChargeKindTxt()
                                if(searchRoot.online) {
                                    text = (textArr[textEnum.UseOnlineWithinRadius]).arg(kindTxt).arg(kindTxt)
                                } else {
                                    text = (textArr[textEnum.UseOfflineWithinRadius]).arg(kindTxt)
                                }
                                showToast(text)
                            }
                        }
                      }
                      withinRadiusFromAlongRoute = false
                  }
                  if (CommonUtils.isEqual(oilSearchType , searchTypeEnum.AlongRoute)) {
                      zone1SearchType = searchTypeEnum.AlongRoute
                      zone1Title = textArr[textEnum.Zone1Title]
                                 + textArr[textEnum.Space]
                                 + "("
                                 + CommonUtils.string(searchTypeArr[searchTypeEnum.AlongRoute]).toLowerCase()
                                 + ")"
                  } else {
                      zone1SearchType = searchTypeEnum.WithinRadius
                      zone1Title = textArr[textEnum.Zone1Title]
                                 + textArr[textEnum.Space]
                                 + "("
                                 + CommonUtils.string(searchTypeArr[searchTypeEnum.WithinRadius]).toLowerCase()
                                 + ")"
                  }
              }

              //검색 결과가 있을때 hide시켜 놓은 것 show
              if (!CommonUtils.isEqual(searchRoot.kind , categoryEnum.ChargingEvSt) ||
                   (resultEvDataSync && getResultOnline() && !getResultAlongRoute())) {
                showHideByOwner()
              }
          }

          if (manualClickOption) {
              manualClickOption = false
              if (listModel.count <= 0) {
                  if (changedOilBrand) {
                      messageText = CommonConstants.Empty
                      var brandNoData = (textArr[textEnum.BrandNoData]).arg(getChargeKindTxt())
                      showDialog(textEnum.BrandNoData,  textArr[textEnum.Notification], brandNoData, false)
                  } else if (changedOilKind) {
                      messageText = CommonConstants.Empty
                      var kindNoData = (textArr[textEnum.KindNoData]).arg(getChargeKindTxt())
                      showDialog(textEnum.KindNoData,  textArr[textEnum.Notification], kindNoData, false)
                  } else if (changedChargeType) {
                      messageText = CommonConstants.Empty
                      showDialog(textEnum.ChargeNoData,  textArr[textEnum.Notification], textArr[textEnum.ChargeNoData], false)
                  } else {
                      messageText =  textArr[textEnum.NoData]
                  }
              }
          } else {
              messageText =  textArr[textEnum.NoData]
          }
          if(startIndex <= 0) {
             requestEvDetailInfoByScroll(500)
          }
          manualClickOption = autoClickOption = false
          console.log("[Perf][SearchResultView][createModel]after")
      }

      // Zon3에 해당하는 정렬 버튼이 눌리도록 ....
      function sortListData() {
          if (CommonUtils.isEqual(oldOrder, searchRoot.order) ) {
              return true
          }
          oldOrder = searchRoot.order

          showProgressView()
          initListView()
          listViewSort()
          return true
      }

      function getSeqByFuel( fuel) {
          var seq = 0
          switch(fuel) {
              case  SettingConstants.OilTypeGasoline: seq = SearchConstants.ResultOPIGasolinePrice;break;
              case  SettingConstants.OilTypeDiesel:seq = SearchConstants.ResultOPIDieselPrice;break;
              case  SettingConstants.OilTypeLPG:seq = SearchConstants.ResultOPILPGPrice;break;
              case  SettingConstants.OilTypeElec:break;
              case  SettingConstants.OilTypePremiumGasoline:seq = SearchConstants.ResultOPIHiGasolinePrice;break;
          }
          return seq
      }

      //리스트 목록 정렬 요청 -> 검색결과: __onResultSort(...)
      function listViewSort() {
        console.log("[Perf][[SearchResultView][RequestSort]")
        if (isSearchType(CommonConstants.Around)) {
            searchRoot.result = searchRoot.resultAround
        }
        timerSort = CommonUtils.destroy(timerSort)
        timerSort = CommonUtils.delay(searchRoot, 1, function (){
            var data = ({})
            data[CommonConstants.Key] = searchRoot.order
            data[CommonConstants.Fuel] = searchRoot.oilKind
            data[CommonConstants.Text] = searchRoot.query
            data[CommonConstants.Value] = searchRoot.result
            sendEvent(SearchResultViewEvent.RequestSort ,data)
            queueChanged(0)
        })
      }

      //(통합,최근)검색 -> 검색결과: __onResultCacheMerge(...)
      function requestCacheMerge(result, startIndex, totalCount, lastPage, resultCache) {
          var data = ({})
          data[CommonConstants.L] = lastPage
          data[CommonConstants.Index] = startIndex
          data[CommonConstants.Count] = totalCount
          data[CommonConstants.Result] = result
          data[CommonConstants.Value] = resultCache
          sendEvent(SearchResultViewEvent.RequestCacheMerge, data)
      }

      function __onResultCacheMerge(result, resultCache, startIndex, totalCount, addCount) {
          if (isClosed) {
              return
          }
          searchRoot.resultCache = resultCache
          totalCount = totalCount + addCount
          createModel(oilSearchType, result, startIndex, totalCount, addCount)
      }

      function initListView() {
          listView.init()
      }

      function initMarker() {
          listView.requestPageIndex = defaultStartIndex
          requestPageMarker(defaultStartIndex)
          setCurrentList(defaultStartIndex)
          setSelectList(-1, defaultStartIndex)
      }

      //캐시 데이타 요청 결과
      function __onResultCache(search, result, totalCount, startIndex) {
          if (isClosed) {
              return
          }
          startIndex = Math.max(0, startIndex)
          if(startIndex <= 0)
            resultCache = ([])
          if (addrCode <= 0) {
              for(var i= 0; i<result.length; i++) {
                  var index =   startIndex   + i
                  resultCache[index] = result[i]
              }
          }
      }

      //최저가 요청 결과
      function __onResultLowest(search, result, totalCount, startIndex) {
          if (isClosed) {
              return
          }
          console.log("[Perf][SearchResultView]__onResultLowest")
          if (totalCount <= 0 || result.length <= 0) {
              lowestPOID = 0
              return
          }
          lowestPOID =  result[startIndex][SearchConstants.ResultPOIID]
      }

      //(경로상) 주유소 브랜드
      function isGasStationBrandOnPath(poleCate) {
          var ret = null
          switch(poleCate) {
              case poleCateOnPathEnum.GasStationSK          :
              case poleCateOnPathEnum.GasStationGS          :
              case poleCateOnPathEnum.GasStationOILBANK     :
              case poleCateOnPathEnum.GasStationSOIL        :
              case poleCateOnPathEnum.GasStationTOIL        :
              case poleCateOnPathEnum.GasStationUNKNOWN     :
              case poleCateOnPathEnum.GasStationNCOIL       :
              case poleCateOnPathEnum.GasStationNHOIL       :
              case poleCateOnPathEnum.GasStationSevenDHOIL  :
              case poleCateOnPathEnum.Alttle                :
              case poleCateOnPathEnum.GasStationEX          :
              case poleCateOnPathEnum.GasStationSKINCHUN    :
                  ret = true
                  break
              default:
                  ret = false
                  break
          }
          return ret
      }

      //(경로상) LPG충전소 브랜드
       function isChargingStBrandOnPath(poleCate) {
           var ret = null
           switch(poleCate) {
              case poleCateOnPathEnum.ChargingStSK          :
              case poleCateOnPathEnum.ChargingStGS          :
              case poleCateOnPathEnum.ChargingStOILBANK     :
              case poleCateOnPathEnum.ChargingStSOIL        :
              case poleCateOnPathEnum.ChargingStUNKNOWN     :
              case poleCateOnPathEnum.ChargingStSKINCHUN    :
              case poleCateOnPathEnum.ChargingStNCOIL       :
              case poleCateOnPathEnum.ChargingStNHOIL       :
              case poleCateOnPathEnum.ChargingStSevenDHOIL  :
              case poleCateOnPathEnum.ChargingStEX          :
              case poleCateOnPathEnum.ChargingStE1          :
              case poleCateOnPathEnum.Alttle                :
                  ret = true
                  break
              default:
                  ret = false
                  break
           }
           return ret
       }

      //경로상 검색결과: 주유소,충전소  <- requestAlongRoute(...)의 검색결과
      property var resultAlongRoute: ([])
      property var resultDetail: ([])
      function __onResultAlongRoute(search, result, totalCount, startIndex) {
          if (isClosed) {
              return
          }
          console.log("[Perf][SearchResultView][__onResultAlongRoute]before")
          resultAlongRoute = ([])
          for (var i=0; i<totalCount; i++ ) {
              //(주유소 || 주유소&충전소) || (충전소 || 주유소&충전소) 인 경우만 표출
              if ((!CommonUtils.isEqual(searchRoot.kind , result[i][SearchConstants.ResultKind])) &&
                  (!CommonUtils.isEqual(categoryOilStEnum.GasChargingSt , result[i][SearchConstants.ResultKind]))) {
                  continue
              }

              //브랜드에 해당하는 경우 체크
              if (CommonUtils.isEqual(searchRoot.kind , categoryEnum.GasSt)) {
                if (!isGasStationBrandOnPath(result[i][SearchConstants.ResultOPIPoleCate])) {
                    continue
                }
              } else if (CommonUtils.isEqual(searchRoot.kind , categoryEnum.ChargingSt)) {
                if (!isChargingStBrandOnPath(result[i][SearchConstants.ResultOPIPoleCate])) {
                    continue
                }
              }

              //반경내 검색과 코드값이 동일하도록 기본값을 더함
              switch(searchRoot.kind) {
              case categoryEnum.GasSt:
                  result[i][SearchConstants.ResultOPIPoleCate]+=SearchConstants.OILBRANDBASE
                  break
              case categoryEnum.ChargingSt:
                  result[i][SearchConstants.ResultOPIPoleCate]+=SearchConstants.CHARGINGBASE
                  break
              }
              //[*이슈*]혹시 온/오프라인 스위칭시 동기화가 안되어 나타날 수 있어 보강
              if (result[i][SearchConstants.ResultOnline]) {
                resultAlongRoute.push(result[i])
              }
          }
          resultDetail = ([])
          processResult("경로상검색", resultAlongRoute, resultAlongRoute.length, defaultStartIndex)

          console.log("[Perf][SearchResultView][__onResultAlongRoute]after")
      }

      //경로상 검색결과 : 전기충전소 <- requestAlongRouteEv(...)의 검색결과
      function __onResultAlongRouteEStation(search, result, totalCount, startIndex) {
          if (isClosed) {
              return
          }
          console.log("[Perf][SearchResultView][__onResultAlongRouteEStation]before")
          resultAlongRoute = ([])
          for (var i=0; i<totalCount; i++ ) {
              //[*이슈*]혹시 온/오프라인 스위칭시 동기화가 안되어 나타날 수 있어 보강
              if (result[i][SearchConstants.ResultOnline]){
                    resultAlongRoute.push(result[i])
              }
          }
          resultDetail = ([])
          processResult("경로상검색", resultAlongRoute, resultAlongRoute.length, defaultStartIndex)
          console.log("[Perf][SearchResultView][__onResultAlongRouteEStation]after")
      }


      //[전기차정보]-한개의 변수에 여러개의 정보가 들어오고 있어 파싱
      function getInfoChargingEvSt(index) {
          if (index < 0 || listModel.count <= index)
              return
          var item = listModel.get(index)
          var info = searchRoot.result[item.idx] [SearchConstants.ResultChargerInfo ]
          var oper = CommonConstants.Empty
          var abbreviation = CommonConstants.Empty
          info = CommonUtils.trim(info)
          if (CommonUtils.isEmpty(info))
              return
          var data = CommonUtils.split(info, ";")
          for(var i= 0; i<data.length; i++) {
              var d = data[i]
              var s = CommonUtils.indexOf(d, "[")
              var e = CommonUtils.indexOf(d, "]")
              if(s >= 0 && e >= 0) {
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
                          } else {
                             if (CommonUtils.isNull(evBrandEnum[v]))
                                continue
                              v = evBrandEnum[v]
                          }
                          abbreviation = evBrandNameArr[v]
                          if (!CommonUtils.isIndexOf(oper, abbreviation)) {
                              if (!CommonUtils.isEmpty(oper)) {
                                  oper += ","
                              } else {
                                  //브랜드 코드를 주유소/충전소와 겹치지 않도록 만듦
                                   searchRoot.result[item.idx][SearchConstants.ResultOPIPoleCate] = v + SearchConstants.EVCHARGINGBASE
                              }
                              oper += evBrandNameArr[v]
                          }
                          break;
                      case SearchConstants.Tid:
                          if (resultEvDataSync &&
                              searchRoot.result[item.idx][SearchConstants.ResultOnline] &&
                              !searchRoot.result[item.idx][SearchConstants.ResultAlongRoute])
                              continue
                          switch(CommonUtils.number(v)) {
                              case evKindEnum.DCDemo:
                                  searchRoot.result[item.idx][SearchConstants.ResultDCDemo] = true
                                  searchRoot.result[item.idx][SearchConstants.ResultFastCharge] = true
                                  break
                              case evKindEnum.ACslow:
                                  searchRoot.result[item.idx][SearchConstants.ResultSlowCharge] = true
                                  break
                              case evKindEnum.DCdemoAC3:
                                  searchRoot.result[item.idx][SearchConstants.ResultAC3] = true
                                  searchRoot.result[item.idx][SearchConstants.ResultDCDemo] = true
                                  searchRoot.result[item.idx][SearchConstants.ResultFastCharge] = true
                                  break
                              case evKindEnum.DCCombo:
                                  searchRoot.result[item.idx][SearchConstants.ResultDCCombo] = true
                                  searchRoot.result[item.idx][SearchConstants.ResultFastCharge] = true
                                  break
                              case evKindEnum.DCdemoCombo:
                                  searchRoot.result[item.idx][SearchConstants.ResultDCDemo] = true
                                  searchRoot.result[item.idx][SearchConstants.ResultDCCombo] = true
                                  searchRoot.result[item.idx][SearchConstants.ResultFastCharge] = true
                                  break
                              case evKindEnum.DCdemoAC3Combo:
                                  searchRoot.result[item.idx][SearchConstants.ResultAC3] = true
                                  searchRoot.result[item.idx][SearchConstants.ResultDCDemo] = true
                                  searchRoot.result[item.idx][SearchConstants.ResultDCCombo] = true
                                  searchRoot.result[item.idx][SearchConstants.ResultFastCharge] = true
                                  break
                              case evKindEnum.AC3:
                                  searchRoot.result[item.idx][SearchConstants.ResultAC3] = true
                                  searchRoot.result[item.idx][SearchConstants.ResultFastCharge] = true
                                  break
                          }
                          break
                  }
              }
         }
    }

    function getLcdCode() {
        var currAddrCode = CommonUtils.string(searchRoot.currAddrCode)
        if (currAddrCode.length < 8)
            return null
        return CommonUtils.substring(currAddrCode, 0, 2)
    }

    function getMcdCode() {
        var currAddrCode = CommonUtils.string(searchRoot.currAddrCode)
        if (currAddrCode.length < 8)
            return null
        return CommonUtils.substring(currAddrCode, 2, 5)
    }

    function getScdCode() {
        var currAddrCode = CommonUtils.string(searchRoot.currAddrCode)
        if (currAddrCode.length < 8)
            return null
        return CommonUtils.substring(currAddrCode, 5, 8)
    }

    function __onCurrentStreetChanged(lcdNmcdCode) {
        var scdCode = "000"
        var addrCode = lcdNmcdCode + scdCode
        currAddrCode = CommonUtils.number(addrCode)
    }

    //전기차충전소 상세정보: requestEvDetailInfo()요청의 검색결과
    function __onResultEvDetailInfo(search, result, totalCount, startIndex) {
        if (isClosed) {
            return
        }
        console.log("[Perf][SearchResultView][__onResultEvDetailInfo]before")
        if (startIndex < 0 || listModel.count <= startIndex) {
            console.log("[Perf][SearchResultView][__onResultEvDetailInfo]after")
            return
        }
        var item = listModel.get(startIndex)
        var res = ++item.response
        listModel.setProperty(startIndex, SearchConstants.Count, totalCount )
        listModel.setProperty(startIndex, SearchConstants.Response, res )
        setResponseEvDetailInfo(startIndex)

        if (!(resultEvDataSync &&
            searchRoot.result[item.idx][SearchConstants.ResultOnline] &&
            !searchRoot.result[item.idx][SearchConstants.ResultAlongRoute])) {
            if (totalCount <= 0) {
                result[SearchConstants.ResultSlowCharge  ] = false
                result[SearchConstants.ResultDCDemo      ] = false
                result[SearchConstants.ResultAC3         ] = false
                result[SearchConstants.ResultDCCombo     ] = false
                result[SearchConstants.ResultStationCount] = 1
                result[SearchConstants.ResultChargerInfo ] = CommonConstants.Empty
                result[SearchConstants.ResultAddress     ] = CommonConstants.Empty
            }
            searchRoot.result[item.idx] [SearchConstants.ResultFastCharge  ] = result[SearchConstants.ResultFastCharge  ]
            searchRoot.result[item.idx] [SearchConstants.ResultSlowCharge  ] = result[SearchConstants.ResultSlowCharge  ]
            searchRoot.result[item.idx] [SearchConstants.ResultDCDemo      ] = result[SearchConstants.ResultDCDemo      ]
            searchRoot.result[item.idx] [SearchConstants.ResultAC3         ] = result[SearchConstants.ResultAC3         ]
            searchRoot.result[item.idx] [SearchConstants.ResultDCCombo     ] = result[SearchConstants.ResultDCCombo     ]
        }

        searchRoot.result[item.idx] [SearchConstants.ResultStationCount] = result[SearchConstants.ResultStationCount]
        searchRoot.result[item.idx] [SearchConstants.ResultChargerInfo ] = result[SearchConstants.ResultChargerInfo ]

        getInfoChargingEvSt(startIndex)

        item = searchRoot.result[item.idx][SearchConstants.ResultObject]
        if (!CommonUtils.isObject(item)) {
            return
        }

        var styleLoader = item.styleLoader
        if (CommonUtils.isNull(styleLoader) || CommonUtils.isNull(styleLoader.children)) {
            return
        }
        if (styleLoader.children.length <= 0) {
            return
        }
        if (CommonUtils.isNull(styleLoader.children[0].refreshCharger)) {
            return
        }
        styleLoader.children[0].refreshCharger()
        console.log("[Perf][SearchResultView][__onResultEvDetailInfo]after")
    }

    //(통합,주변,업종,주소,...)검색의 정렬 결과
    function __onResultSort(search, result, totalCount, startIndex) {
        if (isClosed) {
            return
        }
        console.log("[Perf][SearchResultView][__onResultSort]before")
        processResult(search, result, totalCount, startIndex)
        console.log("[Perf][SearchResultView][__onResultSort]after")
    }

    //경로상 좌표가 잘못되어 상세검색을 통해 x,y좌표 보정
    function __onResultDetailInfo(search, result, totalCount, startIndex) {
        if (isClosed) {
            return
        }
        console.log("[Perf][SearchResultView][__onResultDetailInfo]before")
        hideProgressView()

        if (totalCount <= 0) {
            console.log("[Perf][SearchResultView][__onResultDetailInfo]after")
            return
        }
        var index = startIndex
        if (index < 0 || listModel.count <= index) {
            console.log("[Perf][SearchResultView][__onResultDetailInfo]after")
            return
        }
        var item = listModel.get(index)
        var x = result[SearchConstants.ResultX]
        var y = result[SearchConstants.ResultY]
        if (!CommonUtils.isEqual(x, searchRoot.result[item.idx][SearchConstants.ResultX])) {
            console.log("__onResultDetailInfo=>[success]type:"
                       + searchRoot.searchType
                       + "("
                       + searchRoot.kind
                       + "),poiID:"
                       + result[SearchConstants.ResultPOIID]
                       + ",index:"
                       + index
                       + ",x:"
                       + searchRoot.result[item.idx][SearchConstants.ResultX]
                       + "->"
                       + x)
            searchRoot.result[item.idx][SearchConstants.ResultX] = x
        }
        if (!CommonUtils.isEqual(y, searchRoot.result[item.idx][SearchConstants.ResultY])) {
            console.log("__onResultDetailInfo=>[success]type:"
                       + searchRoot.searchType
                       + "("
                       + searchRoot.kind
                       + "),poiID:"
                       + result[SearchConstants.ResultPOIID]
                       + ",index:"
                       + index
                       + ",y:"
                       + searchRoot.result[item.idx][SearchConstants.ResultY]
                       + "->"
                       + y)
            searchRoot.result[item.idx][SearchConstants.ResultY] = y
        }
        listModel.setProperty(index, CommonConstants.X, x)
        listModel.setProperty(index, CommonConstants.Y, y)
        listModel.setProperty(index, CommonConstants.Response + CommonConstants.Detail, true)
        console.log("[Perf][SearchResultView][__onResultDetailInfo]after")
    }

    //(통합,주변,업종,주소,...)검색의 결과
    function __onResult(search, result, totalCount, startIndex) {
        if (isClosed) {
            return
        }
        console.log("[Perf][SearchResultView][__onResult]before")
        processResult(search, result, totalCount, startIndex)
        console.log("[Perf][SearchResultView][__onResult]after")
    }

    function processResult(search, result, totalCount, startIndex) {
        if (CommonUtils.isIndexOf(owner, [CommonConstants.objectName(ViewRegistry.TextSearchView)]) ) {
            if (search.length > 20) {
               showToast(textArr[textEnum.Over20Length])
            }
        }
         //totalCount가 잘못 넘어오는 경우가 있어 보정 함
         if (startIndex <= 0) {
             if (result.length  <= 0) {
                 totalCount = 0
             }
         }

          var data = ({})

          //[주의] 타임아웃 후에 결과가 들어오는 경우를 위하여 ...
          if (isShowDialog()) {
              data[CommonConstants.ObjectName] = CommonConstants.objectName(ViewRegistry.CommonDialogView)
              sendEvent(SearchResultViewEvent.CloseView, data)
              closeDialog(dlgUniqueNum, false)
              data = ({})
          }


          var oilSearchType = searchRoot.oilSearchType
          startIndex = Math.max(0, startIndex)
          if (startIndex <= 0 && totalCount <= 0) {
             if (CommonUtils.isEqual(searchType ,  CommonConstants.Around) &&
                 (CommonUtils.isEqual(searchRoot.kind , categoryEnum.GasSt)       ||
                  CommonUtils.isEqual(searchRoot.kind , categoryEnum.ChargingSt)  ||
                  CommonUtils.isEqual(searchRoot.kind , categoryEnum.ChargingEvSt))) {
                if (CommonUtils.isEqual(oilSearchType , searchTypeEnum.AlongRoute)){
                    saveWithinRadiusFromAlongRoute()
                    requestOptionMenu()
                    autoClickOption = true
                    requestOption()
                } else {
                    if (!searchRoot.visible) {
                        var messageEnum = (searchRoot.isRg && !searchRoot.online)? textEnum.NoDataNoNet : textEnum.NoDataKo
                        showDialog(messageEnum,  textArr[textEnum.Notification], textArr[messageEnum], false)
                    }
                }
              } else {
                 if (!searchRoot.visible) {
                     if (CommonUtils.isIndexOf(owner, [CommonConstants.objectName(ViewRegistry.TextSearchView)])) {
                         if (CommonUtils.isNull(resultCache) || resultCache.length <= 0) {
                             if (addrCode <= 0) {
                                 data[CommonConstants.ObjectName ] = owner
                                 sendEvent(SearchResultViewEvent.NoResult, data)
                                 if (CommonUtils.isIndexOf(caller, [CommonConstants.objectName(ViewRegistry.SearchTabView)])) {
                                     sendEvent(SearchResultViewEvent.CloseView, data)
                                 }
                                 data[CommonConstants.ObjectName ] = CommonConstants.objectName(ViewRegistry.SearchResultView)
                                 sendEvent(SearchResultViewEvent.CloseView, data)
                                 return
                             } else {
                                 showDialog(textEnum.NoDataKo,  textArr[textEnum.Notification], textArr[textEnum.NoDataKo], false)
                             }
                         }
                     } else {
                         if (useCurrAddrCode && searchRoot.addrCode <= 0) {
                             if (CommonUtils.isEqual(searchType , CommonConstants.Industry)) {
                                 if (!CommonUtils.isEqual(searchRoot.addrCode, CommonConstants.MAX_INT_VALUE)) {
                                     searchRoot.addrCode = CommonConstants.MAX_INT_VALUE
                                     requestSearch(searchRoot.query, searchRoot.order,  listView.requestPageIndex, getReqMaxCount())
                                     return
                                 }
                             }
                         }
                         if (CommonUtils.isIndexOf(owner, [CommonConstants.objectName(ViewRegistry.RecentSearchPageView)])) {
                             if (CommonUtils.isNull(resultCache) || resultCache.length <= 0) {
                                 showDialog(textEnum.NoDataKo,  textArr[textEnum.Notification], textArr[textEnum.NoDataKo], false)
                                 return
                             }

                         } else {
                            showDialog(textEnum.NoDataKo,  textArr[textEnum.Notification], textArr[textEnum.NoDataKo], false)
                         }
                         if (CommonUtils.isEqual(searchType , SearchConstants.Address)) {
                             data[CommonConstants.DontUseEditBox] = true
                             sendEvent(SearchResultViewEvent.HideKeyboard, data)
                         }
                     }
                 }
              }
          }

          //지역검색인 경우 - 검색 데이타가 없는 경우 이전 데이타를 유지
          if (isRegionSearch) {
              if (startIndex <= 0 && totalCount <= 0) {
                  stopProgressViewByCond(oilSearchType, startIndex, totalCount, result.length ? result[0] : null)
                  addrCode = beforAddrCode
                  return
              } else {
                  beforAddrCode = addrCode
              }
          }

          var i = null
          var j = null

          if (isSearchType(CommonConstants.Around)) {
            searchRoot.resultAround = []
            searchRoot.resultAround = result
            var len = Math.min(listView.maxCount, result.length)

            //주유소, 충전소인 경우
            if (CommonUtils.isEqual(searchRoot.kind , categoryEnum.GasSt) ||
                CommonUtils.isEqual(searchRoot.kind , categoryEnum.ChargingSt) ||
                CommonUtils.isEqual(searchRoot.kind , categoryEnum.ChargingEvSt)) {
                var resultBrand = getResultByOilBrand(oilBrand, startIndex )
                createModel(oilSearchType, resultBrand, defaultStartIndex, resultBrand.length, 0)
            } else {
                createModel(oilSearchType, result, startIndex, totalCount, 0)
            }
          } else {
              if (!isSearchType(CommonConstants.Name)) {
                  createModel(oilSearchType, result, startIndex, totalCount, 0)
                  return
              }

              var lastPage = false
              if (startIndex <= 0 || !CommonUtils.isEqual(listView.totalCount , totalCount)) {
                  calcTotalPage(totalCount)
              }
              if ((listView.totalPage - 1) <= listView.requestPageIndex ||
                  CommonUtils.isEqual(listView.maxCount , SearchConstants.RequestMaxCount)) {
                  lastPage = true
              }
              if (totalCount<=0 && searchRoot.resultCache.length > 0) {
                  requestCacheMerge(result, startIndex, totalCount, lastPage, searchRoot.resultCache)
              } else {
                  createModel(oilSearchType, result, startIndex, totalCount, 0)
              }
          }
      }

      //검색 요청을 갯수를 얻음
      function __queueChanged(count) {
          queueChanged(count)
      }

      function queueChanged(count) {
            queueCount = count
            timerQueue = CommonUtils.destroy(timerQueue)
            if (!reqSearch)
                return

            if (count<=0) {
                checkListEvItem(0)
                timerQueue = CommonUtils.delay(searchRoot, SearchConstants.NetworkTimeoutAPP, function() {
                    try {
                        hideProgressView()
                        hideShowByEvData()
                        resetEvIndex()
                    } catch(e) {
                    }
                }, false)
            } else {
                isShowProgressIndicator = true
            }
      }

      function hideShowByEvData() {
          if (isShowDialog()) {
              return
          }

          if (!CommonUtils.isEqual(searchRoot.kind, categoryEnum.ChargingEvSt)) {
              return
          }

          var item = null
          var isShowItem = null
          var startIndex = listView.indexAt(0,listView.contentY)
          var endIndex = listView.indexAt(0,listView.contentY+listView.height)
          if (startIndex < 0) {
              startIndex = 0
          }
          if (endIndex < 0) {
              endIndex = listModel.count - 1
          }

          for (var i=0; i<listModel.count; i++) {
              item = listModel.get(i)
              if (CommonUtils.isNull(item))
                  continue
              if (resultEvDataSync &&
                  searchRoot.result[item.idx][SearchConstants.ResultOnline] &&
                  !searchRoot.result[item.idx][SearchConstants.ResultAlongRoute])
                  return
              if (!item.response) {
                 isShowItem = false
                 continue
              }
              if (item.height<=0) {
                  isShowItem = false
                  continue
              }
              isShowItem = true
              break
          }

          if (searchRoot.visible) {
              if (!isShowItem) {
                messageVisible  = true
                if (CommonUtils.isEqual(objectNameOfSetting, CommonConstants.objectName(ViewRegistry.SettingEvChargeTypeView))) {
                    messageText = CommonConstants.Empty
                    showDialog(textEnum.ChargeNoData,  textArr[textEnum.Notification], textArr[textEnum.ChargeNoData], false)
                }
              } else {
                messageVisible = false
              }
          } else {
              if (isShowItem) {
                  showHideByOwner()
              } else  {
                  if (CommonUtils.isEqual(oilSearchType , searchTypeEnum.AlongRoute)){
                      saveWithinRadiusFromAlongRoute()
                      requestOptionMenu()
                      autoClickOption = true
                      requestOption()
                  } else {
                     if (queueCount>0) {
                         return
                     }
                     hideProgressView()
                     var messageEnum = (withinRadiusFromAlongRoute && !searchRoot.online)? textEnum.NoDataNoNet : textEnum.NoDataKo
                     showDialog(messageEnum,  textArr[textEnum.Notification], textArr[messageEnum], false)
                  }
              }
          }
      }

      function __focusActivatedZone1TextInput() {
        closeAllView()
      }


      function __onObjectHitting(hitTestResult) {
          var id = hitTestResult[SearchConstants.MarkerId]
          var type = hitTestResult[CommonConstants.Type]
          var index = CommonUtils.number(id) - markerBaseIndex
          if (!CommonUtils.isEqual(type , SearchResultViewEvent.HitMarker))
              return
            resetSelectList(listView.selectedIndex, index)
      }

      function isSearchType(v) { return CommonUtils.isIndexOf(searchType, v)}

      function initEnum() {
          allocateTextEnum()
          allocateImgEnum()
          allocateSoftKeyEnum()
          allocateCategoryEnum()
          allocateCategoryOilStEnum()
          allocateOilTypeEnum()
          allocatePoleCateEnum()
          allocatePoleCateOnPathEnum()
          allocateOilBrandEnum()
          allocateSearchTypeEnum()
          allocateSectionEnum()
          allocateKeyEnum()
          allocateEvBrandEnum()
          allocateEvKindEnum()
          allocateEvTypeEnum()

          allocateMenuEnum()

          //검색 결과가 없는 경우 현재 선택된 언어로 변환된 문자열을 대입
          if (!CommonUtils.isEmpty(messageText)) {
             messageText =  textArr[textEnum.NoData]
          }
      }

      function init(){
          initEnum()
          unloadMainComponent()
      }

      function applyDistance() {
          switch(owner) {
            case CommonConstants.objectName(ViewRegistry.SearchAroundView):
            case CommonConstants.objectName(ViewRegistry.RecentSearchPageView):
            case CommonConstants.objectName(ViewRegistry.TextSearchView):
            case CommonConstants.objectName(ViewRegistry.IndustrySearchView):
            case CommonConstants.objectName(ViewRegistry.AddressSearchView):
            default:
                distance = SearchResultViewEvent.Distance10Km
                break
          }
      }

      function initSetting() {
          if (isInitSetting) {
              return
          }
          isInitSetting = true

          requestOptionDefault()
          requestEvOptionDefault()
          requestOptionMenu()

          oilKind      = CommonUtils.isEqual(searchRoot.kind , categoryEnum.GasSt) ? oilTypeEnum.Gasoline :
                         (CommonUtils.isEqual(searchRoot.kind , categoryEnum.ChargingSt) ? oilTypeEnum.LPG : oilTypeEnum.Gasoline)
          order        = (isSearchType(CommonConstants.Around)) ? SearchResultViewEvent.DistanceOrder : SearchResultViewEvent.AlphabeticalOrder
          viaSetting   = CommonUtils.isEqual(caller , CommonConstants.objectName(ViewRegistry.RouteSettingView))

          unloadMainComponent()
      }

      function refreshView(selectIndex, contentX, contentY) {
          resetSelectList(selectIndex, selectIndex)
          listView.contentX = contentX
          listView.contentY = contentY
      }

      function back() {
          var data = ({})
          data[CommonConstants.ObjectName ] = CommonConstants.objectName(ViewRegistry.SearchResultDetailView)
          sendEvent(SearchResultViewEvent.CloseView, data)

          data = ({})
          if (CommonUtils.isIndexOf(caller, [CommonConstants.objectName(ViewRegistry.BookmarkPageView),
                                             CommonConstants.objectName(ViewRegistry.RouteSettingView)])) {

              if (CommonUtils.isIndexOf(owner, [CommonConstants.objectName(ViewRegistry.SearchAroundView),
                          CommonConstants.objectName(ViewRegistry.IndustrySearchView),
                          CommonConstants.objectName(ViewRegistry.AddressSearchView)])) {
                  data[CommonConstants.ObjectName] = owner
                  sendEvent(SearchResultViewEvent.ShowView, data )
               } else {
                  if (CommonUtils.isIndexOf(caller, [CommonConstants.objectName(ViewRegistry.BookmarkPageView)])) {
                      zone1Title = CommonConstants.Empty
                      zone1Edit = CommonConstants.Empty
                      data = ({})
                      data[CommonConstants.Url] = CommonConstants.url(ViewRegistry.SearchTabView)
                      data[SearchConstants.Page] = SearchConstants.FavoritePageIndex
                      sendEvent(SearchResultViewEvent.CreateView, data )
                  } else if (viaSetting && CommonUtils.isIndexOf(owner, [CommonConstants.objectName(ViewRegistry.TextSearchView)])) {
                      data[CommonConstants.ObjectName] = owner
                      sendEvent(SearchResultViewEvent.ShowView, data)
                  } else {
                      data = ({})
                      data[CommonConstants.ObjectName] = caller
                      sendEvent(SearchResultViewEvent.ShowView, data )
                  }
               }
          } else if (CommonUtils.isIndexOf(caller, [CommonConstants.objectName(ViewRegistry.SearchTabView)])) {
              zone1Title = CommonConstants.Empty
              zone1Edit = CommonConstants.Empty
              data = ({})
              data[CommonConstants.Url] = CommonConstants.url(ViewRegistry.SearchTabView)
              sendEvent(SearchResultViewEvent.CreateView, data )
          } else if (CommonUtils.isIndexOf(caller, [CommonConstants.objectName(ViewRegistry.NaviMenuView)])) {
              if (CommonUtils.isIndexOf(owner, [CommonConstants.objectName(ViewRegistry.SearchAroundView),
                        CommonConstants.objectName(ViewRegistry.IndustrySearchView),
                        CommonConstants.objectName(ViewRegistry.AddressSearchView)])) {
                   data[CommonConstants.ObjectName] = owner
                   sendEvent(SearchResultViewEvent.ShowView, data )
              }
          } else if (caller == CommonConstants.System) {
              // closed in cpp
              return
          }

          closeView()
      }

      function requestNrefresh() {
          initSetting()
          loadMainComponent()
          if (isActivated) {
              sendNotiZone1Tile()
              sendNotiZone1Edit()
              sendUpdateSearchResultCount(searchRoot.query, updateCount)
          }
          var data = ({})
          if (isSearchType(CommonConstants.Around)) {
              isRegionSearch = false
              if(isActivated) {
                  if ( (CommonUtils.isEqual(searchRoot.kind , categoryEnum.GasSt)       ||
                        CommonUtils.isEqual(searchRoot.kind , categoryEnum.ChargingSt)  ||
                        CommonUtils.isEqual(searchRoot.kind , categoryEnum.ChargingEvSt) ) &&
                        manualClickOption ) {
                      requestOptionMenu()
                      requestOption()
                  } else {
                      requestPageMarker(defaultStartIndex)
                      setSelectList(listView.selectedIndex, listView.selectedIndex)
                  }
             } else {
                  initOption()
                  if ( CommonUtils.isEqual(searchRoot.kind , categoryEnum.GasSt)      ||
                       CommonUtils.isEqual(searchRoot.kind , categoryEnum.ChargingSt) ||
                       CommonUtils.isEqual(searchRoot.kind , categoryEnum.ChargingEvSt)) {
                    requestOptionMenu()
                    autoClickOption = true
                    requestOption()
                  } else {
                    requestSearch(searchRoot.query, searchRoot.order, defaultStartIndex, getReqMaxCount())
                  }
             }
             isActivated = true

              if (searchRoot.visible) {
                 if (CommonUtils.isEqual(searchRoot.kind , categoryEnum.GasSt)       ||
                     CommonUtils.isEqual(searchRoot.kind , categoryEnum.ChargingSt)  ||
                     CommonUtils.isEqual(searchRoot.kind , categoryEnum.ChargingEvSt)) {
                    switch(zone1SearchType) {
                     case searchTypeEnum.AlongRoute:
                        zone1Title = textArr[textEnum.Zone1Title]
                                   + textArr[textEnum.Space]
                                   + "("
                                   + CommonUtils.string(searchTypeArr[searchTypeEnum.AlongRoute]).toLowerCase()
                                   + ")"
                        break
                     case searchTypeEnum.WithinRadius:
                        zone1Title = textArr[textEnum.Zone1Title]
                                    + textArr[textEnum.Space]
                                    + "("
                                    + CommonUtils.string(searchTypeArr[searchTypeEnum.WithinRadius]).toLowerCase()
                                    + ")"
                        break
                     default:
                        zone1Title = textArr[textEnum.Zone1Title]
                        break
                    }

                    if (CommonUtils.isEqual(searchRoot.kind , categoryEnum.ChargingEvSt)) {
                        data = ({})
                        data[ZoneConstants.OptionMenu] = optionMenus
                        sendEvent(SearchResultViewEvent.Activated, data)
                    } else {
                        data = ({})
                        data[ZoneConstants.OptionMenu] = optionMenus
                        sendEvent(SearchResultViewEvent.SelectSoftkeyIndexMenuMode, data)

                        data = ({})
                        data[ZoneConstants.Menu    ] = [softKeyMenuArr[softKeyEnum.DistanceOrder], softKeyMenuArr[softKeyEnum.PriceOrder]]
                        data[CommonConstants.Select] = (CommonUtils.isEqual(searchRoot.order, SearchResultViewEvent.DistanceOrder) ? 0 : 1)
                        sendEvent(SearchResultViewEvent.Activated, data)
                    }
                 } else {
                     zone1Title = textArr[textEnum.Zone1Title]
                     sendEvent(SearchResultViewEvent.SelectSoftkeyIndexMenuMode, [CommonConstants.NoOption])

                     data = ({})
                     data[ZoneConstants.Menu    ] = [softKeyMenuArr[softKeyEnum.DistanceOrder], softKeyMenuArr[softKeyEnum.NameOrder]]
                     data[CommonConstants.Select] = (CommonUtils.isEqual(searchRoot.order, SearchResultViewEvent.DistanceOrder) ? 0 : 1)
                     sendEvent(SearchResultViewEvent.Activated, data)
                 }
                 modeChange()
              }

          } else {
              if(isActivated) {
                  requestPageMarker(defaultStartIndex)
                  setSelectList(listView.selectedIndex, listView.selectedIndex)
              } else {
                  if (isSearchType(CommonConstants.Industry))
                      requestSearch(searchRoot.query, SearchResultViewEvent.DistanceOrder, defaultStartIndex, getReqMaxCount())
                  else
                      requestSearch(searchRoot.query, SearchResultViewEvent.AlphabeticalOrder, defaultStartIndex, getReqMaxCount())
              }
              isActivated = true

              if (searchRoot.visible) {
                  if (viaSetting                            ||
                      isSearchType(CommonConstants.Address) ||
                      isSearchType(CommonConstants.Industry)||
                      CommonUtils.isIndexOf(relay,  [CommonConstants.objectName(ViewRegistry.BookmarkPageView)])) {
                      zone1Title = textArr[textEnum.Zone1Title]
                  } else {
                      zone1Title = CommonConstants.Empty
                      if (CommonUtils.isNull(searchRoot.query)) {
                        zone1Edit = CommonConstants.Empty
                      } else {
                        zone1Edit = searchRoot.query
                      }
                  }

                  if (isSearchType(CommonConstants.Address)) {
                      sendEvent(SearchResultViewEvent.SelectSoftkeyIndexMenuMode, [CommonConstants.NoOption])
                  } else {
                      data = ({})
                      data[ZoneConstants.OptionMenu] = [menuArr[menuEnum.Region]]
                      sendEvent(SearchResultViewEvent.SelectSoftkeyIndexMenuMode, data)
                  }

                  data = ({})
                  if (isSearchType(CommonConstants.Industry)) {
                      data[ZoneConstants.Menu    ] = [softKeyMenuArr[softKeyEnum.DistanceOrder], softKeyMenuArr[softKeyEnum.NameOrder]]
                      data[CommonConstants.Select] = (CommonUtils.isEqual(searchRoot.order, SearchResultViewEvent.DistanceOrder) ? 0 : 1)
                  } else {
                      data[ZoneConstants.Menu    ] = [softKeyMenuArr[softKeyEnum.AlphabeticalOrder],  softKeyMenuArr[softKeyEnum.DistanceOrder]]
                      data[CommonConstants.Select] = (CommonUtils.isEqual(searchRoot.order, SearchResultViewEvent.AlphabeticalOrder) ? 0 : 1)
                  }
                  sendEvent(SearchResultViewEvent.Activated, data )
                  modeChange()
              }
          }
      }

      function destruct() {
          timerQueue = CommonUtils.destroy(timerQueue)
          timerEvDetailInfo = CommonUtils.destroy(timerEvDetailInfo)
          timerCheckList = CommonUtils.destroy(timerCheckList)
          timerHideProg = CommonUtils.destroy(timerHideProg)
          timerSort = CommonUtils.destroy(timerSort)
          sendEvent(SearchResultViewEvent.RemoveSearchQueue)
          requestRemoveAllMarker()
          hideProgressView()
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


      function sendUpdateSearchResultCount(search, totalCount) {
          if (CommonUtils.isNull(search) ||
              CommonUtils.isNull(totalCount)) {
              return
          }
          if (viaSetting                            ||
              isSearchType(CommonConstants.Around)  ||
              isSearchType(CommonConstants.Address) ||
              isSearchType(CommonConstants.Industry)||
              CommonUtils.isIndexOf(relay,  [CommonConstants.objectName(ViewRegistry.BookmarkPageView)])) {
              return
          }
          var data = ({})
          data[CommonConstants.Text] = search
          data[CommonConstants.Count] = updateCount = totalCount
          sendEvent(SearchResultViewEvent.UpdateSearchResultCount, data)
      }

      function initZone1() {
          zone1Title   = null
          zone1Edit    = null
          applyZone1Title = null
          applyZone1Edit = null
      }

      function sendNotiZone1Tile() {
          if (!visible)
              return
          if (CommonUtils.isEmpty(zone1Title))
              return
          if (CommonUtils.isEqual(applyZone1Title, zone1Title))
              return
          applyZone1Edit = null
          applyZone1Title = zone1Title
          sendEvent(SearchResultViewEvent.SelectZone1Title, zone1Title)
      }

      function sendNotiZone1Edit() {
          if (!visible)
              return
          if (CommonUtils.isNull(zone1Edit))
              return
          if (CommonUtils.isEqual(applyZone1Edit, zone1Edit))
              return
          applyZone1Title = null
          applyZone1Edit = zone1Edit
          var data = ({})
          data[CommonConstants.Text] = zone1Edit
          sendEvent(SearchResultViewEvent.SelectZone1TextInput, data)
      }

      function activate() {
          if (!visible) {
              return
          }
          isClosed = false
          loadMainComponent()
          sendEvent(SearchResultViewEvent.ResetCsw)
          sendEvent(SearchResultViewEvent.RegistCsw, [ csw ])
          if (switchedLine) {
              switchedLine = false
          }
          requestNrefresh()
          //내비->가젯->내비 진입시에 맵의 크기 변경
          if (CommonUtils.isEqual(listViewHeight, frameHeight)) {
               viewSizeChanged(frameHeight)
          }
      }

      onZone1TitleChanged: {
          sendNotiZone1Tile()
      }

      onZone1EditChanged: {
          sendNotiZone1Edit()
      }

      onVisibleChanged: {
          if (!visible)
              return
          sendNotiZone1Tile()
          sendNotiZone1Edit()
      }

      onReFocusChanged: {
          if (!reFocus)
              return
          reFocus = false
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
          oldOrder = null
          initZone1()
          csw.forceLostFocus()
          sendEvent(SearchResultViewEvent.DeActivated)
      }

      Component.onDestruction: {
          destruct()
      }

      //지역설정 검색일 때
      onParamsChanged: {
          if (isClosed) {
              return
          }
          if (CommonUtils.isNull(params)) {
              return
          }
          if (isRegionSearch) {
             //재검색하도록
             isActivated = false
             requestNrefresh()
          }
      }

      onIsActivatedChanged: {
          if (!isActivated) {
              oldOrder = null
          }
      }

      onManualClickOptionChanged: {
          if (manualClickOption) {
              oldOrder = null
          }
      }

      onAutoClickOptionChanged: {
          if (autoClickOption) {
              oldOrder = null
          }
      }

      function onInit() {
          console.log("[Perf][[SearchResultView][onInit]")
          isClosed = false
          if (!isMediated) {
              isMediated = true
              sendEvent(SearchResultViewEvent.RequestLang)
              sendEvent(SearchResultViewEvent.QueryOnlineStatus)
              sendEvent(SearchResultViewEvent.RequestAddressType)
              sendEvent(SearchResultViewEvent.RequestRouteGuidanceState)
              init()
          }
          if (!searchRoot.visible) {
              if (CommonUtils.isEmpty(searchRoot.searchType)) {
                  closeView()
              } else {
                isActivated = false
                requestNrefresh()
              }
          }
      }

      function __onResultViewList(result) {
          if (isClosed) {
              return
          }
          for(var i= 0; i<result.length; i++) {
              if (CommonUtils.isIndexOf(result[i][CommonConstants.ResultViewName], [CommonConstants.objectName(ViewRegistry.SearchResultDetailView)])) {
                  isShowTopView = result[i][CommonConstants.ResultViewVisible]
                  return
              }
          }
          isShowTopView = false
      }


      //언어가 변경되었을 때
      function __retranslateUi(lang) {
        searchRoot.__lang = lang
        init()
        var switchedLine = searchRoot.switchedLine
        if(searchRoot.visible) {
            var selectIndex = listView.selectedIndex
            var contentX = listView.contentX
            var contentY = listView.contentY
            activate()
            if (!switchedLine) {
                refreshView(selectIndex, contentX, contentY)
            }
        }
      }

      function __changedOnlineStatus(online) {
          searchRoot.online = online
          switchedLine = false
          if (CommonUtils.isEqual(searchType , CommonConstants.Industry)) {
              return
          }
          if (!CommonUtils.isNull(searchRoot.result) &&
               searchRoot.result.length > 0) {
            if (!CommonUtils.isEqual(online ,
                searchRoot.result[0][SearchConstants.ResultOnline])) {
                switchedLine = true
            }
          }
      }

      function __changedRgState(isRg) {
          searchRoot.isRg = isRg
      }

      function __doSoftKeyFunction(button, menuName, menuIndex) {
          var data = ({})
          objectNameOfSetting = CommonConstants.Empty
          switch(button) {
            case softKeyMenuArr[softKeyEnum.Back]:
                back()
                break
            case softKeyMenuArr[softKeyEnum.Option]:
                break
            case softKeyMenuArr[softKeyEnum.Navi]:
                break
            case softKeyMenuArr[softKeyEnum.Menu]:
                if (isSearchType(CommonConstants.Around))  {
                    var totalCount = listView.totalCount
                    switch (menuName) {
                        case softKeyMenuArr[softKeyEnum.DistanceOrder]:
                            searchRoot.oldOrder = searchRoot.order
                            searchRoot.order = SearchResultViewEvent.DistanceOrder
                            if (CommonUtils.isEqual(searchRoot.kind , categoryEnum.GasSt)       ||
                                CommonUtils.isEqual(searchRoot.kind , categoryEnum.ChargingSt)  ||
                                CommonUtils.isEqual(searchRoot.kind , categoryEnum.ChargingEvSt)) {
                                searchRoot.result = getResultByOilBrand(oilBrand, defaultStartIndex)
                            }
                            if (!doSort()) {
                                requestSearch(searchRoot.query, searchRoot.order, defaultStartIndex, getReqMaxCount())
                            }
                           break;
                        case softKeyMenuArr[softKeyEnum.PriceOrder]:
                            searchRoot.oldOrder = searchRoot.order
                            searchRoot.order = SearchResultViewEvent.PriceOrder
                            if (CommonUtils.isEqual(searchRoot.kind , categoryEnum.GasSt)       ||
                                CommonUtils.isEqual(searchRoot.kind , categoryEnum.ChargingSt)  ||
                                CommonUtils.isEqual(searchRoot.kind , categoryEnum.ChargingEvSt)) {
                                searchRoot.result = getResultByOilBrand(oilBrand, defaultStartIndex)
                            }
                            if (!doSort()) {
                                requestSearch(searchRoot.query, searchRoot.order, defaultStartIndex, getReqMaxCount())
                            }
                            break
                        case softKeyMenuArr[softKeyEnum.NameOrder]:
                            searchRoot.oldOrder = searchRoot.order
                            searchRoot.order = SearchResultViewEvent.NameOrder
                            if (CommonUtils.isEqual(searchRoot.kind , categoryEnum.GasSt)       ||
                                CommonUtils.isEqual(searchRoot.kind , categoryEnum.ChargingSt)  ||
                                CommonUtils.isEqual(searchRoot.kind , categoryEnum.ChargingEvSt)) {
                                searchRoot.result =  getResultByOilBrand(oilBrand, defaultStartIndex)
                            }
                            if (!doSort()) {
                                requestSearch(searchRoot.query, searchRoot.order, defaultStartIndex, getReqMaxCount())
                            }
                            break
                        //옵션>반경내
                        case searchTypeArr[searchTypeEnum.WithinRadiusKo]:
                            searchRoot.searchTypeButtonClicked(SettingConstants.SearchTypeWithinRadius)
                            break
                        //옵션>경로상
                        case searchTypeArr[searchTypeEnum.AlongRouteKo]:
                            searchRoot.searchTypeButtonClicked(SettingConstants.SearchTypeAlongRoute)
                            break
                        default:
                            //옵션>유종
                            if (CommonUtils.isIndexOf(menuName, menuArr[menuEnum.OilKind])) {
                                searchRoot.oilKindButtonClicked()
                            }
                            //옵션>브랜드
                            else if (CommonUtils.isIndexOf(menuName, menuArr[menuEnum.OilBrand])) {
                                searchRoot.oilBrandButtonClicked()
                            }
                            //옵션>충전타입
                            else if (CommonUtils.isIndexOf(menuName, menuArr[menuEnum.ChargeType])) {
                                searchRoot.chargeTypeButtonClicked()
                            }
                            break
                    }//switch(menuName)
                 } else {
                    switch (menuName) {
                        case softKeyMenuArr[softKeyEnum.AlphabeticalOrder]:
                            searchRoot.oldOrder = searchRoot.order
                            searchRoot.order = SearchResultViewEvent.AlphabeticalOrder
                            requestSearch( searchRoot.query, searchRoot.order, defaultStartIndex, getReqMaxCount())
                            break
                        case softKeyMenuArr[softKeyEnum.DistanceOrder]:
                            searchRoot.oldOrder = searchRoot.order
                            searchRoot.order = SearchResultViewEvent.DistanceOrder
                            if (!doSort()) {
                                requestSearch( searchRoot.query, searchRoot.order, defaultStartIndex, getReqMaxCount())
                            }
                            break
                        case softKeyMenuArr[softKeyEnum.NameOrder]:
                            searchRoot.oldOrder = searchRoot.order
                            searchRoot.order = SearchResultViewEvent.NameOrder
                            if (!doSort()) {
                                requestSearch( searchRoot.query, searchRoot.order, defaultStartIndex, getReqMaxCount())
                            }
                            break
                        //옵션>지역
                        case menuArr[menuEnum.Region]:
                            data[CommonConstants.Url        ] = CommonConstants.url(ViewRegistry.RegionSearchView)
                            data[CommonConstants.Caller     ] = caller
                            data[CommonConstants.Relay      ] = owner
                            data[CommonConstants.Owner      ] = CommonConstants.objectName(ViewRegistry.SearchResultView)
                            data[SearchConstants.SearchType ] = searchType
                            //data[CommonConstants.Z          ] = CommonConstants.MAX_INT_VALUE
                            sendEvent(SearchResultViewEvent.CreateView, data)
                            isRegionSearch = true
                            break
                    }//switch(menuName)
                 }
                break
          }//switch
      }

    ///////////////////////////////////////////////
    //{전기차 부가정보를 동적으로 처리하기 위하여...
      function requestEvDetailInfoByScroll(delayMs) {
          if (!CommonUtils.isEqual(searchRoot.kind, categoryEnum.ChargingEvSt)) {
              return
          }
          if (result.length <= 0)
             return
          timerEvDetailInfo = CommonUtils.destroy(timerEvDetailInfo)
          timerEvDetailInfo = CommonUtils.delay(searchRoot, delayMs, function() {
              try {
                  if (!CommonUtils.isNull(scrollbar) &&
                      scrollbar.visible) {
                      timerEvDetailInfo.interval = 200
                      timerEvDetailInfo.start()
                      return
                  }

                  var item = null
                  var i = null
                  var startListIdx = listView.indexAt(listView.contentX, listView.contentY )
                  var endListIdx   = listView.indexAt(listView.contentX, listView.contentY + listView.height )
                  if (startListIdx < 0) {
                      startListIdx = 0
                  }
                  if (endListIdx < 0) {
                      endListIdx = listModel.count - 1
                  }
                  startListIndex = startListIdx
                  endListIndex = endListIdx

                  for (i=startListIndex; i <= endListIndex; i++) {
                      if (i < 0 || listModel.count <= i)
                        continue
                      item = listModel.get(i)
                      if (CommonUtils.isNull(item))
                          continue
                      if (resultEvDataSync &&
                          searchRoot.result[item.idx][SearchConstants.ResultOnline] &&
                          !searchRoot.result[item.idx][SearchConstants.ResultAlongRoute]) {
                          timerEvDetailInfo = CommonUtils.destroy(timerEvDetailInfo)
                          return
                      }

                      if ((!item.request &&
                           item.visible &&
                           item.height   )||
                          (item.request < reqEvDetailRetryMax &&
                           !item.response   &&
                           item.visible &&
                           item.height &&
                           elapseTimeEvDetail(i) > (SearchConstants.NetworkTimeoutAPP)) ) {
                          requestEvDetailByVisible(item)
                      }

                      if (item.request &&
                          item.response &&
                          CommonUtils.isEqual(item.key, -1) &&
                          !isShowProgressIndicator) {
                          listModel.setProperty(i, CommonConstants.Key, -2)
                          checkListEvItem(0)
                      }
                  }
                  timerEvDetailInfo.interval = 200
                  timerEvDetailInfo.start()
                } catch(e) {
                }
          }, false)
      }


      function requestEvDetailByVisible(item) {
        if (!CommonUtils.isNull(item) &&
               !CommonUtils.isUndefined(typeof searchRoot.result[item.idx])) {
               if(!item.response && item.visible) {
                    var object = searchRoot.result[item.idx][SearchConstants.ResultObject]
                    if (!CommonUtils.isUndefined(typeof object.isChargingEvSt)) {
                        if (object.isChargingEvSt()) {
                           requestEvDetailInfo(item.seq)
                        }
                    }
               }
           }
      }

      onStartListIndexChanged: {
          checkListEvItem(0)
      }

      onEndListIndexChanged: {
          checkListEvItem(0)
      }

      function checkListEvItem(ms) {
          if (isShowDialog()) {
              return
          }
          if (!CommonUtils.isEqual(searchRoot.kind, categoryEnum.ChargingEvSt)) {
              return
          }
          timerHideProg = CommonUtils.destroy(timerHideProg)
          timerCheckList = CommonUtils.destroy(timerCheckList)
          timerCheckList = CommonUtils.delay(searchRoot, ms, function() {
              try {
                  if (!CommonUtils.isNull(scrollbar) &&
                      scrollbar.visible) {
                      timerCheckList.interval = 200
                      timerCheckList.start()
                      return
                  }

                  var i = 0
                  var item = null
                  var needHide = null
                  var startListIdx = listView.indexAt(listView.contentX, listView.contentY)
                  var endListIdx   = listView.indexAt(listView.contentX, listView.contentY + listView.height)
                  if (startListIdx < 0) {
                      startListIdx = 0
                  }
                  if (endListIdx < 0) {
                      endListIdx = listModel.count - 1
                  }
                  startListIndex = startListIdx
                  endListIndex = endListIdx
                  for (i=startListIndex; i <= endListIndex; i++) {
                      if (i < 0 || listModel.count <= i)
                          continue
                      item = listModel.get(i)
                      if (CommonUtils.isNull(item))
                          continue
                      if (resultEvDataSync &&
                          searchRoot.result[item.idx][SearchConstants.ResultOnline] &&
                          !searchRoot.result[item.idx][SearchConstants.ResultAlongRoute]) {
                          timerHideProg = CommonUtils.destroy(timerHideProg)
                          timerCheckList = CommonUtils.destroy(timerCheckList)
                          return
                      }

                      if (CommonUtils.isNull(item) ||
                          CommonUtils.isUndefined(typeof item.request) ||
                          CommonUtils.isUndefined(typeof item.response) ||
                          (item.request < reqEvDetailRetryMax &&
                           !item.response &&
                           item.visible &&
                           item.height)) {
                          needHide = false
                          break
                      } else {
                          needHide = true
                      }
                  }

                  if (needHide &&
                      queueCount <= 0) {
                      //2019.03.19 화면 깜박거림을 줄이기 위하여 타이머를 사용
                      timerHideProg = CommonUtils.destroy(timerHideProg)
                      timerHideProg  = CommonUtils.delay(searchRoot, 500*4, function() {
                        try{
                            hideProgressView()
                            hideShowByEvData()
                            resetEvIndex()
                        } catch(e) {
                        }
                      })
                  }
              } catch(e) {
              }
          }, false)
      }

      function resetEvIndex() {
          if (!active) {
              return
          }
          if (!CommonUtils.isEqual(searchRoot.kind, categoryEnum.ChargingEvSt)) {
              return
          }

          var i=0
          var j=0
          var item = null
          var currIndex= null
          var removeAllmarker = false
          var isFirstIndex = false
          for (i=0; i<listModel.count;i++) {
            item = listModel.get(i)
            if (CommonUtils.isNull(item))
                continue
            if (resultEvDataSync &&
                searchRoot.result[item.idx][SearchConstants.ResultOnline] &&
                !searchRoot.result[item.idx][SearchConstants.ResultAlongRoute])
                return
            if (item.request &&
                item.response &&
                item.height) {
                listModel.setProperty(i, CommonConstants.Key, j++)
                requestAddMarker(i, -1)
                if (!listView.manualClick && !isFirstIndex) {
                    isFirstIndex = true
                    var index = item.seq
                    if (index >= 0) {
                        if (!CommonUtils.isEqual(index, listView.firstIndex)) {
                            resetCurrentList(index)
                            listView.selectedIndex = index
                            listView.firstIndex = index
                        }
                    }
                }
            } else {
                if (item.height <= 0) {
                    requestRemoveMarker(i)
                }
            }
          }
          setSelectList(listView.selectedIndex, listView.selectedIndex)
          if (searchRoot.focused &&
              csw.reqSpinCw) {
            csw.spinCwPress(true)
          }
      }

    function elapseTimeEvDetail(index) {
        return CommonUtils.getTime() - verifyReqEvDetail[CommonUtils.string(index)]
    }
    //}전기차 상세를 위하여 ....
    ///////////////////////////////////////////////


      //main
      Loader {
           id: loader
           width: parent.width
           height:parent.height
      }

      Component {
          id: main
          Item {
              onParentChanged: {
                  if (CommonUtils.isNull(parent))
                      return
                  width = parent.width
                  height = parent.height
              }
              Column {
                  width: parent.width
                  height: parent.height
                  Item {
                     width: parent.width
                     height: parent.height - listViewHeight
                  }
                  Rectangle{
                      width: parent.width
                      height: listViewHeight
                      color: GlobalConstants.kBasicBackgoundColor
                      ListView {
                          Component.onCompleted: searchRoot.listView = this
                          width: parent.width
                          height: messageVisible? 0 : parent.height
                          property int rowHeight: searchRoot.layout.rowHeight
                          property int maxCount: SearchConstants.RequestCount
                          property int firstIndex: 0
                          property int totalPage: 0
                          property int totalCount: 0
                          property int requestPageIndex: 0

                          property int pressedIndex: -1
                          property int selectedIndex: -1
                          property int markerFocusedIndex: -1
                          property var pressedItem: null
                          property var selectedItem: null

                          property bool manualClick: false

                          signal scrollEnd()

                          function init() {
                              contentHeight = 0
                              contentY = 0
                              firstIndex = 0
                              totalPage = 0
                              totalCount = 0
                              requestPageIndex = 0
                              pressedIndex = -1
                              selectedIndex = -1
                              currentIndex = -1
                              markerFocusedIndex = -1
                              pressedItem = null
                              selectedItem = null
                              manualClick = false
                          }

                          function nextPage() {
                              if (CommonUtils.isNull(listView)                ||
                                  CommonUtils.isNull(listView.totalPage)      ||
                                  CommonUtils.isNull(listView.requestPageIndex) ) {
                                  return
                              }
                              if ( (listView.totalPage - 1) <= listView.requestPageIndex) {
                                  return
                              }
                              if (CommonUtils.isEqual(listView.maxCount , SearchConstants.RequestMaxCount)) {
                                  console.log("[Perf][SearchResultView][nextPage][createModel]("+ (listView.requestPageIndex+1)+"/"+(listView.totalPage - 1)+")")
                                  createModel(oilSearchType, searchRoot.result, ++listView.requestPageIndex * SearchConstants.RequestCount, listView.totalCount, 0)
                                  return
                              }
                              console.log("[Perf][SearchResultView][nextPage][requestSearch]("+ (listView.requestPageIndex+1)+"/"+(listView.totalPage - 1)+")")
                              requestSearch(searchRoot.query, searchRoot.order,  ++listView.requestPageIndex, getReqMaxCount())
                          }

                          PressBar {
                              currentItem: listView.pressedItem
                              leftMargin: GlobalConstants.kListBackgroundLeftMargin
                              rightMargin: GlobalConstants.kListBackgroundRightMargin
                          }
                          SelectedBar {
                              currentItem: listView.selectedItem
                              leftMargin: GlobalConstants.kListBackgroundLeftMargin
                              rightMargin: GlobalConstants.kListBackgroundRightMargin
                              color: {
                                  if (searchRoot.focused) {
                                      if (CommonUtils.isEqual(listView.currentItem, listView.selectedItem))
                                          return CommonConstants.Transparent
                                  }
                                  return MultiSenseCoordinator.kAmbientColor
                              }
                          }
                          highlight: HighlightBar {
                              currentItem: listView.currentItem
                              leftMargin: GlobalConstants.kListBackgroundLeftMargin
                              rightMargin: GlobalConstants.kListBackgroundRightMargin
                              color: {
                                   if (searchRoot.focused) {
                                       var item = listView.currentItem
                                       if (item.isButtonFocused)
                                           return CommonConstants.Transparent
                                       return GlobalConstants.kListFocusColor
                                   }
                                   if (CommonUtils.isEqual(listView.currentItem, listView.selectedItem))
                                      return MultiSenseCoordinator.kAmbientColor
                                   else
                                      return CommonConstants.Transparent
                              }
                          }
                          highlightFollowsCurrentItem: false
                          cacheBuffer: rowHeight * 3
                          clip: true

                          model:  listModel
                          delegate: listDelegate
                          visible: !messageVisible

                          Image {
                                id: imgMask
                                width: parent.width
                                height: 30
                                y: parent.y + (parent.height - height)
                                visible: {
                                    if (CommonUtils.isNull(scrollbar) ||
                                        CommonUtils.isNull(listView)) {
                                        return false
                                    }
                                    return scrollbar.heightRatio < 1.0 && !listView.dragging
                                }
                                sourceSize.width: width
                                sourceSize.height: height
                                source: Resource.imageUrl(imgArr[imgEnum.Zone2Mask])
                          }

                          TScrollBar {
                              Component.onCompleted: searchRoot.scrollbar = this
                              flickable: parent
                              dynamicPage: listView.requestPageIndex
                              width: 2
                              color: MultiSenseCoordinator.kAmbientColor
                              grooveColor: GlobalConstants.kScrollBarBackColor
                              rightMargin: GlobalConstants.kScrollBarRightMargin
                          }

                          //스크롤 끝: 리스트 항목의 높이를 동적으로 변경하지 않는 경우 사용
                          onAtYEndChanged: {
                              if (CommonUtils.isNull(scrollbar)) {
                                  return
                              }
                              if (!scrollbar.visible) {
                                  return
                              }
                              if (!atYEnd) {
                                   return
                              }
                              cancelFlick()
                              if (CommonUtils.isEqual(searchRoot.kind , categoryEnum.ChargingEvSt)) {
                                 if (!resultEvDataSync || !getResultOnline()) {
                                    return
                                 }
                              }
                              console.log("[Perf][SearchResultView][onAtYEndChanged]before")
                              nextPage()
                              console.log("[Perf][SearchResultView][onAtYEndChanged]after")
                          }

                          //스크롤 끝: 리스트 항목의 높이를 동적으로 변경하는 경우 사용
                          onScrollEnd: {
                              if (CommonUtils.isEqual(searchRoot.kind , categoryEnum.ChargingEvSt)) {
                                    if (!resultEvDataSync || !getResultOnline()) {
                                        cancelFlick()
                                        console.log("[Perf][SearchResultView][onScrollEnd]before")
                                        nextPage()
                                        console.log("[Perf][SearchResultView][onScrollEnd]after")
                                    }
                              }
                          }

                          property int oldStartListIdx: 0
                          property real oldContentY: 0
                          onContentYChanged: {
                              if (CommonUtils.isEqual(searchRoot.kind, categoryEnum.ChargingEvSt)) {
                                  if (!resultEvDataSync || !getResultOnline()) {
                                      var startListIdx = listView.indexAt(listView.contentX, listView.contentY )
                                      var endListIdx   = listView.indexAt(listView.contentX, listView.contentY + listView.height )
                                      if (startListIdx < 0 && endListIdx < 0) {
                                          positionViewAtIndex(oldStartListIdx, ListView.Beginning)
                                          contentY = oldContentY
                                      }
                                      if (startListIdx >= 0) {
                                          oldStartListIdx = startListIdx
                                          oldContentY = contentY
                                      }
                                  }
                              }
                          }
                      }//ListView
                      Item { width:parent.width;
                             height: messageVisible ? parent.height : 0
                          Text {
                              Component.onCompleted: searchRoot.message = this
                              text: messageText
                              visible: messageVisible
                              anchors.centerIn:  parent
                              font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(GlobalConstants.kGeneralMessageFontSize) }
                              color: GlobalConstants.kListTextColor
                          }
                      }
                  }
              }
        }
    }


      Component {
          id: listDelegate
          Column {
              width: listView.width
              height: model.height
              visible: (model.height > 0)

              property var  delegateItemObj: this
              property var  modelObj: model
              property var styleLoader: null
              property var mainTitle: null
              property bool isPressed:  !CommonUtils.isEqual(listView.currentIndex , model.seq) &&
                                         CommonUtils.isEqual(listView.pressedIndex , model.seq)
              property bool isSelected: CommonUtils.isEqual(listView.selectedIndex , model.seq)
              property bool isFocused: CommonUtils.isEqual(listView.currentIndex , model.seq) && searchRoot.focused
              property bool isButtonFocused: false
              property alias buttonVisible: btnId.visible


              //Normal
              property color poiTxtColor       : GlobalConstants.kListTextColor
              property color mainTxtColor      : GlobalConstants.kListTextColor
              property color subTxtColor       : GlobalConstants.kListSubTextColor
              property color distanceTxtColor  : GlobalConstants.kPressedColor
              property color priceTxtColor     : GlobalConstants.kListTextColor
              property color fuelTxtColor      : GlobalConstants.kListTextColor
              property color fuelRctColor      : CommonConstants.Gray
              property color minimumTxtColor   : GlobalConstants.kListTextColor
              property color minimumRctColor   : GlobalConstants.kListTextColor
              property color btnTxtColor       : GlobalConstants.kListTextColor
              property string btnNormalImgName : imgArr[imgEnum.BaseBtnNormalName]
              property string btnPressImgName  : imgArr[imgEnum.BaseBtnPressName]
              property string directionImgName : getDirectionImgName(imgArr[imgEnum.Normal])
              property string poiListImgName   : imgArr[imgEnum.PoiListG]

              property int offsetY: Math.round((y+height) - listView.contentY)
              property bool isVisible: (offsetY >= listView.y && (offsetY-height) < listView.y + listView.height)

              function setObject() {
                  if (CommonUtils.isUndefined(typeof model)) {
                    return
                  }

                  //스크롤 끝: 리스트 항목의 높이를 동적으로 변경하는 경우 사용
                  if (CommonUtils.isEqual(model.seq, listModel.count-1)) {
                      if (isVisible) {
                          console.log("[Perf][SearchResultView][listDelegate] Next page scrollEnd() signal["+model.seq+"]="+model.name)
                          listView.scrollEnd()
                      }
                  }

                  listModel.setProperty(model.seq, CommonConstants.Visible, isVisible)
                  if (CommonUtils.isUndefined(typeof searchRoot.result[model.idx]))
                      return
                  searchRoot.result[model.idx][SearchConstants.ResultObject] = delegateItemObj
              }

              onModelObjChanged: setObject()
              onIsVisibleChanged: setObject()
              Component.onCompleted: setObject()


              function getDirectionImgName(state) {
                  var imgName  = CommonConstants.Empty
                  if (CommonUtils.isNull(model) || CommonUtils.isNull(model.angle))
                      return imgName
                  var index = parseInt((model.angle)/10)
                  index     = (index%36)+1
                  imgName   = imgArr[imgEnum.Direction]
                  imgName  += imgArr[imgEnum.Underbar]
                  imgName  += CommonUtils.zeroPadding(index, 2)
                  imgName  += imgArr[imgEnum.Underbar]
                  imgName  += state
                  imgName  += imgArr[imgEnum.Dot]
                  imgName  += imgArr[imgEnum.Png]
                  return imgName
              }

              function execButton() {
                  btnId.execButton(GlobalConstants.kPressedColorExposureTime)
              }

              function selectItem () {
                  var oldIndex = listView.selectedIndex
                  var index = model.seq
                  listView.manualClick = true
                  setCurrentList(index)
                  setSelectList(oldIndex, index)
              }

              function destItem() {
                  var index = model.seq
                  listView.manualClick = true
                  moveDestination(index)
              }

              function clickItem(){
                  if (!isSelected) {
                    selectItem()
                  } else {
                    destItem()
                  }
              }

              function normalState() {
                  poiTxtColor       = GlobalConstants.kListTextColor
                  mainTxtColor      = GlobalConstants.kListTextColor
                  subTxtColor       = GlobalConstants.kListSubTextColor
                  distanceTxtColor  = GlobalConstants.kPressedColor
                  priceTxtColor     = GlobalConstants.kListTextColor
                  fuelTxtColor      = GlobalConstants.kListTextColor
                  fuelRctColor      = CommonConstants.Gray
                  minimumTxtColor   = GlobalConstants.kListTextColor
                  minimumRctColor   = GlobalConstants.kListTextColor
                  btnTxtColor       = GlobalConstants.kListTextColor
                  btnNormalImgName  = imgArr[imgEnum.BaseBtnNormalName]
                  btnPressImgName   = imgArr[imgEnum.BaseBtnPressName]
                  directionImgName  = getDirectionImgName(imgArr[imgEnum.Normal])
                  poiListImgName    = imgArr[imgEnum.PoiListG]
              }

              function focusState() {
                  poiTxtColor       = GlobalConstants.kBasicBackgoundColor
                  mainTxtColor      = GlobalConstants.kBasicBackgoundColor
                  subTxtColor       = GlobalConstants.kListSubTextColor
                  distanceTxtColor  = GlobalConstants.kPressedColor
                  priceTxtColor     = GlobalConstants.kBasicBackgoundColor
                  fuelTxtColor      = GlobalConstants.kBasicBackgoundColor
                  fuelRctColor      = CommonConstants.Gray
                  minimumTxtColor   = GlobalConstants.kBasicBackgoundColor
                  minimumRctColor   = GlobalConstants.kBasicBackgoundColor
                  btnTxtColor       = Qt.binding(function(){ return btnId.pressed ? GlobalConstants.kBasicBackgoundColor : GlobalConstants.kListTextColor})
                  btnNormalImgName  = imgArr[imgEnum.BaseBtnPressName]
                  btnPressImgName   = imgArr[imgEnum.BaseBtnPressName]
                  directionImgName  = getDirectionImgName(imgArr[imgEnum.Focus])
                  poiListImgName    = imgArr[imgEnum.PoiListB]
              }

              function selectState() {
                  poiTxtColor       = GlobalConstants.kListTextColor
                  mainTxtColor      = GlobalConstants.kListTextColor
                  subTxtColor       = GlobalConstants.kListTextColor
                  distanceTxtColor  = GlobalConstants.kListTextColor
                  priceTxtColor     = GlobalConstants.kListTextColor
                  fuelTxtColor      = GlobalConstants.kListTextColor
                  fuelRctColor      = CommonConstants.Gray
                  minimumTxtColor   = GlobalConstants.kListTextColor
                  minimumRctColor   = GlobalConstants.kListTextColor
                  btnTxtColor       = GlobalConstants.kListTextColor
                  btnNormalImgName  = imgArr[imgEnum.BaseBtnPressName]
                  btnPressImgName   = imgArr[imgEnum.BaseBtnPressName]
                  directionImgName  = getDirectionImgName(imgArr[imgEnum.Focus])
                  poiListImgName    = imgArr[imgEnum.PoiListB]
              }

              function pressedState() {
                  selectState()
              }

              function processState() {
                  if (isFocused) {
                      focusState()
                  } else if (isSelected) {
                      selectState()
                  } else if (isPressed) {
                      pressedState()
                  } else {
                      normalState()
                      isButtonFocused = false
                  }
              }

              onStyleLoaderChanged: {
                    if (CommonUtils.isNull(styleLoader))
                        return

                    var styleType = normalType
                    if(isSearchType(CommonConstants.Around)) {
                        switch(getCategory()) {
                            case categoryEnum.GasSt:
                            case categoryEnum.ChargingSt:
                                styleType = gasChargType
                                break
                            case categoryEnum.ChargingEvSt:
                                styleType = evChargType
                                break
                            default:
                                styleType = normalType
                                break
                        }
                    } else {
                        switch(getCategory()) {
                        case categoryEnum.ChargingEvSt:
                            styleType = evChargType
                            break
                        default:
                            styleType = normalType
                            break
                        }
                    }
                    styleLoader.sourceComponent = styleType
              }

              onIsPressedChanged : processState()
              onIsSelectedChanged: {
                processState()
                if (isSelected) {
                    listView.selectedItem = delegateItemObj
                }
              }
              onIsFocusedChanged : processState()

              onIsButtonFocusedChanged: {
                  if (isButtonFocused) {
                      normalState()
                      btnTxtColor        = GlobalConstants.kBasicBackgoundColor
                      btnNormalImgName   = imgArr[imgEnum.BaseBtnCswFocusName]
                      directionImgName   = imgArr[imgEnum.Direction]
                      directionImgName  += imgArr[imgEnum.Underbar]
                      directionImgName  += CommonUtils.zeroPadding(parseInt((model.angle)/10)+1, 2)
                      directionImgName  += imgArr[imgEnum.Underbar]
                      directionImgName  += imgArr[imgEnum.Normal  ]
                      directionImgName  += imgArr[imgEnum.Dot     ]
                      directionImgName  += imgArr[imgEnum.Png     ]
                      poiListImgName     = imgArr[imgEnum.PoiListG]
                  } else {
                      if (isFocused) {
                          focusState()
                      } else if (isSelected) {
                          selectState()
                      } else if (isPressed) {
                          pressedState()
                      }
                  }
              }

              function isValidPrice(){
                  if (searchRoot.result.length <= model.idx)
                      return false
                  var result = searchRoot.result[model.idx]
                  if (CommonUtils.isNull(result))
                      return false
                  var gasolinePrice     = result[SearchConstants.ResultOPIGasolinePrice]
                  var dieselPrice       = result[SearchConstants.ResultOPIDieselPrice]
                  var lpgPrice          = result[SearchConstants.ResultOPILPGPrice]
                  var highGasolinePrice = result[SearchConstants.ResultOPIHiGasolinePrice]
                  if( (gasolinePrice + gasolinePrice + lpgPrice + highGasolinePrice) > 0 )
                      return true
                  return false
              }

              function getCategory(){
                  if (isSearchType(CommonConstants.Around)) {
                      return searchRoot.kind
                  }
                  return searchRoot.getCategory(model.seq)
              }

              function isChargingEvSt() {
                  if (CommonUtils.isEqual(searchRoot.kind, categoryEnum.ChargingEvSt)) {
                      return true
                  }
                  if (CommonUtils.isEqual(getCategory(), categoryEnum.ChargingEvSt)) {
                      return true
                  }
                  return false
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
                              var currentIndex = currentItem ? model.seq : -1
                              listView.pressedItem = currentItem
                              listView.pressedIndex = currentIndex
                          }
                      }
                      onClicked: clickItem()
                  }
                  Row {
                    width: parent.width
                    height: parent.height
                    Item {width:GlobalConstants.kListTextLeftMargin;height:parent.height}
                    Item {width: 63 ;height:parent.height
                        Item {width:63;height:63;anchors.centerIn: parent
                              Image {
                                  width: parent.width
                                  height: parent.height
                                  source: {
                                      if (isSearchType([CommonConstants.Industry, CommonConstants.Around])) {
                                        return Resource.imageUrl(directionImgName)
                                      } else {
                                          return Resource.imageUrl(poiListImgName)
                                      }
                                  }
                                  sourceSize.width: width
                                  sourceSize.height: height
                                  Row {
                                      y: 19
                                      height: 14
                                      anchors.horizontalCenter: parent.horizontalCenter
                                      visible: isSearchType([CommonConstants.Industry, CommonConstants.Around]) ? false : true
                                      Repeater {
                                          model: isSearchType([CommonConstants.Industry, CommonConstants.Around]) ? 0 : CommonUtils.string(modelObj.seq + 1).length
                                          delegate:
                                              Image {
                                                  width: CommonUtils.string(modelObj.seq + 1).length >=4 ? 8 : 10
                                                  height: 14
                                                  source: {
                                                      var n = CommonUtils.string(modelObj.seq + 1)[index]
                                                      var tail = CommonUtils.string(modelObj.seq + 1).length>=4 ? "_s2" : "_s"
                                                      tail += imgArr[imgEnum.Dot]
                                                      tail += imgArr[imgEnum.Png]
                                                      if (!CommonUtils.isAlpaNum(n)) {
                                                          return CommonConstants.Empty
                                                      }
                                                      var path = "poi_num_"+n+tail
                                                      return Resource.imageUrl(path)
                                                  }
                                              }
                                      }
                                  }
                              }
                        }
                    }
                    Item {width: 20;height:parent.height}
                    Item {width: parent.width -(GlobalConstants.kListTextLeftMargin+63+20+30+88+GlobalConstants.kListTextRightMargin);height:parent.height
                        Loader {
                           Component.onCompleted:  styleLoader = this
                           width: parent.width
                           height:parent.height

                           onChildrenChanged: {
                                if (children.length <= 0)
                                    return
                                children[0].delegateItemObj = delegateItemObj
                           }
                        }
                    }
                    Item {width: 30;height:parent.height}
                    Item {width: 88;height:parent.height
                      Item {width:parent.width;height:60;  anchors.centerIn: parent
                            TPushButton {
                                id: btnId
                                property bool pressed: false
                                width:  searchRoot.layout.detailBtnWidth
                                height:  searchRoot.layout.detailBtnHeight
                                anchors.verticalCenter: parent.verticalCenter
                                visible: {
                                    if (CommonUtils.isIndexOf(caller, [CommonConstants.objectName(ViewRegistry.AddressSearchView)])) {
                                        return false
                                    }
                                    return true
                                }
                                Component.onCompleted: {
                                    box.border.width    = 2
                                    box.radius          = height/2
                                    box.colorN          = Qt.binding(function(){ return isButtonFocused? GlobalConstants.kListFocusColor : CommonConstants.Transparent})
                                    box.colorP          = Qt.binding(function(){ return isButtonFocused? GlobalConstants.kListFocusColor : isFocused ? GlobalConstants.kListFocusColor : MultiSenseCoordinator.kAmbientColor})
                                    box.border.color    = Qt.binding(function(){ return isButtonFocused? GlobalConstants.kListFocusColor : (isSelected || isPressed) ? GlobalConstants.kFocusedColor : MultiSenseCoordinator.kAmbientColor})
                                }

                                Text2 { anchors.fill: parent
                                    font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(30)}
                                    text: textArr[textEnum.Detail]
                                    verticalAlignment: Text.AlignVCenter
                                    horizontalAlignment: Text.AlignHCenter
                                    offsetY: 2
                                    color: btnTxtColor
                                    scale: btnId.txt.scale
                                }

                                function execButton(delay) {
                                    var index = model.seq
                                    if (delay > 0) {
                                        pressed = true
                                        state = "pressed"
                                    }
                                    if (delay > 0) {
                                        pressed = false
                                        state = "normal"
                                    }
                                    showSearchResultDetailView(index)
                                    csw.forceLostFocus()
                                    listView.currentIndex = index
                                    listView.selectedIndex = index
                                }

                                onButtonPressed: {
                                    csw.forceLostFocus()
                                    pressed = true
                                }

                                onButtonReleased:{
                                    pressed = false
                                }

                                onButtonClicked: execButton(0)
                            }
                      }
                    }
                    Item {width:GlobalConstants.kListTextRightMargin;height:parent.height}
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

      //1.일반
      Component {
        id: normalType
        Row {
          property var delegateItemObj:null
          onDelegateItemObjChanged: {
            if (CommonUtils.isNull(delegateItemObj))
                return
          }
          width: parent.width
          height: parent.height
          Item {width: parent.width -(100+40);height:parent.height
              Column {
                  width:parent.width
                  height: (32+12+28)
                  anchors.verticalCenter: parent.verticalCenter
                  Item {width: parent.width; height: 32
                      Loader {
                           width: parent.width
                           height:parent.height
                           sourceComponent: mainTitle
                           onChildrenChanged: {
                               if (children.length <= 0)
                                   return
                                children[0].delegateItemObj = delegateItemObj
                           }
                      }
                  }
                  Item {width: parent.width; height: 12}
                  Item {width: parent.width; height: 28
                      Loader {
                           width: parent.width
                           height:parent.height
                           sourceComponent: subTitle
                           onChildrenChanged: {
                               if (children.length <= 0)
                                   return
                                children[0].delegateItemObj = delegateItemObj
                           }
                      }
                  }
              }
          }
          Item {width: 40;height:parent.height}
          Item {width: 100;height:parent.height
              Item {height:28;width:parent.width; anchors.centerIn: parent
                  Loader {
                       width: parent.width
                       height:parent.height
                       sourceComponent: distanceTitle
                       onChildrenChanged: {
                           if (children.length <= 0)
                               return
                            children[0].delegateItemObj = delegateItemObj
                       }
                  }
              }
          }
        }//Row
      }//Component

      //2.주유소/충전소
      Component {
          id: gasChargType
            Row {
              property var delegateItemObj:null
              onDelegateItemObjChanged: {
                  if (CommonUtils.isNull(delegateItemObj))
                      return
              }
              width: parent.width
              height: parent.height
              Item {width: parent.width -(40+100);height:parent.height;
                  Component.onCompleted: {mainTitleRow.changeWidth(null, null)}
                  Column {width:parent.width
                      height: (32+12+28)
                      anchors.verticalCenter: parent.verticalCenter
                      Item {width: parent.width; height: 32
                          Row {width: parent.width; height:parent.height
                              id: mainTitleRow
                              property var childEnum: ({main:0,left:1,near:2,middle:3,lowest:4,right:5})
                              function isTag() {
                                  var nearItem   = CommonUtils.getItemFromObj(mainTitleRow, childEnum.near)
                                  var lowestItem = CommonUtils.getItemFromObj(mainTitleRow, childEnum.lowest)
                                  if (nearItem.width || lowestItem.width){
                                      return true
                                  }
                                  return false
                              }

                              function changeWidth(item, contentItem) {
                                var mainItem   = CommonUtils.getItemFromObj(mainTitleRow, childEnum.main)
                                var leftItem   = CommonUtils.getItemFromObj(mainTitleRow, childEnum.left)
                                var nearItem   = CommonUtils.getItemFromObj(mainTitleRow, childEnum.near)
                                var middleItem = CommonUtils.getItemFromObj(mainTitleRow, childEnum.middle)
                                var lowestItem = CommonUtils.getItemFromObj(mainTitleRow, childEnum.lowest)
                                var rightItem  = CommonUtils.getItemFromObj(mainTitleRow, childEnum.right)

                                if (CommonUtils.isEqual(item , nearItem)) {
                                    if (CommonUtils.isEqual(searchRoot.order, SearchResultViewEvent.DistanceOrder)) {
                                        if (!CommonUtils.isNull(contentItem.delegateItemObj.modelObj)) {
                                            if (CommonUtils.isEqual(contentItem.delegateItemObj.modelObj.seq, 0)) {
                                                item.width =  70
                                            }
                                        }
                                    } else {
                                        if (!CommonUtils.isNull(modelNearIndex)) {
                                            if (!CommonUtils.isNull(contentItem.delegateItemObj.modelObj)) {
                                                if (CommonUtils.isEqual(contentItem.delegateItemObj.modelObj.seq, modelNearIndex)) {
                                                    item.width =  70
                                                }
                                            }
                                        }
                                    }
                                }
                                if (CommonUtils.isEqual(item , lowestItem)) {
                                    if (CommonUtils.isEqual(searchRoot.order, SearchResultViewEvent.PriceOrder)) {
                                        if (!CommonUtils.isNull(contentItem.delegateItemObj.modelObj)) {
                                            if (CommonUtils.isEqual(contentItem.delegateItemObj.modelObj.seq, 0)) {
                                                item.width = 70
                                            }
                                        }
                                    } else {
                                        if (!CommonUtils.isNull(modelLowestIndex)) {
                                            if (!CommonUtils.isNull(contentItem.delegateItemObj.modelObj)) {
                                                if (CommonUtils.isEqual(contentItem.delegateItemObj.modelObj.seq, modelLowestIndex)) {
                                                    item.width =  70
                                                }
                                            }
                                        }
                                    }
                                }
                                if (nearItem.width || lowestItem.width){
                                    leftItem.width =  rightItem.width = 10
                                    if (nearItem.width && lowestItem.width) {
                                        nearItem.width = middleItem.width = 0
                                    }
                                } else {
                                    leftItem.width =  rightItem.width = 0
                                    middleItem.width = 0
                                }
                                if (CommonUtils.isEqual(item , mainItem)) {
                                    if (!contentItem.truncated) {
                                        //텍스트 영역보다 글자의 너비가 적을 때
                                        if (contentItem.contentWidth && contentItem.contentWidth < contentItem.width) {
                                            mainItem.width = contentItem.contentWidth
                                        }
                                    }
                                }

                                if ( CommonUtils.getCalcWidth(mainTitleRow) > width )
                                    CommonUtils.applyRemainWidth(mainItem, mainTitleRow)
                              }
                              Loader {
                                   width: parent.width
                                   height:parent.height
                                   clip: true
                                   sourceComponent: mainTitle
                                   Component.onCompleted: delegateItemObj.mainTitle = this

                                   function resetWidth() {
                                       //상세정보에서 이름에 해당하는 항목을 변경하는 경우 처리
                                       width = parent.width
                                   }

                                   function changeStatus(owner) {
                                       mainTitleRow.changeWidth(owner, owner.children[0])
                                   }
                                   onChildrenChanged: {
                                       if (children.length <= 0)
                                           return
                                        children[0].delegateItemObj = delegateItemObj
                                   }
                              }
                              Item {width: 0; height: parent.height}
                              Loader {
                                   width: 0
                                   height: 28
                                   sourceComponent: nearDistance
                                   function changeStatus(owner) {
                                        mainTitleRow.changeWidth(owner, owner.children[0])
                                   }
                                   onChildrenChanged: {
                                       if (children.length <= 0)
                                           return
                                        children[0].delegateItemObj = delegateItemObj
                                   }
                              }
                              Item {width: 0; height: parent.height}
                              Loader {
                                   width: 0
                                   height: 28
                                   sourceComponent: lowestPrice
                                   function changeStatus(owner) {
                                        mainTitleRow.changeWidth(owner, owner.children[0])
                                   }
                                   onChildrenChanged: {
                                       if (children.length <= 0)
                                           return
                                        children[0].delegateItemObj = delegateItemObj
                                   }
                              }
                              Item {width: 0; height: parent.height}
                          }
                      }
                      Item {width: parent.width; height: 12}
                      Item {width: parent.width; height: 28
                          Loader {
                               width: parent.width
                               height:parent.height
                               sourceComponent: subTitle
                               onChildrenChanged: {
                                   if (children.length <= 0)
                                       return
                                    children[0].delegateItemObj = delegateItemObj
                               }
                          }
                      }
                  }
              }
              Item {width: 40;height:parent.height}
              Item {width: 100;height:parent.height
                  Column {width:parent.width
                      height: (32+9+28)
                      anchors.verticalCenter: parent.verticalCenter
                      Item {width: parent.width; height: 32
                          Loader {
                               width: parent.width
                               height:parent.height
                               sourceComponent:  priceTitle
                               onChildrenChanged: {
                                   if (children.length <= 0)
                                       return
                                    children[0].delegateItemObj = delegateItemObj
                               }
                          }
                      }
                      Item {width: parent.width; height: 9}
                      Item {width: parent.width; height: 28
                          Loader {
                               width: parent.width
                               height:parent.height
                               sourceComponent:  distanceTitle
                               onChildrenChanged: {
                                   if (children.length <= 0)
                                       return
                                    children[0].delegateItemObj = delegateItemObj
                               }
                          }
                      }
                  }
              }
          }//Row
      }//Component

      //3.전기차충전소
      Component {
          id: evChargType
            Row {
              property var delegateItemObj:null
              property var modelSeq: (!CommonUtils.isNull(delegateItemObj) &&
                                      !CommonUtils.isNull(delegateItemObj.modelObj) &&
                                      !CommonUtils.isNull(delegateItemObj.modelObj.seq)) ?  delegateItemObj.modelObj.seq : null
              property var fastCharge: null
              property var slowCharge: null
              property string stationIconSource: CommonConstants.Empty
              onModelSeqChanged: {
                  if(CommonUtils.isNull(modelSeq))
                      return
                  refreshCharger()
              }

              function  refreshCharger() {
                  if(CommonUtils.isNull(modelSeq))
                      return
                  if (modelSeq < 0 || listModel.count <= modelSeq)
                      return
                  var item = listModel.get(modelSeq)
                  stationIconSource = CommonConstants.Empty//applyBrandIcon(modelSeq)//전기차 아이콘 표출하지 않기로 함
                  fastCharge = searchRoot.result[item.idx][SearchConstants.ResultFastCharge]
                  slowCharge = searchRoot.result[item.idx][SearchConstants.ResultSlowCharge]

                  if (resultEvDataSync &&
                      searchRoot.result[item.idx][SearchConstants.ResultOnline] &&
                      !searchRoot.result[item.idx][SearchConstants.ResultAlongRoute])
                      return

                  //전기차 상세정보를 얻어 왔는지 체크
                  if (!listModel.get(modelSeq).response)
                      return

                  if (!getChargeTypeShow(modelSeq)) {
                      listModel.setProperty(modelSeq, CommonConstants.Height, 0)
                  } else {
                      if (!searchRoot.visible) {
                          showHideByOwner()
                      }
                  }
              }

              width: parent.width
              height: parent.height
              Item {width: parent.width -(40+100);height:parent.height;
                  Component.onCompleted: {mainTitleRow.changeWidth()}
                  Column {width:parent.width
                      height: (32+12+28)
                      anchors.verticalCenter: parent.verticalCenter
                      Item {width: parent.width; height: 32
                          Row {width: parent.width; height:parent.height
                              id: mainTitleRow
                              property var childEnum: ({main:0,left:1,brand:2,middle:3,rapid:4,right:5,slow:6})
                              function changeWidth(item, contentItem) {
                                var mainItem   = CommonUtils.getItemFromObj(mainTitleRow, childEnum.main)
                                var leftItem   = CommonUtils.getItemFromObj(mainTitleRow, childEnum.left)
                                var brandItem  = CommonUtils.getItemFromObj(mainTitleRow, childEnum.brand)
                                var middleItem = CommonUtils.getItemFromObj(mainTitleRow, childEnum.middle)
                                var rapidItem  = CommonUtils.getItemFromObj(mainTitleRow, childEnum.rapid)
                                var rightItem  = CommonUtils.getItemFromObj(mainTitleRow, childEnum.right)
                                var slowItem   = CommonUtils.getItemFromObj(mainTitleRow, childEnum.slow)
                                if (CommonUtils.isEqual(item , mainItem)) {
                                    if (!contentItem.truncated) {
                                        if (contentItem.contentWidth && contentItem.contentWidth  < contentItem.width) {
                                            mainItem.width = contentItem.contentWidth
                                        }
                                    }
                                }

                                if ( CommonUtils.getCalcWidth(mainTitleRow) > width )
                                    CommonUtils.applyRemainWidth(mainItem, mainTitleRow)
                              }
                              Loader {
                                   width: parent.width
                                   height:parent.height
                                   sourceComponent: mainTitle
                                   Component.onCompleted: delegateItemObj.mainTitle = this

                                   function resetWidth() {
                                       //상세정보에서 이름에 해당하는 항목을 변경하는 경우 처리
                                       width = parent.width
                                   }

                                   function changeStatus(owner) {
                                       mainTitleRow.changeWidth(owner, owner.children[0])
                                   }
                                   onChildrenChanged: {
                                       if (children.length <= 0)
                                           return
                                        children[0].delegateItemObj = delegateItemObj
                                   }
                              }
                              Item {width: 12; height: parent.height}
                              Item {width: 0; height: parent.height
                                  Image {
                                      anchors.centerIn: parent
                                      width: parent.width
                                      height: parent.height
                                      sourceSize.width: width
                                      sourceSize.height: height
                                      source: stationIconSource
                                      onSourceChanged: {
                                          var middelItem = CommonUtils.getItemFromObj(mainTitleRow, mainTitleRow.childEnum.middle)
                                          if (CommonUtils.isEmpty(stationIconSource)) {
                                            height = 0
                                            parent.width = 0
                                            middelItem.width = 0
                                            return
                                          }
                                          height = 30
                                          parent.width = 30
                                          middelItem.width = 12
                                      }
                                      onWidthChanged: {
                                          mainTitleRow.changeWidth()
                                      }
                                  }
                              }
                              Item {width: 0; height: parent.height
                                  onWidthChanged: {
                                      mainTitleRow.changeWidth()
                                  }
                              }
                              Item {width: 0; height: parent.height
                                  Image {
                                      id: idFastCharge
                                      anchors.verticalCenter: parent.verticalCenter
                                      width: visible ? 70 : 0
                                      onWidthChanged: {
                                          parent.width = width
                                          mainTitleRow.changeWidth()
                                      }
                                      height: 28
                                      sourceSize.width: width
                                      sourceSize.height: height
                                      source: {
                                          if ( CommonUtils.isNull(fastCharge) ||
                                              !CommonUtils.isBoolean(fastCharge)) {
                                              return CommonConstants.Empty
                                          }
                                          return fastCharge ? Resource.imageUrl(imgArr[imgEnum.FastOn])
                                                            : Resource.imageUrl(imgArr[imgEnum.FastOff])
                                      }
                                      visible: (CommonUtils.isNull(fastCharge) ||
                                                !CommonUtils.isBoolean(fastCharge)) ? false : fastCharge ? true : false
                                  }
                              }
                              Item {width: 6; height: parent.height}
                              Item {width: 0; height: parent.height
                                  Image {
                                      anchors.verticalCenter: parent.verticalCenter
                                      width: visible ? 70 : 0
                                      onWidthChanged: {
                                          parent.width = width
                                          mainTitleRow.changeWidth()
                                      }
                                      height: 28
                                      sourceSize.width: width
                                      sourceSize.height: height
                                      source: {
                                          if ( CommonUtils.isNull(slowCharge) ||
                                              !CommonUtils.isBoolean(slowCharge)) {
                                              return CommonConstants.Empty
                                          }
                                          return slowCharge ? Resource.imageUrl(imgArr[imgEnum.SlowOn])
                                                            : Resource.imageUrl(imgArr[imgEnum.SlowOff])
                                      }
                                      visible: (CommonUtils.isNull(slowCharge) ||
                                                !CommonUtils.isBoolean(slowCharge)) ? false : slowCharge ? true : false
                                  }
                              }
                              Item {width: 0; height: parent.height}
                          }
                      }
                      Item {width: parent.width; height: 12}
                      Item {width: parent.width; height: 28
                          Loader {
                               width: parent.width
                               height:parent.height
                               sourceComponent: subTitle
                               onChildrenChanged: {
                                   if (children.length <= 0)
                                       return
                                    children[0].delegateItemObj = delegateItemObj
                               }
                          }
                      }
                  }
              }
              Item {width: 40;height:parent.height}
              Item {width: 100;height:parent.height
                  Item {height:28;width:parent.width; anchors.centerIn: parent
                      Loader {
                           width: parent.width
                           height:parent.height
                           sourceComponent: distanceTitle
                           onChildrenChanged: {
                               if (children.length <= 0)
                                   return
                                children[0].delegateItemObj = delegateItemObj
                           }
                      }
                  }
              }
          }//Row
      }//Component


      //제목
      Component {
             id: mainTitle
             TMarqueeText {
                 id: marquee
                 property var delegateItemObj:null
                 property var modelSeq: null
                 property string modelName: CommonConstants.Empty
                 property color mainTxtColor: CommonConstants.Transparent
                 property var focused : searchRoot.focused && CommonUtils.isEqual(listView.currentIndex , modelSeq)

                 //[주의] Text의 property
                 property real contentWidth: 0.0
                 property real contentHeight: 0.0
                 property bool truncated: false

                 onParentChanged:  {
                     if (CommonUtils.isNull(parent))
                         return
                     try {
                         marquee.contentWidth = parent.width
                         marquee.contentHeight = parent.height
                     } catch(e) {
                     }
                 }

                 onDelegateItemObjChanged: {
                     if (CommonUtils.isNull(delegateItemObj))
                         return
                     modelName = Qt.binding(function(){
                         if (CommonUtils.isNull(delegateItemObj))
                             return CommonConstants.Empty
                         if (CommonUtils.isNull(delegateItemObj.modelObj))
                             return CommonConstants.Empty
                         if (CommonUtils.isNull(delegateItemObj.modelObj.name))
                             return CommonConstants.Empty
                         return delegateItemObj.modelObj.name
                     })
                     mainTxtColor = Qt.binding(function(){
                         if (CommonUtils.isNull(delegateItemObj))
                             return CommonConstants.Transparent
                         if (CommonUtils.isNull(delegateItemObj.mainTxtColor))
                             return CommonConstants.Transparent
                         return delegateItemObj.mainTxtColor})
                     modelSeq = Qt.binding(function(){
                         if (CommonUtils.isNull(delegateItemObj))
                             return CommonConstants.Empty
                         if (CommonUtils.isNull(delegateItemObj.modelObj))
                             return CommonConstants.Empty
                         if (CommonUtils.isNull(delegateItemObj.modelObj.seq))
                             return CommonConstants.Empty
                         return delegateItemObj.modelObj.seq})
                 }

                 onModelNameChanged: {
                     if(!CommonUtils.isUndefined(typeof parent.resetWidth)) {
                       parent.resetWidth()
                     }
                 }

                 function changedContentWidth(obj, width) {
                      if (focused)
                          return
                      if(!CommonUtils.isUndefined(typeof obj.parent.changeStatus)) {
                           obj.contentWidth = width
                           obj.parent.changeStatus(obj.parent)
                     }
                 }


                 width: parent.width
                 height: parent.height
                 scrollingText.verticalAlignment: Qt.AlignVCenter
                 scrollingText.clip:  true
                 scrollingText.font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(GlobalConstants.kOneLineListTextFontSize) }
                 scrollingText.color: mainTxtColor
                 scrollingText.text: modelName
                 scrollingText.elide:  focused ? Text.ElideNone : Text.ElideRight
                 scrollingText.visible: focused
                 marqueeTimer.interval: 10
                 running: focused && truncated
                 onRunningChanged: {
                     if (!running) {
                         scrollingText.x = parent.x
                     }
                 }

                 Text2 {
                     property real oldContentWidth: 0.0
                     onContentWidthChanged: {
                         if (oldContentWidth >= contentWidth)
                             return
                         oldContentWidth = contentWidth
                         parent.changedContentWidth(parent, contentWidth)
                     }
                     onTruncatedChanged: parent.truncated = truncated
                     verticalAlignment: parent.scrollingText.verticalAlignment
                     width: parent.width
                     height: parent.height
                     clip: parent.scrollingText.clip
                     font: parent.scrollingText.font
                     color: parent.scrollingText.color
                     text: parent.scrollingText.text
                     elide: focused ? Text.ElideNone : Text.ElideRight
                     visible: !parent.scrollingText.visible
                     offsetY: 2
                 }
             }
         }

      //주소
      Component {
          id: subTitle
          Text2 {
              property var delegateItemObj:null
              property int modelSeq: 0
              property string modelAddress: CommonConstants.Empty
              property color subTxtColor: CommonConstants.Transparent
              onDelegateItemObjChanged: {
                  if (CommonUtils.isNull(delegateItemObj))
                      return
                  modelSeq = Qt.binding(function(){
                      if (CommonUtils.isNull(delegateItemObj))
                          return 0
                      if (CommonUtils.isNull(delegateItemObj.modelObj))
                          return 0
                      if (CommonUtils.isNull(delegateItemObj.modelObj.seq))
                          return 0
                      return delegateItemObj.modelObj.seq})
                  modelAddress = Qt.binding(function(){
                      if (CommonUtils.isNull(delegateItemObj))
                          return CommonConstants.Empty
                      if (CommonUtils.isNull(delegateItemObj.modelObj))
                          return CommonConstants.Empty
                      if (CommonUtils.isNull(delegateItemObj.modelObj.address))
                          return CommonConstants.Empty
                      return delegateItemObj.modelObj.address})
                  subTxtColor = Qt.binding(function(){
                      if (CommonUtils.isNull(delegateItemObj))
                          return CommonConstants.Transparent
                      if (CommonUtils.isNull(delegateItemObj.subTxtColor))
                          return CommonConstants.Transparent
                      return delegateItemObj.subTxtColor})
              }

              width: parent.width
              height: parent.height
              verticalAlignment: Qt.AlignVCenter
              clip:  true
              font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(GlobalConstants.kSecondLineListTextFontSize) }
              color: subTxtColor
              text: modelAddress
              elide: Text.ElideRight
              offsetY: 3
          }
      }

      //거리
      Component {
          id: distanceTitle
          Text2 {
              property var delegateItemObj:null
              property int modelSeq:  0
              property int modelDistance:  0
              property color distanceTxtColor: CommonConstants.Transparent
              property var distanceTxt: Formatter.getDistanceText(modelDistance)
              onDelegateItemObjChanged: {
                  if (CommonUtils.isNull(delegateItemObj))
                      return
                  modelSeq =  Qt.binding(function(){
                      if (CommonUtils.isNull(delegateItemObj))
                          return 0
                      if (CommonUtils.isNull(delegateItemObj.modelObj))
                          return 0
                      if (CommonUtils.isNull(delegateItemObj.modelObj.seq))
                          return 0
                      return CommonUtils.number(delegateItemObj.modelObj.seq)})
                  modelDistance =  Qt.binding(function(){
                      if (CommonUtils.isNull(delegateItemObj))
                          return 0
                      if (CommonUtils.isNull(delegateItemObj.modelObj))
                          return 0
                      if (CommonUtils.isNull(delegateItemObj.modelObj.distance))
                          return 0
                      return CommonUtils.number(delegateItemObj.modelObj.distance)})
                  distanceTxtColor = Qt.binding(function(){
                      if (CommonUtils.isNull(delegateItemObj))
                          return CommonConstants.Transparent
                      if (CommonUtils.isNull(delegateItemObj.distanceTxtColor))
                          return CommonConstants.Transparent
                      return delegateItemObj.distanceTxtColor})
              }

              width: parent.width
              height: parent.height
              verticalAlignment: Qt.AlignVCenter
              horizontalAlignment: Qt.AlignRight
              font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(GlobalConstants.kSecondLineListTextFontSize) }
              color: distanceTxtColor
              text: distanceTxt
              offsetY: 2
          }
      }

      //가격
      Component {
        id: priceTitle
        Text2 {
            property var delegateItemObj:null
            property int modelPrice: 0
            property color priceTxtColor: CommonConstants.Transparent
            property var priceTxt: (modelPrice > 0)  ? (CommonUtils.addComma(CommonUtils.string(modelPrice)) + textArr[textEnum.Won])
                                                     : textArr[textEnum.InfoNone]
            onDelegateItemObjChanged: {
                if (CommonUtils.isNull(delegateItemObj))
                    return
                modelPrice = Qt.binding(function(){
                    if (CommonUtils.isNull(delegateItemObj))
                        return 0
                    if (CommonUtils.isNull(delegateItemObj.modelObj))
                        return 0
                    if (CommonUtils.isNull(delegateItemObj.modelObj.price))
                        return 0
                    return delegateItemObj.modelObj.price})
                priceTxtColor = Qt.binding(function(){
                    if (CommonUtils.isNull(delegateItemObj))
                        return CommonConstants.Transparent
                    if (CommonUtils.isNull(delegateItemObj.priceTxtColor))
                        return CommonConstants.Transparent
                    return delegateItemObj.priceTxtColor})
            }

            width: parent.width
            height: parent.height
            verticalAlignment: Qt.AlignVCenter
            horizontalAlignment: Qt.AlignRight
            font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(GlobalConstants.kOneLineListTextFontSize) }
            color: priceTxtColor
            text: priceTxt
            offsetY: 3
        }
      }

      //가까운
      Component {
          id: nearDistance
          Image {
              onParentChanged: {
                  if (CommonUtils.isNull(parent))
                      return
              }

              property var delegateItemObj:null
              property bool modelNear: false
              property int modelDistance:  0

              onDelegateItemObjChanged: {
                  if (CommonUtils.isNull(delegateItemObj))
                      return
                  modelDistance =  Qt.binding(function(){
                      if (CommonUtils.isNull(delegateItemObj))
                          return 0
                      if (CommonUtils.isNull(delegateItemObj.modelObj))
                          return 0
                      if (CommonUtils.isNull(delegateItemObj.modelObj.distance))
                          return 0
                      return delegateItemObj.modelObj.distance})
              }

              onModelDistanceChanged: {
                 modelNear =  CommonUtils.isEqual(modelDistance , nearDistValue)
              }

              onModelNearChanged: {
                  if (!modelNear)
                      return
                  if(!CommonUtils.isUndefined(typeof parent.changeStatus)) {
                      if (CommonUtils.isEqual(searchRoot.order, SearchResultViewEvent.PriceOrder)) {
                          if (CommonUtils.isNull(modelNearIndex)) {
                            modelNearIndex = delegateItemObj.modelObj.seq
                          }
                      }
                      parent.changeStatus(parent)
                  }
              }

              source: Resource.imageUrl(imgArr[imgEnum.NearTag])
              width: parent.width
              height: parent.height
              sourceSize.width: width
              sourceSize.height: height
          }
      }

      //최저가
      Component {
          id: lowestPrice
          Image {
              onParentChanged: {
                  if (CommonUtils.isNull(parent))
                      return
              }

              property var delegateItemObj:null
              property bool modelLowest: false
              property int modelPrice:  0

              onDelegateItemObjChanged: {
                  if (CommonUtils.isNull(delegateItemObj))
                      return
                  modelPrice =  Qt.binding(function(){
                      if (CommonUtils.isNull(delegateItemObj))
                          return 0
                      if (CommonUtils.isNull(delegateItemObj.modelObj))
                          return 0
                      if (CommonUtils.isNull(delegateItemObj.modelObj.price))
                          return 0
                      return delegateItemObj.modelObj.price})
              }

              onModelPriceChanged: {
                 modelLowest =  CommonUtils.isEqual(modelPrice , lowestPriceValue)
              }

              onModelLowestChanged: {
                  if (!modelLowest)
                      return
                  if(!CommonUtils.isUndefined(typeof parent.changeStatus)) {
                      if (CommonUtils.isEqual(searchRoot.order, SearchResultViewEvent.DistanceOrder)) {
                          if (CommonUtils.isNull(modelLowestIndex)) {
                            modelLowestIndex = delegateItemObj.modelObj.seq
                          }
                      }
                    parent.changeStatus(parent)
                  }
              }

              source: Resource.imageUrl(imgArr[imgEnum.MinimumTag])
              width: parent.width
              height: parent.height
              sourceSize.width: width
              sourceSize.height: height
          }
      }

      Csw {
          id: csw

          section: Csw.Section_SearchResult
          sequence: 0
          enable: searchRoot.visible

          onFocused: {
              reqSpinCw = false
              listView.manualClick = false
              var item = listView.currentItem
              if (!CommonUtils.isNull(item.isButtonFocused)) {
                  item.isButtonFocused = false
              }
              searchRoot.focused = true

              resetCurrentList(listView.firstIndex)
              resetEvIndex()
              listView.manualClick = true
          }
          onLostFocusd: {
              reqSpinCw = false
              listView.manualClick = false
              var item = listView.currentItem
              if (!CommonUtils.isNull(item.isButtonFocused)) {
                  item.isButtonFocused = false
              }
              searchRoot.focused = false
          }
          onLeftPressed: {
              reqSpinCw = false
              var item = listView.currentItem
              if (!CommonUtils.isNull(item.isButtonFocused)) {
                  item.isButtonFocused = false
              }
          }
          onRightPressed: {
              reqSpinCw = false
              var item = listView.currentItem
              if (!CommonUtils.isNull(item.buttonVisible) && item.buttonVisible) {
                  if (!item.isButtonFocused) {
                      item.isButtonFocused = true
                  }
              }

          }
          property bool reqSpinCw: false
          function spinCwPress(oldItemCheck) {
              reqSpinCw = false
              var oldItemHide = false
              var isButtonFocused = false
              var oldItem = listView.currentItem
              if (!CommonUtils.isNull(oldItem.isButtonFocused)) {
                  isButtonFocused = oldItem.isButtonFocused
              }

              if (oldItem && oldItem.height <= 0) {
                  oldItemHide = true
                  spinCcwPress()
                  oldItem = listView.currentItem
                  if (!CommonUtils.isNull(oldItem.isButtonFocused)) {
                     isButtonFocused = oldItem.isButtonFocused
                  }
              }

              if (oldItemCheck && !oldItemHide) {
                  return
              }

              var oldIndex = listView.currentIndex
              if (listView.currentIndex < listView.count - 1) {
                  ++listView.currentIndex
                  //전기차인 경우 비어있는 행이 있어...
                  while( listView.currentIndex < listView.count - 1 &&
                         listView.currentItem.height <= 0) {
                      ++listView.currentIndex
                  }
                  var endListIdx   = listView.indexAt(listView.contentX, listView.contentY + (listView.height) )
                  if ((endListIdx > 0 && CommonUtils.isEqual(endListIdx,oldItem)) ||
                       listView.currentItem.height <= 0) {
                      spinCcwPress()
                      return
                  }
                  reqSpinCw = true
                  var currItem = listView.currentItem
                  if (currItem && currItem.height > 0) {
                      if (!CommonUtils.isNull(oldItem.isButtonFocused)) {
                          if (!CommonUtils.isEqual(oldItem, currItem) ) {
                            oldItem.isButtonFocused = false
                          }
                      }
                      if (!CommonUtils.isNull(currItem.isButtonFocused)) {
                        currItem.isButtonFocused = isButtonFocused
                      }
                  }
              }
          }


          function spinCcwPress() {
              reqSpinCw = false
              var isButtonFocused = false
              var oldItem = listView.currentItem
              if (!CommonUtils.isNull(oldItem.isButtonFocused)) {
                  isButtonFocused = oldItem.isButtonFocused
              }
              var oldIndex = listView.currentIndex
              if (0 < listView.currentIndex) {
                  --listView.currentIndex
                  //전기차인 경우 비어있는 행이 있어...
                  while( 0 < listView.currentIndex &&
                        listView.currentItem.height <= 0) {
                      --listView.currentIndex
                  }
                  var currItem = listView.currentItem
                  if (!CommonUtils.isNull(oldItem.isButtonFocused)) {
                      if (!CommonUtils.isEqual(oldItem, currItem) &&
                         (currItem && currItem.height > 0)) {
                        oldItem.isButtonFocused = false
                      }
                  }
                  if (currItem && !CommonUtils.isNull(currItem.isButtonFocused)) {
                      currItem.isButtonFocused = isButtonFocused
                  }
              }
          }

          onSpinCwPressed: {
              spinCwPress(false)
          }

          onSpinCcwPressed: {
              spinCcwPress()
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
