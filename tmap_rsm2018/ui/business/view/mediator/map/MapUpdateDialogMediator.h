#ifndef MAPUPDATEDIALOGMEDIATOR_H
#define MAPUPDATEDIALOGMEDIATOR_H

#include "mediator/AbstractViewMediator.h"

namespace SKT
{

class MapUpdateDialogMediator : public AbstractViewMediator
{
    Q_OBJECT
    Q_ENUMS(EventK)

public:
    enum EventK {
        Activated,
        Mediated,
        OkPressed,
        CancelPressed
    };

    virtual void onRegister();

    Q_INVOKABLE MapUpdateDialogMediator(QObject * viewComponent = nullptr);
    virtual ~MapUpdateDialogMediator();

    QList<int> listNotificationInterests();

protected:
    bool onEvent( int event, const QVariant& data );
    bool handleNotification( const mvc::INotificationPtr& noti );

private:
    void close();
};

}

#endif // MAPUPDATEDIALOGMEDIATOR_H
