#ifndef SEARCHCONSTANTS_H
#define SEARCHCONSTANTS_H

#include "../../../ndds/include/common_def.h"
#include "Constants.h"
//#include "CommonConstants.h"
#include "ZoneConstants.h"

#include <QString>
#include <QObject>
#include <QNetworkReply>


namespace SKT {

class TMAP_RSM_UI_DLLSHARED_EXPORT SearchConstants  : public QObject
{
    Q_OBJECT

    Q_ENUMS( RequestK )
    Q_ENUMS( OilBrandK )
    Q_ENUMS( TabViewPageIndexK )
    Q_ENUMS( CategoryK )
    Q_ENUMS( OilStationKindK )
    Q_ENUMS( ResultK )
    Q_ENUMS( ResultEvK )
    Q_ENUMS( DestTypeK )
    Q_ENUMS( CallStatusK )
    Q_ENUMS( SearchPosK )
    Q_ENUMS( TimeK )
    Q_ENUMS( VisibleK )
    Q_ENUMS( RecentDestiSortK )
    Q_ENUMS( ShowK )
    Q_ENUMS( NetworkErrorNoK )
    Q_ENUMS( ResultTypeK )
    Q_ENUMS( EvKindK )


    // string
    Q_PROPERTY(QString TextParam                    MEMBER  TextParam                 CONSTANT)
    Q_PROPERTY(QString RoadSearch                   MEMBER  RoadSearch                CONSTANT)
    Q_PROPERTY(QString SortBy                       MEMBER  SortBy                    CONSTANT)
    Q_PROPERTY(QString DistanceOrder                MEMBER  DistanceOrder             CONSTANT)
    Q_PROPERTY(QString AlphabeticalOrder            MEMBER  AlphabeticalOrder         CONSTANT)
    Q_PROPERTY(QString NameOrder                    MEMBER  NameOrder                 CONSTANT)
    Q_PROPERTY(QString PriceOrder                   MEMBER  PriceOrder                CONSTANT)
    Q_PROPERTY(QString MarkerId                     MEMBER  MarkerId                  CONSTANT)

    Q_PROPERTY(QString Bar                          MEMBER  Bar                       CONSTANT)
    Q_PROPERTY(QString List                         MEMBER  List                      CONSTANT)
    Q_PROPERTY(QString SearchType                   MEMBER  SearchType                CONSTANT)
    Q_PROPERTY(QString Params                       MEMBER  Params                    CONSTANT)
    Q_PROPERTY(QString Lcd                          MEMBER  Lcd                       CONSTANT)
    Q_PROPERTY(QString RequestContext               MEMBER  RequestContext            CONSTANT)
    Q_PROPERTY(QString ResponseNotificationId       MEMBER  ResponseNotificationId    CONSTANT)
    Q_PROPERTY(QString Mcd                          MEMBER  Mcd                       CONSTANT)
    Q_PROPERTY(QString Road                         MEMBER  Road                      CONSTANT)
    Q_PROPERTY(QString Dong                         MEMBER  Dong                      CONSTANT)
    Q_PROPERTY(QString Bld                          MEMBER  Bld                       CONSTANT)
    Q_PROPERTY(QString Lot                          MEMBER  Lot                       CONSTANT)
    Q_PROPERTY(QString Page                         MEMBER  Page                      CONSTANT)
    Q_PROPERTY(QString StartIndex                   MEMBER  StartIndex                CONSTANT)
    Q_PROPERTY(QString Depth                        MEMBER  Depth                     CONSTANT)
    Q_PROPERTY(QString RegData                      MEMBER  RegData                   CONSTANT)
    Q_PROPERTY(QString Dist                         MEMBER  Dist                      CONSTANT)
    Q_PROPERTY(QString Time                         MEMBER  Time                      CONSTANT)
    Q_PROPERTY(QString Clear                        MEMBER  Clear                     CONSTANT)
    Q_PROPERTY(QString Data                         MEMBER  Data                      CONSTANT)    
    Q_PROPERTY(QString FocusIndex                   MEMBER  FocusIndex                CONSTANT)
    Q_PROPERTY(QString Query                        MEMBER  Query                     CONSTANT)
    Q_PROPERTY(QString Order                        MEMBER  Order                     CONSTANT)
    Q_PROPERTY(QString Scenario                     MEMBER  Scenario                  CONSTANT)
    Q_PROPERTY(QString Find                         MEMBER  Find                      CONSTANT)
    Q_PROPERTY(QString Go                           MEMBER  Go                        CONSTANT)
    Q_PROPERTY(QString Point                        MEMBER  Point                     CONSTANT)
    Q_PROPERTY(QString ErrNo                        MEMBER  ErrNo                     CONSTANT)
    Q_PROPERTY(QString ErrStr                       MEMBER  ErrStr                    CONSTANT)

