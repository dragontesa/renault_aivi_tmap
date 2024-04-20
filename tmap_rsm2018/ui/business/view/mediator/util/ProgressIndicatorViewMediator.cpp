#include "ProgressIndicatorViewMediator.h"
#include "Constants.h"
#include "ZoneConstants.h"
#include "ViewRegistry.h"

using namespace SKT;

ProgressIndicatorViewMediator::ProgressIndicatorViewMediator(QObject *viewComponent)
    : AbstractViewMediator(viewComponent)
{

}

QList<int> ProgressIndicatorViewMediator::listNotificationInterests()
{
    static QList<int> interests = {
    };

    return interests;
}

bool ProgressIndicatorViewMediator::onEvent(int event, const QVariant &data)
{
    switch (event) {
    case Mediated: {
        }
        break;
    default:
        return false;
    }
    return true;
}

bool ProgressIndicatorViewMediator::handleNotification(const mvc::INotificationPtr &note)
{
    switch (note->getId()) {
    case Constants::None:
        break;
    default:
        return false;
    }

    return false;
}
