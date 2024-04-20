#include "SearchResultDetailViewMediator.h"

#include <QQmlProperty>

#include "ApplicationConstants.h"
#include "Constants.h"
#include "RouteConstants.h"
#include "CommonConstants.h"
#include "SearchConstants.h"
#include "SettingConstants.h"
#include "MapConstants.h"
#include "NavibookmarkConstants.h"
#include "ZoneConstants.h"
#include "ToastsConstants.h"
#include <QJSValueIterator>
#include "../domain/VsmMapProxy.h"
#include "../mns/rpnsearch.h"
#include "ViewRegistry.h"
#include "StringHelper.h"
#include "../common/searchrequest.h"
#include "NavibookmarkProtocol.h"
#include "VSM_Map.h"
#include "VSM_Navi.h"
#include "mediator/app/MapViewMode.h"
#include "NavigationPropertyConstants.h"
#include "NavigationPropertyProtocol.h"
#include "NavigationControllerProtocol.h"
#include "NavigationControllerConstants.h"
#include "searchrequest.h"
#include "SearchResultViewMediator.h"
#include "VsmNaviProxy.h"
#include "PoiHelper.h"
#include "RgConstants.h"
#include "FavoriteDto.h"
#include "LangConstants.h"
#include "UtilConstants.h"
#include "SettingDto.h"
#include "PlatformHelper.h"
#include "BookmarkHelper.h"

using namespace SKT;

SearchResultDetailViewMediator::SearchResultDetailViewMediator( QObject* viewComponent )
    : AbstractViewMediator( viewComponent )
    , mIsFavoriteUpdated(false)
    , mIsRg(false)
    , mIsRoadName(false)
{
    requestAddressType();
}

SearchResultDetailViewMediator::~SearchResultDetailViewMediator()
{

}

QList<int> SearchResultDetailViewMediator::listNotificationInterests()
{
    static QList<int> notes = {
        SearchConstants::ResponseSearch,
        SearchConstants::ResponseIsFavorite,
        MapConstants::ResponseDriveMode,
        ZoneConstants::DoSoftKeyFunction,
        ZoneConstants::DoSoftkeyIndexMenuFunction,
        SearchConstants::UpdateView,
        SearchConstants::AddItemFailure,
        RouteConstants::ReplyToViaMenuText,
        RgConstants::ReplyToRouteGuidanceState,
        RgConstants::ChangedRouteGuidanceState,
        ApplicationConstants::ReplyToViewList,
        SearchConstants::ResponseFavorSearch,
        NavigationControllerConstants::ResponseController,
        NavigationControllerConstants::ResponseOnlineStatus,
        NavigationControllerConstants::ChangeOnlineStatus,
        SettingConstants::DialogAccepted,
        SettingConstants::DialogCanceled,
        SearchConstants::QueueChanged,
        NavigationControllerConstants::ChangeLanguage,
        SettingConstants::ApplySetting,
        SettingConstants::ChangedSetting,
        ApplicationConstants::UpdatedUserProfile,
        ApplicationConstants::ChangedUserProfile,
    };

    return notes;
}

