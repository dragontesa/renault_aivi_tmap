#include "TMediator.h"

using namespace SKT;
using namespace SKT::mvc;

TMediator::TMediator( QObject* viewComponent )
    : mMediatorName( "Undefined" )
{
    mViewComponent = viewComponent;
}

QVariant TMediator::getMediatorId()
{
    return mMediatorId;
}

void TMediator::setMediatorId( const QVariant& id )
{
    mMediatorId = id;
}

QString TMediator::getMediatorName()
{
    return mMediatorName;
}

void TMediator::setMediatorName( const QString& name )
{
    mMediatorName = name;
}

void TMediator::setViewComponent( QObject* viewComponent )
{
    mViewComponent = viewComponent;
}

QObject* TMediator::getViewComponent()
{
    return mViewComponent;
}

QList<int> TMediator::listNotificationInterests()
{
    return QList<int>();
}

bool TMediator::handleNotification( const INotificationPtr& note )
{
    return false;
}

void TMediator::onBeforeRegister()
{
}

void TMediator::onRegister()
{
}

void TMediator::onRemove()
{
}

