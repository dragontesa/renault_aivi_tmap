#include "TestRecentSearchPageViewMediator.h"
#include "Constants.h"
#include "CommonConstants.h"
#include "ZoneConstants.h"
#include "KeyboardConstants.h"
#include "NavigationControllerConstants.h"
#include "SearchConstants.h"
#include "MapConstants.h"
#include "RouteConstants.h"
#include "ViewRegistry.h"
#include "RecentSearchDto.h"
#include "mediator/app/MapViewMode.h"

using namespace SKT;

TestRecentSearchPageViewMediator::TestRecentSearchPageViewMediator( QObject* viewComponent )
    : AbstractViewMediator( viewComponent )
{
    mIsActivate = false;
}

QList<int> TestRecentSearchPageViewMediator::listNotificationInterests()
{
    static QList<int> notes = {
        SearchConstants::ResponseRecentSearch,
        ZoneConstants::DoSoftKeyFunction,
        MapConstants::ResponseDriveMode,
        KeyboardConstants::ProcessKeyboard,
        SearchConstants::ApplyTabPageActivation,
    };
    return notes;
}

void TestRecentSearchPageViewMediator::showChangeDestinationConfirmDialog(const QString &address)
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

void TestRecentSearchPageViewMediator::startRouteGuidance()
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

void TestRecentSearchPageViewMediator::close()
{
    QMetaObject::invokeMethod(mViewComponent, "close");
}

bool TestRecentSearchPageViewMediator::onEvent( int event, const QVariant& data )
{
    switch (event) {
    case Activated:
        mIsActivate = true;
        break;
    case Deactivated:
        mIsActivate = false;
        break;
    case BackwardClicked:
//        T_DEBUGV("BackwardClicked #1");
//        if (!data.isNull()) {
//            T_DEBUGV("BackwardClicked #2");
//            QString caller = data.toString();
//            T_DEBUG( QString( "BackwardClicked #3 %1" ).arg( caller ) );
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
    case FocusZone1TextInput:
        sendNotification(ZoneConstants::FocusZone1TextInput);
        break;
        // check
//    case ActivatedForNavi:
//        {
//            auto objectName = QQmlProperty::read(mViewComponent, CommonConstants::ObjectName).toString();
//            sendNotification(ZoneConstants::RegistSoftkey, objectName);
//            sendNotification(ZoneConstants::SelectSoftkeyNaviMode);
//        }
//        break;
    case CreateView:
        sendNotification(Constants::CreateView, data);
        break;
    case CloseView:
        // Deactivate 이벤트가 발생하지 않으므로 CSW처리는 이곳에서 함
        sendNotification(NavigationControllerConstants::ResetCSW);
        sendNotification(Constants::CloseView, data);
        break;
    case ShowKeyboard:
        sendNotification(KeyboardConstants::ShowKeyboard, data);
        break;
    case HideKeyboard:
        sendNotification(KeyboardConstants::HideKeyboard, data);
        break;
    case ResetCsw:
        sendNotification(NavigationControllerConstants::ResetCSW);
        break;
    case RegistCsw:
        sendNotification(NavigationControllerConstants::RegistCSW, data);
        break;
    case RequestRecentSearch:
       sendNotification(SearchConstants::RequestRecentSearch, data);
        break;
    case ChangeSearchTabViewPage:
        sendNotification(SearchConstants::ChangeSearchTabViewPage, data);
        break;
    case QueryInDriveMode://v
        mStartRouteGuidanceMap = data.toMap();
        sendNotification(MapConstants::RequestDriveMode);
        break;
    }

    return false;
}

bool TestRecentSearchPageViewMediator::handleNotification( const mvc::INotificationPtr& note )
{
    switch ( note->getId() ) {
    case SearchConstants::ApplyTabPageActivation:
        {
            auto m = note->getBody().toMap();
            auto page = m[SearchConstants::Page].toInt();
            T_DEBUGV( "ApplyTabPageActivation #1 page: %d", page );
            QQmlProperty::write(mViewComponent, CommonConstants::Visible, page == 0);
        }
        break;
    case SearchConstants::ResponseRecentSearch:
    {
        auto data = note->getBody().value<QList<RecentSearchDtoPtr> >();
        QVariantMap r;
        int i = 0;
        for ( RecentSearchDtoPtr dto : data )
        {
            QVariantList v;
            uint arrivalTime = 0; // TODO : reqeust rp to arrival time from MNS
            v << dto->id() << dto->keyword();
            r.insert(QString::number(i),QVariant::fromValue(v));
            i++;
        }

        //T_DEBUG("Responsed to recent search: "+QString::number(i));
        QMetaObject::invokeMethod(mViewComponent,"pushRecentSearchList",Q_ARG(QVariant,QVariant::fromValue(r)),Q_ARG(QVariant,i));
    }
        break;
    case ZoneConstants::DoSoftKeyFunction:{
            auto objectName = QQmlProperty::read(mViewComponent, CommonConstants::ObjectName).toString();
            auto data = note->getBody().value<QVariantMap>();
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
            QMetaObject::invokeMethod(mViewComponent, "__doSoftKeyFunction", Q_ARG(QVariant, QVariant::fromValue(objFunction)), Q_ARG(QVariant, QVariant::fromValue(objMenu)) );
        }
        break;

    case KeyboardConstants::ProcessKeyboard: {
         auto args = note->getBody().value<QVariantMap>();
         auto owner = args[CommonConstants::Owner].toString();
         auto objectName = QQmlProperty::read(mViewComponent, CommonConstants::ObjectName).toString();

         auto bookmarkPageView = ViewRegistry::objectName(ViewRegistry::BookmarkPageView);
         if (owner != objectName && owner != bookmarkPageView)
             return false;
         auto key = args[CommonConstants::Key];
         auto text = args[CommonConstants::Text];
         QMetaObject::invokeMethod(mViewComponent, "__processKeyboard", Q_ARG(QVariant, QVariant::fromValue(key)), Q_ARG(QVariant, QVariant::fromValue(text)) );
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
    return true;
}