bool SearchResultDetailViewMediator::onEvent( int event, const QVariant& data )
{
    //T_DEBUGV( "event: %d", event );
    switch ( event ) {
    case RequestViaMenuText:
        sendNotification(RouteConstants::RequestViaMenuText);
        break;

    case Activated: {
            sendNotification(MapConstants::ChangeMapViewMode,QVariantMap{{CommonConstants::Mode,MapViewMode::MapInquiry}});
            sendNotification(MapConstants::ShowCarvata, false);
            sendNotification(MapConstants::EnableCallout, false);

            sendNotification(Constants::CloseView, QVariantMap{{CommonConstants::ObjectName, ViewRegistry::objectName(ViewRegistry::NaviMenuView)}});
            QString objectName = QQmlProperty::read(mViewComponent, CommonConstants::ObjectName).toString();
            sendNotification(ZoneConstants::RegistSoftkey, objectName);
            sendNotification(ZoneConstants::SelectSoftkeyIndexMenuMode, data);

            QVariantMap body;
            body.insert( CommonConstants::ObjectName,   objectName);
            sendNotification(ApplicationConstants::RequestViewList, body);

            body.clear();
            body.insert( CommonConstants::Type, NavigationControllerProtocol::GetBluetoothStatus );
            sendNotification( NavigationControllerConstants::RequestController, body );

            sendNotification(MapConstants::EnableCallout, false);
            sendNotification(RgConstants::BlockReroute, true);
        }
        break;

    case QueryIsFavorite:
        sendNotification(SearchConstants::QueryIsFavorite, data);
        break;

    case Deactivated:
        sendNotification(ZoneConstants::SelectSoftkeyIndexMenuMode, QStringList{});
        sendNotification(MapConstants::EnableCallout, true);
        break;

    case RequestRouteGuidanceState:
        sendNotification(RgConstants::RequestRouteGuidanceState, getMediatorId());
        break;

    case AddFavorite:
        addFavorite(data);
        break;

    case RemoveFromFavorite:
        removeFromFavorite(data);
        break;

    case CreateView:
        {
            auto m = data.toMap();
            auto url = m[CommonConstants::Url].toString();
            if (url == ViewRegistry::url(ViewRegistry::CommonDialogView)) {
                PlatformHelper::instance()->setParentForSystemPopup(m);
            }
            sendNotification(Constants::CreateView, m);

            if (url == ViewRegistry::url(ViewRegistry::SearchTabView)) {
                if (m.contains(SearchConstants::Page)) {
                    int page = m[SearchConstants::Page].toInt();
                    sendNotification(SearchConstants::ChangeSearchTabViewPage, page);
                }
            }
        }
        break;

    case CloseView:
        sendNotification(Constants::CloseView, data);        
        break;

    case SelectZone1Title:
        sendNotification(ZoneConstants::SelectZone1Title, data);
        break;

    case RequestSearch: {
            QJSValue jsVal = data.value<QJSValue>();
            QString query = jsVal.property( SearchConstants::Query).toString();
            QVariant param = jsVal.property(SearchConstants::Params).toVariant();
            QVariantMap params = param.value<QVariantMap>();
            QString searchType  = jsVal.property(SearchConstants::SearchType).toString();

            QVariantMap body;
            if (searchType == SearchConstants::Detail) {
                body.insert( SearchConstants::SearchType, SearchRequestBase::SearchByPOIid);
            } else if (searchType == SearchConstants::EvDetail) {
                body.insert( SearchConstants::SearchType, SearchRequestBase::SearchByEvPOIid);
            }
            body.insert( SearchConstants::TextParam, query);
            body.insert( SearchConstants::Params, QVariant::fromValue(params));
            sendNotification(SearchConstants::RequestSearch, body);
        }
        break;

    case RequestPositionInScreenCenter:
        {            
        //MMV 18.4.18
//            auto proxy = Facade::instance()->retrieveProxy<VsmNaviProxy>();
//            auto pt = proxy->centerPosition();
//            sendNotification(RouteConstants::AddVia, pt);
            QVariantMap m {{CommonConstants::Type, QStringLiteral("center")}};
            sendNotification(MapConstants::RequestMapPosition, m);
        }
        break;

    case QueryInDriveMode:
        mDestData = data;
        queryInDriveMode();
        break;

    case RequestInTheWaypoint:
        sendToVia(data);

        // 경유지 편집 화면 이동 시 검색어 초기
        // 19.10.22 ymhong
        sendNotification(ZoneConstants::UpdateSearchText, "");
        break;

    case ViewSizeChanged: {
            QJSValue jsVal = data.value<QJSValue>();
            int bottom = jsVal.property( CommonConstants::B ).toInt();
            QVariantMap body;
            body.insert(SearchConstants::B, bottom);
            sendNotification(MapConstants::UpdateMapViewPadding, body);
        }
        break;

    case UpdateView:
        sendNotification(SearchConstants::UpdateView, data);
        break;

    case ReturnToFavoriteView:
        {
            auto caller = QQmlProperty::read(mViewComponent, CommonConstants::Caller).toString();

            if (caller == ViewRegistry::objectName(ViewRegistry::NaviMenuView)) {
                QVariantMap m;
                m[CommonConstants::Url] = ViewRegistry::url(ViewRegistry::NaviMenuView);
                sendNotification(Constants::CreateView, m);
            } else {
                QVariantMap m = data.toMap();
                m[CommonConstants::Url] = ViewRegistry::url(ViewRegistry::SearchTabView);
                sendNotification(Constants::CreateView, m);
                sendNotification(SearchConstants::ChangeSearchTabViewPage, 2);
                // qml softkey 처리 후 close()

                // 통합 검색을 통해 들어온 경우 입력 검색어 초기화
                sendNotification(ZoneConstants::UpdateSearchText, "");
            }
        }
        break;

     case ReturnToRecentDestinationView:
        {
            QVariantMap m = data.toMap();
            QString caller = m[CommonConstants::Caller].toString();
            if (caller == ViewRegistry::objectName(ViewRegistry::NaviMenuView) ||
                caller == ViewRegistry::objectName(ViewRegistry::BookmarkPageView)) {
                m.clear();
                m[CommonConstants::ObjectName] = ViewRegistry::objectName(ViewRegistry::RecentDestinationView);
                sendNotification(Constants::ShowView, m);
            } else {
                m[CommonConstants::Url] = ViewRegistry::url(ViewRegistry::SearchTabView);
                m[SearchConstants::Page] = 1;
                sendNotification(Constants::CreateView, m);
            }
        }
     break;

    case RequestAddMarker:
        sendAddSearchMarker(data);
        break;

    case RequestRemoveMarker:
        sendRemoveSearchMarker(data);
        break;

    case RequestAtThePOI: {
            QJSValue jsVal = data.value<QJSValue>();
            auto mapView = jsVal.property( SearchConstants::MapView ).toVariant().value<QObject*>();
            auto mapViewHandle = qvariant_cast<void*>(mapView->property("nativeHandle"));

            int x = jsVal.property( CommonConstants::X).toInt();
            int y = jsVal.property( CommonConstants::Y ).toInt();
            VSMMap_MapMoveTo(mapViewHandle, x,  y, true);
        }
        break;

    case ModalChanged:
        sendNotification(ZoneConstants::UpdateModal,data.toMap());
        break;

    case CallPhoneNumber: {
            QJSValue jsVal = data.value<QJSValue>();
            QString phoneNumber = jsVal.property( SearchConstants::PhoneNumber ).toString();
            QVariantMap body;
            body.insert( CommonConstants::Type, NavigationControllerProtocol::CallPhoneNumber );
            body.insert( SearchConstants::PhoneNumber, phoneNumber );
            sendNotification( NavigationControllerConstants::ApplyController, body );
        }
        break;

    case ResetCsw:
        sendNotification(NavigationControllerConstants::ResetCSW);
        break;

    case RegistCsw:
        sendNotification(NavigationControllerConstants::RegistCSW, data);
        break;

    case ShowToast:
        sendNotification(ToastsConstants::ShowToast, data);
        break;

    case QueryOnlineStatus:
        sendNotification(NavigationControllerConstants::QueryOnlineStatus);
        break;

    case ShowProgressIndicator:
        sendNotification( UtilConstants::ShowProgressIndicator );
        break;

    case HideProgressIndicator:
        sendNotification( UtilConstants::HideProgressIndicator );
        break;

    case RequestAddressType:
        requestAddressType();
        break;

    case RemoveSearchQueue: {
              QVariantMap body;
              body.insert( SearchConstants::SearchType, SearchRequestBase::RemoveSearchQueue);
              sendNotification(SearchConstants::RequestSearch, body);
          }
          break;

    case RequestLang: {
            QVariantMap body;
            body.insert(CommonConstants::Type, NavigationControllerProtocol::GetLang );
            sendNotification( NavigationControllerConstants::RequestController, body );
        }
        break;

    case ShowView: {
            sendNotification(SearchConstants::UpdateSearchResultText);
            sendNotification(Constants::ShowView, data);
        }
        break;

     case UpdateMapView:
        sendNotification(MapConstants::UpdateMapView);
        break;

     case HideTabViewContent:
        sendNotification(SearchConstants::HideTabViewContent);
        break;

    case RequestSetting: {
            QJSValue jsVal = data.value<QJSValue>();
            QString section = jsVal.property( CommonConstants::Section ).toString();

            QVariantMap body;
            body.insert( CommonConstants::Section, section );
            if (jsVal.hasOwnProperty(CommonConstants::Var)) {
                QString var = jsVal.property( CommonConstants::Var ).toString();
                body.insert( CommonConstants::Var, var);
            }
            if (jsVal.hasOwnProperty(CommonConstants::Slot)) {
                QString slot = jsVal.property( CommonConstants::Slot ).toString();
                body.insert( CommonConstants::Slot, slot);
            }
            sendNotification( SettingConstants::RequestSetting, body );
        }
        break;

    default:
        return false;
    }
    return true;
}

