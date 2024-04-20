#include "MapUpdateDialogMediator.h"
#include <QDebug>
#include <QQuickView>

#include "PlatformHelper.h"

namespace SKT
{

void MapUpdateDialogMediator::onRegister()
{
    qDebug() << "MapUpdateDialogMediator::onRegister()";
    qDebug() << "mViewComponent = " << mViewComponent;

    AbstractViewMediator::onRegister();
}

MapUpdateDialogMediator::MapUpdateDialogMediator(QObject *viewComponent) : AbstractViewMediator(viewComponent)
{

}

MapUpdateDialogMediator::~MapUpdateDialogMediator()
{

}

QList<int> MapUpdateDialogMediator::listNotificationInterests()
{
    return QList<int> {};
}

bool MapUpdateDialogMediator::onEvent(int event, const QVariant &data)
{
    switch (event) {
    case Mediated:
        qDebug() << "MapUpdateDialogMediator.Mediated";
        break;

    case Activated:
        break;

    case OkPressed:
        {
            PlatformHelper::instance()->closeView("MapUpdatePopup");
            auto* popupWindow = dynamic_cast<QQuickView*>(PlatformHelper::instance()->popupWindow());
            if (popupWindow) {
                popupWindow->hide();
            } else {
                qDebug() << "popupWindow dynamic casting fail.";
            }
        }
        break;

    case CancelPressed:
        break;
    }
    return false;
}

bool MapUpdateDialogMediator::handleNotification(const mvc::INotificationPtr &noti)
{
    return false;
}

}//SKT