    Q_PROPERTY(QString DestX                        MEMBER  DestX                     CONSTANT)
    Q_PROPERTY(QString DestY                        MEMBER  DestY                     CONSTANT)
    Q_PROPERTY(QString View                         MEMBER  View                      CONSTANT)
    Q_PROPERTY(QString OkCallback                   MEMBER  OkCallback                CONSTANT)

    Q_PROPERTY(QString AddressType                  MEMBER  AddressType               CONSTANT)
    Q_PROPERTY(QString IsSan                        MEMBER  IsSan                     CONSTANT)
    Q_PROPERTY(QString IsApt                        MEMBER  IsApt                     CONSTANT)
    Q_PROPERTY(QString IsRoad                       MEMBER  IsRoad                    CONSTANT)
    Q_PROPERTY(QString NewAddrType                  MEMBER  NewAddrType               CONSTANT)
    Q_PROPERTY(QString OldAddrType                  MEMBER  OldAddrType               CONSTANT)
    Q_PROPERTY(QString Category                     MEMBER  Category                  CONSTANT)

    Q_PROPERTY(QString SearchView                   MEMBER  SearchView                CONSTANT)
    Q_PROPERTY(QString JibunStIdx                   MEMBER  JibunStIdx                CONSTANT)
    Q_PROPERTY(QString JibunCnt                     MEMBER  JibunCnt                  CONSTANT)
    Q_PROPERTY(QString AptStIdx                     MEMBER  AptStIdx                  CONSTANT)
    Q_PROPERTY(QString AptCnt                       MEMBER  AptCnt                    CONSTANT)
    Q_PROPERTY(QString NewAddrStIdx                 MEMBER  NewAddrStIdx              CONSTANT)
    Q_PROPERTY(QString NewAddrCnt                   MEMBER  NewAddrCnt                CONSTANT)
    Q_PROPERTY(QString JibunRicode                  MEMBER  JibunRicode               CONSTANT)
    Q_PROPERTY(QString Distance                     MEMBER  Distance                  CONSTANT)
    Q_PROPERTY(QString Price                        MEMBER  Price                     CONSTANT)
    Q_PROPERTY(QString AddrCode                     MEMBER  AddrCode                  CONSTANT)
    Q_PROPERTY(QString DestType                     MEMBER  DestType                  CONSTANT)
    Q_PROPERTY(QString Dest                         MEMBER  Dest                      CONSTANT)
    Q_PROPERTY(QString Checked                      MEMBER  Checked                   CONSTANT)
    Q_PROPERTY(QString Id                           MEMBER  Id                        CONSTANT)
    Q_PROPERTY(QString PoiID                        MEMBER  PoiID                     CONSTANT)
    Q_PROPERTY(QString NavSeq                       MEMBER  NavSeq                    CONSTANT)
    Q_PROPERTY(QString RpFlag                       MEMBER  RpFlag                    CONSTANT)
    Q_PROPERTY(QString OpiPoleCate                  MEMBER  OpiPoleCate               CONSTANT)
    Q_PROPERTY(QString OpiPrice                     MEMBER  OpiPrice                  CONSTANT)
    Q_PROPERTY(QString FavInsTime                   MEMBER  FavInsTime                CONSTANT)
    Q_PROPERTY(QString Relay                        MEMBER  Relay                     CONSTANT)
    Q_PROPERTY(QString MainCode                     MEMBER  MainCode                  CONSTANT)
    Q_PROPERTY(QString ReqData                      MEMBER  ReqData                   CONSTANT)
    Q_PROPERTY(QString Transaction                  MEMBER  Transaction               CONSTANT)
    Q_PROPERTY(QString SearchPageObj                MEMBER  SearchPageObj             CONSTANT)
    Q_PROPERTY(QString IsHome                       MEMBER  IsHome                    CONSTANT)
    Q_PROPERTY(QString IsOffice                     MEMBER  IsOffice                  CONSTANT)
    Q_PROPERTY(QString IsAnim                       MEMBER  IsAnim                    CONSTANT)
    Q_PROPERTY(QString ViaSetting                   MEMBER  ViaSetting                CONSTANT)
    Q_PROPERTY(QString Home                         MEMBER  Home                      CONSTANT)
    Q_PROPERTY(QString Office                       MEMBER  Office                    CONSTANT)
    Q_PROPERTY(QString None                         MEMBER  None                      CONSTANT)
    Q_PROPERTY(QString RecentDestination            MEMBER  RecentDestination         CONSTANT)
    Q_PROPERTY(QString PhoneNumber                  MEMBER  PhoneNumber               CONSTANT)
    Q_PROPERTY(QString DataKind                     MEMBER  DataKind                  CONSTANT)
    Q_PROPERTY(QString FuelKind                     MEMBER  FuelKind                  CONSTANT)
    Q_PROPERTY(QString CallStatus                   MEMBER  CallStatus                CONSTANT)
    Q_PROPERTY(QString BluetoothStatus              MEMBER  BluetoothStatus           CONSTANT)
    Q_PROPERTY(QString MapView                      MEMBER  MapView                   CONSTANT)
    Q_PROPERTY(QString OptionMenu                   MEMBER  OptionMenu                CONSTANT)
    Q_PROPERTY(QString OilSearchType                MEMBER  OilSearchType             CONSTANT)

