#include "Facade.h"
#include "ViewRegistry.h"
#include "StartupCommand.h"
#include "Constants.h"

using namespace SKT;
using namespace SKT::mvc;

Notification::Notification( QObject* source, int id, const QVariant& body, const QVariant& observerId )
    : TNotification( source, id, body, observerId )
{
    //setType( Constants::enumToString("NotificationK", getId()) );
}

Facade::Facade()
{
    mController.setView( &mView );
    setCore( &mModel, &mView, &mController );

    registerCommand( Constants::Startup, &StartupCommand::staticMetaObject );
}

Facade* Facade::instance()
{
    static Facade Instance;

    return &Instance;
}

void Facade::initialize()
{    
}

void Facade::terminate()
{
//    removeProxy( "PositionProxy" );
//    removeProxy( "CarProxy" );
}

void Facade::registerCommand( int noteId, const QMetaObject* commandClassRef )
{
    mvc::TFacade::registerCommand( noteId, commandClassRef );
}

void Facade::registerObserver( int noteId, const mvc::IObserverPtr& observer )
{
    mView.registerObserver( noteId, observer );
}

void Facade::removeObserver( int noteId, const QObjectPtr& notifyContext )
{
    mView.removeObserver( noteId, notifyContext );
}

void Facade::sendNotification( QObject* source, int noteId, const QVariant& body, const QVariant& observerId, const QString& type )
{
    notifyObservers( t_new_shared<Notification>( source, noteId, body, observerId ) );
}

void Facade::sendNotification( int noteId, const QVariant& body, const QVariant& observerId, const QString& type )
{
}
