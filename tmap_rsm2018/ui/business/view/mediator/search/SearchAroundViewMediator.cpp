#include "SearchAroundViewMediator.h"
#include "Constants.h"
#include "SearchConstants.h"
#include "SettingDto.h"
#include "SettingConstants.h"
#include "MapConstants.h"
#include "ZoneConstants.h"
#include <QJSValueIterator>
#include "StringHelper.h"
#include "../common/searchrequest.h"
#include "../common/PoiHelper.h"
#include "NavigationControllerConstants.h"
#include "ViewRegistry.h"
#include "SearchResultViewMediator.h"
#include "RouteConstants.h"
#include "mediator/app/MapViewMode.h"
#include "LangConstants.h"
#include "UtilConstants.h"
#include "ApplicationConstants.h"
#include "NavigationControllerProtocol.h"
#include "RgConstants.h"

using namespace SKT;

SearchAroundViewMediator::SearchAroundViewMediator( QObject* viewComponent )
    : AbstractViewMediator( viewComponent )
{
}

QList<int> SearchAroundViewMediator::listNotificationInterests()
{
    static QList<int> notes = {
        SearchConstants::ResponseSearch,
        ZoneConstants::DoSoftKeyFunction,
        ZoneConstants::DoSoftkeyIndexMenuFunction,
        SearchConstants::UpdateView,
        SettingConstants::ApplySetting,
        SettingConstants::ChangedSetting,
        RouteConstants::Hitting,
        NavigationControllerConstants::ChangeLanguage,
        ApplicationConstants::ReplyToViewList,
        NavigationControllerConstants::ResponseController,
    };
    return notes;
}