    Q_PROPERTY(QString HollidayKo                   MEMBER  HollidayKo                CONSTANT)
    Q_PROPERTY(QString WorkTimeKo                   MEMBER  WorkTimeKo                CONSTANT)
    Q_PROPERTY(QString ParkingKo                    MEMBER  ParkingKo                 CONSTANT)

    Q_PROPERTY(QString Bid                          MEMBER  Bid                       CONSTANT)
    Q_PROPERTY(QString Sid                          MEMBER  Sid                       CONSTANT)
    Q_PROPERTY(QString Cid                          MEMBER  Cid                       CONSTANT)
    Q_PROPERTY(QString Tid                          MEMBER  Tid                       CONSTANT)
    Q_PROPERTY(QString StId                         MEMBER  StId                      CONSTANT)

    Q_PROPERTY(QString ContactGadgetKey             MEMBER  ContactGadgetKey          CONSTANT)


    //commonConstants와 함께 사용
    Q_PROPERTY(QString Command                      MEMBER  Command                   CONSTANT)
    Q_PROPERTY(QString RequestCommandPtr            MEMBER  RequestCommandPtr         CONSTANT)
    Q_PROPERTY(QString Text                         MEMBER  Text                      CONSTANT)
    Q_PROPERTY(QString ViewId                       MEMBER  ViewId                    CONSTANT)
    Q_PROPERTY(QString Address                      MEMBER  Address                   CONSTANT)
    Q_PROPERTY(QString Industry                     MEMBER  Industry                  CONSTANT)
    Q_PROPERTY(QString Around                       MEMBER  Around                    CONSTANT)
    Q_PROPERTY(QString Detail                       MEMBER  Detail                    CONSTANT)
    Q_PROPERTY(QString EvDetail                     MEMBER  EvDetail                  CONSTANT)
    Q_PROPERTY(QString Lowest                       MEMBER  Lowest                    CONSTANT)
    Q_PROPERTY(QString BookmarkType                 MEMBER  BookmarkType              CONSTANT)
    Q_PROPERTY(QString Favorite                     MEMBER  Favorite                  CONSTANT)
    Q_PROPERTY(QString Level                        MEMBER  Level                     CONSTANT)
    Q_PROPERTY(QString SubLevel                     MEMBER  SubLevel                  CONSTANT)
    Q_PROPERTY(QString Slot                         MEMBER  Slot                      CONSTANT)
    Q_PROPERTY(QString Keyword                      MEMBER  Keyword                   CONSTANT)
    Q_PROPERTY(QString Max                          MEMBER  Max                       CONSTANT)
    Q_PROPERTY(QString Offset                       MEMBER  Offset                    CONSTANT)
    Q_PROPERTY(QString Alias                        MEMBER  Alias                     CONSTANT)
    Q_PROPERTY(QString Count                        MEMBER  Count                     CONSTANT)
    Q_PROPERTY(QString CacheCount                   MEMBER  CacheCount                CONSTANT)
    Q_PROPERTY(QString FavorCount                   MEMBER  FavorCount                CONSTANT)
    Q_PROPERTY(QString DestiCount                   MEMBER  DestiCount                CONSTANT)
    Q_PROPERTY(QString Kind                         MEMBER  Kind                      CONSTANT)
    Q_PROPERTY(QString From                         MEMBER  From                      CONSTANT)
    Q_PROPERTY(QString To                           MEMBER  To                        CONSTANT)
    Q_PROPERTY(QString T                            MEMBER  T                         CONSTANT)
    Q_PROPERTY(QString B                            MEMBER  B                         CONSTANT)
    Q_PROPERTY(QString L                            MEMBER  L                         CONSTANT)
    Q_PROPERTY(QString R                            MEMBER  R                         CONSTANT)
    Q_PROPERTY(QString Fuel                         MEMBER  Fuel                      CONSTANT)
    Q_PROPERTY(QString Code                         MEMBER  Code                      CONSTANT)
    Q_PROPERTY(QString LcdName                      MEMBER  LcdName                   CONSTANT)
    Q_PROPERTY(QString McdName                      MEMBER  McdName                   CONSTANT)
    Q_PROPERTY(QString ScdName                      MEMBER  ScdName                   CONSTANT)
    Q_PROPERTY(QString Request                      MEMBER  Request                   CONSTANT)
    Q_PROPERTY(QString Response                     MEMBER  Response                  CONSTANT)
    Q_PROPERTY(QString Result                       MEMBER  Result                    CONSTANT)
    Q_PROPERTY(QString Notification                 MEMBER  Notification              CONSTANT)
    Q_PROPERTY(QString Sync                         MEMBER  Sync                      CONSTANT)



public:
    SearchConstants();

