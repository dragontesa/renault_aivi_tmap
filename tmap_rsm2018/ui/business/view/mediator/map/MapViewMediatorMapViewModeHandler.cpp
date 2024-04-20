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

void MapViewMediator::handleMapViewMode( const mvc::INotificationPtr& note )
{
    switch ( note->getId() ) {
    case MapConstants::ApplyMapViewMode:
        {
            QMetaObject::invokeMethod( mViewComponent, "closeMapModeDlg" );

            auto data = note->getBody().value<QVariantMap>();
            mMapViewMode = (MapViewMode::ModeK)data[CommonConstants::Mode].toInt();
            mMapViewModeReason = (MapViewMode::ReasonK)data[QStringLiteral("reason")].toInt();
            auto lastMapViewDriveMode = (MapViewMode::ModeK)data[QStringLiteral("lastDriveMode")].toInt();

//            T_DEBUG( QString("ApplyMapViewMode mode: %1 reason: %2")
//                .arg( MapViewMode::modeAsString( mCurMapViewMode ) )
//                .arg( MapViewMode::reasonAsString( mCurMapViewModeReason ) )
//            );

            switch( mMapViewMode ) {
            case MapViewMode::Normal:
                {
                    QString srcName = DebugUtils::name( note->getSource() );
                    qDebug() << "[GlobalMap]MapViewMode::Normal source: " << srcName;

                    sendNotification(MapConstants::ShowCarvata, true);
                    QMetaObject::invokeMethod( mViewComponent, "setNormalMode", Q_ARG(QVariant,mMapViewModeReason) );
                    restoreDriveMapModeAndLevel();
                    if ( mMapViewModeReason == MapViewMode::CancelRoute ) {
                        sendNotification(ZoneConstants::UpdateSoftkeyAddress, mRendererAreaName);
                    }
                }
                break;
            case MapViewMode::MapControl:
                {
                    QString srcName = DebugUtils::name( note->getSource() );
                    qDebug() << "[GlobalMap]MapViewMode::MapControl source: " << srcName;

                    sendNotification(MapConstants::ShowCarvata, true);
                    if ( lastMapViewDriveMode == MapViewMode::Simul ) {
                        QMetaObject::invokeMethod(mViewComponent, "setSimulMapMode", Q_ARG(QVariant,mMapViewModeReason) );
                    }
                    else {
                        QMetaObject::invokeMethod( mViewComponent, "setMapControlMode", Q_ARG(QVariant,mMapViewModeReason) );
                    }
                }
                break;
            case MapViewMode::NormalMapControl:
                {
                    QString srcName = DebugUtils::name( note->getSource() );
                    qDebug() << "[GlobalMap]MapViewMode::NormalMapControl source: " << srcName;

                    sendNotification(MapConstants::ShowCarvata, true);

                    auto isSimul = QQmlProperty::read(mViewComponent, "isSimul").toBool();

                    if ( isSimul/*lastMapViewDriveMode == MapViewMode::Simul*/ ) {
                        QMetaObject::invokeMethod(mViewComponent, "setSimulMapMode", Q_ARG(QVariant,mMapViewModeReason) );
                        Facade::instance()->sendNotification(this, MapConstants::HideVolume);
                    }
                    else {
                        QMetaObject::invokeMethod( mViewComponent, "setMapControlMode", Q_ARG(QVariant,mMapViewModeReason) );
                    }
                }
                break;
            case MapViewMode::MapInquiry:
                {
                    QString srcName = DebugUtils::name( note->getSource() );
                    qDebug() << "[GlobalMap]MapViewMode::MapInquiry source: " << srcName;

                    QMetaObject::invokeMethod( mViewComponent, "setMapInquiryMode", Q_ARG(QVariant,mMapViewModeReason) );
                    sendNotification(MapConstants::ShowCarvata, false);
                }
                break;
            case MapViewMode::Navi:
                {
                    QString srcName = DebugUtils::name( note->getSource() );
                    qDebug() << "[GlobalMap]MapViewMode::Navi source: " << srcName;

                    sendNotification(MapConstants::ShowCarvata, true);
                    restoreDriveMapModeAndLevel();
                    QMetaObject::invokeMethod( mViewComponent, "setNaviMode", Q_ARG(QVariant,mMapViewModeReason) );
                }
                break;
            case MapViewMode::Simul:
                {
                    QString srcName = DebugUtils::name( note->getSource() );
                    qDebug() << "[GlobalMap]MapViewMode::Simul source: " << srcName;

                    sendNotification(MapConstants::ShowCarvata, true);
                    QMetaObject::invokeMethod( mViewComponent, "setSimulMode", Q_ARG(QVariant,mMapViewModeReason) );
                    restoreDriveMapModeAndLevel();
                    if (lastMapViewDriveMode == MapViewMode::Normal) {
                        Facade::instance()->sendNotification(this, MapConstants::ShowVolume);
                    }
                }
                break;
            case MapViewMode::MenuNavi:
                {
                    QString srcName = DebugUtils::name( note->getSource() );
                    qDebug() << "[GlobalMap]MapViewMode::MenuNavi source: " << srcName;

                    sendNotification(MapConstants::ShowCarvata, true);
                    restoreDriveMapModeAndLevel();
                    QMetaObject::invokeMethod( mViewComponent, "setMenuNaviMode", Q_ARG(QVariant,mMapViewModeReason) );
                }
                break;
            }
        }
        break;
    }
}
