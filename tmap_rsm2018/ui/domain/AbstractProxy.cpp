#include "AbstractProxy.h"
#include "Facade.h"

using namespace SKT;

AbstractProxy::AbstractProxy( const QVariant& data )
    : TProxy( data )
{
}

AbstractProxy::~AbstractProxy()
{

}

void AbstractProxy::onBeforeRegister()
{
    QString name;
    auto objName = objectName();
    if ( !objName.isEmpty() ) {
        name = objName;
    }
    else {
        name = metaObject()->className();
    }

    setProxyName( name );
}

void AbstractProxy::sendNotification( int noteId, const QVariant& body, const QVariant& observerId, const QString& type )
{
    Facade::instance()->sendNotification( this, noteId, body, observerId, type );
}
