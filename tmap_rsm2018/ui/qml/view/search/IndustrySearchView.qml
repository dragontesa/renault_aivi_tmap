import QtQuick 2.0
import ViewRegistry 1.0
import CommonConstants 1.0
import ZoneConstants 1.0
import SearchConstants 1.0
import SettingConstants 1.0
import IndustrySearchViewEvent 1.0
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

    //언어 관련
    property var __lang                         : null
    property bool needInit                      : true

    property bool reFocus                       : false
    property bool isShowTopView                 : false


    property bool mouseAreaEnabled              : false
    property bool backgroundVisible             : false

    property var caller                         : null
    property var relay                          : null
    property var owner                          : null
    property var query                          : null

    property bool isMediated                    : false

    property var listModel                      : grid1DepthModel
    property var listView                       : grid1DepthView
    property int currDepth                      : -1
    property int gridRow                        : 2
    property var gridCol                        : null

    //검색 요청 갯수
    property int reqMaxCount                    : SearchConstants.RequestCount

    property size tileMenuImgSize               : Qt.size(71,71)

    property bool focused                       : false
    property color highlightColor               : CommonConstants.Transparent

    property string messageText                 : CommonConstants.Empty
    property bool messageVisible                : false
    property color frameColor                   : CommonConstants.Transparent
    property int frameHeight                    : {
                                                    if (CommonUtils.isIndexOf(caller, [CommonConstants.objectName(ViewRegistry.BookmarkPageView)]) ||
                                                        CommonUtils.isIndexOf(owner , [CommonConstants.objectName(ViewRegistry.BookmarkPageView)]) ||
                                                        CommonUtils.isIndexOf(caller, [CommonConstants.objectName(ViewRegistry.RouteSettingView)]) ||
                                                        CommonUtils.isIndexOf(owner , [CommonConstants.objectName(ViewRegistry.RouteSettingView)]) ||
                                                        currDepth > IndustrySearchViewEvent.DepthFirst) {
                                                        return height
                                                    }  else  {
                                                        return 337
                                                    }
                                                  }

    property bool reqSearch                     : false


    //Tile Menu
    property int tileMenuSpaceWidth             : 0
    property int tileMenuContentButtonYOffset   : 0
    property int tileMenuContentTextYOffset     : 0
    property color tileMenuOutlineColor         : CommonConstants.Transparent

    //Timer
    property var timer1DepthMenu                : null
    property var timer2or3DepthMenu             : null


    //ID
    property var grid1DepthView                 : null
    property var list2DepthView                 : null
    property var list3DepthView                 : null
    property var message                        : null



    //Enum
    property var textEnum                       : ({})
    property var item1DepthEnum                 : ({})
    property var item2DepthEnum                 : ({})
    property var item3DepthEnum                 : ({})
    property var softKeyEnum                    : ({})
    property var menuKoEnum                     : ({})
    property var menuEnum                       : ({})
    property var depthEnum                      : ({})
    property var reqEnum                        : ({})
    property var resEnum                        : ({})

    property var textArr                        : ([])
    property var item1DepthKoArr                : ([])
    property var item1DepthEnArr                : ([])
    property var item2DepthKoArr                : ([])
    property var item2DepthEnArr                : ([])
    property var item3DepthKoArr                : ([])
    property var item3DepthEnArr                : ([])
    property var softKeyMenuArr                 : ([])
    property var menuNameKoArr                  : ([])
    property var menuNameArr                    : ([])
    property var menuCodeArr                    : ([])
    property var menuImgNArr                    : ([])
    property var menuImgPArr                    : ([])
    property var menuImgFArr                    : ([])

    function allocateTextEnum(){
        if (isMediated) {
            sendEvent(IndustrySearchViewEvent.RequestViewList)
//            if (isShowTopView) {
//                return
//            }
        }
        if (!needInit)
            return
        needInit = false

        if (CommonUtils.getObjectLen(textEnum) <= 0) {
            textEnum = CommonUtils.createEnum(textEnum)
        }
        var i = 0
        CommonUtils.insertEnum(textEnum , "NoData"      , i++, [CommonConstants.Name] , [qsTr("검색 데이타가 없습니다.")])
        CommonUtils.insertEnum(textEnum , "Slash"       , i++, [CommonConstants.Name] , ["/"])
        CommonUtils.insertEnum(textEnum , "LessThan"    , i++, [CommonConstants.Name] , ["<"])

        CommonUtils.fetchArr(textEnum , CommonConstants.Name , textArr)
    }

    function allocateItem1DepthEnum() {
           if (CommonUtils.getObjectLen(item1DepthEnum) <= 0) {
               item1DepthEnum = CommonUtils.createEnum(item1DepthEnum)
           }

           var i = 0
           CommonUtils.insertEnum(item1DepthEnum , CommonUtils.string(00000),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Category search"                                                    , "업종 검색"])
           CommonUtils.insertEnum(item1DepthEnum , CommonUtils.string(10000),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Transit<br>convenience"                                             , "교통편의"])
           CommonUtils.insertEnum(item1DepthEnum , CommonUtils.string(20000),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Vehicle<br>convenience"                                             , "자동차편의"])
           CommonUtils.insertEnum(item1DepthEnum , CommonUtils.string(30000),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Building<br>facilities"                                             , "건물시설"])
           CommonUtils.insertEnum(item1DepthEnum , CommonUtils.string(40000),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Travel/Leisure"                                                     , "여행레저"])
           CommonUtils.insertEnum(item1DepthEnum , CommonUtils.string(50000),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Shopping"                                                           , "쇼핑"])
           CommonUtils.insertEnum(item1DepthEnum , CommonUtils.string(60000),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Public<br>convenience"                                              , "공공편의"])
           CommonUtils.insertEnum(item1DepthEnum , CommonUtils.string(80000),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Life<br>convenience"                                                , "생활편의"])
           CommonUtils.insertEnum(item1DepthEnum , CommonUtils.string(90000),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Medical"                                                            , "의료편의"])

           CommonUtils.fetchArr(item1DepthEnum , CommonConstants.Ko , item1DepthKoArr)
           CommonUtils.fetchArr(item1DepthEnum , CommonConstants.En , item1DepthEnArr)

       }


       function allocateItem2DepthEnum() {
           if (CommonUtils.getObjectLen(item2DepthEnum) <= 0) {
               item2DepthEnum = CommonUtils.createEnum(item2DepthEnum)
           }

           var i = 0
           CommonUtils.insertEnum(item2DepthEnum , CommonUtils.string(10000),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Transit convenience"                                                , "교통편의"])
           CommonUtils.insertEnum(item2DepthEnum , CommonUtils.string(10100),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Transit facility"                                                   , "교통시설"])
           CommonUtils.insertEnum(item2DepthEnum , CommonUtils.string(10200),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Rest area"                                                          , "휴게소"])
           CommonUtils.insertEnum(item2DepthEnum , CommonUtils.string(10300),  i++, [CommonConstants.En , CommonConstants.Ko] , ["IC/JC/TG"                                                           , "IC/JC/TG"])
           CommonUtils.insertEnum(item2DepthEnum , CommonUtils.string(10400),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Terminal"                                                           , "터미널"])
           CommonUtils.insertEnum(item2DepthEnum , CommonUtils.string(10500),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Road/Crossing"                                                      , "차도/교차로"])
           CommonUtils.insertEnum(item2DepthEnum , CommonUtils.string(10600),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Bridge/Railroad bridge/Tunnel"                                      , "대교/철교/터널"])
           CommonUtils.insertEnum(item2DepthEnum , CommonUtils.string(19900),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Other transit convenience"                                          , "교통편의기타"])

           CommonUtils.insertEnum(item2DepthEnum , CommonUtils.string(20000),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Vehicle convenience"                                                , "자동차편의"])
           CommonUtils.insertEnum(item2DepthEnum , CommonUtils.string(20100),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Fuel station"                                                       , "주유소"])
           CommonUtils.insertEnum(item2DepthEnum , CommonUtils.string(20200),  i++, [CommonConstants.En , CommonConstants.Ko] , ["LPG station"                                                        , "충전소"])
           CommonUtils.insertEnum(item2DepthEnum , CommonUtils.string(20300),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Car wash/Parking"                                                   , "세차/주차장"])
           CommonUtils.insertEnum(item2DepthEnum , CommonUtils.string(20400),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Direct service center "                                             , "직영서비스센터"])
           CommonUtils.insertEnum(item2DepthEnum , CommonUtils.string(20500),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Designated repair center"                                           , "지정정비센터"])
           CommonUtils.insertEnum(item2DepthEnum , CommonUtils.string(20600),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Repair center"                                                      , "정비센터"])
           CommonUtils.insertEnum(item2DepthEnum , CommonUtils.string(20700),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Car dealership"                                                     , "자동차영업소"])
           CommonUtils.insertEnum(item2DepthEnum , CommonUtils.string(20800),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Driver's license"                                                   , "운전면허"])
           CommonUtils.insertEnum(item2DepthEnum , CommonUtils.string(20900),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Car delivery office"                                                , "출고사업소"])
           CommonUtils.insertEnum(item2DepthEnum , CommonUtils.string(21000),  i++, [CommonConstants.En , CommonConstants.Ko] , ["EV charging station"                                                , "EV충전소"])
           CommonUtils.insertEnum(item2DepthEnum , CommonUtils.string(29900),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Car facility"                                                       , "자동차시설"])


           CommonUtils.insertEnum(item2DepthEnum , CommonUtils.string(30000),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Building facility"                                                  , "건물시설"])
           CommonUtils.insertEnum(item2DepthEnum , CommonUtils.string(30100),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Apartment/Villa"                                                    , "아파트/빌라"])
           CommonUtils.insertEnum(item2DepthEnum , CommonUtils.string(30200),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Building/Multipurpose building"                                     , "빌딩/주상복합"])
           CommonUtils.insertEnum(item2DepthEnum , CommonUtils.string(30300),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Manufacturing facility"                                             , "생산시설"])
           CommonUtils.insertEnum(item2DepthEnum , CommonUtils.string(30400),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Public facility"                                                    , "공공시설"])
           CommonUtils.insertEnum(item2DepthEnum , CommonUtils.string(30600),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Industrial facility"                                                , "산업시설"])
           CommonUtils.insertEnum(item2DepthEnum , CommonUtils.string(30700),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Research Institution"                                               , "연구기관"])
           CommonUtils.insertEnum(item2DepthEnum , CommonUtils.string(30800),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Social institution "                                                , "사회시설"])
           CommonUtils.insertEnum(item2DepthEnum , CommonUtils.string(30900),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Warehouse"                                                          , "창고"])
           CommonUtils.insertEnum(item2DepthEnum , CommonUtils.string(39900),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Structure"                                                          , "건물"])


           CommonUtils.insertEnum(item2DepthEnum , CommonUtils.string(40000),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Travel/Leisure"                                                     , "여행레저"])
           CommonUtils.insertEnum(item2DepthEnum , CommonUtils.string(40100),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Accommodations"                                                     , "숙박시설"])
           CommonUtils.insertEnum(item2DepthEnum , CommonUtils.string(40200),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Tour"                                                               , "관광"])
           CommonUtils.insertEnum(item2DepthEnum , CommonUtils.string(40300),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Golf"                                                               , "골프"])
           CommonUtils.insertEnum(item2DepthEnum , CommonUtils.string(40400),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Leisure"                                                            , "레저"])

           CommonUtils.insertEnum(item2DepthEnum , CommonUtils.string(50000),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Shopping"                                                            , "쇼핑"])
           CommonUtils.insertEnum(item2DepthEnum , CommonUtils.string(50100),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Department store/Outlet"                                             , "백화점/할인점"])
           CommonUtils.insertEnum(item2DepthEnum , CommonUtils.string(50200),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Convenience/Supermarket"                                             , "편의/슈퍼"])
           CommonUtils.insertEnum(item2DepthEnum , CommonUtils.string(50300),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Electronic goods "                                                   , "전자제품"])
           CommonUtils.insertEnum(item2DepthEnum , CommonUtils.string(59900),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Shopping facility"                                                   , "쇼핑시설"])

           CommonUtils.insertEnum(item2DepthEnum , CommonUtils.string(60000),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Public convenience"                                                  , "공공편의"])
           CommonUtils.insertEnum(item2DepthEnum , CommonUtils.string(60100),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Public institution"                                                  , "공공기관"])
           CommonUtils.insertEnum(item2DepthEnum , CommonUtils.string(60200),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Institution for public order"                                        , "치안기관"])
           CommonUtils.insertEnum(item2DepthEnum , CommonUtils.string(60300),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Administrative agency"                                               , "행정기관"])
           CommonUtils.insertEnum(item2DepthEnum , CommonUtils.string(60400),  i++, [CommonConstants.En , CommonConstants.Ko] , ["City hall/Provincial government building"                            , "시청/도청"])
           CommonUtils.insertEnum(item2DepthEnum , CommonUtils.string(60500),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Country office/Ward office"                                          , "군청/구청"])
           CommonUtils.insertEnum(item2DepthEnum , CommonUtils.string(60600),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Eup/Myeon/Dong/Li office"                                            , "읍/면/동/리사무소"])
           CommonUtils.insertEnum(item2DepthEnum , CommonUtils.string(60700),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Educational institution"                                             , "교육기관"])
           CommonUtils.insertEnum(item2DepthEnum , CommonUtils.string(60800),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Mass media organization"                                             , "언론기관"])
           CommonUtils.insertEnum(item2DepthEnum , CommonUtils.string(60900),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Foreign institutions"                                                , "외국기관"])
           CommonUtils.insertEnum(item2DepthEnum , CommonUtils.string(69900),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Administrative agency"                                               , "행정기관"])

           CommonUtils.insertEnum(item2DepthEnum , CommonUtils.string(80000),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Life convenience"                                                    , "생활편의"])
           CommonUtils.insertEnum(item2DepthEnum , CommonUtils.string(80100),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Restaurant"                                                          , "음식점"])
           CommonUtils.insertEnum(item2DepthEnum , CommonUtils.string(80200),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Coffee/Café"                                                         , "커피/카페"])
           CommonUtils.insertEnum(item2DepthEnum , CommonUtils.string(80300),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Family ritual"                                                       , "가정의례"])
           CommonUtils.insertEnum(item2DepthEnum , CommonUtils.string(80400),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Religious facility"                                                  , "종교시설"])
           CommonUtils.insertEnum(item2DepthEnum , CommonUtils.string(80500),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Theater"                                                             , "극장"])
           CommonUtils.insertEnum(item2DepthEnum , CommonUtils.string(80600),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Performance/Exhibition"                                              , "공연/전시"])
           CommonUtils.insertEnum(item2DepthEnum , CommonUtils.string(80700),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Library/Cultural institute"                                          , "도서관/문화원"])
           CommonUtils.insertEnum(item2DepthEnum , CommonUtils.string(80800),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Lyceum"                                                              , "문화회관"])
           CommonUtils.insertEnum(item2DepthEnum , CommonUtils.string(89900),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Living service"                                                      , "생활서비스"])

           CommonUtils.insertEnum(item2DepthEnum , CommonUtils.string(90000),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Medical"                                                             , "의료편의"])
           CommonUtils.insertEnum(item2DepthEnum , CommonUtils.string(90100),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Hospital"                                                            , "병원"])
           CommonUtils.insertEnum(item2DepthEnum , CommonUtils.string(90200),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Clinic"                                                              , "의원"])
           CommonUtils.insertEnum(item2DepthEnum , CommonUtils.string(90300),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Oriental medical clinic"                                             , "한방한의원"])
           CommonUtils.insertEnum(item2DepthEnum , CommonUtils.string(99900),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Medical facility"                                                    , "의료시설"])


           CommonUtils.fetchArr(item2DepthEnum , CommonConstants.Ko , item2DepthKoArr)
           CommonUtils.fetchArr(item2DepthEnum , CommonConstants.En , item2DepthEnArr)

       }

       function allocateItem3DepthEnum() {
           if (CommonUtils.getObjectLen(item3DepthEnum) <= 0) {
               item3DepthEnum = CommonUtils.createEnum(item3DepthEnum)
           }

           var i = 0
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(10100),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Transit facility"                                                   , "교통시설"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(10101),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Airport"                                                            , "공항"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(10102),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Railroad station"                                                   , "기차역"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(10103),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Subway station"                                                     , "지하철역"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(10104),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Harbour"                                                            , "항구"])

           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(10200),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Rest area"                                                          , "휴게소"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(10201),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Highway rest area"                                                  , "고속도로휴게소"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(10202),  i++, [CommonConstants.En , CommonConstants.Ko] , ["General/National road rest area"                                    , "일반/국도휴게소"])

           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(10300),  i++, [CommonConstants.En , CommonConstants.Ko] , ["IC/JC/TG"                                                           , "IC/JC/TG"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(10301),  i++, [CommonConstants.En , CommonConstants.Ko] , ["IC"                                                                 , "IC"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(10302),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Tollgate"                                                           , "톨게이트"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(10303),  i++, [CommonConstants.En , CommonConstants.Ko] , ["JC"                                                                 , "JC"])

           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(10400),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Terminal"                                                           , "터미널"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(10401),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Bus terminal"                                                       , "버스터미널"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(10402),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Cargo terminal"                                                     , "화물터미널"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(10403),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Air terminal"                                                       , "공항터미널"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(10404),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Shipping terminal"                                                  , "선박터미널"])

           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(10500),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Road/Crossing"                                                      , "차도/교차로"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(10501),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Flyover/Underpass"                                                  , "고가/지하차도"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(10502),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Crossroad"                                                          , "교차로"])

           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(10600),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Bridge/Railroad bridge/Tunnel"                                      , "대교/철교/터널"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(10601),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Bridge/Railroad bridge"                                             , "대교/철교"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(10602),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Tunnel"                                                             , "터널"])

           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(19900),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Other transit convenience"                                          , "교통편의기타"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(19901),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Bus stop"                                                           , "버스정류장"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(19902),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Checkpoint"                                                         , "검문소"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(19903),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Fee/Ticket office"                                                  , "요금/매표소"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(19999),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Other"                                                              , "기타"])

           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(20100),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Fuel station"                                                        , "주유소"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(20101),  i++, [CommonConstants.En , CommonConstants.Ko] , ["SK fuel station"                                                     , "SK주유소"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(20102),  i++, [CommonConstants.En , CommonConstants.Ko] , ["GS fuel station"                                                     , "GS주유소"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(20103),  i++, [CommonConstants.En , CommonConstants.Ko] , ["S-OIL"                                                               , "S-OIL"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(20104),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Oilbank"                                                             , "오일뱅크"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(20105),  i++, [CommonConstants.En , CommonConstants.Ko] , ["None-pole sign/other"                                                , "무폴/기타"])

           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(20200),  i++, [CommonConstants.En , CommonConstants.Ko] , ["LPG station"                                                         , "충전소"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(20201),  i++, [CommonConstants.En , CommonConstants.Ko] , ["SK LPG station"                                                      , "SK충전소"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(20202),  i++, [CommonConstants.En , CommonConstants.Ko] , ["GS LPG station"                                                      , "GS충전소"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(20203),  i++, [CommonConstants.En , CommonConstants.Ko] , ["S-OIL LPG station"                                                   , "S-OIL충전소"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(20204),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Oilbank"                                                             , "오일뱅크"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(20205),  i++, [CommonConstants.En , CommonConstants.Ko] , ["E1"                                                                  , "E1"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(20206),  i++, [CommonConstants.En , CommonConstants.Ko] , ["None-pole sign/other"                                                , "무폴/기타"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(20300),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Car wash/Parking"                                                    , "세차/주차장"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(20301),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Parking"                                                             , "주차장"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(20302),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Car wash"                                                            , "세차장"])

           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(20400),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Direct service center "                                              , "직영서비스센터"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(20401),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Hyundai"                                                             , "현대"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(20402),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Kia"                                                                 , "기아"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(20403),  i++, [CommonConstants.En , CommonConstants.Ko] , ["GM Korea"                                                            , "한국지엠"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(20404),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Renault Samsung"                                                     , "르노삼성"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(20405),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Ssangyong"                                                           , "쌍용"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(20406),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Imported car"                                                            , "수입차"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(20407),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Jaguar"                                                              , "Jaguar"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(20408),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Ford"                                                                , "Ford"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(20499),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Other"                                                               , "기타"])

           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(20500),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Designated repair center"                                            , "지정정비센터"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(20501),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Hyundai"                                                             , "현대"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(20502),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Kia"                                                                 , "기아"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(20503),  i++, [CommonConstants.En , CommonConstants.Ko] , ["GM Korea"                                                              , "한국지엠"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(20504),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Renault Samsung"                                                     , "르노삼성"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(20505),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Ssangyong"                                                           , "쌍용"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(20506),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Imported car"                                                            , "수입차"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(20599),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Other"                                                               , "기타"])

           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(20600),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Repair center"                                                       , "정비센터"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(20601),  i++, [CommonConstants.En , CommonConstants.Ko] , ["SpeedMate"                                                           , "스피드메이트"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(20602),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Mando plaza"                                                         , "만도프라자"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(20603),  i++, [CommonConstants.En , CommonConstants.Ko] , ["1st grade"                                                           , "1급"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(20604),  i++, [CommonConstants.En , CommonConstants.Ko] , ["2nd grade"                                                           , "2급"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(20605),  i++, [CommonConstants.En , CommonConstants.Ko] , ["3rd grade"                                                           , "3급"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(20606),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Redmate"                                                             , "Redmate"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(20699),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Other"                                                               , "기타"])

           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(20700),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Car dealership"                                                      , "자동차영업소"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(20701),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Hyundai"                                                             , "현대"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(20702),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Kia"                                                                 , "기아"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(20703),  i++, [CommonConstants.En , CommonConstants.Ko] , ["GM Korea"                                                            , "한국지엠"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(20704),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Renault Samsung"                                                     , "르노삼성"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(20705),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Ssangyong"                                                           , "쌍용"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(20706),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Imported car"                                                        , "수입차"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(20799),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Other"                                                               , "기타"])

           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(20800),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Driver's license"                                                    , "운전면허"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(20801),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Driving school"                                                      , "운전면허학원"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(20802),  i++, [CommonConstants.En , CommonConstants.Ko] , ["License test center"                                                 , "면허시험장"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(20900),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Car delivery office"                                                 , "출고사업소"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(20999),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Other"                                                               , "기타"])

           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(21000),  i++, [CommonConstants.En , CommonConstants.Ko] , ["EV charging station"                                                 , "EV충전소"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(21099),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Other"                                                               , "기타"])

           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(29900),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Car facility"                                                        , "자동차시설"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(29901),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Rent-a-car"                                                          , "렌터카"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(29902),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Place of keeping towaways"                                           , "견인차보관소"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(29903),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Vehicle Registration Office"                                         , "차량등록사업소"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(29904),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Used-car market "                                                    , "중고차매매"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(29999),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Other car facility"                                                  , "자동차시설기타"])


           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(30100),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Apartment/Villa"                                                      , "아파트/빌라"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(30101),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Apartment"                                                            , "아파트"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(30102),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Villa"                                                                , "빌라"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(30103),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Studio flat "                                                         , "오피스텔"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(30104),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Terraced house"                                                       , "연립주택"])

           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(30200),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Building/Multipurpose building"                                       , "빌딩/주상복합"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(30201),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Landmark"                                                             , "주요빌딩"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(30202),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Multipurpose building"                                                , "주상복합"])

           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(30300),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Manufacturing facility"                                               , "생산시설"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(30301),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Fishery facility"                                                     , "어업시설"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(30302),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Agro-livestock facility"                                              , "농축산시설"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(30399),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Other manufacturing facility"                                         , "생산시설기타"])

           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(30400),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Public facility"                                                      , "공공시설"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(30401),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Public equipment"                                                     , "공공설비"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(30405),  i++, [CommonConstants.En , CommonConstants.Ko] , ["General equipment"                                                    , "일반설비"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(30499),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Other public facility"                                                , "공공시설기타"])

           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(30600),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Industrial facility"                                                  , "산업시설"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(30601),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Industrial complex "                                                  , "산업단지"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(30602),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Agro-industrial complex"                                              , "농공단지"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(30603),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Stock raising complex"                                                , "축산단지"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(30604),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Factory"                                                              , "공장"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(30699),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Other industrial facility"                                            , "산업시설기타"])

           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(30700),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Research institution"                                                 , "연구기관"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(30701),  i++, [CommonConstants.En , CommonConstants.Ko] , ["National"                                                             , "국공립"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(30702),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Non-public economy research institution"                              , "민간경제연구소"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(30703),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Private research institute"                                           , "민간연구소"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(30704),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Government-funded research institute"                                 , "정부출연연구기관"])

           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(30800),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Social institution "                                                  , "사회시설"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(30801),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Nursing home"                                                         , "양로원"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(30802),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Orphanage"                                                            , "고아원"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(30803),  i++, [CommonConstants.En , CommonConstants.Ko] , ["The disabled "                                                        , "장애인"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(30804),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Welfare center "                                                      , "복지관"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(30805),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Hall"                                                                 , "회관"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(30899),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Other social institution"                                             , "사회시설기타"])

           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(30900),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Warehouse"                                                            , "창고"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(30901),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Oil reservoir "                                                       , "저유소"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(30902),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Distribution center "                                                 , "물류센터"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(30999),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Other warehouse"                                                      , "창고기타"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(39900),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Structure"                                                            , "건물"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(39999),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Other structure"                                                      , "건물기타"])


           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(40100),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Accommodations"                                                        , "숙박시설"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(40101),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Hotel"                                                                 , "호텔"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(40102),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Condominium/Resort"                                                    , "콘도/리조트"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(40103),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Youth hostel"                                                          , "유스호스텔"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(40104),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Motel/Inn"                                                             , "모텔/여관"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(40199),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Other accommodations"                                                  , "숙박기타"])

           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(40200),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Tour"                                                                  , "관광"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(40201),  i++, [CommonConstants.En , CommonConstants.Ko] , ["National/Provincial park"                                              , "국/도립공원"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(40202),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Ancient palace "                                                       , "고궁"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(40203),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Tourist site "                                                         , "관광지"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(40204),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Tourist facilities complex "                                           , "관광단지"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(40205),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Mineral spring "                                                       , "약수터"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(40206),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Tourist attraction"                                                    , "관광명소"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(40207),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Cultural remains"                                                      , "문화유적지"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(40208),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Hot spring"                                                            , "온천"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(40209),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Beach"                                                                 , "해수욕장"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(40210),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Island"                                                                , "섬"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(40211),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Zoo and botanical Gardens "                                            , "동식물원"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(40212),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Recreational forest "                                                  , "자연휴양림"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(40213),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Waterfall/Valley"                                                      , "폭포/계곡"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(40214),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Lake"                                                                  , "호수"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(40215),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Park"                                                                  , "공원"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(40216),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Reservoir "                                                            , "저수지"])

           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(40300),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Golf"                                                                  , "골프"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(40301),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Membership golf"                                                       , "회원제골프장"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(40302),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Golf driving range"                                                    , "골프연습장"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(40303),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Public golf course"                                                    , "퍼블릭골프장"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(40399),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Other golf course"                                                     , "기타골프장"])

           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(40400),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Leisure"                                                               , "레저"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(40401),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Amusement park "                                                       , "놀이공원"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(40402),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Amusement facilities "                                                 , "위락시설"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(40403),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Airline"                                                               , "항공사"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(40404),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Travel agency "                                                        , "여행사"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(40405),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Ski resort"                                                            , "스키장"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(40406),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Sports center"                                                         , "스포츠센터"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(40407),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Sport facility"                                                        , "스포츠시설"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(40408),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Campground"                                                            , "캠프장"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(40409),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Fishing place"                                                         , "낚시터"])


           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(50100),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Department store/Outlet"                                                , "백화점/할인점"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(50101),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Department store"                                                       , "백화점"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(50102),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Outlet"                                                                 , "할인점"])

           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(50200),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Convenience/Supermarket"                                                , "편의/슈퍼"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(50201),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Convenience store"                                                      , "편의점"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(50202),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Supermarket"                                                            , "슈퍼마켓"])

           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(50300),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Electronic goods "                                                      , "전자제품"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(50301),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Mass merchandiser"                                                      , "양판점"])

           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(59900),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Shopping facility"                                                      , "쇼핑시설"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(59901),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Duty free shop "                                                        , "면세점"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(59902),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Special shopping district"                                              , "전문상가"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(59903),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Shopping center"                                                        , "쇼핑센터"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(59904),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Agro-livestock product"                                                 , "농축산물"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(59905),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Other food & beverage"                                                  , "음식료기타"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(59999),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Other shopping"                                                         , "쇼핑기타"])


           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(60100),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Public institution"                                                      , "공공기관"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(60101),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Public prosecutors' office"                                              , "검찰청"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(60102),  i++, [CommonConstants.En , CommonConstants.Ko] , ["National police agency "                                                 , "경찰청"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(60103),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Court"                                                                   , "법원"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(60104),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Fire station"                                                            , "소방서"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(60105),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Registry office"                                                         , "등기소"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(60106),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Tax office"                                                              , "세무서"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(60107),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Customs"                                                                 , "세관"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(60108),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Weather/Rain gauge place"                                                , "기상/측우소"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(60109),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Post office"                                                             , "우체국"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(60110),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Telecommunication office"                                                , "전화국"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(60199),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Other public institution"                                                , "공공기관기타"])

           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(60200),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Institution for public order"                                            , "치안기관"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(60201),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Police station"                                                          , "경찰서"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(60202),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Police substation"                                                       , "파출소"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(60203),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Coast guard"                                                             , "해양경찰서"])

           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(60300),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Administrative agency"                                                   , "행정기관"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(60301),  i++, [CommonConstants.En , CommonConstants.Ko] , ["National assembly"                                                       , "국회"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(60302),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Assembly"                                                                , "의회"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(60303),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Central administrative agency"                                           , "중앙행정기관"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(60304),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Local administrative agency"                                             , "지방행정기관"])

           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(60400),  i++, [CommonConstants.En , CommonConstants.Ko] , ["City hall/Provincial government building"                                , "시청/도청"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(60401),  i++, [CommonConstants.En , CommonConstants.Ko] , ["City hall"                                                               , "시청"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(60402),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Provincial government building"                                          , "도청"])

           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(60500),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Country office/Ward office"                                              , "군청/구청"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(60501),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Country office"                                                          , "군청"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(60502),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Ward office"                                                             , "구청"])

           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(60600),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Eup/Myeon/Dong/Li office"                                                , "읍/면/동/리사무소"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(60601),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Dong office"                                                             , "동사무소"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(60602),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Eup office"                                                              , "읍사무소"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(60603),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Myeon office"                                                            , "면사무소"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(60604),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Li office"                                                               , "리사무소"])

           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(60700),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Educational institution"                                                 , "교육기관"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(60701),  i++, [CommonConstants.En , CommonConstants.Ko] , ["University"                                                              , "대학교"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(60702),  i++, [CommonConstants.En , CommonConstants.Ko] , ["High school"                                                             , "고등학교"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(60703),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Middle school"                                                           , "중학교"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(60704),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Elementary school"                                                       , "초등학교"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(60705),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Early childhood education"                                               , "유아교육"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(60706),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Special-education school "                                               , "특수학교"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(60707),  i++, [CommonConstants.En , CommonConstants.Ko] , ["International school"                                                    , "외국인학교"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(60708),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Private educational institute "                                          , "학원"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(60709),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Vocational training center "                                             , "직업훈련원"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(60710),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Traning institute"                                                       , "연수원"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(60711),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Service educational activities "                                         , "군교육기관"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(60799),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Other educational institution"                                           , "교육기관기타"])

           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(60800),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Mass media organization"                                                 , "언론기관"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(60801),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Broadcasting station"                                                    , "방송국"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(60802),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Newspaper"                                                               , "신문사"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(60803),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Radio"                                                                   , "라디오"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(60899),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Other mass media organization"                                           , "언론기관기타"])

           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(60900),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Foreign institutions"                                                    , "외국기관"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(60901),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Embassy"                                                                 , "대사관"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(60902),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Consulate "                                                              , "영사관"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(60903),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Foreign organization"                                                    , "외국단체"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(60904),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Cultural institute"                                                      , "문화원"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(69900),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Administrative agency"                                                   , "행정기관"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(69901),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Employment security center"                                              , "고용안정센터"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(69999),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Other administrative agency"                                             , "행정기관기타"])

           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(80100),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Restaurant"                                                               , "음식점"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(80101),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Korean food"                                                              , "한식"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(80102),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Japanese food"                                                            , "일식"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(80103),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Chinese food"                                                             , "중식"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(80104),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Western food"                                                             , "양식"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(80105),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Familyy restaurant"                                                       , "패밀리레스토랑"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(80106),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Fast food"                                                                , "패스트푸드"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(80107),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Specialty restaurant"                                                     , "전문음식점"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(80108),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Bakery"                                                                   , "제과점"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(80109),  i++, [CommonConstants.En , CommonConstants.Ko] , ["World food"                                                               , "세계요리"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(80110),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Buffet"                                                                   , "뷔페"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(80112),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Chicken"                                                                  , "치킨"])

           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(80200),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Coffee/Café"                                                              , "커피/카페"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(80201),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Café"                                                                     , "카페"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(80202),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Coffee shop"                                                              , "커피전문점"])

           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(80300),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Family ritual"                                                            , "가정의례"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(80301),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Wedding hall"                                                             , "예식장"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(80302),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Funeral hall"                                                             , "장례식장"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(80303),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Cemetery"                                                                 , "묘지"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(80304),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Charnel house "                                                           , "납골당"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(80305),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Crematorium "                                                             , "화장터"])

           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(80400),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Religious facility"                                                       , "종교시설"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(80401),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Related to church"                                                        , "교회관련"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(80402),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Related to Catholic church"                                               , "성당관련"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(80403),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Related to Buddhism"                                                      , "불교관련"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(80499),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Other religion"                                                           , "종교기타"])

           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(80500),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Theater"                                                                  , "극장"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(80501),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Theater"                                                                  , "극장"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(80502),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Drive-in theater "                                                        , "자동차극장"])

           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(80600),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Performance/Exhibition"                                                   , "공연/전시"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(80601),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Concert hall "                                                            , "공연장"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(80602),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Art museum"                                                               , "미술관"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(80603),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Museum"                                                                   , "박물관"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(80604),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Memorial hall"                                                            , "기념관"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(80605),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Exhibititon center"                                                       , "전시장"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(80607),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Science museum"                                                           , "과학관"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(80608),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Gallery/Exhibiti hall"                                                    , "화랑/전시관"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(80699),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Other"                                                                    , "기타"])

           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(80700),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Library/Cultural institute"                                               , "도서관/문화원"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(80701),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Library"                                                                  , "도서관"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(80702),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Cultural institute"                                                       , "문화원"])

           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(80800),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Lyceum "                                                                  , "문화회관"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(80801),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Lyceum "                                                                  , "문화회관"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(80802),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Cultural center"                                                          , "문화센터"])

           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(89900),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Living service"                                                           , "생활서비스"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(89901),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Pub"                                                                      , "술집"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(89902),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Nightclub"                                                                , "나이트클럽"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(89903),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Korean dry sauna"                                                         , "찜질방"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(89904),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Beauty salon"                                                             , "미용실"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(89905),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Barber's "                                                                , "이발소"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(89906),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Photo studio "                                                            , "사진관"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(89999),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Other living service"                                                     , "생활서비스기타"])

           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(90100),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Hospital"                                                                 , "병원"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(90101),  i++, [CommonConstants.En , CommonConstants.Ko] , ["General hospital"                                                         , "종합병원"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(90102),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Hospital"                                                                 , "병원"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(90103),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Community health center"                                                  , "보건소/지소"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(90104),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Army hospital"                                                            , "국군병원"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(90105),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Police hospital "                                                         , "경찰병원"])

           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(90200),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Doctor's office"                                                          , "의원"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(90201),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Doctor's office"                                                          , "의원"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(90202),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Clinic"                                                                   , "크리닉"])

           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(90300),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Oriental medical clinic"                                                  , "한방한의원"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(90301),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Oriental medicine hospital "                                              , "한방병원"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(90302),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Chinese acupuncture and herbal clinic"                                    , "한의원"])

           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(99900),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Medical facility"                                                         , "의료시설"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(99901),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Casualty"                                                                 , "응급실"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(99902),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Sanatorium"                                                               , "요양원"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(99903),  i++, [CommonConstants.En , CommonConstants.Ko] , ["National medical center"                                                  , "국립의료원"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(99904),  i++, [CommonConstants.En , CommonConstants.Ko] , ["National institute of health"                                             , "질병관리본부"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(99905),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Pharmacy"                                                                 , "약국"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(99906),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Medical examination center"                                               , "건강진단센터"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(99907),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Blood bank "                                                              , "혈액원"])
           CommonUtils.insertEnum(item3DepthEnum , CommonUtils.string(99999),  i++, [CommonConstants.En , CommonConstants.Ko] , ["Other medical facility"                                                   , "의료시설기타"])


           CommonUtils.fetchArr(item3DepthEnum , CommonConstants.Ko , item3DepthKoArr)
           CommonUtils.fetchArr(item3DepthEnum , CommonConstants.En , item3DepthEnArr)

       }


    function allocateSoftKeyEnum() {
        if (CommonUtils.getObjectLen(softKeyEnum) <= 0) {
            softKeyEnum = CommonUtils.createEnum(softKeyEnum)
        }
        var i = 0
        CommonUtils.insertEnum(softKeyEnum , "Back"      , i++, [CommonConstants.Name] , [ZoneConstants.Back     ])
        CommonUtils.insertEnum(softKeyEnum , "Option"    , i++, [CommonConstants.Name] , [ZoneConstants.Option   ])
        CommonUtils.insertEnum(softKeyEnum , "Navi"      , i++, [CommonConstants.Name] , [ZoneConstants.Navi     ])
        CommonUtils.insertEnum(softKeyEnum , "Menu"      , i++, [CommonConstants.Name] , [ZoneConstants.Menu     ])

        CommonUtils.fetchArr(softKeyEnum, CommonConstants.Name, softKeyMenuArr)
    }

    function allocateMenuEnum(){
        allocateMenuKoEnum()
        if (CommonUtils.getObjectLen(menuEnum) <= 0) {
            menuEnum = CommonUtils.createEnum(menuEnum)
        }
        var i = 0
        CommonUtils.insertEnum( menuEnum , "TrafficConvenience"    , i++
                                                                   , [CommonConstants.Name                                          , SearchConstants.Code    , CommonConstants.Normal , CommonConstants.Press, CommonConstants.Focus]
                                                                   , [qsTr(menuNameKoArr[menuKoEnum.TrafficConvenience])            , undefined               , "category_ic_1.png"    , "category_ic_1.png"  , "category_ic_1_f.png"  ])
        CommonUtils.insertEnum( menuEnum , "AutomobileConvenience" , i++
                                                                   , [CommonConstants.Name                                          , SearchConstants.Code    , CommonConstants.Normal , CommonConstants.Press, CommonConstants.Focus]
                                                                   , [qsTr(menuNameKoArr[menuKoEnum.AutomobileConvenience])         , undefined               , "category_ic_2.png"    , "category_ic_2.png"  , "category_ic_2_f.png"  ])
        CommonUtils.insertEnum( menuEnum , "BuildingFacilities"    , i++
                                                                   , [CommonConstants.Name                                          , SearchConstants.Code    , CommonConstants.Normal , CommonConstants.Press, CommonConstants.Focus]
                                                                   , [qsTr(menuNameKoArr[menuKoEnum.BuildingFacilities])            , undefined               , "category_ic_3.png"    , "category_ic_3.png"  , "category_ic_3_f.png"  ])
        CommonUtils.insertEnum( menuEnum , "TravelLeisure"         , i++
                                                                   , [CommonConstants.Name                                          , SearchConstants.Code    , CommonConstants.Normal , CommonConstants.Press, CommonConstants.Focus]
                                                                   , [qsTr(menuNameKoArr[menuKoEnum.TravelLeisure])                 , undefined               , "category_ic_4.png"    , "category_ic_4.png"  , "category_ic_4_f.png"  ])
        CommonUtils.insertEnum( menuEnum , "Shopping"              , i++
                                                                   , [CommonConstants.Name                                          , SearchConstants.Code    , CommonConstants.Normal , CommonConstants.Press, CommonConstants.Focus]
                                                                   , [qsTr(menuNameKoArr[menuKoEnum.Shopping])                      , undefined               , "category_ic_5.png"    , "category_ic_5.png"  , "category_ic_5_f.png"  ])
        CommonUtils.insertEnum( menuEnum , "PublicConvenience"     , i++
                                                                   , [CommonConstants.Name                                          , SearchConstants.Code    , CommonConstants.Normal , CommonConstants.Press, CommonConstants.Focus]
                                                                   , [qsTr(menuNameKoArr[menuKoEnum.PublicConvenience])             , undefined               , "category_ic_6.png"    , "category_ic_6.png"  , "category_ic_6_f.png"  ])
        CommonUtils.insertEnum( menuEnum , "LifeConvenience"       ,  i++
                                                                   , [CommonConstants.Name                                          , SearchConstants.Code    , CommonConstants.Normal , CommonConstants.Press, CommonConstants.Focus]
                                                                   , [qsTr(menuNameKoArr[menuKoEnum.LifeConvenience])               , undefined               , "category_ic_7.png"    , "category_ic_7.png"  , "category_ic_7_f.png"  ])
        CommonUtils.insertEnum( menuEnum , "MedicalConvenience"    , i++
                                                                   , [CommonConstants.Name                                          , SearchConstants.Code    , CommonConstants.Normal , CommonConstants.Press, CommonConstants.Focus]
                                                                   , [qsTr(menuNameKoArr[menuKoEnum.MedicalConvenience])            , undefined               , "category_ic_8.png"    , "category_ic_8.png"  , "category_ic_8_f.png"  ])

        CommonUtils.fetchArr(menuEnum , CommonConstants.Name , menuNameArr)
        CommonUtils.fetchArr(menuEnum , SearchConstants.Code , menuCodeArr)
        CommonUtils.fetchArr(menuEnum , CommonConstants.Normal , menuImgNArr)
        CommonUtils.fetchArr(menuEnum , CommonConstants.Press , menuImgPArr)
        CommonUtils.fetchArr(menuEnum , CommonConstants.Focus , menuImgFArr)
        if (!CommonUtils.isNull(gridCol)) {
           gridRow = CommonUtils.getGridRowCol(menuNameArr, gridCol)
        } else if (!CommonUtils.isNull(gridRow)) {
           gridCol = CommonUtils.getGridRowCol(menuNameArr, gridRow)
        }
    }

    function allocateDepthEnum(){
        if (CommonUtils.getObjectLen(depthEnum) <= 0) {
            depthEnum = CommonUtils.createEnum(depthEnum)
        }
        var i = 0
        CommonUtils.insertEnum(depthEnum , "None"   ,IndustrySearchViewEvent.DepthNone)
        CommonUtils.insertEnum(depthEnum , "First"  ,IndustrySearchViewEvent.DepthFirst)
        CommonUtils.insertEnum(depthEnum , "Second" ,IndustrySearchViewEvent.DepthSecond)
        CommonUtils.insertEnum(depthEnum , "Third"  ,IndustrySearchViewEvent.DepthThird)
    }

    function allocateReqEnum(){
        if (CommonUtils.getObjectLen(reqEnum) <= 0) {
            reqEnum = CommonUtils.createEnum(reqEnum)
        }
        var i = 0
        CommonUtils.insertEnum(reqEnum , "StartPageIndex",  i++)
    }

    function allocateResEnum(){
        if (CommonUtils.getObjectLen(resEnum) <= 0) {
            resEnum = CommonUtils.createEnum(resEnum)
        }
        CommonUtils.insertEnum(resEnum , "Name"  , IndustrySearchViewEvent.ResultName)
        CommonUtils.insertEnum(resEnum , "Code"  , IndustrySearchViewEvent.ResultCode)
        CommonUtils.insertEnum(resEnum , "Count" , IndustrySearchViewEvent.ResultCount)
    }

    //[주의]한글 메뉴를 사용하여 code값을 얻는 로직때문... 언어 변환하지 말것
    function allocateMenuKoEnum() {
        if (CommonUtils.getObjectLen(menuKoEnum) <= 0) {
            menuKoEnum = CommonUtils.createEnum(menuKoEnum)
        }
        var i = 0
        CommonUtils.insertEnum( menuKoEnum , "TrafficConvenience"    , i++, [CommonConstants.Name], [item1DepthKoArr[item1DepthEnum[CommonUtils.string(10000)]]])
        CommonUtils.insertEnum( menuKoEnum , "AutomobileConvenience" , i++, [CommonConstants.Name], [item1DepthKoArr[item1DepthEnum[CommonUtils.string(20000)]]])
        CommonUtils.insertEnum( menuKoEnum , "BuildingFacilities"    , i++, [CommonConstants.Name], [item1DepthKoArr[item1DepthEnum[CommonUtils.string(30000)]]])
        CommonUtils.insertEnum( menuKoEnum , "TravelLeisure"         , i++, [CommonConstants.Name], [item1DepthKoArr[item1DepthEnum[CommonUtils.string(40000)]]])
        CommonUtils.insertEnum( menuKoEnum , "Shopping"              , i++, [CommonConstants.Name], [item1DepthKoArr[item1DepthEnum[CommonUtils.string(50000)]]])
        CommonUtils.insertEnum( menuKoEnum , "PublicConvenience"     , i++, [CommonConstants.Name], [item1DepthKoArr[item1DepthEnum[CommonUtils.string(60000)]]])
        CommonUtils.insertEnum( menuKoEnum , "LifeConvenience"       , i++, [CommonConstants.Name], [item1DepthKoArr[item1DepthEnum[CommonUtils.string(80000)]]])
        CommonUtils.insertEnum( menuKoEnum , "MedicalConvenience"    , i++, [CommonConstants.Name], [item1DepthKoArr[item1DepthEnum[CommonUtils.string(90000)]]])

        CommonUtils.fetchArr(menuKoEnum , CommonConstants.Name , menuNameKoArr)
    }

    property bool isShowProgressIndicator: false
    function showProgressView() {
        reqSearch = true
        sendEvent(IndustrySearchViewEvent.ShowProgressIndicator)
        isShowProgressIndicator = true
    }

    function hideProgressView() {
        sendEvent(IndustrySearchViewEvent.HideProgressIndicator)
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
        sendEvent(IndustrySearchViewEvent.RequestSetting, data )
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

    function viewSizeAndModeChanged(height){
        if (CommonUtils.isEqual(searchRoot.height, height))
            return
        var data = ({})
        data[CommonConstants.B] = height
        sendEvent(IndustrySearchViewEvent.ViewSizeChanged, data)

        requestDirectionGuid()
    }

    function requestSearch(code, requestPageIndex, maxCount, depth) {
        showProgressView()

        searchRoot.query = code
        searchRoot.currDepth = depth

        var data = ({})
        data[SearchConstants.Query       ] = code
        //부가 정보
        var params = ({})
        params[SearchConstants.StartIndex] = (requestPageIndex) ? (requestPageIndex * listView.maxCount)  : 0
        params[SearchConstants.Count     ] = maxCount
        params[SearchConstants.Depth     ] = depth
        //부가 정보(대입)
        data[SearchConstants.Params      ] = params
        sendEvent(IndustrySearchViewEvent.RequestSearch, data )

        //[주의] 검색 요청이 블럭되는 경우 처리를 위하여 ...
        queueChanged(0)
    }

    property var timerCloseCheck : null
    function createSearchResultView(code, text) {
        var data = ({})
        data[CommonConstants.Url        ] = CommonConstants.url(ViewRegistry.SearchResultView)
        data[CommonConstants.Caller     ] = caller
        data[CommonConstants.Owner      ] = CommonConstants.objectName(ViewRegistry.IndustrySearchView)
        data[CommonConstants.Visible    ] = SearchConstants.DialogNoResult
        data[SearchConstants.SearchType ] = CommonConstants.Industry
        data[SearchConstants.Query      ] = text
        data[SearchConstants.TextParam  ] = code
        data[SearchConstants.AddrCode   ] = 0
        data[SearchConstants.Favorite   ] = CommonUtils.isIndexOf(owner, [CommonConstants.objectName(ViewRegistry.BookmarkPageView)])
        data[SearchConstants.IsHome     ] = CommonUtils.isIndexOf(owner, [textArr[textEnum.Slash] + SearchConstants.Home])
        data[SearchConstants.IsOffice   ] = CommonUtils.isIndexOf(owner, [textArr[textEnum.Slash] + SearchConstants.Office])
        data[SearchConstants.ViaSetting ] = CommonUtils.isIndexOf(caller, CommonConstants.objectName(ViewRegistry.RouteSettingView))

        if (CommonUtils.number(code) >= SearchConstants.OILBRANDBASE && CommonUtils.number(code) < SearchConstants.OILBRANDBASE +  SearchConstants.CATEGORYGAP)
            data[SearchConstants.Kind] = SearchConstants.CategoryGasSt
        else if (CommonUtils.number(code) >= SearchConstants.CHARGINGBASE && CommonUtils.number(code) < SearchConstants.CHARGINGBASE +  SearchConstants.CATEGORYGAP)
            data[SearchConstants.Kind] = SearchConstants.CategoryChargingSt
        else if (CommonUtils.number(code) >= SearchConstants.EVCHARGINGBASE && CommonUtils.number(code) < SearchConstants.EVCHARGINGBASE +  SearchConstants.CATEGORYGAP)
            data[SearchConstants.Kind] = SearchConstants.CategoryChargingEvSt
        else
            data[SearchConstants.Kind] = null

        //data[CommonConstants.Z          ] = CommonConstants.MAX_INT_VALUE
        data[CommonConstants.ReFocus    ] = true
        data[CommonConstants.ObjectName ] = CommonConstants.objectName(ViewRegistry.SearchResultView)
        sendEvent(IndustrySearchViewEvent.UpdateView, data)

        sendEvent(IndustrySearchViewEvent.CreateView, data)

    }

    function createModel(_result, startIndex, listModel) {
        var i = null
        var text = null
        var code = null
        var len = (_result) ? _result.length : 0
        var index =  startIndex

        if (CommonUtils.isEqual(listModel , grid1DepthModel)) {
            len = menuNameArr.length
            for(i = 0; i < len; i++) {
                text = menuNameArr[i]
                code = menuCodeArr[i]
                if ( listModel.count >  ++index) {
                    listModel.set (index, {text: text, code: code})
                } else {
                    listModel.append( {text: text, code: code})
                }
            }

            // 1Depth: apply empty
            for(i=len; i<gridRow*gridCol; i++) {
                if ( listModel.count >  ++index) {
                    listModel.set (index, {text: CommonConstants.Empty, code: -1})
                } else {
                    listModel.append( {text: CommonConstants.Empty, code:-1})
                }
            }
        }else {
            for (i = 0; i< len; i++) {
                var result = _result[i]
                text = result[resEnum.Name]
                code = result[resEnum.Code]
                if (CommonUtils.indexOf(text, textArr[textEnum.LessThan]) >= 0)
                    continue
                if ( listModel.count >  ++index) {
                    listModel.set (index, {text: text, code: code})
                } else {
                    listModel.append( {text: text, code: code})
                }
            }
        }

        messageText = textArr[textEnum.NoData]
        messageVisible = listModel.count ? false : true
    }

    function enableMouseArea(visible) {
        searchRoot.background.visible = backgroundVisible = visible
        searchRoot.mouseArea.enabled  = mouseAreaEnabled = visible
    }

    function setTitle(model,depth) {
        switch(depth) {
            case depthEnum.First : grid1DepthView.reqModel = model; break;
            case depthEnum.Second: list2DepthView.reqModel = model; break;
            case depthEnum.Third : list3DepthView.reqModel = model; break;
        }
    }

    function getTitle(depth) {
        var model = {}
        switch(depth) {
            case depthEnum.First:
                model = {code: item1DepthEnum["0"], text: item1DepthKoArr[item1DepthEnum[CommonUtils.string(0)]]}
                break
            case depthEnum.Second:
                model = grid1DepthView.reqModel
                break
            case depthEnum.Third:
                model = list2DepthView.reqModel
                break
        }
        return getItemTextFromModel(model, depth)
    }

    function getMenuCode(_result, name) {
        for(var i = 0; i< _result.length; i++) {
            var result = _result[i]
            if (CommonUtils.isEqual(result[resEnum.Name] , name))
                return result[resEnum.Code]
        }
        return -1
    }

    function getItemTextFromModel(model, depth) {
        var index = null
        var txt   = model.text
        switch(depth) {
            case depthEnum.None:
            case depthEnum.First :
                index = item1DepthEnum[CommonUtils.string(model.code)]
                if (CommonUtils.isNull(index))
                    break
                txt = CommonUtils.isEqual(searchRoot.__lang, CommonConstants.En) ? item1DepthEnArr[index] : item1DepthKoArr[index]
                break
            case depthEnum.Second:
                index = item2DepthEnum[CommonUtils.string(model.code)]
                if (CommonUtils.isNull(index))
                    break
                txt = CommonUtils.isEqual(searchRoot.__lang, CommonConstants.En) ? item2DepthEnArr[index] : item2DepthKoArr[index]
                break
            case depthEnum.Third :
                index = item3DepthEnum[CommonUtils.string(model.code)]
                if (CommonUtils.isNull(index))
                    break
                txt = CommonUtils.isEqual(searchRoot.__lang, CommonConstants.En) ? item3DepthEnArr[index] : item3DepthKoArr[index]
                break
        }
        return txt
    }

    function changeListView(depth) {
        switch (depth) {
           case depthEnum.First:
               listModel = grid1DepthModel
               listView = grid1DepthView
               sendEvent(IndustrySearchViewEvent.SelectZone1Title, getTitle(depth))
               if (CommonUtils.isIndexOf(caller, [CommonConstants.objectName(ViewRegistry.BookmarkPageView)]) ||
                   CommonUtils.isIndexOf(owner , [CommonConstants.objectName(ViewRegistry.BookmarkPageView)])) {
                   tileMenuSpaceWidth = 50;gridRow = 3; gridCol = 3
                   tileMenuContentButtonYOffset = searchRoot.layout.tileMenuExContentButtonYOffset
                   tileMenuContentTextYOffset = searchRoot.layout.tileMenuExContentTextYOffset
                   frameColor = GlobalConstants.kBasicBackgoundColor
                   enableMouseArea(true)
               } else if (CommonUtils.isIndexOf(caller, [CommonConstants.objectName(ViewRegistry.RouteSettingView)]) ||
                          CommonUtils.isIndexOf(owner , [CommonConstants.objectName(ViewRegistry.RouteSettingView)])) {
                   tileMenuSpaceWidth = 50;gridRow = 3; gridCol = 3
                   tileMenuContentButtonYOffset = searchRoot.layout.tileMenuExContentButtonYOffset
                   tileMenuContentTextYOffset = searchRoot.layout.tileMenuExContentTextYOffset
                   frameColor = GlobalConstants.kBasicBackgoundColor
                   enableMouseArea(true)
               }  else {
                   tileMenuContentButtonYOffset = searchRoot.layout.tileMenuContentButtonYOffset
                   tileMenuContentTextYOffset = searchRoot.layout.tileMenuContentTextYOffset
                   tileMenuOutlineColor = GlobalConstants.kBasicBackgoundColor
                   frameColor = GlobalConstants.kNormalColor
                   enableMouseArea(false)
               }
               break
           case depthEnum.Second:
               listModel = list2DepthModel
               listView = list2DepthView
               sendEvent(IndustrySearchViewEvent.SelectZone1Title, getTitle(depth))
               frameColor = GlobalConstants.kBasicBackgoundColor
               enableMouseArea(true)
               break
           case depthEnum.Third:
               listModel = list3DepthModel
               listView = list3DepthView
               sendEvent(IndustrySearchViewEvent.SelectZone1Title, getTitle(depth))
               frameColor = GlobalConstants.kBasicBackgoundColor
               enableMouseArea(true)
               break
        }
        messageVisible = false
        grid1DepthView.visible = CommonUtils.isEqual(depth , depthEnum.First)  ? true : false
        list2DepthView.visible = CommonUtils.isEqual(depth , depthEnum.Second) ? true : false
        list3DepthView.visible = CommonUtils.isEqual(depth , depthEnum.Third)  ? true : false

        sendEvent(IndustrySearchViewEvent.DepthChanged, depth)
    }

    function setBtnProperty(obj, model) {
        var index = model.index
        var imgN = menuImgNArr[index]
        var imgP = menuImgPArr[index]
        obj.row = Qt.binding(function(){ return index / gridCol})
        obj.col = Qt.binding(function(){ return index % gridCol})
        obj.width = Qt.binding(function(){ return grid1DepthView.rowWidth})
        obj.height = Qt.binding(function(){ return grid1DepthView.rowHeight})
        obj.box.colorN = Qt.binding(function(){ return CommonConstants.Transparent})
        obj.box.colorP = Qt.binding(function(){ return CommonConstants.Transparent})
        if (!CommonUtils.isNull(menuNameArr[index])){
            obj.img.width                   = Qt.binding(function(){ return tileMenuImgSize.width})
            obj.img.height                  = Qt.binding(function(){ return tileMenuImgSize.height})
            obj.img.sourceN                 = Qt.binding(function(){ return !CommonUtils.isEmpty(imgN) ? Resource.imageUrl("industrymenu", imgN) : CommonConstants.Empty})
            obj.img.sourceP                 = Qt.binding(function(){ return !CommonUtils.isEmpty(imgP) ? Resource.imageUrl("industrymenu", imgP) : CommonConstants.Empty})
            obj.img.y                       = Qt.binding(function(){ return obj.contentButtonYOffset})
            obj.img.opacity                 = Qt.binding(function(){ return !CommonUtils.isNull(menuCodeArr[index]) ? 1.0 : 0.3})
            obj.img.anchors.horizontalCenter= Qt.binding(function(){ return obj.box.horizontalCenter})
            obj.txt.text                    = Qt.binding(function(){ return getItemTextFromModel(model, currDepth)})
            obj.txt.anchors.centerIn        = Qt.binding(function(){ return undefined})
            obj.txt.x                       = Qt.binding(function(){ return 0})
            obj.txt.y                       = Qt.binding(function(){ return obj.contentTextYOffset})
            obj.txt.width                   = Qt.binding(function(){ return obj.width})
            obj.txt.horizontalAlignment     = Qt.binding(function(){ return Qt.AlignHCenter})
            obj.txt.font.family             = Qt.binding(function(){ return GlobalConstants.kFontFamily})
            obj.txt.font.pixelSize          = Qt.binding(function(){ return Resource.pixelToDp(GlobalConstants.kOneLineListTextFontSize)})
            obj.txt.colorN                  = Qt.binding(function(){ return !CommonUtils.isNull(menuCodeArr[index]) ? GlobalConstants.kListTextColor : CommonConstants.Dark + CommonConstants.Gray})
            obj.txt.colorP                  = Qt.binding(function(){ return !CommonUtils.isNull(menuCodeArr[index]) ? GlobalConstants.kListTextColor : CommonConstants.Dark + CommonConstants.Gray})
            obj.txt.lineHeightMode          = Qt.binding(function(){ return Text.FixedHeight})
            obj.txt.lineHeight              = Qt.binding(function(){ return searchRoot.layout.tileMenuContentTextLineHeight})
        }
        obj.enabledOpacity                  = Qt.binding(function(){ return (!CommonUtils.isEmpty(menuNameArr[index])) ? false : true})
        obj.enabled                         = Qt.binding(function(){ return (!CommonUtils.isEmpty(menuNameArr[index])) ? true : false})
    }

    function btnClickItem(obj, model) {
        if (!obj.enabled)
            return
       listView.currentIndex = model.index
       if (!obj.isFocused) {
            highlightColor = Qt.binding(function(){ return MultiSenseCoordinator.kAmbientColor})
       }
       var sleep =  GlobalConstants.kPressedColorExposureTime
       timer1DepthMenu = CommonUtils.destroy(timer1DepthMenu)
       timer1DepthMenu = CommonUtils.delay(searchRoot, sleep, function() {
           try {
               highlightColor = CommonConstants.Transparent
               listView.currentIndex = -1
               var index = model.index
               setTitle(model, currDepth)
               searchRoot.query = menuCodeArr[index]
               requestSearch(searchRoot.query, reqEnum.StartPageIndex, listView.maxCount, ++currDepth)
           } catch(e) {
           }
       })
    }

    function listClickItem(obj, model) {
        listView.currentIndex = model.index
        if (!obj.isFocused) {
            highlightColor = Qt.binding(function(){ return MultiSenseCoordinator.kAmbientColor})
        }
        var sleep =  GlobalConstants.kPressedColorExposureTime
        timer2or3DepthMenu = CommonUtils.destroy(timer2or3DepthMenu)
        timer2or3DepthMenu = CommonUtils.delay(searchRoot, sleep, function() {
            try {
                highlightColor = CommonConstants.Transparent
                listView.currentIndex = -1
                processByModel(model)
            } catch(e) {
            }
        })
    }


    function initEnum() {
        allocateTextEnum()
        allocateItem1DepthEnum()
        allocateItem2DepthEnum()
        allocateItem3DepthEnum()
        allocateSoftKeyEnum()
        allocateMenuEnum()
        allocateDepthEnum()
        allocateReqEnum()
        allocateResEnum()
    }

    function init(){
        initEnum()

        loader.sourceComponent = undefined
        loader.sourceComponent = main
    }

    onReFocusChanged: {
        if (!reFocus)
            return
        reFocus = false
        if (isMediated) {
            //viewActivated()
        }
    }

    onMediated: {
        sendEvent(IndustrySearchViewEvent.RequestLang)

        enableMouseArea(false)
        init()

        grid1DepthView.visible = false
        list2DepthView.visible = false
        list3DepthView.visible = false

        searchRoot.query = 0
        sendEvent(IndustrySearchViewEvent.Mediated )

        requestSearch(searchRoot.query, reqEnum.StartPageIndex, searchRoot.reqMaxCount, depthEnum.First)
        isMediated = true
    }

    onActivated: {
        allocateTextEnum()
//        if (isShowTopView) {
//            return
//        }
        enabled = true
        sendEvent(IndustrySearchViewEvent.ResetCsw)
        sendEvent(IndustrySearchViewEvent.RegistCsw, [ csw ])
        sendEvent(IndustrySearchViewEvent.Activated,[CommonConstants.NoOption])
        viewSizeAndModeChanged(frameHeight)
        if (CommonUtils.isEqual(currDepth , depthEnum.Third)) {
            sendEvent(IndustrySearchViewEvent.SelectZone1Title, getTitle(currDepth))
        }
    }
    onDeactivated: {
        enabled = false
        sendEvent(IndustrySearchViewEvent.Deactivated)
    }

    function __onResultViewList(result) {
        for(var i= 0; i<result.length; i++) {
            if (CommonUtils.isIndexOf(result[i][CommonConstants.ResultViewName],
                                      [CommonConstants.objectName(ViewRegistry.SearchResultView),
                                       CommonConstants.objectName(ViewRegistry.SearchResultDetailView)])) {
                if (result[i][CommonConstants.ResultViewVisible]) {
                    isShowTopView = true
                    return
                }
            }
        }
        isShowTopView = false
    }

    Component.onDestruction: {
        timerProg = CommonUtils.destroy(timerProg)
        timer1DepthMenu = CommonUtils.destroy(timer1DepthMenu)
        timer2or3DepthMenu = CommonUtils.destroy(timer2or3DepthMenu)
    }

    //검색 요청을 갯수를 얻음
    property var timerProg: null
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
              timerProg = CommonUtils.delay(searchRoot, SearchConstants.NetworkTimeoutAPP, function() {
                 stopProgressView()
              }, false)
          }
    }

    function __onResult(search, result, totalCount, startIndex, depth) {
        processResult(search, result, totalCount, startIndex, depth)
    }

    function processResult(search, result, totalCount, startIndex, depth) {
        stopProgressView()

        startIndex = Math.max(0, startIndex)
        if (startIndex <= 0) {
            if (CommonUtils.isEqual(depth , depthEnum.First)) {
                for(var i = 0; i< menuNameArr.length; i++)
                     menuCodeArr[i] = getMenuCode(result, menuNameKoArr[i])
            }
            changeListView(depth)
            listModel.clear()
            listView.contentHeight      = 0
            listView.contentY           = 0
            listView.currentIndex       = -1
            listView.totalPage          = 0
            listView.requestPageIndex   = 0
            listView.totalPage          = totalCount / listView.maxCount
            listView.totalPage         += (totalCount % listView.maxCount) ? 1 : 0
        }
        createModel(result, startIndex, listModel)
    }

    function processByModel(model) {
        if ( CommonUtils.isEqual(currDepth , depthEnum.Third)) {
            createSearchResultView(model.code, model.text)
        } else {
            setTitle(model, currDepth)
            searchRoot.query = model.code
            requestSearch(searchRoot.query, reqEnum.StartPageIndex, listView.maxCount, ++currDepth)
        }
    }

    //언어가 변경되었을 때
    function __retranslateUi(lang) {
        searchRoot.__lang = lang
        needInit = true
        if (active) {
            allocateTextEnum()
//            if (isShowTopView) {
//                return
//            }
            sendEvent(IndustrySearchViewEvent.SelectZone1Title, getTitle(currDepth))
        }
    }

    function __doSoftKeyFunction(button, menuName, menuIndex) {
        switch(button) {
          case softKeyMenuArr[softKeyEnum.Back]:
            sendEvent(IndustrySearchViewEvent.RemoveSearchQueue)
            if (currDepth > depthEnum.First) {
                changeListView(--currDepth)
            } else {
                if (CommonUtils.isIndexOf(caller, [CommonConstants.objectName(ViewRegistry.BookmarkPageView)])) {
                    sendEvent(IndustrySearchViewEvent.ReturnToFavorite)
                } else if (CommonUtils.isIndexOf(caller, [CommonConstants.objectName(ViewRegistry.RouteSettingView)])) {
                    sendEvent(IndustrySearchViewEvent.ReturnToViaSetting)
                } else if (CommonUtils.isIndexOf(caller, [CommonConstants.objectName(ViewRegistry.NaviMenuView)])) {
                    sendEvent(IndustrySearchViewEvent.ReturnToNaviMenu)
                }

                var data = ({})
                data[CommonConstants.ObjectName] = CommonConstants.objectName(ViewRegistry.IndustrySearchView)
                sendEvent(IndustrySearchViewEvent.CloseView, data)
            }
            break
          case softKeyMenuArr[softKeyEnum.Option]:
          case softKeyMenuArr[softKeyEnum.Navi]:
          case softKeyMenuArr[softKeyEnum.Menu]:
          default:
              break
        }
    }

    //main
    Loader {
         id: loader
         width: parent.width
         height: parent.height
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
            Item {
               width: parent.width
               height: parent.height - frameHeight
            }
            Rectangle {
                width: parent.width
                height: frameHeight
                color: frameColor
                Column {
                    width: parent.width
                    height: parent.height
                    Row {width: parent.width;height: parent.height - 1
                        Rectangle {width:tileMenuSpaceWidth;height:parent.height;color: frameColor
                            onWidthChanged: CommonUtils.applyRemainWidth(parent.children[1],parent)
                        }
                        GridView {
                            Component.onCompleted: {searchRoot.grid1DepthView = this;CommonUtils.applyRemainWidth(this,parent)}
                            width: parent.width
                            height: parent.height
                            cellWidth: (width/gridCol)
                            cellHeight: (height/gridRow)
                            clip: true
                            model: grid1DepthModel
                            delegate: gridDelegate
                            currentIndex: -1
                            interactive: false
                            property int rowWidth: cellWidth
                            property int rowHeight: cellHeight
                            property int maxCount: searchRoot.reqMaxCount
                            property int totalPage: 0
                            property int requestPageIndex: 0
                            property var reqModel: ({})

                            property var pressedItem: null
                            PressBar { currentItem: grid1DepthView.pressedItem;opacity: 0.3 }
                            highlight: HighlightBar {
                                currentItem: grid1DepthView.currentItem
                                color: highlightColor
                            }
                            highlightFollowsCurrentItem: false


                            TScrollBar {
                                id: scrollbar1Depth
                                flickable: parent
                                width: 2
                                dynamicPage: grid1DepthView.requestPageIndex
                                color: MultiSenseCoordinator.kAmbientColor
                                grooveColor: GlobalConstants.kScrollBarBackColor
                                rightMargin: GlobalConstants.kScrollBarRightMargin
                            }

                            onAtYEndChanged: {
                                if (!scrollbar1Depth.visible) {
                                    return
                                }
                                if (!atYEnd) {
                                     return
                                }
                                cancelFlick()
                                if (CommonUtils.isNull(grid1DepthView)                ||
                                    CommonUtils.isNull(grid1DepthView.totalPage)      ||
                                    CommonUtils.isNull(grid1DepthView.requestPageIndex) ) {
                                    return
                                }
                                if ( (grid1DepthView.totalPage - 1) <= grid1DepthView.requestPageIndex) {
                                    return
                                }
                                requestSearch(searchRoot.query, ++grid1DepthView.requestPageIndex, grid1DepthView.maxCount, depthEnum.First)
                            }
                        }
                        Rectangle {width:tileMenuSpaceWidth;height:parent.height;color:frameColor
                            onWidthChanged: CommonUtils.applyRemainWidth(parent.children[1],parent)
                        }
                    }
                    Rectangle{ width: parent.width; height: 1; color: GlobalConstants.kBasicBackgoundColor}
                }
                ListView  {
                    Component.onCompleted: searchRoot.list2DepthView = this
                    width: parent.width
                    height: parent.height
                    property int rowHeight: searchRoot.layout.rowHeight
                    property int maxCount: searchRoot.reqMaxCount
                    property int totalPage: 0
                    property int requestPageIndex: 0
                    property var reqModel: ({})

                    property var pressedItem: null
                    PressBar {
                        currentItem: list2DepthView.pressedItem
                        leftMargin: GlobalConstants.kListBackgroundLeftMargin
                        rightMargin: GlobalConstants.kListBackgroundRightMargin
                    }
                    highlight: HighlightBar {
                        currentItem: list2DepthView.currentItem
                        leftMargin: GlobalConstants.kListBackgroundLeftMargin
                        rightMargin: GlobalConstants.kListBackgroundRightMargin
                        color: highlightColor
                    }
                    highlightFollowsCurrentItem: false

                    clip: true
                    model: list2DepthModel
                    delegate: listDelegate
                    currentIndex: -1


                    Image {
                          id: imgMask2
                          width: parent.width
                          height: 30
                          y: parent.y + (parent.height - height)
                          visible: scrollbar2Depth.heightRatio < 1.0 && !list2DepthView.dragging
                          sourceSize.width: width
                          sourceSize.height: height
                          source: Resource.imageUrl("zone/zone2_img_mask.png")
                    }

                    TScrollBar {
                        id: scrollbar2Depth
                        flickable: parent
                        width: 2
                        dynamicPage: list2DepthView.requestPageIndex
                        color: MultiSenseCoordinator.kAmbientColor
                        grooveColor: GlobalConstants.kScrollBarBackColor
                        rightMargin: GlobalConstants.kScrollBarRightMargin
                    }

                    onAtYEndChanged: {
                        if (!scrollbar2Depth.visible) {
                            return
                        }
                        if (!atYEnd) {
                             return
                        }
                        cancelFlick()
                        if (CommonUtils.isNull(list2DepthView)                ||
                            CommonUtils.isNull(list2DepthView.totalPage)      ||
                            CommonUtils.isNull(list2DepthView.requestPageIndex) ) {
                            return
                        }

                        if ( (list2DepthView.totalPage - 1) <= list2DepthView.requestPageIndex) {
                            return
                        }
                        requestSearch(searchRoot.query, ++list2DepthView.requestPageIndex, list2DepthView.maxCount, depthEnum.Second)
                    }
                }
                ListView  {
                    Component.onCompleted: searchRoot.list3DepthView = this
                    width: parent.width
                    height: parent.height
                    property int rowHeight:  searchRoot.layout.rowHeight
                    property int maxCount: searchRoot.reqMaxCount
                    property int totalPage: 0
                    property int requestPageIndex: 0
                    property var reqModel: ({})

                    property var pressedItem: null
                    PressBar {
                        currentItem: list3DepthView.pressedItem
                        leftMargin: GlobalConstants.kListBackgroundLeftMargin
                        rightMargin: GlobalConstants.kListBackgroundRightMargin
                    }
                    highlight: HighlightBar {
                        currentItem: list3DepthView.currentItem
                        leftMargin: GlobalConstants.kListBackgroundLeftMargin
                        rightMargin: GlobalConstants.kListBackgroundRightMargin
                        color: highlightColor
                    }
                    highlightFollowsCurrentItem: false

                    clip: true
                    model: list3DepthModel
                    delegate: listDelegate
                    currentIndex: -1

                    Image {
                          id: imgMask3
                          width: parent.width
                          height: 30
                          y: parent.y + (parent.height - height)
                          visible: scrollbar3Depth.heightRatio < 1.0 && !list3DepthView.dragging
                          sourceSize.width: width
                          sourceSize.height: height
                          source: Resource.imageUrl("zone/zone2_img_mask.png")
                    }

                    TScrollBar {
                        id: scrollbar3Depth
                        flickable: parent
                        width: 2
                        dynamicPage: list3DepthView.requestPageIndex
                        color: MultiSenseCoordinator.kAmbientColor
                        grooveColor: GlobalConstants.kScrollBarBackColor
                        rightMargin: GlobalConstants.kScrollBarRightMargin
                    }

                    onAtYEndChanged: {
                        if (!scrollbar3Depth.visible) {
                            return
                        }
                        if (!atYEnd) {
                             return
                        }
                        cancelFlick()
                        if (CommonUtils.isNull(list3DepthView)                ||
                            CommonUtils.isNull(list3DepthView.totalPage)      ||
                            CommonUtils.isNull(list3DepthView.requestPageIndex) ) {
                            return
                        }

                        if ( (list3DepthView.totalPage - 1) <= list3DepthView.requestPageIndex) {
                            return
                        }
                        requestSearch(searchRoot.query, ++list3DepthView.requestPageIndex, list3DepthView.maxCount, depthEnum.Third)
                    }
                }
                Item { width:parent.width;height:parent.height
                    Text {
                        Component.onCompleted: searchRoot.message = this
                        text: messageText
                        visible: messageVisible
                        anchors.centerIn:  parent
                        font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(GlobalConstants.kListTextFontSize) }
                        color: GlobalConstants.kListTextColor
                    }
                }
            }
        }
    }

    ListModel { id: grid1DepthModel   }
    ListModel { id: list2DepthModel   }
    ListModel { id: list3DepthModel   }

    Component {
        id: gridDelegate
        TPushButton {
            id: btn
            width: grid1DepthView.rowWidth
            height: grid1DepthView.rowHeight
            property int contentButtonYOffset:  searchRoot.tileMenuContentButtonYOffset
            property int contentTextYOffset: searchRoot.tileMenuContentTextYOffset
            property color outlineColor: searchRoot.tileMenuOutlineColor
            property bool isFocused: CommonUtils.isEqual(listView.currentIndex , model.index) && searchRoot.focused
            property var pressed: null
            property int row: 0
            property int col: 0            
            function clickItem() {btnClickItem(btn, model)}
            Component.onCompleted: setBtnProperty(btn, model)
            onIsFocusedChanged: {
                var index = null
                var imgF = null
                var imgN = null
                if (isFocused) {
                    txt.colorN = GlobalConstants.kBasicBackgoundColor
                    index = model.index
                    imgF = menuImgFArr[index]
                    img.sourceN  = Resource.imageUrl("industrymenu", imgF)
                } else {
                    txt.colorN =  GlobalConstants.kListTextColor
                    index = model.index
                    imgN = menuImgNArr[index]
                    img.sourceN  = Resource.imageUrl("industrymenu", imgN)
                }
            }
            onButtonPressed:  {pressed = true}
            onButtonReleased: {pressed = false}
            onButtonClicked: clickItem()
            PressBarMouseSync {
                cswObj: csw
                pressed: parent.pressed
                currentItem: grid1DepthView.pressedItem
                onCurrentItemChanged: {
                    if (CommonUtils.isEqual(currentItem, grid1DepthView.pressedItem)) {
                        return
                    }
                    grid1DepthView.pressedItem = currentItem
                }
            }            
            Rectangle {width:parent.width;height:1;color:outlineColor}
            Rectangle {width:1;height:parent.height;color:outlineColor;visible:col}
        }
    }

    Component {
        id: listDelegate
        Column{
            id: listColum
            width: listView.width
            height: listView.rowHeight
            property bool isFocused: CommonUtils.isEqual(listView.currentIndex , model.index) && searchRoot.focused            
            function clickItem(){
                listClickItem(listColum, model)
            }

            Item {
                width: parent.width
                height: parent.height - splitBar.height
                MouseArea {
                    width: parent.width
                    height: parent.height
                    PressBarMouseSync {
                        cswObj: csw
                        currentItem: searchRoot.listView.pressedItem
                        onCurrentItemChanged: {
                            if (CommonUtils.isEqual(currentItem, searchRoot.listView.pressedItem)) {
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
                    Item {width:GlobalConstants.kListTextLeftMargin;height:parent.height}
                    Text {
                        width: parent.width
                        height: parent.height
                        Component.onCompleted: CommonUtils.applyRemainWidth(this,parent)
                        horizontalAlignment: Qt.AlignLeft
                        verticalAlignment: Qt.AlignVCenter
                        font { family: GlobalConstants.kFontFamily; pixelSize: Resource.pixelToDp(GlobalConstants.kOneLineListTextFontSize) }
                        color: (isFocused) ? GlobalConstants.kBasicBackgoundColor : GlobalConstants.kListTextColor
                        text: getItemTextFromModel(model, currDepth)
                    }
                    TImageButton {
                          width: 63
                          height: 63
                          visible: (CommonUtils.isEqual(currDepth , depthEnum.First) || CommonUtils.isEqual(currDepth , depthEnum.Third)) ? false : true
                          bg.sourceSize.width: width
                          bg.sourceSize.height: height
                          anchors.verticalCenter : parent.verticalCenter
                          bgSources: function (state) {
                              switch (state) {
                              case CommonConstants.Normal:  return isFocused ? "list_ic_more_f.png": "list_ic_more.png"
                              case CommonConstants.Pressed: return isFocused ? "list_ic_more_f.png": "list_ic_more.png"
                              }
                          }
                          onClicked: clickItem()
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

    Csw {
            id: csw

            section: Csw.Section_NaviMenuGrid
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
                while (true) {
                    if (listView.currentIndex < listView.count - 1) {
                        ++listView.currentIndex
                    } else {
                        listView.currentIndex = 0
                    }
                    var item = listView.currentItem
                    var label = CommonUtils.trim(item.label)
                    if (CommonUtils.isEmpty(label)) {
                        if(CommonUtils.isEqual(listModel , grid1DepthModel))
                            continue
                    }
                    break
                }
                highlightColor = GlobalConstants.kListFocusColor
            }
            onSpinCcwPressed: {
                while (true) {
                    if (0 >= listView.currentIndex) {
                        listView.currentIndex = listView.count - 1
                    } else if (0 < listView.currentIndex) {
                        --listView.currentIndex
                    }else {
                        listView.currentIndex = 0
                    }
                    var item = listView.currentItem
                    var label = CommonUtils.trim(item.label)
                    if (CommonUtils.isEmpty(label)) {
                        if(CommonUtils.isEqual(listModel , grid1DepthModel))
                            continue
                    }
                    break
                }
                highlightColor = GlobalConstants.kListFocusColor
            }
            onSelected: {
                var item = listView.currentItem
                item.clickItem()
            }
    }

}
