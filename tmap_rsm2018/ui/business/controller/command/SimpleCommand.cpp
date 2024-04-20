#include "StartupCommand.h"
#include "Facade.h"

using namespace SKT;

SimpleCommand::SimpleCommand( QObject* parent )
    : QObject( parent )
{
}

bool SimpleCommand::execute( const mvc::INotificationPtr& note )
{
    return false;
}

void SimpleCommand::sendNotification( int noteId, const QVariant& body, const QVariant& observerId, const QString& type )
{
    Facade::instance()->sendNotification( this, noteId, body, observerId, type );
}