    enum NotificationK {
        UpdateSummary = Constants::Search,
        RequestSearch,
        ResponseSearch,
        RequestCacheSearch,
        ResponseCacheSearch,
        RequestPoiCacheInfo,
        ResponsePoiCacheInfo,
        ResponseOnlineSearch,
        ResponseFavorSearch,
        ResponseDestSearch,
        RequestCommand,
        ApplyAddressSearch,
        ApplyIndustrySearch,
        ApplyRegionSearch,

         // Recent and Favorites
        RequestRecentSearch,
        ResponseRecentSearch,
        AddRecentSearch,
        DeleteRecentSearch,
        ClearRecentSearch,

        RequestRecentDestination,
        ResponseRecentDestination,
        AddRecentDestination,
        DeleteRecentDestination,
        ClearRecentDestination,
        UpdateRecentDestinationScore,

        RequestFavorites,
        ResponseFavorites,
        AddFavorite, //DB
        DeleteFavorite, //DB
        ClearFavorites, //DB
        UpdateFavorite, //DB 갱신
        RequestHomeFavorite,
        ResponseHomeFavorite,
        RequestOfficeFavorite,
        ResponseOfficeFavorite,
        ResponseRegisterSendToCar,
        ResponseFindSendToCar,
        ResponseRemoveSendToCar,
        
        AddItemFailure, // failed to add item into DB
        AddItemOk,

        CancelAddFavorite,
        QueryIsFavorite,
        ResponseIsFavorite,
        ReturnToFavorite,
        UpdateBookmarkPageView,
        UpdateFavoritesProperty,
        InvokeClearFavorite,
        InvokeEditFavoriteAlias,
        InvokeUncheckAllFavorite,

        CancelAddSendToCar,
        QueryIsSendToCar,
        ResponseIsSendToCar,
        ReturnToSendToCar,
        UpdateSendToCarsProperty,
        InvokeClearSendToCar,
        InvokeEditSendToCarAlias,
        InvokeUncheckAllSendToCar,

        RequestRouteOption,
        ResponseRouteOption,

        ChangeSearchTabViewPage,
        ChangeSearchEditMode,

        NewNavibookSession,
        SelectNavibookSearch,
        SelectNavibookAddress,
        RegistNavibookBookmark,
        UpdateView,
        ShowTextSearchView,

        ChangedActiveTabPage,
        ChangedTabPageViewActivation,
        HideTabViewContent,
        ResponseNoResult,
        SearchWait,
        QueueChanged,
        CloseRecentTileGridView,
        CloseSearchResultView,

        TextSearchHasNoResult,  // TextSearch의 결과가 0일 때...

        RequestRemoveMarker,
        ResponseSort,
        RequestGadgetSearchMode,
        ReplyToGadgetSearchMode,
        SetGadgetSearchMode,

        ResponseCurrentStreet,
        ChangedCurrentStreet,

        UpdateSearchResultText,
        UpdateTextSearchZone1,
        ClearTextSearchZone1,
        UpdateGpsTime,
        RequestGpsTime,
        ResponseGpsTime,
        ResponseLockStatus,
    };

    enum RequestK {
        RequestCount = 50,
        RequestMaxCount = 2000,
    };

    enum DestTypeK {
        DestTypeNone = 0,
        DestTypeOffice,
        DestTypeHome,
    };

    enum SearchPosK {
        CurrentPos,
        DestinationPos,
    };

