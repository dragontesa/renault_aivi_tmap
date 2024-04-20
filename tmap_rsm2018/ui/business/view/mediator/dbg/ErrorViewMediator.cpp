#include "ErrorViewMediator.h"

#include <QQmlProperty>

#include "Constants.h"
#include "RouteConstants.h"
#include "MapConstants.h"
#include "ZoneConstants.h"
#include "ViewRegistry.h"
#include "NavibookmarkConstants.h"

namespace SKT
{

ErrorViewMediator::ErrorViewMediator(QObject *viewComponent) : AbstractViewMediator(viewComponent)
{

}

ErrorViewMediator::~ErrorViewMediator()
{

}

QList<int> ErrorViewMediator::listNotificationInterests()
{
    static QList<int> interests = {
        RouteConstants::SetErrorCode
    };

    return interests;
}

bool ErrorViewMediator::onEvent(int event, const QVariant &data)
{
    switch (event) {
    case Activated:
        sendNotification(ZoneConstants::UpdateModal, QVariantMap{{"modal", true}});
        break;

    case OkPressed:
        {
            sendNotification(ZoneConstants::UpdateModal, QVariantMap{{"modal",false}});
            close();

            auto caller = QQmlProperty::read(mViewComponent, "caller").toString();
            if (caller == ViewRegistry::objectName(ViewRegistry::RouteSettingView)) {
                // 목적지가 없는 경우
                auto showCancelButton = QQmlProperty::read(mViewComponent, "showCancelButton").toBool();
                if (showCancelButton) {
                    // 취소 버튼이 있는 경우는 목적지 없는 ...
                    sendNotification(RouteConstants::RequestRouteUsingViaAsArrival, true);
                } else {
                    // 취소 버튼이 없다면 경유지, 목적지, 도착지가 가까울 때 표출 되는 다이얼로그임
                    // 알림 다이얼로그를 닫기만 한다.(qml에서 처리)
                }
            } else if (caller == ViewRegistry::objectName(ViewRegistry::NaviMenuView)) {
                sendNotification(NavibookmarkConstants::RegistBookmarkOk);
            }
        }
        break;

    case CancelPressed:
        {
            sendNotification(ZoneConstants::UpdateModal, QVariantMap{{"modal", false}});
            close();

            auto caller = QQmlProperty::read(mViewComponent, "caller").toString();
            if (caller == ViewRegistry::objectName(ViewRegistry::RouteSettingView)) {
                // 목적지가 없는 경우
                sendNotification(RouteConstants::RequestRouteUsingViaAsArrival, false);
            }
        }
        break;
    }

    return false;
}

bool ErrorViewMediator::handleNotification(const mvc::INotificationPtr &noti)
{
    switch (noti->getId()) {
    case RouteConstants::SetErrorCode:
        QQmlProperty::write(mViewComponent, "errorCode", noti->getBody());
        break;
    }

    return false;
}

void ErrorViewMediator::close()
{
    sendNotification(MapConstants::ResumeMapView);

    auto caller = QQmlProperty::read(mViewComponent, CommonConstants::Caller).toString();
    if (caller == ViewRegistry::objectName(ViewRegistry::RouteCandidateView)) {
        sendNotification(RouteConstants::ErrorViewClosed);
    }

    QMetaObject::invokeMethod(mViewComponent, "close");
}

}
