#include "TestRecentDestinationPageViewMediator.h"
#include "Constants.h"
#include "CommonConstants.h"
#include "ZoneConstants.h"
#include "KeyboardConstants.h"
#include "NavigationControllerConstants.h"
#include "SearchConstants.h"
#include "MapConstants.h"
#include "RouteConstants.h"
#include "ViewRegistry.h"
#include "BookmarkHelper.h"
#include "RecentDestinationDto.h"
#include "mediator/app/MapViewMode.h"

using namespace SKT;

TestRecentDestinationPageViewMediator::TestRecentDestinationPageViewMediator( QObject* viewComponent )
    : AbstractViewMediator( viewComponent )
    , mIsActivate(false)
{
    mEditType = -1;
    mIsEditMode = false;
}

QList<int> TestRecentDestinationPageViewMediator::listNotificationInterests()
{
    static QList<int> notes = {
            SearchConstants::ResponseRecentDestination,
            SearchConstants::ResponseSearch,
            RouteConstants::UpdateSummary,
            ZoneConstants::DoSoftKeyFunction,
            MapConstants::ResponseDriveMode,
            SearchConstants::ApplyTabPageActivation
    };
    return notes;
}

void TestRecentDestinationPageViewMediator::showChangeDestinationConfirmDialog(const QString &address)
{
    auto x = mStartRouteGuidanceMap[CommonConstants::X].toInt();
    auto y = mStartRouteGuidanceMap[CommonConstants::Y].toInt();

    QVariantMap m;

    m[QStringLiteral("url")] = ViewRegistry::url(ViewRegistry::ChangeDestinationView);
    m[QStringLiteral("destX")] = x;
    m[QStringLiteral("destY")] = y;
    m[QStringLiteral("address")] = address;
    m[QStringLiteral("view")] = QVariant::fromValue(mViewComponent);
    m[QStringLiteral("okCallback")] = QStringLiteral("closeView");
    sendNotification(Constants::CreateView, m);
}

void TestRecentDestinationPageViewMediator::close()
{
    QMetaObject::invokeMethod(mViewComponent, "close");
}

void TestRecentDestinationPageViewMediator::startRouteGuidance()
{
    auto m = mStartRouteGuidanceMap;
    auto x = m[CommonConstants::X].toInt();
    auto y = m[CommonConstants::Y].toInt();
    auto name = m[CommonConstants::Name].toString();

    if (x > 0 && y > 0) {
        QVariantMap body;
        body[SearchConstants::Clear] = true;
        sendNotification(RouteConstants::SetDeparture, body);
        body.clear();

        body[CommonConstants::X] = x;
        body[CommonConstants::Y] = y;
        body[CommonConstants::Name] = name;

        sendNotification(RouteConstants::SetArrival, body);

        sendNotification(Constants::CreateView,QVariantMap{ {CommonConstants::Url,ViewRegistry::url(ViewRegistry::RouteCandidateView)} });
        close();
    }
}