bool SearchResultDetailViewMediator::handleNotification( const mvc::INotificationPtr& note )
{
    switch ( note->getId() ) {
    case SearchConstants::UpdateView:
    case RgConstants::ReplyToRouteGuidanceState:
    case RgConstants::ChangedRouteGuidanceState:
    case NavigationControllerConstants::ResponseOnlineStatus:
    case NavigationControllerConstants::ChangeOnlineStatus:
    case NavigationControllerConstants::ChangeLanguage:
    case SettingConstants::ApplySetting:
    case SettingConstants::ChangedSetting:
    case ApplicationConstants::UpdatedUserProfile:
    case ApplicationConstants::ChangedUserProfile:
        break;
    default: {
            if (QQmlProperty::read(mViewComponent, CommonConstants::IsClosed).toBool()) {
                switch ( note->getId() ) {
                    case SearchConstants::ResponseSearch:
                    case SearchConstants::ResponseFavorSearch:	{
                            QVariantMap data = note->getBody().value<QVariantMap>();
                            QVariantMap reqData = data[SearchConstants::ReqData].value<QVariantMap>();
                            auto owner = reinterpret_cast<void*>(reqData[CommonConstants::Owner].toULongLong());
                            if (owner != this)
                                 return false;
                            sendNotification( UtilConstants::HideProgressIndicator );
                     }
                     break;
                }
                return false;
            }
        }
        break;
    }

    switch ( note->getId() ) {
    case RouteConstants::ReplyToViaMenuText:
        {
            auto text = note->getBody().toString();
            QQmlProperty::write(mViewComponent, "viaMenuText", text);
        }
        break;
    case ApplicationConstants::ReplyToViewList:
        {
            auto data = note->getBody().toMap();            
            QString objectName = data[CommonConstants::ObjectName].value<QString>();
            if (objectName == ViewRegistry::objectName(ViewRegistry::SimulationView)  ||
                objectName == ViewRegistry::objectName(ViewRegistry::RouteCandidateView) ||
                objectName == ViewRegistry::objectName(ViewRegistry::SearchResultDetailView)) {
                mViewList = data[ApplicationConstants::ViewIdList].value<QList<int>>();
                break;
            }

            if (objectName != ViewRegistry::objectName(ViewRegistry::SearchResultDetailView))
                break;
            auto viewList = data[ApplicationConstants::ViewIdList].value<QList<int>>();
            auto viewDataMap = data[ApplicationConstants::ViewDataMap].value<QMap<int,QVariantMap>>();
            QVariantList values = QVariantList{};
            for ( auto viewId :viewList ) {
                QVariantList result;
                CommonConstants::resultViewInit(result);
                result[CommonConstants::ResultViewId        ] = viewId;
                result[CommonConstants::ResultViewName      ] = ViewRegistry::objectName( viewId );
                result[CommonConstants::ResultViewVisible   ] = viewDataMap[viewId][ApplicationConstants::Visible].toBool();
                result[CommonConstants::ResultViewActive    ] = viewDataMap[viewId][ApplicationConstants::Active].toBool();
                values.append(QVariant::fromValue(result));
            }
            QMetaObject::invokeMethod(mViewComponent, "__onResultViewList",Q_ARG(QVariant, QVariant::fromValue(values)) );
        }
        break;

    case SearchConstants::ResponseSearch: {
            QVariantMap data = note->getBody().value<QVariantMap>();
            if (!data.contains(SearchConstants::Result)) {
                return false;
            }

            QVariantList values = QVariantList{};
            int totalCount = 0;
            QString search = CommonConstants::Empty;
            int startIndex = -1;
            QVariantMap reqData;
            if (data.contains(SearchConstants::ReqData)) {
                reqData = data[SearchConstants::ReqData].value<QVariantMap>();
                auto owner = reinterpret_cast<void*>(reqData[CommonConstants::Owner].toULongLong());
                if (owner != this)
                    return false;
                if (reqData.contains(SearchConstants::Query)){
                    search = reqData[SearchConstants::Query].value<QString>();
                }
                if (reqData.contains(SearchConstants::Params)){
                    QVariantMap params = reqData[SearchConstants::Params].value<QVariantMap>();
                    if (params.contains(SearchConstants::StartIndex)){
                        startIndex = params[SearchConstants::StartIndex].value<int>();
                    }
                }
            }

            QObject* res = data[SearchConstants::Result].value<QObject*>();
            if (!res) {
                QMetaObject::invokeMethod(mViewComponent, "__onResult", Q_ARG(QVariant, QVariant::fromValue(search)),
                                                                        Q_ARG(QVariant, QVariant::fromValue(values)),
                                                                        Q_ARG(QVariant, QVariant::fromValue(totalCount)),
                                                                        Q_ARG(QVariant, QVariant::fromValue(startIndex)) );
                return false;
            }
            SearchReplyBaseInterface *reply = qobject_cast<SearchReplyBaseInterface*>(res);
            if (!reply || reply->error()) {
                QMetaObject::invokeMethod(mViewComponent, "__onResult", Q_ARG(QVariant, QVariant::fromValue(search)),
                                                                        Q_ARG(QVariant, QVariant::fromValue(values)),
                                                                        Q_ARG(QVariant, QVariant::fromValue(totalCount)),
                                                                        Q_ARG(QVariant, QVariant::fromValue(startIndex)) );
                return false;
            }

            if (reqData.contains(SearchConstants::SearchType)) {                
                int searchType = reqData[SearchConstants::SearchType].value<int>();
                if (searchType == SearchRequestBase::SearchByPOIid) {
                    SearchPoiReplyInterface* searchReply = qobject_cast<SearchPoiReplyInterface*>(res);
                    if (!searchReply) {
                        QMetaObject::invokeMethod(mViewComponent, "__onResult", Q_ARG(QVariant, QVariant::fromValue(search)),
                                                                                Q_ARG(QVariant, QVariant::fromValue(values)),
                                                                                Q_ARG(QVariant, QVariant::fromValue(totalCount)),
                                                                                Q_ARG(QVariant, QVariant::fromValue(startIndex)) );
                        return false;
                    }
                    const TPOI_DETAIL_INFO listitem = *searchReply->results();

                    auto isOnline = reply->request()->getOnline();
                    auto navSeq= isOnline ? PoiHelper::navSeqFromServer(listitem.szNavseq)
                                          : PoiHelper::navSeqFromLocal(listitem.nPoiID);
                    auto poiId = isOnline ? PoiHelper::fromServer(listitem.nPoiID, navSeq.toInt())
                                          : PoiHelper::fromLocal(listitem.nPoiID);



                    //listitem.szDayOffInfo,listitem.szBusinessHours의 데이타가 있는 경우만 사용하도록 수정
                    QString strAddInfo = StringHelper::instance()->fromCp949((const char*)listitem.szAddInformation );
                    if (strlen(listitem.szDayOffInfo)) {
                        QString strDayOffInfo = StringHelper::instance()->fromCp949((const char*)listitem.szDayOffInfo );
                        strDayOffInfo = strDayOffInfo.replace(";", ",");
                        strAddInfo += ";["+SearchConstants::HollidayKo+"]"+strDayOffInfo;
                    }
                    if (strlen(listitem.szBusinessHours)) {
                        QString strBusinessHours = StringHelper::instance()->fromCp949((const char*)listitem.szBusinessHours);
                        strBusinessHours = strBusinessHours.replace(";", ",");
                        strAddInfo += ";["+SearchConstants::WorkTimeKo+"]"+strBusinessHours;
                    }

                    SearchConstants::resultInit(values);
                    values[SearchConstants::ResultName                ] = StringHelper::instance()->fromCp949((const char*)listitem.szName);
                    values[SearchConstants::ResultStdName             ] = StringHelper::instance()->fromCp949((const char*)listitem.szName);
                    values[SearchConstants::ResultAddress             ] = StringHelper::instance()->fromCp949((const char*)listitem.szAddrName);
                    values[SearchConstants::ResultTelNum              ] = StringHelper::instance()->fromCp949((const char*)listitem.szPhoneNumber);
                    values[SearchConstants::ResultPOIID               ] = poiId;
                    values[SearchConstants::ResultKind                ] = listitem.nDataKind;
                    values[SearchConstants::ResultWebSite             ] = listitem.szUrl;
                    values[SearchConstants::ResultHolliday            ] = listitem.bHoliday;
                    values[SearchConstants::ResultParking             ] = listitem.byPark;
                    values[SearchConstants::ResultIndustry            ] = StringHelper::instance()->fromCp949((const char*)listitem.szCateName);
                    values[SearchConstants::ResultIntroduction        ] = StringHelper::instance()->fromCp949((const char*)listitem.szInfo);
                    values[SearchConstants::ResultUserGuide           ] = StringHelper::instance()->fromCp949((const char*)listitem.szUserGuide);
                    values[SearchConstants::ResultInfomation          ] = StringHelper::instance()->fromCp949((const char*)listitem.szInformation );                    
                    values[SearchConstants::ResultX                   ] = listitem.tp.x;
                    values[SearchConstants::ResultY                   ] = listitem.tp.y;
                    values[SearchConstants::ResultWorkTime            ] = strAddInfo;
                    values[SearchConstants::ResultYear                ] = listitem.sGatherTime.nYear;
                    values[SearchConstants::ResultMonth               ] = listitem.sGatherTime.nMonth;
                    values[SearchConstants::ResultDay                 ] = listitem.sGatherTime.nDay;
                    values[SearchConstants::ResultHour                ] = listitem.sGatherTime.nHour;
                    values[SearchConstants::ResultMinute              ] = listitem.sGatherTime.nMinute;
                    values[SearchConstants::ResultSecond              ] = listitem.sGatherTime.nSecond;
                    values[SearchConstants::ResultWeek                ] = listitem.sGatherTime.nWeek;
                    values[SearchConstants::ResultHighGasoline        ] = listitem.bHighGasoline;
                    values[SearchConstants::ResultOPIGasolinePrice    ] = listitem.nOPIGasolinePrice;
                    values[SearchConstants::ResultOPIDieselPrice      ] = listitem.nOPIDieselPrice;
                    values[SearchConstants::ResultOPILPGPrice         ] = listitem.nOPILPGPrice;
                    values[SearchConstants::ResultOPIHiGasolinePrice  ] = listitem.nOPIHiGasolinePrice;
                    values[SearchConstants::ResultOPIHiLPGPrice       ] = listitem.nOPIHiLPGPrice;
                    values[SearchConstants::ResultNavseq              ] = navSeq;
                    values[SearchConstants::ResultOnline              ] = isOnline;
                    totalCount = searchReply->resultCount();
                } else if (searchType == SearchRequestBase::SearchByEvPOIid) {
                    SearchEvPoiReplyInterface* searchReply = qobject_cast<SearchEvPoiReplyInterface*>(res);
                    if (!searchReply) {
                        QMetaObject::invokeMethod(mViewComponent, "__onResult", Q_ARG(QVariant, QVariant::fromValue(search)),
                                                                                Q_ARG(QVariant, QVariant::fromValue(values)),
                                                                                Q_ARG(QVariant, QVariant::fromValue(totalCount)),
                                                                                Q_ARG(QVariant, QVariant::fromValue(startIndex)) );
                        return false;
                    }

                    const TPOI_EvDETAIL_INFO listitem = *searchReply->results();
                    auto isOnline = reply->request()->getOnline();
                    auto navSeq= isOnline ? PoiHelper::navSeqFromServer(listitem.szNavseq)
                                          : PoiHelper::navSeqFromLocal(listitem.nPoiID);
                    auto poiId = isOnline ? PoiHelper::fromServer(listitem.nPoiID, navSeq.toInt())
                                          : PoiHelper::fromLocal(listitem.nPoiID);


                    QString strAddr = CommonConstants::Empty;
                    QString strDetailLocation = CommonConstants::Empty;
                    if (strlen(listitem.szAddrDetail)) {
                       strDetailLocation =  StringHelper::instance()->fromCp949((const char*)listitem.szAddrDetail);
                    }
                    if (mIsRoadName) {
                        strAddr = StringHelper::instance()->fromCp949((const char*)listitem.szRoadAddr);
                        strAddr.trimmed();
                        if (!isOnline) {
                           strDetailLocation = StringHelper::instance()->fromCp949((const char*)listitem.szPoidRoadAddr);
                        }
                    } else {
                        if (!isOnline) {
                           strDetailLocation = StringHelper::instance()->fromCp949((const char*)listitem.szPoidAddr);
                        }
                    }
                    strAddr.trimmed();
                    strDetailLocation.trimmed();
                    if (strAddr.isEmpty())
                        strAddr = StringHelper::instance()->fromCp949((const char*)listitem.szAddrName);

                    SearchConstants::resultEvInit(values);
                    values[SearchConstants::ResultName                ] = StringHelper::instance()->fromCp949((const char*)listitem.szName);
                    values[SearchConstants::ResultStdName             ] = values[SearchConstants::ResultName];
                    values[SearchConstants::ResultAddress             ] = strAddr;
                    values[SearchConstants::ResultTelNum              ] = StringHelper::instance()->fromCp949((const char*)listitem.szPhoneNumber);
                    values[SearchConstants::ResultPOIID               ] = poiId;
                    values[SearchConstants::ResultX                   ] = listitem.tp.x;
                    values[SearchConstants::ResultY                   ] = listitem.tp.y;
                    values[SearchConstants::ResultFastCharge          ] = (listitem.bDCDemo || listitem.bAC3 || listitem.bDCCombo);
                    values[SearchConstants::ResultSlowCharge          ] = listitem.bSlowChage;
                    values[SearchConstants::ResultDCDemo              ] = listitem.bDCDemo;
                    values[SearchConstants::ResultAC3                 ] = listitem.bAC3;
                    values[SearchConstants::ResultDCCombo             ] = listitem.bDCCombo;
                    values[SearchConstants::ResultStationCount        ] = listitem.evStationCnt;
                    values[SearchConstants::ResultOperatingTime       ] = StringHelper::instance()->fromCp949((const char*)listitem.szUseTime);
                    values[SearchConstants::ResultChargingFee         ] = StringHelper::instance()->fromCp949((const char*)listitem.szFee);
                    values[SearchConstants::ResultParkingFree         ] = listitem.bFreeYn;
                    values[SearchConstants::ResultReserveYn           ] = listitem.bReservYn;
                    values[SearchConstants::ResultDetailLocation      ] = strDetailLocation;
                    values[SearchConstants::ResultOperatingOrgan      ] = StringHelper::instance()->fromCp949((const char*)listitem.szMngName);
                    values[SearchConstants::ResultAgencyManagementID  ] = StringHelper::instance()->fromCp949((const char*)listitem.szMngId);
                    QString strInfo = CommonConstants::Empty;                    
                    for (int j=0; j< listitem.evStationCnt; j++) {                        
                        DS_POI_EVSTATION_CHARGERINFO chargerInfo = listitem.chargerInfo[j];
                        strInfo += QString("[%1]%2;[%3]%4;[%5]%6;[%7]%8;[%9]%10;")
                                .arg(SearchConstants::Bid)
                                .arg(chargerInfo.szBid)
                                .arg(SearchConstants::Sid)
                                .arg(chargerInfo.szSid)
                                .arg(SearchConstants::Cid)
                                .arg(chargerInfo.szCid)
                                .arg(SearchConstants::Tid)
                                .arg(chargerInfo.szType)
                                .arg(SearchConstants::StId)
                                .arg(chargerInfo.nStatus);
                    }
                    values[SearchConstants::ResultChargerInfo         ] = strInfo;
                    values[SearchConstants::ResultNavseq              ] = navSeq;
                    values[SearchConstants::ResultOnline              ] = isOnline;
                    totalCount = searchReply->resultCount();
                }
            }
            QMetaObject::invokeMethod(mViewComponent, "__onResult", Q_ARG(QVariant, QVariant::fromValue(search)),
                                                                    Q_ARG(QVariant, QVariant::fromValue(values)),
                                                                    Q_ARG(QVariant, QVariant::fromValue(totalCount)),
                                                                    Q_ARG(QVariant, QVariant::fromValue(0)) );
        }
        break;
    case SearchConstants::ResponseIsFavorite:
        {
            int id = note->getBody().toInt();
            bool isOn = id != 0; // 0은 DB에 데이터 없음
            QMetaObject::invokeMethod(mViewComponent, "__setFavoriteOn", Q_ARG(QVariant, isOn));
        }
        break;

    case RgConstants::ReplyToRouteGuidanceState:
    case RgConstants::ChangedRouteGuidanceState: {
            QVariantMap m = note->getBody().toMap();
            bool isRg = m["isRg"].toBool();
            mIsRg = isRg;
            QMetaObject::invokeMethod(mViewComponent, "__changedRgState", Q_ARG(QVariant, isRg));
        }
        break;

    case MapConstants::ResponseDriveMode:
        {
            int driveMode = note->getBody().toInt();
            bool isRg = driveMode == MapViewMode::Navi;
            if (isVisible() && mDestData.isValid()) {
                if (isRg) {
                    QVariantMap m = mDestData.toMap();
                    auto address = m[CommonConstants::Address].toString();
                    showConfirmDialog(address);
                } else {
                    sendByDestination(mDestData);
                    QMetaObject::invokeMethod(mViewComponent, "closeAllView");

                    // 경로 탐색 시 검색어 초기화
                    // 19.10.22 ymhong
                    sendNotification(ZoneConstants::UpdateSearchText, "");
                }
            }
            mDestData.clear();
        }
        break;
    case ZoneConstants::DoSoftKeyFunction:
    case ZoneConstants::DoSoftkeyIndexMenuFunction:{
            QVariantMap data = note->getBody().value<QVariantMap>();
            QString objectName = data[CommonConstants::Name].toString();
            if (objectName != ViewRegistry::objectName(ViewRegistry::SearchResultDetailView)) {
                if (!isVisible() || !isActive())
                    return false;
            }
            QString objFunction = CommonConstants::Empty;
            QString objMenu = CommonConstants::Empty;
            if (data.contains(ZoneConstants::Function))
                objFunction = data[ZoneConstants::Function].toString();
            if (data.contains(ZoneConstants::Menu) )
                objMenu = data[ZoneConstants::Menu].toString();
            int  objIndex  = -1;
            if (data.contains(ZoneConstants::Index))
                objIndex = data[ZoneConstants::Index].toInt();
            QMetaObject::invokeMethod(mViewComponent, "__doSoftKeyFunction", Q_ARG(QVariant, QVariant::fromValue(objFunction)), Q_ARG(QVariant, QVariant::fromValue(objMenu)), Q_ARG(QVariant, QVariant::fromValue(objIndex)) );
        }
        break;
    case SearchConstants::UpdateView: {
            qDebug() << "[Perf][UpdateView]before";
            QVariantMap data = note->getBody().value<QVariantMap>();
            QString objectName = data[CommonConstants::ObjectName].toString();
            if (objectName != ViewRegistry::objectName(ViewRegistry::SearchResultDetailView))
                return false;
            qDebug() << "[Perf][UpdateView]objectName: " << objectName;
            qDebug() << "[Perf][UpdateView]after";
            for ( auto it = data.constBegin(); it != data.constEnd(); it++ ){
                if (CommonConstants::Url == it.key() ||
                    CommonConstants::ObjectName == it.key() ||
                    CommonConstants::ReFocus == it.key() ||
                    CommonConstants::Visible == it.key()) {
                    continue;
                }
                QQmlProperty::write( mViewComponent, it.key(), QVariant::fromValue( it.value() ) );
            }
            if (data.contains(CommonConstants::Visible)) {
                QQmlProperty::write( mViewComponent, CommonConstants::Visible, QVariant::fromValue( data[CommonConstants::Visible]) );
            }
            if (data.contains(CommonConstants::ReFocus)) {
                QQmlProperty::write( mViewComponent, CommonConstants::ReFocus, QVariant::fromValue( data[CommonConstants::ReFocus]) );
            }
        }
        break;
    case SearchConstants::AddItemFailure:
        {
            auto m = note->getBody().toMap();
            auto type = m["itemType"].toString();
            if (type == CommonConstants::Favorite) {
                QMetaObject::invokeMethod(mViewComponent, "__setFavoriteOn", Q_ARG(QVariant, false));
            }
        }
        break;
    case SearchConstants::ResponseFavorSearch: {
            QVariantMap data = note->getBody().value<QVariantMap>();
            if (!data.contains(SearchConstants::Result))
                return false;

            QString search = CommonConstants::Empty;
            static int totalCount = 0;
            int startIndex = 0;
            if (data.contains(SearchConstants::ReqData)) {
                QVariantMap reqData = data[SearchConstants::ReqData].value<QVariantMap>();
                auto owner = reinterpret_cast<void*>(reqData[CommonConstants::Owner].toULongLong());
                if (owner != this)
                    return false;
                if (reqData.contains(SearchConstants::Query)){
                    search = reqData[SearchConstants::Query].value<QString>();
                }

                if(startIndex == 0) {
                    if (reqData.contains(SearchConstants::Max)){
                        totalCount = reqData[SearchConstants::Max].value<int>();
                    }
                }

                if (reqData.contains(SearchConstants::Params)){
                    QVariantMap params = reqData[SearchConstants::Params].value<QVariantMap>();
                    if (params.contains(SearchConstants::StartIndex)){
                        startIndex = params[SearchConstants::StartIndex].value<int>();
                    }
                }
            }
            QList<FavoriteDtoPtr> list = data[SearchConstants::Result].value<QList<FavoriteDtoPtr>>();
            QVariantList values = QVariantList{};
            for (int i=0 ; i < list.size(); i++) {
                FavoriteDtoPtr listitem = list.at(i);
                QVariantList result;
                SearchConstants::resultInit(result);
                result[SearchConstants::ResultIndex               ] = listitem->id();
                result[SearchConstants::ResultName                ] = listitem->dest();
                result[SearchConstants::ResultStdName             ] = listitem->dest();
                result[SearchConstants::ResultAddress             ] = listitem->address();
                result[SearchConstants::ResultPOIID               ] = listitem->poiID();
                result[SearchConstants::ResultX                   ] = listitem->mapX();
                result[SearchConstants::ResultY                   ] = listitem->mapY();
                result[SearchConstants::ResultType                ] = SearchConstants::ResultTypeFavor;
                values.append(QVariant::fromValue(result));
            }


            QMetaObject::invokeMethod(mViewComponent, "__onResultFavor", Q_ARG(QVariant, QVariant::fromValue(search)),
                                                                    Q_ARG(QVariant, QVariant::fromValue(values)),
                                                                    Q_ARG(QVariant, QVariant::fromValue(totalCount)),
                                                                    Q_ARG(QVariant, QVariant::fromValue(startIndex)) );

          }
          break;

    case NavigationControllerConstants::ResponseController: {
            QVariantMap data = note->getBody().value<QVariantMap>();
            if (data.contains(SearchConstants::BluetoothStatus)) {
                bool value = data[SearchConstants::BluetoothStatus].toBool();
                QMetaObject::invokeMethod(mViewComponent, "__onBluetoothStatus", Q_ARG(QVariant, QVariant::fromValue(value)) );
            } else if (data.contains(SearchConstants::CallStatus)) {
                int value = data[SearchConstants::CallStatus].toInt();
                QMetaObject::invokeMethod(mViewComponent, "__onCallStatus", Q_ARG(QVariant, QVariant::fromValue(value)) );
            } else if (data.contains(CommonConstants::Lang)) {
                QString value = data[CommonConstants::Lang].toString();
                QQmlProperty::write( mViewComponent, "__lang", QVariant::fromValue(value) );
            }
        }
        break;

    case NavigationControllerConstants::ResponseOnlineStatus:
    case NavigationControllerConstants::ChangeOnlineStatus:   {
            bool isOnline = note->getBody().toBool();
            QMetaObject::invokeMethod(mViewComponent, "__changedOnlineStatus", Q_ARG(QVariant, isOnline));
        }
        break;

    case SettingConstants::DialogAccepted: {
            QVariantMap data = note->getBody().value<QVariantMap>();
            QString caller = data[CommonConstants::Caller].toString();
            QVariantMap m = data[CommonConstants::Value].toMap();
            QString objectName = QQmlProperty::read(mViewComponent, CommonConstants::ObjectName).toString();
            if (caller != objectName)
                break;
            QMetaObject::invokeMethod(mViewComponent, "__closeDialog", Q_ARG(QVariant, true));
        }
        break;

    case SettingConstants::DialogCanceled: {
            QVariantMap data = note->getBody().value<QVariantMap>();
            QString caller = data[CommonConstants::Caller].toString();
            QVariantMap m = data[CommonConstants::Value].toMap();
            if (caller != ViewRegistry::objectName(ViewRegistry::SearchResultDetailView))
                break;
            QMetaObject::invokeMethod(mViewComponent, "__closeDialog", Q_ARG(QVariant, false));
        }
        break;

    case SearchConstants::QueueChanged: {
            int count = note->getBody().value<int>();
            QMetaObject::invokeMethod(mViewComponent, "__queueChanged", Q_ARG(QVariant, count));
        }
        break;

    case NavigationControllerConstants::ChangeLanguage: {
            auto data = note->getBody().value<QVariantMap>();
            auto lang = data[LangConstants::Lang].toString();
            QMetaObject::invokeMethod(mViewComponent, "__retranslateUi", Q_ARG(QVariant, QVariant::fromValue(lang)));
        }
        break;

    case SettingConstants::ApplySetting: {
            QVariantMap data = note->getBody().value<QVariantMap>();
            auto owner = reinterpret_cast<void*>(data[CommonConstants::Owner].toULongLong());
            if (owner != this)
                break;
            QString section = data[CommonConstants::Section].toString();
            QString key = data[CommonConstants::Key].toString();
            if ( section == SettingConstants::MapMain &&
                 key == SettingConstants::AddressDisplayMethod ) {
                int method = data[CommonConstants::Setting].value<SettingDtoPtr>()->value().toInt();
                mIsRoadName = (method == SettingConstants::AddressDisplayMethodRoadName) ? true : false;
                QQmlProperty::write(mViewComponent, SearchConstants::AddressType,
                                                    (method == SettingConstants::AddressDisplayMethodLandNumber ) ?
                                                     SearchConstants::OldAddrType : SearchConstants::NewAddrType );
            } else {
                auto settings = data[CommonConstants::Settings].value<QList<SettingDtoPtr>>();
                QVariantMap value;
                for ( auto setting : settings ) {
                    value[setting->key().toString()] =
                    QVariantMap({
                        {CommonConstants::Type,setting->type()},
                        {CommonConstants::Section,setting->section()},
                        {CommonConstants::Key,setting->key()},
                        {CommonConstants::Value,setting->value()}
                    });
                }
                if (data.contains(CommonConstants::Var)) {
                    QString var = data[CommonConstants::Var].toString();
                    QQmlProperty::write( mViewComponent, var, QVariant::fromValue( value ) );
                } else if (data.contains(CommonConstants::Slot)) {
                    QString slot = data[CommonConstants::Slot].toString();
                    QMetaObject::invokeMethod(mViewComponent, slot.toLocal8Bit().data(), Q_ARG(QVariant, value));
                } else {
                   QQmlProperty::write( mViewComponent, CommonConstants::Settings, QVariant::fromValue( value ) );
                }
            }
        }
        break;
    case SettingConstants::ChangedSetting:
        {
            QVariantMap data = note->getBody().value<QVariantMap>();
            QString section = data[CommonConstants::Section].toString();
            QString key = data[CommonConstants::Key].toString();
            if ( section == SettingConstants::MapMain && key == SettingConstants::AddressDisplayMethod ) {
                int method = data[CommonConstants::Setting].value<SettingDtoPtr>()->value().toInt();
                mIsRoadName = (method == SettingConstants::AddressDisplayMethodRoadName) ? true : false;
                QQmlProperty::write(mViewComponent, SearchConstants::AddressType,
                                                    (method == SettingConstants::AddressDisplayMethodLandNumber ) ?
                                                     SearchConstants::OldAddrType : SearchConstants::NewAddrType );
            }
        }
        break;

    case ApplicationConstants::UpdatedUserProfile:
        QMetaObject::invokeMethod(mViewComponent, "__updatedUserProfile");
        break;
    case ApplicationConstants::ChangedUserProfile:
        QMetaObject::invokeMethod(mViewComponent, "__changedUserProfile");
        break;
    default:
        return false;
    }
    return true;
}

