#include "SearchConstants.h"
#include "StringHelper.h"

using namespace SKT;

// string
const QString SearchConstants::TextParam                    = QStringLiteral("textParam");
const QString SearchConstants::RoadSearch                   = QStringLiteral("roadSearch");
const QString SearchConstants::SortBy                       = QStringLiteral("sortBy");
const QString SearchConstants::DistanceOrder                = QStringLiteral("distanceOrder");
const QString SearchConstants::AlphabeticalOrder            = QStringLiteral("alphabeticalOrder");
const QString SearchConstants::NameOrder                    = QStringLiteral("nameOrder");
const QString SearchConstants::PriceOrder                   = QStringLiteral("priceOrder");
const QString SearchConstants::MarkerId                     = QStringLiteral("markerId"); //vsm/mapviewclient.cpp와 searchResultView.qml에서 사용

const QString SearchConstants::Bar                          = QStringLiteral("bar");
const QString SearchConstants::List                         = QStringLiteral("list");
const QString SearchConstants::SearchType                   = QStringLiteral("searchType");
const QString SearchConstants::Params                       = QStringLiteral("params");
const QString SearchConstants::Lcd                          = QStringLiteral("lcd");
const QString SearchConstants::RequestContext               = QStringLiteral("requestContext");
const QString SearchConstants::ResponseNotificationId       = QStringLiteral("responseNotificationId");
const QString SearchConstants::Mcd                          = QStringLiteral("mcd");
const QString SearchConstants::Road                         = QStringLiteral("road");
const QString SearchConstants::Dong                         = QStringLiteral("dong");
const QString SearchConstants::Bld                          = QStringLiteral("bld");
const QString SearchConstants::Lot                          = QStringLiteral("lot");
const QString SearchConstants::Page                         = QStringLiteral("page");
const QString SearchConstants::StartIndex                   = QStringLiteral("startIndex");
const QString SearchConstants::Depth                        = QStringLiteral("depth");
const QString SearchConstants::RegData                      = QStringLiteral("regData");
const QString SearchConstants::Dist                         = QStringLiteral("dist");
const QString SearchConstants::Time                         = QStringLiteral("time");
const QString SearchConstants::Clear                        = QStringLiteral("clear");
const QString SearchConstants::Data                         = QStringLiteral("data");
const QString SearchConstants::Query                        = QStringLiteral("query");
const QString SearchConstants::Order                        = QStringLiteral("order");
const QString SearchConstants::Scenario                     = QStringLiteral("scenario");
const QString SearchConstants::Find                         = QStringLiteral("find");
const QString SearchConstants::Go                           = QStringLiteral("go");
const QString SearchConstants::Point                        = QStringLiteral("point");
const QString SearchConstants::ErrNo                        = QStringLiteral("errno");
const QString SearchConstants::ErrStr                       = QStringLiteral("errstr");

const QString SearchConstants::DestX                        = QStringLiteral("destX");
const QString SearchConstants::DestY                        = QStringLiteral("destY");
const QString SearchConstants::View                         = QStringLiteral("view");
const QString SearchConstants::OkCallback                   = QStringLiteral("okCallback");

