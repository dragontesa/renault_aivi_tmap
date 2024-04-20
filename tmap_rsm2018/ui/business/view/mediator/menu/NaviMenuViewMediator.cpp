#include "NaviMenuViewMediator.h"
#include "Constants.h"
#include "RouteConstants.h"
#include "SearchConstants.h"
#include "MapConstants.h"
#include "ZoneConstants.h"
#include "SettingConstants.h"
#include "SettingDto.h"
#include <QJSValueIterator>
#include "mediator/app/MapViewMode.h"
#include "FavoriteDto.h"
#include "RecentDestinationDto.h"
#include "ViewRegistry.h"
#include "../common/searchrequest.h"
#include "StringHelper.h"
#include "NavigationControllerConstants.h"
#include "RgConstants.h"
#include "NavibookmarkConstants.h"
#include "LangConstants.h"
#include "ApplicationConstants.h"
#include "PlatformHelper.h"
#include "NavigationControllerProtocol.h"

using namespace SKT;

NaviMenuViewMediator::NaviMenuViewMediator( QObject* viewComponent )
    : AbstractViewMediator( viewComponent )
{
}

QList<int> NaviMenuViewMediator::listNotificationInterests()
{
    static QList<int> notes = {
        SettingConstants::ApplySetting,
        ZoneConstants::DoSoftKeyFunction,
        ZoneConstants::DoSoftkeyIndexMenuFunction,
        SearchConstants::ResponseFavorites,
        SearchConstants::ResponseRecentDestination,
        MapConstants::ResponseDriveMode,
        SettingConstants::UpdateView,
        RouteConstants::Hitting,
        RouteConstants::RelayHitting,
        NavibookmarkConstants::RegistBookmarkOk,
        SettingConstants::DialogAccepted,
        SettingConstants::DialogCanceled,
        NavigationControllerConstants::ChangeLanguage,
        ApplicationConstants::ReplyToViewList,
        NavigationControllerConstants::ResponseController,
    };

    return notes;
}