void SearchResultDetailViewMediator::onRemove()
{
    if (mIsFavoriteUpdated) 
        sendNotification(SearchConstants::UpdateBookmarkPageView);
}

void SearchResultDetailViewMediator::addFavorite(const QVariant &data)
{
    QJSValue jsVal = data.value<QJSValue>();
    int x = jsVal.property( CommonConstants::X).toInt();
    int y = jsVal.property( CommonConstants::Y ).toInt();
    QString name = jsVal.property( CommonConstants::Name).toString();
    QString address = jsVal.property( CommonConstants::Address).toString();
    int poiId   = jsVal.property( SearchConstants::PoiID ).toInt();
    bool isHome = jsVal.property( SearchConstants::IsHome).toBool();
    bool isOffice = jsVal.property(SearchConstants::IsOffice).toBool();
    auto navSeq = jsVal.property(SearchConstants::NavSeq).toInt();
    int rpFlag = jsVal.property(SearchConstants::RpFlag).toInt();
    int poleCate = jsVal.property(SearchConstants::OpiPoleCate).toInt();
    auto caller = jsVal.property(CommonConstants::Caller).toString();


    int destType = jsVal.property(SearchConstants::DestType).toInt();

    QVariantMap body;
    body[SearchConstants::Dest] = name;
    body[CommonConstants::Address] = address;
    body[CommonConstants::X] = x;
    body[CommonConstants::Y] = y;
    body[SearchConstants::PoiID] = poiId;
    body[SearchConstants::Favorite] = true;
    body[SearchConstants::IsHome] = isHome;
    body[SearchConstants::IsOffice] = isOffice;
    body[SearchConstants::NavSeq] = navSeq;
    body[SearchConstants::RpFlag] = rpFlag;
    body[SearchConstants::OpiPoleCate] = poleCate;
    body[SearchConstants::DestType] = destType;
    body[CommonConstants::Caller] = caller;
    
    sendNotification(SearchConstants::AddFavorite, body);

    mIsFavoriteUpdated ^= true;
}