bool SearchAroundViewMediator::onEvent( int event, const QVariant& data )
{
    T_DEBUGV( "event: %d", event );
    switch ( event ) {
    case Activated: {
            sendNotification(MapConstants::ChangeMapViewMode,QVariantMap{{CommonConstants::Mode,MapViewMode::MenuNavi}});

            sendNotification(NavigationControllerConstants::QueryOnlineStatus);

            sendNotification(ZoneConstants::RegistSoftkey, ViewRegistry::objectName(ViewRegistry::SearchAroundView));
            sendNotification(ZoneConstants::SelectSoftkeyIndexMenuMode, data);

            auto caller = QQmlProperty::read(mViewComponent, "caller").toString();
            if (caller == ViewRegistry::objectName(ViewRegistry::NaviMenuView)) {
                // 내비메뉴로 부터 왔을 때
                sendNotification(RgConstants::BlockReroute, false);
            } else {
                // 즐겨찾기 및 경유지 추가로 부터 왔을 때
                sendNotification(RgConstants::BlockReroute, true);
            }
        }
        break;

    case Deactivated:
        sendNotification(ZoneConstants::SelectSoftkeyIndexMenuMode, QStringList{});
        break;

    case CreateView:
        sendNotification(Constants::CreateView, data);
        break;

    case SelectZone1Title:{
            sendNotification(ZoneConstants::SelectZone1Title, data);
        }
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

    case RequestSearch: {
            QJSValue jsVal = data.value<QJSValue>();
            QString query = jsVal.property( SearchConstants::Query).toString();
            QString textParam = jsVal.property( SearchConstants::TextParam).toString();
            int order = jsVal.property( SearchConstants::Order).toInt();
            QString searchType  = jsVal.property(SearchConstants::SearchType).toString();
            QVariant param = jsVal.property(SearchConstants::Params).toVariant();
            QVariantMap params = param.value<QVariantMap>();
            QString addressType = jsVal.property(SearchConstants::AddressType).toString();

            QVariantMap body;
            if (order == SearchResultViewMediator::DistanceOrder)
                  body.insert( SearchConstants::SortBy, SearchConstants::DistanceOrder );
            else if (order == SearchResultViewMediator::PriceOrder)
                 body.insert( SearchConstants::SortBy, SearchConstants::PriceOrder );
            else if (order == SearchResultViewMediator::NameOrder)
                 body.insert( SearchConstants::SortBy, SearchConstants::NameOrder );
            body.insert( SearchConstants::SearchType, SearchRequestBase::SearchBySisulAround);
            if (addressType == SearchConstants::NewAddrType) {
                body.insert( SearchConstants::RoadSearch, true );
            } else {
                body.insert( SearchConstants::RoadSearch, false );
            }
            body.insert( SearchConstants::TextParam, (QString)textParam);
            body.insert( SearchConstants::Query,   (QString)query);            
            body.insert( SearchConstants::Params, QVariant::fromValue(params));            
            sendNotification(SearchConstants::RequestSearch, body);
        }
        break;

    case ModalChanged:
        sendNotification(ZoneConstants::UpdateModal,data.toMap());
        break;

    case CloseView:
        sendNotification(Constants::CloseView, data);
        break;

    case ReturnToFavorite:
        sendNotification(SearchConstants::ReturnToFavorite);
        break;

    case ReturnToViaSetting:
        {
            QString caller = QQmlProperty::read(mViewComponent, CommonConstants::Caller).toString();
            sendNotification(Constants::ShowView, QVariantMap{{CommonConstants::ObjectName, caller}});
        }
        break;

    case ReturnToNaviMenu:
        {
            QString url = ViewRegistry::url(ViewRegistry::NaviMenuView);
            sendNotification(Constants::CreateView, QVariantMap{{CommonConstants::Url, url}});
        }
        break;

    case ViewSizeChanged:
        {
            // 경유지 설정 -> 주변 검색 후 다시 경유지 설정으로 가면
            // 화면에 마커가 표시되지 않는 이슈가 있어서 이부분 코드 주석 처리함.
            // 18.02.05 ymhong

            // 경유지 설정을 통해서 들어오는 경우 호출되지 않도록 qml에서 수정됨
            // 경유지를 통한 경우가 아니면 동작하여야 하므로 코드 주석 처리 제거
            // 19.07.11 cjlee
            QJSValue jsVal = data.value<QJSValue>();
            int bottom = jsVal.property( CommonConstants::B ).toInt();
            QVariantMap body;
            body.insert(SearchConstants::B,  bottom);
            sendNotification(MapConstants::UpdateMapViewPadding, body);
        }
        break;

    case ResetCsw:
        sendNotification(NavigationControllerConstants::ResetCSW);
        break;

    case RegistCsw:
        sendNotification(NavigationControllerConstants::RegistCSW, data);
        break;

    case RequestAddressType: {
            QVariantMap body;
            body[CommonConstants::Section] = SettingConstants::MapMain;
            body[CommonConstants::Key] = SettingConstants::AddressDisplayMethod;
            sendNotification( SettingConstants::RequestSetting, body );
        }
        break;

    case RequestViewList: {
            QVariantMap body;
            body.insert( CommonConstants::ObjectName,   ViewRegistry::objectName(ViewRegistry::SearchAroundView));
            sendNotification(ApplicationConstants::RequestViewList, body);
        }
        break;

    case RequestLang: {
             QVariantMap body;
             body.insert(QStringLiteral("type"), NavigationControllerProtocol::GetLang );
             sendNotification( NavigationControllerConstants::RequestController, body );
         }
         break;

    case UpdateView:
        sendNotification(SearchConstants::UpdateView, data);
        break;

    default:
        return false;
    }
    return true;
}