    // mns/serversearch.cpp와 연관됨
    enum OilBrandK {
        SK              = 1,
        GS              = 2,
        SOIL            = 3,
        OILBANK         = 4,
        UNKNOWNBRAND    = 5,
        CATEGORYGAP     = 100,
        OILBRANDBASE    = 20100,
        CHARGINGBASE    = 20200,
        EVCHARGINGBASE  = 21000,
    };

    // 탭뷰 및 검색결과에서 공통 사용
    enum TabViewPageIndexK {
        RecentSearchPageIndex,
        RecentDestPageIndex,
        FavoritePageIndex,
        NoResultPageIndex,
    };

    enum CategoryK {                 // serversearch.cpp 주변검색과 연관됨 - 추후 동기화 시킬 것
        CategoryNone                 = -1000,//Q.-1이 qml에서 undefined로 나오는 문제 있음
        CategoryGasSt                = 0, //주유소
        CategoryChargingSt           = 1, //충전소
        CategoryParkingSp            = 2, //주차장
        CategoryBank                 = 3, //은행
        CategoryShopping             = 4, //쇼핑
        CategoryRestaurant           = 5, //음식점
        CategoryTourism              = 6, //관광
        CategoryMedical              = 7, //의료
        CategoryPublicInstitution    = 8, //공공기관
        CategoryFinance              = 9, //금융
        CategoryChargingEvSt         = 10,//전기차 충전소
        CategoryPatrol               = 11,//유종 자동 검출(VR전용)
        CategoryEnd                      ,
        CategorySearchAddressVr,
    };    

    enum CallStatusK {
        CallStatusFail    = -1000, //Q.-1이 qml에서 undefined로 나오는 문제 있음
        CallStatusSuccess = -1001, //Q.-1이 qml에서 undefined로 나오는 문제 있음
        CallStatusIDLE = 0,
        CallStatusDIALING,
        CallStatusACTIVE,
        CallStatusON_HOLD,
        CallStatusDISCONNECTING,
        CallStatusBUSY,
        CallStatusCONFERENCE,
        CallStatusIN_VOICEMAIL,
        CallStatusRINGTONE,
    };


    //common_def.h - N_SEARCH_LIST의 nDataKind에 사용
    enum OilStationKindK {
       OilStKindJuyuso          = 3,
       OilStKindChungjeonso     = 4,
       OilStKindEVchungjeonso   = 6,
    };

    enum ResultK {
        ResultObject            ,// 00.객체
        ResultIndex             ,// 01.검색 결과 인덱스
        ResultName              ,// 02.명칭
        ResultStdName           ,// 03.대표 명칭
        ResultAddress           ,// 04.구주소, 새주소 (새주소 요청시 DB에 새주소 정보가 없으면 구까지만 전달)
        ResultTelNum            ,// 05.전화번호
        ResultPOIID             ,// 06.POI ID
        ResultAddrCode          ,// 07.주소 코드
        ResultRpFlag            ,// 08.RPFlag (경로 탐색시 사용)
        ResultDistance          ,// 09.현위치에서 거리
        ResultDeg               ,// 10.방위각
        ResultKind              ,// 11.검색 : 업종코드
        ResultX                 ,// 12.위치 좌표
        ResultY                 ,// 13.위치 좌표
        ResultOPIPoleCate       ,// 14.(주유소,충전소)주변검색 및 업종검색의 브랜드 코드 값 또는 상세검색에서 OilStationKindK의 값에 해당함
        ResultOPIPrice          ,// 15.가격(유가 정보 일때만 유효) -1:TPEG 수신정보 없음, 0:TPEG 수신 되었으나 가격 정보 없음
        ResultYear              ,// 16.유가정보 제공일자 (유가 정보 일때만 유효)-년
        ResultMonth             ,// 17.유가정보 제공일자 (유가 정보 일때만 유효)-월
        ResultDay               ,// 18.유가정보 제공일자 (유가 정보 일때만 유효)-일
        ResultHour              ,// 19.유가정보 제공일자 (유가 정보 일때만 유효)-시
        ResultMinute            ,// 20.유가정보 제공일자 (유가 정보 일때만 유효)-분
        ResultSecond            ,// 21.유가정보 제공일자 (유가 정보 일때만 유효)-초
        ResultWeek              ,// 22.유가정보 제공일자 (유가 정보 일때만 유효)-요일
        ResultOilCompany        ,// 23.주유소브랜드(SK, GS, 기타 등), 단말에서 브랜드 필터링시 사용
        ResultHighGasoline      ,// 24.고급휘발유판매점여부 (0:판매안함, 1:판매)
        ResultLowestOilSt       ,// 25.최저가점포여부. 출력리스트상 최저가 점포일 경우(최저가:1, 아닌경우:0). SK주유소가 최저가인 경우에만 1로 설정
        ResultOPIGasolinePrice  ,// 26.휘발유가격(원)
        ResultOPIDieselPrice    ,// 27.경유가격(원)
        ResultOPILPGPrice       ,// 28.LPG가격(원)
        ResultOPIHiGasolinePrice,// 29.프리미엄 휘발유 가격(원)
        ResultOPIHiLPGPrice     ,// 30.프리미엄 LPG 가격(원)
        ResultWebSite           ,// 31.웹사이트
        ResultHolliday          ,// 32.휴무일
        ResultWorkTime          ,// 33.영업시간
        ResultIndustry          ,// 34.업종
        ResultParking           ,// 35.주차
        ResultIntroduction      ,// 36.소개글
        ResultUserGuide         ,// 37.이용안내
        ResultInfomation        ,// 38.업소 소개
        ResultNavseq            ,// 39.입구점번호        
        ResultOnline            ,// 40.on/offline 검색 여부
        ResultBrand             ,// 41.ResultOPIPoleCate의 원본(?) 값
        ResultPrimaryMax        ,// 42.주번 최대값
        ResultPrimaryMin        ,// 43.주번 최소값
        ResultSecondaryMax      ,// 44.부번 최대값
        ResultSecondaryMin      ,// 45.부번 최소값
        ResultSort              ,// 46.소트 여부
        ResultType              ,// 47.검색 결과의 종류: 일반,캐쉬,최근목적지,즐겨찾기,이외의 검색
        ResultAlongRoute        ,// 48.경로상 검색 여부(전기차 검색을 위하여... )
        ResultLineChanged       ,// 49.on/offline 검색 변경 여부
        ResultEnd               ,// 50.끝
     };

