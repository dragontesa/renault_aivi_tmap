#include "ApplicationMediator.h"
#include "Constants.h"
#include "CommonConstants.h"
#include "ApplicationConstants.h"
#include "ViewRegistry.h"
#include "RgConstants.h"
#include "MapConstants.h"
#include "mediator/map/SimulationViewMediator.h"
#include "DebugUtils.h"

using namespace SKT;


void ApplicationMediator::handleMapViewMode( const mvc::INotificationPtr& note )
{
    switch ( note->getId() ) {
    case MapConstants::RequestDriveMode:
        sendNotification(MapConstants::ResponseDriveMode, mMapViewMode.lastDriveMode());
        break;
    case MapConstants::ChangeMapViewMode: {
            auto args = note->getBody().value<QVariantMap>();
            auto mode = (MapViewMode::ModeK)args[CommonConstants::Mode].toInt();
            QString srcName = DebugUtils::name( note->getSource() );
            qDebug() << "[GlobalMap]ChangeMapViewMode mode: " << mode << " source: " << srcName;
            auto reason = MapViewMode::None;
            if ( args.contains(QStringLiteral("reason")) ) {
               reason = (MapViewMode::ReasonK)args[QStringLiteral("reason")].toInt();
            }
            mMapViewMode.setCurrentMode( note->getSource().data(), mode, reason );
        }
        break;
    case MapConstants::RestoreMapViewMode: {
            auto args = note->getBody().value<QVariantMap>();
            MapViewMode::ReasonK reason = MapViewMode::None;
            if ( args.contains(QStringLiteral("reason")) ) {
               reason = (MapViewMode::ReasonK)args[QStringLiteral("reason")].toInt();
            }

            if ( mMapViewMode.currentMode() == MapViewMode::Normal ) {
                if ( reason == MapViewMode::None ) {
                    reason = MapViewMode::ReturnFromNormalMode;
                }
                mMapViewMode.setCurrentMode( note->getSource().data(), MapViewMode::Normal, reason );
            }
            else if ( mMapViewMode.currentMode() == MapViewMode::Navi ) {
                if ( reason == MapViewMode::None ) {
                    reason = MapViewMode::ReturnFromNaviMode;
                }
                mMapViewMode.setCurrentMode( note->getSource().data(), mMapViewMode.lastDriveMode(), reason );
            }
            else if ( mMapViewMode.currentMode() == MapViewMode::Simul ) {
                if ( reason == MapViewMode::None ) {
                    reason = MapViewMode::ReturnFromSimulMode;
                }
                mMapViewMode.setCurrentMode( note->getSource().data(), MapViewMode::Normal, reason );
            }
            else if ( mMapViewMode.currentMode() == MapViewMode::MapControl || mMapViewMode.currentMode() == MapViewMode::NormalMapControl) {
                if ( reason == MapViewMode::None ) {
                    reason = MapViewMode::ReturnFromMapControlMode;
                }
                mMapViewMode.setCurrentMode( note->getSource().data(), mMapViewMode.lastDriveMode(), reason );
            }
            else if ( mMapViewMode.currentMode() == MapViewMode::MapInquiry) {
                auto mediator = Facade::instance()->retrieveMediator<SimulationViewMediator>();
                if ( !mediator.isNull() ) {
                    if ( reason == MapViewMode::None ) {
                        reason = MapViewMode::ReturnFromMapInquiryMode;
                    }
                    mMapViewMode.setCurrentMode( note->getSource().data(), MapViewMode::Simul, reason );
                }
                else {
                    if ( reason == MapViewMode::None ) {
                        reason = MapViewMode::ReturnFromMapInquiryMode;
                    }
                    auto driveMode = mMapViewMode.lastDriveMode();
                    if (mForceNaviMode) {
                        driveMode = MapViewMode::Navi;
                        mForceNaviMode = false;
                    }
                    // 17.5.30 hskim 조회 모드로부터 주행 맵뷰 모드로 돌아오는 처리 누락되어 있어 수정합니다
                    mMapViewMode.setCurrentMode( note->getSource().data(), driveMode, reason );
                    //mMapViewMode.setCurrentMode( note->getSource().data(), MapViewMode::Normal, reason );
                }
            }
			else if ( mMapViewMode.currentMode() == MapViewMode::MenuNavi ) {
				if ( reason == MapViewMode::None ) {
                    reason = MapViewMode::ReturnFromMenuNaviMode;
                }
                mMapViewMode.setCurrentMode( note->getSource().data(), mMapViewMode.lastDriveMode(), reason );
			}
            else {
                T_DEBUG( QString( "<B>Detected an unhandled MapViewMode(%1)</B>" ).arg( mMapViewMode.currentMode() ) );
                mMapViewMode.setCurrentMode( note->getSource().data(), mMapViewMode.lastDriveMode(), reason );
            }
            mMapViewMode.notifyCurrentMode();
        }
        break;
    case MapConstants::NotifyMapViewMode: {
            auto args = note->getBody().value<QVariantMap>();
            if ( args.contains( QStringLiteral("reason") ) ) {
                auto reason = (MapViewMode::ReasonK)args[QStringLiteral("reason")].toInt();
                mMapViewMode.notifyCurrentMode( reason );
            }
            else {
                mMapViewMode.notifyCurrentMode();
            }
        }
        break;
    }
}

