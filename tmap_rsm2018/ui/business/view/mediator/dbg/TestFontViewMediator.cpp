#include "TestFontViewMediator.h".h"

namespace SKT
{

TestFontViewMediator::TestFontViewMediator(QObject *parent)
{

}

QList<int> TestFontViewMediator::listNotificationInterests()
{
    return QList<int> {};
}

bool TestFontViewMediator::onEvent(int event, const QVariant &data)
{
    return true;
}

bool TestFontViewMediator::handleNotification(const mvc::INotificationPtr &note)
{
    return true;
}


}