   enum ResultEvK {
       ResultFastCharge     = 17,// 17.완속
       ResultSlowCharge         ,// 18.완속
       ResultDCDemo             ,// 19.DC차데모
       ResultAC3                ,// 20.AC3상
       ResultDCCombo            ,// 21.DC콤보
       ResultStationCount       ,// 22.충전소 갯수
       ResultChargingFee        ,// 23.충전요금
       ResultParkingFree        ,// 24.주차비 유무
       ResultReserveYn          ,// 25.예약가능 여부
       ResultPathType           ,// 26.경로상(0:존재함, 1:경로주변)
       ResultDetailLocation     ,// 27.상세위치
       ResultCharger            ,// 28.충전기
       ResultOperatingAgencyID  ,// 29.운영기관ID
       ResultAgencyManagementID ,// 30.기관관리ID
       ResultChargingStationID  ,// 31.충전소ID
       ResultOperatingTime      ,// 32.운영시간
       ResultOperatingOrgan     ,// 33.운영기관
       ResultStatus             ,// 34.충전기 상태정보(0:알수없음 / 1:통신이상 /2:충전대기 /3:충전중 /4:운영중지 /5:점검중 /6:예약중)
       ResultChargingType       ,// 35.충전기타입
       ResultChargerInfo        ,// 36.충전기리스트
   };

   enum TimeK {
       NetworkTimeoutAPI = SEARCH_WAITTING_TIME,
       NetworkTimeoutAPP = SEARCH_WAITTING_TIME + (1000*2),
   };

   enum VisibleK {
       DialogNoResult  = false,
       ViewNoResult = true,
   };

   enum ShowK {
       HideToShow = false,
       AlwaysShow = true,
   };

   //[주의]NavibookmarkAgentProxy::recentDestListBySortType함수 안에 정의되어 있음
   enum RecentDestiSortK {
       SortId,
       SortHit,
       SortHybrid,
   };

