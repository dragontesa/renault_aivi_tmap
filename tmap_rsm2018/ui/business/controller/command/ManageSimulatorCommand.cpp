#include "ManageSimulatorCommand.h"
#include "SettingDto.h"
#include "SettingProxy.h"
#include "RouteGuidanceProxy.h"
#include "RouteSearchProxy.h"
#include "Constants.h"
#include "CommonConstants.h"
#include "RgConstants.h"
#include "MapConstants.h"
#include "SimulatorConstants.h"
#include "mediator/app/MapViewMode.h"
#include "VsmMapProxy.h"
#include "VsmNaviProxy.h"
#include "NavigationPropertyProtocol.h"
#include "NavigationPropertyConstants.h"

using namespace SKT;

ManageSimulatorCommand::ManageSimulatorCommand( QObject* parent )
    : SimpleCommand( parent )
{

}

bool ManageSimulatorCommand::execute( const mvc::INotificationPtr& note )
{
    switch ( note->getId() ) {
    case SimulatorConstants::StartSimulator:
        {
            auto proxy = Facade::instance()->retrieveProxy<VsmNaviProxy>();

            QObject* mapView = nullptr;
            quint32 routeId = 0;
            if ( note->getBody().canConvert(QVariant::Map) ) {
                auto m = note->getBody().toMap();
                mapView = m["mapView"].value<QObject*>();
                routeId = m["routeId"].toUInt();
            }
            else {
                routeId = note->getBody().toUInt();
            }

            sendNotification(MapConstants::ChangeMapViewMode,QVariantMap{{CommonConstants::Mode,MapViewMode::Simul}});

            // selectRoute() 선택하면 모의 주행 시 경로 이탈 발생
            // applySelectRoute()를 선택해야 제대로 된 모의 주행이 됨.
            // 17.07.21 ymhong.
            //proxy->selectRoute(routeId);
            proxy->applySelectRoute(mapView, routeId);
            proxy->setDrawGasStationInfo(mapView);

            auto searchProxy = Facade::instance()->retrieveProxy<RouteSearchProxy>();
            searchProxy->updatePoi();

            auto simulator = Facade::instance()->retrieveProxy<RouteGuidanceProxy>();
            simulator->startSimulator();

            // Map in cluster need RG state while simulating, 2019.5.29 required by bosch
            QVariantMap m;
            m["type"] = NavigationPropertyProtocol::TmapNaviStatus;
            m["propertyValue"] = TMAPNAVISTATUS_PLAYROUTE;
            sendNotification(NavigationPropertyConstants::setNavigationProperty, m);
        }
        return true;
    case SimulatorConstants::StopSimulator:
        {
            auto simulator = Facade::instance()->retrieveProxy<RouteGuidanceProxy>();
            if (simulator->isSimulatorRunning()) {
                simulator->stopSimulator();

                auto m = note->getBody().toMap();
                auto cancelRoute = m["cancelRoute"].toBool();
                if (cancelRoute) {
                    auto naviProxy = Facade::instance()->retrieveProxy<VsmNaviProxy>();
                    naviProxy->cancelRoute();
                }
                sendNotification(MapConstants::ChangeMapViewMode,QVariantMap{{CommonConstants::Mode,MapViewMode::Normal}});
            }

            QVariantMap m;
            m["type"] = NavigationPropertyProtocol::TmapNaviStatus;
            m["propertyValue"] = TMAPNAVISTATUS_ANDO;
            sendNotification(NavigationPropertyConstants::setNavigationProperty, m);
        }
        return true;
    }

    return false;
}