void SearchResultDetailViewMediator::removeFromFavorite(const QVariant &v)
{
    sendNotification(SearchConstants::DeleteFavorite, v);

    auto m = v.toMap();
    auto destType = m[SearchConstants::DestType].toInt();
    m.clear();

    if (destType) {
        auto pos = new TS_ROUTE_POS;
        memset(pos, 0, sizeof(TS_ROUTE_POS));

        m["propertyValue"] = QVariant::fromValue(RoutePosPtr(pos));

        if (destType == SearchConstants::DestTypeOffice) {
            m["type"] = NavigationPropertyProtocol::TmapWorkLocation;
            sendNotification(NavigationPropertyConstants::setNavigationProperty, m);
        } else if(destType == SearchConstants::DestTypeHome) {
            m["type"] = NavigationPropertyProtocol::TmapHomeLocation;
            sendNotification(NavigationPropertyConstants::setNavigationProperty, m);
        }
    }
    mIsFavoriteUpdated ^= true;
}

void SearchResultDetailViewMediator::close()
{
    QMetaObject::invokeMethod(mViewComponent, "close");
}

QString SearchResultDetailViewMediator::caller() const
{
    return QQmlProperty::read(mViewComponent, CommonConstants::Caller).toString();
}

void SearchResultDetailViewMediator::queryInDriveMode()
{
    sendNotification(MapConstants::RequestDriveMode);
}