bool TestRecentDestinationPageViewMediator::onEvent( int event, const QVariant& data )
{
    switch (event) {
    case Activated:
        mIsActivate = true;
        break;
    case Deactivated:
        mIsActivate = false;
        break;
    case SelectSoftkey:
        {
            T_DEBUGV( "SelectSoftkey #1" );
            auto objectName = QQmlProperty::read(mViewComponent, CommonConstants::ObjectName).toString();
            sendNotification(ZoneConstants::RegistSoftkey, objectName);
            sendNotification(ZoneConstants::SelectSoftkeyMenuMode, data);
        }
        break;
    case CreateView:
        sendNotification(Constants::CreateView, data);
        break;
    case ResetCsw:
        sendNotification(NavigationControllerConstants::ResetCSW);
        break;
    case RegistCsw:
        sendNotification(NavigationControllerConstants::RegistCSW, data);
        break;
    case RequestRecentDestination:
        sendNotification(SearchConstants::RequestRecentDestination, data);
        break;
    case BackwardClicked:
//        if (!data.isNull()) {
//            QString caller = data.toString();
//            if (!caller.isEmpty()) {
//                T_DEBUGV("SearchTabViewMediator::onEvent(BackwardClicked) : %s", data.toString().toStdString().c_str());
//                QString caller = QQmlProperty::read(mViewComponent,CommonConstants::Caller).toString();
//                if (caller == ViewRegistry::objectName(ViewRegistry::AddressSearchView))
//                    caller = ViewRegistry::objectName(ViewRegistry::RouteSettingView);
//                sendNotification(Constants::ShowView,QVariantMap{{CommonConstants::ObjectName,caller}});
//            }
//        }
        break;
    case SelectZone1TextInput:
        sendNotification(ZoneConstants::SelectZone1TextInput);
        break;
    case CloseView:
        // Deactivate 이벤트가 발생하지 않으므로 CSW처리는 이곳에서 함
        sendNotification(NavigationControllerConstants::ResetCSW);
        sendNotification(Constants::CloseView, data);
        break;
    case ItemClicked:
        {
            auto m = data.toMap();
            if (m.contains(CommonConstants::Favorite) && m[CommonConstants::Favorite].toBool()) {
                auto owner = m[CommonConstants::Owner].toString();
                auto dest = m[SearchConstants::Dest].toString();
                if (dest.isEmpty())
                    m[SearchConstants::Dest] = m[CommonConstants::Address];

                BookmarkHelper::setAddFavoriteDestType(owner, m);

                sendNotification(SearchConstants::AddFavorite, m);
                sendNotification(SearchConstants::UpdateBookmarkPageView);
            } else if (m.contains(SearchConstants::ViaSetting) && m[SearchConstants::ViaSetting].toBool()) {
                // 경유지 추가.
                auto caller = QQmlProperty::read(mViewComponent, CommonConstants::Caller).toString();
                sendNotification(RouteConstants::AddVia, m);
                sendNotification(Constants::ShowView, QVariantMap{{CommonConstants::ObjectName, caller}});
            }
            QMetaObject::invokeMethod(mViewComponent, "close");
        }
        break;
    case QueryInDriveMode:
        mStartRouteGuidanceMap = data.toMap();
        sendNotification(MapConstants::RequestDriveMode);
        break;
    case ChangeSearchTabViewPage:
        sendNotification(SearchConstants::ChangeSearchTabViewPage, data);
        break;
    case HideKeyboard:
        sendNotification(KeyboardConstants::HideKeyboard, data);
        break;
    case ModalChanged:
        sendNotification(ZoneConstants::UpdateModal,data.toMap());
        break;

    }

    return false;
}

