#include "TNotification.h"
#include "Constants.h"

using namespace SKT;
using namespace SKT::mvc;

TNotification::TNotification( QObject* source, int id, const QVariant& body, const QVariant& observerId, const QString& type )
    : mSource( QObjectPtr( source, TShareable<QObject>::preventDeletion ) ),
    mId( id ),
    mBody( body ),
    mType( type ),
    mObserverId( observerId )
{
}

TNotification::~TNotification()
{
}

int TNotification::getId()
{
    return mId;
}

void TNotification::setBody( const QVariant& body )
{
    mBody = body;
}

QVariant TNotification::getBody()
{
    return mBody;
}

void TNotification::setType( const QString& type )
{
    mType = type;
}

QString TNotification::getType()
{
    return mType;
}

QObjectPtr TNotification::getSource()
{
    return mSource;
}

void TNotification::setSource( const QObjectPtr& obj )
{
    mSource = obj;
}

QObjectPtr TNotification::getDestination()
{
    return mDestination;
}

void TNotification::setDestination( const QObjectPtr& obj )
{
    mDestination = obj;
}

QVariant TNotification::getObserverId()
{
    return mObserverId;
}

void TNotification::setObserverId( const QVariant& observerId )
{
    mObserverId = observerId;
}