const QString SearchConstants::AddressType                  = QStringLiteral("addressType");
const QString SearchConstants::IsSan                        = QStringLiteral("isSan");
const QString SearchConstants::IsApt                        = QStringLiteral("isApt");
const QString SearchConstants::IsRoad                       = QStringLiteral("isRoad");
const QString SearchConstants::NewAddrType                  = QStringLiteral("new");
const QString SearchConstants::OldAddrType                  = QStringLiteral("old");
const QString SearchConstants::Category                     = QStringLiteral("category");
const QString SearchConstants::SearchView                   = QStringLiteral("searchView");
const QString SearchConstants::JibunStIdx                   = QStringLiteral("jibunStIdx");
const QString SearchConstants::JibunCnt                     = QStringLiteral("jibunCnt");
const QString SearchConstants::AptStIdx                     = QStringLiteral("aptStIdx");
const QString SearchConstants::AptCnt                       = QStringLiteral("aptCnt");
const QString SearchConstants::NewAddrStIdx                 = QStringLiteral("newAddrStIdx");
const QString SearchConstants::NewAddrCnt                   = QStringLiteral("newAddrCnt");
const QString SearchConstants::JibunRicode                  = QStringLiteral("jibunRicode");
const QString SearchConstants::Distance                     = QStringLiteral("distance");
const QString SearchConstants::Price                        = QStringLiteral("price");
const QString SearchConstants::AddrCode                     = QStringLiteral("addrCode");
const QString SearchConstants::DestType                     = QStringLiteral("destType");
const QString SearchConstants::Dest                         = QStringLiteral("dest");
const QString SearchConstants::Checked                      = QStringLiteral("checked");
const QString SearchConstants::Id                           = QStringLiteral("id");
const QString SearchConstants::PoiID                        = QStringLiteral("poiID");
const QString SearchConstants::NavSeq                       = QStringLiteral("navSeq");
const QString SearchConstants::RpFlag                       = QStringLiteral("rpFlag");
const QString SearchConstants::OpiPoleCate                  = QStringLiteral("opiPoleCate");
const QString SearchConstants::OpiPrice                     = QStringLiteral("opiPrice");
const QString SearchConstants::FavInsTime                   = QStringLiteral("favInsTime");
const QString SearchConstants::Relay                        = QStringLiteral("relay");
const QString SearchConstants::MainCode                     = QStringLiteral("mainCode");
const QString SearchConstants::ReqData                      = QStringLiteral("reqData");
const QString SearchConstants::Transaction                  = QStringLiteral("transaction");
const QString SearchConstants::SearchPageObj                = QStringLiteral("searchPageObj");
const QString SearchConstants::IsHome                       = QStringLiteral("isHome");
const QString SearchConstants::IsOffice                     = QStringLiteral("isOffice");
const QString SearchConstants::IsAnim                       = QStringLiteral("isAnim");
const QString SearchConstants::ViaSetting                   = QStringLiteral("viaSetting");
const QString SearchConstants::Home                         = QStringLiteral("home");
const QString SearchConstants::Office                       = QStringLiteral("office");
const QString SearchConstants::None                         = QStringLiteral("none");
const QString SearchConstants::RecentDestination            = QStringLiteral("recentDestination");
const QString SearchConstants::PhoneNumber                  = QStringLiteral("phoneNumber");
const QString SearchConstants::DataKind                     = QStringLiteral("dataKind");
const QString SearchConstants::FuelKind                     = QStringLiteral("fuelKind");
const QString SearchConstants::CallStatus                   = QStringLiteral("callStatus");
const QString SearchConstants::BluetoothStatus              = QStringLiteral("bluetoothStatus");
const QString SearchConstants::MapView                      = QStringLiteral("mapView");
const QString SearchConstants::OptionMenu                   = QStringLiteral("optionMenu");
const QString SearchConstants::OilSearchType                = QStringLiteral("oilSearchType");

//상세: 문자열 비교에 사용함
const QString SearchConstants::HollidayKo                   = QStringLiteral("휴무일");
const QString SearchConstants::WorkTimeKo                   = QStringLiteral("영업시간");
const QString SearchConstants::ParkingKo                    = QStringLiteral("주차");

//전기차충전소-충전기 리스트 관련
const QString SearchConstants::Bid                         = QStringLiteral("BID");
const QString SearchConstants::Sid                         = QStringLiteral("SID");
const QString SearchConstants::Cid                         = QStringLiteral("CID");
const QString SearchConstants::Tid                         = QStringLiteral("TYPE");
const QString SearchConstants::StId                        = QStringLiteral("STATUS");

//가젯에서 키로 사용하는 문자열
const QString SearchConstants::ContactGadgetKey            = QStringLiteral("responsedContactGadgetSearchResult");

