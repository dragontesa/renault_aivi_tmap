#include "SearchResultViewMediator.h"

#include <QQmlProperty>

#include "Constants.h"
#include "RgConstants.h"
#include "RouteConstants.h"
#include "CommonConstants.h"
#include "SearchConstants.h"
#include "SettingDto.h"
#include "SettingConstants.h"
#include "KeyboardConstants.h"
#include "MapConstants.h"
#include "ZoneConstants.h"
#include "NavibookmarkConstants.h"
#include <QJSValueIterator>
#include "StringHelper.h"
#include "../common/searchrequest.h"
#include "VSM_Map.h"
#include "mediator/app/MapViewMode.h"
#include "ViewRegistry.h"
#include "NavibookmarkProtocol.h"
#include "VSM_Navi.h"
#include "../common/PoiHelper.h"
#include "BookmarkHelper.h"
#include "OfflineJobCacheProxy.h"
#include "NavigationControllerConstants.h"
#include "RgConstants.h"
#include "UtilConstants.h"
#include "LangConstants.h"
#include "NavigationControllerProtocol.h"
#include "PlatformHelper.h"
#include "ApplicationConstants.h"

#include <QtConcurrent/QtConcurrent>
#include <QFuture>
#include <QFutureWatcher>

#include "VSM_Map.h"
#include "NavigationControllerProxy.h"

using namespace SKT;

int SearchResultViewMediator::getIndexByFuel(int fuel)
{
    int seq = 0;
    switch(fuel) {
        case  SettingConstants::OilTypeGasoline:
        seq = SearchConstants::ResultOPIGasolinePrice;
        break;
        case  SettingConstants::OilTypeDiesel:
        seq = SearchConstants::ResultOPIDieselPrice;
        break;
        case  SettingConstants::OilTypeLPG:
         seq = SearchConstants::ResultOPILPGPrice;
        break;
        case  SettingConstants::OilTypeElec:
        break;
        case  SettingConstants::OilTypePremiumGasoline:
        seq = SearchConstants::ResultOPIHiGasolinePrice;
        break;
    }
    return seq;
}

SearchResultViewMediator::SearchResultViewMediator( QObject* viewComponent )
    : AbstractViewMediator( viewComponent )
    , mIsRg(false)
    , mIsRoadName(false)
    , mSearchResultCount(0)
{
    setIndustryCodeMap();
}

SearchResultViewMediator::~SearchResultViewMediator()
{
}

QList<int> SearchResultViewMediator::listNotificationInterests()
{
    static QList<int> notes = {
        SearchConstants::ResponseSearch,
        SearchConstants::ResponseCacheSearch,
        ZoneConstants::FocusActivatedZone1TextInput,
        ZoneConstants::DoSoftKeyFunction,
        ZoneConstants::DoSoftkeyIndexMenuFunction,
        RouteConstants::HitSearchMarker,
        SearchConstants::UpdateView,
        MapConstants::ResponseDriveMode,
        SettingConstants::ApplySetting,
        SettingConstants::ChangedSetting,
        RgConstants::ReplyToRouteGuidanceState,
        RgConstants::ChangedRouteGuidanceState,
        RgConstants::ResponseGetGasStationInfo,
        RgConstants::ResponseGetEStationInfo,
        NavigationControllerConstants::ResponseOnlineStatus,
        NavigationControllerConstants::ChangeOnlineStatus,
        SettingConstants::DialogAccepted,
        SettingConstants::DialogCanceled,
        SearchConstants::QueueChanged,
        NavigationControllerConstants::ChangeLanguage,
        NavigationControllerConstants::ResponseController,
        SettingConstants::SavedSettingFromObjectName,
        ApplicationConstants::ReplyToViewList,
        SearchConstants::RequestRemoveMarker,
        SearchConstants::ResponseSort,
        SearchConstants::ChangedCurrentStreet,
        SearchConstants::CloseSearchResultView,
        SearchConstants::UpdateSearchResultText,
        ApplicationConstants::ReplyToGadgetState,
        ApplicationConstants::ChangedGadgetState,
        ApplicationConstants::UpdatedUserProfile,
        ApplicationConstants::ChangedUserProfile,
    };
    return notes;
}