bool TestRecentDestinationPageViewMediator::handleNotification( const mvc::INotificationPtr& note )
{
    switch ( note->getId() ) {
    case SearchConstants::ApplyTabPageActivation:
        {
            auto m = note->getBody().toMap();
            auto page = m[SearchConstants::Page].toInt();
            T_DEBUGV( "ApplyTabPageActivation #1 page: %d", page );
            QQmlProperty::write(mViewComponent, CommonConstants::Visible, page == 1);
        }
        break;
    case SearchConstants::ResponseRecentDestination:
    {
        auto body = note->getBody().value<QVariantMap>();
        auto reply = body[QStringLiteral("data")].value<QList<RecentDestinationDtoPtr> >();
        QVariantMap r;
        int i = 0;
        for ( RecentDestinationDtoPtr dto : reply )
        {
            QVariantList v;
            uint arrivalTime = 0; // TODO : 1. reqeust rp to arrival time from MNS, 2. POI
            v << dto->id() << dto->dest() << dto->address() << dto->destType() << dto->mapX() << dto->mapY() << dto->hit() << arrivalTime;
            r.insert(QString::number(i),QVariant::fromValue(v));
            i++;
        }

        T_DEBUG("Responsed to recent dest: "+QString::number(i));
        QMetaObject::invokeMethod(mViewComponent,"pushRecentDestList",Q_ARG(QVariant,QVariant::fromValue(r)),Q_ARG(QVariant,i));
    }
    break;
    case RouteConstants::UpdateSummary:
    {
        auto summary = note->getBody().value<QVariantMap>();
        auto reqData = summary[SearchConstants::RegData].value<QVariantMap>();
        auto mapX    = reqData[CommonConstants::X].toInt();
        auto mapY    = reqData[CommonConstants::Y].toInt();

        int distance    = summary[SearchConstants::Dist].toInt();
        int spentTime   = summary[SearchConstants::Time].toInt();
        QVariantList r;
        r << distance << spentTime;

        QMetaObject::invokeMethod(mViewComponent,"updateRouteSimpleInfo",
                Q_ARG(QVariant,QVariant::fromValue(mapX)),
                Q_ARG(QVariant,QVariant::fromValue(mapY)),
                Q_ARG(QVariant,QVariant::fromValue(r))
                );
    }
    break;
    case ZoneConstants::DoSoftKeyFunction:{
            T_DEBUGV( "#1" );
            auto objectName = QQmlProperty::read(mViewComponent, CommonConstants::ObjectName).toString();            
            auto data = note->getBody().value<QVariantMap>();
            auto objName = data[CommonConstants::Name].toString();
            T_DEBUG( QString("#1.1 v %1 n %2").arg( objectName ).arg( objName ) );
            if (data.contains(CommonConstants::Name)) {
                auto objName = data[CommonConstants::Name].toString();
                if (objName != objectName)
                    return false;
            }

            QString objFunction = CommonConstants::Empty;
            QString objMenu = CommonConstants::Empty;
            if (data.contains(ZoneConstants::Function))
                objFunction = data[ZoneConstants::Function].toString();
            if (data.contains(ZoneConstants::Menu) )
                objMenu = data[ZoneConstants::Menu].toString();

            auto caller = QQmlProperty::read(mViewComponent, "caller").toString();
            auto owner = QQmlProperty::read(mViewComponent, "owner").toString();

            // 17.10.27 hskim up to this line, it is the common part of old search tab view and recent desti...
            // here it is divided into two branches.

            auto isRegFavoriteMode = QQmlProperty::read( mViewComponent, QStringLiteral("isRegFavoriteMode") ).toBool();
            auto isViaSettingMode = QQmlProperty::read( mViewComponent, QStringLiteral("isViaSettingMode") ).toBool();

            // 17.10.27 hskim old search tab view branch
            if ( !isRegFavoriteMode && !isViaSettingMode )
            {
                T_DEBUGV( "#2" );
                QMetaObject::invokeMethod(mViewComponent, "__doSoftKeyFunction", Q_ARG(QVariant, QVariant::fromValue(objFunction)), Q_ARG(QVariant, QVariant::fromValue(objMenu)) );
            }
            // 17.10.27 hskim old recent desti... branch
            else {
                T_DEBUGV( "#3" );
                // Edit 모드에서 뒤로가기 눌렀을 때의 처리.
                if (ZoneConstants::Back == objFunction) {
                    T_DEBUGV( "#4" );
                    auto caller = QQmlProperty::read(mViewComponent, CommonConstants::Caller).toString();
                    if (caller == ViewRegistry::objectName(ViewRegistry::RouteSettingView)) {

                    } else if (caller == ViewRegistry::objectName(ViewRegistry::NaviMenuView)) {
                        auto url = ViewRegistry::url(ViewRegistry::NaviMenuView);
                        sendNotification(Constants::CreateView, QVariantMap{{CommonConstants::Url, url}});
                    }

                    T_DEBUGV( "#4" );
                    //sendNotification(SearchConstants::ReturnToFavorite);
                    QMetaObject::invokeMethod(mViewComponent, "close");
                } else {
                    T_DEBUGV( "#5" );
                    QMetaObject::invokeMethod(mViewComponent, "__doSoftKeyFunction", Q_ARG(QVariant, QVariant::fromValue(objFunction)), Q_ARG(QVariant, QVariant::fromValue(objMenu)) );
                }
            }
        }
        break;
    case MapConstants::ResponseDriveMode:
        {
            if (mIsActivate) {
                auto driveMode = note->getBody().toInt();
                if (driveMode == MapViewMode::Navi) {
                    auto address = mStartRouteGuidanceMap[CommonConstants::Name].toString();
                    if (address.isEmpty())
                        address = mStartRouteGuidanceMap[CommonConstants::Address].toString();

                    showChangeDestinationConfirmDialog(address);
                } else {
                    startRouteGuidance();
                    QMetaObject::invokeMethod(mViewComponent, "closeView");

                    auto owner = QQmlProperty::read(mViewComponent, CommonConstants::Owner).toString();
                    if (owner == ViewRegistry::objectName(ViewRegistry::SearchResultView)) {
                        sendNotification(Constants::CloseView, QVariantMap{{CommonConstants::ObjectName, owner}});
                    }
                }
            }
        }
        break;
    default:
        return false;
    }
    return false;
}