bool NaviMenuViewMediator::onEvent( int event, const QVariant& data )
{
    T_DEBUGV( "event: %d", event );

    switch ( event ) {
    case Activated: {
            sendNotification(Constants::CloseView, QVariantMap{{CommonConstants::ObjectName,ViewRegistry::objectName(ViewRegistry::RecentTileGridView)}}); // 2019.05.21 lckxx fixed TMCRSM-3175: Tile view is not closed when csw get unfocused
            sendNotification(MapConstants::ChangeMapViewMode,QVariantMap{{CommonConstants::Mode,MapViewMode::MenuNavi}});
            sendNotification(NavigationControllerConstants::CollectCSW, QVariant::fromValue(mViewComponent));

            sendNotification(ZoneConstants::RegistSoftkey, ViewRegistry::objectName(ViewRegistry::NaviMenuView));
            sendNotification(ZoneConstants::SelectSoftkeyIndexMenuMode, data);

            // 즐겨찾기 집으로, 회사로 Query 요청
            // 즐겨찾기 요청시 첫번째, 두번째 아이템은 집, 회사 순이기 때문에
            // 다만 집, 회사가 DB에 없을 경우 일반 즐겨찾기 아이템이 있을 수 있음.
            QVariantMap m;            
            m[CommonConstants::Max] = 2;
            m[CommonConstants::Offset] = 0;
            sendNotification(SearchConstants::RequestFavorites, m);
            sendNotification(RgConstants::BlockReroute, false);            
        }
        break;

    case Deactivated:
        sendNotification(NavigationControllerConstants::ResetCSW);
        break;

    case CreateView:
        {
            // NaviMenuView.qml 에서 showDialog()내에서 호출 되지만
            // showDialog()를 호출하는 곳이 없음
            // 19.02.14 ymhong.
            auto m = data.toMap();
            auto url = m[CommonConstants::Url].toString();
            if (url == ViewRegistry::url(ViewRegistry::CommonDialogView)) {
                PlatformHelper::instance()->setParentForSystemPopup(m);
            }
            sendNotification(Constants::CreateView, m);
        }
        break;

    case CloseView:
        sendNotification(Constants::CloseView, data);
        break;

    case SelectZone1Title:
        sendNotification(ZoneConstants::SelectZone1Title, data);
        break;

    case SelectZone1TextInput:
        sendNotification(ZoneConstants::SelectZone1TextInput);
        break;

    case ActivatedForNavi:
        {
            sendNotification(ZoneConstants::RegistSoftkey, ViewRegistry::objectName(ViewRegistry::NaviMenuView));

            // 'noOption'으로 dimming 처리된 option활성화(TMCRSM-490)
            sendNotification(ZoneConstants::SelectSoftkeyIndexMenuMode);

            sendNotification(ZoneConstants::SelectSoftkeyNaviMode);
        }
        break;

    case ViewSizeChanged:
        {
            auto jsVal = data.value<QJSValue>();
            int bottom = jsVal.property( CommonConstants::B ).toInt();
            QVariantMap body;
            body.insert(SearchConstants::B,  bottom);
            sendNotification(MapConstants::UpdateMapViewPadding, body);
        }
        break;

     case RequestRoute:
        mArrivalData = data;
        sendNotification(MapConstants::RequestDriveMode);
        break;

     case RequestRecentDestination:
         sendNotification(SearchConstants::RequestRecentDestination, data);
    	 break;

    case ModalChanged:
        sendNotification(ZoneConstants::UpdateModal, data.toMap());
        break;

    // 17.12.8 hskim not used
    case StartRouteGuidance:
        sendByDestination(mArrivalData);
        QMetaObject::invokeMethod(mViewComponent, "close");
        break;

    case ResetCsw:
        sendNotification(NavigationControllerConstants::ResetCSW);
        break;

    case RegistCsw:
        sendNotification(NavigationControllerConstants::RegistCSW, data);
        break;

    case ShowRegistHomeDialog:
    case ShowRegistOfficeDialog:
        {
            auto isHome = event == ShowRegistHomeDialog;
            QVariantMap m;
            m[CommonConstants::Url] = ViewRegistry::url(ViewRegistry::CommonDialogView);
            m[CommonConstants::Caller] = ViewRegistry::objectName(ViewRegistry::NaviMenuView);

            QString errorMessage = isHome ? tr("집이") : tr("회사가");
            errorMessage += tr(" 등록되어 있지 않습니다.\n지금 등록하시겠습니까?");

            m["title"] = "안내";
            m["message"] = errorMessage;
            m["menus"] = QStringList {"취소", "확인"};
            PlatformHelper::instance()->setParentForSystemPopup(m);
            sendNotification(Constants::CreateView, m);

            mRegistHomeAndOfficeData = data;
        }
        break;

    case RequestViewList: {
            QVariantMap body;
            body.insert( CommonConstants::ObjectName,   ViewRegistry::objectName(ViewRegistry::NaviMenuView));
            sendNotification(ApplicationConstants::RequestViewList, body);
        }
        break;

    case RequestLang: {
             QVariantMap body;
             body.insert(QStringLiteral("type"), NavigationControllerProtocol::GetLang );
             sendNotification( NavigationControllerConstants::RequestController, body );
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

    default:
        return false;
    }

    return true;
}

bool NaviMenuViewMediator::handleNotification( const mvc::INotificationPtr& note )
{
    switch ( note->getId() ) {
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
       case ZoneConstants::DoSoftKeyFunction:
       case ZoneConstants::DoSoftkeyIndexMenuFunction:{
            auto data = note->getBody().value<QVariantMap>();
            auto objectName = data[CommonConstants::Name].toString();
            if (objectName != ViewRegistry::objectName(ViewRegistry::NaviMenuView))
                return false;

            QString objFunction = CommonConstants::Empty;
            QString objMenu = CommonConstants::Empty;
            if (data.contains(ZoneConstants::Function))
                objFunction = data[ZoneConstants::Function].toString();
            if (data.contains(ZoneConstants::Menu ) )
                objMenu = data[ZoneConstants::Menu].toString();
            int  objIndex  = -1;
            if (data.contains(ZoneConstants::Index))
                objIndex = data[ZoneConstants::Index].toInt();
            QMetaObject::invokeMethod(mViewComponent, "__doSoftKeyFunction", Q_ARG(QVariant, QVariant::fromValue(objFunction)), Q_ARG(QVariant, QVariant::fromValue(objMenu)), Q_ARG(QVariant, QVariant::fromValue(objIndex)) );
        }
        break;

    case SearchConstants::ResponseFavorites:
        parseFavorites(note->getBody());
        break;

    case SearchConstants::ResponseRecentDestination:
        {
            auto data = note->getBody().value<QVariantMap>();
            auto owner = data[CommonConstants::Owner].toString();
            if (owner != ViewRegistry::objectName(ViewRegistry::NaviMenuView))
                return false;
            int page = SearchConstants::RecentDestPageIndex;
            QMetaObject::invokeMethod(mViewComponent, "__openSearchTabPageView", Q_ARG(QVariant, QVariant::fromValue(page)));
        }
    	break;

    case MapConstants::ResponseDriveMode:
        {
            if (isActive()) {
                // 주변 검색에 의해 ResponseDriveMode 가 발생할 수 있기 때문에
                // Activated 상태가 아닌 경우에는 다른 곳에서 처리될 수 있도록 한다.
                // 중복 처리에 의한 버그
                // 17.06.27 ymhong.
                auto driveMode = note->getBody().toInt();
                if (driveMode == MapViewMode::Navi) {
                    showConfirmDialog();
                } else {
                    sendByDestination(mArrivalData);
                    QMetaObject::invokeMethod(mViewComponent, "close");
                }
            }
        }
        break;
    case SettingConstants::UpdateView: {
            QVariantMap data = note->getBody().value<QVariantMap>();
            QString objectName = data[CommonConstants::ObjectName].toString();
            if (objectName != ViewRegistry::objectName(ViewRegistry::NaviMenuView))
                return false;

            for ( auto it = data.constBegin(); it != data.constEnd(); it++ ){
                if (CommonConstants::Url == it.key() ||
                    CommonConstants::ObjectName == it.key() ||
                    CommonConstants::Visible == it.key() ||
                    CommonConstants::Sync == it.key())
                    continue;
               QQmlProperty::write( mViewComponent, it.key(), QVariant::fromValue( it.value() ) );
            }
            if (data.contains(CommonConstants::Sync)) {
                QQmlProperty::write( mViewComponent, CommonConstants::Sync, QVariant::fromValue( data[CommonConstants::Sync]) );
            }
            if (data.contains(CommonConstants::Visible)) {
                QQmlProperty::write( mViewComponent, CommonConstants::Visible, QVariant::fromValue( data[CommonConstants::Visible]) );
            }
        }
        break;
    case RouteConstants::Hitting:   {
            if (!isActive()) {
                break;
            }
            QMetaObject::invokeMethod(mViewComponent, "close");
        }
        break;

   case RouteConstants::RelayHitting: {
            QMetaObject::invokeMethod(mViewComponent, "close");
        }
        break;

    case NavibookmarkConstants::RegistBookmarkOk:
        {
            auto owner = mRegistHomeAndOfficeData.toMap()[CommonConstants::Owner].toString();

            QVariantMap m;
            m[CommonConstants::Url] = ViewRegistry::url(ViewRegistry::BookmarkRegistView);
            m[CommonConstants::Caller] = ViewRegistry::objectName(ViewRegistry::NaviMenuView);
            m[CommonConstants::Owner] = owner;
            PlatformHelper::instance()->setParentForSystemPopup(m);

            sendNotification(Constants::CreateView, m);
        }
        break;

    case SettingConstants::DialogAccepted: {
            auto m = note->getBody().toMap();
            auto message = m["message"].toString();
            if (message.contains(tr(" 등록되어 있지 않습니다.\n지금 등록하시겠습니까?"))) {
                auto owner = mRegistHomeAndOfficeData.toMap()[CommonConstants::Owner].toString();

                QVariantMap m2;
                m2[CommonConstants::Url] = ViewRegistry::url(ViewRegistry::BookmarkRegistView);
                m2[CommonConstants::Caller] = ViewRegistry::objectName(ViewRegistry::NaviMenuView);
                m2[CommonConstants::Owner] = owner;
                PlatformHelper::instance()->setParentForSystemPopup(m2);

                sendNotification(Constants::CreateView, m2);
            }
        }
        break;

    case SettingConstants::DialogCanceled: {
            QVariantMap data = note->getBody().value<QVariantMap>();
            QString caller = data[CommonConstants::Caller].toString();
            QVariantMap m = data[CommonConstants::Value].toMap();
            if (caller != ViewRegistry::objectName(ViewRegistry::NaviMenuView))
                break;
            QMetaObject::invokeMethod(mViewComponent, "__closeDialog", Q_ARG(QVariant, false));
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
            if (objectName != ViewRegistry::objectName(ViewRegistry::NaviMenuView))
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

void NaviMenuViewMediator::parseFavorites(const QVariant &body)
{
    auto data = body.value<QList<FavoriteDtoPtr> >();
    for ( FavoriteDtoPtr dto : data )
    {
        auto x = dto->mapX().toInt();
        auto y = dto->mapY().toInt();
        auto destType = dto->destType().toInt();

        QVariantMap m;
        m[CommonConstants::X] = x;
        m[CommonConstants::Y] = y;
        m[CommonConstants::Address] = dto->address().toString();
        m[CommonConstants::Alias] = dto->alias().toString();

        switch (destType) {
        case 2: // home
            QMetaObject::invokeMethod(mViewComponent, "__setHome", Q_ARG(QVariant, m));
            break;

        case 1: // office
            QMetaObject::invokeMethod(mViewComponent, "__setOffice", Q_ARG(QVariant, m));
            break;
        }
    }
}

void NaviMenuViewMediator::sendByDestination(const QVariant &data)
{
    QVariantMap body;
    body[SearchConstants::Clear] = true;
    sendNotification(RouteConstants::SetDeparture, body);

    // data = {x:x, y:y}
    auto m = data.toMap();
    auto type = m[CommonConstants::Type].toInt();
    if (type == 2) {
        m[SearchConstants::DestType] = RouteConstants::EXPLORER_HOME_DES;
    } else {
        m[SearchConstants::DestType] = RouteConstants::EXPLORER_OFFICE_DES;
    }
    sendNotification(RouteConstants::SetArrival, m);

    body.clear();
    body[CommonConstants::Url] = ViewRegistry::url(ViewRegistry::RouteCandidateView);
    sendNotification(Constants::CreateView, body);
}

void NaviMenuViewMediator::showConfirmDialog()
{
    auto m = mArrivalData.toMap();
    auto x = m["x"].toInt();
    auto y = m["y"].toInt();
    auto address = m[CommonConstants::Address].toString();
    auto name = m[CommonConstants::Name].toString();

    m.clear();

    m[QStringLiteral("url")] = ViewRegistry::url(ViewRegistry::ChangeDestinationView);
    m[QStringLiteral("destX")] = x;
    m[QStringLiteral("destY")] = y;
    m[QStringLiteral("address")] = address;
    m[QStringLiteral("name")] = name;
    m[QStringLiteral("view")] = QVariant::fromValue(mViewComponent);
    PlatformHelper::instance()->setParentForSystemPopup(m);
    sendNotification(Constants::CreateView, m);
}

bool NaviMenuViewMediator::isVisible() const
{
    auto visible = QQmlProperty::read(mViewComponent, CommonConstants::Visible).toBool();
    return visible;
}

bool NaviMenuViewMediator::isActive() const
{
    auto active = QQmlProperty::read(mViewComponent, CommonConstants::Active).toBool();
    return active;
}