//commonConstants와 함께 사용
const QString SearchConstants::Command                      = QStringLiteral("Command"); //rpnsearch.cpp에서 사용하는 거라 바꾸지 말것
const QString SearchConstants::RequestCommandPtr            = QStringLiteral("RequestCommandPtr");
const QString SearchConstants::Text                         = QStringLiteral("text");
const QString SearchConstants::ViewId                       = QStringLiteral("viewId");
const QString SearchConstants::Address                      = QStringLiteral("address");
const QString SearchConstants::Industry                     = QStringLiteral("industry");
const QString SearchConstants::Around                       = QStringLiteral("around");
const QString SearchConstants::Detail                       = QStringLiteral("detail");
const QString SearchConstants::EvDetail                     = QStringLiteral("evdetail");
const QString SearchConstants::Lowest                       = QStringLiteral("lowest");
const QString SearchConstants::BookmarkType                 = QStringLiteral("bookmarkType");
const QString SearchConstants::Favorite                     = QStringLiteral("favorite");
const QString SearchConstants::Level                        = QStringLiteral("level");
const QString SearchConstants::SubLevel                     = QStringLiteral("subLevel");
const QString SearchConstants::Slot                         = QStringLiteral("slot");
const QString SearchConstants::Keyword                      = QStringLiteral("keyword");
const QString SearchConstants::Max                          = QStringLiteral("max");
const QString SearchConstants::Offset                       = QStringLiteral("offset");
const QString SearchConstants::Alias                        = QStringLiteral("alias");
const QString SearchConstants::Count                        = QStringLiteral("count");
const QString SearchConstants::CacheCount                   = QStringLiteral("cacheCount");
const QString SearchConstants::FavorCount                   = QStringLiteral("favorCount");
const QString SearchConstants::DestiCount                   = QStringLiteral("destiCount");
const QString SearchConstants::FocusIndex                   = QStringLiteral("focusIndex");
const QString SearchConstants::Kind                         = QStringLiteral("kind");
const QString SearchConstants::From                         = QStringLiteral("from");
const QString SearchConstants::To                           = QStringLiteral("to");
const QString SearchConstants::T                            = QStringLiteral("t");
const QString SearchConstants::B                            = QStringLiteral("b");
const QString SearchConstants::L                            = QStringLiteral("l");
const QString SearchConstants::R                            = QStringLiteral("r");
const QString SearchConstants::Fuel                         = QStringLiteral("fuel");
const QString SearchConstants::Code                         = QStringLiteral("code");
const QString SearchConstants::LcdName                      = QStringLiteral("lcdName");
const QString SearchConstants::McdName                      = QStringLiteral("mcdName");
const QString SearchConstants::ScdName                      = QStringLiteral("scdName");
const QString SearchConstants::Request                      = QStringLiteral("request");
const QString SearchConstants::Response                     = QStringLiteral("response");
const QString SearchConstants::Result                       = QStringLiteral("result");
const QString SearchConstants::Notification                 = QStringLiteral("notification");
const QString SearchConstants::Sync                         = QStringLiteral("sync");


SearchConstants::SearchConstants()
{

}

QVariant SearchConstants::resultInit()
{
    QVariantList data;
    SearchConstants::resultInit(data);
    return QVariant::fromValue(data);
}

void SearchConstants::resultInit(QVariantList &result)
{
    for(int i=0; i <= SearchConstants::ResultEnd; i++) {
        result.append(CommonConstants::Empty);
    }
    result[ResultObject            ] = 0;
    result[ResultIndex             ] = 0;
    result[ResultName              ] = CommonConstants::Empty;
    result[ResultStdName           ] = CommonConstants::Empty;
    result[ResultAddress           ] = CommonConstants::Empty;
    result[ResultTelNum            ] = CommonConstants::Empty;
    result[ResultPOIID             ] = 0;
    result[ResultAddrCode          ] = 0;
    result[ResultRpFlag            ] = 0;
    result[ResultDistance          ] = 0;
    result[ResultDeg               ] = 0;
    result[ResultKind              ] = 0;
    result[ResultX                 ] = 0;
    result[ResultY                 ] = 0;
    result[ResultOPIPoleCate       ] = 0;
    result[ResultOPIPrice          ] = 0;
    result[ResultYear              ] = 0;
    result[ResultMonth             ] = 0;
    result[ResultDay               ] = 0;
    result[ResultHour              ] = 0;
    result[ResultMinute            ] = 0;
    result[ResultSecond            ] = 0;
    result[ResultWeek              ] = 0;
    result[ResultOilCompany        ] = CommonConstants::Empty;
    result[ResultHighGasoline      ] = false;
    result[ResultLowestOilSt       ] = false;
    result[ResultOPIGasolinePrice  ] = 0;
    result[ResultOPIDieselPrice    ] = 0;
    result[ResultOPILPGPrice       ] = 0;
    result[ResultOPIHiGasolinePrice] = 0;
    result[ResultOPIHiLPGPrice     ] = 0;
    result[ResultWebSite           ] = CommonConstants::Empty;
    result[ResultHolliday          ] = CommonConstants::Empty;
    result[ResultWorkTime          ] = CommonConstants::Empty;
    result[ResultIndustry          ] = CommonConstants::Empty;
    result[ResultParking           ] = CommonConstants::Empty;
    result[ResultIntroduction      ] = CommonConstants::Empty;
    result[ResultUserGuide         ] = CommonConstants::Empty;
    result[ResultInfomation        ] = CommonConstants::Empty;
    result[ResultNavseq            ] = CommonConstants::Empty;
    result[ResultOnline            ] = false;
    result[ResultBrand             ] = 0;
    result[ResultPrimaryMax        ] = 0;
    result[ResultPrimaryMin        ] = 0;
    result[ResultSecondaryMax      ] = 0;
    result[ResultSecondaryMin      ] = 0;
    result[ResultSort              ] = false;
    result[ResultType              ] = ResultTypeNormal;
    result[ResultAlongRoute        ] = false;
    result[ResultLineChanged       ] = false;
    result[ResultEnd               ] = 0;
}

