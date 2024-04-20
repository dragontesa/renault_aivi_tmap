#include "TObserver.h"

using namespace SKT;
using namespace SKT::mvc;

TObserver::TObserver( const IObserver::NotifyMethod& notifyMethod, const QObjectPtr& notifyContext )
{
    setNotifyMethod( notifyMethod );
    setNotifyContext( notifyContext );
}

TObserver::~TObserver()
{
}

QVariant TObserver::getObserverId()
{
    return mObserverId;
}

void TObserver::setObserverId( const QVariant& id )
{
    mObserverId = id;
}

QString TObserver::getObserverName()
{
    return mObserverName;
}

void TObserver::setObserverName( const QString& name )
{
    mObserverName = name;
}

void TObserver::setNotifyMethod( const IObserver::NotifyMethod& notifyMethod )
{
    mNotifyMethod = notifyMethod;
}

void TObserver::setNotifyContext( const QObjectPtr& notifyContext )
{
    mNotifyContext = notifyContext;
}

IObserver::NotifyMethod TObserver::getNotifyMethod()
{
    return mNotifyMethod;
}

QObjectPtr TObserver::getNotifyContext()
{
    return mNotifyContext;
}

//#define TMAP_RSM_UI_ENABLE_MVC_LOG
#define TMAP_RSM_UI_ENABLE_MVC_MORNITOR_NOTI
//#define TMAP_RSM_UI_ENABLE_MVC_MORNITOR_NOTI_ID id == 1019 || id == 1020
#define TMAP_RSM_UI_ENABLE_MVC_MORNITOR_NOTI_ID id == 1020

void TObserver::notifyObserver( const INotificationPtr& note )
{
#ifdef TMAP_RSM_UI_ENABLE_MVC_MORNITOR_NOTI
    auto id = note->getId();
    if ( TMAP_RSM_UI_ENABLE_MVC_MORNITOR_NOTI_ID ) {
        T_PERF_LOG() << "[Perf][TObserver]notifyObserver id: " << note->getId() << " name: " << getObserverName() << " #begin";
    }
#endif

    bool notify = false;
    auto observerId = note->getObserverId();
    if ( observerId.isNull() ) {
        notify = true;
    }
    else {
        if ( observerId.canConvert( QVariant::List ) ) {
        }
        else {
            auto observerIntId = observerId.toInt();
            if ( observerIntId == mObserverId.toInt() ) {
                notify = true;
            }
        }
    }

    bool res = false;
    if ( notify ) {
        res = mNotifyMethod( note );
#ifdef TMAP_RSM_UI_ENABLE_MVC_MORNITOR_NOTI
        if ( TMAP_RSM_UI_ENABLE_MVC_MORNITOR_NOTI_ID ) {
            T_PERF_LOG() << "[Perf][TObserver]notifyObserver id: " << note->getId() << " name: " << getObserverName() << " #notified";
        }
#endif
    }

#ifdef TMAP_RSM_UI_ENABLE_MVC_MORNITOR_NOTI
    if ( TMAP_RSM_UI_ENABLE_MVC_MORNITOR_NOTI_ID ) {
        T_PERF_LOG() << "[Perf][TObserver]notifyObserver id: " << note->getId() << " name: " << getObserverName() << " #end";
    }
#endif

#ifdef TMAP_RSM_UI_ENABLE_MVC_LOG
    QString srcName;
    auto source = note->getSource();
    if ( source ) {
        QString objName = source->objectName();
        if ( !objName.isEmpty() ) {
            srcName = objName;
        }
        else {
            srcName = source->metaObject()->className();
        }
    }
    QJsonObject noteJson{
        {"id",note->getType()},
        {"body",note->getBody().isNull() ? "null" : note->getBody().toString()}
    };
    T_DEBUG( QJsonObject({
        {"type","Notification"},
        {"isProcessed",res},
        {"note",noteJson},
        {"from",srcName},
        {"to",mObserverName}
    }) );
#endif
}

bool TObserver::compareNotifyContext( const QObjectPtr& object )
{
   return object == mNotifyContext;
}