bool SearchResultDetailViewMediator::sendByDestination(const QVariant &data)
{
    QJSValue jsVal = data.value<QJSValue>();
    int x = jsVal.property( CommonConstants::X).toInt();
    int y = jsVal.property( CommonConstants::Y ).toInt();
    int poiId = jsVal.property( SearchConstants::PoiID).toInt();
    int navSeq = jsVal.property( SearchConstants::NavSeq).toInt();
    int rpFlag = jsVal.property(SearchConstants::RpFlag).toInt();
    int poleCate = jsVal.property( SearchConstants::OpiPoleCate).toInt();
    // int opiPrice = jsVal.property( SearchConstants::OpiPrice).toInt();
    QString name = jsVal.property( CommonConstants::Name).toString();
    QString address = jsVal.property( CommonConstants::Address).toString();

    sendNotification(Constants::CloseView, QVariantMap{{CommonConstants::ObjectName, ViewRegistry::objectName(ViewRegistry::RouteCandidateView)}});

    QVariantMap body;
    body[SearchConstants::Clear] = true;
    sendNotification(RouteConstants::SetDeparture, body);
    body.clear();

    body[CommonConstants::X] = x;
    body[CommonConstants::Y] = y;
    body[CommonConstants::Name] = name;
    body[CommonConstants::Address] = address;
    body[SearchConstants::PoiID] = poiId;
    body[SearchConstants::NavSeq] = navSeq;
    body[SearchConstants::RpFlag] = rpFlag;    
    body[SearchConstants::OpiPoleCate] = poleCate;

    sendNotification(RouteConstants::SetArrival, body);

    // navi bookmark > select address
    QVariantMap bookmark;
    bookmark.insert( CommonConstants::Name, name);
    bookmark.insert( CommonConstants::Address, address);
    bookmark.insert( CommonConstants::X, x);
    bookmark.insert( CommonConstants::Y, y);
    bookmark.insert( SearchConstants::PoiID, poiId);
    bookmark.insert( SearchConstants::NavSeq, navSeq);
    bookmark.insert( SearchConstants::RpFlag, rpFlag);
    bookmark.insert( SearchConstants::OpiPoleCate, poleCate);
    // bookmark.insert( SearchConstants::OpiPrice, opiPrice);
    sendNotification(NavibookmarkConstants::SelectNavibookAddress,bookmark);

    sendNotification(Constants::CreateView,QVariantMap{ {CommonConstants::Url,ViewRegistry::url(ViewRegistry::RouteCandidateView)} });

    // 통합 검색 화면에서 여러 경로로 검색 결과 상세로 이동 후
    // 경로 탐색을 하면 가젯검색모드를 해제한다.
    // 19.05.30 ymhong
    sendNotification(SearchConstants::SetGadgetSearchMode, false);

    return true;
}

