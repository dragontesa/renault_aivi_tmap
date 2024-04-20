#include "TestSearchTabViewMediator.h"
#include "Constants.h"
#include "CommonConstants.h"
#include "SearchConstants.h"
#include "NavigationControllerConstants.h"
#include "ViewRegistry.h"

using namespace SKT;

TestSearchTabViewMediator::TestSearchTabViewMediator( QObject* viewComponent )
    : AbstractViewMediator( viewComponent )
{
}

QList<int> TestSearchTabViewMediator::listNotificationInterests()
{
    static QList<int> notes = {
        SearchConstants::ChangeSearchTabViewPage,
        ZoneConstants::FocusActivatedZone1TextInput
    };

    return notes;
}

bool TestSearchTabViewMediator::onEvent( int event, const QVariant& data )
{
    //T_DEBUGV( "event: %d", event );

    switch ( event ) {
    case Mediated:
        {
            int a = 0;
        }
        break;
    case CreateView:
        {
            auto m = data.toMap();
            auto viewId = m[QStringLiteral("viewId")].toInt();
            auto parent = m[QStringLiteral("parent")].value<QObject*>();

            sendNotification(Constants::CreateView,
                QVariantMap{
                    {CommonConstants::Url,ViewRegistry::url(viewId)},
                    {CommonConstants::Parent,QVariant::fromValue(parent)},
                    {CommonConstants::Visible,false}
                }
            );
        }
        break;
    case CloseView:
        {
            auto m = data.toMap();
            auto viewId = m[QStringLiteral("viewId")].toInt();
            sendNotification(Constants::CloseView,
                    QVariantMap{{CommonConstants::ObjectName,ViewRegistry::objectName(viewId)}});
        }
        break;
    case ResetCsw:
        sendNotification(NavigationControllerConstants::ResetCSW);
        break;
    case RegistCsw:
        sendNotification(NavigationControllerConstants::RegistCSW, data);
        break;
    case TabPageActivated:
        sendNotification(SearchConstants::ApplyTabPageActivation, data);
        break;
    }

    return true;
}

bool TestSearchTabViewMediator::handleNotification( const mvc::INotificationPtr& note )
{
    switch ( note->getId() ) {
    case SearchConstants::ChangeSearchTabViewPage:
        {
            QQmlProperty::write(mViewComponent, SearchConstants::Page, note->getBody());
        }
        break;
    case ZoneConstants::FocusActivatedZone1TextInput:
        {
            //T_DEBUGV( "FocusActivatedZone1TextInput #1" );
            QMetaObject::invokeMethod(mViewComponent, "__focusActivatedZone1TextInput");
        }
        break;
    }

    return true;
}
