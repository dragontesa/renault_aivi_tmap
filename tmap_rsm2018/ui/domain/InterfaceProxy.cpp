#include "InterfaceProxy.h"

#include <QTimer>

#include "ApplicationConstants.h"
#include "CommonConstants.h"
#include "RouteConstants.h"
#include "ViewRegistry.h"

namespace SKT
{

InterfaceProxy::InterfaceProxy()
    : AbstractProxy("InterfaceProxy")
{

}

void InterfaceProxy::showDeleteWaypointListScreen(bool restoreVia)
{
    sendNotification(ApplicationConstants::ResetNavi, QVariantMap{
                         {"restoreVia", restoreVia}
                     });

    // 만약 경유지 설정 화면이 떠 있을 때 CloseAllViews로 close 후 곧바로
    // CreateView를 하면 duplication error로 생성되지 않음
    // 다음 이벤트 루프에서 처리해야 정상적으로 생성 된다.
    QTimer::singleShot(1, this, [&](){
        sendNotification(Constants::CreateView, QVariantMap{
                             {CommonConstants::Url,ViewRegistry::url(ViewRegistry::RouteSettingView)}
                         });
    });
}

void InterfaceProxy::insertWaypoint(int type, const QPoint &pos)
{
    sendNotification(ApplicationConstants::ResetNavi);

    QTimer::singleShot(1, this, [=](){
        sendNotification(RouteConstants::SelectViaSlot, QVariantMap{
                             {CommonConstants::Slot, type}
                         });

        sendNotification(RouteConstants::InsertVia, QVariantMap{
                             {CommonConstants::X, pos.x()},
                             {CommonConstants::Y, pos.y()}
                         });

        sendNotification(Constants::CreateView, QVariantMap{
                             {CommonConstants::Url, ViewRegistry::url(ViewRegistry::RouteSettingView)}
                         });
    });
}

}//SKT