   enum NetworkErrorNoK {
       ConnectionRefusedError               = QNetworkReply::ConnectionRefusedError            , // the remote server refused the connection (the server is not accepting requests)
       RemoteHostClosedError                = QNetworkReply::RemoteHostClosedError             , // the remote server closed the connection prematurely, before the entire reply was received and processed
       HostNotFoundError                    = QNetworkReply::HostNotFoundError                 , // the remote host name was not found (invalid hostname)
       TimeoutError                         = QNetworkReply::TimeoutError                      , // the connection to the remote server timed out
       OperationCanceledError               = QNetworkReply::OperationCanceledError            , // the operation was canceled via calls to abort() or close() before it was finished.
       SslHandshakeFailedError              = QNetworkReply::SslHandshakeFailedError           , // the SSL/TLS handshake failed and the encrypted channel could not be established. The sslErrors() signal should have been emitted.
       TemporaryNetworkFailureError         = QNetworkReply::TemporaryNetworkFailureError      , // the connection was broken due to disconnection from the network, however the system has initiated roaming to another access point. The request should be resubmitted and will be processed as soon as the connection is re-established.
       NetworkSessionFailedError            = QNetworkReply::NetworkSessionFailedError         , // the connection was broken due to disconnection from the network or failure to start the network.
       BackgroundRequestNotAllowedError     = QNetworkReply::BackgroundRequestNotAllowedError  , // the background request is not currently allowed due to platform policy.
       TooManyRedirectsError                = QNetworkReply::TooManyRedirectsError             , // while following redirects, the maximum limit was reached. The limit is by default set to 50 or as set by QNetworkRequest::setMaxRedirectsAllowed(). (This value was introduced in 5.6.)
       InsecureRedirectError                = QNetworkReply::InsecureRedirectError             , // while following redirects, the network access API detected a redirect from a encrypted protocol (https) to an unencrypted one (http). (This value was introduced in 5.6.)
       ProxyConnectionRefusedError	        = QNetworkReply::ProxyConnectionRefusedError	   , // the connection to the proxy server was refused (the proxy server is not accepting requests)
       ProxyConnectionClosedError	        = QNetworkReply::ProxyConnectionClosedError        , // the proxy server closed the connection prematurely, before the entire reply was received and processed
       ProxyNotFoundError                   = QNetworkReply::ProxyNotFoundError                , // the proxy host name was not found (invalid proxy hostname)
       ProxyTimeoutError                    = QNetworkReply::ProxyTimeoutError                 , // the connection to the proxy timed out or the proxy did not reply in time to the request sent
       ProxyAuthenticationRequiredError     = QNetworkReply::ProxyAuthenticationRequiredError  , // the proxy requires authentication in order to honour the request but did not accept any credentials offered (if any)
       ContentAccessDenied                  = QNetworkReply::ContentAccessDenied               , // the access to the remote content was denied (similar to HTTP error 403)
       ContentOperationNotPermittedError    = QNetworkReply::ContentOperationNotPermittedError , // the operation requested on the remote content is not permitted
       ContentNotFoundError                 = QNetworkReply::ContentNotFoundError              , // the remote content was not found at the server (similar to HTTP error 404)
       AuthenticationRequiredError	        = QNetworkReply::AuthenticationRequiredError	   , // the remote server requires authentication to serve the content but the credentials provided were not accepted (if any)
       ContentReSendError                   = QNetworkReply::ContentReSendError                , // the request needed to be sent again, but this failed for example because the upload data could not be read a second time.
       ContentConflictError                 = QNetworkReply::ContentConflictError              , // the request could not be completed due to a conflict with the current state of the resource.
       ContentGoneError                     = QNetworkReply::ContentGoneError                  , // the requested resource is no longer available at the server.
       InternalServerError                  = QNetworkReply::InternalServerError               , // the server encountered an unexpected condition which prevented it from fulfilling the request.
       OperationNotImplementedError	        = QNetworkReply::OperationNotImplementedError	   , // the server does not support the functionality required to fulfill the request.
       ServiceUnavailableError              = QNetworkReply::ServiceUnavailableError           , // the server is unable to handle the request at this time.
       ProtocolUnknownError                 = QNetworkReply::ProtocolUnknownError              , // the Network Access API cannot honor the request because the protocol is not known
       ProtocolInvalidOperationError	    = QNetworkReply::ProtocolInvalidOperationError	   , // the requested operation is invalid for this protocol
       UnknownNetworkError                  = QNetworkReply::UnknownNetworkError               , // an unknown network-related error was detected
       UnknownProxyError                    = QNetworkReply::UnknownProxyError                 , // an unknown proxy-related error was detected
       UnknownContentError                  = QNetworkReply::UnknownContentError               , // an unknown error related to the remote content was detected
       ProtocolFailure                      = QNetworkReply::ProtocolFailure                   , // a breakdown in protocol was detected (parsing error, invalid or unexpected responses, etc.)
       UnknownServerError                   = QNetworkReply::UnknownServerError                , // an unknown error related to the server response was detected
   };

   enum ResultTypeK {
       ResultTypeNormal,
       ResultTypeCache,
       ResultTypeFavor,
       ResultTypeRcntDest,
   };

   enum EvKindK {
       EvKindNone,
       EvKindDCdemo,
       EvKindACslow,
       EvKindDCdemoAC3,
       EvKindDCcombo,
       EvKindDCdemoCombo,
       EvKindDCdemoAC3Combo,
       EvKindAC3,
       EvKindEnd,
   };