void SearchResultDetailViewMediator::sendToVia(const QVariant &data)
{    
    QJSValue jsVal = data.value<QJSValue>();
    int x = jsVal.property( CommonConstants::X).toInt();
    int y = jsVal.property( CommonConstants::Y ).toInt();
    QString name = jsVal.property( CommonConstants::Name).toString();
    QString address = jsVal.property( CommonConstants::Address).toString();
    auto navSeq = jsVal.property(SearchConstants::NavSeq).toInt();
    auto rpFlag = jsVal.property(SearchConstants::RpFlag).toInt();
    auto poiId = jsVal.property(SearchConstants::PoiID).toInt();
    auto poleCate = jsVal.property(SearchConstants::OpiPoleCate).toInt();


    QVariantMap body;
    body.clear();
    body[CommonConstants::X] = x;
    body[CommonConstants::Y] = y;
    body[CommonConstants::Name] = name;
    body[CommonConstants::Address] = address;
    body[SearchConstants::NavSeq] = navSeq;
    body[SearchConstants::RpFlag] = rpFlag;
    body[SearchConstants::PoiID] = poiId;
    body[SearchConstants::OpiPoleCate] = poleCate;
    body[CommonConstants::Caller] = ViewRegistry::objectName(ViewRegistry::SearchResultDetailView);

    sendNotification(RouteConstants::AddVia, body);

    sendNotification(MapConstants::UpdateMapView);

    // 통합검색에서
    // 1. 최근 검색 > 검색 결과 > 상세에서 경유지 추가한 경우
    // 2. 최근 목적지 > 상세에서 경유지 추가한 경우
    // 3. 즐겨 찾기 > 상세에서 경유지 추가한 경우
    // 가젯 검색 모드를 취소한다.
    // 19.05.30 ymhong
    sendNotification(SearchConstants::SetGadgetSearchMode, false);
}

