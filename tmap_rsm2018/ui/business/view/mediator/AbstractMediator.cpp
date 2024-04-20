#include "AbstractMediator.h"
#include "Facade.h"

using namespace SKT;

AbstractMediator::AbstractMediator( QObject* viewComponent )
    : TMediator( viewComponent )
{
}

AbstractMediator::~AbstractMediator()
{
}

void AbstractMediator::onBeforeRegister()
{
    QString name;
    auto objName = objectName();
    if ( !objName.isEmpty() ) {
        name = objName;
    }
    else {
        name = metaObject()->className();
    }

    setMediatorName( name );
}

void AbstractMediator::onRegister()
{
}

void AbstractMediator::sendNotification( int noteId, const QVariant& body, const QVariant& observerId, const QString& type )
{
    Facade::instance()->sendNotification( this, noteId, body, observerId, type );
}

void AbstractMediator::addNotificationInterest( int note )
{
    auto mediator = qSharedPointerDynamicCast<QObject>( ((mvc::TFacade*)Facade::instance())->retrieveMediator( getMediatorName() ) );

    auto notifyMethod = std::bind(&mvc::IMediator::handleNotification, this, std::placeholders::_1);
    auto observer = mvc::IObserverPtr( new mvc::TObserver( notifyMethod, mediator ) );
    observer->setObserverId( getMediatorId() );
    observer->setObserverName( getMediatorName() );

    Facade::instance()->registerObserver( note,  observer );
}

void AbstractMediator::removeNotificationInterest( int note )
{
    auto mediator = qSharedPointerDynamicCast<QObject>( ((mvc::TFacade*)Facade::instance())->retrieveMediator( getMediatorName() ) );

    Facade::instance()->removeObserver( note, mediator );
}
