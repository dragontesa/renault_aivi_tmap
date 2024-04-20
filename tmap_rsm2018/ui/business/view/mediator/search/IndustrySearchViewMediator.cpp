#include "IndustrySearchViewMediator.h"
#include "Constants.h"
#include "SettingDto.h"
#include "MapConstants.h"
#include "ZoneConstants.h"
#include <QJSValueIterator>
#include "CommonConstants.h"
#include "SearchConstants.h"
#include "SettingConstants.h"
#include "../common/searchrequest.h"
#include "StringHelper.h"
#include "ViewRegistry.h"
#include "NavigationControllerConstants.h"
#include "RgConstants.h"
#include "RouteConstants.h"
#include "mediator/app/MapViewMode.h"
#include "LangConstants.h"
#include "UtilConstants.h"
#include "ApplicationConstants.h"
#include "NavigationControllerProtocol.h"

using namespace SKT;

IndustrySearchViewMediator::IndustrySearchViewMediator( QObject* viewComponent )
    : AbstractViewMediator( viewComponent )
{
}

IndustrySearchViewMediator::~IndustrySearchViewMediator()
{

}

QList<int> IndustrySearchViewMediator::listNotificationInterests()
{
    static QList<int> notes = {
        ZoneConstants::DoSoftKeyFunction,
        ZoneConstants::DoSoftkeyIndexMenuFunction,
        SearchConstants::ResponseSearch,
        SearchConstants::UpdateView,
        SettingConstants::ApplySetting,
        SettingConstants::ChangedSetting,
        RouteConstants::Hitting,
        NavigationControllerConstants::ChangeLanguage,
        SearchConstants::QueueChanged,
        ApplicationConstants::ReplyToViewList,
        NavigationControllerConstants::ResponseController,
    };

    return notes;
}