bool SearchAroundViewMediator::handleNotification( const mvc::INotificationPtr& note )
{
    switch ( note->getId() ) {
    case SearchConstants::ResponseSearch: {
            QVariantMap data = note->getBody().value<QVariantMap>();
            if (!data.contains(SearchConstants::Result))
                return false;

            QVariantList values = QVariantList{};
            if (data.contains(SearchConstants::ReqData)) {
                QVariantMap reqData = data[SearchConstants::ReqData].value<QVariantMap>();
                auto owner = reinterpret_cast<void*>(reqData[CommonConstants::Owner].toULongLong());
                if (owner != this)
                    return false;
            }

            QObject* res = data[SearchConstants::Result].value<QObject*>();
            if (!res) {
                QQmlProperty::write( mViewComponent, SearchConstants::Result, QVariant::fromValue( values ) );
                return false;
            }
            SearchReplyBaseInterface* reply = qobject_cast<SearchReplyBaseInterface*>(res);
            if (!reply || reply->error()) {
                QQmlProperty::write( mViewComponent, SearchConstants::Result, QVariant::fromValue( values ) );
                return false;
            }
            SearchReplyInterface* searchReply = qobject_cast<SearchReplyInterface*>(res);
            if (!searchReply) {
                QQmlProperty::write( mViewComponent, SearchConstants::Result, QVariant::fromValue( values ) );
                return false;
            } 

            auto req = reply->request();
            auto isOnline = req->getOnline();

            const TPOI_SEARCH_LIST *listitem = searchReply->results();            
            for (int i=0 ; i < searchReply->resultCount() ; i++) {
                auto navSeq= isOnline ? PoiHelper::navSeqFromServer(listitem[i].szNavseq)
                                      : PoiHelper::navSeqFromLocal(listitem[i].nPoiID);
                auto poiId = isOnline ? PoiHelper::fromServer(listitem[i].nPoiID, navSeq.toInt())
                                      : PoiHelper::fromLocal(listitem[i].nPoiID);
                QVariantList result;
                SearchConstants::resultInit(result);
                result.insert(SearchConstants::ResultIndex               , listitem[i].nIdx);
                result.insert(SearchConstants::ResultName                , StringHelper::instance()->fromCp949((const char*)listitem[i].szName));
                result.insert(SearchConstants::ResultStdName             , StringHelper::instance()->fromCp949((const char*)listitem[i].szStdName));
                result.insert(SearchConstants::ResultAddress             , StringHelper::instance()->fromCp949((const char*)listitem[i].szAddrName));
                result.insert(SearchConstants::ResultTelNum              , StringHelper::instance()->fromCp949((const char*)listitem[i].szPhoneNumber));
                result.insert(SearchConstants::ResultPOIID               , poiId);
                result.insert(SearchConstants::ResultNavseq              , navSeq);
                result.insert(SearchConstants::ResultAddrCode            , listitem[i].nAddrCode);
                result.insert(SearchConstants::ResultRpFlag              , listitem[i].byRPFlag);
                result.insert(SearchConstants::ResultDistance            , listitem[i].nDist);
                result.insert(SearchConstants::ResultDeg                 , listitem[i].nDeg);
                result.insert(SearchConstants::ResultKind                , listitem[i].nKind);
                result.insert(SearchConstants::ResultX                   , listitem[i].tp.x);
                result.insert(SearchConstants::ResultY                   , listitem[i].tp.y);
                result.insert(SearchConstants::ResultOPIPoleCate         , listitem[i].nOPIPoleCate);
                result.insert(SearchConstants::ResultOPIPrice            , listitem[i].nOPIPrice);
                result.insert(SearchConstants::ResultYear                , listitem[i].sGatherTime.nYear);
                result.insert(SearchConstants::ResultMonth               , listitem[i].sGatherTime.nMonth);
                result.insert(SearchConstants::ResultDay                 , listitem[i].sGatherTime.nDay);
                result.insert(SearchConstants::ResultHour                , listitem[i].sGatherTime.nHour);
                result.insert(SearchConstants::ResultMinute              , listitem[i].sGatherTime.nMinute);
                result.insert(SearchConstants::ResultSecond              , listitem[i].sGatherTime.nSecond);
                result.insert(SearchConstants::ResultWeek                , listitem[i].sGatherTime.nWeek);
                result.insert(SearchConstants::ResultOilCompany          , StringHelper::instance()->fromCp949((const char*)listitem[i].szOilCompany));
                result.insert(SearchConstants::ResultHighGasoline        , listitem[i].bHighGasoline);
                result.insert(SearchConstants::ResultLowestOilSt         , listitem[i].bLowestOilSt);
                result.insert(SearchConstants::ResultOPIGasolinePrice    , listitem[i].nOPIGasolinePrice);
                result.insert(SearchConstants::ResultOPIDieselPrice      , listitem[i].nOPIDieselPrice);
                result.insert(SearchConstants::ResultOPILPGPrice         , listitem[i].nOPILPGPrice);
                result.insert(SearchConstants::ResultOPIHiGasolinePrice  , listitem[i].nOPIHiGasolinePrice);
                result.insert(SearchConstants::ResultOPIHiLPGPrice       , listitem[i].nOPIHiLPGPrice);
                result.insert(SearchConstants::ResultWebSite             , CommonConstants::Empty);
                result.insert(SearchConstants::ResultHolliday            , CommonConstants::Empty);
                result.insert(SearchConstants::ResultWorkTime            , CommonConstants::Empty);
                result.insert(SearchConstants::ResultIndustry            , CommonConstants::Empty);
                result.insert(SearchConstants::ResultParking             , CommonConstants::Empty);
                result.insert(SearchConstants::ResultIntroduction        , CommonConstants::Empty);
                values.append(QVariant::fromValue(result));
            }

            QQmlProperty::write( mViewComponent, SearchConstants::Result, QVariant::fromValue( values ) );
        }
        break;
    case ZoneConstants::DoSoftKeyFunction:
    case ZoneConstants::DoSoftkeyIndexMenuFunction:{
            QVariantMap data = note->getBody().value<QVariantMap>();
            QString objectName = data[CommonConstants::Name].toString();
            if (objectName != ViewRegistry::objectName(ViewRegistry::SearchAroundView))
                return false;

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
            QVariantMap data = note->getBody().value<QVariantMap>();
            QString objectName = data[CommonConstants::ObjectName].toString();
            if (objectName != ViewRegistry::objectName(ViewRegistry::SearchAroundView))
                return false;

            for ( auto it = data.constBegin(); it != data.constEnd(); it++ ){
                if (CommonConstants::Url == it.key() ||
                    CommonConstants::ObjectName == it.key() ||
                    CommonConstants::ReFocus == it.key()) {
                    continue;
                }
                QQmlProperty::write( mViewComponent, it.key(), QVariant::fromValue( it.value() ) );
            }
            if (data.contains(CommonConstants::ReFocus)) {
                QQmlProperty::write( mViewComponent, CommonConstants::ReFocus, QVariant::fromValue( data[CommonConstants::ReFocus]) );
            }
        }
        break;
    case SettingConstants::ApplySetting: {
            QVariantMap data = note->getBody().value<QVariantMap>();
            auto owner = reinterpret_cast<void*>(data[CommonConstants::Owner].toULongLong());
            if (owner != this)
                break;
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
        break;
    case RouteConstants::Hitting:   {
            bool mouseAreaEnable = QQmlProperty::read(mViewComponent, "mouseAreaEnabled").toBool();
            if (!isActive() || mouseAreaEnable) {
                break;
            }
            sendNotification(RouteConstants::RelayHitting);
            QMetaObject::invokeMethod(mViewComponent, "close");
        }
        break;

    case NavigationControllerConstants::ChangeLanguage: {
            auto data = note->getBody().value<QVariantMap>();
            auto lang = data[LangConstants::Lang].toString();
            QMetaObject::invokeMethod(mViewComponent, "__retranslateUi", Q_ARG(QVariant, QVariant::fromValue(lang)));
        }
        break;

    case ApplicationConstants::ReplyToViewList:
        {
            auto data = note->getBody().toMap();
            QString objectName = data[CommonConstants::ObjectName].value<QString>();
            if (objectName != ViewRegistry::objectName(ViewRegistry::SearchAroundView))
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


    case NavigationControllerConstants::ResponseController: {
            QVariantMap data = note->getBody().value<QVariantMap>();
            if (data.contains(CommonConstants::Lang)) {
                QString value = data[CommonConstants::Lang].toString();
                QQmlProperty::write( mViewComponent, "__lang", QVariant::fromValue(value) );
            }
        }
        break;


    default:
        return false;
    }
    return true;
}

bool SearchAroundViewMediator::isVisible() const
{
    auto visible = QQmlProperty::read(mViewComponent, CommonConstants::Visible).toBool();
    return visible;
}

bool SearchAroundViewMediator::isActive() const
{
    auto active = QQmlProperty::read(mViewComponent, CommonConstants::Active).toBool();
    return active;
}


