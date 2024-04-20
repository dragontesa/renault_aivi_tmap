#include "RecentDestinationPageViewMediator.h"
#include "Constants.h"
#include "CommonConstants.h"
#include "ZoneConstants.h"
#include "KeyboardConstants.h"
#include "NavigationControllerConstants.h"
#include "SearchConstants.h"
#include "MapConstants.h"
#include "RgConstants.h"
#include "RouteConstants.h"
#include "ViewRegistry.h"
#include "BookmarkHelper.h"
#include "RecentDestinationDto.h"
#include "mediator/app/MapViewMode.h"
#include "LangConstants.h"
#include "PlatformHelper.h"
#include "ApplicationConstants.h"

using namespace SKT;

RecentDestinationPageViewMediator::RecentDestinationPageViewMediator( QObject* viewComponent )
    : AbstractViewMediator( viewComponent )
    , mIsActivate(false)
{
    mEditType = -1;
    mIsEditMode = false;
}

QList<int> RecentDestinationPageViewMediator::listNotificationInterests()
{
    static QList<int> notes = {
            SearchConstants::ResponseRecentDestination,
            SearchConstants::ResponseSearch,
            SearchConstants::ReplyToGadgetSearchMode,
            RouteConstants::UpdateSummary,
            ZoneConstants::DoSoftKeyFunction,
            ZoneConstants::DoSoftkeyIndexMenuFunction,
            RgConstants::ReplyToRouteGuidanceState,
            RgConstants::ChangedRouteGuidanceState,
            SearchConstants::ChangedActiveTabPage,
            NavigationControllerConstants::ChangeLanguage,
            ApplicationConstants::UpdatedUserProfile
    };
    return notes;
}

void RecentDestinationPageViewMediator::showChangeDestinationConfirmDialog(const QString &address)
{
    sendNotification(ApplicationConstants::BlockSearchTabView, true);

    auto x = mStartRouteGuidanceMap[CommonConstants::X].toInt();
    auto y = mStartRouteGuidanceMap[CommonConstants::Y].toInt();
    auto name = mStartRouteGuidanceMap[CommonConstants::Name].toString();
    auto poiId = mStartRouteGuidanceMap[SearchConstants::PoiID].toInt();
    auto navSeq = mStartRouteGuidanceMap[SearchConstants::NavSeq].toInt();
    auto rpFlag = mStartRouteGuidanceMap[SearchConstants::RpFlag].toInt();

    QVariantMap m;

    m[QStringLiteral("url")] = ViewRegistry::url(ViewRegistry::ChangeDestinationView);
    m[QStringLiteral("destX")] = x;
    m[QStringLiteral("destY")] = y;
    m[QStringLiteral("address")] = address;
    m[QStringLiteral("name")] = name;
    m[SearchConstants::PoiID] = poiId;
    m[SearchConstants::NavSeq] = navSeq;
    m[SearchConstants::RpFlag] = rpFlag;
    m[QStringLiteral("view")] = QVariant::fromValue(mViewComponent);
    m[QStringLiteral("okCallback")] = QStringLiteral("closeView");
    PlatformHelper::instance()->setParentForSystemPopup(m);
    sendNotification(Constants::CreateView, m);
}

void RecentDestinationPageViewMediator::close()
{
    auto isViaSettingMode = QQmlProperty::read(mViewComponent, "isViaSettingMode").toBool();
    auto isRegFavoriteMode = QQmlProperty::read(mViewComponent, "isRegFavoriteMode").toBool();
    if (isViaSettingMode || isRegFavoriteMode) {
        QMetaObject::invokeMethod(mViewComponent, "close");
    } else {
        QMetaObject::invokeMethod(mViewComponent, "closeView");
    }
}

void RecentDestinationPageViewMediator::startRouteGuidance()
{
    auto m = mStartRouteGuidanceMap;
    auto x = m[CommonConstants::X].toInt();
    auto y = m[CommonConstants::Y].toInt();
    auto name = m[CommonConstants::Name].toString();

    if (x > 0 && y > 0) {
        sendNotification(SearchConstants::SetGadgetSearchMode, false);

        QVariantMap body;
        body[SearchConstants::Clear] = true;
        sendNotification(RouteConstants::SetDeparture, body);
        body.clear();

        body[CommonConstants::X] = x;
        body[CommonConstants::Y] = y;
        body[CommonConstants::Name] = name;
        body[SearchConstants::PoiID] = m[SearchConstants::PoiID];
        body[SearchConstants::NavSeq] = m[SearchConstants::NavSeq];
        body[SearchConstants::RpFlag] = m[SearchConstants::RpFlag];
        body[SearchConstants::DestType] = RouteConstants::EXPLORER_RECENT_DES;

        sendNotification(RouteConstants::SetArrival, body);

        sendNotification(Constants::CreateView,QVariantMap{ {CommonConstants::Url,ViewRegistry::url(ViewRegistry::RouteCandidateView)} });
        close();
    }
}

