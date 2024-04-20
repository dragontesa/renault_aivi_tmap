#include "NoResultPageViewMediator.h"

#include <QQmlProperty>

#include "Constants.h"
#include "CommonConstants.h"
#include "ZoneConstants.h"
#include "KeyboardConstants.h"
#include "NavigationControllerConstants.h"
#include "SearchConstants.h"
#include "RouteConstants.h"
#include "MapConstants.h"
#include "ViewRegistry.h"
#include "FavoriteDto.h"
#include "NavigationPropertyProtocol.h"
#include "rprequest.h"
#include "NavigationPropertyConstants.h"
#include "mediator/app/MapViewMode.h"
#include "LangConstants.h"
#include "NavigationControllerProtocol.h"

using namespace SKT;

NoResultPageViewMediator::NoResultPageViewMediator( QObject* viewComponent )
    : AbstractViewMediator( viewComponent )
{
}

QList<int> NoResultPageViewMediator::listNotificationInterests()
{
    static QList<int> notes = {
        ZoneConstants::DoSoftKeyFunction,
        ZoneConstants::DoSoftkeyIndexMenuFunction,
        KeyboardConstants::ProcessKeyboard,
        SearchConstants::ChangedActiveTabPage,
        SearchConstants::ResponseNoResult,
        NavigationControllerConstants::ChangeLanguage,
        NavigationControllerConstants::ResponseController,
    };
    return notes;
}


bool NoResultPageViewMediator::onEvent( int event, const QVariant& data )
{
    switch ( event ) {
    case Mediated:
        break;
    case Activated: {
        sendNotification(SearchConstants::ChangedTabPageViewActivation,
            QVariantMap{{QStringLiteral("viewId"),ViewRegistry::NoResultPageView},
                {QStringLiteral("active"),true}});
        }
        break;
    case Deactivated:
        sendNotification(SearchConstants::ChangedTabPageViewActivation,
            QVariantMap{{QStringLiteral("viewId"),ViewRegistry::RecentSearchPageView},
                {QStringLiteral("active"),false}});
        break;
    case CreateView:
        sendNotification(Constants::CreateView, data);
        break;
    case CloseView:
        sendNotification(Constants::CloseView, data);
        break;
    case SelectZone1TextInput:
        sendNotification(ZoneConstants::SelectZone1TextInput, data);
        break;
    case FocusZone1TextInput:
        sendNotification(ZoneConstants::FocusZone1TextInput);
        break;
    case ShowKeyboard:
        sendNotification(KeyboardConstants::ShowKeyboard, data);
        break;
    case HideKeyboard:
        sendNotification(KeyboardConstants::HideKeyboard, data);
        break;

    case UpdateSearchResultCount:
        {
            auto query = QQmlProperty::read(mViewComponent, SearchConstants::Query).toString();
            QVariantMap m;
            m[CommonConstants::Text] = query;
            m[CommonConstants::Count] = 0;
            m[CommonConstants::Show] = true;
            m[CommonConstants::Caller] = ViewRegistry::objectName(ViewRegistry::NoResultPageView);
            sendNotification(ZoneConstants::UpdateSearchResultCount, m);
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

    case ChangeSearchTabViewPage:
        sendNotification(SearchConstants::ChangeSearchTabViewPage, data);
        break;

    }

    return false;
}

bool NoResultPageViewMediator::handleNotification( const mvc::INotificationPtr& note )
{
    switch ( note->getId() ) {
    case SearchConstants::ChangedActiveTabPage:
        {
            auto m = note->getBody().toMap();
            auto page = m[SearchConstants::Page].toInt();
            T_DEBUGV( "ApplyTabPageActivation #1 page: %d", page );
            QQmlProperty::write(mViewComponent, CommonConstants::Visible, page == SearchConstants::NoResultPageIndex);
        }
        break;

     case ZoneConstants::DoSoftKeyFunction:
     case ZoneConstants::DoSoftkeyIndexMenuFunction:{
         if (!QQmlProperty::read(mViewComponent, CommonConstants::Visible).toBool())
             break;

         auto data = note->getBody().value<QVariantMap>();
         auto objectName = data[CommonConstants::Name].toString();
         if (objectName != ViewRegistry::objectName(ViewRegistry::NoResultPageView))
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

    case KeyboardConstants::ProcessKeyboard: {
        if (!QQmlProperty::read(mViewComponent, CommonConstants::Visible).toBool())
            break;
         auto args = note->getBody().value<QVariantMap>();
         auto owner = args[CommonConstants::Owner].toString();
         if (owner != ViewRegistry::objectName(ViewRegistry::NoResultPageView) &&
             owner != ViewRegistry::objectName(ViewRegistry::BookmarkPageView))
             return false;
         auto key = args[CommonConstants::Key];
         auto text = args[CommonConstants::Text];
         QMetaObject::invokeMethod(mViewComponent, "__processKeyboard", Q_ARG(QVariant, QVariant::fromValue(key)), Q_ARG(QVariant, QVariant::fromValue(text)) );
     }
     break;

    case KeyboardConstants::PullDownKeyboard:
        QMetaObject::invokeMethod(mViewComponent, "__pullDownKeyboard");
        break;
    case KeyboardConstants::PullUpKeyboard:
        QMetaObject::invokeMethod(mViewComponent, "__pullUpKeyboard");
        break;

    case SearchConstants::ResponseNoResult:
        {
            auto m = note->getBody().toMap();
            auto query = m[SearchConstants::Query].toString();
            QQmlProperty::write(mViewComponent, SearchConstants::Query, query);
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

    default:
        return false;
    }
    return true;
}

