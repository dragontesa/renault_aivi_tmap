#include "ZoneApiTestViewMediator.h"

#include <QDebug>

#include "CommonConstants.h"
#include "NavigationControllerConstants.h"
#include "ToastsConstants.h"

#ifdef BUILD_TARGET
#include "hmiZonetype.h"
#endif

#include "PlatformHelper.h"

namespace SKT
{

ZoneApiTestViewMediator::ZoneApiTestViewMediator(QObject* viewComponent)
    : AbstractViewMediator(viewComponent)
{

}

QList<int> ZoneApiTestViewMediator::listNotificationInterests()
{
    return QList<int>{};
}

bool ZoneApiTestViewMediator::onEvent(int event, const QVariant &data)
{
    switch (event) {
    case Call:
        call(data);
        break;

    case CallKeyboard:
        callKeyboard(data);
        break;
    }
    return true;
}

bool ZoneApiTestViewMediator::handleNotification(const mvc::INotificationPtr &note)
{
    return true;
}

void ZoneApiTestViewMediator::call(const QVariant &v)
{
    auto m = v.toMap();
    auto appId = m["appId"].toInt();
    auto type = m["type"].toInt();
    auto updateRegion = m["updateRegion"].toBool();
    auto value = m["value"].toInt();

#ifdef BUILD_TARGET
    if (type == TITLE) {
        m["text"] = "TITLE";
    }
    if (type == SEARCH_TEXT_1) {
        if (!mIsShowKeyboard) {
            mIsShowKeyboard = true;
            PlatformHelper::instance()->setupKeyboard(QVariantMap{{"show", true}});
        }
    } else {
        if (mIsShowKeyboard) {
            mIsShowKeyboard = false;
            PlatformHelper::instance()->setupKeyboard(QVariantMap{{"show", false}});
        }
    }
#endif

    QString message = QString("call(%1, %2, %3, %4)").arg(appId).arg(type).arg(updateRegion).arg(value);

    sendNotification(NavigationControllerConstants::CallZoneApi, m);

#if (0)
    m.clear();
    m[CommonConstants::Position] = ToastsConstants::ToastUp;
    m[CommonConstants::Message] = message;
    sendNotification(ToastsConstants::ShowToast, m);
#else
    qDebug() << message;
#endif
}

void ZoneApiTestViewMediator::callKeyboard(const QVariant &v)
{
    auto m = v.toMap();
    auto type = m["type"].toInt();
    auto api = m["api"].toString();
    qDebug() << "callKeyboard(" << type << "," << api << ")";
    sendNotification(NavigationControllerConstants::CallZoneApi, m);
}

}