QVariant SearchConstants::resultEvInit()
{
    QVariantList data;
    SearchConstants::resultEvInit(data);
    return QVariant::fromValue(data);
}

void SearchConstants::resultEvInit(QVariantList &result)
{
    for(int i=0; i <= SearchConstants::ResultEnd; i++) {
        result.append(CommonConstants::Empty);
    }

    //공통
    result[ResultObject            ] = 0;
    result[ResultIndex             ] = 0;
    result[ResultName              ] = CommonConstants::Empty;
    result[ResultStdName           ] = CommonConstants::Empty;
    result[ResultAddress           ] = CommonConstants::Empty;
    result[ResultTelNum            ] = CommonConstants::Empty;
    result[ResultPOIID             ] = 0;
    result[ResultAddrCode          ] = 0;
    result[ResultRpFlag            ] = 0;
    result[ResultDistance          ] = 0;
    result[ResultDeg               ] = 0;
    result[ResultKind              ] = 0;
    result[ResultX                 ] = 0;
    result[ResultY                 ] = 0;
    result[ResultOPIPoleCate       ] = 0;
    result[ResultOPIPrice          ] = 0;
    result[ResultYear              ] = 0;

    //전기차 관련
    result[ResultFastCharge        ] = false;
    result[ResultSlowCharge        ] = false;
    result[ResultDCDemo            ] = false;
    result[ResultAC3               ] = false;
    result[ResultDCCombo           ] = false;
    result[ResultStationCount      ] = 0;
    result[ResultChargingFee       ] = CommonConstants::Empty;
    result[ResultParkingFree       ] = false;
    result[ResultReserveYn         ] = false;
    result[ResultPathType          ] = 0;
    result[ResultDetailLocation    ] = CommonConstants::Empty;
    result[ResultCharger           ] = CommonConstants::Empty;
    result[ResultOperatingAgencyID ] = 0;
    result[ResultAgencyManagementID] = 0;
    result[ResultChargingStationID ] = 0;
    result[ResultOperatingTime     ] = CommonConstants::Empty;
    result[ResultOperatingOrgan    ] = CommonConstants::Empty;
    result[ResultStatus            ] = CommonConstants::Empty;
    result[ResultChargingType      ] = CommonConstants::Empty;
    result[ResultChargerInfo       ] = CommonConstants::Empty;

    //공통
    result[ResultUserGuide         ] = CommonConstants::Empty;
    result[ResultInfomation        ] = CommonConstants::Empty;
    result[ResultNavseq            ] = CommonConstants::Empty;
    result[ResultOnline            ] = false;
    result[ResultBrand             ] = 0;
    result[ResultPrimaryMax        ] = 0;
    result[ResultPrimaryMin        ] = 0;
    result[ResultSecondaryMax      ] = 0;
    result[ResultSecondaryMin      ] = 0;
    result[ResultSort              ] = false;
    result[ResultType              ] = ResultTypeNormal;
    result[ResultAlongRoute        ] = false;
    result[ResultLineChanged       ] = false;
    result[ResultEnd               ] = 0;
}

QString SearchConstants::normalized(QVariant text)
{
    return StringHelper::instance()->normalized(text.toString());
}
