bool IndustrySearchViewMediator::onEvent( int event, const QVariant& data )
{
    T_DEBUGV( "event: %d", event );

    switch ( event ) {
    case Activated: {            
            sendNotification(MapConstants::ChangeMapViewMode,QVariantMap{{CommonConstants::Mode,MapViewMode::MenuNavi}});

            sendNotification(ZoneConstants::RegistSoftkey, ViewRegistry::objectName(ViewRegistry::IndustrySearchView));
            sendNotification(ZoneConstants::SelectSoftkeyIndexMenuMode, data);
        }
        break;

    case Deactivated:
        sendNotification(ZoneConstants::SelectSoftkeyIndexMenuMode, QStringList{});
        break;

    case CreateView:
        sendNotification( Constants::CreateView, data );
        break;

    case SelectZone1Title:{
            sendNotification(ZoneConstants::SelectZone1Title, data);
        }
        break;
    case RequestSearch: {
           QJSValue jsVal = data.value<QJSValue>();
           QString code = jsVal.property( SearchConstants::Query).toString();
           QVariant param = jsVal.property(SearchConstants::Params).toVariant();
           QVariantMap params = param.value<QVariantMap>();

           QVariantMap body;
           body.insert( SearchConstants::SearchType, SearchRequestBase::SearchNextCategory);
           body.insert( SearchConstants::TextParam, code);
           body.insert( SearchConstants::Params, QVariant::fromValue(params));
           sendNotification(SearchConstants::RequestSearch, body);
       }
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

    case ResetCsw:
        sendNotification(NavigationControllerConstants::ResetCSW);
        break;

    case RegistCsw:
        sendNotification(NavigationControllerConstants::RegistCSW, data);
        break;


    case ReturnToNaviMenu:
        {
            QString url = ViewRegistry::url(ViewRegistry::NaviMenuView);
            sendNotification(Constants::CreateView, QVariantMap{{CommonConstants::Url, url}});
        }
        break;

    case RequestAddressType: {
            QVariantMap body;
            body[CommonConstants::Section] = SettingConstants::MapMain;
            body[CommonConstants::Key] = SettingConstants::AddressDisplayMethod;
            sendNotification( SettingConstants::RequestSetting, body );
        }
        break;

    case ViewSizeChanged:
        {
            // 경유지 설정 -> 업종 검색 후 다시 경유지 설정으로 가면
            // 화면에 마커가 표시되지 않는 이슈가 있어서 이부분 코드 주석 처리함.
            // 18.02.13 ymhong

            // 경유지 설정을 통해서 들어오는 경우 호출되지 않도록 qml에서 수정됨
            // 경유지를 통한 경우가 아니면 동작하여야 하므로 코드 주석 처리 제거
            // 19.07.11 cjlee
            auto jsVal = data.value<QJSValue>();
            int bottom = jsVal.property( CommonConstants::B ).toInt();
            QVariantMap body;
            body.insert(SearchConstants::B,  bottom);
            sendNotification(MapConstants::UpdateMapViewPadding, body);
        }
        break;

    case ShowProgressIndicator:
        sendNotification( UtilConstants::ShowProgressIndicator );
        break;

    case HideProgressIndicator:
        sendNotification( UtilConstants::HideProgressIndicator );
        break;

    case RemoveSearchQueue: {
              QVariantMap body;
              body.insert( SearchConstants::SearchType, SearchRequestBase::RemoveSearchQueue);
              sendNotification(SearchConstants::RequestSearch, body);
          }
          break;

    case RequestViewList: {
            QVariantMap body;
            body.insert( CommonConstants::ObjectName,   ViewRegistry::objectName(ViewRegistry::IndustrySearchView));
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

    case DepthChanged:
        {
            // caller가 내비메뉴이고 depth가 1인 경우만 blockReroute를 하지 않음
            // 19.08.07 ymhong.
            auto caller = QQmlProperty::read(mViewComponent, "caller").toString();
            auto depth = data.toInt();
            auto blockReroute = (1 < depth) || (caller != ViewRegistry::objectName(ViewRegistry::NaviMenuView));
            sendNotification(RgConstants::BlockReroute, blockReroute);
        }
        break;

    default:
        return false;
    }
    return true;
}

bool IndustrySearchViewMediator::handleNotification( const mvc::INotificationPtr& note )
{
    switch ( note->getId() ) {
    case SearchConstants::ResponseSearch: {
            QVariantMap data = note->getBody().value<QVariantMap>();
            if (!data.contains(SearchConstants::Result)) {
                return false;
            }

            QObject* res = data[SearchConstants::Result].value<QObject*>();
            if (!res) {
                return false;
            }
            SearchReplyBaseInterface* reply = qobject_cast<SearchReplyBaseInterface*>(res);
            if (!reply || reply->error()) {
                return false;
            }
            SearchNextCateReplyInterface* replyCate = qobject_cast<SearchNextCateReplyInterface*>(res);
            if (!replyCate) {
                return false;
            }

            QString search = "";
            int startIndex = -1;
            int depth = -1;
            if (data.contains(SearchConstants::ReqData)) {
                QVariantMap reqData = data[SearchConstants::ReqData].value<QVariantMap>();
                auto owner = reinterpret_cast<void*>(reqData[CommonConstants::Owner].toULongLong());
                if (owner != this)
                    return false;
                if (reqData.contains(SearchConstants::TextParam)){
                    search = reqData[SearchConstants::TextParam].value<QString>();
                }
                if (reqData.contains(SearchConstants::Params)){
                    QVariantMap params = reqData[SearchConstants::Params].value<QVariantMap>();
                    if (params.contains(SearchConstants::StartIndex)){
                        startIndex = params[SearchConstants::StartIndex].value<int>();
                    }
                    if (params.contains(SearchConstants::Depth)){
                        depth = params[SearchConstants::Depth].value<int>();
                    }
                }
            }

            const TS_RESULT_CATE *listitem = replyCate->results();
            QVariantList values = QVariantList{};
            for (int i=0 ; i < replyCate->resultCount() ; i++) {
                auto& item = listitem[i];
                QString cateName = StringHelper::instance()->fromCp949((const char*)item.szCateName);
                QString cateCode = QString::number( item.nCateCode );
                QString itemCnt = QString::number( item.nItemCnt );
                QVariantList result;
                result.insert(IndustrySearchViewMediator::ResultName              , cateName);
                result.insert(IndustrySearchViewMediator::ResultCode              , cateCode);
                result.insert(IndustrySearchViewMediator::ResultCount             , itemCnt);
                values.append(QVariant::fromValue(result));
            }


            int totalCount = replyCate->totalCount();
            QMetaObject::invokeMethod(mViewComponent, "__onResult", Q_ARG(QVariant, QVariant::fromValue(search)),
                                                                    Q_ARG(QVariant, QVariant::fromValue(values)),
                                                                    Q_ARG(QVariant, QVariant::fromValue(totalCount)),
                                                                    Q_ARG(QVariant, QVariant::fromValue(startIndex)),
                                                                    Q_ARG(QVariant, QVariant::fromValue(depth))   );
        }
        break;
    case ZoneConstants::DoSoftKeyFunction:
    case ZoneConstants::DoSoftkeyIndexMenuFunction:{
            QVariantMap data = note->getBody().value<QVariantMap>();
            QString objectName = data[CommonConstants::Name].toString();
            if (objectName != ViewRegistry::objectName(ViewRegistry::IndustrySearchView))
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
            if (objectName != ViewRegistry::objectName(ViewRegistry::IndustrySearchView))
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
            QString section = data[CommonConstants::Section].toString();
            QString key = data[CommonConstants::Key].toString();
            if ( section == SettingConstants::MapMain &&
                 key == SettingConstants::AddressDisplayMethod ) {
                int method = data[CommonConstants::Setting].value<SettingDtoPtr>()->value().toInt();
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
            QString sect = data[CommonConstants::Section].toString();
            QString key = data[CommonConstants::Key].toString();
            if ( sect == SettingConstants::MapMain && key == SettingConstants::AddressDisplayMethod ) {
                int method = data[CommonConstants::Setting].value<SettingDtoPtr>()->value().toInt();
                QQmlProperty::write(mViewComponent, SearchConstants::AddressType, (method == SettingConstants::AddressDisplayMethodLandNumber ) ? SearchConstants::OldAddrType : SearchConstants::NewAddrType );
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

    case SearchConstants::QueueChanged: {
            int count = note->getBody().value<int>();
            QMetaObject::invokeMethod(mViewComponent, "__queueChanged", Q_ARG(QVariant, count));
        }
        break;

    case ApplicationConstants::ReplyToViewList:
        {
            auto data = note->getBody().toMap();
            QString objectName = data[CommonConstants::ObjectName].value<QString>();
            if (objectName != ViewRegistry::objectName(ViewRegistry::IndustrySearchView))
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

bool IndustrySearchViewMediator::isVisible() const
{
    auto visible = QQmlProperty::read(mViewComponent, CommonConstants::Visible).toBool();
    return visible;
}

bool IndustrySearchViewMediator::isActive() const
{
    auto active = QQmlProperty::read(mViewComponent, CommonConstants::Active).toBool();
    return active;
}