bool SearchResultViewMediator::onEvent( int event, const QVariant& data )
{
    switch ( event ) {
        case Activated: {
            sendNotification(MapConstants::ChangeMapViewMode,QVariantMap{{CommonConstants::Mode,MapViewMode::MapInquiry}});
            sendNotification(MapConstants::ShowCarvata, false);
            sendNotification(MapConstants::EnableCallout, false);

            sendNotification(ZoneConstants::RegistSoftkey, ViewRegistry::objectName(ViewRegistry::SearchResultView));

            changeMenu(data);

            sendNotification(MapConstants::EnableCallout, false);
            sendNotification(RgConstants::BlockReroute, true);

            sendNotification(ApplicationConstants::RequestGadgetState);
        }
        break;
        case ChangeMenu:
            changeMenu(data);
            break;
        case SelectSoftkeyIndexMenuMode:
            sendNotification(ZoneConstants::SelectSoftkeyIndexMenuMode, data);
            break;
        case RemoveSearchQueue: {
                  QVariantMap body;
                  body.insert( SearchConstants::SearchType, SearchRequestBase::RemoveSearchQueue);
                  sendNotification(SearchConstants::RequestSearch, body);
              }
              break;
        case DeActivated:
              sendNotification(MapConstants::EnableCallout, true);
              break;
        case CreateView:
            {
                auto m = data.toMap();
                auto url = m[CommonConstants::Url].toString();
                if (url == ViewRegistry::url(ViewRegistry::CommonDialogView)) {
                    PlatformHelper::instance()->setParentForSystemPopup(m);
                }
                sendNotification(Constants::CreateView, m);
            }
            break;
        case CloseView:
            {                
                sendNotification(Constants::CloseView, data);
            }
            break;
        case ShowView:
            {
                auto caller = QQmlProperty::read(mViewComponent, "caller").toString();
                auto owner = QQmlProperty::read(mViewComponent, "owner").toString();
                if (owner == ViewRegistry::objectName(ViewRegistry::TextSearchView)) {
                    auto isBookmark = caller == ViewRegistry::objectName(ViewRegistry::BookmarkPageView);
                    auto isVia = caller == ViewRegistry::objectName(ViewRegistry::RouteSettingView);
                    if (isBookmark || isVia) {
                        sendNotification(SearchConstants::UpdateTextSearchZone1, true);
                    }
                }
                sendNotification(Constants::ShowView, data);
            }
            break;
        case HideView:
            sendNotification(Constants::HideView, data);
            break;
        case ShowKeyboard:
            sendNotification(KeyboardConstants::ShowKeyboard, data);
            break;
        case HideKeyboard:
            sendNotification(KeyboardConstants::HideKeyboard, data);
            break;
        case SelectZone1Title:
            sendNotification(ZoneConstants::SelectZone1Title, data);
            break;
        case SelectZone1TextInput: {
                // 타이틀 표시하는 경우에는 Zone1에 텍스트 입력 표시 요청은 무시한다.
                QJSValue jsVal = data.value<QJSValue>();
                if (jsVal.hasOwnProperty(CommonConstants::Text)){
                    QString text = jsVal.property(CommonConstants::Text).toString();
                    if (text.length() > 0) {

#if !defined(BUILD_TARGET)
                        QVariantMap body;
                        body.insert(CommonConstants::Text, text);
                        sendNotification(ZoneConstants::SelectZone1TextInput, body);
#else
                        qDebug() << "[ZONE1] SearchResultViewMediator SelectZone1TextInput. setEditFieldText: " << mUpdateSearchResultText;

                        if (mUpdateSearchResultText) {
                            qDebug() << "[KEYBOARD] SearchResultViewMediator SelectZone1TextInput. " << text;
                            PlatformHelper::instance()->setEditFieldText(text);
                        }
                        mUpdateSearchResultText = false;
#endif
                    }
                }
            }
            break;
        case FocusZone1TextInput:
            sendNotification(ZoneConstants::FocusZone1TextInput);
            break;
        case RequestSearch: {
                QJSValue jsVal = data.value<QJSValue>();
                QString query = jsVal.property( SearchConstants::Query ).toString();
                QString textParam = jsVal.property( SearchConstants::TextParam ).toString();
                int order = jsVal.property( SearchConstants::Order ).toInt();
                QString searchType  = jsVal.property(SearchConstants::SearchType).toString();
                QVariant param = jsVal.property(SearchConstants::Params).toVariant();
                QVariantMap params = param.value<QVariantMap>();
                int kind = params[SearchConstants::Kind].toInt();
                QString addressType = jsVal.property(SearchConstants::AddressType).toString();
                QString responseSignal = jsVal.property( SearchConstants::Response ).toString();

                QVariantMap body;
                if (order == DistanceOrder) {
                    body.insert( SearchConstants::SortBy, SearchConstants::DistanceOrder );
                }  else if (order == AlphabeticalOrder) {
                    body.insert( SearchConstants::SortBy, SearchConstants::AlphabeticalOrder);
                } else if (order == PriceOrder) {
                     body.insert( SearchConstants::SortBy, SearchConstants::PriceOrder );
                } else if (order == NameOrder) {
                    body.insert( SearchConstants::SortBy, SearchConstants::NameOrder );
                }

                if (searchType == CommonConstants::Address) {
                    if ( kind == SearchConstants::CategorySearchAddressVr ) {
                        body.insert( SearchConstants::SearchType, SearchRequestBase::SearchByAddressVr );
                    }
                    else {
                        QString text = jsVal.property(CommonConstants::Text).toString();
                        QString keyword = jsVal.property(CommonConstants::Keyword).toString();
                        bool isSan = jsVal.property(SearchConstants::IsSan).toBool();
                        bool isApt = jsVal.property(SearchConstants::IsApt).toBool();
                        bool isRoad = jsVal.property(SearchConstants::IsRoad).toBool();
                        text = StringHelper::instance()->normalized(text);
                        keyword = StringHelper::instance()->normalized(keyword);
                        body.insert( SearchConstants::SearchType, SearchRequestBase::SearchByAddress );
                        body.insert( SearchConstants::IsSan,  isSan);
                        body.insert( SearchConstants::IsApt,  isApt);
                        body.insert( CommonConstants::Text,  text);
                        body.insert( CommonConstants::Keyword,  keyword);
                        body.insert( SearchConstants::IsRoad, isRoad );
                    }
                } else if (searchType == SearchConstants::Industry) {
                    body.insert( SearchConstants::SearchType, SearchRequestBase::SearchBySisulCategory );
                    body.insert( SearchConstants::TextParam, textParam);
                    body.insert( SearchConstants::Query, query);
                } else if (searchType == SearchConstants::Around) {
                    bool sync = false;
                    if (params.contains(SearchConstants::Sync)) {
                        sync = params[SearchConstants::Sync].toBool();
                    }
                    if (sync) {
                        switch(kind) {
                            case SearchConstants::CategoryChargingEvSt:
                                body.insert( SearchConstants::SearchType, SearchRequestBase::SearchByEvAround);
                                break;
                            default:
                                body.insert( SearchConstants::SearchType, SearchRequestBase::SearchBySisulAround);
                                break;
                        }
                    } else {
                        body.insert( SearchConstants::SearchType, SearchRequestBase::SearchBySisulAround);
                    }
                    body.insert( SearchConstants::TextParam, textParam);
                    body.insert( SearchConstants::Query, query);
                } else if (searchType == SearchConstants::Lowest) {
                    body.insert( SearchConstants::SearchType, SearchRequestBase::SearchLowestOilSt);
                    body.insert( SearchConstants::TextParam, textParam);
                    body.insert( SearchConstants::Query, query);
                } else if (searchType == SearchConstants::Code) {
                    body.insert( SearchConstants::SearchType, SearchRequestBase::SearchNextCategory);
                    body.insert( SearchConstants::TextParam, textParam);
                    body.insert( SearchConstants::Query, query);
                } else {
                    body.insert( SearchConstants::SearchType, SearchRequestBase::SearchBySisulName );
                    query = StringHelper::instance()->normalized(query);
                    body.insert( SearchConstants::TextParam, query);
                    body.insert( SearchConstants::Query, query);
                }
                if (addressType == SearchConstants::NewAddrType) {
                    body.insert( SearchConstants::RoadSearch, true );
                } else {
                    body.insert( SearchConstants::RoadSearch, false );
                }
                body.insert( SearchConstants::Response, responseSignal);
                body.insert( SearchConstants::Params, QVariant::fromValue(params));
                sendNotification(SearchConstants::RequestSearch, body);
            }
            break;
        case RequestAtThePOI: {
                QJSValue jsVal = data.value<QJSValue>();
                auto mapView = jsVal.property( SearchConstants::MapView  ).toVariant().value<QObject*>();
                auto mapViewHandle = qvariant_cast<void*>(mapView->property("nativeHandle"));

                qDebug() << "[Perf][RequestAtThePOI]";

                int x = jsVal.property( CommonConstants::X).toInt();
                int y = jsVal.property( CommonConstants::Y ).toInt();
                VSMMap_MapMoveTo(mapViewHandle, x,  y, true);
            }
            break;
        case RequestAddVia: {
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

                sendNotification(RouteConstants::AddVia, body);

                sendNotification(ZoneConstants::UpdateSearchText, "");
            }
            break;
        case ViewSizeChanged: {
                QJSValue jsVal = data.value<QJSValue>();
                int bottom = jsVal.property( CommonConstants::B ).toInt();
                QVariantMap body;
                body.insert(SearchConstants::B, bottom);
                sendNotification(MapConstants::UpdateMapViewPadding, body);
            }
            break;
        case RequestAddMarker:
            sendAddSearchMarker(data);
            break;
        case RequestRemoveMarker:
            sendRemoveSearchMarker(data);
            break;
        case UpdateView:
            sendNotification(SearchConstants::UpdateView, data);
            break;
        case ModalChanged:
            sendNotification(ZoneConstants::UpdateModal,data.toMap());
            break;
        case ResetCsw:
            sendNotification(NavigationControllerConstants::ResetCSW);
            break;
        case RegistCsw:
            sendNotification(NavigationControllerConstants::RegistCSW, data);
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
        case SaveSetting: {
                QJSValue jsVal = data.value<QJSValue>();
                QString section = jsVal.property( CommonConstants::Section ).toString();
                auto setting = jsVal.property( CommonConstants::Settings );

                QList<SettingDtoPtr> settings;
                QJSValueIterator it(setting);
                while (it.hasNext()) {
                    it.next();
                    QString key = it.name();
                    QJSValue value = it.value();
                    auto dto = t_new_shared<SettingDto>( value.property(CommonConstants::Type).toInt(),
                        section,
                        value.property(CommonConstants::Key).toString(),
                        value.property(CommonConstants::Value).toString()
                    );
                    settings.append( dto );
                }

                QVariantMap body;
                body.insert( CommonConstants::Section, section );
                body.insert( CommonConstants::Settings, QVariant::fromValue( settings ) );

                sendNotification( SettingConstants::SaveSetting, body );
            }
            break;
        case RequestAddressType: {
                QVariantMap body;
                body[CommonConstants::Section] = SettingConstants::MapMain;
                body[CommonConstants::Key] = SettingConstants::AddressDisplayMethod;
                sendNotification( SettingConstants::RequestSetting, body );
            }
            break;
        case RequestGetGasStationInfo: {
                sendNotification(RgConstants::RequestGetGasStationInfo);
            }
            break;
        case RequestGetEStationInfo: {
                sendNotification(RgConstants::RequestGetEStationInfo, data);
            }
            break;
        case RequestDetailInfo: {
                QJSValue jsVal = data.value<QJSValue>();
                QVariant param = jsVal.property(SearchConstants::Params).toVariant();
                QVariantMap params = param.value<QVariantMap>();
                QVariantMap body;
                body.insert( SearchConstants::SearchType, SearchRequestBase::SearchByPOIid);
                body.insert( SearchConstants::Params, QVariant::fromValue(params));
                sendNotification(SearchConstants::RequestSearch, body);
            }
            break;
        case RequestEvDetailInfo: {
                QJSValue jsVal = data.value<QJSValue>();
                QVariant param = jsVal.property(SearchConstants::Params).toVariant();
                QVariantMap params = param.value<QVariantMap>();
                QVariantMap body;
                body.insert( SearchConstants::SearchType, SearchRequestBase::SearchByEvPOIid);
                body.insert( SearchConstants::Params, QVariant::fromValue(params));
                sendNotification(SearchConstants::RequestSearch, body);
            }
            break;
        case RequestRouteGuidanceState:
            sendNotification(RgConstants::RequestRouteGuidanceState, getMediatorId());
            break;
        case NoResult:
            {
                auto m = data.toMap();
                auto query = QQmlProperty::read(mViewComponent, SearchConstants::Query).toString();
                m[SearchConstants::Query] = query;

                sendNotification(SearchConstants::ResponseNoResult,m);
            }
            break;
        case QueryOnlineStatus:
            sendNotification(NavigationControllerConstants::QueryOnlineStatus);
            break;
        case ShowToast:
            sendNotification(ToastsConstants::ShowToast, data);
            break;
        case ShowProgressIndicator:
            sendNotification( UtilConstants::ShowProgressIndicator );
            sendNotification(SearchConstants::ResponseLockStatus, true);
            break;

        case HideProgressIndicator:
            sendNotification( UtilConstants::HideProgressIndicator );
            sendNotification(SearchConstants::ResponseLockStatus, false);
            break;

        case RequestSort: {
                QVariantMap dataMap = data.value<QVariantMap>();
                QFuture<QVariantMap> future = QtConcurrent::run( this, &SearchResultViewMediator::runSort, dataMap);
                mWatcher = new QFutureWatcher<QVariantMap>();
                connect(mWatcher, &QFutureWatcher<QVariantMap>::finished, this,&SearchResultViewMediator::finishedSort);
                mWatcher->setFuture( future );
            }
            break;

    case RequestLang: {
            QVariantMap body;
            body.insert(CommonConstants::Type, NavigationControllerProtocol::GetLang );
            sendNotification( NavigationControllerConstants::RequestController, body );
        }
        break;

    case RequestViewList: {
            QVariantMap body;
            body.insert( CommonConstants::ObjectName,   ViewRegistry::objectName(ViewRegistry::SearchResultView));
            sendNotification(ApplicationConstants::RequestViewList, body);
        }
        break;
   case UpdateSearchResultCount: {
            QJSValue jsVal = data.value<QJSValue>();
            QString search = jsVal.property( CommonConstants::Text ).toString();
            int totalCount = jsVal.property( CommonConstants::Count ).toInt();
            updateSearchResultCount(search, totalCount);
        }
        break;
    case RequestCacheMerge: {
            QJSValue jsVal = data.value<QJSValue>();
            bool lastPage = jsVal.property( CommonConstants::L ).toBool();
            int startIndex = jsVal.property( CommonConstants::Index ).toInt();
            int totalCount = jsVal.property( CommonConstants::Count ).toInt();
            QVariantList result   = jsVal.property(CommonConstants::Result).toVariant().toList();
            QVariantList resultCache  = jsVal.property(CommonConstants::Value).toVariant().toList();

            int i= 0;
            int j = 0;
            int x = 0;
            int y = 0;
            int addCount = 0;
            int resultLen = 0;
            int cacheLen = 0;
            QString key;
            QVariantList listCache;
            QVariantList listMerge;
            QVariantList listSort;
            QVariantList sortResult;
            QVariantMap  mapResult;
            QVariantMap  mapMerge;
            resultLen = result.length();
            cacheLen = resultCache.length();
            //1.캐시결과-거리값 수정
            for(i = 0; i<cacheLen; i++  ) {
                listCache = resultCache[i].toList();
                listCache[SearchConstants::ResultIndex] = i;
                x = listCache[SearchConstants::ResultX].toInt();
                y = listCache[SearchConstants::ResultY].toInt();
                listCache[SearchConstants::ResultDistance] = getDistance(x,y);
                listCache[SearchConstants::ResultType] = SearchConstants::ResultTypeCache;
                key = listCache[SearchConstants::ResultPOIID].toString();
                mapMerge[key]  = listCache;
            }
            //2.검색결과
            for (i = 0; i< resultLen; i++ ) {
                listMerge = result[i].toList();
                listMerge[SearchConstants::ResultIndex] = resultCache.length() + i;
                key = listMerge[SearchConstants::ResultPOIID].toString();
                if (mapMerge.contains(key)) {
                    continue;
                }
                mapMerge[key] = listMerge;
            }
            //3.검색결과 및 캐시결과 다시 생성
            result.clear();
            resultCache.clear();
            for(QVariantMap::const_iterator iter = mapMerge.begin(); iter != mapMerge.end(); ++iter) {
                listMerge = iter.value().toList();
                sortResult.append(QVariant::fromValue(listMerge));
            }
            //4.정렬 적용
            int order = QQmlProperty::read(mViewComponent, SearchConstants::Order).toInt();
            QVariantMap data;
            data[CommonConstants::Key] = order;
            data[CommonConstants::Value] = sortResult;
            data = runSort(data);
            sortResult = data[CommonConstants::Value].toList();

            //5.검색결과 및 캐시결과 새로 생성
            result.clear();
            resultCache.clear();
            int resultLastIndex = sortResult.length();
            if (order == DistanceOrder) {
                for(i = sortResult.length()-1; i >= 0; i--  ) {
                    listSort = sortResult[i].toList();
                    if(listSort[SearchConstants::ResultType].toInt() != SearchConstants::ResultTypeCache) {
                        resultLastIndex = i;
                        break;
                    }
                }
            }
            for(i = 0; i<sortResult.length(); i++  ) {
                listSort = sortResult[i].toList();
                listSort[SearchConstants::ResultIndex] = i;
                x = listSort[SearchConstants::ResultX].toInt();
                y = listSort[SearchConstants::ResultY].toInt();
                key = listSort[SearchConstants::ResultPOIID].toString();
                if (lastPage || (resultLastIndex > i &&
                                 result.length() < SearchConstants::RequestCount) ) {
                    result.append(QVariant::fromValue(listSort));
                } else {
                    resultCache.append(QVariant::fromValue(listSort));
                }
            }
            //6.검색결과 갯수를 맞춤
            int resultAddLen = resultLen - result.length();
            if ( resultAddLen> 0) {
                QVariantList empty;
                SearchConstants::resultInit(empty);
                int currResultLen = result.length();
                for (i=0; i<resultAddLen; i++) {
                    empty[SearchConstants::ResultIndex] = currResultLen++;
                    listMerge = empty;
                    result.append(QVariant::fromValue(listMerge));
                }
            }
            QMetaObject::invokeMethod(mViewComponent, "__onResultCacheMerge",
                                                      Q_ARG(QVariant, QVariant::fromValue(result)),
                                                      Q_ARG(QVariant, QVariant::fromValue(resultCache)),
                                                      Q_ARG(QVariant, QVariant::fromValue(startIndex)),
                                                      Q_ARG(QVariant, QVariant::fromValue(totalCount)),
                                                      Q_ARG(QVariant, QVariant::fromValue(addCount)));

        }
        break;

    case QueryInDriveMode:
        mDestData = data;
        queryInDriveMode();
        break;
        default:
            return false;
    }
    return true;
}