bool RecentDestinationPageViewMediator::onEvent( int event, const QVariant& data )
{
    switch (event) {
    case Mediated:
        sendNotification(RgConstants::RequestRouteGuidanceState, getMediatorId());
        break;

    case Activated:
        mIsActivate = true;
        sendNotification(SearchConstants::ChangedTabPageViewActivation,
            QVariantMap{{QStringLiteral("viewId"),ViewRegistry::RecentDestinationPageView},
                {QStringLiteral("active"),true}});
        sendNotification(SearchConstants::ChangeSearchEditMode, false);
        sendNotification(RgConstants::BlockReroute, true);
        break;
    case Deactivated:
        mIsActivate = false;
        sendNotification(SearchConstants::ChangedTabPageViewActivation,
            QVariantMap{{QStringLiteral("viewId"),ViewRegistry::RecentDestinationPageView},
                {QStringLiteral("active"),false}});
        break;
    case HideTabViewContent:
        sendNotification(SearchConstants::HideTabViewContent);
        break;

    case HideView:
        sendNotification(Constants::HideView, data);
        break;

    case SelectSoftkeyIndexMenuMode:
        {
            T_DEBUGV( "SelectSoftkey #1" );
            auto objectName = QQmlProperty::read(mViewComponent, CommonConstants::ObjectName).toString();
            sendNotification(ZoneConstants::RegistSoftkey, objectName);
            sendNotification(ZoneConstants::SelectSoftkeyIndexMenuMode, data);
        }
        break;
    case CreateView:
        {
            auto m = data.toMap();
            auto url = m[CommonConstants::Url].toString();
            if (url == ViewRegistry::url(ViewRegistry::RecentDestinationEditView)) {
                sendNotification(SearchConstants::ChangeSearchEditMode, true);
            }
            sendNotification(Constants::CreateView, data);
        }
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
        sendNotification(SearchConstants::RequestGadgetSearchMode);
        if (mIsGadgetSearchMode) {
            PlatformHelper::instance()->switchToSystem();
            sendNotification(SearchConstants::SetGadgetSearchMode, false);
        }
        break;
    case SelectZone1TextInput:
        sendNotification(ZoneConstants::SelectZone1TextInput);
        break;
    case SelectZone1Title:
        sendNotification(ZoneConstants::SelectZone1Title, data);
        break;
    case CloseView:
        // Deactivate 이벤트가 발생하지 않으므로 CSW처리는 이곳에서 함
        sendNotification(NavigationControllerConstants::ResetCSW);
        if (mIsGadgetSearchMode) {
            sendNotification(NavigationControllerConstants::ReserveSystemSwitchJob, QVariantMap{
                                 {CommonConstants::Command, Constants::CloseView},
                                 {CommonConstants::Arg, data}
                             });
        } else {
            sendNotification(Constants::CloseView, data);
        }
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

                auto caller = m[CommonConstants::Caller].toString();
                if (caller == "NaviMenuView") {
                    // 네비메뉴에서 집으로,회사로 등록 시 최근 목적지를 선택하면 RecentDestinationPageView
                    // 만 standalone으로 생성된 상태이므로 즐겨찾기 화면으로 이동 하기 위해서는
                    // SearchTabView를 생성해야 한다.
                    // 17.12.04 ymhong
                    QVariantMap m {
                        {CommonConstants::Url, ViewRegistry::url(ViewRegistry::NaviMenuView)}
                    };
                    sendNotification(Constants::CreateView, m);
                } else {
                    sendNotification(ApplicationConstants::BlockSearchTabView, false);
                    sendNotification(SearchConstants::ReturnToFavorite);
                }
                sendNotification(SearchConstants::UpdateBookmarkPageView);
            } else if (m.contains(SearchConstants::ViaSetting) && m[SearchConstants::ViaSetting].toBool()) {
                // 경유지 추가.
                auto caller = QQmlProperty::read(mViewComponent, CommonConstants::Caller).toString();
                sendNotification(RouteConstants::AddVia, m);
                sendNotification(Constants::ShowView, QVariantMap{{CommonConstants::ObjectName, caller}});
            }
            close();
        }
        break;

    case ItemPressed:
        sendNotification(ApplicationConstants::BlockSearchTabView, true);
        break;

    case QueryInDriveMode:
        mStartRouteGuidanceMap = data.toMap();
        if (mIsActivate) {
            auto block = mIsRg;
            sendNotification(ApplicationConstants::BlockSearchTabView, block);

            if (mIsRg) {
                auto address = mStartRouteGuidanceMap[CommonConstants::Address].toString();

                showChangeDestinationConfirmDialog(address);
            } else {
                startRouteGuidance();
                close();

                auto owner = QQmlProperty::read(mViewComponent, CommonConstants::Owner).toString();
                if (owner == ViewRegistry::objectName(ViewRegistry::SearchResultView)) {
                    sendNotification(Constants::CloseView, QVariantMap{{CommonConstants::ObjectName, owner}});
                }
            }
        }
        break;

    case ListScrolled:
        sendNotification(ApplicationConstants::BlockSearchTabView, false);
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
     case ShowZone3:
         //sendNotification(ZoneConstants::ShowZone3, ZoneConstants::Animation);
         break;
     case HideZone3:
         //sendNotification(ZoneConstants::HideZone3, ZoneConstants::Animation);
         break;
    case FocusCsw:
        sendNotification(NavigationControllerConstants::SelectCSWByObjectName, data);
        break;

    case UpdateView:
        sendNotification(SearchConstants::UpdateView, data);
        break;
    }

    return false;
}

