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
#include "rprequest.h"
#include "NavigationPropertyProtocol.h"
#include "NavigationPropertyConstants.h"
#include "mediator/rg/RgUtils.h"
#include "DebugUtils.h"
#include "ApplicationConstants.h"

using namespace SKT;

void MapViewMediator::handleApplication( const mvc::INotificationPtr& note )
{
    switch ( note->getId() ) {
    case ApplicationConstants::ChangeToApp:
        {
            qDebug() << "[MAP] ApplicationConstants::ChangeToApp";            
            QMetaObject::invokeMethod(mViewComponent, "setMapForApp");
        }
        break;

    case ApplicationConstants::ChangeToWidget:
        {
            qDebug() << "[MAP] ApplicationConstants::ChangeToWidget";
            // make rg popups invisible
            QMetaObject::invokeMethod(mViewComponent, "setMapForWidget", Q_ARG(QVariant, note->getBody()));
//            sendNotification(MapConstants::ShowCarvata, true);
//            sendNotification(MapConstants::EnableCallout, true);
//            sendNotification(MapConstants::ShowRoute, true);
            sendNotification(MapConstants::RestoreMapViewMode,QVariantMap{{QStringLiteral("reason"),MapViewMode::MapViewActivated}});

//            QVariantMap body;
//            body[CommonConstants::Section] = SettingConstants::DirectionGuid;
//            body[CommonConstants::Key] = SettingConstants::MapMode;
//            sendNotification( SettingConstants::RequestSetting, body );
        }
        break;
    case ApplicationConstants::ReplyToViewList:
        {
            auto data = note->getBody().toMap();
            if (!data.contains(CommonConstants::ObjectName)) {
                break;
            }
            QString objectName = QQmlProperty::read(mViewComponent, CommonConstants::ObjectName).toString();
            QString requestObjectName = data[CommonConstants::ObjectName].value<QString>();
            if (requestObjectName == ViewRegistry::objectName(ViewRegistry::SimulationView)  ||
                requestObjectName == ViewRegistry::objectName(ViewRegistry::RouteCandidateView) ||
                requestObjectName == objectName) {
                mViewList = data[ApplicationConstants::ViewIdList].value<QList<int>>();
                mViewVisibleMap.clear();
                auto viewDataMap = data[ApplicationConstants::ViewDataMap].value<QMap<int,QVariantMap>>();
                for (auto viewId: mViewList) {
                    auto viewVisible = viewDataMap[viewId][ApplicationConstants::Visible].toBool();
                    mViewVisibleMap[viewId] = viewVisible;
                }

                //cjlee 2019.03.18
                if (data.contains(CommonConstants::Relay)) {
                    QString relay = data[CommonConstants::Relay].value<QString>();
                    if (relay == "NavigationControllerProxy") {
                        int viewId = data[CommonConstants::ViewId].value<int>();
                        QVariantMap body;
                        body.insert( CommonConstants::Value, QVariant::fromValue(mViewList) );
                        body.insert( CommonConstants::ViewId, viewId);
                        sendNotification(NavigationControllerConstants::ReplyToViewList, body);
                    }
                }
            }
        }
        break;

    case ApplicationConstants::ActivatedView: {
//            T_DEBUG( "ActivatedView #1" );
//            auto m = note->getBody().value<QVariantMap>();
//            auto objName = m[CommonConstants::ObjectName];
//            if ( objName == ViewRegistry::objectName( ViewRegistry::NaviMenuView ) ) {
//                T_DEBUG( "ActivatedView #1.1" );
//                mNaviMenuViewActivated = true;
//            }
//            else if ( objName == ViewRegistry::objectName( ViewRegistry::SettingMainView ) ) {
//                T_DEBUG( "ActivatedView #1.2" );
//                mSettingMainViewActivated = true;
//            }
//            if ( mNaviMenuViewActivated || mSettingMainViewActivated ) {
//                T_DEBUG( "ActivatedView #1.3" );
//                QMetaObject::invokeMethod(mViewComponent, "setMenuViewActivated");
//                restoreDriveMapModeAndLevel();
//                sendNotification(MapConstants::ShowCarvata, true);
//            }
        }
        break;
    case ApplicationConstants::DeactivatedView: {
//            //T_DEBUG( "DeactivatedView #1" );
//            auto m = note->getBody().value<QVariantMap>();
//            auto objName = m[CommonConstants::ObjectName];
//            if ( objName == ViewRegistry::objectName( ViewRegistry::NaviMenuView ) ) {
//                mNaviMenuViewActivated = false;
//            }
//            else if ( objName == ViewRegistry::objectName( ViewRegistry::SettingMainView ) ) {
//                mSettingMainViewActivated = false;
//            }
        }
        break;
    case ApplicationConstants::UpdateLastSesstionId:
        mLastSessionId = note->getBody().toString();
        break;
    }
}
