#include "TestGuidViewMediator.h"

namespace SKT
{

TestGuidViewMediator::TestGuidViewMediator(QObject *viewComponent)
{

}

QList<int> TestGuidViewMediator::listNotificationInterests()
{
    return QList<int> {};
}

bool TestGuidViewMediator::onEvent(int event, const QVariant &data)
{
    return true;
}

bool TestGuidViewMediator::handleNotification(const mvc::INotificationPtr &note)
{
    return true;
}

}
