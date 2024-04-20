#include "RpErrorHelper.h"

#include <QVariant>
#include "CommonConstants.h"
#include "Facade.h"
#include "MapConstants.h"
#include "RouteConstants.h"
#include "RouteSearchProxy.h"
#include "ViewRegistry.h"
#include "SimpleCommand.h"
#include "view/mediator/AbstractMediator.h"
#include "PlatformHelper.h"

namespace SKT
{

void RpErrorHelper::exec(SimpleCommand *command, int errorCode, bool cancelRoute)
{
    auto m = exec(errorCode);

    command->sendNotification(Constants::CreateView, m);
    if (cancelRoute)
        command->sendNotification(RouteConstants::CancelRoute);
    //command->sendNotification(MapConstants::PauseMapView);
    command->sendNotification(MapConstants::ShowCarvata, false);
}

QVariantMap RpErrorHelper::exec(int errorCode)
{
    QVariantMap m;
    m[CommonConstants::Url] = ViewRegistry::url(ViewRegistry::CommonDialogView);
    m[CommonConstants::Caller] = ViewRegistry::objectName(ViewRegistry::RouteCandidateView);
    m["title"] = QObject::tr("알림");
    auto getErrorMessage = [](int errorCode){
        switch (errorCode) {
        case -297: return QObject::tr("선택한 위치와 동일한 지점이\n등록되어 있습니다.\n다시 확인해 주세요.");
        case -295: return QObject::tr("경로탐색 최대거리를 초과하였습니다.\n경로를 재설정 해주세요.");
        default:
            auto searchProxy = Facade::instance()->retrieveProxy<RouteSearchProxy>();
            if (searchProxy->hasVia()) {
                return QObject::tr("길안내 경로가 없는 지점이 있습니다.\n다른 지점을 선택하여 주세요.");
            } else {
                return QObject::tr("길안내 경로가 없는 지점입니다.\n근처 다른 지점을 선택하여 주세요.");
            }
        }
    };
    m["message"] = getErrorMessage(errorCode);
    if (errorCode == -295) {
        m["message2"] = QString("(에러코드:%1)").arg(errorCode);
    }
    m["showTimer"] = true;
    PlatformHelper::instance()->setParentForSystemPopup(m);

    return m;
}

}
