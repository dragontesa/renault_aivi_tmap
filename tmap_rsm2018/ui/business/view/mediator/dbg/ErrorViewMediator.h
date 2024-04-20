#ifndef ERRORVIEWMEDIATOR_H
#define ERRORVIEWMEDIATOR_H

#include "mediator/AbstractViewMediator.h"

namespace SKT
{

class ErrorViewMediator : public AbstractViewMediator
{
    Q_OBJECT
    Q_ENUMS(EventK)

public:
    enum EventK {
        Activated,
        Mediated,
        OkPressed,
        CancelPressed,
        RestoreRoute
    };

    Q_INVOKABLE ErrorViewMediator(QObject * viewComponent = nullptr);
    virtual ~ErrorViewMediator();

    QList<int> listNotificationInterests();

protected:
    bool onEvent( int event, const QVariant& data );
    bool handleNotification( const mvc::INotificationPtr& noti );

private:
    void close();
};

}

#endif // ERRORVIEWMEDIATOR_H