bool RecentDestinationPageViewMediator::handleNotification( const mvc::INotificationPtr& note )
{
    switch ( note->getId() ) {
    case SearchConstants::ChangedActiveTabPage:
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
        auto viewOwner = QQmlProperty::read(mViewComponent, CommonConstants::Owner).toString();
        auto notiOwner = body[CommonConstants::Owner].toString();

        if (viewOwner != notiOwner) break; // must checkout noti's owner if one in the current view is same, it must need on multiple caller by 2012.12.28  lckxx
        QVariantMap r;
        int i = 0;
        for ( RecentDestinationDtoPtr dto : reply )
        {
            QVariantList v;
            uint arrivalTime = 0; // TODO : 1. reqeust rp to arrival time from MNS, 2. POI
            v << dto->id()
              << dto->dest()
              << dto->address()
              << dto->destType()
              << dto->mapX()
              << dto->mapY()
              << dto->hit()
              << dto->poiID()
              << arrivalTime
              << dto->navSeq()
              << dto->rpFlag()              
              << dto->opiPoleCate();
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
    case ZoneConstants::DoSoftKeyFunction:
    case ZoneConstants::DoSoftkeyIndexMenuFunction:{
        // Donot follow this notification if the view is hidden(prevent it might be bug), by lckxx 2017.11.14
        if (!QQmlProperty::read(mViewComponent, CommonConstants::Visible).toBool()) break;

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
                int  objIndex  = -1;
                if (data.contains(ZoneConstants::Index))
                    objIndex = data[ZoneConstants::Index].toInt();
                QMetaObject::invokeMethod(mViewComponent, "__doSoftKeyFunction", Q_ARG(QVariant, QVariant::fromValue(objFunction)), Q_ARG(QVariant, QVariant::fromValue(objMenu)), Q_ARG(QVariant, QVariant::fromValue(objIndex)) );
            }
            // 17.10.27 hskim old recent desti... branch
            else {
                if (ZoneConstants::Back == objFunction) {

                    auto caller = QQmlProperty::read(mViewComponent, CommonConstants::Caller).toString();

                    if (caller == ViewRegistry::objectName(ViewRegistry::RouteSettingView)) {
                        sendNotification(Constants::ShowView, QVariantMap{{CommonConstants::ObjectName, caller}});
                    } else if (caller == ViewRegistry::objectName(ViewRegistry::NaviMenuView)) {
                        auto url = ViewRegistry::url(ViewRegistry::NaviMenuView);
                        sendNotification(Constants::CreateView, QVariantMap{{CommonConstants::Url, url}});
                    } else if (caller == ViewRegistry::objectName(ViewRegistry::BookmarkPageView)) {
                        sendNotification(SearchConstants::ReturnToFavorite);
                    }

                    close();
                } else {
                    int  objIndex  = -1;
                    if (data.contains(ZoneConstants::Index))
                        objIndex = data[ZoneConstants::Index].toInt();
                    QMetaObject::invokeMethod(mViewComponent, "__doSoftKeyFunction", Q_ARG(QVariant, QVariant::fromValue(objFunction)), Q_ARG(QVariant, QVariant::fromValue(objMenu)), Q_ARG(QVariant, QVariant::fromValue(objIndex)) );
                }
            }
        }
        break;
    case RgConstants::ReplyToRouteGuidanceState:
    case RgConstants::ChangedRouteGuidanceState:
        {
            auto m = note->getBody().toMap();
            mIsRg = m["isRg"].toBool();
        }
        break;
    case NavigationControllerConstants::ChangeLanguage: {
        if (mIsActivate)
            QMetaObject::invokeMethod(mViewComponent, "retranslateUi");
    }
        break;
    case ApplicationConstants::UpdatedUserProfile: {
      // reload view data
      QMetaObject::invokeMethod(mViewComponent, "resetRecentDestList");
      QMetaObject::invokeMethod(mViewComponent, "requestViewData", Q_ARG(QVariant, 0));
    }
      break;

    case SearchConstants::ReplyToGadgetSearchMode:
        mIsGadgetSearchMode = note->getBody().toBool();
        break;

    default:
        return false;
    }
    return false;
}