void SearchResultViewMediator::responsedFreeTextSearchResult(const QVariantMap &data, const QVariantList &values)
{
    QVariantMap reqData = data[SearchConstants::ReqData].value<QVariantMap>();
    if ( reqData.contains(SearchConstants::Response) ) {
        QString responseSignal = reqData[SearchConstants::Response].toString();
        if (!responseSignal.isEmpty()) {
            QVariantList resultValues;
            QVariantList resultValue;
            int distance;
            for(int i=0; i<values.count(); i++) {
                resultValue = values[i].toList();
                distance = resultValue[SearchConstants::ResultDistance].toInt();
                resultValue[SearchConstants::ResultDistance] = getDistanceText(distance);
                resultValues.append(QVariant::fromValue(resultValue));
            }
            QVariantMap body;
            body.insert( CommonConstants::Type, NavigationControllerProtocol::ResponsedFreeTextSearchResult);
            body.insert( SearchConstants::Request, reqData );
            body.insert( SearchConstants::Result, resultValues );
            sendNotification( NavigationControllerConstants::ApplyController, body );
        }
    }
}


bool SearchResultViewMediator::handleNotification( const mvc::INotificationPtr& note )
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
    case SearchConstants::ChangedCurrentStreet:
    case SearchConstants::CloseSearchResultView:
    case SearchConstants::UpdateSearchResultText:
    case ApplicationConstants::UpdatedUserProfile:
    case ApplicationConstants::ChangedUserProfile:
        break;
    default: {
            if (QQmlProperty::read(mViewComponent, CommonConstants::IsClosed).toBool()) {
                switch ( note->getId() ) {
                    case SearchConstants::ResponseSearch:
                    case SearchConstants::ResponseCacheSearch:{
                            QVariantMap data = note->getBody().value<QVariantMap>();
                            QVariantMap reqData = data[SearchConstants::ReqData].value<QVariantMap>();
                            auto owner = reinterpret_cast<void*>(reqData[CommonConstants::Owner].toULongLong());
                            if (owner != this) {
                                return false;
                            }
                            sendNotification( UtilConstants::HideProgressIndicator );
                        }
                        break;
                    case RgConstants::ResponseGetGasStationInfo:
                    case RgConstants::ResponseGetEStationInfo:
                        sendNotification( UtilConstants::HideProgressIndicator );
                        break;
                }
                return false;
            }
        }
        break;
    }

    switch ( note->getId() ) {
    case SearchConstants::UpdateSearchResultText:
        // SearchResultDetailView 에서 SearchResultView로 이행할 때
        // Zone1의 TextBox에 setEditFieldText()가 호출 될 수 있도록 한다.
        mUpdateSearchResultText = true;
        break;

    case SearchConstants::ResponseSearch: {
            QVariantMap data = note->getBody().value<QVariantMap>();
            if (!data.contains(SearchConstants::Result)) {
                return false;
            }
            if (!data.contains(SearchConstants::ReqData)) {
                return false;
            }

            QVariantMap reqData = data[SearchConstants::ReqData].value<QVariantMap>();
            auto owner = reinterpret_cast<void*>(reqData[CommonConstants::Owner].toULongLong());
            if (owner != this) {
                return false;
            }

            int searchType = reqData[SearchConstants::SearchType].value<int>();
            if (searchType == SearchRequestBase::SearchLowestOilSt) {
              return invokeResultLowest(data);
            } else if (searchType == SearchRequestBase::SearchByPOIid) {
              return invokeResultDetailInfo(data);
            } else if (searchType == SearchRequestBase::SearchByEvPOIid) {
              return invokeResultEvDetailInfo(data);
            } else {
                if (!invokeResult(data))
                    return false;
                QObject* res = data[SearchConstants::Result].value<QObject*>();
                SearchReplyBaseInterface* reply = qobject_cast<SearchReplyBaseInterface*>(res);
                // Navibookmark Select Search and Address type
                QVariantMap reqData = reply->request()->extra();
                int searchType = reqData[SearchConstants::SearchType].toInt();
                if (searchType != SearchRequestBase::SearchBySisulAround &&
                    searchType != SearchRequestBase::SearchByEvAround    &&
                    searchType != SearchRequestBase::SearchBySisulCategory) {
                    // Start Navibookmark Agent Session
                    QVariantMap body1;
                    body1.insert(CommonConstants::Type,(int)NavibookmarkProtocol::TransactionSearch); // transaction type
                    sendNotification(NavibookmarkConstants::NewNavibookSession, body1);

                    body1.clear();
                    if (searchType == SearchRequestBase::SearchByAddress ) body1.insert(SearchConstants::SearchType,NavibookmarkProtocol::AddressSearch); // search type
                    else if ( searchType == SearchRequestBase::SearchBySisulName ) body1.insert(SearchConstants::SearchType,NavibookmarkProtocol::SisulNameSearch);
                    else body1.insert(SearchConstants::SearchType,NavibookmarkProtocol::TextSearch);

                    if (reqData.contains(SearchConstants::Query)) { // case in general text search
                        body1.insert(CommonConstants::Keyword,reqData[SearchConstants::Query]);
                    } else if (reqData.contains(CommonConstants::Keyword)) { // case in address search
                        body1.insert(CommonConstants::Keyword,reqData[CommonConstants::Keyword]);
                    }
                    sendNotification(NavibookmarkConstants::SelectNavibookSearch,body1);

                    // regist recent search
                    QVariantMap body2;
                    body2.insert(CommonConstants::BookmarkType, (int)NavibookmarkProtocol::BookmarkRecentSearch);
                    sendNotification(NavibookmarkConstants::RegistNavibookBookmark,body2);
                }

                T_DEBUGV("search result --> %s\t%s",
                        reqData[SearchConstants::TextParam].toString().toLocal8Bit().constData(),
                        reqData[SearchConstants::Query].toString().toLocal8Bit().constData());
            }
        }
        break;
    case SearchConstants::ResponseCacheSearch: {
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

            SearchCacheInfoListPtr list = data[SearchConstants::Result].value<SearchCacheInfoListPtr>();
            QVariantList values = QVariantList{};
            if(list) {
                for (int i=0 ; i < list->size(); i++) {
                    TPOI_SEARCH_LIST listitem = list->at(i);
                    auto navSeq = QString(listitem.szNavseq);

                    QVariantList result;
                    SearchConstants::resultInit(result);
                    result[SearchConstants::ResultIndex               ] = listitem.nIdx;
                    result[SearchConstants::ResultName                ] = (const char*)listitem.szName;
                    result[SearchConstants::ResultStdName             ] = (const char*)listitem.szStdName;
                    result[SearchConstants::ResultAddress             ] = (const char*)listitem.szAddrName;
                    result[SearchConstants::ResultTelNum              ] = (const char*)listitem.szPhoneNumber;
                    result[SearchConstants::ResultPOIID               ] = listitem.nPoiID;
                    result[SearchConstants::ResultAddrCode            ] = listitem.nAddrCode;
                    result[SearchConstants::ResultRpFlag              ] = listitem.byRPFlag;
                    result[SearchConstants::ResultDistance            ] = listitem.nDist;
                    result[SearchConstants::ResultDeg                 ] = listitem.nDeg;
                    result[SearchConstants::ResultKind                ] = listitem.nKind;
                    result[SearchConstants::ResultX                   ] = listitem.tp.x;
                    result[SearchConstants::ResultY                   ] = listitem.tp.y;
                    result[SearchConstants::ResultOPIPoleCate         ] = listitem.nOPIPoleCate;
                    result[SearchConstants::ResultOPIPrice            ] = listitem.nOPIPrice;
                    result[SearchConstants::ResultYear                ] = listitem.sGatherTime.nYear;
                    result[SearchConstants::ResultMonth               ] = listitem.sGatherTime.nMonth;
                    result[SearchConstants::ResultDay                 ] = listitem.sGatherTime.nDay;
                    result[SearchConstants::ResultHour                ] = listitem.sGatherTime.nHour;
                    result[SearchConstants::ResultMinute              ] = listitem.sGatherTime.nMinute;
                    result[SearchConstants::ResultSecond              ] = listitem.sGatherTime.nSecond;
                    result[SearchConstants::ResultWeek                ] = listitem.sGatherTime.nWeek;
                    result[SearchConstants::ResultOilCompany          ] = (const char*)listitem.szOilCompany;
                    result[SearchConstants::ResultHighGasoline        ] = listitem.bHighGasoline;
                    result[SearchConstants::ResultLowestOilSt         ] = listitem.bLowestOilSt;
                    result[SearchConstants::ResultOPIGasolinePrice    ] = listitem.nOPIGasolinePrice;
                    result[SearchConstants::ResultOPIDieselPrice      ] = listitem.nOPIDieselPrice;
                    result[SearchConstants::ResultOPILPGPrice         ] = listitem.nOPILPGPrice;
                    result[SearchConstants::ResultOPIHiGasolinePrice  ] = listitem.nOPIHiGasolinePrice;
                    result[SearchConstants::ResultOPIHiLPGPrice       ] = listitem.nOPIHiLPGPrice;
                    result[SearchConstants::ResultNavseq              ] = navSeq;
                    result[SearchConstants::ResultType                ] = SearchConstants::ResultTypeCache;
                    values.append(QVariant::fromValue(result));
                }
            }


            QMetaObject::invokeMethod(mViewComponent, "__onResultCache",
                                                      Q_ARG(QVariant, QVariant::fromValue(search)),
                                                      Q_ARG(QVariant, QVariant::fromValue(values)),
                                                      Q_ARG(QVariant, QVariant::fromValue(totalCount)),
                                                      Q_ARG(QVariant, QVariant::fromValue(startIndex)) );
         }
         break;
    case ZoneConstants::FocusActivatedZone1TextInput:
        QMetaObject::invokeMethod(mViewComponent, "__focusActivatedZone1TextInput");
        break;
    case ZoneConstants::DoSoftKeyFunction:
    case ZoneConstants::DoSoftkeyIndexMenuFunction: {
            QVariantMap data = note->getBody().value<QVariantMap>();
            QString objectName = data[CommonConstants::Name].toString();
            if (objectName != ViewRegistry::objectName(ViewRegistry::SearchResultView)) {
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

            // close()가 호출 되면 caller가 초기화 되기 때문에 미리 property값을 읽어둔다.
            auto caller = QQmlProperty::read(mViewComponent, CommonConstants::Caller).toString();

            QMetaObject::invokeMethod(mViewComponent, "__doSoftKeyFunction", Q_ARG(QVariant, QVariant::fromValue(objFunction)), Q_ARG(QVariant, QVariant::fromValue(objMenu)), Q_ARG(QVariant, QVariant::fromValue(objIndex)) );

            if (objFunction == ZoneConstants::Back) {
                auto fromGadget = caller == CommonConstants::System;

                if (fromGadget) {
                    sendNotification(NavigationControllerConstants::ReserveSystemSwitchJob, QVariantMap{
                                         {CommonConstants::Command, SearchConstants::CloseSearchResultView}
                                     });
                } else {
                    QMetaObject::invokeMethod(mViewComponent, ("close"));
                }

                auto viewHandle = QQmlProperty::read(mViewComponent, "viewHandle");
                if (!viewHandle.isNull()) {
                    sendNotification(MapConstants::ResetMapPosition, viewHandle);
                }

                // TMCRSM-3164
                // Parking, Patrol gadget을 터치하여 들어온 경우
                // back키를 누르면 다시 시스템으로 복귀한다.
                // 19.05.30 ymhong
                if (fromGadget) {
                    PlatformHelper::instance()->switchToSystem();
                }
            }
        }
        break;
    case RouteConstants::HitSearchMarker: {
           QVariantMap hitTestResult = note->getBody().value<QVariantMap>();
           QMetaObject::invokeMethod(mViewComponent, "__onObjectHitting", Q_ARG(QVariant, QVariant::fromValue(hitTestResult)) );
        }
        break;
    case SearchConstants::UpdateView: {
            qDebug() << "[Perf][UpdateView]before";
            QVariantMap data = note->getBody().value<QVariantMap>();
            QString objectName = data[CommonConstants::ObjectName].toString();
            if (objectName != ViewRegistry::objectName(ViewRegistry::SearchResultView))
                return false;
            qDebug() << "[Perf][UpdateView]objectName: " << objectName;
            qDebug() << "[Perf][UpdateView]after";
            for ( auto it = data.constBegin(); it != data.constEnd(); it++ ){
                if (CommonConstants::Url == it.key() ||
                    CommonConstants::ObjectName == it.key() ||
                    CommonConstants::ReFocus == it.key() ||
                    CommonConstants::Visible == it.key() ||
                    SearchConstants::Params == it.key()) {
                    continue;
                }
                QQmlProperty::write( mViewComponent, it.key(), QVariant::fromValue( it.value() ) );
            }
            if (data.contains(CommonConstants::Visible)) {
                QQmlProperty::write( mViewComponent, CommonConstants::Visible, QVariant::fromValue( data[CommonConstants::Visible]) );
            }
            if (data.contains(SearchConstants::Params)) {
                QQmlProperty::write( mViewComponent, SearchConstants::Params, QVariant::fromValue( data[SearchConstants::Params]) );
            }
            if (data.contains(CommonConstants::ReFocus)) {
                QQmlProperty::write( mViewComponent, CommonConstants::ReFocus, QVariant::fromValue( data[CommonConstants::ReFocus]) );
            }
        }
        break;
    case MapConstants::ResponseDriveMode: {
            int driveMode = note->getBody().toInt();
            bool isRg = driveMode == MapViewMode::Navi;
            if (isVisible() && mDestData.isValid()) {
                QVariantMap m = mDestData.toMap();
                bool isFavorite = m[CommonConstants::Favorite].toBool();

                if (isFavorite) {
                    //closeAllView();
                    auto caller = QQmlProperty::read(mViewComponent, CommonConstants::Caller).toString();
                    if (caller == ViewRegistry::objectName(ViewRegistry::NaviMenuView)) {
                        QVariantMap m;
                        m[CommonConstants::Url] = ViewRegistry::url(ViewRegistry::NaviMenuView);
                        sendNotification(Constants::CreateView, m);
                    } else {
                        sendNotification(SearchConstants::ReturnToFavorite);

                        // 통합 검색을 통해 들어온 경우 입력 검색어 초기화
                        sendNotification(ZoneConstants::UpdateSearchText, "");
                    }
                    closeAllViewWithoutCaller(caller);
                    // TMCRSM-1869.
                    // 우분투에서 즐겨찾기 최대갯수 초과시 팝업이 뒤에 표시 되는 문제 수정.
                    // 원래는 [1]에 있었음.
                    bool isHome = m[SearchConstants::IsHome].toBool();
                    bool isOffice = m[SearchConstants::IsOffice].toBool();
                    BookmarkHelper::setAddFavoriteDestType(isHome, isOffice, m);
                    sendNotification(SearchConstants::AddFavorite, m);
                } else {
                    if (isRg) {
                        auto address = m[CommonConstants::Address].toString();
                        showConfirmDialog(address);
                    } else {
                        sendByDestination(mDestData);
                        closeAllView();

                        // 경로 탐색 시 검색어 초기화
                        // 19.10.22 ymhong
                        sendNotification(ZoneConstants::UpdateSearchText, "");
                    }
                }
            }
            mDestData.clear();
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
    case SettingConstants::ChangedSetting: {
            QVariantMap data = note->getBody().value<QVariantMap>();
            QString sect = data[CommonConstants::Section].toString();
            QString key = data[CommonConstants::Key].toString();
            if (sect == SettingConstants::MapMain &&
                key == SettingConstants::AddressDisplayMethod ) {
                int method = data[CommonConstants::Setting].value<SettingDtoPtr>()->value().toInt();
                mIsRoadName = (method == SettingConstants::AddressDisplayMethodRoadName) ? true : false;
                QQmlProperty::write(mViewComponent, SearchConstants::AddressType,
                                    (method == SettingConstants::AddressDisplayMethodLandNumber ) ?
                                     SearchConstants::OldAddrType : SearchConstants::NewAddrType );
            }
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
    case RgConstants::ResponseGetGasStationInfo: {
            int totalCount = 0;
            int startIndex = 0;
            QVariantMap m = note->getBody().toMap();
            QVariantList items = m["info"].toList();
            QVariantList values = QVariantList{};
            for (int i=0; i<items.count(); i++) {
               QVariantList result = items.at(i).toList();
               bool isOnline = result[SearchConstants::ResultOnline].toBool();
               QString navSeq = isOnline ? PoiHelper::navSeqFromServer("0")
                                     : PoiHelper::navSeqFromLocal(result[SearchConstants::ResultPOIID].toInt());
               int poiId = isOnline ? PoiHelper::fromServer(result[SearchConstants::ResultPOIID].toInt(), navSeq.toInt())
                                     : PoiHelper::fromLocal(result[SearchConstants::ResultPOIID].toInt());
               result[SearchConstants::ResultPOIID ] = poiId;
               result[SearchConstants::ResultNavseq] = navSeq;
               result[SearchConstants::ResultOnline] = isOnline;
               values.append(QVariant::fromValue(result));
            }
            totalCount = values.count();
            QMetaObject::invokeMethod(mViewComponent, "__onResultAlongRoute",
                                                      Q_ARG(QVariant, QVariant::fromValue(values)),
                                                      Q_ARG(QVariant, QVariant::fromValue(values)),
                                                      Q_ARG(QVariant, QVariant::fromValue(totalCount)),
                                                      Q_ARG(QVariant, QVariant::fromValue(startIndex)) );
        }
        break;

    case RgConstants::ResponseGetEStationInfo: {
            int totalCount = 0;
            int startIndex = 0;
            int evChargeType = -1;
            QVariantMap m = note->getBody().toMap();
            if (m.contains(SettingConstants::ChargeType)) {
                evChargeType = m[SettingConstants::ChargeType].toInt();
            }
            QVariantList items = m["info"].toList();
            QVariantList values = QVariantList{};
            for (int i=0; i<items.count(); i++) {
               QVariantList result = items.at(i).toList();
               bool isOnline = result[SearchConstants::ResultOnline].toBool();
               QString navSeq = isOnline ? PoiHelper::navSeqFromServer("0")
                                     : PoiHelper::navSeqFromLocal(result[SearchConstants::ResultPOIID].toInt());
               int poiId = isOnline ? PoiHelper::fromServer(result[SearchConstants::ResultPOIID].toInt(), navSeq.toInt())
                                     : PoiHelper::fromLocal(result[SearchConstants::ResultPOIID].toInt());
               result[SearchConstants::ResultPOIID ] = poiId;
               result[SearchConstants::ResultNavseq] = navSeq;
               result[SearchConstants::ResultOnline] = isOnline;
               switch(evChargeType) {
               case SettingConstants::ChargeTypeAll:
                   break;
               case SettingConstants::ChargeTypeAC3:
                   if (!result[SearchConstants::ResultAC3].toBool())
                       continue;
                   break;
               case SettingConstants::ChargeTypeDCDemo:
                   if (!result[SearchConstants::ResultDCDemo].toBool())
                       continue;
                   break;
               case SettingConstants::ChargeTypeDCCombo:
                   if (!result[SearchConstants::ResultDCCombo].toBool())
                       continue;
                   break;
               case SettingConstants::ChargeTypeSlowSpeed:
                   if (!result[SearchConstants::ResultSlowCharge].toBool())
                       continue;
                   break;
               }
               values.append(QVariant::fromValue(result));
            }
            totalCount = values.count();
            QMetaObject::invokeMethod(mViewComponent, "__onResultAlongRouteEStation",
                                                      Q_ARG(QVariant, QVariant::fromValue(values)),
                                                      Q_ARG(QVariant, QVariant::fromValue(values)),
                                                      Q_ARG(QVariant, QVariant::fromValue(totalCount)),
                                                      Q_ARG(QVariant, QVariant::fromValue(startIndex)) );
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
            if (caller != ViewRegistry::objectName(ViewRegistry::SearchResultView))
                break;

            QMetaObject::invokeMethod(mViewComponent, "__closeDialog", Q_ARG(QVariant, true));
        }
        break;

    case SettingConstants::DialogCanceled: {
            QVariantMap data = note->getBody().value<QVariantMap>();
            QString caller = data[CommonConstants::Caller].toString();
            QVariantMap m = data[CommonConstants::Value].toMap();
            if (caller != ViewRegistry::objectName(ViewRegistry::SearchResultView))
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

    case NavigationControllerConstants::ResponseController: {
            QVariantMap data = note->getBody().value<QVariantMap>();
            if (data.contains(CommonConstants::Lang)) {
                QString value = data[CommonConstants::Lang].toString();
                QQmlProperty::write( mViewComponent, "__lang", QVariant::fromValue(value) );
            }
        }
        break;

    case SettingConstants::SavedSettingFromObjectName: {
            QString objectName = note->getBody().toString();
            QMetaObject::invokeMethod(mViewComponent, "__savedSettingFromOption", Q_ARG(QVariant, QVariant::fromValue(objectName)));
        }
        break;

    case ApplicationConstants::ReplyToViewList:
        {
            auto data = note->getBody().toMap();
            QString objectName = data[CommonConstants::ObjectName].value<QString>();
            if (objectName != ViewRegistry::objectName(ViewRegistry::SearchResultView))
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

    case SearchConstants::RequestRemoveMarker:
        QMetaObject::invokeMethod(mViewComponent, "requestRemoveAllMarker");
        break;

    case SearchConstants::ResponseSort: {
            QVariantMap data = note->getBody().toMap();
            QString search = data[CommonConstants::Text].toString();
            QVariantList result  = data[CommonConstants::Value].toList();
            QMetaObject::invokeMethod(mViewComponent, "__onResultSort",
                                                  Q_ARG(QVariant, QVariant::fromValue(search)),
                                                  Q_ARG(QVariant, QVariant::fromValue(result)),
                                                  Q_ARG(QVariant, QVariant::fromValue(result.count())),
                                                  Q_ARG(QVariant, QVariant::fromValue(0)) );
        }
        break;
    case SearchConstants::ResponseCurrentStreet:
    case SearchConstants::ChangedCurrentStreet: {
            unsigned short value = note->getBody().toInt();
            QMetaObject::invokeMethod(mViewComponent, "__onCurrentStreetChanged", Q_ARG(QVariant, QVariant::fromValue(value)) );
        }
        break;
    case SearchConstants::CloseSearchResultView:
        QMetaObject::invokeMethod(mViewComponent, "close");
        break;
    case ApplicationConstants::ReplyToGadgetState:
    case ApplicationConstants::ChangedGadgetState:
        {
            auto m = note->getBody().toMap();
            mIsGadget = m["visible"].toBool();
            if ( mIsGadget ) {
                QMetaObject::invokeMethod(mViewComponent, "requestRemoveAllMarker");
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

bool SearchResultViewMediator::invokeResultLowest(const QVariantMap &data)
{
    const char *member = "__onResultLowest";
    int totalCount = 0;
    int startIndex = -1;
    QString search =  CommonConstants::Empty;
    QVariantList values = QVariantList{};

    QVariantMap reqData = data[SearchConstants::ReqData].value<QVariantMap>();
    auto owner = reinterpret_cast<void*>(reqData[CommonConstants::Owner].toULongLong());
    if (owner != this) {
       return false;
    }
    if (reqData.contains(SearchConstants::Query)){
       search = reqData[SearchConstants::Query].value<QString>();
    }
    if (reqData.contains(SearchConstants::Params)){
       QVariantMap params = reqData[SearchConstants::Params].value<QVariantMap>();
       if (params.contains(SearchConstants::StartIndex)){
           startIndex = params[SearchConstants::StartIndex].value<int>();
       }
    }

    QObject* res = data[SearchConstants::Result].value<QObject*>();
    if (!res) {
        responsedFreeTextSearchResult(data, values);
        QMetaObject::invokeMethod(mViewComponent, member,
                                      Q_ARG(QVariant, QVariant::fromValue(search)),
                                      Q_ARG(QVariant, QVariant::fromValue(values)),
                                      Q_ARG(QVariant, QVariant::fromValue(totalCount)),
                                      Q_ARG(QVariant, QVariant::fromValue(startIndex)) );
        return false;
    }
    SearchReplyBaseInterface* reply = qobject_cast<SearchReplyBaseInterface*>(res);
    if (!reply || reply->error()) {
        responsedFreeTextSearchResult(data, values);
        QMetaObject::invokeMethod(mViewComponent, member,
                                      Q_ARG(QVariant, QVariant::fromValue(search)),
                                      Q_ARG(QVariant, QVariant::fromValue(values)),
                                      Q_ARG(QVariant, QVariant::fromValue(totalCount)),
                                      Q_ARG(QVariant, QVariant::fromValue(startIndex)) );
        return false;
    }

    SearchPoiStationReplyInterface* searchReply = qobject_cast<SearchPoiStationReplyInterface*>(res);
    if (!searchReply) {
        QMetaObject::invokeMethod(mViewComponent, member  ,
                                            Q_ARG(QVariant, QVariant::fromValue(search)),
                                            Q_ARG(QVariant, QVariant::fromValue(values)),
                                            Q_ARG(QVariant, QVariant::fromValue(totalCount)),
                                            Q_ARG(QVariant, QVariant::fromValue(startIndex)) );
        return false;
    }

    const TPOI_STATION_INFO listitem = *searchReply->results();
    auto isOnline = reply->request()->getOnline();
    auto navSeq= isOnline ? PoiHelper::navSeqFromServer((const char *)"0")
                          : PoiHelper::navSeqFromLocal(listitem.nPoiID);
    auto poiId = isOnline ? PoiHelper::fromServer(listitem.nPoiID, navSeq.toInt())
                          : PoiHelper::fromLocal(listitem.nPoiID);

    SearchConstants::resultInit(values);
    values[SearchConstants::ResultName                ] = StringHelper::instance()->fromCp949((const char*)listitem.szName);
    values[SearchConstants::ResultPOIID               ] = poiId;
    values[SearchConstants::ResultX                   ] = listitem.tp.x;
    values[SearchConstants::ResultY                   ] = listitem.tp.y;
    values[SearchConstants::ResultKind                ] = listitem.nDataKind;
    values[SearchConstants::ResultOPIPoleCate         ] = listitem.nDataKind;
    values[SearchConstants::ResultOPIGasolinePrice    ] = listitem.nOPIGasolinePrice;
    values[SearchConstants::ResultOPIDieselPrice      ] = listitem.nOPIDieselPrice;
    values[SearchConstants::ResultOPILPGPrice         ] = listitem.nOPILPGPrice;
    values[SearchConstants::ResultOPIHiGasolinePrice  ] = listitem.nOPIHiGasolinePrice;
    values[SearchConstants::ResultOPIHiLPGPrice       ] = listitem.nOPIHiLPGPrice;
    values[SearchConstants::ResultNavseq              ] = navSeq;
    values[SearchConstants::ResultOnline              ] = isOnline;

    totalCount = searchReply->resultCount();
    QMetaObject::invokeMethod(mViewComponent, member,
                                            Q_ARG(QVariant, QVariant::fromValue(search)),
                                            Q_ARG(QVariant, QVariant::fromValue(values)),
                                            Q_ARG(QVariant, QVariant::fromValue(totalCount)),
                                            Q_ARG(QVariant, QVariant::fromValue(0)) );

    return true;
}

bool SearchResultViewMediator::invokeResultDetailInfo(const QVariantMap &data)
{
    const char *member = "__onResultDetailInfo";
    int totalCount = 0;
    int startIndex = -1;
    QString search =  CommonConstants::Empty;
    QVariantList values = QVariantList{};

    QVariantMap reqData = data[SearchConstants::ReqData].value<QVariantMap>();
    auto owner = reinterpret_cast<void*>(reqData[CommonConstants::Owner].toULongLong());
    if (owner != this) {
       return false;
    }
    if (reqData.contains(SearchConstants::Query)){
       search = reqData[SearchConstants::Query].value<QString>();
    }
    if (reqData.contains(SearchConstants::Params)){
       QVariantMap params = reqData[SearchConstants::Params].value<QVariantMap>();
       if (params.contains(SearchConstants::StartIndex)){
           startIndex = params[SearchConstants::StartIndex].value<int>();
       }
    }

    QObject* res = data[SearchConstants::Result].value<QObject*>();
    if (!res) {
        responsedFreeTextSearchResult(data, values);
        QMetaObject::invokeMethod(mViewComponent, member,
                                      Q_ARG(QVariant, QVariant::fromValue(search)),
                                      Q_ARG(QVariant, QVariant::fromValue(values)),
                                      Q_ARG(QVariant, QVariant::fromValue(totalCount)),
                                      Q_ARG(QVariant, QVariant::fromValue(startIndex)) );
        return false;
    }
    SearchReplyBaseInterface* reply = qobject_cast<SearchReplyBaseInterface*>(res);
    if (!reply || reply->error()) {
        responsedFreeTextSearchResult(data, values);
        QMetaObject::invokeMethod(mViewComponent, member,
                                      Q_ARG(QVariant, QVariant::fromValue(search)),
                                      Q_ARG(QVariant, QVariant::fromValue(values)),
                                      Q_ARG(QVariant, QVariant::fromValue(totalCount)),
                                      Q_ARG(QVariant, QVariant::fromValue(startIndex)) );
        return false;
    }

    SearchPoiReplyInterface* searchReply = qobject_cast<SearchPoiReplyInterface*>(res);
    if (!searchReply) {
        QMetaObject::invokeMethod(mViewComponent, member,
                                      Q_ARG(QVariant, QVariant::fromValue(search)),
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
    values[SearchConstants::ResultWorkTime            ] = StringHelper::instance()->fromCp949((const char*)listitem.szAddInformation );
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
    QMetaObject::invokeMethod(mViewComponent, member,
                                              Q_ARG(QVariant, QVariant::fromValue(search)),
                                              Q_ARG(QVariant, QVariant::fromValue(values)),
                                              Q_ARG(QVariant, QVariant::fromValue(totalCount)),
                                              Q_ARG(QVariant, QVariant::fromValue(startIndex)) );
    return true;
}

bool  SearchResultViewMediator::invokeResultEvDetailInfo(const QVariantMap &data)
{
    const char *member = "__onResultEvDetailInfo";
    int totalCount = 0;
    int startIndex = -1;
    QString search =  CommonConstants::Empty;
    QVariantList values = QVariantList{};

    QVariantMap reqData = data[SearchConstants::ReqData].value<QVariantMap>();
    auto owner = reinterpret_cast<void*>(reqData[CommonConstants::Owner].toULongLong());
    if (owner != this) {
       return false;
    }
    if (reqData.contains(SearchConstants::Query)){
       search = reqData[SearchConstants::Query].value<QString>();
    }
    if (reqData.contains(SearchConstants::Params)){
       QVariantMap params = reqData[SearchConstants::Params].value<QVariantMap>();
       if (params.contains(SearchConstants::StartIndex)){
           startIndex = params[SearchConstants::StartIndex].value<int>();
       }
    }

    QObject* res = data[SearchConstants::Result].value<QObject*>();
    if (!res) {
        responsedFreeTextSearchResult(data, values);
        QMetaObject::invokeMethod(mViewComponent, member,
                                      Q_ARG(QVariant, QVariant::fromValue(search)),
                                      Q_ARG(QVariant, QVariant::fromValue(values)),
                                      Q_ARG(QVariant, QVariant::fromValue(totalCount)),
                                      Q_ARG(QVariant, QVariant::fromValue(startIndex)) );
        return false;
    }
    SearchReplyBaseInterface* reply = qobject_cast<SearchReplyBaseInterface*>(res);
    if (!reply || reply->error()) {
        responsedFreeTextSearchResult(data, values);
        QMetaObject::invokeMethod(mViewComponent, member,
                                      Q_ARG(QVariant, QVariant::fromValue(search)),
                                      Q_ARG(QVariant, QVariant::fromValue(values)),
                                      Q_ARG(QVariant, QVariant::fromValue(totalCount)),
                                      Q_ARG(QVariant, QVariant::fromValue(startIndex)) );
        return false;
    }

    SearchEvPoiReplyInterface* searchReply = qobject_cast<SearchEvPoiReplyInterface*>(res);
    if (!searchReply) {
        QMetaObject::invokeMethod(mViewComponent, member,
                                              Q_ARG(QVariant, QVariant::fromValue(search)),
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
    values[SearchConstants::ResultStdName             ] = values[SearchConstants::ResultName                ];
    values[SearchConstants::ResultAddress             ] = strAddr;
    values[SearchConstants::ResultTelNum              ] = StringHelper::instance()->fromCp949((const char*)listitem.szPhoneNumber);
    values[SearchConstants::ResultPOIID               ] = poiId;
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

    QMetaObject::invokeMethod(mViewComponent, member,
                                              Q_ARG(QVariant, QVariant::fromValue(search)),
                                              Q_ARG(QVariant, QVariant::fromValue(values)),
                                              Q_ARG(QVariant, QVariant::fromValue(totalCount)),
                                              Q_ARG(QVariant, QVariant::fromValue(startIndex)) );
    return true;
}

bool  SearchResultViewMediator::invokeResult(const QVariantMap &data)
{
     const char *member = "__onResult";
     int totalCount = 0;
     int startIndex = -1;
     int kind = -1;
     int evChargeType = -1;
     bool sync = false;
     bool industrySearch = false;
     QVariantMap industryPoiIdMap;
     QString search =  CommonConstants::Empty;
     QString searchCode = CommonConstants::Empty;
     QVariantList values = QVariantList{};
     QVariantMap reqData = data[SearchConstants::ReqData].value<QVariantMap>();
     auto owner = reinterpret_cast<void*>(reqData[CommonConstants::Owner].toULongLong());
     if (owner != this) {
        return false;
     }
     if (reqData.contains(SearchConstants::Query)){
        search = reqData[SearchConstants::Query].value<QString>();
     }

     if (reqData.contains(SearchConstants::TextParam)){
        searchCode = reqData[SearchConstants::TextParam].value<QString>();
     }

     if (reqData.contains(SearchConstants::Params)){
        QVariantMap params = reqData[SearchConstants::Params].value<QVariantMap>();
        if (params.contains(SearchConstants::StartIndex)){
            startIndex = params[SearchConstants::StartIndex].value<int>();
        }
        if (params.contains(SearchConstants::Kind)){
            kind = params[SearchConstants::Kind].value<int>();
        }
        if (params.contains(SettingConstants::ChargeType)){
            evChargeType = params[SettingConstants::ChargeType].value<int>();
        }
        if (params.contains(SearchConstants::Sync)) {
            sync = params[SearchConstants::Sync].toBool();
        }
     }

     QObject* res = data[SearchConstants::Result].value<QObject*>();
     if (!res) {
         responsedFreeTextSearchResult(data, values);
         QMetaObject::invokeMethod(mViewComponent, member,
                                       Q_ARG(QVariant, QVariant::fromValue(search)),
                                       Q_ARG(QVariant, QVariant::fromValue(values)),
                                       Q_ARG(QVariant, QVariant::fromValue(totalCount)),
                                       Q_ARG(QVariant, QVariant::fromValue(startIndex)) );
         return false;
     }
     SearchReplyBaseInterface* reply = qobject_cast<SearchReplyBaseInterface*>(res);
     if (!reply || reply->error()) {
         responsedFreeTextSearchResult(data, values);
         QMetaObject::invokeMethod(mViewComponent, member,
                                       Q_ARG(QVariant, QVariant::fromValue(search)),
                                       Q_ARG(QVariant, QVariant::fromValue(values)),
                                       Q_ARG(QVariant, QVariant::fromValue(totalCount)),
                                       Q_ARG(QVariant, QVariant::fromValue(startIndex)) );
         return false;
     }

     SearchReplyInterface* searchReply = qobject_cast<SearchReplyInterface*>(res);
     if (!searchReply) {
         responsedFreeTextSearchResult(data, values);
         QMetaObject::invokeMethod(mViewComponent, member,
                                                       Q_ARG(QVariant, QVariant::fromValue(search)),
                                                       Q_ARG(QVariant, QVariant::fromValue(values)),
                                                       Q_ARG(QVariant, QVariant::fromValue(totalCount)),
                                                       Q_ARG(QVariant, QVariant::fromValue(startIndex)) );

         return false;
     }

     totalCount = searchReply->totalCount();
     int industryTotalCount = totalCount;
     int searchType = -1;
     if (reqData.contains(SearchConstants::SearchType )) {
         searchType = reqData[SearchConstants::SearchType].value<int>();
         if (searchType == SearchRequestBase::SearchBySisulCategory) {
            industrySearch = true;
            industryPoiIdMap = getIndustryPoiId(startIndex ,searchCode);
         }
     }

     auto req = reply->request();
     auto isOnline = req->getOnline();
     auto isLineChanged = req->getLineChanged();

     //캐시 통합검색
     if (!isOnline && totalCount <= 0) {
         if (searchType == SearchRequestBase::SearchBySisulName) {
            QVariantList resultCache = QQmlProperty::read(mViewComponent, "resultCache").toList();
            if (resultCache.length() <= 0) {
                sendNotification(SearchConstants::RequestCacheSearch, reqData);
            }
         }
     }

     const TPOI_SEARCH_LIST *listitem = searchReply->results();
     for (int i=0 ; i < searchReply->resultCount() ; i++) {
         auto navSeq= isOnline ? PoiHelper::navSeqFromServer(listitem[i].szNavseq)
                               : PoiHelper::navSeqFromLocal(listitem[i].nPoiID);
         auto poiId = isOnline ? PoiHelper::fromServer(listitem[i].nPoiID, navSeq.toInt())
                               : PoiHelper::fromLocal(listitem[i].nPoiID);
         QVariantList result;
         SearchConstants::resultInit(result);
         result[SearchConstants::ResultIndex               ] = listitem[i].nIdx;
         result[SearchConstants::ResultName                ] = StringHelper::instance()->fromCp949((const char*)listitem[i].szName);
         result[SearchConstants::ResultStdName             ] = StringHelper::instance()->fromCp949((const char*)listitem[i].szStdName);
         result[SearchConstants::ResultAddress             ] = StringHelper::instance()->fromCp949((const char*)listitem[i].szAddrName);
         result[SearchConstants::ResultTelNum              ] = StringHelper::instance()->fromCp949((const char*)listitem[i].szPhoneNumber);
         result[SearchConstants::ResultPOIID               ] = poiId;
         result[SearchConstants::ResultAddrCode            ] = listitem[i].nAddrCode;
         result[SearchConstants::ResultRpFlag              ] = listitem[i].byRPFlag;
         result[SearchConstants::ResultDistance            ] = listitem[i].nDist;
         result[SearchConstants::ResultDeg                 ] = listitem[i].nDeg;
         result[SearchConstants::ResultKind                ] = listitem[i].nKind;
         result[SearchConstants::ResultX                   ] = listitem[i].tp.x;
         result[SearchConstants::ResultY                   ] = listitem[i].tp.y;
         result[SearchConstants::ResultOPIPoleCate         ] = listitem[i].nOPIPoleCate;
         result[SearchConstants::ResultOPIPrice            ] = listitem[i].nOPIPrice;
         result[SearchConstants::ResultYear                ] = listitem[i].sGatherTime.nYear;
         result[SearchConstants::ResultMonth               ] = listitem[i].sGatherTime.nMonth;
         result[SearchConstants::ResultDay                 ] = listitem[i].sGatherTime.nDay;
         result[SearchConstants::ResultHour                ] = listitem[i].sGatherTime.nHour;
         result[SearchConstants::ResultMinute              ] = listitem[i].sGatherTime.nMinute;
         result[SearchConstants::ResultSecond              ] = listitem[i].sGatherTime.nSecond;
         result[SearchConstants::ResultWeek                ] = listitem[i].sGatherTime.nWeek;
         result[SearchConstants::ResultOilCompany          ] = StringHelper::instance()->fromCp949((const char*)listitem[i].szOilCompany);
         result[SearchConstants::ResultHighGasoline        ] = listitem[i].bHighGasoline;
         result[SearchConstants::ResultLowestOilSt         ] = listitem[i].bLowestOilSt;
         result[SearchConstants::ResultOPIGasolinePrice    ] = listitem[i].nOPIGasolinePrice;
         result[SearchConstants::ResultOPIDieselPrice      ] = listitem[i].nOPIDieselPrice;
         result[SearchConstants::ResultOPILPGPrice         ] = listitem[i].nOPILPGPrice;
         result[SearchConstants::ResultOPIHiGasolinePrice  ] = listitem[i].nOPIHiGasolinePrice;
         result[SearchConstants::ResultOPIHiLPGPrice       ] = listitem[i].nOPIHiLPGPrice;
         result[SearchConstants::ResultNavseq              ] = navSeq;
         result[SearchConstants::ResultOnline              ] = isOnline;
         result[SearchConstants::ResultPrimaryMax          ] = listitem[i].nPrimaryMax;
         result[SearchConstants::ResultPrimaryMin          ] = listitem[i].nPrimaryMin;
         result[SearchConstants::ResultSecondaryMax        ] = listitem[i].nSecondaryMax;
         result[SearchConstants::ResultSecondaryMin        ] = listitem[i].nSecondaryMin;
         result[SearchConstants::ResultType                ] = SearchConstants::ResultTypeNormal;         
         result[SearchConstants::ResultLineChanged         ] = isLineChanged;

         if (industrySearch) {
             if (industryPoiIdMap.count() > 0) {
                if (isIndustryPoiID(industryPoiIdMap, poiId)) {
                    result[SearchConstants::ResultStdName] = CommonConstants::Empty;
                    result[SearchConstants::ResultName] = CommonConstants::Empty;
                    industryTotalCount -= 1;
                }
             }
             if (industryTotalCount <= 0) {
                 totalCount = 0;
             }
         }

         if (sync && isOnline) {
             if (kind == SearchConstants::CategoryChargingEvSt) {
                 QString strInfo = CommonConstants::Empty;
                 for (int j=0; j< listitem[i].evStationCnt; j++) {
                     DS_POI_EVSTATION_CHARGERINFO chargerInfo = listitem[i].chargerInfo[j];
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
                 result[SearchConstants::ResultChargerInfo         ] = strInfo;
                 getInfoChargingEvSt(result);
                 switch(evChargeType) {
                 case SettingConstants::ChargeTypeAll:
                     break;
                 case SettingConstants::ChargeTypeAC3:
                     if (!result[SearchConstants::ResultAC3].toBool())
                         continue;
                     break;
                 case SettingConstants::ChargeTypeDCDemo:
                     if (!result[SearchConstants::ResultDCDemo].toBool())
                         continue;
                     break;
                 case SettingConstants::ChargeTypeDCCombo:
                     if (!result[SearchConstants::ResultDCCombo].toBool())
                         continue;
                     break;
                 case SettingConstants::ChargeTypeSlowSpeed:
                     if (!result[SearchConstants::ResultSlowCharge].toBool())
                         continue;
                     break;
                 }
             }
         }
         qDebug(ui) << "hhtest invokeResult]" << listitem[i].nPrimaryMin << "-" << listitem[i].nSecondaryMin << " ~ " << listitem[i].nPrimaryMax << "-" << listitem[i].nSecondaryMax;
         values.append(QVariant::fromValue(result));
     }

     responsedFreeTextSearchResult(data, values);


     QMetaObject::invokeMethod(mViewComponent, member,
                                               Q_ARG(QVariant, QVariant::fromValue(search)),
                                               Q_ARG(QVariant, QVariant::fromValue(values)),
                                               Q_ARG(QVariant, QVariant::fromValue(totalCount)),
                                               Q_ARG(QVariant, QVariant::fromValue(startIndex)) );

     return true;
}

void SearchResultViewMediator::getInfoChargingEvSt(QVariantList &result)
{
     QList<QVariant> evBrandNameArr = QQmlProperty::read(mViewComponent, "evBrandNameArr").toList();
     QMap<QString,QVariant> evBrandEnum = QQmlProperty::read(mViewComponent, "evBrandEnum").toMap();
     QString info = result[SearchConstants::ResultChargerInfo].toString();
     QString oper = CommonConstants::Empty;
     QString abbreviation = CommonConstants::Empty;
     int index = 0;
     info = info.trimmed();
     if (info.isEmpty())
         return;
     QStringList data = info.split(";");
     for(int i= 0; i<data.count(); i++) {
         QString d = data.at(i);
         int s = d.indexOf("[");
         int e = d.indexOf("]");
         if(s >= 0 && e >= 0) {
             QString t = d.mid( s+1, e-(s+1));
             QString v = d.mid(e+1);
             t = t.trimmed();
             v = v.trimmed();
             if (t.isEmpty() || v.isEmpty())
                 continue;
             if (t == SearchConstants::Bid) {
                 QRegExp re("\\d*");
                 if (re.exactMatch(v)){
                      index = v.toInt();
                 } else {
                     index = evBrandEnum[v].toInt();
                 }
                 abbreviation = evBrandNameArr[index].toString();
                 if (oper.indexOf(abbreviation)<0) {
                     if (!oper.isEmpty()) {
                         oper += ",";
                     } else {
                         //브랜드 코드를 주유소/충전소와 겹치지 않도록 만듦
                          result[SearchConstants::ResultOPIPoleCate] = index + SearchConstants::EVCHARGINGBASE;
                     }
                     oper += evBrandNameArr[index].toString();
                 }
              } else if (t == SearchConstants::Tid) {
                 switch(v.toInt()) {
                     case SearchConstants::EvKindDCdemo:
                         result[SearchConstants::ResultDCDemo] = true;
                         result[SearchConstants::ResultFastCharge] = true;
                     break;
                     case SearchConstants::EvKindACslow:
                         result[SearchConstants::ResultSlowCharge] = true;
                     break;
                     case SearchConstants::EvKindDCdemoAC3:
                         result[SearchConstants::ResultAC3] = true;
                         result[SearchConstants::ResultDCDemo] = true;
                         result[SearchConstants::ResultFastCharge] = true;
                     break;
                     case SearchConstants::EvKindDCcombo:
                         result[SearchConstants::ResultDCCombo] = true;
                         result[SearchConstants::ResultFastCharge] = true;
                     break;
                     case SearchConstants::EvKindDCdemoCombo:
                         result[SearchConstants::ResultDCDemo] = true;
                         result[SearchConstants::ResultDCCombo] = true;
                         result[SearchConstants::ResultFastCharge] = true;
                     break;
                     case SearchConstants::EvKindDCdemoAC3Combo:
                         result[SearchConstants::ResultAC3] = true;
                         result[SearchConstants::ResultDCDemo] = true;
                         result[SearchConstants::ResultDCCombo] = true;
                         result[SearchConstants::ResultFastCharge] = true;
                     break;
                     case SearchConstants::EvKindAC3:
                         result[SearchConstants::ResultAC3] = true;
                         result[SearchConstants::ResultFastCharge] = true;
                     break;
                 }
            }
         }
    }
}


void SearchResultViewMediator::queryInDriveMode()
{
    sendNotification(MapConstants::RequestDriveMode);
}

bool SearchResultViewMediator::sendByDestination(const QVariant &data)
{
    QJSValue jsVal = data.value<QJSValue>();
    TPOI_SEARCH_LIST *destination = nullptr;
    QString name;
    QString address;
    int poiId;
    int navSeq;
    int rpFlag;
    int x = 0;
    int y = 0;
    int poleCate = 0;
    int opiPrice = 0;
    if (destination) {
        x = destination->tp.x;
        y = destination->tp.y;
        name = StringHelper::instance()->fromCp949((const char*)destination->szName);
        address = StringHelper::instance()->fromCp949((const char*)destination->szAddrName);
        poiId   = destination->nPoiID;
        navSeq  = QString(destination->szNavseq).toInt();
        poleCate = destination->nOPIPoleCate;
        opiPrice = destination->nOPIPrice;
        rpFlag = destination->byRPFlag;
    } else {
        QVariantMap m = data.toMap();
        x = m[CommonConstants::X].toInt();
        y = m[CommonConstants::Y].toInt();
        name = m[CommonConstants::Name].toString();
        address = m[CommonConstants::Address].toString();
        poiId   = m[SearchConstants::PoiID].toInt();
        navSeq  = m[SearchConstants::NavSeq].toString().toInt();
        poleCate = m[SearchConstants::OpiPoleCate].toInt();
        rpFlag = m[SearchConstants::RpFlag].toInt();
    }

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

    auto searchType = QQmlProperty::read(mViewComponent, SearchConstants::SearchType).toString();
    RouteConstants::DestSearchCode destSearchCode;
    if (searchType == CommonConstants::Name) {
        destSearchCode = RouteConstants::EXPLORER_TSEARCH_DES;
    } else if (searchType == CommonConstants::Around) {
        destSearchCode = RouteConstants::EXPLORER_ROUNDINFO_DES;
    } else if (searchType == CommonConstants::Industry) {
        destSearchCode = RouteConstants::EXPLORER_LOCAL_BUSINESS_TYPE_DES;
    } else if (searchType == CommonConstants::Address) {
        destSearchCode = RouteConstants::EXPLORER_ADDRESS_DES;
    } else {
        destSearchCode = RouteConstants::EXPLORER_FIRST_DES; // 기타
    }
    body[SearchConstants::DestType] = destSearchCode;

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
    bookmark.insert( SearchConstants::OpiPrice, opiPrice);

    sendNotification(NavibookmarkConstants::SelectNavibookAddress,bookmark);

    sendNotification(Constants::CreateView,QVariantMap{ {CommonConstants::Url,ViewRegistry::url(ViewRegistry::RouteCandidateView)} });
    return true;
}

bool SearchResultViewMediator::sendAddSearchMarker(const QVariant &data)
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

bool SearchResultViewMediator::sendRemoveSearchMarker(const QVariant &data)
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

void SearchResultViewMediator::closeAllViewWithoutCaller(const QString &caller)
{
    // 즐겨 찾기를 통합검색을 통해 검색 결과 화면으로 들어 오면 caller와 owner가 동일함.
    // 19.08.27 ymhong
    auto owner = QQmlProperty::read(mViewComponent, CommonConstants::Owner).toString();
    auto relay = QQmlProperty::read(mViewComponent, CommonConstants::Relay).toString();
    if (!owner.isEmpty() && (owner != caller)) {
        sendNotification(Constants::CloseView, QVariantMap{{CommonConstants::ObjectName, owner}});
    }
    if (!relay.isEmpty() && (relay != caller)) {
        sendNotification(Constants::CloseView, QVariantMap{{CommonConstants::ObjectName, relay}});
    }
    QMetaObject::invokeMethod(mViewComponent, "closeView");
}

void SearchResultViewMediator::closeAllView()
{
    QMetaObject::invokeMethod(mViewComponent, "closeAllView");
}

void SearchResultViewMediator::showConfirmDialog(const QString &address)
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

void SearchResultViewMediator::changeMenu( const QVariant& data ) {
    QVariantMap d = data.value<QVariantMap>();
    if (d.size() >= 2)  {
        sendNotification(ZoneConstants::SelectSoftkeyToggleIndexMode, data);
    } else {
        sendNotification(ZoneConstants::SelectSoftkeyIndexMenuMode, data);
    }
}

void SearchResultViewMediator::updateSearchResultCount(const QString &text, int count)
{
    auto owner = QQmlProperty::read(mViewComponent, CommonConstants::Owner).toString();
    mSearchResultCount = count;
    QVariantMap m;
    m[CommonConstants::Owner] = owner;
    m[CommonConstants::Text] = text;
    m[CommonConstants::Count] = count;
    m[CommonConstants::Caller] = ViewRegistry::objectName(ViewRegistry::SearchResultView);

    // 경유지 -> 통합 검색 시 caller는 RouteSettingView
    // 통합 검색 시 caller는 SearchTabView
    auto caller = QQmlProperty::read(mViewComponent, CommonConstants::Caller).toString();
    if (caller == CommonConstants::System) {
        // 가젯을 터치한 경우 타이틀만 보여 준다.
        return;
    }

    // 경유지 설정 > 통합 검색 > 검색어 입력 후 검색결과가 없을 때
    // 아래 UpdateSearchResultCount에 의해 키보드가 표시 되지만 Owner가 ""가 되어
    // 키보드 뒤로가기 처리가 안되는 문제 수정
    // 19.05.17 ymhong
    if (owner == ViewRegistry::objectName(ViewRegistry::TextSearchView)) {
        if (count == 0) {
            sendNotification(KeyboardConstants::NotifyKeyboardOwner, owner);
            sendNotification(SearchConstants::TextSearchHasNoResult);
        }
    }

#ifdef QT_DEBUG
    m["debugCaller"] = caller;
#endif
    if (caller == ViewRegistry::objectName(ViewRegistry::RouteSettingView)) {
        // 통합 검색 화면에서
        // 1. 검색어 리스트에서 선택한 경우 visible은 true
        // 2. 검색어를 누르고 입력한 경우 검색결과가 있으면 visible은 true, 없으면 visible은 false
        auto visible = QQmlProperty::read(mViewComponent, CommonConstants::Visible).toBool();
#ifdef QT_DEBUG
        m["debugVisible"] = visible;
#endif
        // 검색어 입력 하고 엔터를 눌렀는데 검색 결과가 없는 경우.
        // 키보드를 그대로 유지하도록 한다.
        // TMCRSM-2144.
        // 18.11.20 ymhong.
        if ( !visible ) {
            m[CommonConstants::Show] = true;
        }

        // 경유지 설정 > 통합 검색 > 검색 결과인 경우 타이틀이 노출 되므로
        // UpdateSearchResultCount를 호출할 필요 없음.
        // 19.11.25 ymhong
        return;
    }

    sendNotification(ZoneConstants::UpdateSearchResultCount, m);
}


QVariantMap SearchResultViewMediator::runSort(QVariantMap &data)
{
    int index = -1;
    int key = data[CommonConstants::Key].toInt();
    int fuel = data[CommonConstants::Fuel].toInt();
    QVariantList result  = data[CommonConstants::Value].toList();
    switch(key) {
        case DistanceOrder: index = SearchConstants::ResultDistance; break;
        case PriceOrder: index = getIndexByFuel(fuel); break;
        case NameOrder: index = SearchConstants::ResultStdName; break;
        case AlphabeticalOrder: index = SearchConstants::ResultIndex; break;
    }
    if (index >= 0) {
        qSort(result.begin(), result.end(), [=](QVariant &a, QVariant &b){
          QVariant left,right;
          if (key == PriceOrder ||
              key == DistanceOrder ) {
              //TMCRSM-457 - 0원일 경우 리스트의 최하단에 위치하도록
              left = a.toList()[index] <= 0 ? INT_MAX : a.toList()[index];
              right = b.toList()[index] <= 0 ? INT_MAX : b.toList()[index];
              if (left == right) {
                  //가격이나 거리가 동일한 경우 소트
                  int idx = 0;
                  if (key == SearchResultViewMediator::PriceOrder) {
                      idx = SearchConstants::ResultDistance;
                  } else {
                      idx = getIndexByFuel(fuel);
                  }
                  left = a.toList()[idx];
                  right = b.toList()[idx];
              }
          } else if (key == AlphabeticalOrder) {
              left = a.toList()[index];
              right = b.toList()[index];
          } else {
              left = a.toList()[index];
              right = b.toList()[index];
              //TMCRSM-1552-이름순 정렬이 안되는 현상 수정
              if (index == SearchConstants::ResultStdName) {
                  if (left == CommonConstants::Empty) {
                      left = a.toList()[SearchConstants::ResultName];
                  }
                  if (right == CommonConstants::Empty) {
                      right = b.toList()[SearchConstants::ResultName];
                  }
              }
          }
          return  left < right;
        });
    }

    int count = result.count();
    for(int i=0; i<count; i++) {
        QVariantList item = result.at(i).toList();
        item[SearchConstants::ResultSort] = true;
        result[i] = item;
    }

   data[CommonConstants::Key] = key;
   data[CommonConstants::Fuel] = fuel;
   data[CommonConstants::Value] = result;

   return data;
}

void SearchResultViewMediator::finishedSort()
{
    QFuture<QVariantMap> future = mWatcher->future();
    mWatcher->deleteLater();
    QVariantMap data = future.result();
    sendNotification(SearchConstants::ResponseSort,data);
}

bool SearchResultViewMediator::isVisible() const
{
    auto visible = QQmlProperty::read(mViewComponent, CommonConstants::Visible).toBool();
    return visible;
}

bool SearchResultViewMediator::isActive() const
{
    auto active = QQmlProperty::read(mViewComponent, CommonConstants::Active).toBool();
    return active;
}

void SearchResultViewMediator::setIndustryCodeMap()
{
    QVariantMap values;
    //////////////////////////////////////////////////////////////
    //TMCRSM-3360
    //////////////////////////////////////////////////////////////
    //0.category search > public convenience > public institution > other public institution
    //고봉중고등학교(679839000),
    values.clear();
    values[QStringLiteral("679839000")] = true;
    mIndustryCodeMap[QStringLiteral("60199")] = values;

    //1.category search > vehicle convenience > car dealership > other
    //옥션네트웍스(753604100)
    values.clear();
    values[QStringLiteral("753604100")] = true;
    mIndustryCodeMap[QStringLiteral("20799")] = values;

    //2.category search > building facilities > manufacturing facility > fishery facility
    values.clear();
    mIndustryCodeMap[QStringLiteral("30301")] = values;

    //3.category search > building facilities > public facility > general equipment
    values.clear();
    mIndustryCodeMap[QStringLiteral("30405")] = values;

    //4.category search > public convenience > country office/ward office > country office
    //음성구청(848730700)
    values.clear();
    values[QStringLiteral("848730700")] = true;
    mIndustryCodeMap[QStringLiteral("60501")] = values;

    //5.category search > public convenience > educational institution > international school
    values.clear();
    mIndustryCodeMap[QStringLiteral("60707")] = values;

    //6.category search > medical > hospital > police hospital
    //한양성심의원(187213100)
    values.clear();
    values[QStringLiteral("187213100")] = true;
    mIndustryCodeMap[QStringLiteral("90105")] = values;

    //*.category search > vehicle convenience > Direct service center > other
    //죽전현대모터스 (320608800)
    values.clear();
    values[QStringLiteral("320608800")] = true;
    mIndustryCodeMap[QStringLiteral("20499")] = values;

    //*category search > Building Facility > apartment/villa > apartment
    //동광빌라 (797791000)
    values.clear();
    values[QStringLiteral("797791000")] = true;
    mIndustryCodeMap[QStringLiteral("30101")] = values;

    //*category search > Building Facility > Public facility > Public facility etc
    //JOB카페(151454300),JOB카페 정문(151454302),JOB카페 주차장(151454301)
    values.clear();
    values[QStringLiteral("151454300")] = true;
    values[QStringLiteral("151454302")] = true;
    values[QStringLiteral("151454301")] = true;
    mIndustryCodeMap[QStringLiteral("30499")] = values;

    //*category search > Building Facility > social facility > hall
    //경기도여성능력개발센터(73480700)
    values.clear();
    values[QStringLiteral("73480700")] = true;
    mIndustryCodeMap[QStringLiteral("30805")] = values;

    //*category search > TourLeisure > Golf > a golf practice range
    //청남건설(695759800)
    values.clear();
    values[QStringLiteral("695759800")] = true;
    mIndustryCodeMap[QStringLiteral("40302")] = values;

    //*category search > TourLeisure > Golf > Etc golf
    //경찰남부경찰청 용인체력단련장(72328900)
    values.clear();
    values[QStringLiteral("72328900")] = true;
    mIndustryCodeMap[QStringLiteral("40399")] = values;

    //*category search > TourLeisure > Leisure > Fishing place
    //런커몰(579907800)
    values.clear();
    values[QStringLiteral("579907800")] = true;
    mIndustryCodeMap[QStringLiteral("40409")] = values;

    //*category search > Shopping > convenience store /super market > super market
    //한국유통 수원동마을(320605300)
    values.clear();
    values[QStringLiteral("320605300")] = true;
    mIndustryCodeMap[QStringLiteral("50202")] = values;

    //*category search > Living convenience > restaurant > korean style(
    //BHC용인공세점(287666700)
    values.clear();
    values[QStringLiteral("287666700")] = true;
    mIndustryCodeMap[QStringLiteral("80101")] = values;

    //*category search > Living convenience > religious facility > church
    //보라동성가정성당(320548500)
    values.clear();
    values[QStringLiteral("320548500")] = true;
    mIndustryCodeMap[QStringLiteral("80401")] = values;

    //*category search > Living convenience > lifestyle > beauty shop
    //박승철헤어스투디오 수지롯데마트 3개 표시(182085000,182085001,182085002)
    //박승철헤어스투디오 흥덕이마트점 3개 표시(266183300,266183301,266183302)
    values.clear();
    values[QStringLiteral("182085001")] = true;
    values[QStringLiteral("182085002")] = true;
    values[QStringLiteral("266183301")] = true;
    values[QStringLiteral("266183302")] = true;
    mIndustryCodeMap[QStringLiteral("89904")] = values;

    //*category search > Medical servicee > clinics > Doctor's office
    //선함빌딩(772433000), 현진프라자(772401200)
    values.clear();
    values[QStringLiteral("772433000")] = true;
    values[QStringLiteral("772401200")] = true;
    mIndustryCodeMap[QStringLiteral("90201")] = values;

    //*category search > Medical servicee > Oriental medicine clinic > Oriental medicine hospital
    //해든마루빌딩(772451300), 상당빌딩(772453300), 한서빌딩(772450900)
    values.clear();
    values[QStringLiteral("772451300")] = true;
    values[QStringLiteral("772453300")] = true;
    values[QStringLiteral("772450900")] = true;
    mIndustryCodeMap[QStringLiteral("90301")] = values;

    //*category search > Medical servicee > medical facility > Casualty
    //애견사랑(384976500)
    values.clear();
    values[QStringLiteral("384976500")] = true;
    mIndustryCodeMap[QStringLiteral("99901")] = values;

    //////////////////////////////////////////////////////////////
    //TMCRSM-3326
    //////////////////////////////////////////////////////////////
    //7.category search > building facilities > warehouse > other warehouse
    //신세계법인사업부(782707100)
    values.clear();
    values[QStringLiteral("782707100")] = true;
    mIndustryCodeMap[QStringLiteral("30999")] = values;

    //////////////////////////////////////////////////////////////
    //TMCRSM-3325
    //////////////////////////////////////////////////////////////
    //8.category search > building facilities > warehouse > oil reservoir
    //SK에너지 인천물류센터(834437200)
    //SK에너지 서울물류센터(39149300)
    //SK에너지 와동무류센터(39156700)
    //SK에너지 대전물류센터(39155800)
    //SK에너지 전주물류센터(39170400)
    //SK에너지 군산물류센터(53296700)
    //SK에너지 동해물류센터(39170900)
    //SK에너지 대구물류센터(39153700)
    //SK에너지 광주물류센터(39158300)
    //SK에너지 포항물류센터(70870500)
    //SK에너지 마산물류센터(39157000)
    //SK에너지 부산물류센터(39152800)
    //오일뱅크 군산물류센터(407623800)
    values.clear();
    values[QStringLiteral("834437200")] = true;
    values[QStringLiteral("39149300") ] = true;
    values[QStringLiteral("39156700") ] = true;
    values[QStringLiteral("39155800") ] = true;
    values[QStringLiteral("39170400") ] = true;
    values[QStringLiteral("53296700") ] = true;
    values[QStringLiteral("39170900") ] = true;
    values[QStringLiteral("39153700") ] = true;
    values[QStringLiteral("39158300") ] = true;
    values[QStringLiteral("70870500") ] = true;
    values[QStringLiteral("39157000") ] = true;
    values[QStringLiteral("39152800") ] = true;
    values[QStringLiteral("407623800")] = true;
    mIndustryCodeMap[QStringLiteral("30901")] = values;

    //////////////////////////////////////////////////////////////
    //TMCRSM-3322
    //////////////////////////////////////////////////////////////
    //9.category search > travel/leisure > accommodations > condominium/Resort
    //나인트리프리미어호텔 명동2(794081100,827410800)
    //나인트리프리미어호텔 명동II(828159400)
    //이코노미호텔(816770700,581472500)
    //&MEAL(890511500)
    //센추럴팍(753584800)
    values.clear();
    values[QStringLiteral("794081100")] = true;
    values[QStringLiteral("827410800")] = true;
    values[QStringLiteral("828159400")] = true;
    values[QStringLiteral("816770700")] = true;
    values[QStringLiteral("581472500")] = true;
    values[QStringLiteral("890511500")] = true;
    values[QStringLiteral("753584800")] = true;
    mIndustryCodeMap[QStringLiteral("40102")] = values;
}

QVariantMap SearchResultViewMediator::getIndustryPoiId(const int &startListIndex, const QString &code)
{
    if (startListIndex > 0)
        return mIndustryPoiIdMap;
    mIndustryPoiIdMap.clear();
    if (mIndustryCodeMap.contains(code)) {
        mIndustryPoiIdMap[CommonConstants::Result] =  mIndustryCodeMap[code];
    }
    return mIndustryPoiIdMap;
}

bool SearchResultViewMediator::isIndustryPoiID(const QVariantMap &poiIdMap, const int &poiId)
{    
    if (poiIdMap.contains(CommonConstants::Result)) {
        QVariantMap poiIdMapValue = poiIdMap[CommonConstants::Result].value<QVariantMap>();
        if (poiIdMapValue.contains(QString::number(poiId)))
            return poiIdMapValue[QString::number(poiId)].toBool();
    }
    return false;
}

QString SearchResultViewMediator::getDistanceText(const int &distance)
{
    QString ret;
    if (distance < 1000) {
        ret = QString("%1m").arg(distance);
    } else if (distance < 10 * 1000) {
        int d1 = distance / 1000;
        int d2 = (distance % 1000) / 100;
        ret = QString("%1.%2km").arg(d1).arg(d2);
    } else {
        ret = QString("%1km").arg(distance / 1000);
    }
    return ret;
}

int SearchResultViewMediator::getDistance(const int &x, const int &y)
{
    VSM_Int32 distance;
    int wx , wy;
    auto proxy = Facade::instance()->retrieveProxy<NavigationControllerProxy>();
    NavigationController* controller = proxy->getControllerHandle();
    controller->currentPosition(wx,wy);
    VSMMap_GetDistanceWorld(x, y, wx, wy, distance);
    return distance;
}
