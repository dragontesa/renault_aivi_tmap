#include "ManageInterfaceCommand.h"

#include <QTimer>

#include "Facade.h"
#include "ApplicationConstants.h"
#include "CommonConstants.h"
#include "InterfaceConstants.h"
#include "InterfaceProxy.h"
#include "MNS_Define.h"
#include "RgConstants.h"
#include "RouteConstants.h"
#include "RouteGuidanceProxy.h"
#include "RouteSearchProxy.h"
#include "navigationproperty.h"
#include "VsmMapProxy.h"

namespace SKT
{

ManageInterfaceCommand::ManageInterfaceCommand(QObject *parent)
    : SimpleCommand(parent)
{
}

bool ManageInterfaceCommand::execute(const mvc::INotificationPtr &note)
{
    switch (note->getId()) {
    case InterfaceConstants::ShowDeleteWaypointListScreen: {
        showWaypointEditView();
        break;
    }
    case InterfaceConstants::InsertWaypoint: {
        auto m = note->getBody().toMap();
        auto x = m[CommonConstants::X].toInt();
        auto y = m[CommonConstants::Y].toInt();
        auto slot = m[CommonConstants::Type].toInt();

        auto searchProxy = Facade::instance()->retrieveProxy<RouteSearchProxy>();
        searchProxy->insertWaypoint(slot, QPoint{x, y});
        break;
    }
    case InterfaceConstants::InsertWaypointFromSelectInMap: {
        auto slot = note->getBody().toInt();
        auto searchProxy = Facade::instance()->retrieveProxy<RouteSearchProxy>();
        searchProxy->insertWaypointFromSelectInMap(slot);
        break;
    }
    case InterfaceConstants::ReplaceWaypoint: {
        auto m = note->getBody().toMap();
        auto index = m[CommonConstants::Index].toInt();
        auto mapProxy = Facade::instance()->retrieveProxy<VsmMapProxy>();
        auto pos = mapProxy->centerPosition(nullptr);
        auto searchProxy = Facade::instance()->retrieveProxy<RouteSearchProxy>();
        searchProxy->replaceVia(index, pos.x(), pos.y());

        showWaypointEditView();
        break;
    }
    case InterfaceConstants::ApplyWaypointListChange: {
        // 주행중이라면 정리
        sendNotification(RgConstants::StopRouteGuidance);

        // 내비 초기화(모든 뷰 닫기 및 기타)
        sendNotification(ApplicationConstants::ResetNavi);

        QTimer::singleShot(1, this, [=](){
            auto m = note->getBody().toMap();
            auto points = m["points"].value<QRoutePosListPtr>();
            auto option = m["option"].toInt();
            auto apply = m["apply"].toBool();
            auto searchProxy = Facade::instance()->retrieveProxy<RouteSearchProxy>();
            searchProxy->requestRoute(points, option, apply);
        });
        break;
    }

    default:
        break;
    }//switch

    return true;
}

void ManageInterfaceCommand::showWaypointEditView()
{
    auto rgProxy = Facade::instance()->retrieveProxy<RouteGuidanceProxy>();
    auto isRg = rgProxy->isRg();
    if (isRg) {
        auto interfaceProxy = Facade::instance()->retrieveProxy<InterfaceProxy>();
        auto restoreVia = false;
        interfaceProxy->showDeleteWaypointListScreen(restoreVia);
    }
}

}//SKT