    static SearchConstants* instance() {
        static SearchConstants Instance;
        return &Instance;
    }

    static QObject *qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine) {
        Q_UNUSED(engine);
        Q_UNUSED(scriptEngine);

        return new SearchConstants;
    }
    Q_INVOKABLE QVariant resultInit();
    Q_INVOKABLE QVariant resultEvInit();
    Q_INVOKABLE QString normalized(QVariant text);
    static void resultInit(QVariantList &result);
    static void resultEvInit(QVariantList &result);

    // string
    static const QString TextParam;
    static const QString RoadSearch;
    static const QString SortBy;
    static const QString DistanceOrder;
    static const QString AlphabeticalOrder;
    static const QString NameOrder;
    static const QString PriceOrder;
    static const QString MarkerId;

    static const QString Bar;
    static const QString List;
    static const QString SearchType;
    static const QString Params;
    static const QString Lcd;
    static const QString RequestContext;
    static const QString ResponseNotificationId;
    static const QString Mcd;
    static const QString Road;
    static const QString Dong;
    static const QString Bld;
    static const QString Lot;
    static const QString Page;
    static const QString Url;
    static const QString StartIndex;
    static const QString Depth;
    static const QString RegData;
    static const QString Dist;
    static const QString Time;
    static const QString Clear;
    static const QString Data;
    static const QString Query;
    static const QString Order;
    static const QString Scenario;
    static const QString Find;
    static const QString Go;
    static const QString Point;
    static const QString ErrNo;
    static const QString ErrStr;


    static const QString DestX;
    static const QString DestY;
    static const QString View;
    static const QString OkCallback;


    static const QString AddressType;
    static const QString IsSan;
    static const QString IsApt;
    static const QString IsRoad;
    static const QString NewAddrType;
    static const QString OldAddrType;
    static const QString Category;

    static const QString SearchView;
    static const QString JibunStIdx;
    static const QString JibunCnt;
    static const QString AptStIdx;
    static const QString AptCnt;
    static const QString NewAddrStIdx;
    static const QString NewAddrCnt;
    static const QString JibunRicode;
    static const QString Distance;
    static const QString Price;
    static const QString AddrCode;
    static const QString DestType;
    static const QString Dest;
    static const QString Checked;
    static const QString Id;
    static const QString PoiID;
    static const QString NavSeq;
    static const QString RpFlag;
    static const QString OpiPoleCate;
    static const QString OpiPrice;
    static const QString FavInsTime;
    static const QString Relay;
    static const QString MainCode;
    static const QString ReqData;
    static const QString Transaction;
    static const QString SearchPageObj;
    static const QString IsHome;
    static const QString IsOffice;
    static const QString IsAnim;
    static const QString ViaSetting;
    static const QString Home;
    static const QString Office;
    static const QString None;
    static const QString RecentDestination;
    static const QString PhoneNumber;
    static const QString DataKind;
    static const QString FuelKind;
    static const QString CallStatus;
    static const QString BluetoothStatus;
    static const QString MapView;
    static const QString OptionMenu;
    static const QString OilSearchType;

    //상세검색: 문자열 비교
    static const QString HollidayKo;
    static const QString WorkTimeKo;
    static const QString ParkingKo;


    //전기차충전소-충전기 리스트 관련
    static const QString Bid;
    static const QString Sid;
    static const QString Cid;
    static const QString Tid;
    static const QString StId;

    //가젯에서 키로 사용하는 문자열
    static const QString ContactGadgetKey;


    //commonConstants와 함께 사용
    static const QString Command;
    static const QString RequestCommandPtr;
    static const QString Text;
    static const QString ViewId;
    static const QString Address;
    static const QString Industry;
    static const QString Around;
    static const QString Detail;
    static const QString EvDetail;
    static const QString Lowest;
    static const QString BookmarkType;
    static const QString Favorite;
    static const QString Level;
    static const QString SubLevel;
    static const QString Slot;
    static const QString Keyword;
    static const QString Max;
    static const QString Offset;
    static const QString Alias;
    static const QString Count;
    static const QString CacheCount;
    static const QString FavorCount;
    static const QString DestiCount;
    static const QString FocusIndex;
    static const QString Kind;
    static const QString From;
    static const QString To;
    static const QString T;
    static const QString B;
    static const QString L;
    static const QString R;
    static const QString Fuel;
    static const QString Code;
    static const QString LcdName;
    static const QString McdName;
    static const QString ScdName;

    static const QString Request;
    static const QString Response;
    static const QString Result;
    static const QString Notification;
    static const QString Sync;

};

}

#endif
