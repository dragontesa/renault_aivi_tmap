#include "FullscreenAlertViewMediator.h"
#include "Constants.h"
#include "ZoneConstants.h"
#include "ViewRegistry.h"
#include "NavigationControllerConstants.h"
#include "DebugUtils.h"
#include "MapConstants.h"
#include "mediator/app/MapViewMode.h"
#include "RgConstants.h"

using namespace SKT;

// FullscreenAlertActivity.java

FullscreenAlertViewMediator::FullscreenAlertViewMediator(QObject *viewComponent)
    : AbstractViewMediator(viewComponent)
{

}

QList<int> FullscreenAlertViewMediator::listNotificationInterests()
{
    static QList<int> interests = {
        MapConstants::ApplyMapViewMode,
        MapConstants::ChangeMapViewMode,
        ZoneConstants::SelectZone1TextInput,
        ZoneConstants::SelectZone1Title,
        ZoneConstants::FocusZone1TextInput,
        NavigationControllerConstants::GoToRunning,
        NavigationControllerConstants::GoToStandby,
        RgConstants::UpdateSdiState
    };

    return interests;
}

bool FullscreenAlertViewMediator::onEvent(int event, const QVariant &data)
{
    switch (event) {
    case Mediated: {
            sendNotification(MapConstants::NotifyMapViewMode);
            sendNotification( RgConstants::RequestSdiStateUpdate );
        }
        break;
    default:
        return false;
    }
    return true;
}

bool FullscreenAlertViewMediator::handleNotification(const mvc::INotificationPtr &note)
{
    switch (note->getId()) {
    case MapConstants::ChangeMapViewMode:
    case MapConstants::ApplyMapViewMode: {
            auto data = note->getBody().value<QVariantMap>();
            auto mode = (MapViewMode::ModeK)data[CommonConstants::Mode].toInt();
            auto visible = ( mode == MapViewMode::Normal || mode == MapViewMode::Navi || mode == MapViewMode::MenuNavi );
            if ( visible == false ) {                
                QQmlProperty::write( mViewComponent, QStringLiteral("visible"), false );
            }
            else {
                if ( mAlert ) {
                    QQmlProperty::write( mViewComponent, QStringLiteral("visible"), true );
                }
            }
        }
        break;
    case NavigationControllerConstants::GoToRunning:
        QQmlProperty::write( mViewComponent, QStringLiteral("appRunning"), true );
        break;
    case NavigationControllerConstants::GoToStandby:
        QQmlProperty::write( mViewComponent, QStringLiteral("appRunning"), false );
        break;
    case RgConstants::UpdateSdiState:
        {
            auto m = note->getBody().toMap();
            auto item = m["item"].toInt();
            switch ( item ) {
            case RgConstants::SdiAlert:
                {
                    mAlert = m["value"].toBool();
                }
                break;
            case RgConstants::SdiReset:
                {
                    mAlert = false;
                }
                break;
            }
        }
        break;
    }

    return false;
}