bool SearchResultDetailViewMediator::sendAddSearchMarker(const QVariant &data)
{
    QJSValue jsVal = data.value<QJSValue>();
    int index = jsVal.property( CommonConstants::Index ).toInt();
    int focusIndex = jsVal.property( SearchConstants::FocusIndex ).toInt();
    QString text = jsVal.property( CommonConstants::Text ).toString();
    int x = jsVal.property( CommonConstants::X).toInt();
    int y = jsVal.property( CommonConstants::Y ).toInt();
    QVariantMap body;
    body[CommonConstants::Index] = index;
    body[SearchConstants::FocusIndex] = focusIndex;
    body[CommonConstants::Text] = text;
    body[CommonConstants::X] = x;
    body[CommonConstants::Y] = y;
    sendNotification(RouteConstants::AddSearchMarker, body );
    return true;
}

bool SearchResultDetailViewMediator::sendRemoveSearchMarker(const QVariant &data)
{
    QJSValue jsVal = data.value<QJSValue>();
    int index = jsVal.property( CommonConstants::Index ).toInt();
    int x = jsVal.property( CommonConstants::X).toInt();
    int y = jsVal.property( CommonConstants::Y ).toInt();
    QVariantMap body;
    body[CommonConstants::Index] = index;
    body[CommonConstants::X] = x;
    body[CommonConstants::Y] = y;
    sendNotification(RouteConstants::RemoveSearchMarker, body );
    return true;
}

void SearchResultDetailViewMediator::showConfirmDialog(const QString &address)
{
    auto m0 = mDestData.toMap();
    int x = m0[CommonConstants::X].toInt();
    int y = m0[CommonConstants::Y].toInt();
    auto name = m0[CommonConstants::Name].toString();
    QVariantMap m;

    m[CommonConstants::Url] = ViewRegistry::url(ViewRegistry::ChangeDestinationView);
    m[SearchConstants::DestX] = x;
    m[SearchConstants::DestY] = y;
    m[CommonConstants::Address] = address;
    m[CommonConstants::Name] = name;
    m[SearchConstants::PoiID] = m0[SearchConstants::PoiID];
    m[SearchConstants::NavSeq] = m0[SearchConstants::NavSeq];
    m[SearchConstants::RpFlag] = m0[SearchConstants::RpFlag];
    m[SearchConstants::View] = QVariant::fromValue(mViewComponent);
    m[SearchConstants::OkCallback] = QStringLiteral("closeAllView");
    PlatformHelper::instance()->setParentForSystemPopup(m);
    sendNotification(Constants::CreateView, m);
}

void SearchResultDetailViewMediator::closeView(const QString &viewName)
{
    QVariantMap m;
    m[CommonConstants::ObjectName] = viewName;
    sendNotification(Constants::CloseView, m);
}

bool SearchResultDetailViewMediator::fromSearchResultView() const
{
    auto caller = QQmlProperty::read(mViewComponent, CommonConstants::Caller).toString();
    auto owner = QQmlProperty::read(mViewComponent, CommonConstants::Owner).toString();

    return caller == ViewRegistry::objectName(ViewRegistry::SearchTabView) &&
           owner == ViewRegistry::objectName(ViewRegistry::SearchResultView);
}

void SearchResultDetailViewMediator::requestAddressType()
{
    QVariantMap body;
    body[CommonConstants::Section] = SettingConstants::MapMain;
    body[CommonConstants::Key] = SettingConstants::AddressDisplayMethod;
    sendNotification( SettingConstants::RequestSetting, body );
}

bool SearchResultDetailViewMediator::isVisible() const
{
    auto visible = QQmlProperty::read(mViewComponent, CommonConstants::Visible).toBool();
    return visible;
}

bool SearchResultDetailViewMediator::isActive() const
{
    auto active = QQmlProperty::read(mViewComponent, CommonConstants::Active).toBool();
    return active;
}
