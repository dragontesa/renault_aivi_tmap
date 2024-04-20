#include "MapViewMediator.h"
#include "Constants.h"
#include "RgConstants.h"
#include "RouteConstants.h"
#include "MapConstants.h"
#include "ZoneConstants.h"
#include "SimulatorConstants.h"
#include "SettingConstants.h"
#include "RgProtocol.h"
#include "mediator/app/MapViewMode.h"
#include "ResourceResolver.h"
#include "QmlAppHelper.h"
#include <QWindow>
#include <QMouseEvent>
#include <QQuickItem>
#include "RouteManager.h"
#include "StringHelper.h"
#include "SimulationViewMediator.h"
#include "ViewRegistry.h"
#include "SettingDto.h"
#include "PreferenceVo.h"
#include "PreferenceConstants.h"
#include "SoundConstants.h"
#include "NavigationControllerConstants.h"
#include "NavigationControllerProtocol.h"
#include "rprequest.h"
#include "NavigationPropertyProtocol.h"
#include "NavigationPropertyConstants.h"
#include "mediator/rg/RgUtils.h"
#include "DebugUtils.h"
#include "ApplicationConstants.h"

using namespace SKT;

void MapViewMediator::handleController( const mvc::INotificationPtr& note )
{
    switch ( note->getId() ) {
    case NavigationControllerConstants::ApplyController:
        {

        }
        break;
    case NavigationControllerConstants::UpdateSkyState:
        {
            auto m = note->getBody().toMap();
            auto skyState = m["skyState"].toInt();
            QQmlProperty::write(mViewComponent, "skyState", skyState);

            auto nightMode = m["nightMode"].toBool();
            QQmlProperty::write(mViewComponent, "nightMode", nightMode);
        }
        break;
    case NavigationControllerConstants::ChangeOnlineStatus:
        {
            auto isOnline = note->getBody().toBool();
            QQmlProperty::write(mViewComponent, "isOnline", isOnline);
            mIsOnline = isOnline;
        }
        break;

    case NavigationControllerConstants::ShowTrafficInfo:
        {
            auto show = note->getBody().toBool();
            QQmlProperty::write(mViewComponent, "showTrafficInfo", show);
        }
        break;
    case NavigationControllerConstants::GoToStandby: {
            QMetaObject::invokeMethod(mViewComponent, "pause");
        }
        break;

    case NavigationControllerConstants::GoToRunning: {
            QMetaObject::invokeMethod(mViewComponent, "resume");
        }
        break;

    case NavigationControllerConstants::PauseMapView: {
            auto b = note->getBody().toBool();
            qDebug() << "NavigationControllerConstants::PauseMapView : " << b;

            if (b) {
                QMetaObject::invokeMethod(mViewComponent, "pause");
            } else {
                QMetaObject::invokeMethod(mViewComponent, "resume");
            }
        }
        break;

    case NavigationControllerConstants::ResponseController:
        {
            static int volume = SettingConstants::DefaultVolume;
            auto m = note->getBody().toMap();
            if (m.contains(SettingConstants::NaviMute)) {                                 
                 bool mute = m[SettingConstants::NaviMute].toBool();
                 if (volume <= 0)
                     mute = true;
                 QQmlProperty::write( mViewComponent, "mute", mute );
            } else if (m.contains(SettingConstants::NaviVolume)) {
                volume = m[SettingConstants::NaviVolume].toInt();
                if (volume <= 0) {
                    QQmlProperty::write( mViewComponent, "mute", true );
                }
            }
        }
        break;

    case NavigationControllerConstants::ChangeLanguage:
        updateLanguage();
        break;
    }
}
