#ifndef ABSTRACTMEDIATOR_H
#define ABSTRACTMEDIATOR_H

#include "TMediator.h"

namespace SKT {

class AbstractMediator : public mvc::TMediator
{
public:
    AbstractMediator( QObject* viewComponent = nullptr );

    virtual ~AbstractMediator();

    virtual void onBeforeRegister();

    virtual void onRegister();

    void sendNotification( int noteId, const QVariant& body = QVariant(), const QVariant& observerId = QVariant(), const QString& type = QString() );

    void addNotificationInterest( int note );

    void removeNotificationInterest( int note );
};

}

#endif // ABSTRACTMEDIATOR_H
